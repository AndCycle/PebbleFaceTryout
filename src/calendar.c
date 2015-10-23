#include <pebble.h>
#include "calendar.h"
#include "main.h"

Layer *s_weekday_base_layer;

TextLayer *s_year_layer;
TextLayer *s_month_layer;
TextLayer *s_2weeks_layer[24];

char *b_year;
char *b_month;	
char *b_2weeks[14];
	
const int16_t font_pt = 16;
const int16_t font_height = 11;
const int16_t font_width = 6;

const int16_t font_height_bias = -4;

void draw_weekday_base_layer(Layer *layer, GContext *ctx) {
	GRect bounds = layer_get_bounds(layer);
	//graphics_draw_round_rect(ctx, bounds, 1);
	int16_t width_end = bounds.size.w-1;
	int16_t height_end = font_height*2+1;

	graphics_context_set_stroke_color(ctx, default_color);
	
	//graphics_draw_round_rect(ctx, bounds, 1);
	
	//graphics_draw_line(ctx, GPoint(0, 0), GPoint(0, height_end));
	graphics_draw_line(ctx, GPoint(0, font_height), GPoint(width_end, font_height));
	//graphics_draw_line(ctx, GPoint(0, font_height*2+1), GPoint(width_end, font_height*2+1));
	//graphics_draw_line(ctx, GPoint(font_width*4, 0), GPoint(font_width*4, height_end));
	for (int i=0; i<7; i++) {
		int x = font_width*4+(font_width*2+1)*i;
		/*
		if (i == 0 || i == 6) {
			graphics_context_set_fill_color(ctx, default_color);
		} else {
			graphics_context_set_fill_color(ctx, default_bg_color);
		}
		*/
		graphics_context_set_fill_color(ctx, default_color);
		/*
		graphics_fill_rect(ctx, GRect(x+1, 0, font_width*2, font_height), 0, GCornerNone);
		graphics_fill_rect(ctx, GRect(x+1, font_height+1, font_width*2, font_height), 0, GCornerNone);
		if (i != 0 && i != 1 && i != 6) {
			graphics_draw_line(ctx, GPoint(x, 0), GPoint(x, height_end-1));	
		}
		*/
		graphics_draw_line(ctx, GPoint(x, 0), GPoint(x, height_end-1));	
	}
	
}


void update_calendar_year(struct tm *tick_time) {
	strftime(b_year, 5*sizeof(char), "%Y", tick_time);
	text_layer_set_text(s_year_layer, b_year);
}

void update_calendar_month(struct tm *tick_time) {
	strftime(b_month, 3*sizeof(char), "%m", tick_time);
	text_layer_set_text(s_month_layer, b_month);	
}

void update_calendar_day(struct tm *tick_time) {

	time_t t_unix_time = time(NULL); //DO NOT USE mktime, it alter tm struct and shift time.
	
	struct tm *c_tm = localtime(&t_unix_time);
	int c_mday = tick_time->tm_mday;
	
	if (c_tm->tm_wday < 2) {
		t_unix_time -= (7*86400);
	}
	t_unix_time -= (c_tm->tm_wday*86400);
		
	for (int i=0; i<14; i++, t_unix_time += 86400) {
		c_tm = localtime(&t_unix_time);

		if (c_tm->tm_mday == c_mday) {
			text_layer_set_font(s_2weeks_layer[i], fonts_get_system_font(FONT_KEY_GOTHIC_14_BOLD));	
		} else {
			text_layer_set_font(s_2weeks_layer[i], fonts_get_system_font(FONT_KEY_GOTHIC_14));	
		}
		if (c_tm->tm_wday == 0 || c_tm->tm_wday == 6) {
			text_layer_set_text_color(s_2weeks_layer[i], GColorRed);
			//text_layer_set_text_color(s_2weeks_layer[i], default_bg_color);
			//text_layer_set_background_color(s_2weeks_layer[i], GColorBlack);
		} else {
			text_layer_set_text_color(s_2weeks_layer[i], default_color);
		}
		
		strftime(b_2weeks[i], sizeof(3*sizeof(char)), "%d", c_tm);
		text_layer_set_text(s_2weeks_layer[i], b_2weeks[i]);
	}

	t_unix_time = time(NULL);
	localtime(&t_unix_time); //sanitize tm struct as localtime point to
}

void init_calendar() {
	b_year = calloc(5, sizeof(char));
	b_month = calloc(3, sizeof(char));	
	for (int i=0; i<14; i++) {
		b_2weeks[i] = calloc(3, sizeof(char));
	}
}

void deinit_calendar() {
	free(b_year);
	free(b_month);
	for (int i=0; i<14; i++) {
		free(b_2weeks[i]);
	}
}

void load_calendar(Window *window) {

	int16_t offset_x = 0;
	int16_t offset_y = 145;
	
	int16_t text_col1_y = offset_y+font_height_bias;
	int16_t text_col2_y = offset_y+font_height_bias+font_height+1;	
	
	GRect weekday_base_rect = GRect(offset_x, offset_y, font_width*4+1+(font_width*2+1)*7, (font_height+1)*2);
	s_weekday_base_layer = layer_create(weekday_base_rect);
	layer_set_update_proc(s_weekday_base_layer, draw_weekday_base_layer);
	layer_add_child(window_get_root_layer(window), s_weekday_base_layer);
	
	s_year_layer = text_layer_create(GRect(offset_x, text_col1_y, font_width*4, font_pt));
	
	
	text_layer_set_font(s_year_layer, fonts_get_system_font(FONT_KEY_GOTHIC_14_BOLD));
	text_layer_set_text_alignment(s_year_layer, GTextAlignmentCenter);
	//text_layer_set_overflow_mode(s_year_layer, GTextOverflowModeFill);
	text_layer_set_text_color(s_year_layer, default_color);
	text_layer_set_background_color(s_year_layer, GColorClear);
	layer_add_child(window_get_root_layer(window), text_layer_get_layer(s_year_layer));
	
	s_month_layer = text_layer_create(GRect(offset_x, text_col2_y, font_width*4, font_pt));
	text_layer_set_font(s_month_layer, fonts_get_system_font(FONT_KEY_GOTHIC_14_BOLD));
	text_layer_set_text_alignment(s_month_layer, GTextAlignmentCenter);
	text_layer_set_text_color(s_month_layer, default_color);
	text_layer_set_background_color(s_month_layer, GColorClear);
	layer_add_child(window_get_root_layer(window), text_layer_get_layer(s_month_layer));
	

	
	for (int i=7; i<14; i++) {
		s_2weeks_layer[i] = text_layer_create(GRect(offset_x+(font_width*4+1)+(font_width*2+1)*(i-7), text_col2_y, font_width*2, font_pt));
		text_layer_set_background_color(s_2weeks_layer[i], GColorClear);
		layer_add_child(window_get_root_layer(window), text_layer_get_layer(s_2weeks_layer[i]));
	}

	for (int i=0; i<7; i++) {
		s_2weeks_layer[i] = text_layer_create(GRect(offset_x+(font_width*4+1)+(font_width*2+1)*i, text_col1_y, font_width*2, font_pt));
		text_layer_set_background_color(s_2weeks_layer[i], GColorClear);
		layer_add_child(window_get_root_layer(window), text_layer_get_layer(s_2weeks_layer[i]));
	}
	
	update_calendar();
}

void unload_calendar(Window * window) {

	layer_destroy(s_weekday_base_layer);
	text_layer_destroy(s_month_layer);
	text_layer_destroy(s_year_layer);
	for (int i=0; i<14; i++) {
		text_layer_destroy(s_2weeks_layer[i]);	
	}
}

void update_calendar() {
	time_t temp = time(NULL); 
  struct tm *tick_time = localtime(&temp);
	
	update_calendar_day(tick_time);
	update_calendar_month(tick_time);
	update_calendar_year(tick_time);
}

void calendar_tick_handler(struct tm *tick_time, TimeUnits units_changed) {
	if (units_changed & DAY_UNIT) {
		update_calendar_day(tick_time);
	}
	if (units_changed & MONTH_UNIT) {
		update_calendar_month(tick_time);
	}
	if (units_changed & YEAR_UNIT) {
		update_calendar_year(tick_time);
	}
}

