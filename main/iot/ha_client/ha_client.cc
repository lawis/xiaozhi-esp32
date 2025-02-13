#include "ha_client.h"

#include <esp_log.h>

#define TAG "HaClient"

HaClient::HaClient(const std::string& server_url, const std::string& access_token) {
    _http_client = new EspHttp();
    _http_client->SetHeader("Content-Type", "application/json");
    _http_client->SetHeader("Authorization", "Bearer " + access_token);
}

HaClient::~HaClient() {

}

// bool HaClient::send_event(const std::string& event_type, const std::string& event_data)
// {
//     std::string url = HA_URL + event_type;
//     std::string payload = "{\"event_type\":\"" + event_type + "\",\"event_data\":" + event_data + "}";
//     int status_code = _http_client->Post(url.c_str(), payload.c_str());
// }
bool HaClient::post_services_switch_toggle(const std::string& entity_id, const bool on_off)
{
    std::string url = HA_URL;
    url += "services/switch/turn_";
    url += (on_off? "on" : "off");
    std::string payload = "{\"entity_id\":\"" + entity_id + "\"}";

    if (!_http_client->Open("POST", url, payload)) {
        ESP_LOGI(TAG, "Failed to send HTTP post request");
        return false;
    }
    auto response = _http_client->GetBody();
    _http_client->Close();
    return true;
}

bool HaClient::post_services_light_toggle(const std::string& entity_id, const bool on_off)
{
    std::string url = HA_URL;
    url += "services/light/turn_";
    url += (on_off? "on" : "off");
    std::string payload = "{\"entity_id\":\"" + entity_id + "\"}";

    if (!_http_client->Open("POST", url, payload)) {
        ESP_LOGI(TAG, "Failed to send HTTP post request");
        return false;
    }
    auto response = _http_client->GetBody();
    _http_client->Close();
    return true;
}


bool HaClient::post_services_cover_toggle(const std::string& entity_id, const bool on_off)
{
    std::string url = HA_URL;
    url += "services/cover/";
    url += (on_off? "open_cover" : "close_cover");
    std::string payload = "{\"entity_id\":\"" + entity_id + "\"}";

    if (!_http_client->Open("POST", url, payload)) {
        ESP_LOGI(TAG, "Failed to send HTTP post request");
        return false;
    }
    auto response = _http_client->GetBody();
    _http_client->Close();
    return true;
}

bool HaClient::post_services_vacuum_start(const std::string& entity_id)
{
    std::string url = HA_URL;
    url += "services/vacuum/start";
    std::string payload = "{\"entity_id\":\"" + entity_id + "\"}";

    if (!_http_client->Open("POST", url, payload)) {
        ESP_LOGI(TAG, "Failed to send HTTP post request");
        return false;
    }
    auto response = _http_client->GetBody();
    _http_client->Close();
    return true;
}

bool HaClient::post_services_vacuum_stop(const std::string& entity_id)
{
    std::string url = HA_URL;
    url += "services/vacuum/stop";
    std::string payload = "{\"entity_id\":\"" + entity_id + "\"}";

    if (!_http_client->Open("POST", url, payload)) {
        ESP_LOGI(TAG, "Failed to send HTTP post request");
        return false;
    }
    auto response = _http_client->GetBody();
    _http_client->Close();
    return true;
}

bool HaClient::post_services_vacuum_return_to_base(const std::string& entity_id)
{
    std::string url = HA_URL;
    url += "services/vacuum/return_to_base";
    std::string payload = "{\"entity_id\":\"" + entity_id + "\"}";

    if (!_http_client->Open("POST", url, payload)) {
        ESP_LOGI(TAG, "Failed to send HTTP post request");
        return false;
    }
    auto response = _http_client->GetBody();
    _http_client->Close();
    return true;
}

bool HaClient::post_services_vacuum_locate(const std::string& entity_id)
{
    std::string url = HA_URL;
    url += "services/vacuum/locate";
    std::string payload = "{\"entity_id\":\"" + entity_id + "\"}";

    if (!_http_client->Open("POST", url, payload)) {
        ESP_LOGI(TAG, "Failed to send HTTP post request");
        return false;
    }
    auto response = _http_client->GetBody();
    _http_client->Close();
    return true;
}   

