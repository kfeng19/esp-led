/* Blink Example

   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/
#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "sdkconfig.h"
#include "my_led.h"
#include "var.h"
#include "stdbool.h"
#include "led_strip.h"

// bool s_led_state = false;
led_strip_handle_t my_led;

void app_main(void)
{

    /* Configure the peripheral according to the LED type */
    configure_led(&my_led);
    double level = 0;
    bool up = true;
    while (1) {
        if (level > 99) up = false;
        if (level < 1 ) up = true;
        // blink_led(&my_led, s_led_state);
        /* Toggle the LED state */
        // s_led_state = !s_led_state;
        inv_jet(&my_led, level, 0, 100, 0.3);
        if (up) level += 1;
        else level -= 1;
        vTaskDelay(CONFIG_BLINK_PERIOD / portTICK_PERIOD_MS);
    }
}
