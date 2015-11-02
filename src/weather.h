#pragma once

extern TextLayer *s_weather_layer;

void req_weather_update();
void process_weather_app_message(DictionaryIterator *, void *);

void weather_handler(struct tm *, TimeUnits);

void load_weather(Window *);
void unload_weather(Window *);

void init_weather();
void deinit_weather();
