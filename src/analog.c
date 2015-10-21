#include <pebble.h>
#include "analog.h"
#include "extra.h"
#include "main.h"

Layer *s_analog_hour_hand_layer;
Layer *s_analog_min_hand_layer;
Layer *s_analog_sec_hand_layer;


Layer *s_analog_tick_layer;

TextLayer *s_no12_layer[12];


int32_t hour_hand_length;
int32_t min_hand_length;
int32_t sec_hand_length;
int32_t analog_radius;

GRect s_no12_grect[12];
const char *s_no12buffer[12] = {"1", "2", "3", "4", "5", "6", "7", "8", "9", "10", "11", "12"};


void draw_analog_hour_hand_layer(Layer *layer, GContext *ctx) {
	GRect bounds = layer_get_bounds(layer);
	GOvalScaleMode default_govalscalemode = GOvalScaleModeFitCircle;
	GPoint center = ret_carry_center(bounds, default_govalscalemode);

	time_t now = time(NULL);
  struct tm *t = localtime(&now);

	int32_t hour_angle = (TRIG_MAX_ANGLE * (((t->tm_hour % 12) * 6) + (t->tm_min / 10))) / (12 * 6);
	
	graphics_context_set_stroke_color(ctx, default_color);
	
	graphics_context_set_stroke_width(ctx, 3);
	graphics_draw_line(ctx, center, gpoint_to_polar(center, hour_angle, hour_hand_length));
	
	graphics_context_set_stroke_width(ctx, 7);
	graphics_draw_line(ctx, gpoint_to_polar(center, hour_angle, analog_radius*0.2), 
										 gpoint_to_polar(center, hour_angle, hour_hand_length));
	
	graphics_context_set_stroke_width(ctx, 5);
	graphics_context_set_stroke_color(ctx, default_bg_color);
	graphics_draw_line(ctx, gpoint_to_polar(center, hour_angle, analog_radius*0.3), 
										 gpoint_to_polar(center, hour_angle, hour_hand_length*0.9));
}

void draw_analog_min_hand_layer(Layer *layer, GContext *ctx) {
	GRect bounds = layer_get_bounds(layer);
	GOvalScaleMode default_govalscalemode = GOvalScaleModeFitCircle;
	GPoint center = ret_carry_center(bounds, default_govalscalemode);

	time_t now = time(NULL);
  struct tm *t = localtime(&now);
	
	int32_t min_angle = TRIG_MAX_ANGLE * t->tm_min / 60;
	
	graphics_context_set_stroke_color(ctx, default_color);
	
	graphics_context_set_stroke_width(ctx, 3);
	graphics_draw_line(ctx, center, gpoint_to_polar(center, min_angle, min_hand_length));
	graphics_context_set_stroke_width(ctx, 5);
	graphics_draw_line(ctx, gpoint_to_polar(center, min_angle, analog_radius*0.2), 
										 gpoint_to_polar(center, min_angle, min_hand_length));
}

void draw_analog_sec_hand_layer(Layer *layer, GContext *ctx) {
	GRect bounds = layer_get_bounds(layer);
	GOvalScaleMode default_govalscalemode = GOvalScaleModeFitCircle;
	GPoint center = ret_carry_center(bounds, default_govalscalemode);
	
	time_t now = time(NULL);
  struct tm *t = localtime(&now);

	int32_t second_angle = TRIG_MAX_ANGLE * t->tm_sec / 60;
	graphics_context_set_stroke_color(ctx, default_color);
	graphics_draw_line(ctx, center, gpoint_to_polar(center, second_angle, sec_hand_length));
}

void draw_tick_layer(Layer *layer, GContext *ctx) {
	GRect bounds = layer_get_bounds(layer);
	GOvalScaleMode default_govalscalemode = GOvalScaleModeFitCircle;
	GPoint center = ret_carry_center(bounds, default_govalscalemode);
	
	int32_t radius = bounds.size.w/2;

	graphics_context_set_stroke_color(ctx, default_color);
	
	// fill min tick

	
	int32_t tick_length = radius*0.95;
	
	graphics_context_set_stroke_width(ctx, 1);

	for(int32_t angle=0; angle<TRIG_MAX_ANGLE; angle+=(TRIG_MAX_ANGLE/60)) {
		graphics_draw_line(ctx, gpoint_from_polar(bounds, default_govalscalemode, angle), gpoint_to_polar(center, angle, tick_length));
	}
	
	// fill hour tick
	
	graphics_context_set_stroke_width(ctx, 3);

	for(int32_t angle=0; angle<TRIG_MAX_ANGLE; angle+=(TRIG_MAX_ANGLE/12)) {
		graphics_draw_line(ctx, gpoint_from_polar(bounds, default_govalscalemode, angle), gpoint_to_polar(center, angle, tick_length));
	}
	
	graphics_context_set_stroke_color(ctx, default_color);
	graphics_context_set_fill_color(ctx, default_bg_color);
	graphics_context_set_stroke_width(ctx, 2);
	
	graphics_draw_circle(ctx, center, 3);
	graphics_fill_circle(ctx, center, 2);
}

void analog_tick_handler(struct tm *tick_time, TimeUnits units_changed) {
	if (enable_second && units_changed & SECOND_UNIT) {
		layer_mark_dirty(s_analog_sec_hand_layer);
	}
	if (units_changed & MINUTE_UNIT) {
		layer_mark_dirty(s_analog_min_hand_layer);
		layer_mark_dirty(s_analog_hour_hand_layer);
	}
}


void load_analog(Window *window) {
	
	GRect analog_grect = squre_grect(layer_get_frame(window_get_root_layer(window)));

	analog_grect.origin.y = 0;
	
	analog_radius = analog_grect.size.w/2;
	
	
	hour_hand_length = analog_radius*0.6;
	s_analog_hour_hand_layer = layer_create(analog_grect);
	layer_set_update_proc(s_analog_hour_hand_layer, draw_analog_hour_hand_layer);
	layer_add_child(window_get_root_layer(window), s_analog_hour_hand_layer);

	
	min_hand_length = analog_radius*0.8;
	s_analog_min_hand_layer = layer_create(analog_grect);
	layer_set_update_proc(s_analog_min_hand_layer, draw_analog_min_hand_layer);
	layer_add_child(window_get_root_layer(window), s_analog_min_hand_layer);
	
	sec_hand_length = analog_radius;
	
	if (enable_second) {
		s_analog_sec_hand_layer = layer_create(analog_grect);
		layer_set_update_proc(s_analog_sec_hand_layer, draw_analog_sec_hand_layer);
		layer_add_child(window_get_root_layer(window), s_analog_sec_hand_layer);	
	}
	
	s_analog_tick_layer = layer_create(analog_grect);
	layer_set_update_proc(s_analog_tick_layer, draw_tick_layer);
	layer_add_child(window_get_root_layer(window), s_analog_tick_layer);
	
	//APP_LOG(APP_LOG_LEVEL_DEBUG, "a x %d y %d w %d h %d", analog_grect.origin.x, analog_grect.origin.y, analog_grect.size.w, analog_grect.size.h);
	GRect s_no12_rect = shrink_grect(analog_grect, 0.8);
	//APP_LOG(APP_LOG_LEVEL_DEBUG, "b x %d y %d w %d h %d", s_no12_rect.origin.x, s_no12_rect.origin.y, s_no12_rect.size.w, s_no12_rect.size.h);
	for(int i=0; i<12; i+=1) {
		s_no12_grect[i] = grect_centered_from_polar(s_no12_rect, GOvalScaleModeFitCircle, DEG_TO_TRIGANGLE(i*30+30), GSize(24,24));
		s_no12_grect[i].origin.x += 1;
		s_no12_grect[i].origin.y += -4;
		s_no12_layer[i] = text_layer_create(s_no12_grect[i]);
		text_layer_set_font(s_no12_layer[i], fonts_get_system_font(FONT_KEY_GOTHIC_24_BOLD));
		text_layer_set_text_color(s_no12_layer[i], default_color);
		text_layer_set_background_color(s_no12_layer[i], GColorClear);
		//text_layer_set_background_color(s_no12_layer[i], GColorRed);

		text_layer_set_text_alignment(s_no12_layer[i], GTextAlignmentCenter);
		//text_layer_set_overflow_mode
		text_layer_set_text(s_no12_layer[i], s_no12buffer[i]);
		layer_add_child(window_get_root_layer(window), text_layer_get_layer(s_no12_layer[i]));
	}
	
}


void unload_analog(Window *window) {
	layer_destroy(s_analog_hour_hand_layer);
	layer_destroy(s_analog_min_hand_layer);
	layer_destroy(s_analog_sec_hand_layer);
	layer_destroy(s_analog_tick_layer);
	
	for (int i=0; i<12; i++) {
		text_layer_destroy(s_no12_layer[i]);
	}
}

