/*
 * Copyright (c) 2026 Huawei Device Co., Ltd.
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

#ifndef RS_UNI_HWC_PREVALIDATE_PARAM_UTIL_H
#define RS_UNI_HWC_PREVALIDATE_PARAM_UTIL_H

#include <string>
#include <unordered_set>
#include <vector>

#include "feature/hwc/rs_uni_hwc_prevalidate_util.h"
#include "feature/vcld/rs_vcld_param.h"
#include "pipeline/rs_surface_render_node.h"

namespace OHOS {
namespace Rosen {
class RSUniHwcPrevalidateParamUtil {
public:
    static bool IsYUVBufferFormat(const sptr<SurfaceBuffer>& buffer);
    static bool CheckIfDoArsrPre(const sptr<SurfaceBuffer>& buffer, const std::string& nodeName);
    static bool IsNeedDssRotate(GraphicTransformType transform);
    // pre-scaling operator
    static void SetIsVcldEnabled(bool isEnabled);
    static bool GetIsVcldEnabled();

    static void ApplyArsrEnhance(RSSurfaceRenderNode::SharedPtr node, RequestLayerInfo& info);
    static void ApplyCopybit(const sptr<SurfaceBuffer>& buffer, GraphicTransformType transform,
        RequestLayerInfo& info, const RSSurfaceRenderNode::SharedPtr node);
    static void ApplyVcldParam(const RSVcldParam& vcldInfo, RequestLayerInfo& info);
    static void ApplySourceCropTuning(const sptr<SurfaceBuffer>& buffer, RequestLayerInfo& info);
    static void ApplyLayerLinearMatrix(const std::vector<float>& matrix, RequestLayerInfo& info);
    static void ProcessOfflineStrategy(const std::map<uint64_t, RequestCompositionType>& strategy);

private:
    static const std::unordered_set<std::string> VIDEO_LAYERS;
    // Surfaces with rounded corners can enable DSS.
    static bool isVcldEnabled_;
};
} // namespace Rosen
} // namespace OHOS
#endif // RS_UNI_HWC_PREVALIDATE_PARAM_UTIL_H
