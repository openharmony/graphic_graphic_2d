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

    void SetBlurParams(std::vector<float>& blurParams)
    {
        blurParams_ = blurParams;
    }

    const std::vector<float> GetBlurParams() const
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

    void SetInnerShadowParams(std::vector<float>& innerShadowParams)
    {
        innerShadowParams_ = innerShadowParams;
    }

    const std::vector<float> GetInnerShadowParams() const
    {
        return innerShadowParams_;
    }

    void SetSdParams(std::vector<float>& sdParams)
    {
        sdParams_ = sdParams;
    }

    const std::vector<float> GetSdParams() const
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

    void SetEnvParams(std::vector<float>& envParams)
    {
        envParams_ = envParams;
    }

    const std::vector<float> GetEnvParams() const
    {
        return envParams_;
    }

    void SetHighLightParams(std::vector<float>& highLightParams)
    {
        highLightParams_ = highLightParams;
    }

    const std::vector<float> GetHighLightParams() const
    {
        return highLightParams_;
    }

    void SetHlParams(std::vector<float>& hlParams)
    {
        hlParams_ = hlParams;
    }

    const std::vector<float> GetHlParams() const
    {
        return hlParams_;
    }

private:
    Vector2f borderSize_;
    std::vector<float> blurParams_;
    float cornerRadius_;
    float borderWidth_;
    float offset_;
    float downSampleFactor_;
    float bgFactor_;
    std::vector<float> innerShadowParams_;
    std::vector<float> sdParams_;
    float refractOutPx_;
    std::vector<float> envParams_;
    std::vector<float> highLightParams_;
    std::vector<float> hlParams_;
};
} // namespace Rosen
} // namespace OHOS
#endif // UIEFFECT_FILTER_FROSTED_GLASS_PARA_H