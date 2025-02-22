#include "qm_y1091_display.h"
#include "font_awesome_symbols.h"
#include <esp_log.h>
#include <esp_err.h>
#include <driver/ledc.h>
#include <vector>
#include "qmsd_board.h"

#define TAG "QmY1091Display"

#define LCD_LEDC_CH LEDC_CHANNEL_0

#define LCD_LVGL_TICK_PERIOD_MS 2
#define LCD_LVGL_TASK_MAX_DELAY_MS 20
#define LCD_LVGL_TASK_MIN_DELAY_MS 1
#define LCD_LVGL_TASK_STACK_SIZE (4 * 1024)
#define LCD_LVGL_TASK_PRIORITY 10

LV_FONT_DECLARE(font_puhui_14_1);
LV_FONT_DECLARE(font_awesome_30_1);
LV_FONT_DECLARE(font_awesome_14_1);

FT5x06 *touch_panel_ = nullptr;

static lv_disp_drv_t disp_drv;
static void lcd_lvgl_flush_cb(lv_disp_drv_t *drv, const lv_area_t *area, lv_color_t *color_map)
{
    // esp_lcd_panel_handle_t panel_handle = (esp_lcd_panel_handle_t)drv->user_data;
    int offsetx1 = area->x1;
    int offsetx2 = area->x2;
    int offsety1 = area->y1;
    int offsety2 = area->y2;
    // copy a buffer's content to a specific area of the display
    // esp_lcd_panel_draw_bitmap(panel_handle, offsetx1, offsety1, offsetx2 + 1, offsety2 + 1, color_map);

    screen_draw_bitmap(offsetx1, offsety1, offsetx2-offsetx1+1,offsety2-offsety1+1,(uint16_t *)color_map);

    lv_disp_flush_ready(&disp_drv);
}

/* Rotate display and touch, when rotated screen in LVGL. Called when driver parameters are updated. */
static void lcd_lvgl_port_update_callback(lv_disp_drv_t *drv)
{
    // esp_lcd_panel_handle_t panel_handle = (esp_lcd_panel_handle_t)drv->user_data;

    // switch (drv->rotated)
    // {
    // case LV_DISP_ROT_NONE:
    //     // Rotate LCD display
    //     esp_lcd_panel_swap_xy(panel_handle, false);
    //     esp_lcd_panel_mirror(panel_handle, true, false);
    //     break;
    // case LV_DISP_ROT_90:
    //     // Rotate LCD display
    //     esp_lcd_panel_swap_xy(panel_handle, true);
    //     esp_lcd_panel_mirror(panel_handle, true, true);
    //     break;
    // case LV_DISP_ROT_180:
    //     // Rotate LCD display
    //     esp_lcd_panel_swap_xy(panel_handle, false);
    //     esp_lcd_panel_mirror(panel_handle, false, true);
    //     break;
    // case LV_DISP_ROT_270:
    //     // Rotate LCD display
    //     esp_lcd_panel_swap_xy(panel_handle, true);
    //     esp_lcd_panel_mirror(panel_handle, false, false);
    //     break;
    // }
}

QmY1091Display::QmY1091Display(int width, int height, int offset_x, int offset_y, bool mirror_x, bool mirror_y, bool swap_xy):
mirror_x_(mirror_x),
mirror_y_(mirror_y),
swap_xy_(swap_xy)
{
    width_ = width;
    height_ = height;
    offset_x_ = offset_x;
    offset_y_ = offset_y;

    qmsd_board_config_t config = QMSD_BOARD_DEFAULT_CONFIG;
    config.board_dir = BOARD_ROTATION_0;
    config.touch.en = 1;
    config.backlight.value = 0;
    config.gui.en = 1;
    qmsd_board_init(&config);

    std::vector<uint16_t> buffer(width_, 0xFFFF);
    for (int y = 0; y < height_; y++) {
        screen_draw_bitmap(0,y,width_,1,buffer.data());
    }

    lv_init();
    // alloc draw buffers used by LVGL
    static lv_disp_draw_buf_t disp_buf; // contains internal graphic buffer(s) called draw buffer(s)
    // it's recommended to choose the size of the draw buffer(s) to be at least 1/10 screen sized
    lv_color_t *buf1 = (lv_color_t *)heap_caps_malloc(width_ * 10 * sizeof(lv_color_t), MALLOC_CAP_DMA);
    assert(buf1);
    lv_color_t *buf2 = (lv_color_t *)heap_caps_malloc(width_ * 10 * sizeof(lv_color_t), MALLOC_CAP_DMA);
    assert(buf2);
    // initialize LVGL draw buffers
    lv_disp_draw_buf_init(&disp_buf, buf1, buf2, width_ * 10);

    ESP_LOGI(TAG, "Register display driver to LVGL");
    lv_disp_drv_init(&disp_drv);
    disp_drv.hor_res = width_;
    disp_drv.ver_res = height_;
    disp_drv.offset_x = offset_x_;
    disp_drv.offset_y = offset_y_;
    disp_drv.flush_cb = lcd_lvgl_flush_cb;
    disp_drv.drv_update_cb = lcd_lvgl_port_update_callback;
    disp_drv.draw_buf = &disp_buf;
    // disp_drv.user_data = panel_;

    lv_disp_drv_register(&disp_drv);

    ESP_LOGI(TAG, "Install LVGL tick timer");
    // Tick interface for LVGL (using esp_timer to generate 2ms periodic event)
    const esp_timer_create_args_t lvgl_tick_timer_args = {
        .callback = [](void* arg) {
            lv_tick_inc(LCD_LVGL_TICK_PERIOD_MS);
        },
        .arg = NULL,
        .dispatch_method = ESP_TIMER_TASK,
        .name = "LVGL Tick Timer",
        .skip_unhandled_events = false
    };
    ESP_ERROR_CHECK(esp_timer_create(&lvgl_tick_timer_args, &lvgl_tick_timer_));
    ESP_ERROR_CHECK(esp_timer_start_periodic(lvgl_tick_timer_, LCD_LVGL_TICK_PERIOD_MS * 1000));

    lvgl_mutex_ = xSemaphoreCreateRecursiveMutex();
    assert(lvgl_mutex_ != nullptr);
    ESP_LOGI(TAG, "Create LVGL task");
    xTaskCreate([](void *arg) {
        static_cast<QmY1091Display*>(arg)->LvglTask();
        vTaskDelete(NULL);
    }, "LVGL", LCD_LVGL_TASK_STACK_SIZE, this, LCD_LVGL_TASK_PRIORITY, NULL);

    // SetBacklight(100);

    SetupUI();
}

QmY1091Display::~QmY1091Display()
{

}



void QmY1091Display::LvglTask()
{
    ESP_LOGI(TAG, "Starting LVGL task");
    uint32_t task_delay_ms = LCD_LVGL_TASK_MAX_DELAY_MS;
    while (1)
    {
        // Lock the mutex due to the LVGL APIs are not thread-safe
        if (Lock())
        {
            task_delay_ms = lv_timer_handler();
            Unlock();
        }
        if (task_delay_ms > LCD_LVGL_TASK_MAX_DELAY_MS)
        {
            task_delay_ms = LCD_LVGL_TASK_MAX_DELAY_MS;
        }
        else if (task_delay_ms < LCD_LVGL_TASK_MIN_DELAY_MS)
        {
            task_delay_ms = LCD_LVGL_TASK_MIN_DELAY_MS;
        }
        vTaskDelay(pdMS_TO_TICKS(task_delay_ms));
    }
}

bool QmY1091Display::Lock(int timeout_ms)
{
    const TickType_t timeout_ticks = (timeout_ms == 0) ? portMAX_DELAY : pdMS_TO_TICKS(timeout_ms);
    return xSemaphoreTakeRecursive(lvgl_mutex_, timeout_ticks) == pdTRUE;
}

void QmY1091Display::Unlock()
{
    xSemaphoreGiveRecursive(lvgl_mutex_);
}

static inline lv_color_t lv_color_make_brg(uint8_t r, uint8_t g, uint8_t b)
{
    return lv_color_make(b, r, g);
}

void QmY1091Display::SetupUI() {
    DisplayLockGuard lock(this);

    auto screen = lv_disp_get_scr_act(lv_disp_get_default());
    lv_obj_set_style_text_font(screen, &font_puhui_14_1, 0);
    lv_obj_set_style_text_color(screen, lv_color_black(), 0);

    

    /* Container */
    container_ = lv_obj_create(screen);
    lv_obj_set_size(container_, LV_HOR_RES, LV_VER_RES);
    lv_obj_set_flex_flow(container_, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_style_pad_all(container_, 0, 0);
    lv_obj_set_style_border_width(container_, 0, 0);
    lv_obj_set_style_pad_row(container_, 0, 0);

    // /* Status bar */
    status_bar_ = lv_obj_create(container_);
    lv_obj_set_size(status_bar_, LV_HOR_RES, 18);
    lv_obj_set_style_radius(status_bar_, 0, 0);

    lv_obj_set_style_bg_color(status_bar_, lv_color_make_brg(0, 0,0), LV_PART_MAIN | LV_STATE_DEFAULT);
    

    



    // /* Content */
    content_ = lv_obj_create(container_);
    lv_obj_set_scrollbar_mode(content_, LV_SCROLLBAR_MODE_OFF);
    lv_obj_set_style_radius(content_, 0, 0);
    lv_obj_set_width(content_, LV_HOR_RES);
    lv_obj_set_flex_grow(content_, 1);
    lv_obj_set_style_bg_color(content_, lv_color_make(0x00, 0x0, 0x00), LV_PART_MAIN | LV_STATE_DEFAULT);

    lv_obj_set_flex_flow(content_, LV_FLEX_FLOW_COLUMN); // 垂直布局（从上到下）
    lv_obj_set_flex_align(content_, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_SPACE_EVENLY); // 子对象居中对齐，等距分布


    // lv_obj_t* rect = lv_obj_create(content_);
    // lv_obj_set_size(rect, 50, 50);
    // lv_obj_set_style_border_width(rect, 0, 0);
    // lv_obj_set_style_bg_color(rect, lv_color_make(0xFF, 0xFF, 0xFF), LV_PART_MAIN | LV_STATE_DEFAULT);


    lv_obj_t* mainIconArea = lv_obj_create(content_);
    lv_obj_set_size(mainIconArea, LV_HOR_RES, 200);//LV_FLEX_FLOW_ROW
    lv_obj_set_style_border_width(mainIconArea, 0, 0);
    lv_obj_set_style_radius(mainIconArea, 0, 0);
    lv_obj_set_flex_flow(mainIconArea, LV_FLEX_FLOW_ROW);
    lv_obj_set_flex_align(mainIconArea, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_SPACE_EVENLY); // 子对象居中对齐，等距分布
    lv_obj_set_style_bg_color(mainIconArea, lv_color_make(0x00, 0x0, 0x00), LV_PART_MAIN | LV_STATE_DEFAULT);

    // 
    int ui_speakBar_height[5] = {40, 50, 80, 50, 40};

    for (size_t i = 0; i < 5; i++)
    {
        ui_speakBar[i] = lv_obj_create(mainIconArea);
        lv_obj_set_width(ui_speakBar[i], 30);
        lv_obj_set_height(ui_speakBar[i], ui_speakBar_height[i]);
        lv_obj_set_style_radius(ui_speakBar[i], 15, 0);
        lv_obj_set_style_border_width(ui_speakBar[i], 0, 0);
        // lv_obj_set_x(ui_iconBar[i], 0);
        // lv_obj_set_y(ui_iconBar[i], -14);
        // lv_obj_set_align(ui_iconBar[i], LV_ALIGN_CENTER);
        lv_obj_set_style_bg_color(ui_speakBar[i], lv_color_make(0xFF, 0xFF, 0xFF), LV_PART_MAIN | LV_STATE_DEFAULT);
    }
    

    // emotion_label_ = lv_label_create(content_);
    // lv_obj_set_style_text_font(emotion_label_, &font_awesome_30_1, 0);
    // lv_label_set_text(emotion_label_, FONT_AWESOME_AI_CHIP);
    // // lv_obj_center(emotion_label_);

    // chat_message_label_ = lv_label_create(content_);
    // lv_label_set_text(chat_message_label_, "");
    // lv_obj_set_width(chat_message_label_, LV_HOR_RES * 0.8); // 限制宽度为屏幕宽度的 80%
    // lv_label_set_long_mode(chat_message_label_, LV_LABEL_LONG_WRAP); // 设置为自动换行模式
    // lv_obj_set_style_text_align(chat_message_label_, LV_TEXT_ALIGN_CENTER, 0); // 设置文本居中对齐

    // lv_obj_del(chat_message_label_);

    lv_obj_set_flex_align(content_, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_SPACE_EVENLY); // 子对象居中对齐，等距分布

    // lv_obj_set_style_text_font(emotion_label_, &font_awesome_30_1, 0);
    // lv_label_set_text(emotion_label_, FONT_AWESOME_AI_CHIP);
    // lv_obj_align(emotion_label_, LV_ALIGN_TOP_MID, 0, -10); // 左侧居中，向右偏移10个单位

    static lv_style_t style_msg;
    lv_style_init(&style_msg);
    lv_style_set_width(&style_msg, LV_HOR_RES - 25);

    user_messge_label_ = lv_label_create(content_);
    // lv_obj_set_style_text_color(user_messge_label_, lv_color_make(0xFF, 0xFF, 0xFF), 0);
    lv_obj_set_style_text_font(user_messge_label_, &font_puhui_14_1, 0);
    lv_label_set_text(user_messge_label_, "用户: ");
    lv_obj_add_style(user_messge_label_, &style_msg, 0);
    lv_obj_align(user_messge_label_, LV_ALIGN_TOP_LEFT, 2, 25);

    ai_messge_label_ = lv_label_create(content_);
    // lv_obj_set_style_text_color(ai_messge_label_, lv_color_make(0xFF, 0xFF, 0xFF), 0);
    lv_obj_set_style_text_font(ai_messge_label_, &font_puhui_14_1, 0);
    lv_label_set_text(ai_messge_label_, "小志: ");
    lv_obj_add_style(ai_messge_label_, &style_msg, 0);
    lv_obj_align(ai_messge_label_, LV_ALIGN_TOP_LEFT, 2, 77);

    // /* Status bar */
    lv_obj_set_flex_flow(status_bar_, LV_FLEX_FLOW_ROW);
    lv_obj_set_style_pad_all(status_bar_, 0, 0);
    lv_obj_set_style_border_width(status_bar_, 0, 0);
    lv_obj_set_style_pad_column(status_bar_, 0, 0);

    // content_
    // lv_obj_set_flex_flow(content_, LV_FLEX_FLOW_ROW);
    // lv_obj_set_style_pad_all(content_, 0, 0);
    lv_obj_set_style_border_width(content_, 0, 0);
    // lv_obj_set_style_pad_column(content_, 0, 0);

    network_label_ = lv_label_create(status_bar_);
    // lv_obj_set_style_text_color(network_label_, lv_color_make(0xFF, 0xFF, 0xFF), 0);
    lv_label_set_text(network_label_, "");
    lv_obj_set_style_text_font(network_label_, &font_awesome_14_1, 0);

    notification_label_ = lv_label_create(status_bar_);
    lv_obj_set_flex_grow(notification_label_, 1);
    lv_obj_set_style_text_align(notification_label_, LV_TEXT_ALIGN_CENTER, 0);
    lv_label_set_text(notification_label_, "通知");
    lv_obj_add_flag(notification_label_, LV_OBJ_FLAG_HIDDEN);

    status_label_ = lv_label_create(status_bar_);
    // lv_obj_set_style_text_color(status_label_, lv_color_make(0xFF, 0xFF, 0xFF), 0);
    lv_obj_set_flex_grow(status_label_, 1);
    lv_label_set_long_mode(status_label_, LV_LABEL_LONG_SCROLL_CIRCULAR);
    lv_label_set_text(status_label_, "正在初始化");
    lv_obj_set_style_text_align(status_label_, LV_TEXT_ALIGN_CENTER, 0);

    mute_label_ = lv_label_create(status_bar_);
    lv_label_set_text(mute_label_, "");
    lv_obj_set_style_text_font(mute_label_, &font_awesome_14_1, 0);

    battery_label_ = lv_label_create(status_bar_);
    lv_label_set_text(battery_label_, "");
    lv_obj_set_style_text_font(battery_label_, &font_awesome_14_1, 0);
}

static void lvgl_tp_read(struct _lv_indev_drv_t* indev_drv, lv_indev_data_t* data) {
    int8_t press = 0;
    // int16_t x, y;
    touch_panel_points_t points;
    touch_panel_->ft5x06_read_point_data(&points);
    press = points.event == TOUCH_EVT_PRESS;
    if (press) {
        data->point.x = points.curx[0];
        data->point.y = points.cury[0];
        data->state = LV_INDEV_STATE_PR;
    } else {
        data->state = LV_INDEV_STATE_REL;
    }
    // ESP_LOGI(TAG, "Press:%d, x:%d, y:%d", press, data->point.x, data->point.y);
}

void QmY1091Display::initTouchPanel(FT5x06* tp)
{
    touch_panel_ = tp;
    static lv_indev_drv_t* indev_drv;
    indev_drv = (lv_indev_drv_t*)malloc(sizeof(lv_indev_drv_t));
    lv_indev_drv_init(indev_drv);
    indev_drv->type = LV_INDEV_TYPE_POINTER;
    indev_drv->read_cb = lvgl_tp_read;
    lv_indev_drv_register(indev_drv);
}

void QmY1091Display::SetChatMessage(const std::string &role, const std::string &content)
{
    if (ai_messge_label_ == nullptr || user_messge_label_ == nullptr)
        {
            return;
        }

        DisplayLockGuard lock(this);
        ESP_LOGI(TAG, "role:%s", role.c_str());
        if (role == "assistant")
        {
            std::string new_content = "小志: " + content;
            lv_label_set_text(ai_messge_label_, new_content.c_str());
        }
        else if (role == "user")
        {
            std::string new_content = "用户: " + content;
            lv_label_set_text(user_messge_label_, new_content.c_str());
        }
        else
        {
            lv_label_set_text(ai_messge_label_, "小志: ");
            lv_label_set_text(user_messge_label_, "用户: ");
        }
}