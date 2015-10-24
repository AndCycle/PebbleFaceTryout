#pragma once

void battery_handler(BatteryChargeState);

void battery_update_proc(Layer *, GContext *);

void load_battery(Window *);
void unload_battery(Window *);
