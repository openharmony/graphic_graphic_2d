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
#ifndef UIEFFECT_RADIAL_GRADIENT_MASK_PARA_H
#define UIEFFECT_RADIAL_GRADIENT_MASK_PARA_H
#include <vector>

#include "ui_effect/mask/include/mask_para.h"
#include "common/rs_vector2.h"
#include "common/rs_macros.h"

namespace OHOS {
namespace Rosen {
class RSC_EXPORT RadialGradientMaskPara : public MaskPara {
public:
    RadialGradientMaskPara()
    {
        type_ = MaskPara::Type::RADIAL_GRADIENT_MASK;
    }
    ~RadialGradientMaskPara() override = default;

    RadialGradientMaskPara(const RadialGradientMaskPara& other);

    void SetCenter(Vector2f& center)
    {
        center_ = center;
    }

    const Vector2f& GetCenter() const
    {
        return center_;
    }

    void SetRadiusX(float radiusX)
    {
        radiusX_ = radiusX;
    }

    const float& GetRadiusX() const
    {
        return radiusX_;
    }

    void SetRadiusY(float radiusY)
    {
        radiusY_ = radiusY;
    }

    const float& GetRadiusY() const
    {
        return radiusY_;
    }

    void SetColors(std::vector<float>& colors)
    {
        colors_ = colors;
    }

    const std::vector<float>& GetColors() const
    {
        return colors_;
    }

    void SetPositions(std::vector<float>& positions)
    {
        positions_ = positions;
    }

    const std::vector<float>& GetPositions() const
    {
        return positions_;
    }

    bool Marshalling(Parcel& parcel) const override;

    static void RegisterUnmarshallingCallback();

    [[nodiscard]] static bool OnUnmarshalling(Parcel& parcel, std::shared_ptr<MaskPara>& val);

    std::shared_ptr<MaskPara> Clone() const override;

private:
    Vector2f center_ = { 0.0f, 0.0f };
    float radiusX_ = 0.0f;
    float radiusY_ = 0.0f;
    std::vector<float> colors_;
    std::vector<float> positions_;
};
} // namespace Rosen
} // namespace OHOS
#endif // UIEFFECT_RADIAL_GRADIENT_MASK_PARA_H
