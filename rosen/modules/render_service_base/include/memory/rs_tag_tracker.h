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
#include "image/gpu_context.h"
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
    enum SOURCETYPE : uint32_t {
        SOURCE_OTHER,
        SOURCE_RSCUSTOMMODIFIERDRAWABLE,
        SOURCE_RSBEGINBLENDERDRAWABLE,
        SOURCE_RSSHADOWDRAWABLE,
        SOURCE_RSBACKGROUNDIMAGEDRAWABLE,
        SOURCE_RSBACKGROUNDEFFECTDRAWABLE,
        SOURCE_RSUSEEFFECTDRAWABLE,
        SOURCE_RSDYNAMICLIGHTUPDRAWABLE,
        SOURCE_RSBINARIZATIONDRAWABLE,
        SOURCE_RSCOLORFILTERDRAWABLE,
        SOURCE_RSLIGHTUPEFFECTDRAWABLE,
        SOURCE_RSDYNAMICDIMDRAWABLE,
        SOURCE_RSFOREGROUNDFILTERDRAWABLE,
        SOURCE_RSFOREGROUNDFILTERRESTOREDRAWABLE,
        SOURCE_RSPIXELSTRETCHDRAWABLE,
        SOURCE_RSPOINTLIGHTDRAWABLE,
        SOURCE_RSPROPERTYDRAWABLE,
        SOURCE_RSFILTERDRAWABLE,
        SOURCE_FINISHOFFSCREENRENDER,
        SOURCE_DRAWSELFDRAWINGNODEBUFFER,
        SOURCE_ONCAPTURE,
        SOURCE_INITCACHEDSURFACE,
        SOURCE_DRAWRENDERCONTENT
    };
    RSTagTracker(const std::shared_ptr<Drawing::GPUContext>& gpuContext, RSTagTracker::TAGTYPE tagType);
    RSTagTracker(const std::shared_ptr<Drawing::GPUContext>& gpuContext, RSTagTracker::SOURCETYPE sourceType);
    RSTagTracker(const std::shared_ptr<Drawing::GPUContext>& gpuContext, NodeId nodeId,
        RSTagTracker::TAGTYPE tagType, const std::string& name);
    RSTagTracker(const std::shared_ptr<Drawing::GPUContext>& gpuContext, Drawing::GPUResourceTag& tag);
    void SetTagEnd();
    ~RSTagTracker();
    static void UpdateReleaseResourceEnabled(bool releaseResEnabled);
    static std::string TagType2String(TAGTYPE type);
private:
    bool isSetTagEnd_ = false;
    std::shared_ptr<Drawing::GPUContext> gpuContext_ = nullptr;
};
} // namespace Rosen
} // namespace OHOS
#endif