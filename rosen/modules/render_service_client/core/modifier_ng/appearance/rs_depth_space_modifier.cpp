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

#include "modifier_ng/appearance/rs_depth_space_modifier.h"

namespace OHOS::Rosen::ModifierNG {
void RSDepthSpaceModifier::SetDepthImage(const std::shared_ptr<RSImage>& depthImage)
{
    Setter<RSProperty>(RSPropertyType::DEPTH_IMAGE, depthImage);
}

void RSDepthSpaceModifier::SetDepthCameraPara(const DepthCameraPara& cameraPara)
{
    Setter<RSProperty>(RSPropertyType::DEPTH_CAMERA_PARA, cameraPara);
}

void RSDepthSpaceModifier::SetDepthLightPara(const DepthLightPara& lightPara)
{
    Setter<RSProperty>(RSPropertyType::DEPTH_LIGHT_PARA, lightPara);
}

void RSDepthSpaceModifier::SetDepthImageMatrix(const Matrix3f& imageMatrix)
{
    Setter<RSProperty>(RSPropertyType::DEPTH_IMAGE_MATRIX, imageMatrix);
}
} // namespace OHOS::Rosen::ModifierNG