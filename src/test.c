#include <pebble.h>
#include "test.h"

Layer *s_test_layer;

GPath *s_my_path_ptr = NULL;


TextLayer *s_test_text_layer;
TextLayer *s_test_text_layer2;
TextLayer *s_test_text_layer3;


const GPathInfo BOLT_PATH_INFO = {
  //.num_points = 6,
	.num_points = 4,
  //.points = (GPoint []) {{21, 0}, {14, 26}, {28, 26}, {7, 60}, {14, 34}, {0, 34}}
	.points = (GPoint []) {{0, 0}, {0, 10}, {10, 10}, {10, 0}}
};


void draw_test_layer(Layer *layer, GContext *ctx) {
	
  GRect bounds = layer_get_bounds(layer);
	APP_LOG(APP_LOG_LEVEL_DEBUG, "test inside rect x %d y %d w %d h %d", bounds.origin.x, bounds.origin.y, bounds.size.w, bounds.size.h);
  GPoint center = grect_center_point(&bounds);
	uint16_t radius = ((bounds.size.w<bounds.size.h)?bounds.size.w:bounds.size.h)/2-1;
  // Fill the path:
  graphics_context_set_fill_color(ctx, GColorRed);
  //gpath_draw_filled(ctx, s_my_path_ptr);
  // Stroke the path:
  //graphics_context_set_stroke_color(ctx, GColorBlue);
  //gpath_draw_outline(ctx, s_my_path_ptr);
	//graphics_context_set_stroke_width(ctx, 1);
	//graphics_draw_line(ctx, GPoint(10,0), GPoint(10, 10));
	//graphics_draw_circle(ctx, center, 3);
	//graphics_draw_circle(ctx, center, radius);
	graphics_context_set_stroke_width(ctx, 1);
	//graphics_draw_line(ctx, center, GPoint(center.x+radius, center.y+radius));
	//graphics_draw_line(ctx, GPoint(center.x,center.y), GPoint(center.x+30, center.y+40));
	//graphics_draw_arc(ctx, bounds, GOvalScaleModeFitCircle, 0, DEG_TO_TRIGANGLE(90));
	
	GPoint x = gpoint_from_polar(bounds, GOvalScaleModeFillCircle, DEG_TO_TRIGANGLE(0));
	APP_LOG(APP_LOG_LEVEL_DEBUG, "polar 1 x %d y %d", x.x, x.y);
	GPoint y = gpoint_from_polar(bounds, GOvalScaleModeFillCircle, DEG_TO_TRIGANGLE(90));
	APP_LOG(APP_LOG_LEVEL_DEBUG, "polar 2 x %d y %d", y.x, y.y);

	APP_LOG(APP_LOG_LEVEL_DEBUG, "center x %d y %d", center.x, center.y);

	if (center.x != x.x) {
		center.x = x.x;
	}
	if (center.y != y.y) {
		center.y = y.y;
	}
	
	for(int i=0; i<360; i+=(360/12)) {
		graphics_draw_line(ctx, center, gpoint_from_polar(bounds, GOvalScaleModeFitCircle, DEG_TO_TRIGANGLE(i)));
	}
	
	for(int i=0; i<12; i+=1) {
		GRect a = grect_centered_from_polar(bounds, GOvalScaleModeFitCircle, DEG_TO_TRIGANGLE(i*30+30), GSize(10,10));
		APP_LOG(APP_LOG_LEVEL_DEBUG, "inside react %d %d", a.origin.x, a.origin.y);
		graphics_draw_round_rect(ctx, a, 3);
	}
	
	/*
	for(int i=0; i<360; i+=(360/60)) {
		graphics_context_set_fill_color(ctx, GColorRed);
		graphics_context_set_stroke_width(ctx, 1);
		graphics_fill_radial(ctx, bounds, GOvalScaleModeFillCircle, 5, DEG_TO_TRIGANGLE(i-1), DEG_TO_TRIGANGLE(i+1));
	}
	
	for(int i=0; i<360; i+=(360/12)) {
		graphics_context_set_fill_color(ctx, GColorBlue);
		graphics_context_set_stroke_width(ctx, 3);
		graphics_fill_radial(ctx, bounds, GOvalScaleModeFillCircle, 10, DEG_TO_TRIGANGLE(i-1), DEG_TO_TRIGANGLE(i+1));
	}
	*/
}

void what() {
  s_my_path_ptr = gpath_create(&BOLT_PATH_INFO);
  // Rotate 15 degrees:
  //gpath_rotate_to(s_my_path_ptr, TRIG_MAX_ANGLE / 360 * 15);
  // Translate by (5, 5):
  //gpath_move_to(s_my_path_ptr, GPoint(5, 5));
}

void load_test(Window *window) {
	
	//APP_LOG(APP_LOG_LEVEL_DEBUG, "analog_grect x %d y %d w %d h %d", analog_grect.origin.x, analog_grect.origin.y, analog_grect.size.w, analog_grect.size.h);
	//APP_LOG(APP_LOG_LEVEL_DEBUG, "window_grect x %d y %d w %d h %d", window_grect.origin.x, window_grect.origin.y, window_grect.size.w, window_grect.size.h);
	
	/*
	//GRect test_layer_rect = GRect(10, 10, 99, 99);
	GRect test_layer_rect = layer_get_frame(window_get_root_layer(window));
	APP_LOG(APP_LOG_LEVEL_DEBUG, "test rect x %d y %d w %d h %d", test_layer_rect.origin.x, test_layer_rect.origin.y, test_layer_rect.size.w, test_layer_rect.size.h);
	
	s_test_layer = layer_create(test_layer_rect);
	layer_set_update_proc(s_test_layer, draw_test_layer);
	layer_add_child(window_get_root_layer(window), s_test_layer);
	
	
	APP_LOG(APP_LOG_LEVEL_DEBUG, "test rect x %d y %d w %d h %d", test_layer_rect.origin.x, test_layer_rect.origin.y, test_layer_rect.size.w, test_layer_rect.size.h);
	GRect temp_rect;
	temp_rect = layer_get_bounds(s_test_layer);
	APP_LOG(APP_LOG_LEVEL_DEBUG, "get bound x %d y %d w %d h %d", temp_rect.origin.x, temp_rect.origin.y, temp_rect.size.w, temp_rect.size.h);

	
	s_test_text_layer = text_layer_create(layer_get_bounds(window_get_root_layer(window)));
	text_layer_set_font(s_test_text_layer, fonts_get_system_font(FONT_KEY_GOTHIC_14));
	text_layer_set_text_color(s_test_text_layer, GColorBlack);
	text_layer_set_background_color(s_test_text_layer, GColorClear);
	text_layer_set_text(s_test_text_layer, "word");
	layer_add_child(window_get_root_layer(window), text_layer_get_layer(s_test_text_layer));
	
	APP_LOG(APP_LOG_LEVEL_DEBUG, "here");
	
	*/
	
}
