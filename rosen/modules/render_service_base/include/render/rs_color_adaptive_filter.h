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
#ifndef RENDER_SERVICE_BASE_RENDER_RS_COLOR_ADAPTIVE_FILTER_H
#define RENDER_SERVICE_BASE_RENDER_RS_COLOR_ADAPTIVE_FILTER_H

#include "rs_skia_filter.h"

#include "draw/brush.h"
#include "draw/color.h"
#include "effect/runtime_effect.h"
#include "effect/runtime_shader_builder.h"
#include "pipeline/rs_paint_filter_canvas.h"

namespace OHOS {
namespace Rosen {
/**
 * @brief A foreground filter that adaptively changes the color of grayscale pixels
 *
 */
class RSColorAdaptiveFilter : public RSDrawingFilterOriginal {
public:
    RSColorAdaptiveFilter() : RSDrawingFilterOriginal(nullptr) {}
    RSColorAdaptiveFilter(const RSColorAdaptiveFilter&) = delete;
    RSColorAdaptiveFilter operator=(const RSColorAdaptiveFilter&) = delete;
    ~RSColorAdaptiveFilter() override {}

    bool IsValid() const override
    {
        return true;
    }
    std::string GetDescription() override
    {
        return "RSColorAdaptiveFilter";
    }
    void UpdateColor(Drawing::ColorQuad color) override
    {
        grayScale_ = static_cast<float>(Drawing::Color::ColorQuadGetB(color)) / Drawing::Color::RGB_MAX;
    }
    void DrawImageRect(Drawing::Canvas& canvas, const std::shared_ptr<Drawing::Image>& image, const Drawing::Rect& src,
        const Drawing::Rect& dst) const override;

    std::shared_ptr<RSDrawingFilterOriginal> Compose(
        const std::shared_ptr<RSDrawingFilterOriginal>& other) const override
    {
        return nullptr;
    }

    static std::shared_ptr<Drawing::Image> ApplyFilter(
        std::shared_ptr<Drawing::GPUContext>, std::shared_ptr<Drawing::Image> image, float grayScale);

private:
    float grayScale_; // gray color scale in range [0.0, 1.0]
};
} // namespace Rosen
} // namespace OHOS

#endif // RENDER_SERVICE_BASE_RENDER_RS_COLOR_ADAPTIVE_FILTER_H
