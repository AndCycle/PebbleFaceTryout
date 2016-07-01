#include <pebble.h>
#include "analog.h"
#include "extra.h"
#include "main.h"

static Layer *s_analog_root_layer;

static Layer *s_analog_hand_layer;
static Layer *s_analog_hour_hand_layer;
static Layer *s_analog_min_hand_layer;
static Layer *s_analog_sec_hand_layer;

static Layer *s_analog_cal_layer;
static Layer *s_analog_wday_cal_layer;
static Layer *s_analog_wday_text_cal_layer;
static Layer *s_analog_day_cal_layer;

static Layer *s_analog_text_cal_layer;
static Layer *s_analog_day_text_cal_layer;
static Layer *s_analog_month_cal_layer;
static Layer *s_analog_month_text_cal_layer;

static Layer *s_analog_tick_layer;

static struct tm *analog_tick_time;

typedef enum {
	HOUR, 
	MINUTE, 
	SECOND,
} ANALOG_HAND;

typedef enum {
	MONTH,
	DAY,
	WDAY
} ANALOG_DATE_CIRCLE;

typedef enum {
	MONTH_TEXT,
	DAY_TEXT,
	WDAY_TEXT
} ANALOG_DATE_TEXT;

typedef struct {
	GPoint center;
	ANALOG_HAND analog_hand;
	int32_t hand_length;
	int32_t analog_radius;
} analog_time_layer_data;

static int8_t daysinmonth;

typedef struct {
	GRect bounds;
	ANALOG_DATE_CIRCLE analog_date_circle;
	int16_t inset_thickness;
	int32_t circle_angle;
	int32_t angle_start;
} analog_date_circle_layer_data;

typedef struct {
	GRect bounds;
	ANALOG_DATE_TEXT analog_date_text;
	int32_t text_angle;
	GRect text_grect;
} analog_date_text_layer_data;


typedef struct {
	GPoint center;
	GPoint min_gpoint_a[60];
	GPoint min_gpoint_b[60];
	GRect text_grect[12];
} analog_tick_layer_data;

static void draw_analog_time_layer(Layer *layer, GContext *ctx) {
	
	analog_time_layer_data *data = layer_get_data(layer);
	
	GPoint center = data->center;
	
	int32_t analog_radius = data->analog_radius;
	int32_t hand_length = data->hand_length;
	
	int32_t hand_angle;
	
	switch (data->analog_hand) {
		case HOUR:
	
			hand_angle = (TRIG_MAX_ANGLE * (((analog_tick_time->tm_hour % 12) * 6) + (analog_tick_time->tm_min / 10))) / (12 * 6);
		
			graphics_context_set_stroke_color(ctx, default_color);

			graphics_context_set_stroke_width(ctx, 3);
			graphics_draw_line(ctx, center, gpoint_to_polar(center, hand_angle, hand_length));

			graphics_context_set_stroke_width(ctx, 7);
			graphics_draw_line(ctx, gpoint_to_polar(center, hand_angle, analog_radius*2/10), 
												 gpoint_to_polar(center, hand_angle, hand_length));

			graphics_context_set_stroke_width(ctx, 3);
			graphics_context_set_stroke_color(ctx, default_bg_color);
			graphics_draw_line(ctx, gpoint_to_polar(center, hand_angle, analog_radius*3/10), 
												 gpoint_to_polar(center, hand_angle, hand_length*8/10));
			APP_LOG(APP_LOG_LEVEL_DEBUG_VERBOSE, "%s", "Draw analog hour");
			break;
		
		case MINUTE:
		
			hand_angle = TRIG_MAX_ANGLE * analog_tick_time->tm_min / 60;

			graphics_context_set_stroke_color(ctx, default_color);

			graphics_context_set_stroke_width(ctx, 3);
			graphics_draw_line(ctx, center, gpoint_to_polar(center, hand_angle, hand_length));
			graphics_context_set_stroke_width(ctx, 5);
			graphics_draw_line(ctx, gpoint_to_polar(center, hand_angle, analog_radius*2/10), 
												 gpoint_to_polar(center, hand_angle, hand_length));

			APP_LOG(APP_LOG_LEVEL_DEBUG_VERBOSE, "%s", "Draw analog minute");
			break;
		
		case SECOND:
		
			hand_angle = TRIG_MAX_ANGLE * analog_tick_time->tm_sec / 60;

			graphics_context_set_stroke_color(ctx, default_color);
			graphics_draw_line(ctx, center, gpoint_to_polar(center, hand_angle, hand_length));
			APP_LOG(APP_LOG_LEVEL_DEBUG_VERBOSE, "%s", "Draw analog second");
			break;
	}
}

static void draw_analog_date_circle_layer(Layer *layer, GContext *ctx) {
	
	analog_date_circle_layer_data *data = layer_get_data(layer);

	GRect bounds = data->bounds;
	int16_t *inset_thickness = &data->inset_thickness;
	int32_t *circle_angle = &data->circle_angle;
	int32_t *angle_start = &data->angle_start;
	
	switch (data->analog_date_circle) {
		case MONTH:
			data->circle_angle = (TRIG_MAX_ANGLE*(analog_tick_time->tm_mon)+(TRIG_MAX_ANGLE*analog_tick_time->tm_mday/daysinmonth))/12;	
				
			APP_LOG(APP_LOG_LEVEL_DEBUG_VERBOSE, "%s", "Draw analog month");

			break;
		
		case DAY:
			data->circle_angle = (TRIG_MAX_ANGLE*(analog_tick_time->tm_mday)/daysinmonth);
			
			APP_LOG(APP_LOG_LEVEL_DEBUG_VERBOSE, "%s", "Draw analog day");

			break;
		
		case WDAY:
			data->circle_angle = (TRIG_MAX_ANGLE*(analog_tick_time->tm_wday+1)/7);

			/*
			graphics_context_set_stroke_color(ctx, default_color);
			for (int32_t i=TRIG_MAX_ANGLE/7; i < wday_angle; i+=(TRIG_MAX_ANGLE/7)) {

				graphics_draw_line(ctx, center, gpoint_to_polar(center, i, bounds.size.w/2));

			}
			*/
			APP_LOG(APP_LOG_LEVEL_DEBUG_VERBOSE, "%s", "Draw analog wday");		
			break;
		
	}
	
	graphics_context_set_fill_color(ctx, GColorLightGray);
	graphics_fill_radial(ctx, bounds, GOvalScaleModeFitCircle, *inset_thickness, *angle_start, *circle_angle);	
	
}

static void draw_analog_date_text_layer(Layer *layer, GContext *ctx) {
	
	analog_date_text_layer_data *data = layer_get_data(layer);
	GRect bounds = data->bounds;
	
	static char b_month[3];
	static char b_day[3];
	static const char *b_wday[] = {"Su", "Mo", "Tu", "We", "Th", "Fr", "Sa"};
	
	int32_t box_angle;
	GRect *text_grect = &data->text_grect;
		
	switch (data->analog_date_text) {
		case MONTH_TEXT:
		
			box_angle = (TRIG_MAX_ANGLE*(analog_tick_time->tm_mon))/12+TRIG_MAX_ANGLE/24;
			data->text_angle = box_angle;
			data->text_grect = grect_centered_from_polar(bounds, GOvalScaleModeFitCircle, box_angle, GSize(18,18));
			strftime(b_month, sizeof(char)*3, "%m", analog_tick_time);
			
			graphics_context_set_text_color(ctx, default_color);		
			graphics_context_set_fill_color(ctx, default_bg_color);
			graphics_context_set_stroke_color(ctx, GColorDarkGray);
		
			graphics_fill_rect(ctx, *text_grect, 1, GCornersAll);
			graphics_draw_round_rect(ctx, *text_grect, 1);
		
			graphics_draw_text(ctx, b_month, fonts_get_system_font(FONT_KEY_GOTHIC_14_BOLD), *text_grect, GTextOverflowModeWordWrap, GTextAlignmentCenter, NULL);
			APP_LOG(APP_LOG_LEVEL_DEBUG_VERBOSE, "%s", "Draw analog month text");
			break;
		
		case DAY_TEXT:
      
      box_angle = (analog_tick_time->tm_mday)*(TRIG_MAX_ANGLE/(daysinmonth+1));
  
      data->text_angle = box_angle;
  
      int32_t hour = (TRIG_MAX_ANGLE * (((analog_tick_time->tm_hour % 12) * 6) + (analog_tick_time->tm_min / 10))) / (12 * 6);
      int32_t min = TRIG_MAX_ANGLE * analog_tick_time->tm_min / 60;
      
    
      while (angle_check(box_angle, hour, TRIG_MAX_ANGLE/24) ||
             angle_check(box_angle, min, TRIG_MAX_ANGLE/24)) {
        if (analog_tick_time->tm_mday < 7) {
          box_angle += TRIG_MAX_ANGLE/24;
        } else {
          box_angle -= TRIG_MAX_ANGLE/24;
        }
      }
  
      data->text_grect = grect_centered_from_polar(bounds, GOvalScaleModeFitCircle, box_angle, GSize(24,24));
      strftime(b_day, sizeof(char)*3, "%d", analog_tick_time);
  		
			graphics_context_set_text_color(ctx, default_color);
			
			graphics_context_set_fill_color(ctx, default_bg_color);
			graphics_context_set_stroke_color(ctx, GColorDarkGray);
		
			graphics_fill_rect(ctx, grect_inset(*text_grect, GEdgeInsets(2)), 1, GCornersAll);
			graphics_draw_round_rect(ctx, grect_inset(*text_grect, GEdgeInsets(2)), 1);
			
			graphics_draw_text(ctx, b_day, fonts_get_system_font(FONT_KEY_GOTHIC_18_BOLD), *text_grect, GTextOverflowModeWordWrap, GTextAlignmentCenter, NULL);
			APP_LOG(APP_LOG_LEVEL_DEBUG_VERBOSE, "%s", "Draw analog day text");
			break;
		
		case WDAY_TEXT:
		
      box_angle = (TRIG_MAX_ANGLE*(analog_tick_time->tm_wday+1)/7)-TRIG_MAX_ANGLE/2/7;
      data->text_angle = box_angle;
      data->text_grect = grect_centered_from_polar(bounds, GOvalScaleModeFitCircle, box_angle, GSize(20,20));
  		
			graphics_context_set_text_color(ctx, default_color);
			graphics_context_set_fill_color(ctx, default_bg_color);
			graphics_fill_rect(ctx, *text_grect, 1, GCornersAll);
			graphics_context_set_stroke_color(ctx, GColorDarkGray);
			graphics_draw_round_rect(ctx, *text_grect, 1);
			graphics_draw_text(ctx, b_wday[analog_tick_time->tm_wday], fonts_get_system_font(FONT_KEY_GOTHIC_14_BOLD), *text_grect, GTextOverflowModeWordWrap, GTextAlignmentCenter, NULL);
			APP_LOG(APP_LOG_LEVEL_DEBUG_VERBOSE, "%s", "Draw analog wday text");
			break;

	}
	
}

static void draw_tick_layer(Layer *layer, GContext *ctx) {
	
	analog_tick_layer_data *data = layer_get_data(layer);

	graphics_context_set_stroke_color(ctx, default_color);
	
	// fill min tick
	
	graphics_context_set_stroke_width(ctx, 1);

	for(int32_t min=0; min<60; min++) {
		graphics_draw_line(ctx, data->min_gpoint_a[min], data->min_gpoint_b[min]);
	}
	
	// fill hour tick
	
	graphics_context_set_stroke_width(ctx, 3);

	for(int32_t hour=0; hour<12; hour++) {
		graphics_draw_line(ctx, data->min_gpoint_a[hour*5], data->min_gpoint_b[hour*5]);
	}
	

	// draw center circle
	
	graphics_context_set_stroke_color(ctx, default_color);
	graphics_context_set_fill_color(ctx, default_bg_color);
	graphics_context_set_stroke_width(ctx, 2);
	
	graphics_draw_circle(ctx, data->center, 3);
	graphics_fill_circle(ctx, data->center, 2);
	
	// draw hour number
	const char *s_no12buffer[12] = {"1", "2", "3", "4", "5", "6", "7", "8", "9", "10", "11", "12"};
	
	graphics_context_set_text_color(ctx, default_color);
	
	//APP_LOG(APP_LOG_LEVEL_DEBUG, "b x %d y %d w %d h %d", s_no12_rect.origin.x, s_no12_rect.origin.y, s_no12_rect.size.w, s_no12_rect.size.h);
	for(int i=0; i<12; i++) {
		graphics_draw_text(ctx, s_no12buffer[i], fonts_get_system_font(FONT_KEY_GOTHIC_24_BOLD), data->text_grect[i], GTextOverflowModeWordWrap, GTextAlignmentCenter, NULL);
	}
	APP_LOG(APP_LOG_LEVEL_DEBUG_VERBOSE, "%s", "Draw analog tick");
}

void analog_tick_handler(struct tm *tick_time, TimeUnits units_changed) {
	
	analog_tick_time = tick_time;

	if (enable_second && (units_changed & SECOND_UNIT)) {
		layer_mark_dirty(s_analog_sec_hand_layer);
		//APP_LOG(APP_LOG_LEVEL_DEBUG_VERBOSE, "%s", "Update analog second");
	}
	
	if (units_changed & MINUTE_UNIT) {
		
		layer_mark_dirty(s_analog_min_hand_layer);
		layer_mark_dirty(s_analog_hour_hand_layer);
		layer_mark_dirty(s_analog_day_text_cal_layer);
		
		APP_LOG(APP_LOG_LEVEL_DEBUG_VERBOSE, "%s", "Update analog minute");
	}
	
	if (units_changed & DAY_UNIT) {
		
		layer_mark_dirty(s_analog_month_cal_layer);
		layer_mark_dirty(s_analog_day_cal_layer);
		layer_mark_dirty(s_analog_wday_cal_layer);
		
		layer_mark_dirty(s_analog_month_text_cal_layer);
		layer_mark_dirty(s_analog_day_text_cal_layer);
		layer_mark_dirty(s_analog_wday_text_cal_layer);
		
		APP_LOG(APP_LOG_LEVEL_DEBUG_VERBOSE, "%s", "Update analog day");
	}
	
	if (units_changed & MONTH_UNIT) {
		daysinmonth = days_in_month(analog_tick_time);
	}
	
}

void refresh_analog_enable_second() {
	if (layer_get_hidden(s_analog_sec_hand_layer) != !enable_second) {
		layer_set_hidden(s_analog_sec_hand_layer, !enable_second);	
	}
	APP_LOG(APP_LOG_LEVEL_DEBUG_VERBOSE, "change analog second status");
}

static Layer * analog_date_circle_layer_create(GRect frame, ANALOG_DATE_CIRCLE analog_date_circle, int16_t inset_thickness, int32_t angle_start) {
	
	Layer *temp_layer;
	analog_date_circle_layer_data *temp_analog_date_circle_layer_data;
		
	//year
	temp_layer = layer_create_with_data(frame, sizeof(analog_date_circle_layer_data));
		
	temp_analog_date_circle_layer_data = layer_get_data(temp_layer);
	temp_analog_date_circle_layer_data->analog_date_circle = analog_date_circle;
	temp_analog_date_circle_layer_data->inset_thickness = inset_thickness;
	temp_analog_date_circle_layer_data->angle_start = angle_start;
	temp_analog_date_circle_layer_data->bounds = layer_get_bounds(temp_layer);
	
	layer_set_update_proc(temp_layer, draw_analog_date_circle_layer);
	
	return temp_layer;
}

static Layer * analog_date_text_layer_create(GRect frame, ANALOG_DATE_TEXT analog_date_text) {
	
	Layer *temp_layer;
	analog_date_text_layer_data *temp_analog_date_text_layer_data;
		
	temp_layer = layer_create_with_data(frame, sizeof(analog_date_text_layer_data));
	temp_analog_date_text_layer_data = layer_get_data(temp_layer);
	temp_analog_date_text_layer_data->bounds = grect_inset(layer_get_bounds(temp_layer), GEdgeInsets(10));
	temp_analog_date_text_layer_data->analog_date_text = analog_date_text;
	
	layer_set_update_proc(temp_layer, draw_analog_date_text_layer);
	
	return temp_layer;
}


static Layer * analog_time_layer_create(GRect frame, ANALOG_HAND analog_hand, int32_t analog_radius, int32_t hand_length) {
	
	Layer *temp_layer;
	analog_time_layer_data *temp_time_layer_data;
			
	temp_layer = layer_create_with_data(frame, sizeof(analog_time_layer_data));
	temp_time_layer_data = layer_get_data(temp_layer);
	temp_time_layer_data->analog_hand = analog_hand;
	temp_time_layer_data->center = ret_carry_center(layer_get_bounds(temp_layer), GOvalScaleModeFitCircle);
	temp_time_layer_data->analog_radius = analog_radius;
	temp_time_layer_data->hand_length = hand_length;
	
	layer_set_update_proc(temp_layer, draw_analog_time_layer);
	
	return temp_layer;
}


static Layer * tick_layer_create(GRect frame) {
	Layer * temp_layer;
	analog_tick_layer_data * temp_tick_layer_data;
	
	temp_layer = layer_create_with_data(frame, sizeof(analog_tick_layer_data));
	temp_tick_layer_data = layer_get_data(temp_layer);
	
	GOvalScaleMode default_govalscalemode = GOvalScaleModeFitCircle;
	
	GRect bounds = layer_get_bounds(temp_layer);
	int32_t tick_length = (bounds.size.w/2)*95/100;
	
	temp_tick_layer_data->center = ret_carry_center(bounds, default_govalscalemode);
	
	
	for(int32_t min=0, angle=0; min<60; min+=1, angle+=(TRIG_MAX_ANGLE/60)) {
		temp_tick_layer_data->min_gpoint_a[min] = gpoint_from_polar(bounds, default_govalscalemode, angle);
		temp_tick_layer_data->min_gpoint_b[min] = gpoint_to_polar(temp_tick_layer_data->center, angle, tick_length);
	}
		
	GRect no12_grect = grect_inset(bounds, GEdgeInsets(bounds.size.w/10));
	
	GRect temp_grect;
	for(int i=0; i<12; i+=1) {
		temp_grect = grect_centered_from_polar(no12_grect, GOvalScaleModeFitCircle, (i+1)*TRIG_MAX_ANGLE/12, GSize(24,24));
		temp_grect.origin.x += 0;
		temp_grect.origin.y += -5;
		temp_tick_layer_data->text_grect[i] = temp_grect;
	}
	
	layer_set_update_proc(temp_layer, draw_tick_layer);
	
	return temp_layer;
}

void load_analog(Window *window) {
	
	s_analog_root_layer = layer_create(GRect(0, 24, 144, 144));
	layer_add_child(window_get_root_layer(window), s_analog_root_layer);

	//
	
	GRect analog_grect = layer_get_bounds(s_analog_root_layer);
	
	int32_t analog_radius = analog_grect.size.w/2;
	
	time_t now = time(NULL);
  analog_tick_time = localtime(&now);

	daysinmonth = days_in_month(analog_tick_time);
	
	
	// m:d:wday
	
	s_analog_month_cal_layer = analog_date_circle_layer_create(grect_inset(analog_grect,  GEdgeInsets(analog_radius*0/3+analog_radius/48)), MONTH,	analog_radius*9/10/3, 0);
	s_analog_day_cal_layer = analog_date_circle_layer_create(grect_inset(analog_grect,  GEdgeInsets(analog_radius*1/3+analog_radius/48)), DAY, analog_radius*9/10/3, 0);
	s_analog_wday_cal_layer = analog_date_circle_layer_create(grect_inset(analog_grect,  GEdgeInsets(analog_radius*2/3+analog_radius/48)), WDAY,	analog_radius*9/10/3, 0);
	
  s_analog_cal_layer = layer_create(analog_grect);
  
	layer_add_child(s_analog_cal_layer, s_analog_month_cal_layer);
	layer_add_child(s_analog_cal_layer, s_analog_day_cal_layer);
	layer_add_child(s_analog_cal_layer, s_analog_wday_cal_layer);
	
	// mdwday text
	s_analog_month_text_cal_layer = analog_date_text_layer_create(grect_inset(analog_grect,  GEdgeInsets(analog_radius*0/3)), MONTH_TEXT);
	s_analog_day_text_cal_layer = analog_date_text_layer_create(grect_inset(analog_grect,  GEdgeInsets(analog_radius*1/3)), DAY_TEXT);
	s_analog_wday_text_cal_layer = analog_date_text_layer_create(grect_inset(analog_grect,  GEdgeInsets(analog_radius*2/3)), WDAY_TEXT);
	
  s_analog_text_cal_layer = layer_create(analog_grect);
  
	layer_add_child(s_analog_text_cal_layer, s_analog_month_text_cal_layer);
	layer_add_child(s_analog_text_cal_layer, s_analog_day_text_cal_layer);
	layer_add_child(s_analog_text_cal_layer, s_analog_wday_text_cal_layer);
	
	// h:m:s

	s_analog_hour_hand_layer = analog_time_layer_create(analog_grect, HOUR, analog_radius, analog_radius*6/10);
	s_analog_min_hand_layer = analog_time_layer_create(analog_grect, MINUTE, analog_radius, analog_radius*8/10);
	s_analog_sec_hand_layer = analog_time_layer_create(analog_grect, SECOND, analog_radius, analog_radius);
	
  s_analog_hand_layer = layer_create(analog_grect);
  
	layer_add_child(s_analog_hand_layer, s_analog_hour_hand_layer);
	layer_add_child(s_analog_hand_layer, s_analog_min_hand_layer);
	layer_add_child(s_analog_hand_layer, s_analog_sec_hand_layer);
	
	refresh_analog_enable_second();
		
	// tick
	
	s_analog_tick_layer = tick_layer_create(analog_grect);
  
  // composite
  
  layer_add_child(s_analog_root_layer, s_analog_cal_layer);
  layer_add_child(s_analog_root_layer, s_analog_text_cal_layer);
  layer_add_child(s_analog_root_layer, s_analog_hand_layer);
  
	layer_add_child(s_analog_root_layer, s_analog_tick_layer);
	
}


void unload_analog(Window *window) {
	layer_destroy(s_analog_hour_hand_layer);
	layer_destroy(s_analog_min_hand_layer);
	layer_destroy(s_analog_sec_hand_layer);
  layer_destroy(s_analog_hand_layer);
  
	layer_destroy(s_analog_month_cal_layer);
	layer_destroy(s_analog_day_cal_layer);
	layer_destroy(s_analog_wday_cal_layer);
  layer_destroy(s_analog_cal_layer);
	
	layer_destroy(s_analog_month_text_cal_layer);
	layer_destroy(s_analog_day_text_cal_layer);
	layer_destroy(s_analog_wday_text_cal_layer);
  layer_destroy(s_analog_text_cal_layer);
  
	layer_destroy(s_analog_tick_layer);
	
	layer_destroy(s_analog_root_layer);
}

