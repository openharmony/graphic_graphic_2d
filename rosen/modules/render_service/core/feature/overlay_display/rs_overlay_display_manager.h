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

#ifndef RS_OVERLAY_DISPLAY_MANAGER_H
#define RS_OVERLAY_DISPLAY_MANAGER_H

#include <shared_mutex>

#include "common/rs_occlusion_region.h"
#include "pipeline/rs_dirty_region_manager.h"
#include "pipeline/rs_paint_filter_canvas.h"
#include "screen_manager/rs_screen_info.h"

namespace OHOS::Rosen {
using OverlayPostProcFunc = void (*)(RSPaintFilterCanvas&);
using OverlayExpandDirtyRegionFunc = void (*)(RSDirtyRegionManager&, const ScreenInfo&, Occlusion::Region&);

class RSOverlayDisplayManager {
public:
    static RSOverlayDisplayManager& Instance();

    void PostProcFilter(RSPaintFilterCanvas& canvas);
    void ExpandDirtyRegion(
        RSDirtyRegionManager& dirtyManager, const ScreenInfo& screenInfo, Occlusion::Region& dirtyRegion,
        Occlusion::Region& damageRegion);
    int32_t SetOverlayDisplayMode(int32_t mode);
    // pre-process before render for judge whether the mode changed
    void PreProcForRender();
    bool IsOverlayDisplayEnableForCurrentVsync();
    bool CheckStatusChanged();

private:
    RSOverlayDisplayManager() = default;
    ~RSOverlayDisplayManager() = default;

    bool LoadLibrary();
    void CloseLibrary();
    void *LoadSymbol(const std::string& symName);

    bool isStatusChanged_ = false;
    int32_t mode_ = 0;
    int32_t modeOfCurrentVsync_ = 0;
    bool isSoLoaded_ = false;
    void *soHandle_ = nullptr;
    OverlayPostProcFunc postProcFunc_ = nullptr;
    OverlayExpandDirtyRegionFunc expandDirtyRegionFunc_ = nullptr;
    mutable std::shared_mutex mutex_;
};
} // namespace OHOS::Rosen
#endif // RS_OVERLAY_DISPLAY_MANAGER_H