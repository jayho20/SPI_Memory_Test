/**
 * Copyright (c) 2016 - 2021, Nordic Semiconductor ASA
 *
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice, this
 *    list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form, except as embedded into a Nordic
 *    Semiconductor ASA integrated circuit in a product or a software update for
 *    such product, must reproduce the above copyright notice, this list of
 *    conditions and the following disclaimer in the documentation and/or other
 *    materials provided with the distribution.
 *
 * 3. Neither the name of Nordic Semiconductor ASA nor the names of its
 *    contributors may be used to endorse or promote products derived from this
 *    software without specific prior written permission.
 *
 * 4. This software, with or without modification, must only be used with a
 *    Nordic Semiconductor ASA integrated circuit.
 *
 * 5. Any software provided in binary form under this license must not be reverse
 *    engineered, decompiled, modified and/or disassembled.
 *
 * THIS SOFTWARE IS PROVIDED BY NORDIC SEMICONDUCTOR ASA "AS IS" AND ANY EXPRESS
 * OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
 * OF MERCHANTABILITY, NONINFRINGEMENT, AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL NORDIC SEMICONDUCTOR ASA OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE
 * GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT
 * OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 */
/** @file
 * @defgroup qspi_example_main main.c
 * @{
 * @ingroup qspi_example
 *
 * @brief QSPI Example Application main file.
 *
 * This file contains the source code for a sample application using QSPI.
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "nrf_drv_qspi.h"
#include "nrf_delay.h"
#include "app_util_platform.h"
#include "app_error.h"
#include "boards.h"

#include "nrf_gpio.h"
#include "nrf_drv_rtc.h"
#include "nrf_drv_clock.h"

#include "nrf_log.h"
#include "nrf_log_ctrl.h"
#include "nrf_log_default_backends.h"
#include "sdk_config.h"

#include "counter.h"


//const nrf_drv_rtc_t rtc = NRF_DRV_RTC_INSTANCE(2);


#define QSPI_STD_CMD_WRSR   0x01
#define QSPI_STD_CMD_RSTEN  0x66
#define QSPI_STD_CMD_RST    0x99

#define QSPI_TEST_DATA_SIZE 256//4096//2048//1024 //256

#define WAIT_FOR_PERIPH() do { \
        while (!m_finished) {} \
        m_finished = false;    \
    } while (0)

static volatile bool m_finished = false;
static uint8_t m_buffer_tx[QSPI_TEST_DATA_SIZE];
static uint8_t m_buffer_rx[QSPI_TEST_DATA_SIZE];

static void qspi_handler(nrf_drv_qspi_evt_t event, void * p_context)
{
    UNUSED_PARAMETER(event);
    UNUSED_PARAMETER(p_context);
    m_finished = true;
}

static void configure_memory()
{
    uint8_t temporary = 0x40;
    uint32_t err_code;
    nrf_qspi_cinstr_conf_t cinstr_cfg = {
        .opcode    = QSPI_STD_CMD_RSTEN,
        .length    = NRF_QSPI_CINSTR_LEN_1B,
        .io2_level = true,
        .io3_level = true,
        .wipwait   = true,
        .wren      = true
    };

    // Send reset enable
    err_code = nrf_drv_qspi_cinstr_xfer(&cinstr_cfg, NULL, NULL);
    APP_ERROR_CHECK(err_code);

    // Send reset command
    cinstr_cfg.opcode = QSPI_STD_CMD_RST;
    err_code = nrf_drv_qspi_cinstr_xfer(&cinstr_cfg, NULL, NULL);
    APP_ERROR_CHECK(err_code);

    // Switch to qspi mode
    ///for micron this command is not needed 
    //cinstr_cfg.opcode = QSPI_STD_CMD_WRSR;
    //cinstr_cfg.length = NRF_QSPI_CINSTR_LEN_2B;
    //err_code = nrf_drv_qspi_cinstr_xfer(&cinstr_cfg, &temporary, NULL);
    //APP_ERROR_CHECK(err_code);
}
//static void rtc_handler(nrf_drv_rtc_int_type_t int_type)
//{
//    if (int_type == NRF_DRV_RTC_INT_COMPARE0)
//    {
//        nrf_gpio_pin_toggle(13);
//    }
//    else if (int_type == NRF_DRV_RTC_INT_TICK)
//    {
//        nrf_gpio_pin_toggle(14);
//    }
//}

//static void rtc_config(void)
//{
//    uint32_t err_code;

//    //Initialize RTC instance
//    nrf_drv_rtc_config_t config = NRF_DRV_RTC_DEFAULT_CONFIG;
//    config.prescaler = 4095;
//    err_code = nrf_drv_rtc_init(&rtc, &config, rtc_handler);
//    APP_ERROR_CHECK(err_code);

//    //Enable tick event & interrupt
//    nrf_drv_rtc_tick_enable(&rtc,true);

//    //Set compare channel to trigger interrupt after COMPARE_COUNTERTIME seconds
//    //err_code = nrf_drv_rtc_cc_set(&rtc,0,COMPARE_COUNTERTIME * 8,true);
//    //APP_ERROR_CHECK(err_code);

//    //Power on RTC instance
//    nrf_drv_rtc_enable(&rtc);
//}
//uint32_t get_rtc_counter(void)

//{
    

//    return NRF_RTC2->COUNTER;
//}
static void lfclk_config(void)
{
    ret_code_t err_code = nrf_drv_clock_init();
    APP_ERROR_CHECK(err_code);

    nrf_drv_clock_lfclk_request(NULL);
}
    uint32_t err_code;

int main(void)
{
    uint32_t i;

    lfclk_config();
    bsp_board_init(BSP_INIT_LEDS);

    //  rtc_config();

 //  err_code = NRF_LOG_INIT(get_rtc_counter,32768);

//   err_code = NRF_LOG_INIT(NULL);

  


   //////////////
 #if NRF_LOG_USES_TIMESTAMP==1
    counter_init();
    counter_start();
    
   err_code = NRF_LOG_INIT(counter_get);
    #else
    ret_code_t err_code = NRF_LOG_INIT(NULL);
    #endif


   //////////////
    APP_ERROR_CHECK(err_code);

    NRF_LOG_DEFAULT_BACKENDS_INIT();
      NRF_LOG_INFO("Time Between 2 nrf  time stamp Log calls:START");
    NRF_LOG_INFO("Time Between 2 nrf timestamp  Log calls:STOP");

    NRF_LOG_INFO(""
                 "QSPI write and read example using 24bit addressing mode");

    srand(2);
    for (i = 0; i < QSPI_TEST_DATA_SIZE; ++i)
    {
        m_buffer_tx[i] = (uint8_t)rand();
    }

    nrf_drv_qspi_config_t config = NRF_DRV_QSPI_DEFAULT_CONFIG;

  
    err_code = nrf_drv_qspi_init(&config, qspi_handler, NULL);
    APP_ERROR_CHECK(err_code);
    NRF_LOG_INFO("QSPI example started waiting for 500ms.");
    nrf_delay_ms(500);
    configure_memory();

    m_finished = false;
     NRF_LOG_INFO("Calling Erase Function to erase block of 64KB");
    // nrf_gpio_pin_toggle(13);
    err_code = nrf_drv_qspi_erase(NRF_QSPI_ERASE_LEN_64KB, 0);
//    nrf_gpio_pin_toggle(13);

 //    NRF_LOG_INFO("EraseCommand sent waiting for qspi handler event");
   //err_code = nrf_drv_qspi_erase(NRF_QSPI_ERASE_LEN_ALL, 0);
 // err_code = nrf_drv_qspi_chip_erase();
    APP_ERROR_CHECK(err_code);


  //  nrf_gpio_pin_toggle(14);
    WAIT_FOR_PERIPH();
  //  nrf_gpio_pin_toggle(14);

   // NRF_LOG_INFO("Process of erasing first block start");
    NRF_LOG_INFO("Erase Done");
//     nrf_delay_ms(500);

 NRF_LOG_INFO("Calling Read Function to read  256 bytes");

  //  nrf_gpio_pin_toggle(13);
    err_code = nrf_drv_qspi_read(m_buffer_rx, QSPI_TEST_DATA_SIZE, 0);
   // nrf_gpio_pin_toggle(13);

   //   NRF_LOG_INFO("Read Command sent waiting for qspi handler event");
   //nrf_gpio_pin_toggle(14);
    WAIT_FOR_PERIPH();
  //  nrf_gpio_pin_toggle(14);
   // NRF_LOG_INFO("Data read");
     NRF_LOG_INFO("Read Done");

    NRF_LOG_INFO("Calling Write Function to write 256 bytes-1");

  //   nrf_gpio_pin_toggle(13);
    err_code = nrf_drv_qspi_write(m_buffer_tx, QSPI_TEST_DATA_SIZE, 0);
 //   nrf_gpio_pin_toggle(13);

  //    NRF_LOG_INFO("Write Command Sent waiting for qspi handler event" );
    APP_ERROR_CHECK(err_code);

    //nrf_gpio_pin_toggle(14);
    WAIT_FOR_PERIPH();
    //nrf_gpio_pin_toggle(14);
  //  NRF_LOG_INFO("Process of writing data start");

    NRF_LOG_INFO("Write Done");
    
    NRF_LOG_INFO("Calling Read Function to read  256 bytes");

  //  nrf_gpio_pin_toggle(13);
    err_code = nrf_drv_qspi_read(m_buffer_rx, QSPI_TEST_DATA_SIZE, 0);
   // nrf_gpio_pin_toggle(13);

   //   NRF_LOG_INFO("Read Command sent waiting for qspi handler event");
   //nrf_gpio_pin_toggle(14);
    WAIT_FOR_PERIPH();
  //  nrf_gpio_pin_toggle(14);
   // NRF_LOG_INFO("Data read");
     NRF_LOG_INFO("Read Done");

    NRF_LOG_INFO("Compare read and write buffer");
    if (memcmp(m_buffer_tx, m_buffer_rx, QSPI_TEST_DATA_SIZE) == 0)
    {
        NRF_LOG_INFO("Data consistent");
    }
    else
    {
        NRF_LOG_INFO("Data inconsistent");
    }

    nrf_drv_qspi_uninit();
    NRF_LOG_INFO("--------------------------------");

    for (;;)
    {
    }
}

/** @} */
