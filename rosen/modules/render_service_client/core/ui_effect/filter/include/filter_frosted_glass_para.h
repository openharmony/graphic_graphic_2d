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
#ifndef UIEFFECT_FILTER_FROSTED_GLASS_PARA_H
#define UIEFFECT_FILTER_FROSTED_GLASS_PARA_H
#include <iostream>
#include "common/rs_vector2.h"
#include "common/rs_vector3.h"
#include "filter_para.h"

namespace OHOS {
namespace Rosen {
class FrostedGlassPara : public FilterPara {
public:
    FrostedGlassPara()
    {
        this->type_ = FilterPara::ParaType::FROSTED_GLASS;
    }
    ~FrostedGlassPara() override = default;

    void SetBorderSize(Vector2f& borderSize)
    {
        borderSize_ = borderSize;
    }

    const Vector2f GetBorderSize() const
    {
        return borderSize_;
    }

    void SetBlurParams(Vector2f& blurParams)
    {
        blurParams_ = blurParams;
    }

    const Vector2f GetBlurParams() const
    {
        return blurParams_;
    }

    void SetCornerRadius(float& cornerRadius)
    {
        cornerRadius_ = cornerRadius;
    }

    const float& GetCornerRadius() const
    {
        return cornerRadius_;
    }

    void SetBorderWidth(float& borderWidth)
    {
        borderWidth_ = borderWidth;
    }

    const float& GetBorderWidth() const
    {
        return borderWidth_;
    }

    void SetOffset(float& offset)
    {
        offset_ = offset;
    }

    const float& GetOffset() const
    {
        return offset_;
    }

    void SetDownSampleFactor(float& downSampleFactor)
    {
        downSampleFactor_ = downSampleFactor;
    }

    const float& GetDownSampleFactor() const
    {
        return downSampleFactor_;
    }

    void SetBgFactor(float& bgFactor)
    {
        bgFactor_ = bgFactor;
    }

    const float& GetBgFactor() const
    {
        return bgFactor_;
    }

    void SetInnerShadowParams(Vector3f& innerShadowParams)
    {
        innerShadowParams_ = innerShadowParams;
    }

    const Vector3f GetInnerShadowParams() const
    {
        return innerShadowParams_;
    }

    void SetSdParams(Vector3f& sdParams)
    {
        sdParams_ = sdParams;
    }

    const Vector3f GetSdParams() const
    {
        return sdParams_;
    }

    void SetRefractOutPx(float& refractOutPx)
    {
        refractOutPx_ = refractOutPx;
    }

    const float& GetRefractOutPx() const
    {
        return refractOutPx_;
    }

    void SetEnvParams(Vector3f& envParams)
    {
        envParams_ = envParams;
    }

    const Vector3f GetEnvParams() const
    {
        return envParams_;
    }

    void SetEdgeLightAngleParams(Vector3f& edgeLightAngle)
    {
        edgeLightAngle_ = edgeLightAngle;
    }

    const Vector3f GetEdgeLightAngleParams() const
    {
        return edgeLightAngle_;
    }

    void SetEdgeLightBlurParams(Vector2f& edgeLightBlur)
    {
        edgeLightBlur_ = edgeLightBlur;
    }

    const Vector2f GetEdgeLightBlurParams() const
    {
        return edgeLightBlur_;
    }

    void SetEdgeLightDirParams(Vector2f& edgeLightDir)
    {
        edgeLightDir_ = edgeLightDir;
    }

    const Vector2f GetEdgeLightDirParams() const
    {
        return edgeLightDir_;
    }

    void SetHlParams(Vector3f& hlParams)
    {
        hlParams_ = hlParams;
    }

    const Vector3f GetHlParams() const
    {
        return hlParams_;
    }

private:
    Vector2f borderSize_ = Vector2f(0.0f, 0.0f);
    Vector2f blurParams_ = Vector2f(0.0f, 0.0f);
    float cornerRadius_ = 0.0f;
    float borderWidth_ = 0.0f;
    float offset_ = 0.0f;
    float downSampleFactor_ = 0.0f;
    float bgFactor_ = 0.0f;
    Vector3f innerShadowParams_ = Vector3f(0.0f, 0.0f, 0.0f);
    Vector3f sdParams_ = Vector3f(0.0f, 0.0f, 0.0f);
    float refractOutPx_ = 0.0f;
    Vector3f envParams_ = Vector3f(0.0f, 0.0f, 0.0f);
    Vector3f edgeLightAngle_ = Vector3f(0.0f, 0.0f, 0.0f);
    Vector2f edgeLightBlur_ = Vector2f(0.0f, 0.0f);
    Vector2f edgeLightDir_ = Vector2f(0.0f, 0.0f);
    Vector3f hlParams_ = Vector3f(0.0f, 0.0f, 0.0f);
};
} // namespace Rosen
} // namespace OHOS
#endif // UIEFFECT_FILTER_FROSTED_GLASS_PARA_H