/*
 * Copyright (C) 2026 Huawei Device Co., Ltd.
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

#include "ui_effect/mask/include/mask.h"

#include "ui_effect/mask/include/image_mask_para.h"
#include "ui_effect/mask/include/pixel_map_mask_para.h"
#include "ui_effect/mask/include/radial_gradient_mask_para.h"
#include "ui_effect/mask/include/use_effect_mask_para.h"

namespace OHOS {
namespace Rosen {

void Mask::RegisterUnmarshallingCallback()
{
    PixelMapMaskPara::RegisterUnmarshallingCallback();
    RadialGradientMaskPara::RegisterUnmarshallingCallback();
    ImageMaskPara::RegisterUnmarshallingCallback();
    UseEffectMaskPara::RegisterUnmarshallingCallback();
}

} // namespace Rosen
} // namespace OHOS