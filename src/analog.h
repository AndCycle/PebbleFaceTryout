#pragma once

void load_analog(Window *);
void unload_analog(Window *);

void analog_tick_handler(struct tm *, TimeUnits);

void refresh_analog_enable_second();
