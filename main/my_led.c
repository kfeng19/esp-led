#include <stdint.h>
#include "driver/gpio.h"
#include "led_strip.h"
#include "esp_log.h"
#include "var.h"
#include "my_led.h"

/* Use project configuration menu (idf.py menuconfig) to choose the GPIO to blink,
   or you can edit the following line and set a number here.
*/
#define BLINK_GPIO CONFIG_BLINK_GPIO


/*
   Return a RGB colour value given a scalar v in the range [vmin,vmax]
   In this case each colour component ranges from 0 (no contribution) to
   1 (fully saturated), modifications for other ranges is trivial.
   The colour is clipped at the end of the scales if v is outside
   the range [vmin,vmax]
   https://paulbourke.net/miscellaneous/colourspace/
   https://stackoverflow.com/questions/7706339/grayscale-to-red-green-blue-matlab-jet-color-scale
*/

typedef struct {
    double r,g,b;
} color_t;

color_t GetColour(double v,double vmin,double vmax)
{
   color_t c = {1.0,1.0,1.0}; // white
   double dv;

   if (v < vmin)
      v = vmin;
   if (v > vmax)
      v = vmax;
   dv = vmax - vmin;

   if (v < (vmin + 0.25 * dv)) {
      c.r = 0;
      c.g = 4 * (v - vmin) / dv;
   } else if (v < (vmin + 0.5 * dv)) {
      c.r = 0;
      c.b = 1 + 4 * (vmin + 0.25 * dv - v) / dv;
   } else if (v < (vmin + 0.75 * dv)) {
      c.r = 4 * (v - vmin - 0.5 * dv) / dv;
      c.b = 0;
   } else {
      c.g = 1 + 4 * (vmin + 0.75 * dv - v) / dv;
      c.b = 0;
   }

   return(c);
}

#ifdef CONFIG_BLINK_LED_STRIP


void inv_jet(led_strip_handle_t *led_strip, double level, double min, double max, double scale){
    if (scale < 0 || scale > 1) ESP_LOGE(TAG, "Scale must be between 0 and 1");
    color_t color = GetColour(level, min, max);
    int r = color.r * 255 * scale;
    int g = color.g * 255 * scale;
    int b = color.b * 255 * scale;
    led_strip_set_pixel(*led_strip, 0, b, g, r); // Flip r and b
    led_strip_refresh(*led_strip);
}

void blink_led(led_strip_handle_t *led_strip, bool s_led_state)
{
    ESP_LOGI(TAG, "Turning the LED %s!", s_led_state == true ? "ON" : "OFF");
    /* If the addressable LED is enabled */
    if (s_led_state) {
        /* Set the LED pixel using RGB from 0 (0%) to 255 (100%) for each color */
        led_strip_set_pixel(*led_strip, 0, 16, 16, 16);
        /* Refresh the strip to send data */
        led_strip_refresh(*led_strip);
    } else {
        /* Set all LED off to clear all pixels */
        led_strip_clear(*led_strip);
    }
}

void configure_led(led_strip_handle_t* led_strip)
{
    ESP_LOGI(TAG, "Example configured to blink addressable LED!");
    /* LED strip initialization with the GPIO and pixels number*/
    led_strip_config_t strip_config = {
        .strip_gpio_num = BLINK_GPIO,
        .max_leds = 1, // at least one LED on board
    };
#if CONFIG_BLINK_LED_STRIP_BACKEND_RMT
    led_strip_rmt_config_t rmt_config = {
        .resolution_hz = 10 * 1000 * 1000, // 10MHz
        .flags.with_dma = false,
    };
    ESP_ERROR_CHECK(led_strip_new_rmt_device(&strip_config, &rmt_config, led_strip));
#elif CONFIG_BLINK_LED_STRIP_BACKEND_SPI
    led_strip_spi_config_t spi_config = {
        .spi_bus = SPI2_HOST,
        .flags.with_dma = true,
    };
    ESP_ERROR_CHECK(led_strip_new_spi_device(&strip_config, &spi_config, led_strip));
#else
#error "unsupported LED strip backend"
#endif
    /* Set all LED off to clear all pixels */
    led_strip_clear(*led_strip);
}

#elif CONFIG_BLINK_LED_GPIO

void blink_led(bool s_led_state)
{
    ESP_LOGI(TAG, "Turning the LED %s!", s_led_state == true ? "ON" : "OFF");
    /* Set the GPIO level according to the state (LOW or HIGH)*/
    gpio_set_level(BLINK_GPIO, s_led_state);
}

void configure_led(void)
{
    ESP_LOGI(TAG, "Example configured to blink GPIO LED!");
    gpio_reset_pin(BLINK_GPIO);
    /* Set the GPIO as a push/pull output */
    gpio_set_direction(BLINK_GPIO, GPIO_MODE_OUTPUT);
}

#else
#error "unsupported LED type"
#endif