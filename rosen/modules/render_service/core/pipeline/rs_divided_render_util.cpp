/*
 * Copyright (c) 2021-2022 Huawei Device Co., Ltd.
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
#include "rs_divided_render_util.h"

#include "common/rs_obj_abs_geometry.h"
#include "platform/common/rs_log.h"
#include "property/rs_properties_painter.h"
#include "property/rs_transition_properties.h"
#include "render/rs_skia_filter.h"
#include "rs_trace.h"

namespace OHOS {
namespace Rosen {
bool RSDividedRenderUtil::enableClient = false;

void RSDividedRenderUtil::SetNeedClient(bool flag)
{
    enableClient = flag;
}

bool RSDividedRenderUtil::IsNeedClient(RSSurfaceRenderNode& node, const ComposeInfo& info)
{
    if (IsForceClient()) {
        RS_LOGD("RsDebug RSDividedRenderUtil::IsNeedClient: client composition is force enabled.");
        return true;
    }

    if (enableClient) {
        RS_LOGD("RsDebug RSDividedRenderUtil::IsNeedClient enable composition client");
        return true;
    }

    const auto& property = node.GetRenderProperties();
    auto backgroundColor = static_cast<SkColor>(property.GetBackgroundColor().AsArgbInt());
    // If node's gravity is not RESIZE and backgroundColor is not transparent,
    // we check the src and dst size to decide whether to use client composition or not.
    if (property.GetFrameGravity() != Gravity::RESIZE && SkColorGetA(backgroundColor) != SK_AlphaTRANSPARENT &&
        (info.srcRect.w != info.dstRect.w || info.srcRect.h != info.dstRect.h)) {
        return true;
    }

    if (property.GetBackgroundFilter() || property.GetFilter()) {
        RS_LOGD("RsDebug RSDividedRenderUtil::IsNeedClient enable composition client need filter");
        return true;
    }
    
    if (!property.GetCornerRadius().IsZero()) {
        RS_LOGD("RsDebug RSDividedRenderUtil::IsNeedClient enable composition client need round corner");
        return true;
    }
    if (property.IsShadowValid()) {
        RS_LOGD("RsDebug RSDividedRenderUtil::IsNeedClient enable composition client need shadow");
        return true;
    }
    auto transitionProperties = node.GetAnimationManager().GetTransitionProperties();
    if (!transitionProperties) {
        return false;
    }
    SkMatrix matrix = transitionProperties->GetRotate();
    float value[9];
    matrix.get9(value);
    if (SkMatrix::kMSkewX < 0 || SkMatrix::kMSkewX >= 9 || // 9 is the upper bound
        SkMatrix::kMScaleX < 0 || SkMatrix::kMScaleX >= 9) { // 9 is the upper bound
        RS_LOGE("RSDividedRenderUtil:: The value of kMSkewX or kMScaleX is illegal");
        return false;
    } else {
        float rAngle = -round(atan2(value[SkMatrix::kMSkewX], value[SkMatrix::kMScaleX]) * (180 / PI));
        bool isNeedClient = rAngle > 0;
        if (isNeedClient) {
            RS_LOGD("RsDebug RSDividedRenderUtil::IsNeedClient enable composition client need animation rotate");
        }
        return isNeedClient;
    }
}

bool RSDividedRenderUtil::IsForceClient()
{
    return (std::atoi((system::GetParameter("rosen.client_composition.enabled", "0")).c_str()) != 0);
}
} // namespace Rosen
} // namespace OHOS
