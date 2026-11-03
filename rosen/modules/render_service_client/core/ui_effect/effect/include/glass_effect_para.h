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
#ifndef UIEFFECT_EFFECT_GLASS_EFFECT_PARA_H
#define UIEFFECT_EFFECT_GLASS_EFFECT_PARA_H
#include <iostream>

#include "visual_effect_para.h"
#include "ui_effect/mask/include/mask_para.h"
#include "ui_effect/utils.h"
#include "pixel_map.h"

namespace OHOS {
namespace Rosen {
class GlassEffectPara : public VisualEffectPara {
public:
    GlassEffectPara()
    {
        this->type_ = VisualEffectPara::ParaType::GLASS_EFFECT;
    }
    ~GlassEffectPara() override = default;

    void SetSphereCenter(const Vector2f& center)
    {
        sphereCenter_ = center;
    }

    const Vector2f& GetSphereCenter() const
    {
        return sphereCenter_;
    }

    void SetSphereRadius(float radius)
    {
        sphereRadius_ = radius;
    }

    const float& GetSphereRadius() const
    {
        return sphereRadius_;
    }

    void SetAverageBgColor(const Vector4f& color)
    {
        averageBgColor_ = color;
    }

    const Vector4f& GetAverageBgColor() const
    {
        return averageBgColor_;
    }

    void SetOpacity(float opacity)
    {
        opacity_ = opacity;
    }

    const float& GetOpacity() const
    {
        return opacity_;
    }

    void SetShapeScale(float scale)
    {
        shapeScale_ = scale;
    }

    const float& GetShapeScale() const
    {
        return shapeScale_;
    }

    void SetShadowOffset(float offset)
    {
        shadowOffset_ = offset;
    }

    const float& GetShadowOffset() const
    {
        return shadowOffset_;
    }

    void SetShadowRadius(float radius)
    {
        shadowRadius_ = radius;
    }

    const float& GetShadowRadius() const
    {
        return shadowRadius_;
    }

    void SetShadowEdgeSoftness(float softness)
    {
        shadowEdgeSoftness_ = softness;
    }

    const float& GetShadowEdgeSoftness() const
    {
        return shadowEdgeSoftness_;
    }

    void SetShadowOpacity(float opacity)
    {
        shadowOpacity_ = opacity;
    }

    const float& GetShadowOpacity() const
    {
        return shadowOpacity_;
    }

    void SetCausticOffset(float offset)
    {
        causticOffset_ = offset;
    }

    const float& GetCausticOffset() const
    {
        return causticOffset_;
    }

    void SetCausticRadius(float radius)
    {
        causticRadius_ = radius;
    }

    const float& GetCausticRadius() const
    {
        return causticRadius_;
    }

    void SetCausticEdgeSoftness(float softness)
    {
        causticEdgeSoftness_ = softness;
    }

    const float& GetCausticEdgeSoftness() const
    {
        return causticEdgeSoftness_;
    }

    void SetCausticOpacity(float opacity)
    {
        causticOpacity_ = opacity;
    }

    const float& GetCausticOpacity() const
    {
        return causticOpacity_;
    }

    void SetContentTintColor(const Vector4f& color)
    {
        contentTintColor_ = color;
    }

    const Vector4f& GetContentTintColor() const
    {
        return contentTintColor_;
    }

    void SetContentScale(float scale)
    {
        contentScale_ = scale;
    }

    const float& GetContentScale() const
    {
        return contentScale_;
    }

    void SetContentSaturation(float saturation)
    {
        contentSaturation_ = saturation;
    }

    const float& GetContentSaturation() const
    {
        return contentSaturation_;
    }

    void SetContentDispersion(float dispersion)
    {
        contentDispersion_ = dispersion;
    }

    const float& GetContentDispersion() const
    {
        return contentDispersion_;
    }

    void SetShapeMask(std::shared_ptr<MaskPara> mask)
    {
        shapeMaskPara_ = mask;
    }

    const std::shared_ptr<MaskPara>& GetShapeMask() const
    {
        return shapeMaskPara_;
    }

    void SetContentMask(std::shared_ptr<MaskPara> mask)
    {
        contentMaskPara_ = mask;
    }

    const std::shared_ptr<MaskPara>& GetContentMask() const
    {
        return contentMaskPara_;
    }

    void SetReflectionImage(std::shared_ptr<Media::PixelMap> image)
    {
        reflectionImage_ = image;
    }

    const std::shared_ptr<Media::PixelMap>& GetReflectionImage() const
    {
        return reflectionImage_;
    }

private:
    Vector2f sphereCenter_ = Vector2f(0.5f, 0.5f);
    float sphereRadius_ = 0.0f;
    Vector4f averageBgColor_;
    float opacity_ = 0.0f;
    float shapeScale_ = 0.0f;
    float shadowOffset_ = 0.0f;
    float shadowRadius_ = 0.0f;
    float shadowEdgeSoftness_ = 0.0f;
    float shadowOpacity_ = 0.0f;
    float causticOffset_ = 0.0f;
    float causticRadius_ = 0.0f;
    float causticEdgeSoftness_ = 0.0f;
    float causticOpacity_ = 0.0f;
    Vector4f contentTintColor_;
    float contentScale_ = 0.0f;
    float contentSaturation_ = 0.0f;
    float contentDispersion_ = 0.0f;
    std::shared_ptr<MaskPara> shapeMaskPara_ = nullptr;
    std::shared_ptr<MaskPara> contentMaskPara_ = nullptr;
    std::shared_ptr<Media::PixelMap> reflectionImage_ = nullptr;
};
} // namespace Rosen
} // namespace OHOS
#endif // UIEFFECT_EFFECT_GLASS_EFFECT_PARA_H
