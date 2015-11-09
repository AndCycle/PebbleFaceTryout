#include <pebble.h>
#include "battery.h"
#include "main.h"

Layer *s_battery_layer;
TextLayer *s_battery_text_layer;

BatteryChargeState battery_state;

typedef struct {
	GRect bounds;
	BatteryChargeState last_battery_state;
	int32_t battery_angle;
} battery_layer_data;

void battery_handler(BatteryChargeState state) {
  // Record the new battery level
	
	if (battery_state.charge_percent == state.charge_percent) {
		APP_LOG(APP_LOG_LEVEL_DEBUG_VERBOSE, "battery state doesnt change, skip");
		//nothing changed
		return;
	}
	battery_state = state;
	APP_LOG(APP_LOG_LEVEL_DEBUG_VERBOSE, "Battery level %d", state.charge_percent);
	// Update meter
	layer_mark_dirty(s_battery_layer);
}


void battery_update_proc(Layer *layer, GContext *ctx) {
	battery_layer_data *data = layer_get_data(s_battery_layer);
	GRect *bounds = &data->bounds;
	
	static char b_battery_level[4];
	
	if (data->last_battery_state.charge_percent != battery_state.charge_percent) {
		data->last_battery_state = battery_state;
		snprintf(b_battery_level, sizeof(char)*4, "%d", battery_state.charge_percent);
		data->battery_angle = TRIG_MAX_ANGLE*(100-(battery_state.charge_percent))/100;
		APP_LOG(APP_LOG_LEVEL_DEBUG_VERBOSE, "%s", "battery val changed");
	}
	
	graphics_context_set_fill_color(ctx, GColorLightGray);
	graphics_fill_radial(ctx, *bounds, GOvalScaleModeFillCircle, 5, 0, TRIG_MAX_ANGLE);
	
	GColor batter_color;
	if (battery_state.charge_percent > 30) {
		batter_color = GColorGreen;
	} else {
		batter_color = GColorRed;
	}
	
	graphics_context_set_fill_color(ctx, batter_color);
	graphics_fill_radial(ctx, *bounds, GOvalScaleModeFillCircle, 5, data->battery_angle, TRIG_MAX_ANGLE);

	graphics_context_set_text_color(ctx, default_color);
	
	graphics_draw_text(ctx, b_battery_level, fonts_get_system_font(FONT_KEY_GOTHIC_18_BOLD), *bounds, GTextOverflowModeWordWrap, GTextAlignmentCenter, NULL);

	APP_LOG(APP_LOG_LEVEL_DEBUG_VERBOSE, "%s", "Draw battery");
}

void load_battery(Window *window) {
	
	GRect battery_grect = GRect(120, 144, 24, 24);
	
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
