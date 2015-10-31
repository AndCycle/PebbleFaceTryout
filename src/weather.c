#include <pebble.h>
#include "weather.h"
#include "main.h"

TextLayer *s_weather_layer;

void req_weather_update() {
	APP_LOG(APP_LOG_LEVEL_DEBUG_VERBOSE, "update weather");
	// Begin dictionary
	DictionaryIterator *iter;
	app_message_outbox_begin(&iter);

	// Add a key-value pair
	//dict_write_uint8(iter, 0, 0);
	//dict_write_cstring(iter, KEY_FUNC, "weather");
	dict_write_uint8(iter, KEY_WEATHER, true);

	dict_write_end(iter);
	
	// Send the message!
	app_message_outbox_send();
	
}

void process_weather_app_message(DictionaryIterator *iterator, void *context) {
	APP_LOG(APP_LOG_LEVEL_DEBUG_VERBOSE, "do weather");
	
  // Store incoming information
  static char temperature_buffer[8];
  static char conditions_buffer[32];
  static char weather_layer_buffer[32];
  
  // Read first item
  Tuple *t = dict_read_first(iterator);

  // For all items
  while(t != NULL) {
    // Which key was received?
    switch(t->key) {
    case KEY_WEATHER_TEMPERATURE:
			APP_LOG(APP_LOG_LEVEL_DEBUG_VERBOSE, "%d", (int)t->value->int32);
      snprintf(temperature_buffer, sizeof(temperature_buffer), "%dC", (int)t->value->int32);
      break;
    case KEY_WEATHER_CONDITIONS:
			//APP_LOG(APP_LOG_LEVEL_DEBUG_VERBOSE, "%s", t->value->cstring);
      //snprintf(conditions_buffer, sizeof(conditions_buffer), "%s", t->value->cstring);
			APP_LOG(APP_LOG_LEVEL_DEBUG_VERBOSE, "%d", (int)t->value->int32);
      snprintf(conditions_buffer, sizeof(conditions_buffer), "%d", (int)t->value->int32);
      break;
		case KEY_WEATHER:
			// just identifier
			break;
    default:
      APP_LOG(APP_LOG_LEVEL_ERROR, "Key %d not recognized!", (int)t->key);
      break;
    }

    // Look for next item
    t = dict_read_next(iterator);
  }

  // Assemble full string and display
  snprintf(weather_layer_buffer, sizeof(weather_layer_buffer), "%s, %s", temperature_buffer, conditions_buffer);
	text_layer_set_text(s_weather_layer, weather_layer_buffer);
	
}

void weather_handler(struct tm * tick_time, TimeUnits units_changed) {
	  
	//APP_LOG(APP_LOG_LEVEL_DEBUG_VERBOSE, "weather min %d", tick_time->tm_min);
  // Get weather update every 30 minutes
  //if((tick_time->tm_min % 30 == 0) && (tick_time->tm_sec == 0)) {
	if(units_changed & HOUR_UNIT) {
		req_weather_update();
  }
}

void load_weather(Window *window) {
	s_weather_layer = text_layer_create(GRect(0,0,144,50));
	//text_layer_set_text_color(s_weather_layer, GColorWhite);
  text_layer_set_text_alignment(s_weather_layer, GTextAlignmentCenter);
	text_layer_set_background_color(s_weather_layer, GColorClear);
	text_layer_set_font(s_weather_layer, fonts_get_system_font(FONT_KEY_GOTHIC_14));
  text_layer_set_text(s_weather_layer, "Loading...");
  
  // Create second custom font, apply it and add to Window

  layer_add_child(window_get_root_layer(window), text_layer_get_layer(s_weather_layer));
}

void unload_weather(Window *window) {
	text_layer_destroy(s_weather_layer);
	
}

