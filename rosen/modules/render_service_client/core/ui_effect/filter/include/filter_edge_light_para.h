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
#ifndef UIEFFECT_FILTER_EDGE_LIGHT_PARA_H
#define UIEFFECT_FILTER_EDGE_LIGHT_PARA_H
#include "filter_para.h"
#include "ui_effect/mask/include/mask_para.h"
#include "ui_effect/mask/include/radial_gradient_mask_para.h"

namespace OHOS {
namespace Rosen {
class EdgeLightPara : public FilterPara {
public:
    EdgeLightPara()
    {
        this->type_ = FilterPara::ParaType::EDGE_LIGHT;
    }
    ~EdgeLightPara() override = default;

    void SetMask(std::shared_ptr<MaskPara> maskPara)
    {
        maskPara_ = maskPara;
    }

    const std::shared_ptr<MaskPara>& GetMask() const
    {
        return maskPara_;
    }

    void SetDetectColor(uint32_t detectColor)
    {
        detectColor_ = detectColor;
    }

    uint32_t GetDetectColor() const
    {
        return detectColor_;
    }

    void SetColor(uint32_t color)
    {
        color_ = color;
    }

    uint32_t GetColor() const
    {
        return color_;
    }

    void SetEdgeThreshold(float edgeThreshold)
    {
        edgeThreshold_ = edgeThreshold;
    }

    float GetEdgeThreshold() const
    {
        return edgeThreshold_;
    }

    void SetEdgeIntensity(float edgeIntensity)
    {
        edgeIntensity_ = edgeIntensity;
    }

    float GetEdgeIntensity() const
    {
        return edgeIntensity_;
    }

    void SetEdgeSoftThreshold(float edgeSoftThreshold)
    {
        edgeSoftThreshold_ = edgeSoftThreshold;
    }

    float GetEdgeSoftThreshold() const
    {
        return edgeSoftThreshold_;
    }

    void SetBloomLevel(int bloomLevel)
    {
        bloomLevel_ = bloomLevel;
    }

    int GetBloomLevel() const
    {
        return bloomLevel_;
    }

    void SetUseRawColor(bool useRawColor)
    {
        useRawColor_ = useRawColor;
    }

    bool GetUseRawColor() const
    {
        return useRawColor_;
    }

    void SetGradient(bool gradient)
    {
        gradient_ = gradient;
    }

    bool GetGradient() const
    {
        return gradient_;
    }

    void SetAlphaProgress(float alphaProgress)
    {
        alphaProgress_ = alphaProgress;
    }

    float GetAlphaProgress() const
    {
        return alphaProgress_;
    }

    void SetAddImage(bool addImage)
    {
        addImage_ = addImage;
    }

    bool GetAddImage() const
    {
        return addImage_;
    }

private:
    std::shared_ptr<MaskPara> maskPara_ = nullptr;
    uint32_t detectColor_ = 0;
    uint32_t color_ = 0;
    float edgeThreshold_ = 0.f;
    float edgeIntensity_ = 0.f;
    float edgeSoftThreshold_ = 0.f;
    int bloomLevel_ = 0;
    bool useRawColor_ = false;
    bool gradient_ = false;
    float alphaProgress_ = 0.f;
    bool addImage_ = false;
};
} // namespace Rosen
} // namespace OHOS
#endif // UIEFFECT_FILTER_EDGE_LIGHT_PARA_H
