#include <pebble.h>
#include "battery.h"
#include "main.h"

Layer *s_battery_layer;
TextLayer *s_battery_text_layer;

GPath *s_battery_path_ptr = NULL;

#define BIZoom 4

const GPathInfo BATTERY_PATH_INFO = {
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

BatteryChargeState last_battery_state;
char *b_battery_level;

void battery_handler(BatteryChargeState state) {
  // Record the new battery level
  last_battery_state = state;
	APP_LOG(APP_LOG_LEVEL_INFO, "Battery level %d", state.charge_percent);
	// Update meter
	layer_mark_dirty(s_battery_layer);
}


void battery_update_proc(Layer *layer, GContext *ctx) {
	GRect bounds = layer_get_bounds(layer);

	graphics_context_set_fill_color(ctx, GColorLightGray);
	graphics_fill_radial(ctx, bounds, GOvalScaleModeFillCircle, 5, 0, TRIG_MAX_ANGLE);
	
	GColor batter_color;
	if (last_battery_state.charge_percent > 70) {
		batter_color = GColorGreen;
	} else if (last_battery_state.charge_percent > 30) {
		batter_color = GColorOrange;
	} else {
		batter_color = GColorRed;
	}
	
	graphics_context_set_fill_color(ctx, batter_color);
	graphics_fill_radial(ctx, bounds, GOvalScaleModeFillCircle, 5, TRIG_MAX_ANGLE*(100-last_battery_state.charge_percent)/100, TRIG_MAX_ANGLE);

	snprintf(b_battery_level, sizeof(char)*4, "%d", last_battery_state.charge_percent);
	graphics_context_set_text_color(ctx, default_color);
	graphics_draw_text(ctx, b_battery_level, fonts_get_system_font(FONT_KEY_GOTHIC_18), bounds, GTextOverflowModeWordWrap, GTextAlignmentCenter, NULL);

}


void init_battery() {
	b_battery_level = calloc(4, sizeof(char));
}
void deinit_battery() {
	free(b_battery_level);
}

void load_battery(Window *window) {

	GRect text_battery_grect = GRect(10, 10, 20, 20);
	
	s_battery_text_layer = text_layer_create(text_battery_grect);
	
	GRect battery_grect = GRect(120, 0, 24, 24);
	
	s_battery_layer = layer_create(battery_grect);
	layer_set_update_proc(s_battery_layer, battery_update_proc);
	layer_add_child(window_get_root_layer(window), s_battery_layer);
	
}

void unload_battery(Window *window) {
	layer_destroy(s_battery_layer);
	
}
