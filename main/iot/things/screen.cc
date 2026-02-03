#include "iot/thing.h"
#include "board.h"
#include "display/lcd_display.h"
#include "settings.h"
#include "application.h"

#include <esp_log.h>
#include <string>

#define TAG "Screen"

namespace iot {

// 这里仅定义 Screen 的属性和方法，不包含具体的实现
class Screen : public Thing {
public:
    Screen() : Thing("Screen", "A screen that can set theme and brightness") {
        properties_.AddStringProperty("mode", "Current mode", [this]() -> std ::string {
            auto mode = Board::GetInstance().GetDisplay()->GetMode();
            return mode;
            
        });

        properties_.AddNumberProperty("brightness", "Current brightness percentage", [this]() -> int {
            // 这里可以添加获取当前亮度的逻辑
            auto backlight = Board::GetInstance().GetBacklight();
            return backlight ? backlight->brightness() : 100;
        });

        // 定义设备可以被远程执行的指令
        methods_.AddMethod("set_mode", "Set the screen mode", ParameterList({
            Parameter("mode_name", "Valid string values are 'normal' (文字模式) and 'pet' (宠物模式)", kValueTypeString, true)
        }), [this](const ParameterList& parameters) {
            std::string mode_name = static_cast<std::string>(parameters["mode_name"].string());
            auto display = Board::GetInstance().GetDisplay();
            if (display) {
                if (mode_name == "pet" || mode_name == "PET") {
                    auto& app = Application::GetInstance();
                    app.SetAecMode(kAecOff);
            
                }
                display->SetMode(mode_name);
            }
        });

        
        methods_.AddMethod("set_brightness", "Set the brightness", ParameterList({
            Parameter("brightness", "An integer between 0 and 100", kValueTypeNumber, true)
        }), [this](const ParameterList& parameters) {
            uint8_t brightness = static_cast<uint8_t>(parameters["brightness"].number());
            auto backlight = Board::GetInstance().GetBacklight();
            if (backlight) {
                backlight->SetBrightness(brightness, true);
            }
        });
    }
};

} // namespace iot

DECLARE_THING(Screen);