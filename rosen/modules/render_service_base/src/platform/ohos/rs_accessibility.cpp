/*
 * Copyright (c) 2023 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "platform/common/rs_accessibility.h"

#include "accessibility_config.h"
#include "platform/common/rs_log.h"

using namespace OHOS::AccessibilityConfig;

namespace OHOS {
namespace Rosen {
class RSAccessibilityOHOS : public RSAccessibility {
public:
    static RSAccessibilityOHOS &GetInstance();
    void ListenHighContrastChange(OnHighContrastChange callback) override;

private:
    class Observer : public AccessibilityConfigObserver {
    public:
        virtual void OnConfigChanged(const CONFIG_ID id, const ConfigValue &value) override;
    };

    void ListenHighContrast();

    std::shared_ptr<Observer> observer_ = std::make_shared<Observer>();
    bool listeningHighContrast_ = false;
    OnHighContrastChange onHighContrastChange_ = nullptr;
};

RSAccessibilityOHOS &RSAccessibilityOHOS::GetInstance()
{
    static RSAccessibilityOHOS instance;
    return instance;
}

RSAccessibility &RSAccessibility::GetInstance()
{
    return RSAccessibilityOHOS::GetInstance();
}

void RSAccessibilityOHOS::ListenHighContrastChange(OnHighContrastChange callback)
{
    onHighContrastChange_ = callback;
    ListenHighContrast();
}

void RSAccessibilityOHOS::ListenHighContrast()
{
    if (!listeningHighContrast_) {
        auto &config = OHOS::AccessibilityConfig::AccessibilityConfig::GetInstance();
        config.InitializeContext();
        config.SubscribeConfigObserver(CONFIG_ID::CONFIG_HIGH_CONTRAST_TEXT, observer_);
        listeningHighContrast_ = true;
    }
}

void RSAccessibilityOHOS::Observer::OnConfigChanged(const CONFIG_ID id, const ConfigValue &value)
{
    ROSEN_LOGD("OnConfigChanged %d", static_cast<int>(id));
    auto &accessbility = RSAccessibilityOHOS::GetInstance();
    if (id == CONFIG_ID::CONFIG_HIGH_CONTRAST_TEXT && accessbility.onHighContrastChange_) {
        accessbility.onHighContrastChange_(value.highContrastText);
    }
}
} // namespace Rosen
} // namespace OHOS
