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

#ifndef RENDER_SERVICE_BASE_PIPELINE_RS_HWC_RECORDER_H
#define RENDER_SERVICE_BASE_PIPELINE_RS_HWC_RECORDER_H

#include <bitset>

#include "common/rs_common_def.h"

namespace OHOS {
namespace Rosen {

struct RSHwcRecorder {
public:
    RSHwcRecorder() = default;
    ~RSHwcRecorder() = default;

    RSHwcRecorder(const RSHwcRecorder&) = delete;
    RSHwcRecorder(RSHwcRecorder&&) = delete;
    RSHwcRecorder& operator=(const RSHwcRecorder&) = delete;
    RSHwcRecorder& operator=(RSHwcRecorder&&) = delete;

    void SetBlendWithBackground(bool isBlendWithBackground) {
        flags_.set(BLEND_WITH_BACKGROUND, isBlendWithBackground);
    }
    bool IsBlendWithBackground() const { return flags_.test(BLEND_WITH_BACKGROUND); }
    void SetForegroundColorValid(bool isForegroundColorValid) {
        flags_.set(FOREGROUND_COLOR_VALID, isForegroundColorValid);
    }
    bool IsForegroundColorValid() const { return flags_.test(FOREGROUND_COLOR_VALID); }
    bool GetZorderChanged() const { return flags_.test(Z_ORDER_CHANGED); }
    void UpdatePositionZ(float positionZ) {
        flags_.set(Z_ORDER_CHANGED, !ROSEN_EQ(positionZ, positionZ_));
        positionZ_ = positionZ;
    }

    void SetZOrderForHwcEnableByFilter(uint32_t zOrderForHwcEnableByFilter) {
        zOrderForHwcEnableByFilter_ = zOrderForHwcEnableByFilter;
    }
    uint32_t GetZOrderForHwcEnableByFilter() const { return zOrderForHwcEnableByFilter_; }

private:
    // Bit flags for memory optimization
    enum FlagBits {
        BLEND_WITH_BACKGROUND = 0,
        FOREGROUND_COLOR_VALID = 1,
        Z_ORDER_CHANGED = 2
    };

    std::bitset<3> flags_;  // Compresses 3 bools into ~4 bytes instead of ~3 bytes + padding
    uint32_t zOrderForHwcEnableByFilter_ = 0;
    float positionZ_ = 0.0f;
};

struct RSHwcSurfaceRecorder {
public:
    RSHwcSurfaceRecorder() = default;
    ~RSHwcSurfaceRecorder() = default;

    RSHwcSurfaceRecorder(const RSHwcSurfaceRecorder&) = delete;
    RSHwcSurfaceRecorder(RSHwcSurfaceRecorder&&) = delete;
    RSHwcSurfaceRecorder& operator=(const RSHwcSurfaceRecorder&) = delete;
    RSHwcSurfaceRecorder& operator=(RSHwcSurfaceRecorder&&) = delete;

    void SetLastFrameHasVisibleRegion(bool lastFrameHasVisibleRegion) {
        lastFrameHasVisibleRegion_ = lastFrameHasVisibleRegion;
    }
    bool GetLastFrameHasVisibleRegion() const { return lastFrameHasVisibleRegion_; }
    void SetIntersectWithPreviousFilter(bool isIntersectWithPreviousFilter) {
        isIntersectWithPreviousFilter_ = isIntersectWithPreviousFilter;
    }
    bool IsIntersectWithPreviousFilter() const { return isIntersectWithPreviousFilter_; }

private:
    bool isIntersectWithPreviousFilter_ = false;
    bool lastFrameHasVisibleRegion_ = true;
};

struct RSHwcDisplayRecorder {
    RSHwcDisplayRecorder() = default;
    ~RSHwcDisplayRecorder() = default;

    RSHwcDisplayRecorder(const RSHwcDisplayRecorder&) = delete;
    RSHwcDisplayRecorder(RSHwcDisplayRecorder&&) = delete;
    RSHwcDisplayRecorder& operator=(const RSHwcDisplayRecorder&) = delete;
    RSHwcDisplayRecorder& operator=(RSHwcDisplayRecorder&&) = delete;

    bool GetNeedForceUpdateHwcNodes() const { return needForceUpdateHwcNodes_; }
    void SetNeedForceUpdateHwcNodes(bool needForceUpdateHwcNodes) {
        needForceUpdateHwcNodes_ = needForceUpdateHwcNodes;
    }
    bool HasVisibleHwcNodes() const { return hasVisibleHwcNodes_; }
    void SetHasVisibleHwcNodes(bool hasVisibleHwcNodes) { hasVisibleHwcNodes_ = hasVisibleHwcNodes; }

    bool needForceUpdateHwcNodes_ = false;
    bool hasVisibleHwcNodes_ = false;
};

} // namespace Rosen
} // namespace OHOS

#endif