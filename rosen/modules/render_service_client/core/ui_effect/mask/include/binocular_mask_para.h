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
#ifndef UIEFFECT_BINOCULAR_MASK_PARA_H
#define UIEFFECT_BINOCULAR_MASK_PARA_H

#include "ui_effect/mask/include/mask_para.h"
#include "common/rs_macros.h"

namespace OHOS {
namespace Rosen {
class RSC_EXPORT BinocularMaskPara : public MaskPara {
public:
    BinocularMaskPara()
    {
        type_ = MaskPara::Type::BINOCULAR_MASK;
    }
    ~BinocularMaskPara() override = default;

    BinocularMaskPara(const BinocularMaskPara& other) = default;

    void SetRadiusX(float radiusX) { radiusX_ = radiusX; }
    float GetRadiusX() const { return radiusX_; }
    void SetRadiusY(float radiusY) { radiusY_ = radiusY; }
    float GetRadiusY() const { return radiusY_; }
    void SetGap(float gap) { gap_ = gap; }
    float GetGap() const { return gap_; }
    void SetSoftness(float softness) { softness_ = softness; }
    float GetSoftness() const { return softness_; }

    bool Marshalling(Parcel& parcel) const override;

    static void RegisterUnmarshallingCallback();

    [[nodiscard]] static bool OnUnmarshalling(Parcel& parcel, std::shared_ptr<MaskPara>& val);

    std::shared_ptr<MaskPara> Clone() const override;

private:
    float radiusX_ = 0.28f;
    float radiusY_ = 0.48f;
    float gap_ = 0.52f;
    float softness_ = 0.20f;
};
} // namespace Rosen
} // namespace OHOS
#endif // UIEFFECT_BINOCULAR_MASK_PARA_H