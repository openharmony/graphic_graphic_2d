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
#ifndef RENDER_SERVICE_BASE_CORE_RENDER_RS_LIGHT_BLUR_SHADER_FILTER_H
#define RENDER_SERVICE_BASE_CORE_RENDER_RS_LIGHT_BLUR_SHADER_FILTER_H

#include "common/rs_macros.h"
#include "render/rs_shader_filter.h"
namespace OHOS {
namespace Rosen {
struct LightBlurParameter {
    Drawing::Rect src;
    Drawing::Rect dst;
    Drawing::Brush brush;
};

class RSB_EXPORT RSLightBlurShaderFilter : public RSShaderFilter {
public:
    RSLightBlurShaderFilter(int radius);
    ~RSLightBlurShaderFilter() = default;
    int GetRadius() const;
    void ApplyLightBlur(Drawing::Canvas& canvas, const std::shared_ptr<Drawing::Image>& image,
        const LightBlurParameter& param);
private:
    Drawing::Rect GetDownSampleRect(int width, int height) const;
    std::shared_ptr<Drawing::Image> GetDownSampleImage(Drawing::Image& srcImage,
        Drawing::Surface& surface, Drawing::Rect& src, Drawing::Rect& dst) const;
    RSColor MixColor(const RSColor& twoFrameBefore, const RSColor& oneFrameBefore, const RSColor& curColor) const;
    bool GetAverageColorFromImageAndCache(Drawing::Image& image, RSColor& color) const;
    void UpdateLightBlurResultCache(const RSColor& color);
    int radius_ {0};

    // save light blur result, if current is the nth light blur, first is the (n-2) result, second is n-1.
    std::unique_ptr<std::pair<RSColor, RSColor>> lightBlurResultCache_ {nullptr};
};
}
}

#endif // RENDER_SERVICE_BASE_CORE_RENDER_RS_LIGHT_BLUR_SHADER_FILTER_H