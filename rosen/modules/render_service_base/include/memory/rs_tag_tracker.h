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
#ifndef RS_TAG_TRACKER
#define RS_TAG_TRACKER
#ifndef USE_ROSEN_DRAWING
#ifndef NEW_SKIA
#include "include/gpu/GrContext.h"
#else
#include "include/gpu/GrDirectContext.h"
#endif
#else
#include "image/gpu_context.h"
#endif
#include "common/rs_common_def.h"
#include "common/rs_macros.h"
#include "platform/common/rs_system_properties.h"

namespace OHOS {
namespace Rosen {
class RSB_EXPORT RSTagTracker {
public:
    enum TAGTYPE : uint32_t {
        TAG_DRAW_SURFACENODE = 1, // don't change head and tail, insert the middle if u add data.
        TAG_SUB_THREAD,
        TAG_SAVELAYER_DRAW_NODE,
        TAG_RESTORELAYER_DRAW_NODE,
        TAG_SAVELAYER_COLOR_FILTER,
        TAG_ACQUIRE_SURFACE,
        TAG_FILTER,
        TAG_RENDER_FRAME,
        TAG_UNTAGGED,
        TAG_CAPTURE,
    };
#ifndef USE_ROSEN_DRAWING
#if defined(NEW_SKIA)
    RSTagTracker(GrDirectContext* grContext, RSTagTracker::TAGTYPE tagType);
    RSTagTracker(GrDirectContext* grContext, NodeId nodeId, RSTagTracker::TAGTYPE tagType);
    RSTagTracker(GrDirectContext* grContext, GrGpuResourceTag& tag);
#else
    RSTagTracker(GrContext* grContext, RSTagTracker::TAGTYPE tagType);
    RSTagTracker(GrContext* grContext, NodeId nodeId, RSTagTracker::TAGTYPE tagType);
    RSTagTracker(GrContext* grContext, GrGpuResourceTag& tag);
#endif
#else
    RSTagTracker(Drawing::GPUContext* gpuContext, RSTagTracker::TAGTYPE tagType);
    RSTagTracker(Drawing::GPUContext* gpuContext, NodeId nodeId, RSTagTracker::TAGTYPE tagType);
    RSTagTracker(Drawing::GPUContext* gpuContext, Drawing::GPUResourceTag& tag);
#endif
    void SetTagEnd();
    ~RSTagTracker();
    static void UpdateReleaseResourceEnabled(bool releaseResEnabled);
    static std::string TagType2String(TAGTYPE type);
private:
    bool isSetTagEnd_ = false;
#ifndef USE_ROSEN_DRAWING
#if defined(NEW_SKIA)
    GrDirectContext* grContext_ = nullptr;
#else
    GrContext* grContext_ = nullptr;
#endif
#else
    Drawing::GPUContext* gpuContext_ = nullptr;
#endif
};
} // namespace Rosen
} // namespace OHOS
#endif