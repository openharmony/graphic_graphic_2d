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

#include "include/core/SkPaint.h"

#include "property/rs_property_drawable.h"

namespace OHOS::Rosen {
class RSSkiaFilter;
enum class FilterType;

class RSBoundsGeometryDrawable : public RSPropertyDrawable {
public:
    explicit RSBoundsGeometryDrawable() = default;
    ~RSBoundsGeometryDrawable() override = default;
    void Draw(const RSRenderContent& content, RSPaintFilterCanvas& canvas) const override;

    static RSPropertyDrawable::DrawablePtr Generate(const RSRenderContent& content);
};

class RSClipBoundsDrawable : public RSPropertyDrawable {
public:
    explicit RSClipBoundsDrawable() = default;
    ~RSClipBoundsDrawable() override = default;
    void Draw(const RSRenderContent& content, RSPaintFilterCanvas& canvas) const override;

    static RSPropertyDrawable::DrawablePtr Generate(const RSRenderContent& content);
};

// ============================================================================
// PointLight
class RSPointLightDrawable : public RSPropertyDrawable {
public:
    explicit RSPointLightDrawable() = default;
    ~RSPointLightDrawable() override = default;
    void Draw(const RSRenderContent& content, RSPaintFilterCanvas& canvas) const override;

    static RSPropertyDrawable::DrawablePtr Generate(const RSRenderContent& content);
};

// ============================================================================
// Border
class RSBorderDrawable : public RSPropertyDrawable {
public:
#ifndef USE_ROSEN_DRAWING
    explicit RSBorderDrawable(SkPaint&& paint) : RSPropertyDrawable(), paint_(std::move(paint)) {}
#else
    explicit RSBorderDrawable(Drawing::Brush&& brush, Drawing::Pen&& pen)
        : RSPropertyDrawable(), brush_(std::move(brush)), pen_(std::move(pen))
    {}
#endif
    ~RSBorderDrawable() override = default;
    static RSPropertyDrawable::DrawablePtr Generate(const RSRenderContent& content);

protected:
#ifndef USE_ROSEN_DRAWING
    SkPaint paint_;
#else
    Drawing::Brush brush_;
    Drawing::Pen pen_;
#endif
};

class RSBorderDRRectDrawable : public RSBorderDrawable {
public:
#ifndef USE_ROSEN_DRAWING
    explicit RSBorderDRRectDrawable(SkPaint&& paint, const RSProperties& properties, const bool& drawBorder);
#else
    explicit RSBorderDRRectDrawable(Drawing::Brush&& brush, Drawing::Pen&& pen,
        const RSProperties& properties, const bool& drawBorder);
#endif
    ~RSBorderDRRectDrawable() override = default;
    void Draw(const RSRenderContent& content, RSPaintFilterCanvas& canvas) const override;
    void OnBoundsChange(const RSProperties& properties);

private:
    bool drawBorder_;
#ifndef USE_ROSEN_DRAWING
    SkRRect inner_;
    SkRRect outer_;
#else
    Drawing::RoundRect inner_;
    Drawing::RoundRect outer_;
#endif
};

class RSBorderFourLineDrawable : public RSBorderDrawable {
public:
#ifndef USE_ROSEN_DRAWING
    explicit RSBorderFourLineDrawable(SkPaint&& paint, const RSProperties& properties, const bool& drawBorder);
#else
    explicit RSBorderFourLineDrawable(Drawing::Brush&& brush, Drawing::Pen&& pen,
        const RSProperties& properties, const bool& drawBorder);
#endif
    ~RSBorderFourLineDrawable() override = default;
    void Draw(const RSRenderContent& content, RSPaintFilterCanvas& canvas) const override;
    void OnBoundsChange(const RSProperties& properties);

private:
    bool drawBorder_;
    RectF rect_;
};

class RSBorderPathDrawable : public RSBorderDrawable {
public:
#ifndef USE_ROSEN_DRAWING
    explicit RSBorderPathDrawable(SkPaint&& paint, const RSProperties& properties, const bool& drawBorder);
#else
    explicit RSBorderPathDrawable(Drawing::Brush&& brush, Drawing::Pen&& pen,
        const RSProperties& properties, const bool& drawBorder);
#endif
    ~RSBorderPathDrawable() override = default;
    void Draw(const RSRenderContent& content, RSPaintFilterCanvas& canvas) const override;
    void OnBoundsChange(const RSProperties& properties);

private:
    bool drawBorder_;
#ifndef USE_ROSEN_DRAWING
    SkPath borderPath_;
#else
    Drawing::Path borderPath_;
#endif
};

class RSBorderFourLineRoundCornerDrawable : public RSBorderDrawable {
public:
#ifndef USE_ROSEN_DRAWING
    explicit RSBorderFourLineRoundCornerDrawable(SkPaint&& paint, const RSProperties& properties,
        const bool& drawBorder);
#else
    explicit RSBorderFourLineRoundCornerDrawable(Drawing::Brush&& brush, Drawing::Pen&& pen,
        const RSProperties& properties, const bool& drawBorder);
#endif
    ~RSBorderFourLineRoundCornerDrawable() override = default;
    void Draw(const RSRenderContent& content, RSPaintFilterCanvas& canvas) const override;
    void OnBoundsChange(const RSProperties& properties);

private:
    bool drawBorder_;
#ifndef USE_ROSEN_DRAWING
    SkRRect innerRrect_;
    SkRRect rrect_;
#else
    Drawing::RoundRect innerRrect_;
    Drawing::RoundRect rrect_;
#endif
};

// ============================================================================
// Outline
class RSOutlineDrawable : public RSBorderDrawable {
public:
#ifndef USE_ROSEN_DRAWING
    explicit RSOutlineDrawable(SkPaint&& paint) : RSBorderDrawable(std::move(paint)) {}
#else
    explicit RSOutlineDrawable(Drawing::Brush&& brush, Drawing::Pen&& pen)
        : RSBorderDrawable(std::move(brush), std::move(pen))
    {}
#endif
    ~RSOutlineDrawable() override = default;
    static RSPropertyDrawable::DrawablePtr Generate(const RSRenderContent& content);
};

// ============================================================================
// Mask
class RSMaskDrawable : public RSPropertyDrawable {
public:
    explicit RSMaskDrawable(std::shared_ptr<RSMask> mask);
    ~RSMaskDrawable() override = default;
    static RSPropertyDrawable::DrawablePtr Generate(const RSRenderContent& content);

protected:
    std::shared_ptr<RSMask> mask_;
#ifndef USE_ROSEN_DRAWING
    SkPaint maskFilter_;
    SkPaint maskPaint_;
#else
    Drawing::Brush maskFilterBrush_;
    Drawing::Brush maskBrush_;
#endif
};

class RSSvgDomMaskDrawable : public RSMaskDrawable {
public:
    explicit RSSvgDomMaskDrawable(std::shared_ptr<RSMask> mask);
    ~RSSvgDomMaskDrawable() override = default;
    void Draw(const RSRenderContent& content, RSPaintFilterCanvas& canvas) const override;
};

class RSSvgPictureMaskDrawable : public RSMaskDrawable {
public:
    explicit RSSvgPictureMaskDrawable(std::shared_ptr<RSMask> mask);
    ~RSSvgPictureMaskDrawable() override = default;
    void Draw(const RSRenderContent& content, RSPaintFilterCanvas& canvas) const override;
};

class RSGradientMaskDrawable : public RSMaskDrawable {
public:
    explicit RSGradientMaskDrawable(std::shared_ptr<RSMask> mask);
    ~RSGradientMaskDrawable() override = default;
    void Draw(const RSRenderContent& content, RSPaintFilterCanvas& canvas) const override;
};

class RSPathMaskDrawable : public RSMaskDrawable {
public:
    explicit RSPathMaskDrawable(std::shared_ptr<RSMask> mask);
    ~RSPathMaskDrawable() override = default;
    void Draw(const RSRenderContent& content, RSPaintFilterCanvas& canvas) const override;
};

// ============================================================================
// Shadow
class RSShadowBaseDrawable : public RSPropertyDrawable {
public:
    explicit RSShadowBaseDrawable(const RSProperties& properties);
    ~RSShadowBaseDrawable() override = default;
    static RSPropertyDrawable::DrawablePtr Generate(const RSRenderContent& content);

protected:
#ifndef USE_ROSEN_DRAWING
    void ClipShadowPath(const RSRenderContent& content, RSPaintFilterCanvas& canvas, SkPath& skPath) const;
#else
    void ClipShadowPath(const RSRenderContent& content, RSPaintFilterCanvas& canvas, Drawing::Path& path) const;
#endif
    float offsetX_;
    float offsetY_;
    const Color color_;
};

class RSShadowDrawable : public RSShadowBaseDrawable {
public:
    explicit RSShadowDrawable(const RSProperties& properties);
    ~RSShadowDrawable() override = default;
#ifndef USE_ROSEN_DRAWING
    RSColor GetColorForShadow(const RSRenderContent& content, RSPaintFilterCanvas& canvas,
        SkPath& skPath, SkMatrix& matrix, SkIRect& deviceClipBounds) const;
#else
    RSColor GetColorForShadow(const RSRenderContent& content, RSPaintFilterCanvas& canvas,
        Drawing::Path& skPath, Drawing::Matrix& matrix, Drawing::RectI& deviceClipBounds) const;
#endif
    void Draw(const RSRenderContent& content, RSPaintFilterCanvas& canvas) const override;

protected:
    const float radius_;
};

class RSColorfulShadowDrawable : public RSShadowBaseDrawable {
public:
    explicit RSColorfulShadowDrawable(const RSProperties& properties);
    ~RSColorfulShadowDrawable() override = default;
    void Draw(const RSRenderContent& content, RSPaintFilterCanvas& canvas) const override;

protected:
#ifndef USE_ROSEN_DRAWING
    SkPaint blurPaint_;
#else
    Drawing::Brush blurBrush_;
#endif
    std::weak_ptr<RSRenderNode> node_;
};

class RSHardwareAccelerationShadowDrawable : public RSShadowBaseDrawable {
public:
    explicit RSHardwareAccelerationShadowDrawable(const RSProperties& properties);
    ~RSHardwareAccelerationShadowDrawable() override = default;
    void Draw(const RSRenderContent& content, RSPaintFilterCanvas& canvas) const override;

protected:
    const float shadowElevation_;
};

// ============================================================================
// DynamicLightUp
class RSDynamicLightUpDrawable : public RSPropertyDrawable {
public:
    explicit RSDynamicLightUpDrawable() = default;
    ~RSDynamicLightUpDrawable() override = default;
    static RSPropertyDrawable::DrawablePtr Generate(const RSRenderContent& content);
    void Draw(const RSRenderContent& content, RSPaintFilterCanvas& canvas) const override;
    bool Update(const RSRenderContent& content) override;
};

// ============================================================================
// LightUpEffect
class RSLightUpEffectDrawable : public RSPropertyDrawable {
public:
    explicit RSLightUpEffectDrawable() = default;
    ~RSLightUpEffectDrawable() override = default;
    void Draw(const RSRenderContent& content, RSPaintFilterCanvas& canvas) const override;
    static RSPropertyDrawable::DrawablePtr Generate(const RSRenderContent& content);
    bool Update(const RSRenderContent& content) override;
};

// ============================================================================
// BackgroundFilter
class RSBackgroundFilterDrawable : public RSPropertyDrawable {
public:
    explicit RSBackgroundFilterDrawable() = default;
    ~RSBackgroundFilterDrawable() override = default;
    static RSPropertyDrawable::DrawablePtr Generate(const RSRenderContent& content);
    void Draw(const RSRenderContent& content, RSPaintFilterCanvas& canvas) const override;
    bool Update(const RSRenderContent& content) override;
};

// ForegroundFilter
class RSForegroundFilterDrawable : public RSPropertyDrawable {
public:
    explicit RSForegroundFilterDrawable() = default;
    ~RSForegroundFilterDrawable() override = default;
    static RSPropertyDrawable::DrawablePtr Generate(const RSRenderContent& content);
    void Draw(const RSRenderContent& content, RSPaintFilterCanvas& canvas) const override;
    bool Update(const RSRenderContent& content) override;
};

// EffectDataGenerate
class RSEffectDataGenerateDrawable : public RSPropertyDrawable {
public:
    explicit RSEffectDataGenerateDrawable() = default;
    ~RSEffectDataGenerateDrawable() override = default;
    void Draw(const RSRenderContent& content, RSPaintFilterCanvas& canvas) const override;
    bool Update(const RSRenderContent& content) override;
};

// RSEffectDataApply
class RSEffectDataApplyDrawable : public RSPropertyDrawable {
public:
    explicit RSEffectDataApplyDrawable() = default;
    ~RSEffectDataApplyDrawable() override = default;
    static RSPropertyDrawable::DrawablePtr Generate(const RSRenderContent& content);
    void Draw(const RSRenderContent& content, RSPaintFilterCanvas& canvas) const override;
    bool Update(const RSRenderContent& content) override;
};

// ============================================================================
// ForegroundColor
class RSForegroundColorDrawable : public RSPropertyDrawable {
public:
#ifndef USE_ROSEN_DRAWING
    explicit RSForegroundColorDrawable(SkPaint&& paint) : paint_(std::move(paint)) {}
#else
    explicit RSForegroundColorDrawable(Drawing::Brush&& brush) : brush_(std::move(brush)) {}
#endif
    ~RSForegroundColorDrawable() override = default;
    void Draw(const RSRenderContent& content, RSPaintFilterCanvas& canvas) const override;
    static RSPropertyDrawable::DrawablePtr Generate(const RSRenderContent& content);
    bool Update(const RSRenderContent& content) override;

private:
#ifndef USE_ROSEN_DRAWING
    SkPaint paint_;
#else
    Drawing::Brush brush_;
#endif
};

// ============================================================================
// Particle
class RSParticleDrawable : public RSPropertyDrawable {
public:
    explicit RSParticleDrawable() = default;
    ~RSParticleDrawable() override = default;
    static RSPropertyDrawable::DrawablePtr Generate(const RSRenderContent& content);
    void Draw(const RSRenderContent& content, RSPaintFilterCanvas& canvas) const override;
    bool Update(const RSRenderContent& content) override;
};

// ============================================================================
// PixelStretch
class RSPixelStretchDrawable : public RSPropertyDrawable {
public:
    explicit RSPixelStretchDrawable() = default;
    ~RSPixelStretchDrawable() override = default;

    void Draw(const RSRenderContent& content, RSPaintFilterCanvas& canvas) const override;
    static RSPropertyDrawable::DrawablePtr Generate(const RSRenderContent& content);
    bool Update(const RSRenderContent& content) override;
};

// ============================================================================
// Background
class RSBackgroundDrawable : public RSPropertyDrawable {
public:
    explicit RSBackgroundDrawable() = default;
    ~RSBackgroundDrawable() override = default;
    static void setForceBgAntiAlias(bool antiAlias);
    void Draw(const RSRenderContent& content, RSPaintFilterCanvas& canvas) const override;

protected:
#ifndef USE_ROSEN_DRAWING
    SkPaint paint_;
#else
    Drawing::Brush brush_;
#endif
};

class RSBackgroundColorDrawable : public RSBackgroundDrawable {
public:
#ifndef USE_ROSEN_DRAWING
    explicit RSBackgroundColorDrawable(SkColor color) : RSBackgroundDrawable()
    {
        paint_.setColor(color);
    }
#else
    explicit RSBackgroundColorDrawable(Drawing::Color color) : RSBackgroundDrawable()
    {
        brush_.SetColor(color);
    }
#endif
    ~RSBackgroundColorDrawable() override = default;
    static RSPropertyDrawable::DrawablePtr Generate(const RSRenderContent& content);
    bool Update(const RSRenderContent& content) override;
};

class RSBackgroundShaderDrawable : public RSBackgroundDrawable {
public:
#ifndef USE_ROSEN_DRAWING
    explicit RSBackgroundShaderDrawable(sk_sp<SkShader> filter) : RSBackgroundDrawable()
    {
        paint_.setShader(std::move(filter));
    }
#else
    explicit RSBackgroundShaderDrawable(std::shared_ptr<Drawing::ShaderEffect> filter) : RSBackgroundDrawable()
    {
        brush_.SetShaderEffect(std::move(filter));
    }
#endif
    ~RSBackgroundShaderDrawable() override = default;
    static RSPropertyDrawable::DrawablePtr Generate(const RSRenderContent& content);
    bool Update(const RSRenderContent& content) override;
};

class RSBackgroundImageDrawable : public RSBackgroundDrawable {
public:
    explicit RSBackgroundImageDrawable() = default;
    ~RSBackgroundImageDrawable() override = default;
    static RSPropertyDrawable::DrawablePtr Generate(const RSRenderContent& content);
    void Draw(const RSRenderContent& content, RSPaintFilterCanvas& canvas) const override;
    bool Update(const RSRenderContent& content) override;
};

// blend mode save and restore
std::unique_ptr<RSPropertyDrawable> BlendSaveDrawableGenerate(const RSRenderContent& context);
std::unique_ptr<RSPropertyDrawable> BlendRestoreDrawableGenerate(const RSRenderContent& context);

class RSBlendSaveLayerDrawable : public RSPropertyDrawable {
public:
    explicit RSBlendSaveLayerDrawable(int blendMode);
    ~RSBlendSaveLayerDrawable() override = default;
    void Draw(const RSRenderContent& content, RSPaintFilterCanvas& canvas) const override;

private:
#ifndef USE_ROSEN_DRAWING
    SkPaint blendPaint_;
#else
    Drawing::Brush blendBrush_;
#endif
};

class RSBlendFastDrawable : public RSPropertyDrawable {
public:
    explicit RSBlendFastDrawable(int blendMode) : blendMode_(blendMode) {}
    ~RSBlendFastDrawable() override = default;
    void Draw(const RSRenderContent& content, RSPaintFilterCanvas& canvas) const override;

private:
    int blendMode_;
};

class RSBlendSaveLayerRestoreDrawable : public RSPropertyDrawable {
public:
    explicit RSBlendSaveLayerRestoreDrawable() = default;
    ~RSBlendSaveLayerRestoreDrawable() override = default;
    void Draw(const RSRenderContent& content, RSPaintFilterCanvas& canvas) const override;
};

class RSBlendFastRestoreDrawable : public RSPropertyDrawable {
public:
    explicit RSBlendFastRestoreDrawable() = default;
    ~RSBlendFastRestoreDrawable() override = default;
    void Draw(const RSRenderContent& content, RSPaintFilterCanvas& canvas) const override;
};
} // namespace OHOS::Rosen
#endif // RENDER_SERVICE_BASE_PROPERTY_RS_PROPERTY_DRAWABLE_BOUNDS_GEOMETRY_H
