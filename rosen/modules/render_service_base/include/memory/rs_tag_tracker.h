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
#include "include/gpu/GrContext.h"
#include "common/rs_common_def.h"
#include "common/rs_macros.h"

namespace OHOS {
namespace Rosen {
class RSB_EXPORT RSTagTracker {
public:
    enum TAGTYPE : uint32_t {
        TAG_SAVELAYER_DRAW_NODE = 1,
        TAG_RESTORELAYER_DRAW_NODE,
        TAG_SAVELAYER_COLOR_FILTER,
        TAG_CAPTURE,
        TAG_COLD_START,
        TAG_ACQUIRE_SURFACE,
        TAG_DRAW_SURFACENODE,
    };
    RSTagTracker(GrContext* grContext, RSTagTracker::TAGTYPE tagType);
    RSTagTracker(GrContext* grContext, NodeId nodeId, RSTagTracker::TAGTYPE tagType);
    RSTagTracker(GrContext* grContext, GrGpuResourceTag& tag);
    void SetTagEnd();
    ~RSTagTracker();
    static void UpdateReleseGpuReousrceEnable(bool releaseResEnable);
private:
    bool isSetTagEnd_ = false;
    GrContext* grContext_ = nullptr;
    static bool releaseGpuResourceEnable_;
}; 
} // namespace OHOS  
} // namespace Rosen
#endif