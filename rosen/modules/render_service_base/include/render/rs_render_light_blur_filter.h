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
#include "render/rs_render_filter_base.h"
namespace OHOS {
namespace Rosen {
struct LightBlurParameter {
    Drawing::Rect src;
    Drawing::Rect dst;
    Drawing::Brush brush;
};

namespace Drawing {
class RuntimeEffect;
}

class RSB_EXPORT RSLightBlurShaderFilter : public RSRenderFilterParaBase {
public:
    RSLightBlurShaderFilter(int radius) : RSRenderFilterParaBase(RSUIFilterType::LIGHT_BLUR), radius_(radius) {}
    ~RSLightBlurShaderFilter() override = default;
    int GetRadius() const;
    void ApplyLightBlur(Drawing::Canvas& canvas, const std::shared_ptr<Drawing::Image>& image,
        const LightBlurParameter& param);
private:
    static bool InitDownSample4xAndMixShader();
    static bool InitDownSample4xShader();
    Drawing::RectI GetDownSampleRect(int width, int height) const;
    std::shared_ptr<Drawing::Image> GetDownSampleImage(Drawing::Image& srcImage,
        Drawing::Surface& surface, Drawing::RectI& src, Drawing::RectI& dst) const;
    std::shared_ptr<Drawing::Image> GetDownSampleImage4x(const std::shared_ptr<Drawing::Image>& srcImage,
        Drawing::Canvas& canvas, Drawing::RectI& src, Drawing::RectI& dst) const;
    std::shared_ptr<Drawing::Image> GetDownSample4xAndMixImage(Drawing::Canvas& canvas,
        const std::shared_ptr<Drawing::Image>& image) const;
    void UpdateLightBlurResultCache(const std::shared_ptr<Drawing::Image>& image);
    bool NeedClearLightBlurResultCache(Drawing::Canvas& canvas) const;
    void ClearLightBlurResultCache();
    void DrawImageOnCanvas(Drawing::Canvas& canvas, const Drawing::Image& image, const LightBlurParameter& param);

    static std::shared_ptr<Drawing::RuntimeEffect> downSample4xAndMixShader_;
    static std::shared_ptr<Drawing::RuntimeEffect> downSample4xShader_;

    int radius_ {0};
    // save light blur result, if current is the nth light blur, first is the (n-2) result, second is n-1.
    std::array<std::shared_ptr<Drawing::Image>, 2> lightBlurResultCache_ = {nullptr, nullptr};
};
}
}

#endif // RENDER_SERVICE_BASE_CORE_RENDER_RS_LIGHT_BLUR_SHADER_FILTER_H