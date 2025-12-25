/*
 * Copyright (C) 2025 Huawei Device Co., Ltd.
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
#ifndef UIEFFECT_FILTER_MAGNIFIER_PARA_H
#define UIEFFECT_FILTER_MAGNIFIER_PARA_H
#include <iostream>
#include "filter_para.h"

namespace OHOS {
namespace Rosen {
class MagnifierPara : public FilterPara {
public:
    MagnifierPara()
    {
        this->type_ = FilterPara::ParaType::MAGNIFIER;
    }
    ~MagnifierPara() override = default;

    void SetFactor(float& factor)
    {
        factor_ = factor;
    }

    float GetFactor() const
    {
        return factor_;
    }

    void SetWidth(float& width)
    {
        width_ = width;
    }

    float GetWidth() const
    {
        return width_;
    }

    void SetHeight(float& height)
    {
        height_ = height;
    }

    float GetHeight() const
    {
        return height_;
    }

    void SetCornerRadius(float& cornerRadius)
    {
        cornerRadius_ = cornerRadius;
    }

    float GetCornerRadius() const
    {
        return cornerRadius_;
    }

    void SetBorderWidth(float& borderWidth)
    {
        borderWidth_ = borderWidth;
    }

    float GetBorderWidth() const
    {
        return borderWidth_;
    }

    void SetOffsetX(float& offsetX)
    {
        offsetX_ = offsetX;
    }

    float GetOffsetX() const
    {
        return offsetX_;
    }

    void SetOffsetY(float& offsetY)
    {
        offsetY_ = offsetY;
    }

    float GetOffsetY() const
    {
        return offsetY_;
    }

    void SetZoomOffsetX(float& zoomOffsetX)
    {
        zoomOffsetX_ = zoomOffsetX;
    }

    float GetZoomOffsetX() const
    {
        return zoomOffsetX_;
    }

    void SetZoomOffsetY(float& zoomOffsetY)
    {
        zoomOffsetY_ = zoomOffsetY;
    }

    float GetZoomOffsetY() const
    {
        return zoomOffsetY_;
    }

    void SetShadowOffsetX(float& shadowOffsetX)
    {
        shadowOffsetX_ = shadowOffsetX;
    }

    float GetShadowOffsetX() const
    {
        return shadowOffsetX_;
    }

    void SetShadowOffsetY(float& shadowOffsetY)
    {
        shadowOffsetY_ = shadowOffsetY;
    }

    float GetShadowOffsetY() const
    {
        return shadowOffsetY_;
    }

    void SetShadowSize(float& shadowSize)
    {
        shadowSize_ = shadowSize;
    }

    float GetShadowSize() const
    {
        return shadowSize_;
    }

    void SetShadowStrength(float& shadowStrength)
    {
        shadowStrength_ = shadowStrength;
    }

    float GetShadowStrength() const
    {
        return shadowStrength_;
    }

    void SetGradientMaskColor1(uint32_t& gradientMaskColor1)
    {
        gradientMaskColor1_ = gradientMaskColor1;
    }

    uint32_t GetGradientMaskColor1() const
    {
        return gradientMaskColor1_;
    }

    void SetGradientMaskColor2(uint32_t& gradientMaskColor2)
    {
        gradientMaskColor2_ = gradientMaskColor2;
    }

    uint32_t GetGradientMaskColor2() const
    {
        return gradientMaskColor2_;
    }

    void SetOuterContourColor1(uint32_t& outerContourColor1)
    {
        outerContourColor1_ = outerContourColor1;
    }

    uint32_t GetOuterContourColor1() const
    {
        return outerContourColor1_;
    }

    void SetOuterContourColor2(uint32_t& outerContourColor2)
    {
        outerContourColor2_ = outerContourColor2;
    }

    uint32_t GetOuterContourColor2() const
    {
        return outerContourColor2_;
    }

private:
    float factor_ = 0.f;
    float width_ = 0.f;
    float height_ = 0.f;
    float cornerRadius_ = 0.f;
    float borderWidth_ = 0.f;
    float offsetX_ = 0.f;
    float offsetY_ = 0.f;
    float zoomOffsetX_ = 0.f;
    float zoomOffsetY_ = 0.f;

    float shadowOffsetX_ = 0.f;
    float shadowOffsetY_ = 0.f;
    float shadowSize_ = 0.f;
    float shadowStrength_ = 0.f;

    // rgba
    uint32_t gradientMaskColor1_ = 0x00000000;
    uint32_t gradientMaskColor2_ = 0x00000000;
    uint32_t outerContourColor1_ = 0x00000000;
    uint32_t outerContourColor2_ = 0x00000000;
};
} // namespace Rosen
} // namespace OHOS
#endif // UIEFFECT_FILTER_MAGNIFIER_PARA_H