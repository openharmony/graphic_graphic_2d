/*
 * Copyright (c) 2025 Huawei Device Co., Ltd.
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

#ifndef RP_FRAME_RATE_POLICY_H
#define RP_FRAME_RATE_POLICY_H

#include <chrono>
#include <map>
#include <memory>
#include <mutex>
#include <unordered_map>
#include "transaction/rp_hgm_config_data.h"

namespace OHOS {
namespace Rosen {
enum class RSPropertyUnit : uint8_t;

struct AnimDynamicAttribute {
    int32_t minSpeed = 0;
    int32_t maxSpeed = 0;
    int32_t preferredFps = 0;
};

class RPFrameRatePolicy {
public:
    RPFrameRatePolicy() = default;
    ~RPFrameRatePolicy();
    int32_t GetExpectedFrameRate(const RSPropertyUnit unit, float velocityPx, int32_t areaPx, int32_t lengthPx) const;
    void HgmConfigUpdateCallback(std::shared_ptr<RPHgmConfigData> configData);
private:
    int32_t GetPreferredFps(const std::string& type, float velocityMM, float areaSqrMM, float lengthMM) const;
    template<typename T>
    float PixelToMM(T pixel) const;
    template<typename T>
    float SqrPixelToSqrMM(T sqrPixel) const;
    void Reset()
    {
        smallSizeArea_ = -1;
        smallSizeLength_ = -1;
        ppi_ = 1.0f;
        xDpi_ = 1.0f;
        yDpi_ = 1.0f;
        animAttributes_.clear();
        smallSizeAnimAttributes_.clear();
    }
    int32_t smallSizeArea_ = -1;
    int32_t smallSizeLength_ = -1;
    float ppi_ = 1.0f;
    float xDpi_ = 1.0f;
    float yDpi_ = 1.0f;
    std::unordered_map<std::string, std::unordered_map<std::string, AnimDynamicAttribute>> animAttributes_;
    std::unordered_map<std::string, std::unordered_map<std::string, AnimDynamicAttribute>> smallSizeAnimAttributes_;
};
} // namespace Rosen
} // namespace OHOS
#endif // RP_FRAME_RATE_POLICY_H