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

#ifndef UIEFFECT_SWEEP_REFRACTION_MASK_PARA_H
#define UIEFFECT_SWEEP_REFRACTION_MASK_PARA_H

#include "ui_effect/mask/include/mask_para.h"
#include "common/rs_macros.h"
#include "ui_effect/utils.h"

namespace OHOS {
namespace Rosen {
constexpr std::pair<float, float> SWEEP_REFRACTION_MASK_RADIUS_LIMITS { 0.0f, 10.0f };
constexpr std::pair<float, float> SWEEP_REFRACTION_MASK_EDGE_THICKNESS_LIMITS { 0.0f, 1000.0f };
constexpr std::pair<float, float> SWEEP_REFRACTION_MASK_REFRACT_AMOUNT_LIMITS { 0.0f, 1.0f };
constexpr std::pair<float, float> SWEEP_REFRACTION_MASK_RIPPLE_WIDTH_LIMITS { 0.01f, 1.0f };
constexpr std::pair<float, float> SWEEP_REFRACTION_MASK_SWEEP_OFFSET_LIMITS { -2.0f, 2.0f };
constexpr std::pair<float, float> SWEEP_REFRACTION_MASK_CHROMA_DELTA_LIMITS { 0.0f, 0.5f };
// shapeType is an enum (0=ROUNDED_RECT, 1=ELLIPSE), stored as int32_t, not clamped
constexpr std::pair<float, float> SWEEP_REFRACTION_MASK_CORNER_RADIUS_LIMITS { 0.0f, 1.0f };
constexpr std::pair<float, float> SWEEP_REFRACTION_MASK_PRISM_WIDTH_LIMITS { 0.01f, 2.0f };
constexpr std::pair<float, float> SWEEP_REFRACTION_MASK_PRISM_HEIGHT_LIMITS { 0.01f, 2.0f };
constexpr std::pair<float, float> SWEEP_REFRACTION_MASK_SWEEP_CENTER_LIMITS { 0.0f, 1.0f };

class RSC_EXPORT SweepRefractionMaskPara : public MaskPara {
public:
    SweepRefractionMaskPara()
    {
        type_ = MaskPara::Type::SWEEP_REFRACTION_MASK;
    }
    SweepRefractionMaskPara(const SweepRefractionMaskPara& other);
    ~SweepRefractionMaskPara() override = default;

    bool Marshalling(Parcel& parcel) const override;
    static void RegisterUnmarshallingCallback();
    static bool OnUnmarshalling(Parcel& parcel, std::shared_ptr<MaskPara>& val);
    std::shared_ptr<MaskPara> Clone() const override;

    void SetMaskRadius(float maskRadius)
    {
        maskRadius_ = UIEffect::GetLimitedPara(maskRadius, SWEEP_REFRACTION_MASK_RADIUS_LIMITS);
    }
    float GetMaskRadius() const { return maskRadius_; }

    void SetEdgeThickness(float edgeThickness)
    {
        edgeThickness_ = UIEffect::GetLimitedPara(edgeThickness, SWEEP_REFRACTION_MASK_EDGE_THICKNESS_LIMITS);
    }
    float GetEdgeThickness() const { return edgeThickness_; }

    void SetRefractAmount(float refractAmount)
    {
        refractAmount_ = UIEffect::GetLimitedPara(refractAmount, SWEEP_REFRACTION_MASK_REFRACT_AMOUNT_LIMITS);
    }
    float GetRefractAmount() const { return refractAmount_; }

    void SetRippleWidth(float rippleWidth)
    {
        rippleWidth_ = UIEffect::GetLimitedPara(rippleWidth, SWEEP_REFRACTION_MASK_RIPPLE_WIDTH_LIMITS);
    }
    float GetRippleWidth() const { return rippleWidth_; }

    void SetSweepOffset(float sweepOffset)
    {
        sweepOffset_ = UIEffect::GetLimitedPara(sweepOffset, SWEEP_REFRACTION_MASK_SWEEP_OFFSET_LIMITS);
    }
    float GetSweepOffset() const { return sweepOffset_; }

    void SetChromaDelta(float chromaDelta)
    {
        chromaDelta_ = UIEffect::GetLimitedPara(chromaDelta, SWEEP_REFRACTION_MASK_CHROMA_DELTA_LIMITS);
    }
    float GetChromaDelta() const { return chromaDelta_; }

    void SetShapeType(int32_t shapeType)
    {
        shapeType_ = shapeType;
    }
    int32_t GetShapeType() const { return shapeType_; }

    void SetCornerRadius(float cornerRadius)
    {
        cornerRadius_ = UIEffect::GetLimitedPara(cornerRadius, SWEEP_REFRACTION_MASK_CORNER_RADIUS_LIMITS);
    }
    float GetCornerRadius() const { return cornerRadius_; }

    void SetPrismWidth(float prismWidth)
    {
        prismWidth_ = UIEffect::GetLimitedPara(prismWidth, SWEEP_REFRACTION_MASK_PRISM_WIDTH_LIMITS);
    }
    float GetPrismWidth() const { return prismWidth_; }

    void SetPrismHeight(float prismHeight)
    {
        prismHeight_ = UIEffect::GetLimitedPara(prismHeight, SWEEP_REFRACTION_MASK_PRISM_HEIGHT_LIMITS);
    }
    float GetPrismHeight() const { return prismHeight_; }

    void SetSweepCenter(Vector2f& center)
    {
        sweepCenterX_ = UIEffect::GetLimitedPara(center.x_, SWEEP_REFRACTION_MASK_SWEEP_CENTER_LIMITS);
        sweepCenterY_ = UIEffect::GetLimitedPara(center.y_, SWEEP_REFRACTION_MASK_SWEEP_CENTER_LIMITS);
    }
    float GetSweepCenterX() const { return sweepCenterX_; }
    float GetSweepCenterY() const { return sweepCenterY_; }

private:
    float maskRadius_ = 0.0f;
    float edgeThickness_ = 300.0f;
    float refractAmount_ = 0.3f;
    float rippleWidth_ = 0.4f;
    float sweepOffset_ = 0.0f;
    float chromaDelta_ = 0.08f;
    int32_t shapeType_ = 0;
    float cornerRadius_ = 0.16f;
    float prismWidth_ = 1.0f;
    float prismHeight_ = 1.0f;
    float sweepCenterX_ = 0.0f;
    float sweepCenterY_ = 0.0f;
};

} // namespace Rosen
} // namespace OHOS

#endif // UIEFFECT_SWEEP_REFRACTION_MASK_PARA_H