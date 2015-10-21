#include <pebble.h>
#include "digit.h"
#include "main.h"

TextLayer *s_digit_layer;

char *b_digit;

void update_digit_time() {
	// Get a tm structure
  time_t temp = time(NULL); 
  struct tm *tick_time = localtime(&temp);
	
	if (enable_second) {
		strftime(b_digit, sizeof(char)*9, "%H:%M:%S", tick_time);	
	} else {
		strftime(b_digit, sizeof(char)*9, "%H:%M", tick_time);
	}
	
	text_layer_set_text(s_digit_layer, b_digit);
}

void digit_tick_handler(struct tm *tick_time, TimeUnits units_changed) {
	if (enable_second && units_changed & SECOND_UNIT) {
		update_digit_time();
	} else if (units_changed & MINUTE_UNIT) {
		update_digit_time();
	}
}

void init_digit() {
	b_digit = calloc(9, sizeof(char));
}
void deinit_digit() {
	free(b_digit);
}

void load_digit(Window *window) {
	//GRect window_grect = layer_get_frame(window_get_root_layer(window));
	GRect digit_grect = GRect(116, 147, 33, 32);
	
	s_digit_layer = text_layer_create(digit_grect);
	
	text_layer_set_font(s_digit_layer, fonts_get_system_font(FONT_KEY_GOTHIC_14));
	text_layer_set_text_color(s_digit_layer, default_color);
	text_layer_set_overflow_mode(s_digit_layer, GTextOverflowModeWordWrap);
	text_layer_set_background_color(s_digit_layer, GColorClear);
	text_layer_set_text_alignment(s_digit_layer, GTextAlignmentLeft);
	layer_add_child(window_get_root_layer(window), text_layer_get_layer(s_digit_layer));
	
}

void unload_digit(Window * window) {
	text_layer_destroy(s_digit_layer);
}