/*
 * Copyright (c) 2021-2023 Huawei Device Co., Ltd.
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

#ifndef RENDER_SERVICE_CLIENT_CORE_PIPELINE_RS_PAINT_FILTER_CANVAS_H
#define RENDER_SERVICE_CLIENT_CORE_PIPELINE_RS_PAINT_FILTER_CANVAS_H

#ifndef USE_ROSEN_DRAWING
#include <include/utils/SkPaintFilterCanvas.h>
#endif
#include <optional>
#include <stack>
#include <vector>

#ifndef USE_ROSEN_DRAWING
#include "include/core/SkSurface.h"
#else
#include "draw/canvas.h"
#include "draw/surface.h"
#endif

#include "common/rs_color.h"
#include "common/rs_macros.h"

namespace OHOS {
namespace Rosen {

#ifdef USE_ROSEN_DRAWING
class RSB_EXPORT RSPaintFilterCanvasBase : public Drawing::Canvas {
public:
    RSPaintFilterCanvasBase(Drawing::Canvas* canvas);
    ~RSPaintFilterCanvasBase() override = default;

    void DrawPoint(const Drawing::Point& point) override;
    void DrawLine(const Drawing::Point& startPt, const Drawing::Point& endPt) override;
    void DrawRect(const Drawing::Rect& rect) override;
    void DrawRoundRect(const Drawing::RoundRect& roundRect) override;
    void DrawNestedRoundRect(const Drawing::RoundRect& outer, const Drawing::RoundRect& inner) override;
    void DrawArc(const Drawing::Rect& oval, Drawing::scalar startAngle, Drawing::scalar sweepAngle) override;
    void DrawPie(const Drawing::Rect& oval, Drawing::scalar startAngle, Drawing::scalar sweepAngle) override;
    void DrawOval(const Drawing::Rect& oval) override;
    void DrawCircle(const Drawing::Point& centerPt, Drawing::scalar radius) override;
    void DrawPath(const Drawing::Path& path) override;
    void DrawBackground(const Drawing::Brush& brush) override;
    void DrawShadow(const Drawing::Path& path, const Drawing::Point3& planeParams,
        const Drawing::Point3& devLightPos, Drawing::scalar lightRadius,
        Drawing::Color ambientColor, Drawing::Color spotColor, Drawing::ShadowFlags flag) override;
    void DrawRegion(const Drawing::Region& region) override;

    void DrawBitmap(const Drawing::Bitmap& bitmap, const Drawing::scalar px, const Drawing::scalar py) override;
    void DrawBitmap(Media::PixelMap& pixelMap, const Drawing::scalar px, const Drawing::scalar py) override;
    void DrawImage(const Drawing::Image& image,
        const Drawing::scalar px, const Drawing::scalar py, const Drawing::SamplingOptions& sampling) override;
    void DrawImageRect(const Drawing::Image& image, const Drawing::Rect& src, const Drawing::Rect& dst,
        const Drawing::SamplingOptions& sampling, Drawing::SrcRectConstraint constraint) override;
    void DrawImageRect(const Drawing::Image& image,
        const Drawing::Rect& dst, const Drawing::SamplingOptions& sampling) override;
    void DrawPicture(const Drawing::Picture& picture) override;

    void ClipRect(const Drawing::Rect& rect, Drawing::ClipOp op, bool doAntiAlias) override;
    void ClipRoundRect(const Drawing::RoundRect& roundRect, Drawing::ClipOp op, bool doAntiAlias) override;
    void ClipPath(const Drawing::Path& path, Drawing::ClipOp op, bool doAntiAlias) override;

    void SetMatrix(const Drawing::Matrix& matrix) override;
    void ResetMatrix() override;
    void ConcatMatrix(const Drawing::Matrix& matrix) override;
    void Translate(Drawing::scalar dx, Drawing::scalar dy) override;
    void Scale(Drawing::scalar sx, Drawing::scalar sy) override;
    void Rotate(Drawing::scalar deg, Drawing::scalar sx, Drawing::scalar sy) override;
    void Shear(Drawing::scalar sx, Drawing::scalar sy) override;

    void Flush() override;
    void Clear(Drawing::ColorQuad color) override;
    void Save() override;
    void SaveLayer(const Drawing::SaveLayerOps& saveLayerOps) override;
    void Restore() override;

    CoreCanvas& AttachPen(const Drawing::Pen& pen) override;
    CoreCanvas& AttachBrush(const Drawing::Brush& brush) override;
    CoreCanvas& DetachPen() override;
    CoreCanvas& DetachBrush() override;

protected:
    virtual bool OnFilter() const = 0;
    Drawing::Canvas* canvas_ = nullptr;
};
#endif // USE_ROSEN_DRAWING

// This class is used to filter the paint before drawing. currently, it is used to filter the alpha and foreground
// color.
#ifndef USE_ROSEN_DRAWING
class RSB_EXPORT RSPaintFilterCanvas : public SkPaintFilterCanvas {
public:
    RSPaintFilterCanvas(SkCanvas* canvas, float alpha = 1.0f);
    RSPaintFilterCanvas(SkSurface* skSurface, float alpha = 1.0f);
#else
class RSB_EXPORT RSPaintFilterCanvas : public RSPaintFilterCanvasBase {
public:
    RSPaintFilterCanvas(Drawing::Canvas* canvas, float alpha = 1.0f);
    RSPaintFilterCanvas(Drawing::Surface* surface, float alpha = 1.0f);
#endif
    ~RSPaintFilterCanvas() override {};

    void CopyConfiguration(const RSPaintFilterCanvas& other);

    // alpha related
    void MultiplyAlpha(float alpha);
    void SetAlpha(float alpha);
    float GetAlpha() const;
    int SaveAlpha();
    void RestoreAlpha();
    int GetAlphaSaveCount() const;
    void RestoreAlphaToCount(int count);

    // env related
    void SetEnvForegroundColor(Color color);
    Color GetEnvForegroundColor() const;
    int SaveEnv();
    void RestoreEnv();
    int GetEnvSaveCount() const;
    void RestoreEnvToCount(int count);

    // save/restore utils
    struct SaveStatus {
        int canvasSaveCount = -1;
        int alphaSaveCount = -1;
        int envSaveCount = -1;
    };
#ifndef USE_ROSEN_DRAWING
    SaveStatus Save();
#else
    SaveStatus DrSave();
#endif
    SaveStatus GetSaveStatus() const;
    void RestoreStatus(const SaveStatus& status);

#ifndef USE_ROSEN_DRAWING
    SkSurface* GetSurface() const;
#else
    Drawing::Surface* GetSurface() const;
#endif

    // high contrast
    void SetHighContrast(bool enabled);
    bool isHighContrastEnabled() const;

    enum CacheType : uint8_t {
        UNDEFINED, // do not change current cache status
        ENABLED,   // explicitly enable cache
        DISABLED   // explicitly disable cache
    };
    // cache
    void SetCacheType(CacheType type);
    CacheType GetCacheType() const;

    // visible rect
#ifndef USE_ROSEN_DRAWING
    void SetVisibleRect(SkRect visibleRect);
    SkRect GetVisibleRect() const;

    static std::optional<SkRect> GetLocalClipBounds(const SkCanvas& canvas, const SkIRect* clipBounds = nullptr);
#else
    void SetVisibleRect(Drawing::Rect visibleRect);
    Drawing::Rect GetVisibleRect() const;

    static std::optional<Drawing::Rect> GetLocalClipBounds(const Drawing::Canvas& canvas,
        const Drawing::RectI* clipBounds = nullptr);
    std::stack<float> GetAlphaStack();
    std::stack<Env> GetEnvStack();

    CoreCanvas& AttachPen(const Drawing::Pen& pen) override;
    CoreCanvas& AttachBrush(const Drawing::Brush& brush) override;
#endif
    void SetIsParallelCanvas(bool isParallel) {
        isParallelCanvas_ = isParallel;
    }

    bool GetIsParallelCanvas() const
    {
        return isParallelCanvas_;
    }

protected:
#ifndef USE_ROSEN_DRAWING
    bool onFilter(SkPaint& paint) const override;
    void onDrawPicture(const SkPicture* picture, const SkMatrix* matrix, const SkPaint* paint) override;
    SkCanvas::SaveLayerStrategy getSaveLayerStrategy(const SaveLayerRec& rec) override;
#else
    using Env = struct {
        Color envForegroundColor;
    };
    bool OnFilter() const override;
#endif

private:
#ifndef USE_ROSEN_DRAWING
    SkSurface* skSurface_ = nullptr;
#else
    Drawing::Surface* surface_ = nullptr;
#endif
    std::stack<float> alphaStack_;
#ifndef USE_ROSEN_DRAWING
    using Env = struct {
        Color envForegroundColor;
    };
#endif
    std::stack<Env> envStack_;

    std::atomic_bool isHighContrastEnabled_ { false };
    CacheType cacheType_ { UNDEFINED };
#ifndef USE_ROSEN_DRAWING
    SkRect visibleRect_ = SkRect::MakeEmpty();
#else
    Drawing::Rect visibleRect_ = Drawing::Rect();
#endif
    bool isParallelCanvas_ = false;
};

// This class extends RSPaintFilterCanvas to also create a color filter for the paint.
class RSB_EXPORT RSColorFilterCanvas : public RSPaintFilterCanvas {
public:
    explicit RSColorFilterCanvas(RSPaintFilterCanvas* canvas);
    ~RSColorFilterCanvas() override {};

#ifdef USE_ROSEN_DRAWING
    CoreCanvas& AttachPen(const Drawing::Pen& pen) override;
    CoreCanvas& AttachBrush(const Drawing::Brush& brush) override;
#endif

protected:
#ifndef USE_ROSEN_DRAWING
    bool onFilter(SkPaint& paint) const override;
#else
    bool onFilter() const;
#endif
};

// Helper class similar to SkAutoCanvasRestore, but also restores alpha and/or env
class RSB_EXPORT RSAutoCanvasRestore {
public:
    enum SaveType : uint8_t {
        kNone   = 0x0,
        kCanvas = 0x1,
        kAlpha  = 0x2,
        kEnv    = 0x4,
        kALL    = kCanvas | kAlpha | kEnv,
    };

    /** Preserves canvas save count. Optionally call SkCanvas::save() and/or RSPaintFilterCanvas::SaveAlpha() and/or
       RSPaintFilterCanvas::SaveEnv().
        @param canvas     RSPaintFilterCanvas to guard
        @param saveCanvas call SkCanvas::save()
        @param saveAlpha  call RSPaintFilterCanvas::SaveAlpha()
        @return           utility to restore RSPaintFilterCanvas state on destructor
    */
    RSAutoCanvasRestore(RSPaintFilterCanvas* canvas, SaveType type = SaveType::kALL);

    /** Allow RSAutoCanvasRestore to be used with std::unique_ptr and std::shared_ptr */
    RSAutoCanvasRestore(const std::unique_ptr<RSPaintFilterCanvas>& canvas, SaveType type = SaveType::kALL)
        : RSAutoCanvasRestore(canvas.get(), type)
    {}
    RSAutoCanvasRestore(const std::shared_ptr<RSPaintFilterCanvas>& canvas, SaveType type = SaveType::kALL)
        : RSAutoCanvasRestore(canvas.get(), type)
    {}

    /** Restores RSPaintFilterCanvas to saved state. Destructor is called when container goes out of
        scope.
    */
    ~RSAutoCanvasRestore()
    {
        if (canvas_ && saveCount_.has_value()) {
            canvas_->RestoreStatus(saveCount_.value());
        }
    }

    /** Restores RSPaintFilterCanvas to saved state immediately. Subsequent calls and
        ~RSAutoCanvasRestore() have no effect.
    */
    void restore()
    {
        if (canvas_ && saveCount_.has_value()) {
            canvas_->RestoreStatus(saveCount_.value());
            canvas_ = nullptr;
            saveCount_.reset();
        }
    }

private:
    RSPaintFilterCanvas* canvas_ = nullptr;
    std::optional<RSPaintFilterCanvas::SaveStatus> saveCount_;

    RSAutoCanvasRestore(RSAutoCanvasRestore&&) = delete;
    RSAutoCanvasRestore(const RSAutoCanvasRestore&) = delete;
    RSAutoCanvasRestore& operator=(RSAutoCanvasRestore&&) = delete;
    RSAutoCanvasRestore& operator=(const RSAutoCanvasRestore&) = delete;
};
} // namespace Rosen
} // namespace OHOS
#endif // RENDER_SERVICE_CLIENT_CORE_PIPELINE_RS_PAINT_FILTER_CANVAS_H
