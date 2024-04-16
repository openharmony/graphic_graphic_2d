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

#include <optional>
#include <stack>
#include <vector>

#include "draw/canvas.h"
#include "draw/surface.h"

#include "common/rs_color.h"
#include "common/rs_macros.h"
#include "utils/region.h"

namespace OHOS {
namespace Rosen {

class RSB_EXPORT RSPaintFilterCanvasBase : public Drawing::Canvas {
public:
    RSPaintFilterCanvasBase(Drawing::Canvas* canvas);
    ~RSPaintFilterCanvasBase() override = default;

    Drawing::Matrix GetTotalMatrix() const override;

    Drawing::Rect GetLocalClipBounds() const override;

    Drawing::RectI GetDeviceClipBounds() const override;

    uint32_t GetSaveCount() const override;

#ifdef ACE_ENABLE_GPU
    std::shared_ptr<Drawing::GPUContext> GetGPUContext() override;
#endif

    void DrawSdf(const Drawing::SDFShapeBase& shape) override;
    void DrawPoint(const Drawing::Point& point) override;
    void DrawPoints(Drawing::PointMode mode, size_t count, const Drawing::Point pts[]) override;
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
    void DrawColor(Drawing::ColorQuad color, Drawing::BlendMode mode = Drawing::BlendMode::SRC_OVER) override;
    void DrawRegion(const Drawing::Region& region) override;
    void DrawPatch(const Drawing::Point cubics[12], const Drawing::ColorQuad colors[4],
        const Drawing::Point texCoords[4], Drawing::BlendMode mode) override;
    void DrawVertices(const Drawing::Vertices& vertices, Drawing::BlendMode mode) override;

    void DrawImageNine(const Drawing::Image* image, const Drawing::RectI& center, const Drawing::Rect& dst,
        Drawing::FilterMode filter, const Drawing::Brush* brush = nullptr) override;
    void DrawImageLattice(const Drawing::Image* image, const Drawing::Lattice& lattice, const Drawing::Rect& dst,
        Drawing::FilterMode filter, const Drawing::Brush* brush = nullptr) override;

    // opinc_begin
    bool BeginOpRecording(const Drawing::Rect* bound = nullptr, bool isDynamic = false) override;
    Drawing::OpListHandle EndOpRecording() override;
    void DrawOpList(Drawing::OpListHandle handle) override;
    int CanDrawOpList(Drawing::OpListHandle handle) override;
    bool OpCalculateBefore(const Drawing::Matrix& matrix) override;
    std::shared_ptr<Drawing::OpListHandle> OpCalculateAfter(const Drawing::Rect& bound) override;
    // opinc_end

    void DrawBitmap(const Drawing::Bitmap& bitmap, const Drawing::scalar px, const Drawing::scalar py) override;
    void DrawImage(const Drawing::Image& image,
        const Drawing::scalar px, const Drawing::scalar py, const Drawing::SamplingOptions& sampling) override;
    void DrawImageRect(const Drawing::Image& image, const Drawing::Rect& src, const Drawing::Rect& dst,
        const Drawing::SamplingOptions& sampling, Drawing::SrcRectConstraint constraint) override;
    void DrawImageRect(const Drawing::Image& image,
        const Drawing::Rect& dst, const Drawing::SamplingOptions& sampling) override;
    void DrawPicture(const Drawing::Picture& picture) override;
    void DrawTextBlob(const Drawing::TextBlob* blob, const Drawing::scalar x, const Drawing::scalar y) override;

    void ClipRect(const Drawing::Rect& rect, Drawing::ClipOp op = Drawing::ClipOp::INTERSECT,
        bool doAntiAlias = false) override;
    void ClipIRect(const Drawing::RectI& rect, Drawing::ClipOp op = Drawing::ClipOp::INTERSECT) override;
    void ClipRoundRect(const Drawing::RoundRect& roundRect, Drawing::ClipOp op = Drawing::ClipOp::INTERSECT,
        bool doAntiAlias = false) override;
    void ClipRoundRect(const Drawing::Rect& rect, std::vector<Drawing::Point>& pts, bool doAntiAlias = false) override;
    void ClipPath(const Drawing::Path& path, Drawing::ClipOp op = Drawing::ClipOp::INTERSECT,
        bool doAntiAlias = false) override;
    void ClipRegion(const Drawing::Region& region, Drawing::ClipOp op = Drawing::ClipOp::INTERSECT) override;

    void SetMatrix(const Drawing::Matrix& matrix) override;
    void ResetMatrix() override;
    void ConcatMatrix(const Drawing::Matrix& matrix) override;
    void Translate(Drawing::scalar dx, Drawing::scalar dy) override;
    void Scale(Drawing::scalar sx, Drawing::scalar sy) override;
    void Rotate(Drawing::scalar deg, Drawing::scalar sx, Drawing::scalar sy) override;
    void Shear(Drawing::scalar sx, Drawing::scalar sy) override;

    void Flush() override;
    void Clear(Drawing::ColorQuad color) override;
    uint32_t Save() override;
    void SaveLayer(const Drawing::SaveLayerOps& saveLayerOps) override;
    void Restore() override;
    void Discard() override;

    CoreCanvas& AttachPen(const Drawing::Pen& pen) override;
    CoreCanvas& AttachBrush(const Drawing::Brush& brush) override;
    CoreCanvas& AttachPaint(const Drawing::Paint& paint) override;
    CoreCanvas& DetachPen() override;
    CoreCanvas& DetachBrush() override;
    CoreCanvas& DetachPaint() override;

protected:
    virtual bool OnFilter() const = 0;
    virtual bool OnFilterWithBrush(Drawing::Brush& brush) const = 0;
    Drawing::Canvas* canvas_ = nullptr;
};

// This class is used to filter the paint before drawing. currently, it is used to filter the alpha and foreground
// color.
class RSB_EXPORT RSPaintFilterCanvas : public RSPaintFilterCanvasBase {
public:
    RSPaintFilterCanvas(Drawing::Canvas* canvas, float alpha = 1.0f);
    RSPaintFilterCanvas(Drawing::Surface* surface, float alpha = 1.0f);
    ~RSPaintFilterCanvas() override {};

    void CopyConfiguration(const RSPaintFilterCanvas& other);
    void PushDirtyRegion(Drawing::Region& resultRegion);
    void PopDirtyRegion();
    bool IsDirtyRegionStackEmpty();
    Drawing::Region& GetCurDirtyRegion();

    // alpha related
    void MultiplyAlpha(float alpha);
    void SetAlpha(float alpha);
    float GetAlpha() const override;
    int SaveAlpha();
    void RestoreAlpha();
    int GetAlphaSaveCount() const override;
    void RestoreAlphaToCount(int count);

    // env related
    void SetEnvForegroundColor(Color color);
    Drawing::ColorQuad GetEnvForegroundColor() const override;
    int SaveEnv();
    void RestoreEnv();
    int GetEnvSaveCount() const;
    void RestoreEnvToCount(int count);

    void SetBlendMode(std::optional<int> blendMode);
    int GetBlendOffscreenLayerCnt() const
    {
        return 0;
    };

    // save/restore utils
    struct SaveStatus {
        int canvasSaveCount = -1;
        int alphaSaveCount = -1;
        int envSaveCount = -1;
    };
    enum SaveType : uint8_t {
        kNone           = 0x0,
        kCanvas         = 0x1,
        kAlpha          = 0x2,
        kEnv            = 0x4,
        kCanvasAndAlpha = kCanvas | kAlpha,
        kAll            = kCanvas | kAlpha | kEnv,
    };

    SaveStatus SaveAllStatus(SaveType type = kAll);
    SaveStatus GetSaveStatus() const;
    void RestoreStatus(const SaveStatus& status);

    Drawing::Surface* GetSurface() const override;

    // high contrast
    void SetHighContrast(bool enabled);
    bool isHighContrastEnabled() const override;

    using CacheType = Drawing::CacheType;
    // cache
    void SetCacheType(CacheType type);
    Drawing::CacheType GetCacheType() const override;

    // visible rect
    void SetVisibleRect(Drawing::Rect visibleRect);
    Drawing::Rect GetVisibleRect() const;

    static std::optional<Drawing::Rect> GetLocalClipBounds(const Drawing::Canvas& canvas,
        const Drawing::RectI* clipBounds = nullptr);

    CoreCanvas& AttachPen(const Drawing::Pen& pen) override;
    CoreCanvas& AttachBrush(const Drawing::Brush& brush) override;
    CoreCanvas& AttachPaint(const Drawing::Paint& paint) override;

    void SetParallelThreadIdx(uint32_t idx);
    uint32_t GetParallelThreadIdx() const;
    void SetIsParallelCanvas(bool isParallel);
    bool GetIsParallelCanvas() const;

    void SetDisableFilterCache(bool disable);
    bool GetDisableFilterCache() const;

    void SetRecordDrawable(bool enable);
    bool GetRecordDrawable() const;

    // effect cache data relate
    struct CachedEffectData {
        CachedEffectData() = default;
        CachedEffectData(std::shared_ptr<Drawing::Image>&& image, const Drawing::RectI& rect);
        ~CachedEffectData() = default;
        std::shared_ptr<Drawing::Image> cachedImage_ = nullptr;
        Drawing::RectI cachedRect_ = {};
    };
    void SetEffectData(const std::shared_ptr<CachedEffectData>& effectData);
    const std::shared_ptr<CachedEffectData>& GetEffectData() const;

    // for foregroundFilter to store offscreen canvas & surface
    struct OffscreenData {
        std::shared_ptr<Drawing::Surface> offscreenSurface_ = nullptr;
        std::shared_ptr<RSPaintFilterCanvas> offscreenCanvas_ = nullptr;
    };
    // for foregroundFilter to store and restore offscreen canvas & surface
    void ReplaceMainScreenData(std::shared_ptr<Drawing::Surface>& offscreenSurface,
        std::shared_ptr<RSPaintFilterCanvas>& offscreenCanvas);
    void SwapBackMainScreenData();
    void SavePCanvasList();
    void RestorePCanvasList();
    void StoreCanvas()
    {
        if (storeMainCanvas_ == nullptr) {
            storeMainCanvas_ = canvas_;
        }
    }
    Drawing::Canvas* GetOriginalCanvas()
    {
        return storeMainCanvas_;
    }

    // canvas status relate
    struct CanvasStatus {
        float alpha_;
        Drawing::Matrix matrix_;
        std::shared_ptr<CachedEffectData> effectData_;
    };
    CanvasStatus GetCanvasStatus() const;
    void SetCanvasStatus(const CanvasStatus& status);
    Drawing::Canvas* GetRecordingCanvas() const override;
    bool GetRecordingState() const override;
    void SetRecordingState(bool flag) override;

protected:
    using Env = struct {
        Color envForegroundColor_;
        std::shared_ptr<CachedEffectData> effectData_;
        std::optional<int> blendMode_;
    };
    const std::stack<float>& GetAlphaStack();
    const std::stack<Env>& GetEnvStack();

    bool OnFilter() const override;
    inline bool OnFilterWithBrush(Drawing::Brush& brush) const override
    {
        float alpha = alphaStack_.top();
        // foreground color and foreground color strategy identification
        if (brush.GetColor().CastToColorQuad() == 0x00000001) {
            brush.SetColor(envStack_.top().envForegroundColor_.AsArgbInt());
        }

        // use alphaStack_.top() to multiply alpha
        if (alpha < 1 && alpha > 0) {
            brush.SetAlpha(brush.GetAlpha() * alpha);
        }
        return alpha > 0.f;
    }

private:
    Drawing::Surface* surface_ = nullptr;
    std::stack<float> alphaStack_;
    std::stack<Env> envStack_;

    // save every dirty region of the current surface for quick reject
    std::stack<Drawing::Region> dirtyRegionStack_;
    
    // greater than 0 indicates canvas currently is drawing on a new layer created offscreen blendmode
    // std::stack<bool> blendOffscreenStack_;

    // foregroundFilter related
    std::vector<std::vector<Canvas*>> storedPCanvasList_; // store pCanvasList_
    std::stack<OffscreenData> offscreenDataList_; // store offscreen canvas & surface
    std::stack<Drawing::Surface*> storeMainScreenSurface_; // store surface_
    std::stack<Drawing::Canvas*> storeMainScreenCanvas_; // store canvas_
    Drawing::Canvas* storeMainCanvas_ = nullptr; // store main canvas_

    std::atomic_bool isHighContrastEnabled_ { false };
    CacheType cacheType_ { RSPaintFilterCanvas::CacheType::UNDEFINED };
    Drawing::Rect visibleRect_ = Drawing::Rect();

    uint32_t threadIndex_ = UNI_RENDER_THREAD_INDEX; // default
    bool isParallelCanvas_ = false;
    bool disableFilterCache_ = false;
    bool recordingState_ = false;
    bool recordDrawable_ = false;
};

// Helper class similar to SkAutoCanvasRestore, but also restores alpha and/or env
class RSB_EXPORT RSAutoCanvasRestore {
public:
    /** Preserves canvas save count. Optionally call SkCanvas::save() and/or RSPaintFilterCanvas::SaveAlpha() and/or
       RSPaintFilterCanvas::SaveEnv().
        @param canvas     RSPaintFilterCanvas to guard
        @param saveCanvas call SkCanvas::save()
        @param saveAlpha  call RSPaintFilterCanvas::SaveAlpha()
        @return           utility to restore RSPaintFilterCanvas state on destructor
    */
    RSAutoCanvasRestore(
        RSPaintFilterCanvas* canvas, RSPaintFilterCanvas::SaveType type = RSPaintFilterCanvas::SaveType::kAll)
        : canvas_(canvas), saveCount_(canvas ? canvas->SaveAllStatus(type) : RSPaintFilterCanvas::SaveStatus())
    {}

    /** Allow RSAutoCanvasRestore to be used with std::unique_ptr and std::shared_ptr */
    RSAutoCanvasRestore(const std::unique_ptr<RSPaintFilterCanvas>& canvas,
        RSPaintFilterCanvas::SaveType type = RSPaintFilterCanvas::SaveType::kAll)
        : RSAutoCanvasRestore(canvas.get(), type)
    {}
    RSAutoCanvasRestore(const std::shared_ptr<RSPaintFilterCanvas>& canvas,
        RSPaintFilterCanvas::SaveType type = RSPaintFilterCanvas::SaveType::kAll)
        : RSAutoCanvasRestore(canvas.get(), type)
    {}

    RSAutoCanvasRestore(RSAutoCanvasRestore&&) = delete;
    RSAutoCanvasRestore(const RSAutoCanvasRestore&) = delete;
    RSAutoCanvasRestore& operator=(RSAutoCanvasRestore&&) = delete;
    RSAutoCanvasRestore& operator=(const RSAutoCanvasRestore&) = delete;

    /** Restores RSPaintFilterCanvas to saved state. Destructor is called when container goes out of
        scope.
    */
    ~RSAutoCanvasRestore()
    {
        if (canvas_) {
            canvas_->RestoreStatus(saveCount_);
        }
    }

    /** Restores RSPaintFilterCanvas to saved state immediately. Subsequent calls and
        ~RSAutoCanvasRestore() have no effect.
    */
    void restore()
    {
        if (canvas_) {
            canvas_->RestoreStatus(saveCount_);
            canvas_ = nullptr;
        }
    }

private:
    RSPaintFilterCanvas* canvas_ = nullptr;
    RSPaintFilterCanvas::SaveStatus saveCount_;
};
} // namespace Rosen
} // namespace OHOS
#endif // RENDER_SERVICE_CLIENT_CORE_PIPELINE_RS_PAINT_FILTER_CANVAS_H
