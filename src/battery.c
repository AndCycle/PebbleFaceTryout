#include <pebble.h>
#include "battery.h"
#include "main.h"

Layer *s_battery_layer;
TextLayer *s_battery_text_layer;

typedef struct {
	GRect bounds;
	BatteryChargeState battery_state;
} battery_layer_data;

void battery_handler(BatteryChargeState state) {
  // Record the new battery level
	battery_layer_data *temp_battery_layer_data = layer_get_data(s_battery_layer);
	if (temp_battery_layer_data->battery_state.charge_percent == state.charge_percent) {
		APP_LOG(APP_LOG_LEVEL_DEBUG_VERBOSE, "battery state doesnt change");
		//nothing changed
		return;
	}
	temp_battery_layer_data->battery_state = state;
	APP_LOG(APP_LOG_LEVEL_DEBUG_VERBOSE, "Battery level %d", state.charge_percent);
	// Update meter
	layer_mark_dirty(s_battery_layer);
}


void battery_update_proc(Layer *layer, GContext *ctx) {
	battery_layer_data *temp_battery_layer_data = layer_get_data(s_battery_layer);
	GRect bounds = temp_battery_layer_data->bounds;
	BatteryChargeState battery_state = temp_battery_layer_data->battery_state;
	static char b_battery_level[4];
	
	graphics_context_set_fill_color(ctx, GColorLightGray);
	graphics_fill_radial(ctx, bounds, GOvalScaleModeFillCircle, 5, 0, TRIG_MAX_ANGLE);
	
	GColor batter_color;
	if (battery_state.charge_percent > 30) {
		batter_color = GColorGreen;
	} else {
		batter_color = GColorRed;
	}
	
	graphics_context_set_fill_color(ctx, batter_color);
	graphics_fill_radial(ctx, bounds, GOvalScaleModeFillCircle, 5, TRIG_MAX_ANGLE*(100-(battery_state.charge_percent))/100, TRIG_MAX_ANGLE);

	graphics_context_set_text_color(ctx, default_color);
	snprintf(b_battery_level, sizeof(char)*4, "%d", battery_state.charge_percent);
	graphics_draw_text(ctx, b_battery_level, fonts_get_system_font(FONT_KEY_GOTHIC_18), bounds, GTextOverflowModeWordWrap, GTextAlignmentCenter, NULL);

}

void load_battery(Window *window) {
	
	GRect battery_grect = GRect(120, 0, 24, 24);
	
	s_battery_layer = layer_create_with_data(battery_grect, sizeof(battery_layer_data));
	battery_layer_data *temp_battery_layer_data = layer_get_data(s_battery_layer);
	temp_battery_layer_data->bounds = layer_get_bounds(s_battery_layer);
	layer_set_update_proc(s_battery_layer, battery_update_proc);
	layer_add_child(window_get_root_layer(window), s_battery_layer);

	battery_handler(battery_state_service_peek());
}

void unload_battery(Window *window) {
	battery_layer_data *temp_battery_layer_data = layer_get_data(s_battery_layer);
	layer_destroy(s_battery_layer);
}
