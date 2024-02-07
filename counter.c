/**
 * Copyright (c) 2016 - 2020, Nordic Semiconductor ASA
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
/**@cond To Make Doxygen skip documentation generation for this file.
 * @{
 */

#include "counter.h"
#include "nrfx_rtc.h"
#include "nrf_gpio.h"


// RTC driver instance using RTC2.
// RTC0 is used by the SoftDevice, and RTC1 by the app_timer library.
static const nrfx_rtc_t m_rtc = NRFX_RTC_INSTANCE(2);
static volatile uint32_t overflow_counter = 0;

static void rtc_handler(nrfx_rtc_int_type_t int_type)
{
    if (int_type == NRFX_RTC_INT_OVERFLOW) overflow_counter++;    // Used for timer extension
  
    //nrf_gpio_pin_toggle(13);
}


void counter_init(void)
{
    ret_code_t err_code;

    // Initialize the RTC instance.
    nrfx_rtc_config_t config = NRFX_RTC_DEFAULT_CONFIG;

    // 1 ms interval.
    config.prescaler = 1;
    //config.prescaler = 256; // prescaler = 1 gives proper timestamps

    err_code = nrfx_rtc_init(&m_rtc, &config, rtc_handler);
    APP_ERROR_CHECK(err_code);

       //Enable tick event & interrupt
    nrfx_rtc_tick_enable(&m_rtc,true);
  //  nrfx_rtc_tick_disable(&m_rtc);
}


void counter_start(void)
{
    nrfx_rtc_counter_clear(&m_rtc);
    nrfx_rtc_overflow_enable(&m_rtc, true);  // Count overflow events to extend counter, see below
    // Power on!
    nrfx_rtc_enable(&m_rtc);
}


void counter_stop(void)
{
    nrfx_rtc_disable(&m_rtc);
}


uint32_t counter_get(void)
{
    uint32_t cnt = nrfx_rtc_counter_get(&m_rtc);
    cnt |= overflow_counter << 24;    // We artifically "extend" the counter from 24 bit to 32 bit doing so.
    return cnt;
}

/** @}
 *  @endcond
 */