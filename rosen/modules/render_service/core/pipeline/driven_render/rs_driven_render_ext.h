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

#ifndef RENDER_SERVICE_CORE_DRIVEN_RENDER_EXT_H
#define RENDER_SERVICE_CORE_DRIVEN_RENDER_EXT_H

#include "rs_driven_render_manager.h"

namespace OHOS {
namespace Rosen {
class RSDrivenRenderExt {
public:
    static RSDrivenRenderExt& Instance();
    bool OpenDrivenRenderExt();

    void DisabledRenderMode(DrivenExtInfo& mutableInfo) const;
    void SetCurrFrameBounds(
        DrivenExtInfo& mutableInfo, const RectF& bounds, const RectF& viewPort, const RectI& contentAbsRect) const;
    void UpdateActivateFrameState(DrivenExtInfo& mutableInfo, const RectI& dstRect, bool backgroundDirty,
        bool contentDirty, bool nonContentDirty) const;

    bool IsValidSurfaceName(const std::string& surfaceName) const;

private:
    RSDrivenRenderExt() = default;
    ~RSDrivenRenderExt();
    RSDrivenRenderExt(const RSDrivenRenderExt&) = delete;
    RSDrivenRenderExt(const RSDrivenRenderExt&&) = delete;
    RSDrivenRenderExt& operator=(const RSDrivenRenderExt&) = delete;
    RSDrivenRenderExt& operator=(const RSDrivenRenderExt&&) = delete;

    bool DrivenRenderVersionMatched();
    bool LoadDrivenRenderFunc();
    void CloseDrivenRenderExt();

    bool drivenRenderExtLoaded_ = false;
    void* drivenRenderExtHandle_ = nullptr;

    void* disabledRenderModeFunc_ = nullptr;
    void* setCurrFrameBoundsFunc_ = nullptr;
    void* updateActivateFrameStateFunc_ = nullptr;
    void* isValidSurfaceNameFunc_ = nullptr;
};
} // namespace Rosen
} // namespace OHOS
#endif // RENDER_SERVICE_CORE_DRIVEN_RENDER_EXT_H
