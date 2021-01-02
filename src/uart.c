/**
 * @file uart.c
 * @author Bernhard Kraemer, Uwe Fechner
 * @brief Uart mirror with message queue.
 * @version 0.2
 * @date 2021-01-02
 * 
 * @copyright Copyright (c) 2020
 * 
 */

/*** Imported Objects ***************************************************************/
#include <zephyr.h>
#include <logging/log.h>

#include <drivers/uart.h>
#include <globals.h>

/*** Constants **********************************************************************/
#define LOG_MODULE_NAME uart

#define START_DELAY     500
#define STACK_SIZE      500
#define PRIORITY        -5

#define UART_DEV        DT_LABEL(DT_NODELABEL(lpuart4))

/*** Types **************************************************************************/
extern struct k_msgq data_message_q; 

/*** Variables **********************************************************************/
static volatile bool tx_done;
static data_message_t tx_msg;

/*** Function Prototypes ************************************************************/
void uart_main_f();
static void uart_fifo_cb(const struct device *dev, void *user_data);

/*** Macros *************************************************************************/
LOG_MODULE_REGISTER(LOG_MODULE_NAME, LOG_LEVEL_DBG);

K_THREAD_DEFINE(uart_id, STACK_SIZE, uart_main_f, NULL, NULL, NULL,
                PRIORITY, 0, START_DELAY);

/*** Function implementation ********************************************************/

void uart_main_f()
{
    const struct device *uart;

    uart = device_get_binding(UART_DEV);
    if (uart == NULL) {
        LOG_ERR("Device binding is NULL");
        return;
    }

    // struct uart_config cfg;
    // cfg.baudrate=115200;
    // cfg.parity=UART_CFG_PARITY_NONE;
    // cfg.stop_bits=UART_CFG_STOP_BITS_1;
    // cfg.data_bits=UART_CFG_DATA_BITS_8;
    // cfg.flow_ctrl=UART_CFG_FLOW_CTRL_NONE;

    // ret = uart_configure(uart, &cfg);
    // if (ret != 0)
    // {
    //     LOG_ERR("Failed to configure %s, Error: %u", UART_DEV, ret);
    //     return;
    // }

    uart_irq_callback_set(uart, uart_fifo_cb);
    uart_irq_rx_enable(uart);

    LOG_INF("UART thread started");
    
    while (true) {
        /* get a data item */
        k_msgq_get(&data_message_q, &tx_msg, K_FOREVER);
        tx_done=false;
        uart_irq_tx_enable(uart);

        while(!tx_done){}
    }
}

static void uart_fifo_cb(const struct device *dev, void *user_data)
{
    static data_message_t rx_msg;
    static int tx_data_idx;

    uart_irq_update(dev);

    if (uart_irq_tx_ready(dev) && tx_data_idx < tx_msg.length) {
        uart_fifo_fill(dev, &tx_msg.data[tx_data_idx++], 1);

        if (tx_data_idx == tx_msg.length) {
            uart_irq_tx_disable(dev);
            tx_done=true;
            tx_data_idx=0;
        }
	}

    if (uart_irq_rx_ready(dev)) {
        uart_fifo_read(dev, &rx_msg.data[rx_msg.length++], 1);

        if ((rx_msg.data[rx_msg.length] == '\n') || (rx_msg.data[rx_msg.length - 1] == '\r')) {
            LOG_HEXDUMP_DBG((const uint8_t *)&rx_msg.data, rx_msg.length, "Received data:");
            rx_msg.length=0;
        }
    }
}
