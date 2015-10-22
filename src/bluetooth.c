#include <pebble.h>
#include "bluetooth.h"
#include "main.h"

Layer *s_bluetooth_layer;

GPath *s_bluetooth_path_ptr = NULL;

#define BIZoom 4

const GPathInfo BLUETOOTH_PATH_INFO = {
  .num_points = 7,
	.points = (GPoint []) {
		{0, 1*BIZoom}, {2*BIZoom, 3*BIZoom}, 	{1*BIZoom, 4*BIZoom}, {1*BIZoom, 0}, {2*BIZoom, 1*BIZoom}, 	{0, 3*BIZoom}, {1*BIZoom, 2*BIZoom}
	}
	//.points = (GPoint []) {{0, 1}, {2, 3}, {1, 4}, {1, 0}, {2, 1}, {0, 3}, {1, 2}}
	//.points = (GPoint []) {{0, 2}, {4, 6}, {2, 8}, {2, 0}, {4, 2}, {0, 6}, {2, 4}}
	//.points = (GPoint []) {{0, 3}, {6, 9}, {3, 12}, {3, 0}, {6, 3}, {0, 9}, {3, 6}}
	//.points = (GPoint []) {{0, 4}, {8, 12}, {4, 16}, {4, 0}, {8, 4}, {0, 12}, {4, 8}}
	// .points = (GPoint []) {{0, 5}, {10, 15}, {5, 20}, {5, 0}, {10, 5}, {0, 15}, {5, 10}}
};

bool bluetooth_connected;

void bluetooth_handler(bool connected) {
	bluetooth_connected = connected;
	layer_mark_dirty(s_bluetooth_layer);
  // Show current connection state
	if (connected) {
    APP_LOG(APP_LOG_LEVEL_INFO, "Phone is connected!");
  } else {
    APP_LOG(APP_LOG_LEVEL_INFO, "Phone is not connected!");
  }
}

void update_bluetooth_proc(Layer *layer, GContext *ctx) {
	GRect bounds = layer_get_bounds(layer);
	graphics_context_set_fill_color(ctx, GColorBlue);
	graphics_fill_rect(ctx, bounds, 4, GCornersAll);
	
  // Stroke the path:
	//graphics_context_set_antialiased(ctx, false);
	if (bluetooth_connected) {
		graphics_context_set_stroke_color(ctx, GColorWhite);	
	} else {
		graphics_context_set_stroke_color(ctx, GColorRed);
	}
  
	graphics_context_set_stroke_width(ctx, 1);
  gpath_draw_outline(ctx, s_bluetooth_path_ptr);
	// Fill the path:
  graphics_context_set_fill_color(ctx, GColorBlue);
	gpath_draw_filled(ctx, s_bluetooth_path_ptr);
}

void init_bluetooth() {
	s_bluetooth_path_ptr = gpath_create(&BLUETOOTH_PATH_INFO);
	gpath_move_to(s_bluetooth_path_ptr, GPoint(1,1));
}

void deinit_bluetooth() {
	gpath_destroy(s_bluetooth_path_ptr);
}

void load_bluetooth(Window *window) {
	//GRect window_grect = layer_get_frame(window_get_root_layer(window));
	//GRect bluetooth_grect = GRect(10, 10, 3*BIZoom, 5*BIZoom);
	GRect bluetooth_grect = GRect(129, 120, 3*BIZoom-1, 5*BIZoom-1);
	
	s_bluetooth_layer = layer_create(bluetooth_grect);
	layer_set_update_proc(s_bluetooth_layer, update_bluetooth_proc);
	layer_add_child(window_get_root_layer(window), s_bluetooth_layer);
	
	
}

void unload_bluetooth(Window * window) {
	layer_destroy(s_bluetooth_layer);
}