#include <pebble.h>
#include "main.h"

#include "calendar.h"
#include "analog.h"
#include "digit.h"
#include "bluetooth.h"
#include "battery.h"
#include "test.h"

Window *s_main_window;

GColor default_color;
GColor default_bg_color;

bool enable_second;

static void tick_handler(struct tm *tick_time, TimeUnits units_changed) {
  digit_tick_handler(tick_time, units_changed);
	analog_tick_handler(tick_time, units_changed);
	calendar_tick_handler(tick_time, units_changed);
}


static void main_window_load(Window *window) {
	
	window_set_background_color(window, default_bg_color);
	
	// draw analog part

	load_analog(window);
	load_calendar(window);
	load_digit(window);
	load_bluetooth(window);
	load_battery(window);
	
	update_digit_time();
	update_calendar();
	bluetooth_handler(connection_service_peek_pebble_app_connection());
	battery_handler(battery_state_service_peek());
}

static void main_window_unload(Window *window) {
	unload_calendar(window);
	unload_analog(window);
	unload_digit(window);
	unload_bluetooth(window);
	unload_battery(window);
	layer_destroy(s_test_layer);
}

static void init() {
	
	default_color = GColorBlack;
	default_bg_color = GColorWhite;
	
	enable_second = false;
	enable_second = true;
	
	init_calendar();
	init_digit();
	init_bluetooth();
	init_battery();
	
	// Create main Window element and assign to pointer
  s_main_window = window_create();

  // Set handlers to manage the elements inside the Window
  window_set_window_handlers(s_main_window, (WindowHandlers) {
    .load = main_window_load,
    .unload = main_window_unload
  });

	// Register with TickTimerService
	
	if (enable_second) {
		tick_timer_service_subscribe(SECOND_UNIT, tick_handler);	
	} else {
		tick_timer_service_subscribe(MINUTE_UNIT, tick_handler);
	}


  // Show the Window on the watch, with animated=true
  window_stack_push(s_main_window, true);

  connection_service_subscribe((ConnectionHandlers) {
    .pebble_app_connection_handler = bluetooth_handler
  });

	battery_state_service_subscribe(battery_handler);
}

static void deinit() {
  // Destroy Window
  window_destroy(s_main_window);
	deinit_calendar();
	deinit_digit();
	deinit_bluetooth();
	deinit_battery();
}

int main(void) {
	init();
	app_event_loop();
	deinit();
}