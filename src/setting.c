#include <pebble.h>
#include "setting.h"

#define SETTING_VER_KEY 166
#define SETTING_VER 1

bool setting_check_ver(void) {
  if (persist_exists(SETTING_VER_KEY)) {
    return (SETTING_VER == persist_read_int(SETTING_VER_KEY));
  }
  return false;
}

void setting_write_ver(void) {
  if (
    persist_exists(SETTING_VER_KEY) &&
    (SETTING_VER == persist_read_int(SETTING_VER_KEY))
    ) {
      return;
  } 
  persist_write_int(SETTING_VER_KEY, SETTING_VER);
}
