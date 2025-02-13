#ifndef _HA_CLIENT_H_
#define _HA_CLIENT_H_



#include <esp_http.h>
#include <string>


#define HA_URL "http://192.168.13.227:8123/api/"
#define HA_ACCESS_TOKEN "eyJhbGciOiJIUzI1NiIsInR5cCI6IkpXVCJ9.eyJpc3MiOiI1MzkwNTNlOGQyZmY0MTM3OTU5MGUyMzM3MDBlMTk0YSIsImlhdCI6MTczNjMzMDk3NSwiZXhwIjoyMDUxNjkwOTc1fQ.aHOeJQZkCruKFkD9VGWfqLrobZbTAxKJldwa-k7iazc"

class HaClient {

public:

    static HaClient& GetInstance()
    {
        static HaClient instance(HA_URL,HA_ACCESS_TOKEN);
        return instance;
    }

    HaClient(const std::string& server_url, const std::string& access_token);
    ~HaClient();

    bool post_services_switch_toggle(const std::string& entity_id, const bool on_off);

    bool post_services_light_toggle(const std::string& entity_id, const bool on_off);

    bool post_services_cover_toggle(const std::string& entity_id, const bool on_off);

    bool post_services_vacuum_start(const std::string& entity_id);

    bool post_services_vacuum_stop(const std::string& entity_id);

    bool post_services_vacuum_return_to_base(const std::string& entity_id);

    bool post_services_vacuum_locate(const std::string& entity_id);

private:

    EspHttp* _http_client;

};



#endif // _HA_CLIENT_H_ 