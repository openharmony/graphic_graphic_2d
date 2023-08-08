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

#ifndef HGM_FRAME_RATE_TOOL_H
#define HGM_FRAME_RATE_TOOL_H

#include <cstdint>
#include <memory>
#include <mutex>
#include <cmath>
#include <refbase.h>
#include <unordered_map>

#include "screen_manager/screen_types.h"
#include "animation/rs_frame_rate_range.h"

#include "hgm_command.h"

namespace OHOS::Rosen {
constexpr float INCH_2_MM = 25.4f;

class HgmFrameRateTool final {
public:
    static std::shared_ptr<HgmFrameRateTool> GetInstance();

    ~HgmFrameRateTool() noexcept;
    // noncopyable
    HgmFrameRateTool(const HgmFrameRateTool &) = delete;
    HgmFrameRateTool &operator=(const HgmFrameRateTool &) = delete;
    int32_t AddScreenProfile(ScreenId id, int32_t width, int32_t height, int32_t phyWidth, int32_t phyHeight);
    int32_t RemoveScreenProfile(ScreenId id);
    int32_t CalModifierPreferred(
        ScreenId id, HgmModifierProfile &hgmModifierProfile, std::shared_ptr<ParsedConfigData> parsedConfigData) const;
private:
    class ScreenProfile {
    public:
        ScreenProfile(ScreenId id, int32_t width, int32_t height, int32_t phyWidth, int32_t phyHeight)
            : id_(id), width_(width), height_(height), phyWidth_(phyWidth), phyHeight_(phyHeight)
        {
            auto screenSize = sqrt(pow(width, 2) + pow(height, 2));
            auto phyScreenSize = sqrt(pow(phyWidth, 2) + pow(phyHeight, 2));
            ppi_ = screenSize / (phyScreenSize / INCH_2_MM);
            xDpi_ = width / (phyWidth / INCH_2_MM);
            yDpi_ = height / (phyHeight / INCH_2_MM);
        }

        ~ScreenProfile() = default;

        int32_t GetWidth() const
        {
            return width_;
        }

        int32_t GetHeight() const
        {
            return height_;
        }

        int32_t GetPhyWidth() const
        {
            return phyWidth_;
        }

        int32_t GetPhyHeight() const
        {
            return phyHeight_;
        }

        float GetPpi() const
        {
            return ppi_;
        }

        float GetXDpi() const
        {
            return xDpi_;
        }

        float GetYDpi() const
        {
            return yDpi_;
        }

        ScreenId GetId() const
        {
            return id_;
        }

    private:
        ScreenId id_;
        int32_t width_;
        int32_t height_;
        int32_t phyWidth_;
        int32_t phyHeight_;
        float ppi_;
        float xDpi_;
        float yDpi_;
    };

    std::vector<std::shared_ptr<ScreenProfile>> screenProfileList_;
    HgmFrameRateTool();
    std::shared_ptr<ScreenProfile> GetScreenProfile(ScreenId id) const;
    static std::pair<float, float> applyDimension(
        SpeedTransType speedTransType, float xSpeed, float ySpeed, std::shared_ptr<ScreenProfile> screenProfile);
    mutable std::mutex listMutex_;
};
} // namespace OHOS::Rosen
#endif // HGM_FRAME_RATE