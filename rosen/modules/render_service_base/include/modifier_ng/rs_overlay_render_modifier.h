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

#ifndef RENDER_SERVICE_BASE_MODIFIER_NG_RS_OVERLAY_RENDER_MODIFIER_H
#define RENDER_SERVICE_BASE_MODIFIER_NG_RS_OVERLAY_RENDER_MODIFIER_H

#include "animation/rs_render_particle.h"
#include "modifier_ng/rs_render_modifier_ng.h"

namespace OHOS::Rosen::ModifierNG {
class RSB_EXPORT RSParticleRenderModifier : public RSDisplayListRenderModifier {
public:
    RSParticleRenderModifier() = default;
    RSParticleRenderModifier(
        std::shared_ptr<OHOS::Rosen::RSRenderProperty<OHOS::Rosen::RSRenderParticleVector>> property)
    {
        properties_.emplace(RSPropertyType::PARTICLE, property);
    }
    ~RSParticleRenderModifier() override = default;

    static inline constexpr auto Type = ModifierNG::RSModifierType::PARTICLE_EFFECT;
    ModifierNG::RSModifierType GetType() const override
    {
        return Type;
    };

    void Draw(RSPaintFilterCanvas& canvas, Drawing::Rect& rect) override;
    void Purge() override;

    void ResetProperties(RSProperties& properties) override;

protected:
    bool OnApply(RSModifierContext& context) override;
    void OnSync() override;

private:
    static const LegacyPropertyApplierMap LegacyPropertyApplierMap_;
    const LegacyPropertyApplierMap& GetLegacyPropertyApplierMap() const override
    {
        return LegacyPropertyApplierMap_;
    }

    std::shared_ptr<Drawing::Image> MakeCircleImage(int radius);
    Drawing::RSXform MakeRSXform(Vector2f ofs, Vector2f position, float spin, float scale);
    void CaculatePointAtlsArry(
        const std::shared_ptr<RSRenderParticle>& particle, Vector2f position, float opacity, float scale);
    void CaculateImageAtlsArry(Drawing::Canvas& canvas, const std::shared_ptr<RSRenderParticle>& particle,
        Vector2f position, float opacity, float scale);
    void DrawImageFill(Drawing::Canvas& canvas, const std::shared_ptr<RSRenderParticle>& particle, Vector2f position,
        float opacity, float scale);
    void DrawParticles(Drawing::Canvas& canvas);
    void DrawCircle(Drawing::Canvas& canvas);
    void DrawImages(Drawing::Canvas& canvas);

    void ClearStagingValues();

    std::vector<std::shared_ptr<RSRenderParticle>> stagingParticles_;
    size_t stagingImageCount_;
    std::vector<std::shared_ptr<RSImage>> stagingImageVector_;
    std::vector<int> stagingCount_; // image numbers
    std::vector<std::vector<Drawing::RSXform>> stagingImageRsxform_;
    std::vector<std::vector<Drawing::Rect>> stagingImageTex_;
    std::vector<std::vector<Drawing::ColorQuad>> stagingImageColors_;

    std::vector<std::shared_ptr<RSRenderParticle>> particles_;
    size_t imageCount_;
    std::vector<std::shared_ptr<RSImage>> imageVector_;
    std::vector<int> count_; // image numbers
    std::vector<std::vector<Drawing::RSXform>> imageRsxform_;
    std::vector<std::vector<Drawing::Rect>> imageTex_;
    std::vector<std::vector<Drawing::ColorQuad>> imageColors_;

    std::shared_ptr<Drawing::Image> circleImage_;
    size_t pointCount_ = 0;
    std::vector<Drawing::RSXform> pointRsxform_;
    std::vector<Drawing::Rect> pointTex_;
    std::vector<Drawing::ColorQuad> pointColors_;
};
} // namespace OHOS::Rosen::ModifierNG
#endif // RENDER_SERVICE_BASE_MODIFIER_NG_RS_OVERLAY_RENDER_MODIFIER_H
