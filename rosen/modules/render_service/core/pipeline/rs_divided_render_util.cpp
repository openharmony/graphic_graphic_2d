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

#include <parameters.h>

#include "common/rs_obj_abs_geometry.h"
#include "platform/common/rs_log.h"
#include "property/rs_properties_painter.h"
#include "render/rs_skia_filter.h"

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
    if (property.GetRotation() != 0 || property.GetRotationX() != 0 || property.GetRotationY() != 0 ||
        property.GetQuaternion() != Quaternion()) {
        RS_LOGD("RsDebug RSDividedRenderUtil::IsNeedClient enable composition client need rotation");
        return true;
    }
    return false;
}

bool RSDividedRenderUtil::IsForceClient()
{
    return (std::atoi((system::GetParameter("rosen.client_composition.enabled", "0")).c_str()) != 0);
}
} // namespace Rosen
} // namespace OHOS
