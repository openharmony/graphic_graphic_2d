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

#include "ui_effect/mask/include/fractal_glass_mask_para.h"
#include "platform/common/rs_log.h"

#ifdef ROSEN_OHOS
#include "media_errors.h"
#endif

namespace OHOS {
namespace Rosen {

FractalGlassMaskPara::FractalGlassMaskPara(const FractalGlassMaskPara& other)
    : MaskPara(other), pixelMap_(nullptr), src_(other.src_), dst_(other.dst_),
      glassNum_(other.glassNum_), glassStrength_(other.glassStrength_), glassSoftness_(other.glassSoftness_),
      isSymmetric_(other.isSymmetric_)
{
#ifdef ROSEN_OHOS
    if (other.pixelMap_ != nullptr) {
        int32_t errorCode = Media::ERR_MEDIA_INVALID_VALUE;
        auto pixelMap = other.pixelMap_->Clone(errorCode);
        pixelMap_ = std::move(pixelMap);
        if (errorCode != Media::SUCCESS || pixelMap_ == nullptr) {
            RS_LOGE("[ui_effect] FractalGlassMaskPara clone pixelMap failed");
        }
    }
#endif
}

std::shared_ptr<MaskPara> FractalGlassMaskPara::Clone() const
{
    return std::make_shared<FractalGlassMaskPara>(*this);
}

} // namespace Rosen
} // namespace OHOS
