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

#include "rs_overlay_display_manager.h"

#include <dlfcn.h>

#include "platform/common/rs_log.h"
#ifdef RS_ENABLE_TV_PQ_METADATA
#include "feature/tv_metadata/rs_tv_metadata_manager.h"
#endif

namespace OHOS::Rosen {
const std::string OVERLAY_DISPLAY_SO_PATH = "liboverlay_display.z.so";
const std::string OVERLAY_DISPLAY_POST_PROC_HOOK = "OverlayDisplayPostProcHook";
const std::string OVERLAY_DISPLAY_EXPAND_DIRTY_REGION = "OverlayDisplayExpandDirtyRegion";

const int32_t OVERLAY_RET_FAILED = -1;
const int32_t OVERLAY_RET_SUCCESS = 0;

RSOverlayDisplayManager& RSOverlayDisplayManager::Instance()
{
    static RSOverlayDisplayManager instance;
    return instance;
}

int32_t RSOverlayDisplayManager::SetOverlayDisplayMode(int32_t mode)
{
    std::unique_lock lock(mutex_);
    if (mode == mode_) {
        return OVERLAY_RET_SUCCESS;
    }

    // Non-zero is overlay mode
    if (mode != 0) {
        if (!LoadLibrary()) {
            RS_LOGE("%{public}s: load library failed!", __func__);
            return OVERLAY_RET_FAILED;
        }
    } else {
        CloseLibrary();
    }
    mode_ = mode;
    RS_LOGI("%{public}s: mode:%{public}d", __func__, mode);
    return OVERLAY_RET_SUCCESS;
}

void RSOverlayDisplayManager::PreProcForRender()
{
    std::unique_lock lock(mutex_);
    isStatusChanged_ = false;
    if (modeOfCurrentVsync_ != mode_) {
        isStatusChanged_ = true;
        RS_LOGI("%{public}s: mode changed to %{public}d", __func__, mode_);
    }
    modeOfCurrentVsync_ = mode_;
}

bool RSOverlayDisplayManager::IsOverlayDisplayEnableForCurrentVsync()
{
    return modeOfCurrentVsync_ != 0;
}

bool RSOverlayDisplayManager::CheckStatusChanged()
{
    return isStatusChanged_;
}

void RSOverlayDisplayManager::PostProcFilter(RSPaintFilterCanvas& canvas)
{
    std::shared_lock lock(mutex_);
    if (modeOfCurrentVsync_ == 0) {
#ifdef RS_ENABLE_TV_PQ_METADATA
        RSTvMetadataManager::Instance().ResetDpPixelFormat();
#endif
        return;
    }
    if (postProcFunc_ == nullptr) {
        RS_LOGE("%{public}s function is null.", __func__);
        return;
    }
    postProcFunc_(canvas);

#ifdef RS_ENABLE_TV_PQ_METADATA
    TvPQMetadata info = { 0 };
    info.dpPixFmt = 2; // 0x2 : ARGB private frame format
    RSTvMetadataManager::Instance().RecordAndCombineMetadata(info);
#endif
}

void RSOverlayDisplayManager::ExpandDirtyRegion(
    RSDirtyRegionManager& dirtyManager, const ScreenInfo& screenInfo, Occlusion::Region& drawnRegion,
    Occlusion::Region& damageRegion)
{
    std::shared_lock lock(mutex_);
    if (modeOfCurrentVsync_ == 0) {
        return;
    }
    if (expandDirtyRegionFunc_ == nullptr) {
        RS_LOGI("%{public}s function is null.", __func__);
        return;
    }
    expandDirtyRegionFunc_(dirtyManager, screenInfo, drawnRegion);
    damageRegion = drawnRegion;
}

bool RSOverlayDisplayManager::LoadLibrary()
{
    if (isSoLoaded_) {
        return true;
    }
    dlerror();
    soHandle_ = dlopen(OVERLAY_DISPLAY_SO_PATH.c_str(), RTLD_LAZY);
    if (soHandle_ == nullptr) {
        RS_LOGE("%{public}s: dlopen failed! error = %{public}s", __func__, dlerror());
        return false;
    }
    RS_LOGI("%{public}s: dlopen success!", __func__);
    postProcFunc_ = (OverlayPostProcFunc)LoadSymbol(OVERLAY_DISPLAY_POST_PROC_HOOK);
    expandDirtyRegionFunc_ = (OverlayExpandDirtyRegionFunc)LoadSymbol(OVERLAY_DISPLAY_EXPAND_DIRTY_REGION);
    if (postProcFunc_ == nullptr || expandDirtyRegionFunc_ == nullptr) {
        CloseLibrary();
        return false;
    }
    RS_LOGI("%{public}s load func success!", __func__);
    isSoLoaded_ = true;
    return true;
}

void RSOverlayDisplayManager::CloseLibrary()
{
    postProcFunc_ = nullptr;
    expandDirtyRegionFunc_ = nullptr;
    if (soHandle_ != nullptr) {
        dlclose(soHandle_);
        soHandle_ = nullptr;
    }
    isSoLoaded_ = false;
    RS_LOGI("%{public}s done!", __func__);
}

void *RSOverlayDisplayManager::LoadSymbol(const std::string& symName)
{
    dlerror();
    void *funcSym = dlsym(soHandle_, symName.c_str());
    if (funcSym == nullptr) {
        RS_LOGE("%{public}s Get %{public}s symbol failed: %{public}s", __func__, symName.c_str(), dlerror());
        return nullptr;
    }
    return funcSym;
}
} // namespace OHOS::Rosen