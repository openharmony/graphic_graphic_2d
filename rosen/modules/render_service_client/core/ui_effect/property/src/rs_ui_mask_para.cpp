/*
 * Copyright (c) 2025 Huawei Device Co., Ltd.
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

#include "ui_effect/property/include/rs_ui_mask_para.h"

#include "platform/common/rs_log.h"
#include "render/rs_render_ripple_mask.h"

namespace OHOS {
namespace Rosen {

bool RSUIMaskPara::Equals(const std::shared_ptr<RSUIFilterParaBase>& other)
{
    if (other == nullptr || std::static_pointer_cast<RSUIMaskPara>(other) == nullptr) {
        ROSEN_LOGE("RSUIMaskPara::Equals NG %{public}d!", other == nullptr ? -1 : static_cast<int>(other->GetType()));
        ;
        return false;
    }
    return true;
}

RSUIFilterType RSUIMaskPara::ConvertMaskType(MaskPara::Type maskType)
{
    switch (maskType) {
        case MaskPara::Type::RADIAL_GRADIENT_MASK: {
            return RSUIFilterType::RADIAL_GRADIENT_MASK;
        }
        case MaskPara::Type::RIPPLE_MASK: {
            return RSUIFilterType::RIPPLE_MASK;
        }
        default:
            return RSUIFilterType::NONE;
    }
}
} // namespace Rosen
} // namespace OHOS