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

namespace OHOS {
namespace Rosen {
namespace {
constexpr float INCH_2_MM = 25.4f;
struct AnimDynamicAttribute {
    int32_t minSpeed = 0;
    int32_t maxSpeed = 0;
    int32_t preferredFps = 0;
};
static std::unordered_map<std::string, std::unordered_map<std::string,
    AnimDynamicAttribute>> animAttributes;
}

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

    auto refreshRateModeChangeCallback = std::bind(&RSFrameRatePolicy::HgmRefreshRateModeChangeCallback, this,
        std::placeholders::_1);
    if (RSInterfaces::GetInstance().RegisterHgmRefreshRateModeChangeCallback(
        refreshRateModeChangeCallback) != NO_ERROR) {
        ROSEN_LOGE("RegisterHgmRefreshRateModeChangeCallback failed");
    }
}

void RSFrameRatePolicy::HgmConfigChangeCallback(std::shared_ptr<RSHgmConfigData> configData)
{
    if (configData == nullptr) {
        ROSEN_LOGE("RSFrameRatePolicy configData is null");
        return;
    }

    auto data = configData->GetConfigData();
    if (data.empty()) {
        return;
    }
    auto ppi = configData->GetPpi();
    auto xDpi = configData->GetXDpi();
    auto yDpi = configData->GetYDpi();
    RSUIDirector::PostFrameRateTask([this, data, ppi, xDpi, yDpi]() {
        for (auto item : data) {
            if (item.animType.empty() || item.animName.empty()) {
                return;
            }
            animAttributes[item.animType][item.animName] = {item.minSpeed, item.maxSpeed, item.preferredFps};
            ROSEN_LOGD("RSFrameRatePolicy: config item type = %{public}s, name = %{public}s, "\
                "minSpeed = %{public}d, maxSpeed = %{public}d, preferredFps = %{public}d",
                item.animType.c_str(), item.animName.c_str(), static_cast<int>(item.minSpeed),
                static_cast<int>(item.maxSpeed), static_cast<int>(item.preferredFps));
        }
        ppi_ = ppi;
        xDpi_ = xDpi;
        yDpi_ = yDpi;
    });
}

void RSFrameRatePolicy::HgmRefreshRateModeChangeCallback(int32_t refreshRateMode)
{
    SetRefreshRateMode(refreshRateMode);
}

void RSFrameRatePolicy::SetRefreshRateMode(int32_t refreshRateMode)
{
    currentRefreshRateMode_ = refreshRateMode;
}

int32_t RSFrameRatePolicy::GetRefreshRateMode()
{
    return currentRefreshRateMode_;
}

int RSFrameRatePolicy::GetPreferredFps(const std::string& scene, float speed)
{
    if (animAttributes.count(scene) == 0 || ppi_ == 0) {
        return 0;
    }
    float speedMM = speed / ppi_ * INCH_2_MM;
    const auto& attributes = animAttributes[scene];
    auto iter = std::find_if(attributes.begin(), attributes.end(), [&speedMM](const auto& pair) {
        return speedMM >= pair.second.minSpeed && (speedMM < pair.second.maxSpeed ||
            pair.second.maxSpeed == -1);
    });
    if (iter != attributes.end()) {
        return iter->second.preferredFps;
    }
    return 0;
}
} // namespace Rosen
} // namespace OHOS