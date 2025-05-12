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

namespace OHOS {
namespace Rosen {

struct RSHwcRecorder {
    RSHwcRecorder() = default;
    ~RSHwcRecorder() = default;

    RSHwcRecorder(const RSHwcRecorder&) = delete;
    RSHwcRecorder(RSHwcRecorder&&) = delete;
    RSHwcRecorder& operator=(const RSHwcRecorder&) = delete;
    RSHwcRecorder& operator=(RSHwcRecorder&&) = delete;

    void SetBlendWithBackground(bool isBlendWithBackground) { isBlendWithBackground_ = isBlendWithBackground; }
    bool IsBlendWithBackground() const { return isBlendWithBackground_; }

    void SetZOrderForHwcEnableByFilter(int32_t zOrderForHwcEnableByFilter)
    {
        zOrderForHwcEnableByFilter_ = zOrderForHwcEnableByFilter;
    }
    int32_t GetZOrderForHwcEnableByFilter() const { return zOrderForHwcEnableByFilter_; }

    bool isBlendWithBackground_ = false;

    int32_t zOrderForHwcEnableByFilter_ = 0;
};

struct RSHwcSurfaceRecorder {
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