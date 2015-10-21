#pragma once

GRect shrink_grect(GRect, float);

GPoint gpoint_from_polar_shrink(GRect, GOvalScaleMode, int32_t, float);

GRect squre_grect(GRect);
GRect odd_grect(GRect);

GPoint gpoint_to_polar(GPoint, int32_t, int32_t);
GPoint ret_carry_center(GRectReturn, GOvalScaleMode);
