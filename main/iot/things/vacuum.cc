#include "iot/thing.h"
#include "board.h"
#include "audio_codec.h"

#include <driver/gpio.h>
#include <esp_log.h>
#include "iot/ha_client/ha_client.h"

#define TAG "Vacuum"

namespace iot {

// 这里仅定义 Cover 的属性和方法，不包含具体的实现
class Vacuum : public Thing {
private:
    gpio_num_t gpio_num_ = GPIO_NUM_18;
    bool working_ = false;

    void InitializeGpio() {
        // gpio_config_t config = {
        //     .pin_bit_mask = (1ULL << gpio_num_),
        //     .mode = GPIO_MODE_OUTPUT,
        //     .pull_up_en = GPIO_PULLUP_DISABLE,
        //     .pull_down_en = GPIO_PULLDOWN_DISABLE,
        //     .intr_type = GPIO_INTR_DISABLE,
        // };
        // ESP_ERROR_CHECK(gpio_config(&config));
        // gpio_set_level(gpio_num_, 0);
    }

public:
    Vacuum() : Thing("Vacuum", "家里的扫地机器人，充电站在办公区"), working_(false) {
        InitializeGpio();

        // 定义设备的属性
        properties_.AddBooleanProperty("working", "扫地机器人是否在工作中", [this]() -> bool {
            return working_;
        });

        // 定义设备可以被远程执行的指令
        methods_.AddMethod("Start", "开始扫地", ParameterList(), [this](const ParameterList& parameters) {
            working_ = true;
            // gpio_set_level(gpio_num_, 1);
            ESP_LOGI(TAG, "开始扫地");
            HaClient::GetInstance().post_services_vacuum_start("vacuum.roborock_a11_81f8_robot_cleaner");
        });

        methods_.AddMethod("Stop", "停止扫地", ParameterList(), [this](const ParameterList& parameters) {
            working_ = false;
            ESP_LOGI(TAG, "停止扫地");
            HaClient::GetInstance().post_services_vacuum_stop("vacuum.roborock_a11_81f8_robot_cleaner");
            // gpio_set_level(gpio_num_, 0);
        });

        methods_.AddMethod("return_to_base", "回到充电基地，或者叫开始回充", ParameterList(), [this](const ParameterList& parameters) {
            working_ = false;
            ESP_LOGI(TAG, "回到充电基地，或者叫开始回充");
            HaClient::GetInstance().post_services_vacuum_return_to_base("vacuum.roborock_a11_81f8_robot_cleaner");
            // gpio_set_level(gpio_num_, 0);
        });

        methods_.AddMethod("locate", "让扫地机器人告诉我们它在哪里", ParameterList(), [this](const ParameterList& parameters) {
            working_ = false;
            ESP_LOGI(TAG, "让扫地机器人告诉我们它在哪里");
            HaClient::GetInstance().post_services_vacuum_locate("vacuum.roborock_a11_81f8_robot_cleaner");
            // gpio_set_level(gpio_num_, 0);
        });
    }
};

} // namespace iot

DECLARE_THING(Vacuum);
