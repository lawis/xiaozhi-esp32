#include "string.h"
#include "hal/gpio_hal.h"
#include "esp_log.h"
#include "qmsd_utils.h"
#include "qmsd_board.h"
#include "qmsd_lcd_wrapper.h"
#include "aw9523.h"


#define TAG "QMSD_BOARD"


static qmsd_board_config_t g_board_config;


static void touch_read(uint8_t* press, uint16_t* x, uint16_t* y);

void qmsd_board_init(qmsd_board_config_t* config) {
    memcpy(&g_board_config, config, sizeof(qmsd_board_config_t));
    
    qmsd_board_init_screen();
    qmsd_board_backlight_init(LCD_BL_PIN, 1, QMSD_SCREEN_BK_FREQ);
    qmsd_board_backlight_set_delay(g_board_config.backlight.value, g_board_config.backlight.delay_ms);;

    if (g_board_config.touch.en) {
        qmsd_board_init_touch();
    }

    if (g_board_config.gui.en) {
        // qmsd_board_init_gui();
    }
}

void qmsd_board_init_screen() {
    scr_dir_t screen_dir[] = { QMSD_SCREEN_DIR_0, QMSD_SCREEN_DIR_90, QMSD_SCREEN_DIR_180, QMSD_SCREEN_DIR_270};

    // Only for ZX3D50CE02S to detect screen version
    uint8_t board_screen_select = 0;
    gpio_hal_iomux_func_sel(GPIO_PIN_MUX_REG[LCD_D0_PIN], PIN_FUNC_GPIO);
    gpio_set_pull_mode((gpio_num_t)LCD_D0_PIN, GPIO_PULLUP_ONLY);
    gpio_set_direction((gpio_num_t)LCD_D0_PIN, GPIO_MODE_INPUT);
    gpio_hal_iomux_func_sel(GPIO_PIN_MUX_REG[LCD_D3_PIN], PIN_FUNC_GPIO);
    gpio_set_pull_mode((gpio_num_t)LCD_D3_PIN, GPIO_PULLUP_ONLY);
    gpio_set_direction((gpio_num_t)LCD_D3_PIN, GPIO_MODE_INPUT);
    board_screen_select = gpio_get_level((gpio_num_t)LCD_D0_PIN) << 4 | gpio_get_level((gpio_num_t)LCD_D3_PIN);

    i2s_lcd_config_t i2s_lcd_cfg = {
        .data_width = 8,
        .pin_data_num = {
            LCD_D0_PIN,
            LCD_D1_PIN,
            LCD_D2_PIN,
            LCD_D3_PIN,
            LCD_D4_PIN,
            LCD_D5_PIN,
            LCD_D6_PIN,
            LCD_D7_PIN,
        },
        .pin_num_cs = LCD_CS_PIN,
        .pin_num_wr = LCD_WR_PIN,                          
        .pin_num_rs = LCD_RS_PIN,
        .clk_freq = QMSD_SCREEN_CLK_SPEED,
        .i2s_port = (i2s_port_t)I2S_NUM_1,
        .swap_data = true,
        .buffer_size = QMSD_SCREEN_DRIVER_CACHE_SIZE,
    };

    scr_interface_driver_t *iface_drv;
    scr_interface_create(SCREEN_IFACE_8080, &i2s_lcd_cfg, &iface_drv);
    if (board_screen_select == 0x11) {
        extern esp_err_t qmsd_lcd_reg_config(void);
        g_lcd_driver->init_reg = qmsd_lcd_reg_config;
    } else {
        extern esp_err_t qmsd_lcd_reg_config_v2(void);
        g_lcd_driver->init_reg = qmsd_lcd_reg_config_v2;
    }
    scr_controller_config_t lcd_cfg = {
        .interface_drv = iface_drv,
        .pin_num_rst = LCD_RST_PIN,
        .pin_num_bckl = -1,
        .rst_active_level = 0,
        .bckl_active_level = 1,
        .width = QMSD_SCREEN_WIDTH,
        .height = QMSD_SCREEN_HIGHT,
        .offset_hor = 0,
        .offset_ver = 0,
        .rotate = screen_dir[g_board_config.board_dir],
    };
    g_lcd_driver->init(&lcd_cfg);
}

void qmsd_board_init_touch() {
    // if (TP_I2C_SDA_PIN == -1 || TP_I2C_SCL_PIN == -1) {
    //     ESP_LOGW(TAG, "Enable touch but TP_I2C_SDA_PIN or TP_I2C_SCL_PIN not set");
    //     return ;
    // }
    // touch_panel_dir_t touch_dir[] = {QMSD_TOUCH_DIR_0, QMSD_TOUCH_DIR_90, QMSD_TOUCH_DIR_180, QMSD_TOUCH_DIR_270};
    // touch_panel_config_t touch_config = {
    //     .sda_pin = TP_I2C_SDA_PIN,
    //     .scl_pin = TP_I2C_SCL_PIN,
    //     .rst_pin = TP_I2C_RST_PIN,
    //     .intr_pin = TP_I2C_INT_PIN,
    //     .i2c_num = 0,
    //     .i2c_freq = g_board_config.touch.i2c_freq,
    //     .task_en = g_board_config.touch.update_task.en,
    //     .task_priority = g_board_config.touch.update_task.priority,
    //     .task_core = g_board_config.touch.update_task.core,
    //     .task_stack_size = g_board_config.touch.update_task.stack_size,
    //     .width = QMSD_SCREEN_WIDTH,
    //     .height = QMSD_SCREEN_HIGHT,
    //     .direction = touch_dir[g_board_config.board_dir],
    // };
    // touch_init(&touch_ft5x06_driver, &touch_config);
}


scr_driver_t* qmsd_board_get_screen_driver() {
    return g_lcd_driver;
}



static void touch_read(uint8_t* press, uint16_t* x, uint16_t* y) {
    // touch_panel_points_t points;
    // touch_read_points(&points);
    // *press = points.event == TOUCH_EVT_PRESS;
    // *x = points.curx[0];
    // *y = points.cury[0];
}
