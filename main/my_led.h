#include <stdbool.h>
#include "led_strip.h"
// Turn the LED on or off depending on the input bool
void blink_led(led_strip_handle_t*, bool);
// Configure a LED strip
void configure_led(led_strip_handle_t*);
// Use a inverse jetish color map to show level
void inv_jet(led_strip_handle_t *led_strip, double level, double min, double max, double scale);