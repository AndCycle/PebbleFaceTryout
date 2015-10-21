#pragma once

void update_digit_time();

void digit_tick_handler(struct tm *, TimeUnits);

void load_digit(Window *);
void unload_digit(Window *);

void init_digit();
void deinit_digit();


