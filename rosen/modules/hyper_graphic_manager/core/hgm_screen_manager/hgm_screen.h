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

#ifndef HGM_SCREEN_H
#define HGM_SCREEN_H

#include <cstdint>
#include <memory>
#include <mutex>
#include <unistd.h>
#include <unordered_map>
#include <unordered_set>
#include <vector>

#include "refbase.h"
#include "screen_manager/screen_types.h"
#include "screen_manager/rs_screen_mode_info.h"

#include "hgm_command.h"

namespace OHOS::Rosen {
constexpr float INCH_2_MM = 25.4f;

struct ScreenSize {
    int32_t width;
    int32_t height;
    int32_t phyWidth;
    int32_t phyHeight;
};

class HgmScreen : public virtual RefBase {
public:
    HgmScreen();
    HgmScreen(ScreenId id, int32_t mode, ScreenSize& screenSize);
    virtual ~HgmScreen();

    ScreenId GetId() const
    {
        return id_;
    }

    uint32_t GetActiveMode() const
    {
        return activeModeId_;
    }

    std::unordered_set<uint32_t> GetSupportedRates() const
    {
        return supportedRefreshRates_;
    }

    bool IsSupportRate(uint32_t rate) const
    {
        return supportedRefreshRates_.find(rate) != supportedRefreshRates_.end() ? true : false;
    }

    int32_t GetActiveRefreshRateMode() const
    {
        return customFrameRateMode_;
    }

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

    uint32_t GetActiveRefreshRate() const;
    int32_t SetActiveRefreshRate(int32_t sceneId, uint32_t rate);
    int32_t SetRateAndResolution(int32_t sceneId, uint32_t rate, int32_t width, int32_t height);
    int32_t SetRefreshRateRange(uint32_t minRate, uint32_t maxRate);
    int32_t AddScreenModeInfo(int32_t width, int32_t height, uint32_t rate, int32_t modeId);

private:
    class ScreenProfile {
    public:
        ScreenProfile(int32_t width, int32_t height, uint32_t rate, int32_t modeId)
            : width_(width), height_(height), refreshrate_(rate), modeId_(modeId) {}

        ~ScreenProfile() = default;

        bool operator==(const ScreenProfile& rValue) const
        {
            if (rValue.GetWidth() != width_ || rValue.GetHeight() != height_ ||
                rValue.GetRate() != refreshrate_) {
                return false;
            }
            return true;
        }

        int32_t GetWidth() const
        {
            return width_;
        }

        int32_t GetHeight() const
        {
            return height_;
        }

        uint32_t GetRate() const
        {
            return refreshrate_;
        }

        int32_t GetModeId() const
        {
            return modeId_;
        }

    private:
        int32_t width_ = -1;
        int32_t height_ = -1;
        uint32_t refreshrate_ = 0;
        int32_t modeId_ = -1;
    };

    ScreenId id_ = 0;
    int32_t activeModeId_ = 0;
    int32_t width_ = 0;
    int32_t height_ = 0;
    int32_t phyWidth_ = 0;
    int32_t phyHeight_ = 0;
    float ppi_ = 0;
    float xDpi_ = 0;
    float yDpi_ = 0;
    int32_t customFrameRateMode_ = -1;
    std::unordered_set<uint32_t> supportedRefreshRates_;
    std::unordered_set<int32_t> supportedModeIds_;
    std::vector<std::shared_ptr<ScreenProfile>> screenModeInfos_;
    std::mutex baseMutex_;

    void SetActiveModeId(int32_t modeId);
    std::shared_ptr<ScreenProfile> GetModeViaId(int32_t id) const;
    bool IfSwitchToRate(int32_t sceneId, uint32_t rate) const;
    int32_t GetModeIdViaRate(uint32_t rate) const;
    int32_t GetModeIdViaResolutionAndRate(int32_t width, int32_t height, uint32_t rate) const;
    static constexpr uint32_t RATE_NOT_SUPPORTED = 0;

    // defined by xml configuration, participate in refresh rate decision system
    uint32_t minRefreshRateRange_ = 1;
    uint32_t maxRefreshRateRange_ = 120;
};
} // namespace Ohos::Rosen
#endif // HGM_SCREEN_H