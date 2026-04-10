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
#ifndef UIEFFECT_FILTER_BLUR_BUBBLES_RISE_PARA_H
#define UIEFFECT_FILTER_BLUR_BUBBLES_RISE_PARA_H

#include <cstdint>

#include "filter_para.h"
#include "pixel_map.h"

namespace OHOS {
namespace Rosen {
class RSC_EXPORT BlurBubblesRisePara : public FilterPara {
public:
    BlurBubblesRisePara()
    {
        this->type_ = FilterPara::ParaType::BLUR_BUBBLES_RISE;
    }
    ~BlurBubblesRisePara() override = default;

    void SetBlurRadius(float blurRadius)
    {
        blurRadius_ = blurRadius;
    }

    float GetBlurRadius() const
    {
        return blurRadius_;
    }

    void SetMixStrength(float mixStrength)
    {
        mixStrength_ = mixStrength;
    }

    float GetMixStrength() const
    {
        return mixStrength_;
    }

    void SetProgress(float progress)
    {
        progress_ = progress;
    }

    float GetProgress() const
    {
        return progress_;
    }

    void SetMaskImage(std::shared_ptr<Media::PixelMap> maskImage)
    {
        maskImage_ = maskImage;
    }

    const std::shared_ptr<Media::PixelMap>& GetMaskImage() const
    {
        return maskImage_;
    }

private:
    float blurRadius_ = 3.0f;
    float mixStrength_ = 1.0f;
    float progress_ = 0.0f;
    std::shared_ptr<Media::PixelMap> maskImage_;
};
} // namespace Rosen
} // namespace OHOS

#endif // UIEFFECT_FILTER_BLUR_BUBBLES_RISE_PARA_H
