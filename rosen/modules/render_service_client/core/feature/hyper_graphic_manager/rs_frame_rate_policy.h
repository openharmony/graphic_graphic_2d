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
#ifndef RENDER_SERVICE_CLIENT_CORE_UI_RS_FRAME_RATE_POLICY_H
#define RENDER_SERVICE_CLIENT_CORE_UI_RS_FRAME_RATE_POLICY_H

#include <chrono>
#include <mutex>
#include <unordered_map>
#include <memory>
#include "transaction/rs_hgm_config_data.h"

namespace OHOS {
namespace Rosen {
enum class RSPropertyUnit : int16_t;

enum TouchStatus : uint32_t {
    TOUCH_CANCEL = 1,
    TOUCH_DOWN = 2,
    TOUCH_MOVE = 3,
    TOUCH_UP = 4,
    TOUCH_BUTTON_DOWN = 8,
    TOUCH_BUTTON_UP = 9,
    TOUCH_PULL_DOWN = 12,
    TOUCH_PULL_MOVE = 13,
    TOUCH_PULL_UP = 14,
};

struct AnimDynamicAttribute {
    int32_t minSpeed = 0;
    int32_t maxSpeed = 0;
    int32_t preferredFps = 0;
};

class RSFrameRatePolicy {
public:
    static RSFrameRatePolicy* GetInstance();

    void RegisterHgmConfigChangeCallback();

    int32_t GetPreferredFps(const std::string& scene, float speed);
    int32_t GetRefreshRateModeName() const;
    int32_t GetExpectedFrameRate(const RSPropertyUnit unit, float velocity);
    bool GetTouchOrPointerAction(int32_t pointerAction);

    const std::unordered_set<std::string>& GetPageNameList() const;
private:
    RSFrameRatePolicy() = default;
    ~RSFrameRatePolicy();

    void HgmConfigChangeCallback(std::shared_ptr<RSHgmConfigData> configData);
    void HgmRefreshRateModeChangeCallback(int32_t refreshRateMode);

    float ppi_ = 1.0f;
    float xDpi_ = 1.0f;
    float yDpi_ = 1.0f;
    std::unordered_set<std::string> pageNameList_;
    int32_t currentRefreshRateModeName_ = -1;
    std::unordered_map<std::string, std::unordered_map<std::string, AnimDynamicAttribute>> animAttributes_;
    std::mutex mutex_;
    std::chrono::steady_clock::time_point sendMoveTime_ = std::chrono::steady_clock::now();
};
} // namespace Rosen
} // namespace OHOS
#endif // RENDER_SERVICE_CLIENT_CORE_UI_RS_FRAME_RATE_POLICY_H
