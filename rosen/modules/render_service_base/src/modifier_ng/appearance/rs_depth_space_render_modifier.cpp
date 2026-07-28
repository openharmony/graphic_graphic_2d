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

#include "modifier_ng/appearance/rs_depth_space_render_modifier.h"

#include "property/rs_spatial_effect_def.h"

namespace OHOS::Rosen::ModifierNG {
const RSDepthSpaceRenderModifier::LegacyPropertyApplierMap
    RSDepthSpaceRenderModifier::LegacyPropertyApplierMap_ = {
        { RSPropertyType::DEPTH_IMAGE,
            RSRenderModifier::PropertyApplyHelper<std::shared_ptr<RSImage>, &RSProperties::SetDepthImage> },
        { RSPropertyType::DEPTH_CAMERA_PARA,
            RSRenderModifier::PropertyApplyHelper<DepthCameraPara, &RSProperties::SetDepthCameraPara> },
        { RSPropertyType::DEPTH_LIGHT_PARA,
            RSRenderModifier::PropertyApplyHelper<DepthLightPara, &RSProperties::SetDepthLightPara> },
        { RSPropertyType::DEPTH_IMAGE_MATRIX,
            RSRenderModifier::PropertyApplyHelper<Matrix3f, &RSProperties::SetDepthImageMatrix> },
    };

void RSDepthSpaceRenderModifier::ResetProperties(RSProperties& properties)
{
    properties.SetDepthImage(nullptr);
    properties.SetDepthCameraPara(DepthCameraPara());
    properties.SetDepthLightPara(DepthLightPara());
    properties.SetDepthImageMatrix(Matrix3f::IDENTITY);
}
} // namespace OHOS::Rosen::ModifierNG