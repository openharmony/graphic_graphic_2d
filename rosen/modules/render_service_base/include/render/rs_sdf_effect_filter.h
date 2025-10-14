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

#ifndef RENDER_SERVICE_BASE_RENDER_RS_SDF_EFFECT_FILTER_H
#define RENDER_SERVICE_BASE_RENDER_RS_SDF_EFFECT_FILTER_H

#include <memory>
#include <optional>

#include "effect/runtime_effect.h"
#include "effect/runtime_shader_builder.h"
#include "render/rs_skia_filter.h"

namespace OHOS::GraphicsEffectEngine {
class GERender;
}

namespace OHOS {
namespace Rosen {
class RSNGRenderMaskBase;
namespace Drawing {
    class GEVisualEffectContainer;
    class GEVisualEffect;
}
struct SDFEffectParam {
    Drawing::Rect src;
    Drawing::Rect dst;

    SDFEffectParam(const Drawing::Rect& src, const Drawing::Rect& dst)
        : src(src), dst(dst) {}
};

class RSB_EXPORT RSSDFEffectFilter : public RSDrawingFilterOriginal {
public:
    RSSDFEffectFilter(std::shared_ptr<RSNGRenderMaskBase> SDFMask);
    RSSDFEffectFilter(const RSSDFEffectFilter&) = delete;
    RSSDFEffectFilter operator=(const RSSDFEffectFilter&) = delete;
    ~RSSDFEffectFilter() override;

    std::string GetDescription() override;
    void DrawImageRect(Drawing::Canvas& canvas, const std::shared_ptr<Drawing::Image>& image,
        const Drawing::Rect& src, const Drawing::Rect& dst) const override;
    void PreProcess(std::shared_ptr<Drawing::Image> image) override {};
    void PostProcess(Drawing::Canvas& canvas) override {};
    void OnSync() override;

    std::shared_ptr<RSDrawingFilterOriginal> Compose(
        const std::shared_ptr<RSDrawingFilterOriginal>& other) const override
    {
        return nullptr;
    }

    void SetBorder(const Drawing::Color& color, float width);
    [[nodiscard]] const Drawing::Color& GetBorderColor() const;
    [[nodiscard]] float GetBorderWidth() const;
    [[nodiscard]] bool HasBorder() const;

    void SetShadow(
        const Drawing::Color& color, float offsetX, float offsetY, float radius, Drawing::Path path, bool isFilled);
    [[nodiscard]] const Drawing::Color& GetShadowColor() const;
    [[nodiscard]] float GetShadowOffsetX() const;
    [[nodiscard]] float GetShadowOffsetY() const;
    [[nodiscard]] float GetShadowRadius() const;
    [[nodiscard]] Drawing::Path GetShadowPath() const;
    [[nodiscard]] bool GetShadowIsFill() const;
    [[nodiscard]] bool HasShadow() const;

private:
    std::shared_ptr<RSNGRenderMaskBase> SDFMask_;
    std::shared_ptr<Drawing::GEVisualEffect> geFilter_;
    std::shared_ptr<Drawing::GEVisualEffectContainer> geContainer_;
    std::shared_ptr<GraphicsEffectEngine::GERender> geRender_;

    Drawing::Color borderColor_;
    float borderWidth_ = 0.0f;
    bool hasBorder_ = false;

    Drawing::Color shadowColor_;
    float shadowOffsetX_ = 0.0f;
    float shadowOffsetY_ = 0.0f;
    float shadowRadius_ = 0.0f;
    Drawing::Path shadowPath_;
    bool isShadowFilled_ = false;
    bool hasShadow_ = false;

    friend class RSMarshallingHelper;
};
} // namespace Rosen
} // namespace OHOS

#endif // RENDER_SERVICE_BASE_RENDER_RS_SDF_EFFECT_FILTER_H
