#include "iot/thing.h"
#include "board.h"
#include "audio_codec.h"

#include <driver/gpio.h>
#include <esp_log.h>
#include "iot/ha_client/ha_client.h"

#define TAG "Lamp2"

namespace iot {

// 这里仅定义 Lamp 的属性和方法，不包含具体的实现
class Lamp2 : public Thing {
private:
    gpio_num_t gpio_num_ = GPIO_NUM_18;
    bool power_ = false;

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
    Lamp2() : Thing("Lamp2", "会议室的吸顶灯"), power_(false) {
        InitializeGpio();

        // 定义设备的属性
        properties_.AddBooleanProperty("power", "灯是否打开", [this]() -> bool {
            return power_;
        });

        // 定义设备可以被远程执行的指令
        methods_.AddMethod("TurnOn", "打开灯", ParameterList(), [this](const ParameterList& parameters) {
            power_ = true;
            // gpio_set_level(gpio_num_, 1);
            ESP_LOGI(TAG, "打开会议室灯");
            HaClient::GetInstance().post_services_light_toggle("light.tze200_do3lhj3y_ts0601_deng_guang_3", true);
        });

        methods_.AddMethod("TurnOff", "关闭灯", ParameterList(), [this](const ParameterList& parameters) {
            power_ = false;
            ESP_LOGI(TAG, "关闭会议室灯");
            HaClient::GetInstance().post_services_light_toggle("light.tze200_do3lhj3y_ts0601_deng_guang_3", false);
        });
    }
};

} // namespace iot

DECLARE_THING(Lamp2);
