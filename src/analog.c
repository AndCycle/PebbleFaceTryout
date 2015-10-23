#include <pebble.h>
#include "analog.h"
#include "extra.h"
#include "main.h"

Layer *s_analog_hour_hand_layer;
Layer *s_analog_min_hand_layer;
Layer *s_analog_sec_hand_layer;
Layer *s_analog_tick_layer;

typedef enum {
	HOUR, 
	MINUTE, 
	SECOND
} hand;


typedef struct {
	GPoint center;
  hand my_hand;
	int32_t hand_length;
	int32_t analog_radius;
	struct tm *t;
} hand_layer_data;

void draw_analog_hand_layer(Layer *layer, GContext *ctx) {
	
	hand_layer_data *data = layer_get_data(layer);
	GPoint center = data->center;
  struct tm *t = data->t;

	if (data->my_hand == HOUR) {
		int32_t hour_angle = (TRIG_MAX_ANGLE * (((t->tm_hour % 12) * 6) + (t->tm_min / 10))) / (12 * 6);

		graphics_context_set_stroke_color(ctx, default_color);

		graphics_context_set_stroke_width(ctx, 3);
		graphics_draw_line(ctx, center, gpoint_to_polar(center, hour_angle, data->hand_length));

		graphics_context_set_stroke_width(ctx, 9);
		graphics_draw_line(ctx, gpoint_to_polar(center, hour_angle, data->analog_radius*2/10), 
											 gpoint_to_polar(center, hour_angle, data->hand_length));

		graphics_context_set_stroke_width(ctx, 5);
		graphics_context_set_stroke_color(ctx, default_bg_color);
		graphics_draw_line(ctx, gpoint_to_polar(center, hour_angle, data->analog_radius*3/10), 
											 gpoint_to_polar(center, hour_angle, data->hand_length*9/10));
		
	} else if (data->my_hand == MINUTE) {
		
		int32_t min_angle = TRIG_MAX_ANGLE * t->tm_min / 60;

		graphics_context_set_stroke_color(ctx, default_color);

		graphics_context_set_stroke_width(ctx, 3);
		graphics_draw_line(ctx, center, gpoint_to_polar(center, min_angle, data->hand_length));
		graphics_context_set_stroke_width(ctx, 5);
		graphics_draw_line(ctx, gpoint_to_polar(center, min_angle, data->analog_radius*2/10), 
											 gpoint_to_polar(center, min_angle, data->hand_length));
		
	} else if (data->my_hand == SECOND) {
		
		int32_t second_angle = TRIG_MAX_ANGLE * t->tm_sec / 60;
		graphics_context_set_stroke_color(ctx, default_color);
		graphics_draw_line(ctx, center, gpoint_to_polar(center, second_angle, data->hand_length));
		
	}
}
	
void draw_tick_layer(Layer *layer, GContext *ctx) {
	GRect bounds = layer_get_bounds(layer);
	GOvalScaleMode default_govalscalemode = GOvalScaleModeFitCircle;
	GPoint center = ret_carry_center(bounds, default_govalscalemode);
	
	int32_t radius = bounds.size.w/2;

	graphics_context_set_stroke_color(ctx, default_color);
	
	// fill min tick
	
	int32_t tick_length = radius*95/100;
	
	graphics_context_set_stroke_width(ctx, 1);

	for(int32_t angle=0; angle<TRIG_MAX_ANGLE; angle+=(TRIG_MAX_ANGLE/60)) {
		graphics_draw_line(ctx, gpoint_from_polar(bounds, default_govalscalemode, angle), gpoint_to_polar(center, angle, tick_length));
	}
	
	// fill hour tick
	
	graphics_context_set_stroke_width(ctx, 3);

	for(int32_t angle=0; angle<TRIG_MAX_ANGLE; angle+=(TRIG_MAX_ANGLE/12)) {
		graphics_draw_line(ctx, gpoint_from_polar(bounds, default_govalscalemode, angle), gpoint_to_polar(center, angle, tick_length));
	}

	// draw center circle
	
	graphics_context_set_stroke_color(ctx, default_color);
	graphics_context_set_fill_color(ctx, default_bg_color);
	graphics_context_set_stroke_width(ctx, 2);
	
	graphics_draw_circle(ctx, center, 3);
	graphics_fill_circle(ctx, center, 2);
	
	// draw hour number
	const char *s_no12buffer[12] = {"1", "2", "3", "4", "5", "6", "7", "8", "9", "10", "11", "12"};
	
	GRect no12_grect = grect_inset(bounds, GEdgeInsets(bounds.size.w/10));
	graphics_context_set_text_color(ctx, default_color);
	
	GRect temp_grect;
	//APP_LOG(APP_LOG_LEVEL_DEBUG, "b x %d y %d w %d h %d", s_no12_rect.origin.x, s_no12_rect.origin.y, s_no12_rect.size.w, s_no12_rect.size.h);
	for(int i=0; i<12; i+=1) {
		temp_grect = grect_centered_from_polar(no12_grect, GOvalScaleModeFitCircle, (i+1)*TRIG_MAX_ANGLE/12, GSize(24,24));
		temp_grect.origin.x += 0;
		temp_grect.origin.y += -5;
		graphics_draw_text(ctx, s_no12buffer[i], fonts_get_system_font(FONT_KEY_GOTHIC_24_BOLD), temp_grect, GTextOverflowModeWordWrap, GTextAlignmentCenter, NULL);
	}

}

void analog_tick_handler(struct tm *tick_time, TimeUnits units_changed) {
	hand_layer_data *temp_hand_layer_data;
	time_t now = time(NULL);
  struct tm *t = localtime(&now);
	if (enable_second && units_changed & SECOND_UNIT) {
		temp_hand_layer_data = layer_get_data(s_analog_sec_hand_layer);
		temp_hand_layer_data->t = t;
		layer_mark_dirty(s_analog_sec_hand_layer);
	}
	if (units_changed & MINUTE_UNIT) {
		temp_hand_layer_data = layer_get_data(s_analog_min_hand_layer);
		temp_hand_layer_data->t = t;
		temp_hand_layer_data = layer_get_data(s_analog_hour_hand_layer);
		temp_hand_layer_data->t = t;
		layer_mark_dirty(s_analog_min_hand_layer);
		layer_mark_dirty(s_analog_hour_hand_layer);
	}
}


void load_analog(Window *window) {
	
	GRect analog_grect = squre_grect(layer_get_frame(window_get_root_layer(window)));

	analog_grect.origin.y = 0;
	
	int32_t analog_radius = analog_grect.size.w/2;
	
	GPoint center = ret_carry_center(analog_grect, GOvalScaleModeFitCircle);
	
	hand_layer_data *temp_hand_layer_data;
	time_t now = time(NULL);
  struct tm *t = localtime(&now);
	
	s_analog_hour_hand_layer = layer_create_with_data(analog_grect, sizeof(hand_layer_data));
	temp_hand_layer_data = layer_get_data(s_analog_hour_hand_layer);
	temp_hand_layer_data->my_hand = HOUR;
	temp_hand_layer_data->center = center;
	temp_hand_layer_data->analog_radius = analog_radius;
	temp_hand_layer_data->hand_length = analog_radius*6/10;
	temp_hand_layer_data->t = t;
	
	layer_set_update_proc(s_analog_hour_hand_layer, draw_analog_hand_layer);
	layer_add_child(window_get_root_layer(window), s_analog_hour_hand_layer);

	s_analog_min_hand_layer = layer_create_with_data(analog_grect, sizeof(hand_layer_data));
	temp_hand_layer_data = layer_get_data(s_analog_min_hand_layer);
	temp_hand_layer_data->my_hand = MINUTE;
	temp_hand_layer_data->center = center;
	temp_hand_layer_data->analog_radius = analog_radius;
	temp_hand_layer_data->hand_length = analog_radius*8/10;
	temp_hand_layer_data->t = t;
	
	layer_set_update_proc(s_analog_min_hand_layer, draw_analog_hand_layer);
	layer_add_child(window_get_root_layer(window), s_analog_min_hand_layer);
	
	if (enable_second) {
		s_analog_sec_hand_layer = layer_create_with_data(analog_grect, sizeof(hand_layer_data));
		temp_hand_layer_data = layer_get_data(s_analog_sec_hand_layer);
		temp_hand_layer_data->my_hand = SECOND;
		temp_hand_layer_data->center = center;
		temp_hand_layer_data->analog_radius = analog_radius;
		temp_hand_layer_data->hand_length = analog_radius;
		temp_hand_layer_data->t = t;
		 
		layer_set_update_proc(s_analog_sec_hand_layer, draw_analog_hand_layer);
		layer_add_child(window_get_root_layer(window), s_analog_sec_hand_layer);	
	}
	
	s_analog_tick_layer = layer_create(analog_grect);
	layer_set_update_proc(s_analog_tick_layer, draw_tick_layer);
	layer_add_child(window_get_root_layer(window), s_analog_tick_layer);
}


void unload_analog(Window *window) {
	layer_destroy(s_analog_hour_hand_layer);
	layer_destroy(s_analog_min_hand_layer);
	layer_destroy(s_analog_sec_hand_layer);
	layer_destroy(s_analog_tick_layer);
}

