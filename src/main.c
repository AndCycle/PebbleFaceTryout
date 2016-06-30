#include <pebble.h>
#include "main.h"

#include "calendar.h"
#include "analog.h"
#include "digit.h"
#include "bluetooth.h"
#include "battery.h"
#include "weather.h"

#include "extra.h"
  
Window *s_main_window;

GColor default_color;
GColor default_bg_color;

bool enable_second;
time_t enable_second_expire;
const time_t enable_second_age = 60;

static void inbox_received_callback(DictionaryIterator *iterator, void *context) {
	
  APP_LOG(APP_LOG_LEVEL_DEBUG_VERBOSE, "message inbox received");
  
	if (dict_find(iterator, MESSAGE_KEY_JS_READY)) {
		req_weather_update();
	}
	
	if (dict_find(iterator, MESSAGE_KEY_WEATHER)) {
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
	
	if (enable_second) {
		if (time(NULL) > enable_second_expire) {
			APP_LOG(APP_LOG_LEVEL_DEBUG_VERBOSE, "disable sec");
			enable_second = false;
			tick_timer_service_unsubscribe();
			tick_timer_service_subscribe(MINUTE_UNIT, tick_handler);
			units_changed |= MINUTE_UNIT;
			refresh_analog_enable_second();
		}
	}
	
  digit_tick_handler(tick_time, units_changed);
	analog_tick_handler(tick_time, units_changed);
	//calendar_tick_handler(tick_time, units_changed);
	weather_handler(tick_time, units_changed);
}


static void main_window_load(Window *window) {
	
	load_bluetooth(window);
	load_battery(window);
	load_weather(window);
	load_digit(window);
	//load_calendar(window);
	
	load_analog(window);
}

static void main_window_unload(Window *window) {
	unload_bluetooth(window);
	unload_battery(window);
	unload_weather(window);
	//unload_calendar(window);
	unload_analog(window);
	unload_digit(window);
}


void tap_handler(AccelAxisType axis, int32_t direction) {
	APP_LOG(APP_LOG_LEVEL_DEBUG_VERBOSE, "accel tap!");
	enable_second_expire = time(NULL) + enable_second_age;
	if (!enable_second) {
		APP_LOG(APP_LOG_LEVEL_DEBUG_VERBOSE, "enable sec");
		enable_second = true;
		tick_timer_service_unsubscribe();
		tick_timer_service_subscribe(SECOND_UNIT, tick_handler);	
		refresh_analog_enable_second();
	}
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
		tick_timer_service_subscribe(MINUTE_UNIT|HOUR_UNIT|DAY_UNIT|MONTH_UNIT|YEAR_UNIT, tick_handler);
	}

	//accel_tap_service_subscribe(tap_handler);

  // Show the Window on the watch, with animated=true
  window_stack_push(s_main_window, false);
	
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