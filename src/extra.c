#include <pebble.h>
#include "extra.h"

struct tm * sanitize_localtime() {
	return localtime(&(time_t){time(NULL)}); //sanitize localtime tm struct pointer
}

GRect squre_grect(GRect t_rect) {
	int16_t short_side = t_rect.size.w < t_rect.size.h ? t_rect.size.w : t_rect.size.h;
	return GRect(
			t_rect.origin.x,
			t_rect.origin.y,
			short_side,
			short_side
		);
}

GRect odd_grect(GRect t_rect) {
		return GRect(
			t_rect.origin.x,
			t_rect.origin.y,
			t_rect.size.w%2 == 0 ? t_rect.size.w-1 : t_rect.size.w,
			t_rect.size.h%2 == 0 ? t_rect.size.h-1 : t_rect.size.h
		);
}

GPoint gpoint_to_polar(GPoint t_center, int32_t angle, int32_t length) {
	return GPoint(
			(int32_t)(sin_lookup(angle) * (int32_t)length / TRIG_MAX_RATIO) + t_center.x,
  	  (int32_t)(-cos_lookup(angle) * (int32_t)length / TRIG_MAX_RATIO) + t_center.y
  	);
}

GPoint ret_carry_center(GRect bounds, GOvalScaleMode govalscalemode) {
	GPoint center = grect_center_point(&bounds);
	
	// trying to avoid carry error
	GPoint x = gpoint_from_polar(bounds, govalscalemode, TRIG_MAX_ANGLE);
	GPoint y = gpoint_from_polar(bounds, govalscalemode, TRIG_MAX_ANGLE/4);
	if (center.x != x.x) {center.x = x.x;};
	if (center.y != y.y) {center.y = y.y;};
	return center;
}
