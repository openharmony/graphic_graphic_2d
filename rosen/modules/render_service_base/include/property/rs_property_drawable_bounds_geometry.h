/*
 * Copyright (c) 2023 Huawei Device Co., Ltd.
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

#ifndef RENDER_SERVICE_BASE_PROPERTY_RS_PROPERTY_DRAWABLE_BOUNDS_GEOMETRY_H
#define RENDER_SERVICE_BASE_PROPERTY_RS_PROPERTY_DRAWABLE_BOUNDS_GEOMETRY_H

#include <utility>

#include "include/core/SkMatrix.h"
#include "include/core/SkPath.h"
#include "include/core/SkRRect.h"

#include "property/rs_property_drawable.h"

namespace OHOS::Rosen {
class RSSkiaFilter;

class RSBoundsGeometryDrawable : public RSPropertyDrawable {
public:
    explicit RSBoundsGeometryDrawable(const SkMatrix& boundsMatrix);
    ~RSBoundsGeometryDrawable() override = default;
    void Draw(RSPropertyDrawableRenderContext& context) override;

    static RSPropertyDrawable::DrawablePtr Generate(const RSPropertyDrawableGenerateContext& context);

private:
    SkMatrix boundsMatrix_;
};

class RSClipBoundsDrawable : public RSPropertyDrawable {
public:
    explicit RSClipBoundsDrawable() = default;
    ~RSClipBoundsDrawable() override = default;
    void Draw(RSPropertyDrawableRenderContext& context) override;

    static RSPropertyDrawable::DrawablePtr Generate(const RSPropertyDrawableGenerateContext& context);
};

// ============================================================================
// Border
class RSBorderDrawable : public RSPropertyDrawable {
public:
    explicit RSBorderDrawable(SkPaint&& paint) : RSPropertyDrawable(), paint_(std::move(paint)) {}
    ~RSBorderDrawable() override = default;
    static RSPropertyDrawable::DrawablePtr Generate(const RSPropertyDrawableGenerateContext& context);

protected:
    SkPaint paint_;
};

class RSBorderDRRectDrawable : public RSBorderDrawable {
public:
    explicit RSBorderDRRectDrawable(SkPaint&& paint, const RSProperties& properties);
    ~RSBorderDRRectDrawable() override = default;
    void OnGeometryChange(const RSProperties& properties) override;
    void Draw(RSPropertyDrawableRenderContext& context) override;

private:
    SkRRect inner_;
    SkRRect outer_;
};

class RSBorderFourLineDrawable : public RSBorderDrawable {
public:
    explicit RSBorderFourLineDrawable(SkPaint&& paint, const RSProperties& properties);
    ~RSBorderFourLineDrawable() override = default;
    void OnGeometryChange(const RSProperties& properties) override;
    void Draw(RSPropertyDrawableRenderContext& context) override;

private:
    RectF rect_;
};

class RSBorderPathDrawable : public RSBorderDrawable {
public:
    explicit RSBorderPathDrawable(SkPaint&& paint, const RSProperties& properties);
    ~RSBorderPathDrawable() override = default;
    void OnGeometryChange(const RSProperties& properties) override;
    void Draw(RSPropertyDrawableRenderContext& context) override;

private:
    SkPath borderPath_;
};

class RSBorderFourLineRoundCornerDrawable : public RSBorderDrawable {
public:
    explicit RSBorderFourLineRoundCornerDrawable(SkPaint&& paint, const RSProperties& properties);
    ~RSBorderFourLineRoundCornerDrawable() override = default;
    void OnGeometryChange(const RSProperties& properties) override;
    void Draw(RSPropertyDrawableRenderContext& context) override;

private:
    SkRRect innerRrect_;
    SkRRect rrect_;
};

// ============================================================================
// Mask
class RSMaskDrawable : public RSPropertyDrawable {
public:
    explicit RSMaskDrawable(std::shared_ptr<RSMask> mask, RectF bounds);
    ~RSMaskDrawable() override = default;
    static std::unique_ptr<RSPropertyDrawable> Generate(const RSPropertyDrawableGenerateContext& context);

protected:
    SkRect maskBounds_;
    std::shared_ptr<RSMask> mask_;
    SkPaint maskFilter_;
    SkPaint maskPaint_;
};

class RSSvgDomMaskDrawable : public RSMaskDrawable {
public:
    explicit RSSvgDomMaskDrawable(std::shared_ptr<RSMask> mask, RectF bounds);
    ~RSSvgDomMaskDrawable() override = default;
    void Draw(RSPropertyDrawableRenderContext& context) override;
};

class RSSvgPictureMaskDrawable : public RSMaskDrawable {
public:
    explicit RSSvgPictureMaskDrawable(std::shared_ptr<RSMask> mask, RectF bounds);
    ~RSSvgPictureMaskDrawable() override = default;
    void Draw(RSPropertyDrawableRenderContext& context) override;
};

class RSGradientMaskDrawable : public RSMaskDrawable {
public:
    explicit RSGradientMaskDrawable(std::shared_ptr<RSMask> mask, RectF bounds);
    ~RSGradientMaskDrawable() override = default;
    void Draw(RSPropertyDrawableRenderContext& context) override;
};

class RSPathMaskDrawable : public RSMaskDrawable {
public:
    explicit RSPathMaskDrawable(std::shared_ptr<RSMask> mask, RectF bounds);
    ~RSPathMaskDrawable() override = default;
    void Draw(RSPropertyDrawableRenderContext& context) override;
};

// ============================================================================
// Shadow
class RSShadowBaseDrawable : public RSPropertyDrawable {
public:
    explicit RSShadowBaseDrawable(SkPath skPath, const RSProperties& properties);
    ~RSShadowBaseDrawable() override = default;
    static std::unique_ptr<RSPropertyDrawable> Generate(const RSPropertyDrawableGenerateContext& context);

protected:
    SkPath skPath_;
    float offsetX_;
    float offsetY_;
    Color color_;
};

class RSShadowDrawable : public RSShadowBaseDrawable {
public:
    explicit RSShadowDrawable(SkPath skPath, const RSProperties& properties);
    ~RSShadowDrawable() override = default;
    void Draw(RSPropertyDrawableRenderContext& context) override;

protected:
    float radius_;
};

class RSColorfulShadowDrawable : public RSShadowBaseDrawable {
public:
    explicit RSColorfulShadowDrawable(SkPath skPath, const RSProperties& properties);
    ~RSColorfulShadowDrawable() override = default;
    void Draw(RSPropertyDrawableRenderContext& context) override;

protected:
    SkPaint blurPaint_;
    std::weak_ptr<RSRenderNode> node_;
};

class RSHardwareAccelerationShadowDrawable : public RSShadowBaseDrawable {
public:
    explicit RSHardwareAccelerationShadowDrawable(SkPath skPath, const RSProperties& properties);
    ~RSHardwareAccelerationShadowDrawable() override = default;
    void Draw(RSPropertyDrawableRenderContext& context) override;

protected:
    float shadowElevation_;
};

// ============================================================================
// DynamicLightUp
class RSDynamicLightUpDrawable : public RSPropertyDrawable {
public:
    explicit RSDynamicLightUpDrawable(float rate, float degree);
    ~RSDynamicLightUpDrawable() override = default;
    static std::unique_ptr<RSPropertyDrawable> Generate(const RSPropertyDrawableGenerateContext& context);
    void Draw(RSPropertyDrawableRenderContext& context) override;

protected:
    float rate_;
    float degree_;
};

// ============================================================================
// RSEffectDataApplyDrawable
class RSEffectDataApplyDrawable : public RSPropertyDrawable {
public:
    explicit RSEffectDataApplyDrawable() = default;
    ~RSEffectDataApplyDrawable() override = default;
    static std::unique_ptr<RSPropertyDrawable> Generate(const RSPropertyDrawableGenerateContext& context);
    void Draw(RSPropertyDrawableRenderContext& context) override;
};

// ============================================================================
// LightUpEffect
class RSLightUpEffectDrawable : public RSPropertyDrawable {
public:
    explicit RSLightUpEffectDrawable() = default;
    ~RSLightUpEffectDrawable() override = default;
    void Draw(RSPropertyDrawableRenderContext& context) override;
    static std::unique_ptr<RSPropertyDrawable> Generate(const RSPropertyDrawableGenerateContext& context);
};

class RSFilterDrawable : public RSPropertyDrawable {
public:
    enum FilterType {
        BACKGROUND_FILTER,
        FOREGROUND_FILTER,
    };
    RSFilterDrawable() = default;
    ~RSFilterDrawable() override = default;
    void DrawFilter(RSModifierContext& context, FilterType filterType, const std::shared_ptr<RSFilter>& rsFilter);
    static bool GetBlurEnabled();
};

// ============================================================================
// BackgroundFilter
class RSBackgroundFilterDrawable : public RSFilterDrawable {
public:
    explicit RSBackgroundFilterDrawable(const std::shared_ptr<RSFilter>& filter) : filter_(filter) {}
    ~RSBackgroundFilterDrawable() override = default;
    static std::unique_ptr<RSPropertyDrawable> Generate(const RSPropertyDrawableGenerateContext& context);
    void Draw(RSPropertyDrawableRenderContext& context) override;

private:
    const std::shared_ptr<RSFilter>& filter_;
};

// ============================================================================
// ForegroundFilter
class RSForegroundFilterDrawable : public RSFilterDrawable {
public:
    explicit RSForegroundFilterDrawable(const std::shared_ptr<RSFilter>& filter) : filter_(filter) {}
    ~RSForegroundFilterDrawable() override = default;
    static std::unique_ptr<RSPropertyDrawable> Generate(const RSPropertyDrawableGenerateContext& context);
    void Draw(RSPropertyDrawableRenderContext& context) override;

private:
    const std::shared_ptr<RSFilter>& filter_;
};

// ============================================================================
// LinearGradientBlurFilter
class RSLinearGradientBlurFilterDrawable : public RSPropertyDrawable {
public:
    explicit RSLinearGradientBlurFilterDrawable(const std::shared_ptr<RSLinearGradientBlurPara>& para) : para_(para) {}
    ~RSLinearGradientBlurFilterDrawable() override = default;
    void Draw(RSPropertyDrawableRenderContext& context) override;
    static std::unique_ptr<RSPropertyDrawable> Generate(const RSPropertyDrawableGenerateContext& context);

private:
    const std::shared_ptr<RSLinearGradientBlurPara>& para_;
};

// ============================================================================
// ForegroundColor
class RSForegroundColorDrawable : public RSPropertyDrawable {
public:
    explicit RSForegroundColorDrawable(SkPaint&& paint) : paint_(std::move(paint)) {}
    ~RSForegroundColorDrawable() override = default;
    void Draw(RSPropertyDrawableRenderContext& context) override;
    static std::unique_ptr<RSPropertyDrawable> Generate(const RSPropertyDrawableGenerateContext& context);

private:
    SkPaint paint_;
};

// ============================================================================
// Particle
class RSParticleDrawable : public RSPropertyDrawable {
public:
    explicit RSParticleDrawable() = default;
    ~RSParticleDrawable() override = default;
    void AddPropertyDrawable(std::shared_ptr<RSPropertyDrawable> drawable);
    static std::unique_ptr<RSPropertyDrawable> Generate(const RSPropertyDrawableGenerateContext& context);
    void Draw(RSPropertyDrawableRenderContext& context) override;

private:
    std::vector<std::shared_ptr<RSPropertyDrawable>> drawableVec_ = {};
};

class RSPointParticleDrawable : public RSPropertyDrawable {
public:
    explicit RSPointParticleDrawable(SkPaint&& paint, std::shared_ptr<RSRenderParticle> particles, SkRect bounds)
        : paint_(std::move(paint)), particles_(std::move(particles)), bounds_(bounds)
    {}
    ~RSPointParticleDrawable() override = default;
    void Draw(RSPropertyDrawableRenderContext& context) override;

private:
    SkPaint paint_;
    std::shared_ptr<RSRenderParticle> particles_;
    SkRect bounds_;
};

class RSImageParticleDrawable : public RSPropertyDrawable {
public:
    explicit RSImageParticleDrawable(SkPaint&& paint, std::shared_ptr<RSRenderParticle> particles, SkRect bounds)
        : paint_(std::move(paint)), particles_(std::move(particles)), bounds_(bounds)
    {}
    ~RSImageParticleDrawable() override = default;
    void Draw(RSPropertyDrawableRenderContext& context) override;

private:
    SkPaint paint_;
    std::shared_ptr<RSRenderParticle> particles_;
    SkRect bounds_;
};

// ============================================================================
// PixelStretch
class RSPixelStretchDrawable : public RSPropertyDrawable {
public:
    explicit RSPixelStretchDrawable(const Vector4f& pixelStretch, SkRect bounds)
        : pixelStretch_(pixelStretch), bounds_(bounds)
    {}
    ~RSPixelStretchDrawable() override = default;
    void Draw(RSPropertyDrawableRenderContext& context) override;
    static std::unique_ptr<RSPropertyDrawable> Generate(const RSPropertyDrawableGenerateContext& context);

private:
    const Vector4f& pixelStretch_;
    SkRect bounds_;
};

// ============================================================================
// Background
class RSBackgroundDrawable : public RSPropertyDrawable {
public:
    explicit RSBackgroundDrawable(const std::shared_ptr<RSShader>& bgShader, bool isTransparent, SkPaint&& paint)
        : bgShader_(bgShader), isTransparent_(isTransparent), paint_(std::move(paint))
    {}
    ~RSBackgroundDrawable() override = default;
    void Draw(RSPropertyDrawableRenderContext& context) override;
    static void setForceBgAntiAlias(bool antiAlias);
    static std::unique_ptr<RSPropertyDrawable> Generate(const RSPropertyDrawableGenerateContext& context);

private:
    const std::shared_ptr<RSShader>& bgShader_;
    bool isTransparent_ = false;
    SkPaint paint_;
    static bool forceBgAntiAlias_;
};

// ============================================================================
// BackgroundEffect
class RSEffectDataGenerateDrawable : public RSPropertyDrawable {
public:
    explicit RSEffectDataGenerateDrawable(std::shared_ptr<RSSkiaFilter>&& filter) : filter_(std::move(filter)) {}
    ~RSEffectDataGenerateDrawable() override = default;
    void Draw(RSPropertyDrawableRenderContext& context) override;
    static void setForceBgAntiAlias(bool antiAlias);
    static std::unique_ptr<RSPropertyDrawable> Generate(const RSPropertyDrawableGenerateContext& context);

private:
    std::shared_ptr<RSSkiaFilter> filter_ = nullptr;
};

};     // namespace OHOS::Rosen
#endif // RENDER_SERVICE_BASE_PROPERTY_RS_PROPERTY_DRAWABLE_BOUNDS_GEOMETRY_H
