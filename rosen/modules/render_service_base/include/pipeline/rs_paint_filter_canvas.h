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

#include <limits>
#include <optional>
#include <stack>
#include <vector>

#include "common/rs_color.h"
#include "common/rs_macros.h"
#include "common/rs_occlusion_region.h"
#include "common/rs_rect.h"
#include "draw/canvas.h"
#include "draw/surface.h"
#include "screen_manager/screen_types.h"
#include "surface_type.h"
#include "utils/region.h"

namespace OHOS {
namespace Rosen {
class RSFilter;

class RSB_EXPORT RSPaintFilterCanvasBase : public Drawing::Canvas {
public:
    RSPaintFilterCanvasBase(Drawing::Canvas* canvas);
    ~RSPaintFilterCanvasBase() override = default;

    void SetParallelRender(bool parallelEnable) override;
    Drawing::Matrix GetTotalMatrix() const override;

    Drawing::Rect GetLocalClipBounds() const override;

    Drawing::RectI GetDeviceClipBounds() const override;

    Drawing::RectI GetRoundInDeviceClipBounds() const override;

    uint32_t GetSaveCount() const override;

#ifdef RS_ENABLE_GPU
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
    void DrawPathWithStencil(const Drawing::Path& path, uint32_t stencilVal) override;
    void DrawBackground(const Drawing::Brush& brush) override;
    void DrawShadow(const Drawing::Path& path, const Drawing::Point3& planeParams,
        const Drawing::Point3& devLightPos, Drawing::scalar lightRadius,
        Drawing::Color ambientColor, Drawing::Color spotColor, Drawing::ShadowFlags flag) override;
    void DrawShadowStyle(const Drawing::Path& path, const Drawing::Point3& planeParams,
        const Drawing::Point3& devLightPos, Drawing::scalar lightRadius,
        Drawing::Color ambientColor, Drawing::Color spotColor, Drawing::ShadowFlags flag,
        bool isLimitElevation) override;
    void DrawColor(Drawing::ColorQuad color, Drawing::BlendMode mode = Drawing::BlendMode::SRC_OVER) override;
    void DrawRegion(const Drawing::Region& region) override;
    void DrawPatch(const Drawing::Point cubics[12], const Drawing::ColorQuad colors[4],
        const Drawing::Point texCoords[4], Drawing::BlendMode mode) override;
    void DrawVertices(const Drawing::Vertices& vertices, Drawing::BlendMode mode) override;

    void DrawImageNine(const Drawing::Image* image, const Drawing::RectI& center, const Drawing::Rect& dst,
        Drawing::FilterMode filter, const Drawing::Brush* brush = nullptr) override;
    void DrawImageLattice(const Drawing::Image* image, const Drawing::Lattice& lattice, const Drawing::Rect& dst,
        Drawing::FilterMode filter) override;

    bool OpCalculateBefore(const Drawing::Matrix& matrix) override;
    std::shared_ptr<Drawing::OpListHandle> OpCalculateAfter(const Drawing::Rect& bound) override;

    void DrawAtlas(const Drawing::Image* atlas, const Drawing::RSXform xform[], const Drawing::Rect tex[],
        const Drawing::ColorQuad colors[], int count, Drawing::BlendMode mode,
        const Drawing::SamplingOptions& sampling, const Drawing::Rect* cullRect) override;
    void DrawBitmap(const Drawing::Bitmap& bitmap, const Drawing::scalar px, const Drawing::scalar py) override;
    void DrawImage(const Drawing::Image& image,
        const Drawing::scalar px, const Drawing::scalar py, const Drawing::SamplingOptions& sampling) override;
    void DrawImageWithStencil(const Drawing::Image& image, const Drawing::scalar px, const Drawing::scalar py,
        const Drawing::SamplingOptions& sampling, uint32_t stencilVal) override;
    void DrawImageRect(const Drawing::Image& image, const Drawing::Rect& src, const Drawing::Rect& dst,
        const Drawing::SamplingOptions& sampling, Drawing::SrcRectConstraint constraint) override;
    void DrawImageRect(const Drawing::Image& image,
        const Drawing::Rect& dst, const Drawing::SamplingOptions& sampling) override;
    void DrawPicture(const Drawing::Picture& picture) override;
    void DrawTextBlob(const Drawing::TextBlob* blob, const Drawing::scalar x, const Drawing::scalar y) override;

    void ClearStencil(const Drawing::RectI& rect, uint32_t stencilVal) override;
    void ClipRect(const Drawing::Rect& rect, Drawing::ClipOp op = Drawing::ClipOp::INTERSECT,
        bool doAntiAlias = false) override;
    void ClipIRect(const Drawing::RectI& rect, Drawing::ClipOp op = Drawing::ClipOp::INTERSECT) override;
    void ClipRoundRect(const Drawing::RoundRect& roundRect, Drawing::ClipOp op = Drawing::ClipOp::INTERSECT,
        bool doAntiAlias = false) override;
    void ClipRoundRect(const Drawing::Rect& rect, std::vector<Drawing::Point>& pts, bool doAntiAlias = false) override;
    void ClipPath(const Drawing::Path& path, Drawing::ClipOp op = Drawing::ClipOp::INTERSECT,
        bool doAntiAlias = false) override;
    void ClipRegion(const Drawing::Region& region, Drawing::ClipOp op = Drawing::ClipOp::INTERSECT) override;
    void ResetClip() override;

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

    /**
     * @brief DDK Draw HPS Effect on image
     *
     * DDK provides an interface for high-performance implementation of certain 2D visual effects
     *
     * @param image                 the image needed to draw
     * @param hpsEffectParams       the params of different effects needed to excute on image
     * @return true if DDK excute HPS Effect success; false when failed.
     */
    bool DrawImageEffectHPS(const Drawing::Image& image,
        const std::vector<std::shared_ptr<Drawing::HpsEffectParameter>>& hpsEffectParams) override;
    /**
     * @brief DDK Draw Gaussian Blur on image
     *
     * DDK provides an interface for high-performance implementation of Gaussian Blur
     *
     * @param image                 the image needed to draw
     * @param blurParams            the params of Gaussian Blur which will excute on image
     * @return true if DDK excute Blur success; false when failed.
     */
    bool DrawBlurImage(const Drawing::Image& image, const Drawing::HpsBlurParameter& blurParams) override;
    /**
     * @brief DDK calculate the size of the off-screen canvas that Blur Effect excute on
     *
     * DDK will excute Blur Effect on an off-screen canvas, the size of the off-screen canvas
     *  will be calculated in DDK.
     *
     * @param blurParams            the params of Gaussian Blur which will excute on image
     * @return size of the off-screen canvas.
     */
    std::array<int, 2> CalcHpsBluredImageDimension(const Drawing::HpsBlurParameter& blurParams) override;

    bool IsClipRect() override;

protected:
    virtual bool OnFilter() const = 0;
    virtual bool OnFilterWithBrush(Drawing::Brush& brush) const = 0;
    virtual Drawing::Brush* GetFilteredBrush() const = 0;
    Drawing::Canvas* canvas_ = nullptr;
};

// This class is used to filter the paint before drawing. currently, it is used to filter the alpha and foreground
// color.
class RSB_EXPORT RSPaintFilterCanvas : public RSPaintFilterCanvasBase {
public:
    RSPaintFilterCanvas(Drawing::Canvas* canvas, float alpha = 1.0f);
    RSPaintFilterCanvas(Drawing::Surface* surface, float alpha = 1.0f);
    ~RSPaintFilterCanvas() override = default;

    void CopyConfigurationToOffscreenCanvas(const RSPaintFilterCanvas& other);
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

    // blendmode and blender related
    void SaveLayer(const Drawing::SaveLayerOps& saveLayerOps) override;
    void SetBlendMode(std::optional<int> blendMode);
    void SetBlender(std::shared_ptr<Drawing::Blender>);
    bool HasOffscreenLayer() const;

    // save/restore utils
    struct SaveStatus {
        int canvasSaveCount = -1;
        int alphaSaveCount = -1;
        int envSaveCount = -1;
    };

    enum class ScreenshotType {
        NON_SHOT = 0,
        SDR_SCREENSHOT,
        SDR_WINDOWSHOT,
        HDR_SCREENSHOT,
        HDR_WINDOWSHOT,
    };

    struct HDRProperties {
        bool isHDREnabledVirtualScreen = false;
        float hdrBrightness = 1.0f; // Default 1.0f means max available headroom
        ScreenshotType screenshotType = ScreenshotType::NON_SHOT;
    };

    enum SaveType : uint8_t {
        kNone           = 0x0,
        kCanvas         = 0x1,
        kAlpha          = 0x2,
        kEnv            = 0x4,
        kCanvasAndAlpha = kCanvas | kAlpha,
        kAll            = kCanvas | kAlpha | kEnv,
    };

    ScreenshotType GetScreenshotType() const;
    void SetScreenshotType(ScreenshotType type);
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
    uint32_t GetParallelThreadId();
    void SetParallelThreadId(int idx);
    void SetIsParallelCanvas(bool isParallel);
    bool GetIsParallelCanvas() const;

    void RecordState(const RSPaintFilterCanvas& other);
    std::weak_ptr<Drawing::Surface> GetWeakSurface();
    // just used in SubTree, used for check whether a new Surface needs to be created in the SubTree thread.
    void SetWeakSurface(std::shared_ptr<Drawing::Surface> surface);
    inline void SetQuickGetDrawState(bool isQuickGetDrawState)
    {
        isQuickGetDrawState_ = isQuickGetDrawState;
    }
    inline bool IsQuickGetDrawState()
    {
        return isQuickGetDrawState_;
    }

    void SetDisableFilterCache(bool disable);
    bool GetDisableFilterCache() const;

    void SetRecordDrawable(bool enable);
    bool GetRecordDrawable() const;

    // effect cache data relate
    struct CachedEffectData {
        CachedEffectData() = default;
        CachedEffectData(std::shared_ptr<Drawing::Image>&& image, const Drawing::RectI& rect);
        CachedEffectData(const std::shared_ptr<Drawing::Image>& image, const Drawing::RectI& rect);
        ~CachedEffectData() = default;
        std::string GetInfo() const;
        std::shared_ptr<Drawing::Image> cachedImage_ = nullptr;
        Drawing::RectI cachedRect_ = {};
        Drawing::Matrix cachedMatrix_ = Drawing::Matrix();
    };
    void SetEffectData(const std::shared_ptr<CachedEffectData>& effectData);
    const std::shared_ptr<CachedEffectData>& GetEffectData() const;
    void SetDrawnRegion(const Occlusion::Region& region);
    const Occlusion::Region& GetDrawnRegion() const;
    // behind window cache relate
    void SetBehindWindowData(const std::shared_ptr<CachedEffectData>& behindWindowData);
    const std::shared_ptr<CachedEffectData>& GetBehindWindowData() const;

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

    const std::stack<OffscreenData>& GetOffscreenDataList() const
    {
        return offscreenDataList_;
    }

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

    Drawing::DrawingType GetDrawingType() const override
    {
        return Drawing::DrawingType::PAINT_FILTER;
    }
    bool IsOnMultipleScreen() const;
    void SetOnMultipleScreen(bool multipleScreen);
    ScreenId GetScreenId() const;
    void SetScreenId(ScreenId screenId);
    GraphicColorGamut GetTargetColorGamut() const;
    void SetTargetColorGamut(GraphicColorGamut colorGamut);
    float GetHDRBrightness() const;
    void SetHDRBrightness(float hdrBrightness);
    float GetBrightnessRatio() const;
    void SetBrightnessRatio(float brightnessRatio);
    void CopyHDRConfiguration(const RSPaintFilterCanvas& other);
    bool GetHdrOn() const;
    void SetHdrOn(bool isHdrOn);
    bool GetHDREnabledVirtualScreen() const;
    void SetHDREnabledVirtualScreen(bool isHDREnabledVirtualScreen);
    const HDRProperties& GetHDRProperties() const;
    bool GetIsWindowFreezeCapture() const;
    void SetIsWindowFreezeCapture(bool isWindowFreezeCapture);
    bool GetIsDrawingCache() const;
    void SetIsDrawingCache(bool isDrawingCache);
    void SetEffectIntersectWithDRM(bool intersect);
    bool GetEffectIntersectWithDRM() const;
    void SetDarkColorMode(bool isDark);
    bool GetDarkColorMode() const;

    struct CacheBehindWindowData {
        CacheBehindWindowData() = default;
        CacheBehindWindowData(std::shared_ptr<RSFilter> filter, const Drawing::Rect rect);
        ~CacheBehindWindowData() = default;
        std::shared_ptr<RSFilter> filter_ = nullptr;
        Drawing::Rect rect_ = {};
    };
    void SetCacheBehindWindowData(const std::shared_ptr<CacheBehindWindowData>& data);
    const std::shared_ptr<CacheBehindWindowData>& GetCacheBehindWindowData() const;

    // Set culled nodes for control-level occlusion culling
    void SetCulledNodes(std::unordered_set<NodeId>&& culledNodes)
    {
        culledNodes_ = std::move(culledNodes);
    }

    // Get culled nodes for control-level occlusion culling
    const std::unordered_set<NodeId>& GetCulledNodes() const
    {
        return culledNodes_;
    }

    // Set culled entire subtree for control-level occlusion culling
    void SetCulledEntireSubtree(std::unordered_set<NodeId>&& culledEntireSubtree)
    {
        culledEntireSubtree_ = std::move(culledEntireSubtree);
    }

    // Get culled entire subtree for control-level occlusion culling
    const std::unordered_set<NodeId>& GetCulledEntireSubtree() const
    {
        return culledEntireSubtree_;
    }

protected:
    using Env = struct {
        Color envForegroundColor_;
        std::shared_ptr<CachedEffectData> effectData_;
        std::shared_ptr<CachedEffectData> behindWindowData_;
        std::shared_ptr<Drawing::Blender> blender_;
        bool hasOffscreenLayer_;
    };

    bool OnFilter() const override;
    inline bool OnFilterWithBrush(Drawing::Brush& brush) const override
    {
        if (isQuickGetDrawState_) {
            return false;
        }
        float alpha = alphaStack_.top();
        // foreground color and foreground color strategy identification
        if (brush.GetColor().CastToColorQuad() == 0x00000001) {
            brush.SetColor(envStack_.top().envForegroundColor_.AsArgbInt());
        }

        if (envStack_.top().blender_) {
            brush.SetBlender(envStack_.top().blender_);
        }

        // use alphaStack_.top() to multiply alpha
        if (alpha < 1 && alpha > 0) {
            brush.SetAlpha(brush.GetAlpha() * alpha);
        }
        return alpha > 0.f;
    }
    inline Drawing::Brush* GetFilteredBrush() const override
    {
        static Drawing::Brush brush;
        float alpha = alphaStack_.top();
        if (alpha >= 1) {
            return nullptr;
        }
        brush.SetAlphaF(alpha);
        return &brush;
    }

private:
    bool isParallelCanvas_ = false;
    bool disableFilterCache_ = false;
    bool recordingState_ = false;
    bool recordDrawable_ = false;
    bool multipleScreen_ = false;
    bool isHdrOn_ = false;
    bool isWindowFreezeCapture_ = false;
    // Drawing window cache or uifirst cache
    bool isDrawingCache_ = false;
    bool isIntersectWithDRM_ = false;
    bool isDarkColorMode_ = false;
    CacheType cacheType_ { RSPaintFilterCanvas::CacheType::UNDEFINED };
    std::atomic_bool isHighContrastEnabled_ { false };
    GraphicColorGamut targetColorGamut_ = GraphicColorGamut::GRAPHIC_COLOR_GAMUT_SRGB;
    float brightnessRatio_ = 1.0f; // Default 1.0f means no discount
    ScreenId screenId_ = INVALID_SCREEN_ID;
    uint32_t threadIndex_ = UNI_RENDER_THREAD_INDEX; // default
    HDRProperties hdrProperties_;
    Drawing::Surface* surface_ = nullptr;
    Drawing::Canvas* storeMainCanvas_ = nullptr; // store main canvas
    Drawing::Rect visibleRect_ = Drawing::Rect();
    std::unordered_set<NodeId> culledNodes_; // store culled nodes for control-level occlusion culling
    std::unordered_set<NodeId> culledEntireSubtree_; // store culled entire subtree for control-level occlusion culling

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

    std::shared_ptr<CacheBehindWindowData> cacheBehindWindowData_ = nullptr;
    Occlusion::Region drawnRegion_;
    uint32_t threadId_;
    std::weak_ptr<Drawing::Surface> weakSurface_;
    bool isQuickGetDrawState_= false;
};

#ifdef RS_ENABLE_VK
class RSHybridRenderPaintFilterCanvas : public RSPaintFilterCanvas {
public:
    RSHybridRenderPaintFilterCanvas(Drawing::Canvas* canvas, float alpha = 1.0f) : RSPaintFilterCanvas(canvas, alpha)
    {}

    RSHybridRenderPaintFilterCanvas(Drawing::Surface* surface, float alpha = 1.0f) : RSPaintFilterCanvas(surface, alpha)
    {}

    //Override the AttachPaint method
    CoreCanvas& AttachPaint(const Drawing::Paint& paint) override;

    bool IsRenderWithForegroundColor() const
    {
        return isRenderWithForegroundColor;
    }

    void SetRenderWithForegroundColor(bool renderFilterStatus)
    {
        isRenderWithForegroundColor = renderFilterStatus;
    }
private:
    bool isRenderWithForegroundColor = false;
};
#endif

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
