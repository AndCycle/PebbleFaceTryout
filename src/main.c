#include <pebble.h>
#include "main.h"

#include "calendar.h"
#include "analog.h"
#include "digit.h"
#include "bluetooth.h"
#include "battery.h"
#include "weather.h"

#include "extra.h"
  
//#define KEY_TEMPERATURE 0
//#define KEY_CONDITIONS 1

enum {
	KEY_JS_READY = 0,
	KEY_WEATHER = 2,
};

Window *s_main_window;

GColor default_color;
GColor default_bg_color;

bool enable_second;

static void inbox_received_callback(DictionaryIterator *iterator, void *context) {
	
	if (dict_find(iterator, KEY_JS_READY)) {
		req_weather_update();
	}
	
	if (dict_find(iterator, KEY_WEATHER)) {
		APP_LOG(APP_LOG_LEVEL_DEBUG_VERBOSE, "process weather req result");
		process_weather_app_message(iterator, context);
	}
	
}

static void inbox_dropped_callback(AppMessageResult reason, void *context) {
  APP_LOG(APP_LOG_LEVEL_ERROR, "Message dropped!");
}

static void outbox_failed_callback(DictionaryIterator *iterator, AppMessageResult reason, void *context) {
  APP_LOG(APP_LOG_LEVEL_ERROR, "Outbox send failed!");
}

static void outbox_sent_callback(DictionaryIterator *iterator, void *context) {
  APP_LOG(APP_LOG_LEVEL_INFO, "Outbox send success!");
}

static void tick_handler(struct tm *tick_time, TimeUnits units_changed) {
	
  tick_time = sanitize_localtime(); //there seems no guarantee on tick_time, make our own.
	
  digit_tick_handler(tick_time, units_changed);
	analog_tick_handler(tick_time, units_changed);
	//calendar_tick_handler(tick_time, units_changed);
	weather_handler(tick_time, units_changed);
}


static void main_window_load(Window *window) {
	
	window_set_background_color(window, default_bg_color);
	
	load_analog(window);
	//load_calendar(window);
	load_digit(window);
	load_bluetooth(window);
	load_battery(window);
	load_weather(window);
}

static void main_window_unload(Window *window) {
	//unload_calendar(window);
	unload_analog(window);
	unload_digit(window);
	unload_bluetooth(window);
	unload_battery(window);
	unload_weather(window);
	
}

static void init() {
	
	default_color = GColorBlack;
	default_bg_color = GColorWhite;
	
	/*
	default_color = GColorWhite;
	default_bg_color = GColorBlack;
	*/
	
	enable_second = false;
	//enable_second = true;
	
	init_weather();
	
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

	// Register callbacks
	app_message_register_inbox_received(inbox_received_callback);
	app_message_register_inbox_dropped(inbox_dropped_callback);
	app_message_register_outbox_failed(outbox_failed_callback);
	app_message_register_outbox_sent(outbox_sent_callback);
	
	// Open AppMessage
	//app_message_open(app_message_inbox_size_maximum(), app_message_outbox_size_maximum());
	app_message_open(APP_MESSAGE_INBOX_SIZE_MINIMUM, APP_MESSAGE_OUTBOX_SIZE_MINIMUM);

}

static void deinit() {
  // Destroy Window
  window_destroy(s_main_window);
	
	deinit_weather();
}

int main(void) {
	init();
	app_event_loop();
	deinit();
}