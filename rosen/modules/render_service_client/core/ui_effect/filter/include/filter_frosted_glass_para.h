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

    // ===== Common parameters =====
    void SetBorderSize(Vector2f& borderSize)
    {
        borderSize_ = borderSize;
    }
    Vector2f GetBorderSize() const
    {
        return borderSize_;
    }

    void SetBlurParams(std::vector<float>& blurParams)
    {
        blurParams_ = blurParams;
    }
    std::vector<float> GetBlurParams() const
    {
        return blurParams_;
    }

    void SetCornerRadius(float cornerRadius)
    {
        cornerRadius_ = cornerRadius;
    }
    float GetCornerRadius() const
    {
        return cornerRadius_;
    }

    void SetBorderWidth(float borderWidth)
    {
        borderWidth_ = borderWidth;
    }
    float GetBorderWidth() const
    {
        return borderWidth_;
    }

    void SetOffset(float offset)
    {
        offset_ = offset;
    }
    float GetOffset() const
    {
        return offset_;
    }

    void SetDownSampleFactor(float downSampleFactor)
    {
        downSampleFactor_ = downSampleFactor;
    }
    float GetDownSampleFactor() const
    {
        return downSampleFactor_;
    }

    // ===== Background darken =====
    void SetBgFactor(float bgFactor)
    {
        bgFactor_ = bgFactor;
    }
    float GetBgFactor() const
    {
        return bgFactor_;
    }

    // ===== Inner shadow =====
    void SetInnerShadowParams(std::vector<float>& innerShadowParams)
    {
        innerShadowParams_ = innerShadowParams;
    }
    std::vector<float> GetInnerShadowParams() const
    {
        return innerShadowParams_;
    }

    void SetSdParams(std::vector<float>& sdParams)
    {
        sdParams_ = sdParams;
    }
    std::vector<float> GetSdParams() const
    {
        return sdParams_;
    }

    // ===== Env refraction =====
    void SetRefractOutPx(float refractOutPx)
    {
        refractOutPx_ = refractOutPx;
    }
    float GetRefractOutPx() const
    {
        return refractOutPx_;
    }

    void SetEnvParams(std::vector<float>& envParams)
    {
        envParams_ = envParams;
    }
    std::vector<float> GetEnvParams() const
    {
        return envParams_;
    }

    // ===== Edge highlights =====
    void SetHighLightParams(std::vector<float>& highLightParams)
    {
        highLightParams_ = highLightParams;
    }
    std::vector<float> GetHighLightParams() const
    {
        return highLightParams_;
    }

    void SetHlParams(std::vector<float>& hlParams)
    {
        hlParams_ = hlParams;
    }
    std::vector<float> GetHlParams() const
    {
        return hlParams_;
    }

private:
    // ===== Common parameters =====
    Vector2f borderSize_;
    std::vector<float> blurParams_;
    float cornerRadius_;
    float borderWidth_;
    float offset_;
    float downSampleFactor_;
    // ===== Background darken =====
    float bgFactor_ = 0.9f;
    // ===== Inner shadow =====
    std::vector<float> innerShadowParams_;
    std::vector<float> sdParams_;
    // ===== Env refraction =====
    float refractOutPx_ = 20.0f;
    std::vector<float> envParams_;
    // ===== Edge highlights =====
    std::vector<float> highLightParams_;
    std::vector<float> hlParams_;
};
} // namespace Rosen
} // namespace OHOS
#endif // UIEFFECT_FILTER_FROSTED_GLASS_PARA_H