#include "iot/thing.h"
#include "board.h"
#include "audio_codec.h"

#include <driver/gpio.h>
#include <esp_log.h>
#include "iot/ha_client/ha_client.h"

#define TAG "Cover"

namespace iot {

// 这里仅定义 Cover 的属性和方法，不包含具体的实现
class Cover : public Thing {
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
    Cover() : Thing("Cover", "直播间的窗帘"), power_(false) {
        InitializeGpio();

        // 定义设备的属性
        properties_.AddBooleanProperty("power", "窗帘是否打开", [this]() -> bool {
            return power_;
        });

        // 定义设备可以被远程执行的指令
        methods_.AddMethod("TurnOn", "把窗帘拉开", ParameterList(), [this](const ParameterList& parameters) {
            power_ = true;
            // gpio_set_level(gpio_num_, 1);
            ESP_LOGI(TAG, "打开直播间窗帘");
            HaClient::GetInstance().post_services_cover_toggle("cover.lonsam_cn_1121903107_ct05_s_2", true);
        });

        methods_.AddMethod("TurnOff", "把窗帘拉上", ParameterList(), [this](const ParameterList& parameters) {
            power_ = false;
            ESP_LOGI(TAG, "关闭直播间窗帘");
            HaClient::GetInstance().post_services_cover_toggle("cover.lonsam_cn_1121903107_ct05_s_2", false);
            // gpio_set_level(gpio_num_, 0);
        });
    }
};

} // namespace iot

DECLARE_THING(Cover);
