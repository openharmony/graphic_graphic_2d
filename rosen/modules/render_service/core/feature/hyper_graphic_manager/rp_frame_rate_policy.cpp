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
#include "feature/hyper_graphic_manager/rp_frame_rate_policy.h"

#include "common/rs_optional_trace.h"
#include "modifier/rs_render_property.h"
#include "platform/common/rs_log.h"

namespace OHOS {
namespace Rosen {
namespace {
constexpr float INCH_2_MM = 25.4f;
}

void RPFrameRatePolicy::HgmConfigUpdateCallback(std::shared_ptr<RPHgmConfigData> configData)
{
    Reset();
    if (configData == nullptr) {
        ROSEN_LOGE("%{public}s configData is null", __func__);
        return;
    }

    auto& data = configData->GetConfigData();
    auto& smallSizeData = configData->GetSmallSizeConfigData();
    if (data.empty() && smallSizeData.empty()) {
        return;
    }

    smallSizeArea_ = configData->GetSmallSizeArea();
    smallSizeLength_ = configData->GetSmallSizeLength();
    ppi_ = configData->GetPpi();
    xDpi_ = configData->GetXDpi();
    yDpi_ = configData->GetYDpi();
    ROSEN_LOGD("%{public}s ppi %{public}f xDpi %{public}f yDpi %{public}f", __func__, ppi_, xDpi_, yDpi_);
    for (auto& item : data) {
        if (item.animType.empty() || item.animName.empty()) {
            Reset();
            return;
        }
        animAttributes_[item.animType][item.animName] = {item.minSpeed, item.maxSpeed, item.preferredFps};
        ROSEN_LOGD("%{public}s config item type = %{public}s, name = %{public}s, "
            "minSpeed = %{public}d, maxSpeed = %{public}d, preferredFps = %{public}d",
            __func__, item.animType.c_str(), item.animName.c_str(), static_cast<int>(item.minSpeed),
            static_cast<int>(item.maxSpeed), static_cast<int>(item.preferredFps));
    }
    for (auto& item : smallSizeData) {
        if (item.animType.empty() || item.animName.empty()) {
            Reset();
            return;
        }
        smallSizeAnimAttributes_[item.animType][item.animName] = {item.minSpeed, item.maxSpeed, item.preferredFps};
        ROSEN_LOGD("%{public}s small config item type = %{public}s, name= %{public}s, "
            "minSpeed = %{public}d, maxSpeed = %{public}d, preferredFps = %{public}d",
            __func__, item.animType.c_str(), item.animName.c_str(), static_cast<int>(item.minSpeed),
            static_cast<int>(item.maxSpeed), static_cast<int>(item.preferredFps));
    }
}

int32_t RPFrameRatePolicy::GetExpectedFrameRate(const RSPropertyUnit unit, float velocityPx,
    int32_t areaPx, int32_t lengthPx) const
{
    static const std::map<RSPropertyUnit, std::string> typeMap = {
        {RSPropertyUnit::PIXEL_POSITION, "translate"},
        {RSPropertyUnit::PIXEL_SIZE, "scale"},
        {RSPropertyUnit::RATIO_SCALE, "scale"},
        {RSPropertyUnit::ANGLE_ROTATION, "rotation"}
    };
    if (auto it = typeMap.find(unit); it != typeMap.end()) {
        return GetPreferredFps(it->second, PixelToMM(velocityPx), SqrPixelToSqrMM(areaPx), PixelToMM(lengthPx));
    }
    return 0;
}

int32_t RPFrameRatePolicy::GetPreferredFps(const std::string& type, float velocityMM,
    float areaSqrMM, float lengthMM) const
{
    static bool isBeta = RSSystemProperties::GetVersionType() == "beta";
    if (isBeta) {
        RS_TRACE_NAME_FMT("GetPreferredFps: type: %s, speed: %f, area: %f, length: %f",
            type.c_str(), velocityMM, areaSqrMM, lengthMM);
    }
    if ((animAttributes_.count(type) == 0 && smallSizeAnimAttributes_.count(type) == 0) || ROSEN_EQ(velocityMM, 0.f)) {
        return 0;
    }
    auto matchFunc = [velocityMM](const auto& pair) {
        return velocityMM >= pair.second.minSpeed && (velocityMM < pair.second.maxSpeed || pair.second.maxSpeed == -1);
    };
    // find result if it's small size animation
    bool needCheck = smallSizeArea_ > 0 && smallSizeLength_ > 0;
    bool matchArea = areaSqrMM > 0 && areaSqrMM < smallSizeArea_;
    bool matchLength = lengthMM > 0 && lengthMM < smallSizeLength_;
    if (needCheck && matchArea && matchLength) {
        if (auto it = smallSizeAnimAttributes_.find(type); it != smallSizeAnimAttributes_.end()) {
            const auto& config = smallSizeAnimAttributes_.at(type);
            if (auto iter = std::find_if(config.begin(), config.end(), matchFunc);
                iter != config.end()) {
                RS_OPTIONAL_TRACE_NAME_FMT("GetPreferredFps (small size): type: %s, speed: %f, area: %f, length: %f,"
                    "rate: %d", type.c_str(), velocityMM, areaSqrMM, lengthMM, iter->second.preferredFps);
                return iter->second.preferredFps;
            }
        }
    }

    // it's not a small size animation or current small size config don't cover it, find result in normal config
    if (auto it = animAttributes_.find(type); it != animAttributes_.end()) {
        const auto& config = animAttributes_.at(type);
        if (auto iter = std::find_if(config.begin(), config.end(), matchFunc); iter != config.end()) {
            RS_OPTIONAL_TRACE_NAME_FMT("GetPreferredFps: type: %s, speed: %f, area: %f, length: %f, rate: %d",
                type.c_str(), velocityMM, areaSqrMM, lengthMM, iter->second.preferredFps);
            return iter->second.preferredFps;
        }
    }
    return 0;
}

template<typename T>
float RPFrameRatePolicy::PixelToMM(T pixel) const
{
    if (ppi_ > 0.f) {
        return pixel / ppi_ * INCH_2_MM;
    }
    return 0.f;
}

template<typename T>
float RPFrameRatePolicy::SqrPixelToSqrMM(T sqrPixel) const
{
    return PixelToMM(PixelToMM(sqrPixel));
}


void RPFrameRatePolicy::Reset()
{
    smallSizeArea_ = -1;
    smallSizeLength_ = -1;
    ppi_ = 1.0f;
    xDpi_ = 1.0f;
    yDpi_ = 1.0f;
    animAttributes_.clear();
    smallSizeAnimAttributes_.clear();
}
} // namespace Rosen
} // namespace OHOS