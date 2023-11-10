#include <stdbool.h>
#include "led_strip.h"
// Turn the LED on or off depending on the input bool
void blink_led(led_strip_handle_t*, bool);
// Configure a LED strip
void configure_led(led_strip_handle_t*);