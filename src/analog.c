#include <pebble.h>
#include "analog.h"
#include "extra.h"
#include "main.h"

Layer *s_analog_hour_hand_layer;
Layer *s_analog_min_hand_layer;
Layer *s_analog_sec_hand_layer;
Layer *s_analog_tick_layer;

Layer *s_analog_wday_cal_layer;
Layer *s_analog_wday_text_cal_layer;
Layer *s_analog_day_cal_layer;
Layer *s_analog_day_text_cal_layer;
Layer *s_analog_month_cal_layer;
Layer *s_analog_month_text_cal_layer;


typedef enum {
	HOUR, 
	MINUTE, 
	SECOND,
	MONTH,
	MONTH_TEXT,
	DAY,
	DAY_TEXT,
	WDAY,
	WDAY_TEXT
} hand;


typedef struct {
	GRect bounds;
	GPoint center;
  hand my_hand;
	int32_t hand_length;
	int32_t analog_radius;
	struct tm *t;
} hand_layer_data;

void draw_analog_hand_layer(Layer *layer, GContext *ctx) {
	
	hand_layer_data *data = layer_get_data(layer);
	GRect bounds = data->bounds;
	GPoint center = data->center;
	int32_t analog_radius = data->analog_radius;
	int32_t hand_length = data->hand_length;
	
	static const int32_t one_degree_angle = TRIG_MAX_ANGLE/360;
	
  struct tm *t = data->t;

	if (data->my_hand == MONTH || data->my_hand == MONTH_TEXT) {
		static char b_month[3];

		
		
		if (data->my_hand == MONTH) {
			struct tm c_tm = *t;
			c_tm.tm_mon += 1;
			c_tm.tm_mday = 0;
			mktime(&c_tm);
			int32_t month_angle = (TRIG_MAX_ANGLE*(t->tm_mon+1)+(TRIG_MAX_ANGLE*t->tm_mday/c_tm.tm_mday))/12;
			graphics_context_set_fill_color(ctx, GColorLightGray);
			graphics_fill_radial(ctx, bounds, GOvalScaleModeFitCircle, hand_length, TRIG_MAX_ANGLE/12, month_angle);	
			APP_LOG(APP_LOG_LEVEL_DEBUG_VERBOSE, "%s", "Draw analog month");
		} else {
			int32_t month_angle = (TRIG_MAX_ANGLE*(t->tm_mon+1)+TRIG_MAX_ANGLE/2)/12;
			graphics_context_set_text_color(ctx, default_color);
			GRect temp_grect = grect_centered_from_polar(grect_inset(bounds, GEdgeInsets(hand_length/2)), GOvalScaleModeFitCircle, month_angle, GSize(18,18));
			graphics_context_set_fill_color(ctx, default_bg_color);
			graphics_context_set_stroke_color(ctx, GColorDarkGray);
			graphics_fill_rect(ctx, temp_grect, 1, GCornersAll);
			graphics_draw_round_rect(ctx, temp_grect, 1);
			strftime(b_month, sizeof(char)*3, "%m", t);
			graphics_draw_text(ctx, b_month, fonts_get_system_font(FONT_KEY_GOTHIC_14_BOLD), temp_grect, GTextOverflowModeWordWrap, GTextAlignmentCenter, NULL);
			APP_LOG(APP_LOG_LEVEL_DEBUG_VERBOSE, "%s", "Draw analog month text");
		}
		
	}
	
	if (data->my_hand == DAY || data->my_hand == DAY_TEXT) {
		static char b_day[3];
		struct tm c_tm = *t;
		c_tm.tm_mon += 1;
		c_tm.tm_mday = 0;
		mktime(&c_tm);
		int32_t day_angle = (TRIG_MAX_ANGLE*(t->tm_mday)/c_tm.tm_mday);
		
		if (data->my_hand == DAY) {
			graphics_context_set_fill_color(ctx, GColorLightGray);
			graphics_fill_radial(ctx, bounds, GOvalScaleModeFitCircle, hand_length, 0, day_angle);
			APP_LOG(APP_LOG_LEVEL_DEBUG_VERBOSE, "%s", "Draw analog day");
		} else {
			
			day_angle -= (TRIG_MAX_ANGLE/c_tm.tm_mday); // center day 1
			
			int32_t hour_angle = (TRIG_MAX_ANGLE * (((t->tm_hour % 12) * 6) + (t->tm_min / 10))) / (12 * 6);
			int32_t min_angle = TRIG_MAX_ANGLE * t->tm_min / 60;
			int32_t half_12 = TRIG_MAX_ANGLE/24;
			
			while (angle_check(day_angle, hour_angle, half_12) ||
						 angle_check(day_angle, min_angle, half_12)
						) {
				if (t->tm_mday < 7) {
					day_angle += half_12;
				} else {
					day_angle -= half_12;
				}
			}
			
			graphics_context_set_text_color(ctx, default_color);
			GRect temp_grect = grect_centered_from_polar(grect_inset(bounds, GEdgeInsets(hand_length/2)), GOvalScaleModeFitCircle, day_angle, GSize(24,24));
			graphics_context_set_fill_color(ctx, default_bg_color);
			graphics_context_set_stroke_color(ctx, GColorDarkGray);
			graphics_fill_rect(ctx, grect_inset(temp_grect, GEdgeInsets(2)), 1, GCornersAll);
			graphics_draw_round_rect(ctx, grect_inset(temp_grect, GEdgeInsets(2)), 1);
			strftime(b_day, sizeof(char)*3, "%d", t);
			graphics_draw_text(ctx, b_day, fonts_get_system_font(FONT_KEY_GOTHIC_18_BOLD), temp_grect, GTextOverflowModeWordWrap, GTextAlignmentCenter, NULL);
			APP_LOG(APP_LOG_LEVEL_DEBUG_VERBOSE, "%s", "Draw analog day text");
		}
	}
	
	if (data->my_hand == WDAY || data->my_hand == WDAY_TEXT) {
		static const char *b_wday[] = {"Su", "Mo", "Tu", "We", "Th", "Fr", "Sa"};
		
		int32_t wday_angle = (TRIG_MAX_ANGLE*(t->tm_wday+1)/7);
		
		if (data->my_hand == WDAY) {
			graphics_context_set_fill_color(ctx, GColorLightGray);
			graphics_fill_radial(ctx, bounds, GOvalScaleModeFitCircle, hand_length, 0, wday_angle);

			/*
			graphics_context_set_stroke_color(ctx, default_color);
			for (int32_t i=TRIG_MAX_ANGLE/7; i < wday_angle; i+=(TRIG_MAX_ANGLE/7)) {

				graphics_draw_line(ctx, center, gpoint_to_polar(center, i, bounds.size.w/2));

			}
			*/
			APP_LOG(APP_LOG_LEVEL_DEBUG_VERBOSE, "%s", "Draw analog wday");
		} else {
			graphics_context_set_text_color(ctx, default_color);
			GRect temp_grect = grect_centered_from_polar(grect_inset(bounds, GEdgeInsets(hand_length/2)), GOvalScaleModeFitCircle, wday_angle-TRIG_MAX_ANGLE/2/7, GSize(20,20));
			graphics_context_set_fill_color(ctx, default_bg_color);
			graphics_context_set_stroke_color(ctx, GColorDarkGray);
			graphics_fill_rect(ctx, temp_grect, 1, GCornersAll);
			graphics_draw_round_rect(ctx, temp_grect, 1);
			graphics_draw_text(ctx, b_wday[t->tm_wday], fonts_get_system_font(FONT_KEY_GOTHIC_14_BOLD), temp_grect, GTextOverflowModeWordWrap, GTextAlignmentCenter, NULL);
			APP_LOG(APP_LOG_LEVEL_DEBUG_VERBOSE, "%s", "Draw analog wday text");
		}
	}
	
	if (data->my_hand == HOUR) {
		int32_t hour_angle = (TRIG_MAX_ANGLE * (((t->tm_hour % 12) * 6) + (t->tm_min / 10))) / (12 * 6);

		graphics_context_set_stroke_color(ctx, default_color);

		graphics_context_set_stroke_width(ctx, 3);
		graphics_draw_line(ctx, center, gpoint_to_polar(center, hour_angle, hand_length));

		graphics_context_set_stroke_width(ctx, 7);
		graphics_draw_line(ctx, gpoint_to_polar(center, hour_angle, analog_radius*2/10), 
											 gpoint_to_polar(center, hour_angle, hand_length));

		graphics_context_set_stroke_width(ctx, 3);
		graphics_context_set_stroke_color(ctx, default_bg_color);
		graphics_draw_line(ctx, gpoint_to_polar(center, hour_angle, analog_radius*3/10), 
											 gpoint_to_polar(center, hour_angle, hand_length*8/10));
		APP_LOG(APP_LOG_LEVEL_DEBUG_VERBOSE, "%s", "Draw analog hour");
		
	} else if (data->my_hand == MINUTE) {
		
		int32_t min_angle = TRIG_MAX_ANGLE * t->tm_min / 60;

		graphics_context_set_stroke_color(ctx, default_color);

		graphics_context_set_stroke_width(ctx, 3);
		graphics_draw_line(ctx, center, gpoint_to_polar(center, min_angle, hand_length));
		graphics_context_set_stroke_width(ctx, 5);
		graphics_draw_line(ctx, gpoint_to_polar(center, min_angle, analog_radius*2/10), 
											 gpoint_to_polar(center, min_angle, hand_length));
		
		APP_LOG(APP_LOG_LEVEL_DEBUG_VERBOSE, "%s", "Draw analog minute");
	} else if (data->my_hand == SECOND) {
		
		int32_t second_angle = TRIG_MAX_ANGLE * t->tm_sec / 60;
		graphics_context_set_stroke_color(ctx, default_color);
		graphics_draw_line(ctx, center, gpoint_to_polar(center, second_angle, hand_length));
		
		APP_LOG(APP_LOG_LEVEL_DEBUG_VERBOSE, "%s", "Draw analog second");
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
	APP_LOG(APP_LOG_LEVEL_DEBUG_VERBOSE, "%s", "Draw analog tick");
}

void analog_tick_handler(struct tm *tick_time, TimeUnits units_changed) {
	hand_layer_data *temp_hand_layer_data;

	if (enable_second && units_changed & SECOND_UNIT) {
		if (layer_get_hidden(s_analog_sec_hand_layer)) {
			layer_set_hidden(s_analog_sec_hand_layer, !enable_second);
		}
		
		temp_hand_layer_data = layer_get_data(s_analog_sec_hand_layer);
		temp_hand_layer_data->t = tick_time;
		layer_mark_dirty(s_analog_sec_hand_layer);
		//APP_LOG(APP_LOG_LEVEL_DEBUG_VERBOSE, "%s", "Update analog second");
	}
	
	if (units_changed & MINUTE_UNIT) {
		if (!layer_get_hidden(s_analog_sec_hand_layer)) {
			layer_set_hidden(s_analog_sec_hand_layer, !enable_second);
		}
		
		
		temp_hand_layer_data = layer_get_data(s_analog_min_hand_layer);
		temp_hand_layer_data->t = tick_time;
		temp_hand_layer_data = layer_get_data(s_analog_hour_hand_layer);
		temp_hand_layer_data->t = tick_time;
		layer_mark_dirty(s_analog_min_hand_layer);
		layer_mark_dirty(s_analog_hour_hand_layer);
		layer_mark_dirty(s_analog_day_text_cal_layer);	

		/*
		struct tm c_tm = *tick_time;
		c_tm.tm_mon += 1;
		c_tm.tm_mday = 0;
		mktime(&c_tm);
		int32_t day_angle = (TRIG_MAX_ANGLE*(tick_time->tm_mday+1)/c_tm.tm_mday);
		int32_t hour_angle = (TRIG_MAX_ANGLE * (((tick_time->tm_hour % 12) * 6) + (tick_time->tm_min / 10))) / (12 * 6);
		int32_t min_angle = TRIG_MAX_ANGLE * tick_time->tm_min / 60;
		int32_t half_12 = TRIG_MAX_ANGLE/24;
		if (angle_check(day_angle, hour_angle, half_12*3) || angle_check(day_angle, min_angle, half_12*3)) {
			layer_mark_dirty(s_analog_day_text_cal_layer);	
		}
		*/
		
		APP_LOG(APP_LOG_LEVEL_DEBUG_VERBOSE, "%s", "Update analog minute");
	}
	
	if (units_changed & DAY_UNIT) {
		temp_hand_layer_data = layer_get_data(s_analog_wday_cal_layer);
		temp_hand_layer_data->t = tick_time;
		temp_hand_layer_data = layer_get_data(s_analog_wday_text_cal_layer);
		temp_hand_layer_data->t = tick_time;
		temp_hand_layer_data = layer_get_data(s_analog_day_cal_layer);
		temp_hand_layer_data->t = tick_time;
		temp_hand_layer_data = layer_get_data(s_analog_day_text_cal_layer);
		temp_hand_layer_data->t = tick_time;
		temp_hand_layer_data = layer_get_data(s_analog_month_cal_layer);
		temp_hand_layer_data->t = tick_time;
		temp_hand_layer_data = layer_get_data(s_analog_month_text_cal_layer);
		temp_hand_layer_data->t = tick_time;

		layer_mark_dirty(s_analog_wday_cal_layer);
		layer_mark_dirty(s_analog_wday_text_cal_layer);
		layer_mark_dirty(s_analog_day_cal_layer);
		layer_mark_dirty(s_analog_day_text_cal_layer);
		layer_mark_dirty(s_analog_month_cal_layer);
		layer_mark_dirty(s_analog_month_text_cal_layer);
		
		APP_LOG(APP_LOG_LEVEL_DEBUG_VERBOSE, "%s", "Update analog day");
	}
}

void refresh_analog_enable_second() {
	layer_set_hidden(s_analog_sec_hand_layer, !enable_second);
	APP_LOG(APP_LOG_LEVEL_DEBUG_VERBOSE, "change analog second status");
}


void load_analog(Window *window) {
	
	GRect analog_grect = squre_grect(layer_get_frame(window_get_root_layer(window)));

	int32_t analog_radius = analog_grect.size.w/2;
	
	GPoint center = ret_carry_center(analog_grect, GOvalScaleModeFitCircle);
	
	analog_grect.origin.y = 24;
	
	Layer *temp_layer;
	hand_layer_data *temp_hand_layer_data;
	time_t now = time(NULL);
  struct tm *t = localtime(&now);
	
	// y:m:wday
	
	s_analog_month_cal_layer = layer_create_with_data(analog_grect, sizeof(hand_layer_data));
	temp_layer = s_analog_month_cal_layer;
	temp_hand_layer_data = layer_get_data(temp_layer);
	temp_hand_layer_data->bounds = layer_get_bounds(temp_layer);
	temp_hand_layer_data->my_hand = MONTH;
	temp_hand_layer_data->center = center;
	temp_hand_layer_data->analog_radius = analog_radius;
	temp_hand_layer_data->hand_length = analog_radius*9/10/3;
	temp_hand_layer_data->t = t;
	
	layer_set_update_proc(temp_layer, draw_analog_hand_layer);
	layer_add_child(window_get_root_layer(window), temp_layer);
	
	s_analog_day_cal_layer = layer_create_with_data(analog_grect, sizeof(hand_layer_data));
	temp_layer = s_analog_day_cal_layer;
	temp_hand_layer_data = layer_get_data(temp_layer);
	temp_hand_layer_data->bounds = grect_inset(layer_get_bounds(temp_layer), GEdgeInsets(analog_radius/3));
	temp_hand_layer_data->my_hand = DAY;
	temp_hand_layer_data->center = center;
	temp_hand_layer_data->analog_radius = analog_radius;
	temp_hand_layer_data->hand_length = analog_radius*9/10/3;
	temp_hand_layer_data->t = t;
	
	layer_set_update_proc(temp_layer, draw_analog_hand_layer);
	layer_add_child(window_get_root_layer(window), temp_layer);
	
	s_analog_wday_cal_layer = layer_create_with_data(analog_grect, sizeof(hand_layer_data));
	temp_layer = s_analog_wday_cal_layer;
	temp_hand_layer_data = layer_get_data(temp_layer);
	temp_hand_layer_data->bounds = grect_inset(layer_get_bounds(temp_layer), GEdgeInsets(analog_radius*2/3));
	temp_hand_layer_data->my_hand = WDAY;
	temp_hand_layer_data->center = center;
	temp_hand_layer_data->analog_radius = analog_radius;
	temp_hand_layer_data->hand_length = analog_radius*9/10/3;
	temp_hand_layer_data->t = t;
	
	layer_set_update_proc(temp_layer, draw_analog_hand_layer);
	layer_add_child(window_get_root_layer(window), temp_layer);

	// ymw day text
	s_analog_month_text_cal_layer = layer_create_with_data(analog_grect, sizeof(hand_layer_data));
	temp_layer = s_analog_month_text_cal_layer;
	temp_hand_layer_data = layer_get_data(temp_layer);
	temp_hand_layer_data->bounds = layer_get_bounds(temp_layer);
	temp_hand_layer_data->my_hand = MONTH_TEXT;
	temp_hand_layer_data->center = center;
	temp_hand_layer_data->analog_radius = analog_radius;
	temp_hand_layer_data->hand_length = analog_radius*9/10/3;
	temp_hand_layer_data->t = t;
	
	layer_set_update_proc(temp_layer, draw_analog_hand_layer);
	layer_add_child(window_get_root_layer(window), temp_layer);
	
	s_analog_day_text_cal_layer = layer_create_with_data(analog_grect, sizeof(hand_layer_data));
	temp_layer = s_analog_day_text_cal_layer;
	temp_hand_layer_data = layer_get_data(temp_layer);
	temp_hand_layer_data->bounds = grect_inset(layer_get_bounds(temp_layer), GEdgeInsets(analog_radius/3));
	temp_hand_layer_data->my_hand = DAY_TEXT;
	temp_hand_layer_data->center = center;
	temp_hand_layer_data->analog_radius = analog_radius;
	temp_hand_layer_data->hand_length = analog_radius*9/10/3;
	temp_hand_layer_data->t = t;
	
	layer_set_update_proc(temp_layer, draw_analog_hand_layer);
	layer_add_child(window_get_root_layer(window), temp_layer);
	
	s_analog_wday_text_cal_layer = layer_create_with_data(analog_grect, sizeof(hand_layer_data));
	temp_layer = s_analog_wday_text_cal_layer;
	temp_hand_layer_data = layer_get_data(temp_layer);
	temp_hand_layer_data->bounds = grect_inset(layer_get_bounds(temp_layer), GEdgeInsets(analog_radius*2/3));
	temp_hand_layer_data->my_hand = WDAY_TEXT;
	temp_hand_layer_data->center = center;
	temp_hand_layer_data->analog_radius = analog_radius;
	temp_hand_layer_data->hand_length = analog_radius*9/10/3;
	temp_hand_layer_data->t = t;
	
	layer_set_update_proc(temp_layer, draw_analog_hand_layer);
	layer_add_child(window_get_root_layer(window), temp_layer);

	// h:m:s
	
	s_analog_hour_hand_layer = layer_create_with_data(analog_grect, sizeof(hand_layer_data));
	temp_layer = s_analog_hour_hand_layer;
	temp_hand_layer_data = layer_get_data(temp_layer);
	temp_hand_layer_data->bounds = layer_get_bounds(temp_layer);
	temp_hand_layer_data->my_hand = HOUR;
	temp_hand_layer_data->center = center;
	temp_hand_layer_data->analog_radius = analog_radius;
	temp_hand_layer_data->hand_length = analog_radius*6/10;
	temp_hand_layer_data->t = t;
	
	layer_set_update_proc(temp_layer, draw_analog_hand_layer);
	layer_add_child(window_get_root_layer(window), temp_layer);

	s_analog_min_hand_layer = layer_create_with_data(analog_grect, sizeof(hand_layer_data));
	temp_layer = s_analog_min_hand_layer;
	temp_hand_layer_data = layer_get_data(temp_layer);
	temp_hand_layer_data->bounds = layer_get_bounds(temp_layer);
	temp_hand_layer_data->my_hand = MINUTE;
	temp_hand_layer_data->center = center;
	temp_hand_layer_data->analog_radius = analog_radius;
	temp_hand_layer_data->hand_length = analog_radius*8/10;
	temp_hand_layer_data->t = t;
	
	layer_set_update_proc(temp_layer, draw_analog_hand_layer);
	layer_add_child(window_get_root_layer(window), temp_layer);
	
	
	s_analog_sec_hand_layer = layer_create_with_data(analog_grect, sizeof(hand_layer_data));
	temp_layer = s_analog_sec_hand_layer;
	temp_hand_layer_data = layer_get_data(temp_layer);
	temp_hand_layer_data->bounds = layer_get_bounds(temp_layer);
	temp_hand_layer_data->my_hand = SECOND;
	temp_hand_layer_data->center = center;
	temp_hand_layer_data->analog_radius = analog_radius;
	temp_hand_layer_data->hand_length = analog_radius;
	temp_hand_layer_data->t = t;
	
	layer_set_update_proc(temp_layer, draw_analog_hand_layer);
	layer_add_child(window_get_root_layer(window), temp_layer);	
	
	layer_set_hidden(s_analog_sec_hand_layer, !enable_second);
	
	refresh_analog_enable_second();
		
	// tick
	
	s_analog_tick_layer = layer_create(analog_grect);
	layer_set_update_proc(s_analog_tick_layer, draw_tick_layer);
	layer_add_child(window_get_root_layer(window), s_analog_tick_layer);
	
}


void unload_analog(Window *window) {
	layer_destroy(s_analog_hour_hand_layer);
	layer_destroy(s_analog_min_hand_layer);
	layer_destroy(s_analog_sec_hand_layer);
	layer_destroy(s_analog_tick_layer);
	
	layer_destroy(s_analog_wday_cal_layer);
	layer_destroy(s_analog_wday_text_cal_layer);
	layer_destroy(s_analog_day_cal_layer);
	layer_destroy(s_analog_day_text_cal_layer);
	layer_destroy(s_analog_month_cal_layer);
	layer_destroy(s_analog_month_text_cal_layer);

}

