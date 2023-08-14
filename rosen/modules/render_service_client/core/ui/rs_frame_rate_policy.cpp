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

#include "rs_frame_rate_policy.h"

#include <unordered_map>

#include "platform/common/rs_log.h"
#include "transaction/rs_interfaces.h"
#include "ui/rs_ui_director.h"

namespace {
    struct AnimDynamicAttribute {
        int32_t minSpeed = 0;
        int32_t maxSpeed = 0;
        int32_t preferredFps = 0;
    };
    static std::unordered_map<std::string, std::unordered_map<std::string,
        AnimDynamicAttribute>> animAttributes;
}

namespace OHOS {
namespace Rosen {
RSFrameRatePolicy* RSFrameRatePolicy::GetInstance()
{
    static RSFrameRatePolicy instance;
    return &instance;
}

RSFrameRatePolicy::RSFrameRatePolicy()
{
}

RSFrameRatePolicy::~RSFrameRatePolicy()
{
    animAttributes.clear();
}

void RSFrameRatePolicy::RegisterHgmConfigChangeCallback()
{
    auto callback = std::bind(&RSFrameRatePolicy::HgmConfigChangeCallback, this,
        std::placeholders::_1);
    if (RSInterfaces::GetInstance().RegisterHgmConfigChangeCallback(callback) != NO_ERROR) {
        ROSEN_LOGE("RegisterHgmConfigChangeCallback failed");
    }
}

void RSFrameRatePolicy::HgmConfigChangeCallback(std::shared_ptr<RSHgmConfigData> configData)
{
    if (configData == nullptr) {
        ROSEN_LOGE("RSFrameRatePolicy configData is null");
        return;
    }

    auto data = configData->GetConfigData();
    RSUIDirector::PostFrameRateTask([data]() {
        for (auto item : data) {
            animAttributes[item.animType][item.animName] = {item.minSpeed, item.maxSpeed, item.preferredFps};
            ROSEN_LOGD("RSFrameRatePolicy: config item type = %s, name = %s, minSpeed = %d, maxSpeed = %d, \
                preferredFps = %d", item.animType.c_str(), item.animName.c_str(), static_cast<int>(item.minSpeed),
                static_cast<int>(item.maxSpeed), static_cast<int>(item.preferredFps));
        }
    });
}

int RSFrameRatePolicy::GetPreferredFps(const std::string& scene, float speed)
{
    auto& attributes = animAttributes[scene];
    for (auto attribute : attributes) {
        // if speed is -1, it means infinity
        if (speed >= attribute.second.minSpeed && (speed <= attribute.second.maxSpeed ||
            attribute.second.maxSpeed == -1)) {
            return attribute.second.preferredFps;
        }
    }
    return 0;
}
} // namespace Rosen
} // namespace OHOS