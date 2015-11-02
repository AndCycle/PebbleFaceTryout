#include <pebble.h>
#include "weather.h"
#include "main.h"

TextLayer *s_weather_layer;

GBitmap *s_weather_icon_bitmap;
BitmapLayer *s_weather_icon_layer;
Layer *s_weather_icon_draw_layer;

TextLayer *s_weather_temp_min;
TextLayer *s_weather_temp_max;

GDrawCommandImage *s_weather_cmd_img;

enum {
	KEY_WEATHER = 2,
	KEY_WEATHER_TEMPERATURE = 3,
	KEY_WEATHER_TEMPERATURE_MIN = 4,
	KEY_WEATHER_TEMPERATURE_MAX = 5,
	KEY_WEATHER_CONDITIONS_ID = 6,
	KEY_WEATHER_ICON_ID = 7
};

enum {
	PERSIST_KEY_WEATHER_FETCH_TIME,
	PERSIST_KEY_WEATHER_TEMPERATURE,
	PERSIST_KEY_WEATHER_TEMPERATURE_MIN,
	PERSIST_KEY_WEATHER_TEMPERATURE_MAX,
	PERSIST_KEY_WEATHER_CONDITION_ID,
	PERSIST_KEY_WEATHER_ICON_ID
};

const time_t valid_weather_age = 60*60*2;

time_t last_weather_fetch_time = 0;
int32_t last_weather_temperature = 0;
int32_t last_weather_temperature_min = 0;
int32_t last_weather_temperature_max = 0;
int32_t last_weather_condition_id = 0;
char last_weather_icon_id[4];

bool weather_expire() {
	if ((last_weather_fetch_time+valid_weather_age) < time(NULL)) {
		return true;
	}
	return false;
}

void req_weather_update() {
	APP_LOG(APP_LOG_LEVEL_DEBUG_VERBOSE, "update weather");
	
	if (!weather_expire()) { 
		APP_LOG(APP_LOG_LEVEL_DEBUG_VERBOSE, "weather not expire, skip update req");
		return; 
	}
	
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

static void update_weather_cmd_img_proc(Layer *layer, GContext *ctx) {

  // If the image was loaded successfully...
  if (s_weather_cmd_img) {
    // Draw it
    gdraw_command_image_draw(ctx, s_weather_cmd_img, GPoint(0,0));
  }
}

void weather_display_refresh() {
	
	APP_LOG(APP_LOG_LEVEL_DEBUG_VERBOSE, "weather display refresh");
	
  // Store incoming information
  static char temperature_buffer[8];
	static char temperature_min_buffer[8];
	static char temperature_max_buffer[8];
  static char conditions_buffer[32];
  static char weather_layer_buffer[32];
	
	if (last_weather_fetch_time == 0) {
		text_layer_set_text(s_weather_layer, "Loading...");
		return;
	}
	
	snprintf(temperature_buffer, sizeof(temperature_buffer), "%d°", (int)last_weather_temperature);
	snprintf(temperature_min_buffer, sizeof(temperature_min_buffer), "%d°", (int)last_weather_temperature_min);
	snprintf(temperature_max_buffer, sizeof(temperature_max_buffer), "%d°", (int)last_weather_temperature_max);
	snprintf(conditions_buffer, sizeof(conditions_buffer), "%d", (int)last_weather_condition_id);
		
  // Assemble full string and display
  snprintf(weather_layer_buffer, sizeof(weather_layer_buffer), "%s, %s", temperature_buffer, conditions_buffer);
	text_layer_set_text(s_weather_layer, weather_layer_buffer);
	text_layer_set_text(s_weather_temp_min, temperature_min_buffer);
	text_layer_set_text(s_weather_temp_max, temperature_max_buffer);
	


	
	gdraw_command_image_destroy(s_weather_cmd_img);
	
	switch (last_weather_condition_id) {
		//Group 2xx: Thunderstorm
		case 200:
		case 201:
		case 202:
		case 210:
		case 211:
		case 212:
		case 221:
		case 230:
		case 231:
		case 232:
			s_weather_cmd_img = gdraw_command_image_create_with_resource(RESOURCE_ID_RAINING_AND_SNOWING);
			break;
				
		//Group 3xx: Drizzle
		case 300:
		case 301:
		case 302:
		case 310:
		case 311:
		case 312:
		case 313:
		case 314:
		case 321:
				s_weather_cmd_img = gdraw_command_image_create_with_resource(RESOURCE_ID_HEAVY_RAIN);
				break;
		
		//Group 5xx: Rain
		case 500:
				s_weather_cmd_img = gdraw_command_image_create_with_resource(RESOURCE_ID_LIGHT_RAIN);
				break;
		case 501:
		case 502:
		case 503:
		case 504:
				s_weather_cmd_img = gdraw_command_image_create_with_resource(RESOURCE_ID_HEAVY_RAIN);
				break;
		case 511:
				s_weather_cmd_img = gdraw_command_image_create_with_resource(RESOURCE_ID_HEAVY_SNOW);
				break;
		case 520:
		case 521:
		case 522:
		case 531:
				s_weather_cmd_img = gdraw_command_image_create_with_resource(RESOURCE_ID_HEAVY_RAIN);
				break;
		
		//Group 6xx: Snow
		case 600:
				s_weather_cmd_img = gdraw_command_image_create_with_resource(RESOURCE_ID_LIGHT_SNOW);
				break;
		case 601:
		case 602:
		case 611:
		case 612:
		case 615:
		case 616:
		case 620:
		case 621:
		case 622:
				s_weather_cmd_img = gdraw_command_image_create_with_resource(RESOURCE_ID_HEAVY_SNOW);
				break;
		
		//Group 7xx: Atmosphere
		case 701:
		case 711:
		case 721:
		case 731:
		case 741:
		case 751:
		case 761:
		case 762:
		case 771:
		case 781:
			break;
		
		//Group 800: Clear
		case 800:
				s_weather_cmd_img = gdraw_command_image_create_with_resource(RESOURCE_ID_TIMELINE_SUN);
				break;
		
		//Group 80x: Clouds
		case 801:
		case 802:
				s_weather_cmd_img = gdraw_command_image_create_with_resource(RESOURCE_ID_PARTLY_CLOUDY);
				break;
		case 803:
		case 804:
				s_weather_cmd_img = gdraw_command_image_create_with_resource(RESOURCE_ID_CLOUDY_DAY);
				break;
		
		//Group 90x: Extreme
		case 900:
		case 901:
		case 902:
		case 903:
		case 904:
		case 905:
		case 906:
			break;
				
		//Group 9xx: Additional
		case 951:
		case 952:
		case 953:
		case 954:
		case 955:
		case 956:
		case 957:
		case 958:
		case 959:
		case 960:
		case 961:
		case 962:
			break;
	
		default:
			break;
		
	}
	
	if (!s_weather_cmd_img) {
		s_weather_cmd_img = gdraw_command_image_create_with_resource(RESOURCE_ID_GENERIC_WARNING);	
	}
		
	
	layer_mark_dirty(s_weather_icon_draw_layer);
	
	/*
	
	gbitmap_destroy(s_weather_icon_bitmap);
	
	if (strncmp(last_weather_icon_id, "01d", 4) == 0) {
		s_weather_icon_bitmap = gbitmap_create_with_resource(RESOURCE_ID_OWM_01D);
	} else if (strncmp(last_weather_icon_id, "01n", 4) == 0) {
		s_weather_icon_bitmap = gbitmap_create_with_resource(RESOURCE_ID_OWM_01N);
	} else if (strncmp(last_weather_icon_id, "02d", 4) == 0) {
		s_weather_icon_bitmap = gbitmap_create_with_resource(RESOURCE_ID_OWM_02D);
	} else if (strncmp(last_weather_icon_id, "02n", 4) == 0) {
		s_weather_icon_bitmap = gbitmap_create_with_resource(RESOURCE_ID_OWM_02N);
	} else if (strncmp(last_weather_icon_id, "03d", 4) == 0) {
		s_weather_icon_bitmap = gbitmap_create_with_resource(RESOURCE_ID_OWM_03D);
	} else if (strncmp(last_weather_icon_id, "03n", 4) == 0) {
		s_weather_icon_bitmap = gbitmap_create_with_resource(RESOURCE_ID_OWM_03N);
	} else if (strncmp(last_weather_icon_id, "04d", 4) == 0) {
		s_weather_icon_bitmap = gbitmap_create_with_resource(RESOURCE_ID_OWM_04D);
	} else if (strncmp(last_weather_icon_id, "04n", 4) == 0) {
		s_weather_icon_bitmap = gbitmap_create_with_resource(RESOURCE_ID_OWM_04N);
	} else if (strncmp(last_weather_icon_id, "09d", 4) == 0) {
		s_weather_icon_bitmap = gbitmap_create_with_resource(RESOURCE_ID_OWM_09D);
	} else if (strncmp(last_weather_icon_id, "09n", 4) == 0) {
		s_weather_icon_bitmap = gbitmap_create_with_resource(RESOURCE_ID_OWM_09N);
	} else if (strncmp(last_weather_icon_id, "10d", 4) == 0) {
		s_weather_icon_bitmap = gbitmap_create_with_resource(RESOURCE_ID_OWM_10D);
	} else if (strncmp(last_weather_icon_id, "10n", 4) == 0) {
		s_weather_icon_bitmap = gbitmap_create_with_resource(RESOURCE_ID_OWM_10N);
	} else if (strncmp(last_weather_icon_id, "11d", 4) == 0) {
		s_weather_icon_bitmap = gbitmap_create_with_resource(RESOURCE_ID_OWM_11D);
	} else if (strncmp(last_weather_icon_id, "11n", 4) == 0) {
		s_weather_icon_bitmap = gbitmap_create_with_resource(RESOURCE_ID_OWM_11N);
	} else if (strncmp(last_weather_icon_id, "13d", 4) == 0) {
		s_weather_icon_bitmap = gbitmap_create_with_resource(RESOURCE_ID_OWM_13D);
	} else if (strncmp(last_weather_icon_id, "13n", 4) == 0) {
		s_weather_icon_bitmap = gbitmap_create_with_resource(RESOURCE_ID_OWM_13N);
	} else if (strncmp(last_weather_icon_id, "50d", 4) == 0) {
		s_weather_icon_bitmap = gbitmap_create_with_resource(RESOURCE_ID_OWM_50D);
	} else if (strncmp(last_weather_icon_id, "50n", 4) == 0) {
		s_weather_icon_bitmap = gbitmap_create_with_resource(RESOURCE_ID_OWM_50N);
	}
	
	bitmap_layer_set_bitmap(s_weather_icon_layer, s_weather_icon_bitmap);
	
	*/
}

void process_weather_app_message(DictionaryIterator *iterator, void *context) {
	APP_LOG(APP_LOG_LEVEL_DEBUG_VERBOSE, "do weather update");
	
  // Read first item
  Tuple *t = dict_read_first(iterator);

  // For all items
  while(t != NULL) {
    // Which key was received?
    switch(t->key) {
    case KEY_WEATHER_TEMPERATURE:
			APP_LOG(APP_LOG_LEVEL_DEBUG_VERBOSE, "temperature %d", (int)t->value->int32);
  		last_weather_temperature = t->value->int32;
      break;
		case KEY_WEATHER_TEMPERATURE_MIN:
			APP_LOG(APP_LOG_LEVEL_DEBUG_VERBOSE, "temperature %d", (int)t->value->int32);
  		last_weather_temperature_min = t->value->int32;
      break;
		case KEY_WEATHER_TEMPERATURE_MAX:
			APP_LOG(APP_LOG_LEVEL_DEBUG_VERBOSE, "temperature %d", (int)t->value->int32);
  		last_weather_temperature_max = t->value->int32;
      break;
    case KEY_WEATHER_CONDITIONS_ID:
			//APP_LOG(APP_LOG_LEVEL_DEBUG_VERBOSE, "%s", t->value->cstring);
      //snprintf(conditions_buffer, sizeof(conditions_buffer), "%s", t->value->cstring);
			APP_LOG(APP_LOG_LEVEL_DEBUG_VERBOSE, "condition id %d", (int)t->value->int32);
			last_weather_condition_id = t->value->int32;
      break;
		case KEY_WEATHER_ICON_ID:
			APP_LOG(APP_LOG_LEVEL_DEBUG_VERBOSE, "icon id %s", t->value->cstring);
			snprintf(last_weather_icon_id, sizeof(last_weather_icon_id), "%s", t->value->cstring);
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

	last_weather_fetch_time = time(NULL);

	weather_display_refresh();
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
	text_layer_set_text_color(s_weather_layer, default_color);
  text_layer_set_text_alignment(s_weather_layer, GTextAlignmentCenter);
	text_layer_set_background_color(s_weather_layer, GColorClear);
	text_layer_set_font(s_weather_layer, fonts_get_system_font(FONT_KEY_GOTHIC_14));
	
	s_weather_temp_min = text_layer_create(GRect(126,34,144,50));
	text_layer_set_text_color(s_weather_temp_min, default_color);
	text_layer_set_background_color(s_weather_temp_min, GColorClear);
	text_layer_set_font(s_weather_temp_min, fonts_get_system_font(FONT_KEY_GOTHIC_14));
	
	s_weather_temp_max = text_layer_create(GRect(126,23,144,50));
	text_layer_set_text_color(s_weather_temp_max, default_color);
	text_layer_set_background_color(s_weather_temp_max, GColorClear);
	text_layer_set_font(s_weather_temp_max, fonts_get_system_font(FONT_KEY_GOTHIC_14));
	
  s_weather_icon_layer = bitmap_layer_create(GRect(100, -8, 50, 50));
	//bitmap_layer_set_background_color(s_weather_icon_layer, GColorWhite);
	bitmap_layer_set_compositing_mode(s_weather_icon_layer, GCompOpSet);
  	
	s_weather_icon_draw_layer = layer_create(GRect(118, 1, 25, 25));
	layer_set_update_proc(s_weather_icon_draw_layer, update_weather_cmd_img_proc);
	
	weather_display_refresh();

  layer_add_child(window_get_root_layer(window), text_layer_get_layer(s_weather_layer));
	layer_add_child(window_get_root_layer(window), text_layer_get_layer(s_weather_temp_max));
	layer_add_child(window_get_root_layer(window), text_layer_get_layer(s_weather_temp_min));
	layer_add_child(window_get_root_layer(window), bitmap_layer_get_layer(s_weather_icon_layer));
	layer_add_child(window_get_root_layer(window), s_weather_icon_draw_layer);
}

void unload_weather(Window *window) {
	text_layer_destroy(s_weather_layer);
	text_layer_destroy(s_weather_temp_min);
	text_layer_destroy(s_weather_temp_max);

	bitmap_layer_destroy(s_weather_icon_layer);
  gbitmap_destroy(s_weather_icon_bitmap);

	layer_destroy(s_weather_icon_draw_layer);
	gdraw_command_image_destroy(s_weather_cmd_img);
}

void init_weather() {
	if (persist_exists(PERSIST_KEY_WEATHER_FETCH_TIME)) {
		APP_LOG(APP_LOG_LEVEL_DEBUG_VERBOSE, "weather fetch last persist weather");
		last_weather_fetch_time = persist_read_int(PERSIST_KEY_WEATHER_FETCH_TIME);
	}
	if (persist_exists(PERSIST_KEY_WEATHER_TEMPERATURE)) {
		last_weather_temperature = persist_read_int(PERSIST_KEY_WEATHER_TEMPERATURE);
	}
	if (persist_exists(PERSIST_KEY_WEATHER_TEMPERATURE_MIN)) {
		last_weather_temperature_min = persist_read_int(PERSIST_KEY_WEATHER_TEMPERATURE_MIN);
	}
	if (persist_exists(PERSIST_KEY_WEATHER_TEMPERATURE_MAX)) {
		last_weather_temperature_max = persist_read_int(PERSIST_KEY_WEATHER_TEMPERATURE_MAX);
	}
	if (persist_exists(PERSIST_KEY_WEATHER_CONDITION_ID)) {
		last_weather_condition_id = persist_read_int(PERSIST_KEY_WEATHER_CONDITION_ID);
	}
	if (persist_exists(PERSIST_KEY_WEATHER_ICON_ID)) {
		persist_read_string(PERSIST_KEY_WEATHER_ICON_ID, last_weather_icon_id, 4);
	}
}

void deinit_weather() {
	persist_write_int(PERSIST_KEY_WEATHER_FETCH_TIME, last_weather_fetch_time);
	persist_write_int(PERSIST_KEY_WEATHER_TEMPERATURE, last_weather_temperature);
	persist_write_int(PERSIST_KEY_WEATHER_TEMPERATURE_MIN, last_weather_temperature);
	persist_write_int(PERSIST_KEY_WEATHER_TEMPERATURE_MAX, last_weather_temperature);
	persist_write_int(PERSIST_KEY_WEATHER_CONDITION_ID, last_weather_condition_id);
	persist_write_string(PERSIST_KEY_WEATHER_ICON_ID, last_weather_icon_id);
}
