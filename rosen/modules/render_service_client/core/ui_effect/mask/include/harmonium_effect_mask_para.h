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
#ifndef UIEFFECT_HARMONIUM_EFFECT_MASK_PARA_H
#define UIEFFECT_HARMONIUM_EFFECT_MASK_PARA_H

#include "pixel_map.h"
#include "ui_effect/mask/include/mask_para.h"
#include "common/rs_common_def.h"
#include "common/rs_vector4.h"
#include "common/rs_macros.h"

namespace OHOS {
namespace Rosen {
class RSC_EXPORT HarmoniumEffectMaskPara : public MaskPara {
public:
    HarmoniumEffectMaskPara()
    {
        type_ = MaskPara::Type::HARMONIUM_EFFECT_MASK;
    }
    ~HarmoniumEffectMaskPara() override = default;

    HarmoniumEffectMaskPara(const HarmoniumEffectMaskPara& other);

    void SetPixelMap(std::shared_ptr<Media::PixelMap>& pixelMap)
    {
        pixelMap_ = pixelMap;
    }

    const std::shared_ptr<Media::PixelMap>& GetPixelMap() const
    {
        return pixelMap_;
    }

    bool Marshalling(Parcel& parcel) const override;

    static void RegisterUnmarshallingCallback();

    [[nodiscard]] static bool OnUnmarshalling(Parcel& parcel, std::shared_ptr<MaskPara>& val);

    std::shared_ptr<MaskPara> Clone() const override;

private:
    std::shared_ptr<Media::PixelMap> pixelMap_ = nullptr;
};
} // namespace Rosen
} // namespace OHOS
#endif
