#ifndef __QmY1091Display_H__
#define __QmY1091Display_H__


#include "display/display.h"
#include <esp_lcd_panel_vendor.h>

#include <freertos/FreeRTOS.h>
#include <freertos/semphr.h>
#include <driver/gpio.h>
#include <esp_timer.h>

#include "ft5x06.h"



class QmY1091Display : public Display 
{
private:
    lv_obj_t *user_messge_label_ = nullptr;
    lv_obj_t *ai_messge_label_ = nullptr;

    
protected:
    bool backlight_output_invert_ = false;
    bool mirror_x_ = false;
    bool mirror_y_ = false;
    bool swap_xy_ = false;
    int offset_x_ = 0;
    int offset_y_ = 0;
    SemaphoreHandle_t lvgl_mutex_ = nullptr;
    esp_timer_handle_t lvgl_tick_timer_ = nullptr;
    
    lv_obj_t* status_bar_ = nullptr;
    lv_obj_t* content_ = nullptr;
    lv_obj_t* container_ = nullptr;
    lv_obj_t* side_bar_ = nullptr;
    lv_obj_t* chat_message_label_ = nullptr;
    lv_obj_t* ui_speakBar[5] = {nullptr};

    void InitializeBacklight(gpio_num_t backlight_pin);

    void SetBacklight(uint8_t brightness);

    void LvglTask();


    virtual bool Lock(int timeout_ms = 0) override;

    virtual void Unlock() override;

public:

    QmY1091Display(int width, int height,  int offset_x, int offset_y, bool mirror_x, bool mirror_y, bool swap_xy);
    
    ~QmY1091Display();

    virtual void SetupUI();

    void SetChatMessage(const std::string &role, const std::string &content) override;

    void initTouchPanel(FT5x06* tp);
};



#endif //__QmY1091Display_H__