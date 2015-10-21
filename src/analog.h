#pragma once

extern Layer *s_analog_hour_hand_layer;
extern Layer *s_analog_min_hand_layer;
extern Layer *s_analog_sec_hand_layer;


extern Layer *s_analog_tick_layer;

extern TextLayer *s_no12_layer[12];

void load_analog(Window *);
void unload_analog(Window *);

void analog_tick_handler(struct tm *, TimeUnits);

void draw_analog_hour_hand_layer(Layer *, GContext *);
void draw_analog_min_hand_layer(Layer *, GContext *);
void draw_analog_sec_hand_layer(Layer *, GContext *);
void draw_tick_layer(Layer *, GContext *);

