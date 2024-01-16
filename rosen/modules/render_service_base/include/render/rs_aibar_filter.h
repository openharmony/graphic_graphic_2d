/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
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
#ifndef RENDER_SERVICE_CLIENT_CORE_RENDER_RS_AIBAR_FILTER_H
#define RENDER_SERVICE_CLIENT_CORE_RENDER_RS_AIBAR_FILTER_H

#include "render/rs_skia_filter.h"

#ifndef USE_ROSEN_DRAWING
#include "include/effects/SkRuntimeEffect.h"
#else
#include "effect/runtime_effect.h"
#include "effect/runtime_shader_builder.h"
#endif

namespace OHOS {
namespace Rosen {

#ifndef USE_ROSEN_DRAWING
class RSB_EXPORT RSAIBarFilter : public RSSkiaFilter {
#else
class RSB_EXPORT RSAIBarFilter : public RSDrawingFilter {
#endif
public:
    RSAIBarFilter();
    RSAIBarFilter(const RSAIBarFilter&) = delete;
    RSAIBarFilter operator=(const RSAIBarFilter&) = delete;
    ~RSAIBarFilter() override;

#ifndef USE_ROSEN_DRAWING
    void DrawImageRect(
        SkCanvas& canvas, const sk_sp<SkImage>& image, const SkRect& src, const SkRect& dst) const override;
#else
    void DrawImageRect(Drawing::Canvas& canvas, const std::shared_ptr<Drawing::Image>& image,
        const Drawing::Rect& src, const Drawing::Rect& dst) const override;
#endif

#ifndef USE_ROSEN_DRAWING
    std::shared_ptr<RSSkiaFilter> Compose(const std::shared_ptr<RSSkiaFilter>& other) const override
    {
        return nullptr;
    };
    void PreProcess(sk_sp<SkImage> image) override {};
#else
    std::shared_ptr<RSDrawingFilter> Compose(const std::shared_ptr<RSDrawingFilter>& other) const override
    {
        return nullptr;
    };
    void PreProcess(std::shared_ptr<Drawing::Image> image) override {};
#endif

    void PostProcess(RSPaintFilterCanvas& canvas) override {};
    std::string GetDescription() override;

    bool CanSkipFrame() const override { return false; };
    static bool IsAiInvertCoefValid(const std::vector<float>& aiInvertCoef);
    static std::vector<float> GetAiInvertCoef();

private:
#ifndef USE_ROSEN_DRAWING
    static std::shared_ptr<SkRuntimeShaderBuilder> MakeBinarizationShader(float imageWidth, float imageHeight,
        sk_sp<SkShader> imageShader);
#else
    static std::shared_ptr<Drawing::RuntimeShaderBuilder> MakeBinarizationShader(float imageWidth, float imageHeight,
        std::shared_ptr<Drawing::ShaderEffect> imageShader);
#endif

private:
    friend class RSMarshallingHelper;
};
} // namespace Rosen
} // namespace OHOS

#endif // RENDER_SERVICE_CLIENT_CORE_RENDER_RS_BLUR_FILTER_H