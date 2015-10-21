#pragma once

void init_calendar();
void deinit_calendar();

void calendar_tick_handler(struct tm *, TimeUnits);
void update_calendar();

void load_calendar(Window *);
void unload_calendar(Window *);

void draw_weekday_base_layer(Layer *, GContext *);
void update_calendar_year(struct tm *);
void update_calendar_month(struct tm *);
void update_calendar_day(struct tm *);

