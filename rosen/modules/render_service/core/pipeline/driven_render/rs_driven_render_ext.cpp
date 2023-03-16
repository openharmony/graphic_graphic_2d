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

#include "rs_driven_render_ext.h"
#include <dlfcn.h>

namespace OHOS {
namespace Rosen {
// If need to modify libdriven_render together, update the version to keep both sides in sync.
static const std::string DRIVEN_RENDER_VERSION = "1.0.0";

RSDrivenRenderExt& RSDrivenRenderExt::Instance()
{
    static RSDrivenRenderExt instance;
    return instance;
}

RSDrivenRenderExt::~RSDrivenRenderExt()
{
    CloseDrivenRenderExt();
}

bool RSDrivenRenderExt::OpenDrivenRenderExt()
{
    drivenRenderExtHandle_ = dlopen("libdriven_render.z.so", RTLD_NOW);
    if (!drivenRenderExtHandle_) {
        RS_LOGW("RSDrivenRenderExt: dlopen libdriven_render.z.so failed!");
        return false;
    }
    if (!DrivenRenderVersionMatched()) {
        CloseDrivenRenderExt();
        RS_LOGW("RSDrivenRenderExt: version not matched!");
        return false;
    }
    drivenRenderExtLoaded_ = LoadDrivenRenderFunc();
    if (!drivenRenderExtLoaded_) {
        CloseDrivenRenderExt();
        RS_LOGW("RSDrivenRenderExt: load driven render func failed!");
    }
    return drivenRenderExtLoaded_;
}

bool RSDrivenRenderExt::DrivenRenderVersionMatched()
{
    void* versionMatchedFunc = dlsym(drivenRenderExtHandle_, "DrivenRenderVersionMatched");
    if (!versionMatchedFunc) {
        RS_LOGW("RSDrivenRenderExt: load version func failed!");
        return false;
    }
    // If need to modify the definition of this interface,
    // init RSDrivenRenderManager as disabled until libdriven_render updated.
    auto versionMatched = reinterpret_cast<bool (*)(const std::string&)>(versionMatchedFunc);
    return versionMatched(DRIVEN_RENDER_VERSION);
}

bool RSDrivenRenderExt::LoadDrivenRenderFunc()
{
    disabledRenderModeFunc_ = dlsym(drivenRenderExtHandle_, "DisabledRenderMode");
    setCurrFrameBoundsFunc_ = dlsym(drivenRenderExtHandle_, "SetCurrFrameBounds");
    updateActivateFrameStateFunc_ = dlsym(drivenRenderExtHandle_, "UpdateActivateFrameState");
    isValidSurfaceNameFunc_ = dlsym(drivenRenderExtHandle_, "IsValidSurfaceName");

    bool success =
        disabledRenderModeFunc_ && setCurrFrameBoundsFunc_ && updateActivateFrameStateFunc_ && isValidSurfaceNameFunc_;
    if (!success) {
        RS_LOGW("RSDrivenRenderExt: load driven render func failed!");
    }
    return success;
}

void RSDrivenRenderExt::CloseDrivenRenderExt()
{
    if (drivenRenderExtHandle_) {
        dlclose(drivenRenderExtHandle_);
    }
    drivenRenderExtLoaded_ = false;
    drivenRenderExtHandle_ = nullptr;

    disabledRenderModeFunc_ = nullptr;
    setCurrFrameBoundsFunc_ = nullptr;
    updateActivateFrameStateFunc_ = nullptr;
    isValidSurfaceNameFunc_ = nullptr;
}

void RSDrivenRenderExt::DisabledRenderMode(DrivenExtInfo& mutableInfo) const
{
    if (!drivenRenderExtLoaded_) {
        return;
    }
    auto disabledRenderMode = reinterpret_cast<void (*)(DrivenExtInfo&)>(disabledRenderModeFunc_);
    disabledRenderMode(mutableInfo);
}

void RSDrivenRenderExt::SetCurrFrameBounds(
    DrivenExtInfo& mutableInfo, const RectF& bounds, const RectF& viewPort, const RectI& contentAbsRect) const
{
    if (!drivenRenderExtLoaded_) {
        return;
    }
    auto setCurrFrameBounds =
        reinterpret_cast<void (*)(DrivenExtInfo&, const RectF&, const RectF&, const RectI&)>(setCurrFrameBoundsFunc_);
    setCurrFrameBounds(mutableInfo, bounds, viewPort, contentAbsRect);
}

void RSDrivenRenderExt::UpdateActivateFrameState(DrivenExtInfo& mutableInfo, const RectI& dstRect,
    bool backgroundDirty, bool contentDirty, bool nonContentDirty) const
{
    if (!drivenRenderExtLoaded_) {
        return;
    }
    auto updateActivateFrameState =
        reinterpret_cast<void (*)(DrivenExtInfo&, const RectI&, bool, bool, bool)>(updateActivateFrameStateFunc_);
    updateActivateFrameState(mutableInfo, dstRect, backgroundDirty, contentDirty, nonContentDirty);
}

bool RSDrivenRenderExt::IsValidSurfaceName(const std::string& surfaceName) const
{
    if (!drivenRenderExtLoaded_) {
        return false;
    }
    auto isValidSurfaceName = reinterpret_cast<bool (*)(const std::string&)>(isValidSurfaceNameFunc_);
    return isValidSurfaceName(surfaceName);
}
} // namespace Rosen
} // namespace OHOS
