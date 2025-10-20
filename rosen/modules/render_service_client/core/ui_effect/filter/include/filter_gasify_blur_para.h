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
#ifndef UIEFFECT_FILTER_GASIFY_BLUR_PARA_H
#define UIEFFECT_FILTER_GASIFY_BLUR_PARA_H

#include "pixel_map.h"
#include "common/rs_macros.h"
#include "filter_para.h"
#include "ui_effect/mask/include/mask_para.h"
#include "ui_effect/utils.h"

namespace OHOS {
namespace Rosen {

class RSC_EXPORT GasifyBlurPara : public FilterPara {
public:
    GasifyBlurPara()
    {
        this->type_ = FilterPara::ParaType::GASIFY_BLUR;
    }
    ~GasifyBlurPara() override = default;

    GasifyBlurPara(const GasifyBlurPara& other);

    void SetProgress(float progress)
    {
        progress_ = progress;
    }

    float GetProgress() const
    {
        return progress_;
    }

    void SetSourceImage(std::shared_ptr<Media::PixelMap> sourceImage)
    {
        sourceImage_ = sourceImage;
    }

    const std::shared_ptr<Media::PixelMap>& GetSourceImage() const
    {
        return sourceImage_;
    }
    
    void SetMaskImage(std::shared_ptr<Media::PixelMap> maskImage)
    {
        maskImage_ = maskImage;
    }

    const std::shared_ptr<Media::PixelMap>& GetMaskImage() const
    {
        return maskImage_;
    }
    
    bool Marshalling(Parcel& parcel) const override;

    static void RegisterUnmarshallingCallback();

    [[nodiscard]] static bool OnUnmarshalling(Parcel& parcel, std::shared_ptr<FilterPara>& val);

    std::shared_ptr<FilterPara> Clone() const override;
private:
    float progress_ = 0.0f;
    std::shared_ptr<Media::PixelMap> sourceImage_;
    std::shared_ptr<Media::PixelMap> maskImage_;
};
} // namespace Rosen
} // namespace OHOS
#endif // UIEFFECT_FILTER_GASIFY_BLUR_PARA_H