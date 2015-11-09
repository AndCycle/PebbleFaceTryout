#pragma once

bool angle_check(int32_t, int32_t, int32_t);

struct tm * sanitize_localtime();

GRect squre_grect(GRect);
GRect odd_grect(GRect);

GPoint gpoint_to_polar(GPoint, int32_t, int32_t);
GPoint ret_carry_center(GRectReturn, GOvalScaleMode);

int8_t days_in_month(struct tm *);
