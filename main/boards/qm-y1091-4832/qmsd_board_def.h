#pragma once

#include "sdkconfig.h"

// lcd driver true width and hight, not board width and hight
#define QMSD_SCREEN_WIDTH 320
#define QMSD_SCREEN_HIGHT 480
#define QMSD_GUI_MAX_BUFFER_NUM 3

#define QMSD_SCREEN_BK_FREQ     11111

#define QMSD_SCREEN_CLK_SPEED (40 * 1000 * 1000)

// lcd 8080 driver and rgb driver need cache
#define QMSD_SCREEN_DRIVER_CACHE_SIZE (8 * 1024)

#define QMSD_SCREEN_DIR_0       SCR_MIRROR_X
#define QMSD_SCREEN_DIR_90      (scr_dir_t)(QMSD_SCREEN_DIR_0 ^ SCR_MIRROR_X ^ SCR_SWAP_XY)
#define QMSD_SCREEN_DIR_180     (scr_dir_t)(QMSD_SCREEN_DIR_0 ^ SCR_MIRROR_X ^ SCR_MIRROR_Y)
#define QMSD_SCREEN_DIR_270     (scr_dir_t)(QMSD_SCREEN_DIR_0 ^ SCR_MIRROR_Y ^ SCR_SWAP_XY)

#define QMSD_TOUCH_DIR_0        0
#define QMSD_TOUCH_DIR_90       (scr_dir_t)(QMSD_TOUCH_DIR_0 ^ TOUCH_MIRROR_X ^ TOUCH_SWAP_XY)
#define QMSD_TOUCH_DIR_180      (scr_dir_t)(QMSD_TOUCH_DIR_0 ^ TOUCH_MIRROR_X ^ TOUCH_MIRROR_Y)
#define QMSD_TOUCH_DIR_270      (scr_dir_t)(QMSD_TOUCH_DIR_0 ^ TOUCH_MIRROR_Y ^ TOUCH_SWAP_XY)
