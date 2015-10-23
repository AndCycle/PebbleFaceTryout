#pragma once

void load_analog(Window *);
void unload_analog(Window *);

void analog_tick_handler(struct tm *, TimeUnits);

void draw_analog_hour_hand_layer(Layer *, GContext *);
void draw_analog_min_hand_layer(Layer *, GContext *);
void draw_analog_sec_hand_layer(Layer *, GContext *);
void draw_tick_layer(Layer *, GContext *);

