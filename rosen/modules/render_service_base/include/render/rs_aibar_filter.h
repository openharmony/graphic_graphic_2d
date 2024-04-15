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

#include "effect/runtime_effect.h"
#include "effect/runtime_shader_builder.h"

namespace OHOS {
namespace Rosen {

class RSB_EXPORT RSAIBarFilter : public RSDrawingFilter {
public:
    RSAIBarFilter();
    RSAIBarFilter(const RSAIBarFilter&) = delete;
    RSAIBarFilter operator=(const RSAIBarFilter&) = delete;
    ~RSAIBarFilter() override;

    void DrawImageRect(Drawing::Canvas& canvas, const std::shared_ptr<Drawing::Image>& image,
        const Drawing::Rect& src, const Drawing::Rect& dst) const override;

    std::shared_ptr<RSDrawingFilter> Compose(const std::shared_ptr<RSDrawingFilter>& other) const override
    {
        return nullptr;
    };
    void PreProcess(std::shared_ptr<Drawing::Image> image) override {};

    void PostProcess(Drawing::Canvas& canvas) override {};
    std::string GetDescription() override;
    std::string GetDetailedDescription() override;

    bool CanSkipFrame() const override { return false; };
    static bool IsAiInvertCoefValid(const std::vector<float>& aiInvertCoef);
    static std::vector<float> GetAiInvertCoef();

private:
    static std::shared_ptr<Drawing::RuntimeShaderBuilder> MakeBinarizationShader(float imageWidth, float imageHeight,
        std::shared_ptr<Drawing::ShaderEffect> imageShader);

private:
    friend class RSMarshallingHelper;
};
} // namespace Rosen
} // namespace OHOS

#endif // RENDER_SERVICE_CLIENT_CORE_RENDER_RS_BLUR_FILTER_H