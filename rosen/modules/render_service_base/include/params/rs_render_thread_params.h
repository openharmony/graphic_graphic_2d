/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
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

#ifndef RENDER_SERVICE_BASE_PARAMS_RS_RENDER_THREAD_PARAMS_H
#define RENDER_SERVICE_BASE_PARAMS_RS_RENDER_THREAD_PARAMS_H

#include "property/rs_properties.h"

namespace OHOS::Rosen {
class RSB_EXPORT RSRenderThreadParams {
public:
    RSRenderThreadParams() = default;
    virtual ~RSRenderThreadParams() = default;

    bool IsPartialRenderEnabled()
    {
        return isPartialRenderEnabled_;
    }


    bool IsRegionDebugEnabled()
    {
        return isRegionDebugEnabled_;
    }

private:
    // RSDirtyRectsDfx dfx
    std::vector<std::string> dfxTargetSurfaceNames_;
    bool isRegionDebugEnabled_ = false;
    bool isPartialRenderEnabled_ = false;
    bool isDirtyRegionDfxEnabled_ = false;
    bool isTargetDirtyRegionDfxEnabled_ = false;
    bool isDisplayDirtyDfxEnabled_ = false;
    bool isOpaqueRegionDfxEnabled_ = false;
    bool isVisibleRegionDfxEnabled_ = false;
    DirtyRegionDebugType dirtyRegionDebugType_ = DirtyRegionDebugType::DISABLED;

    friend class RSUniRenderVisitor;
    friend class RSDirtyRectsDfx;
};
} // namespace OHOS::Rosen
#endif // RENDER_SERVICE_BASE_PARAMS_RS_RENDER_THREAD_PARAMS_H
