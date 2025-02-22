#ifndef __FT5x06_H__
#define __FT5x06_H__

#include "ft5x06_def.h"
#include "i2c_device.h"
#include <driver/i2c_master.h>


#define TOUCH_MAX_POINT_NUMBER (1)

typedef enum {
    TOUCH_MIRROR_X = 0x40, /**< Mirror X-axis */
    TOUCH_MIRROR_Y = 0x20, /**< Mirror Y-axis */
    TOUCH_SWAP_XY  = 0x80, /**< Swap XY axis */
} touch_panel_dir_t;

typedef enum {
    TOUCH_EVT_RELEASE = 0x0,  /*!< Release event */
    TOUCH_EVT_PRESS   = 0x1,  /*!< Press event */
} touch_panel_event_t;

typedef struct {
    touch_panel_event_t event;   /*!< Event of touch */
    uint8_t point_num;           /*!< Touch point number */
    uint16_t curx[TOUCH_MAX_POINT_NUMBER];            /*!< Current x coordinate */
    uint16_t cury[TOUCH_MAX_POINT_NUMBER];            /*!< Current y coordinate */
} touch_panel_points_t;

class FT5x06 : public I2cDevice {

public:

    FT5x06(i2c_master_bus_handle_t i2c_bus, uint8_t addr, int8_t rst_pin, int8_t intr_pin);

    esp_err_t ft5x06_deinit(void);

    esp_err_t ft5x06_read_point_data(touch_panel_points_t *point);
};



#endif //__FT5x06_H__