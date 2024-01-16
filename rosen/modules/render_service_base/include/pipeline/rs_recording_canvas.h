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

#ifndef RENDER_SERVICE_CLIENT_CORE_PIPELINE_RS_RECORDING_CANVAS_H
#define RENDER_SERVICE_CLIENT_CORE_PIPELINE_RS_RECORDING_CANVAS_H

#ifndef USE_ROSEN_DRAWING

#include "include/core/SkBitmap.h"
#include "include/core/SkCanvas.h"
#include "include/core/SkCanvasVirtualEnforcer.h"
#include "include/core/SkDrawable.h"
#include "include/core/SkPaint.h"
#include "include/core/SkPath.h"
#include "include/core/SkRect.h"
#include "include/utils/SkNoDrawCanvas.h"

#include "common/rs_macros.h"
#include "pipeline/rs_draw_cmd_list.h"
#include "property/rs_properties_def.h"
#include "render/rs_image.h"

#ifdef NEW_SKIA
#include "src/core/SkVerticesPriv.h"
#endif
#ifdef ROSEN_OHOS
#include "surface_buffer.h"
#endif

#include "include/core/HMSymbol.h"

namespace OHOS {
namespace Media {
class PixelMap;
}
namespace Rosen {
class DrawCmdList;
class OpItem;
#ifdef ROSEN_OHOS
struct RSSurfaceBufferInfo {
    RSSurfaceBufferInfo() = default;
    RSSurfaceBufferInfo(
        const sptr<SurfaceBuffer>& surfaceBuffer, int offSetX, int offSetY, int width, int height)
        : surfaceBuffer_(surfaceBuffer), offSetX_(offSetX), offSetY_(offSetY), width_(width), height_(height)
    {}
    sptr<SurfaceBuffer> surfaceBuffer_ = nullptr;
    int offSetX_ = 0;
    int offSetY_ = 0;
    int width_ = 0;
    int height_ = 0;
};
#endif
class RSB_EXPORT RSRecordingCanvas : public SkCanvasVirtualEnforcer<SkNoDrawCanvas> {
public:
    RSRecordingCanvas(int width, int height);
    virtual ~RSRecordingCanvas();
    std::shared_ptr<DrawCmdList> GetDrawCmdList() const;
    void Clear() const;
    void AddOp(std::unique_ptr<OpItem>&& opItem);
#ifdef NEW_SKIA
    GrRecordingContext* recordingContext() override;
    void SetGrRecordingContext(GrRecordingContext* context);
    void didConcat44(const SkM44&) override;
    void didSetM44(const SkM44&) override;
    void didScale(SkScalar, SkScalar) override;
    void onDrawGlyphRunList(const SkGlyphRunList& glyphRunList, const SkPaint& paint) override;
    void onDrawImage2(const SkImage* img, SkScalar dx, SkScalar dy, const SkSamplingOptions& samplingOptions,
        const SkPaint* paint) override;
    void onDrawImageRect2(const SkImage* img, const SkRect& src, const SkRect& dst,
        const SkSamplingOptions& samplingOptions, const SkPaint* paint, SrcRectConstraint constraint) override;
    void onDrawImageLattice2(const SkImage* img, const Lattice& lattice, const SkRect& dst,
                                        SkFilterMode mode, const SkPaint* paint) override;
    void onDrawAtlas2(const SkImage*, const SkRSXform[], const SkRect src[],
        const SkColor[], int count, SkBlendMode mode, const SkSamplingOptions& samplingOptions,
        const SkRect* cull, const SkPaint* paint) override;
    void onDrawEdgeAAImageSet2(const ImageSetEntry imageSet[], int count,
        const SkPoint dstClips[], const SkMatrix preViewMatrices[],
        const SkSamplingOptions& samplingOptions, const SkPaint* paint,
        SrcRectConstraint constraint) override;
    void onDrawVerticesObject(const SkVertices* vertices, SkBlendMode mode, const SkPaint& paint) override;
    void onDrawEdgeAAQuad(const SkRect& rect, const SkPoint clip[4], QuadAAFlags aaFlags,
        const SkColor4f& color, SkBlendMode mode) override;
    void DrawPixelMapRect(const std::shared_ptr<Media::PixelMap>& pixelmap, const SkRect& src, const SkRect& dst,
        const SkSamplingOptions& samplingOptions, const SkPaint* paint,
        SrcRectConstraint constraint = kStrict_SrcRectConstraint);
    void DrawPixelMapRect(const std::shared_ptr<Media::PixelMap>& pixelmap, const SkRect& dst,
        const SkSamplingOptions& samplingOptions, const SkPaint* paint);
    void DrawRsImage(const std::shared_ptr<RSImageBase>& rsImage, const SkSamplingOptions& samplingOptions,
        const SkPaint& paint, SrcRectConstraint constraint = kStrict_SrcRectConstraint);
    void DrawPixelMap(const std::shared_ptr<Media::PixelMap>& pixelmap, SkScalar x, SkScalar y,
        const SkSamplingOptions& samplingOptions, const SkPaint* paint = nullptr);
    void DrawImageWithParm(const sk_sp<SkImage> image, const sk_sp<SkData> data,
        const Rosen::RsImageInfo& rsImageInfo, const SkSamplingOptions& samplingOptions, const SkPaint& paint);
    void DrawPixelMapWithParm(
        const std::shared_ptr<Media::PixelMap>& pixelmap,
        const Rosen::RsImageInfo& rsImageInfo, const SkSamplingOptions& samplingOptions, const SkPaint& paint);
    void DrawImageNine(const std::shared_ptr<Media::PixelMap>& pixelmap, const SkIRect& center,
        const SkRect& dst, SkFilterMode filter, const SkPaint* paint);
    using DrawFunc  = std::function<void(RSPaintFilterCanvas& canvas, const SkRect*)>;
    void DrawDrawFunc(DrawFunc && drawFunc);
#else
    GrContext* getGrContext() override;
    void SetGrContext(GrContext* grContext);
    void didConcat(const SkMatrix& matrix) override;
    void didSetMatrix(const SkMatrix& matrix) override;
    void onDrawBitmap(const SkBitmap& bm, SkScalar x, SkScalar y, const SkPaint* paint) override;
    void onDrawBitmapLattice(const SkBitmap& bm, const SkCanvas::Lattice& lattice, const SkRect& dst,
        const SkPaint* paint) override;
    void onDrawBitmapNine(const SkBitmap& bm, const SkIRect& center, const SkRect& dst, const SkPaint* paint) override;
    void onDrawBitmapRect(const SkBitmap& bm, const SkRect* src, const SkRect& dst,
        const SkPaint* paint, SrcRectConstraint constraint) override;
    void onDrawImage(const SkImage* img, SkScalar x, SkScalar y, const SkPaint* paint)override;
    void onDrawImageLattice(const SkImage* img, const SkCanvas::Lattice& lattice,
        const SkRect& dst, const SkPaint* paint) override;
    void onDrawImageNine(const SkImage* img, const SkIRect& center, const SkRect& dst, const SkPaint* paint) override;
    void onDrawImageRect(const SkImage* img, const SkRect* src, const SkRect& dst, const SkPaint* paint,
        SrcRectConstraint constraint) override;
    void onDrawVerticesObject(
        const SkVertices*, const SkVertices::Bone bones[], int boneCount, SkBlendMode, const SkPaint&) override;
    void onDrawAtlas(const SkImage*, const SkRSXform[], const SkRect[], const SkColor[], int, SkBlendMode,
        const SkRect*, const SkPaint*) override;
    void onDrawEdgeAAQuad(const SkRect&, const SkPoint[4], QuadAAFlags, SkColor,
                          SkBlendMode) override {}
    void onDrawEdgeAAImageSet(const ImageSetEntry[], int, const SkPoint[],
                              const SkMatrix[], const SkPaint*, SrcRectConstraint) override {}
    void DrawPixelMapRect(const std::shared_ptr<Media::PixelMap>& pixelmap, const SkRect& src, const SkRect& dst,
        const SkPaint* paint, SrcRectConstraint constraint = kStrict_SrcRectConstraint);
    void DrawPixelMapRect(const std::shared_ptr<Media::PixelMap>& pixelmap, const SkRect& dst, const SkPaint* paint);
    void DrawPixelMap(const std::shared_ptr<Media::PixelMap>& pixelmap, SkScalar x, SkScalar y,
        const SkPaint* paint = nullptr);
    void DrawImageWithParm(const sk_sp<SkImage> image, const sk_sp<SkData> data,
        const Rosen::RsImageInfo& rsImageInfo, const SkPaint& paint);
    void DrawPixelMapWithParm(
        const std::shared_ptr<Media::PixelMap>& pixelmap, const Rosen::RsImageInfo& rsImageInfo, const SkPaint& paint);
#endif
    sk_sp<SkSurface> onNewSurface(const SkImageInfo& info, const SkSurfaceProps& props) override;

    void willSave() override;
    SaveLayerStrategy getSaveLayerStrategy(const SaveLayerRec& rec) override;
    void willRestore() override;

    void onFlush() override;

    void didTranslate(SkScalar dx, SkScalar dy) override;

    void onClipRect(const SkRect& rect, SkClipOp clipOp, ClipEdgeStyle style) override;
    void onClipRRect(const SkRRect& rect, SkClipOp clipOp, ClipEdgeStyle style) override;
    void onClipPath(const SkPath& path, SkClipOp clipOp, ClipEdgeStyle style) override;
    void onClipRegion(const SkRegion& region, SkClipOp clipop) override;

    void onDrawPaint(const SkPaint& paint) override;
    void onDrawBehind(const SkPaint&) override;
    void onDrawPath(const SkPath& path, const SkPaint& paint) override;
    void onDrawRect(const SkRect& rect, const SkPaint& paint) override;
    void onDrawRegion(const SkRegion& region, const SkPaint& paint) override;
    void onDrawOval(const SkRect& oval, const SkPaint& paint) override;
    void onDrawArc(const SkRect& oval, SkScalar startAngle, SkScalar sweepAngle,
        bool useCenter, const SkPaint& paint) override;
    void onDrawRRect(const SkRRect& rrect, const SkPaint& paint) override;
    void onDrawDRRect(const SkRRect& out, const SkRRect& in, const SkPaint& paint) override;

    void onDrawDrawable(SkDrawable* drawable, const SkMatrix* matrix) override;
    void onDrawPicture(const SkPicture* picture, const SkMatrix* matrix, const SkPaint* paint) override;
    void onDrawAnnotation(const SkRect&, const char[], SkData*) override;

    void onDrawTextBlob(const SkTextBlob* blob, SkScalar x, SkScalar y, const SkPaint& paint) override;
    void onDrawSymbol(const HMSymbolData& symbol, SkPoint locate, const SkPaint& paint) override;

    void onDrawPatch(const SkPoint[12], const SkColor[4], const SkPoint[4], SkBlendMode, const SkPaint&) override;
    void onDrawPoints(SkCanvas::PointMode mode, size_t count, const SkPoint pts[], const SkPaint& paint) override;
    void onDrawShadowRec(const SkPath&, const SkDrawShadowRec&) override;

    void ClipOutsetRect(float dx, float dy);
    void DrawAdaptiveRRect(float radius, const SkPaint& paint);
    void DrawAdaptiveRRectScale(float radiusRatio, const SkPaint& paint);
    void ClipAdaptiveRRect(const SkVector radius[]);
#ifdef ROSEN_OHOS
    void DrawSurfaceBuffer(const RSSurfaceBufferInfo& surfaceBufferInfo);
#endif
    void MultiplyAlpha(float alpha);
    void SaveAlpha();
    void RestoreAlpha();

    void SetIsCustomTextType(bool isCustomTextType);
    bool IsCustomTextType() const;
private:
    void DrawImageLatticeAsBitmap(
        const SkImage* image, const SkCanvas::Lattice& lattice, const SkRect& dst, const SkPaint* paint);

    std::shared_ptr<DrawCmdList> drawCmdList_ { nullptr };
    int saveCount_ = 0;
    bool isCustomTextType_ = false;
#ifdef NEW_SKIA
    GrRecordingContext* grContext_ = nullptr;
#else
    GrContext* grContext_ = nullptr;
#endif
};
} // namespace Rosen
} // namespace OHOS

#else
#include "recording/recording_canvas.h"
#include "pipeline/rs_draw_cmd.h"

namespace OHOS {
namespace Media {
class PixelMap;
}
namespace Rosen {
class RSB_EXPORT ExtendRecordingCanvas : public Drawing::RecordingCanvas {
public:
    ExtendRecordingCanvas(int width, int weight, bool addDrawOpImmediate = true);
    ~ExtendRecordingCanvas() override = default;
    void DrawImageWithParm(const std::shared_ptr<Drawing::Image>& image, const std::shared_ptr<Drawing::Data>& data,
        const Drawing::AdaptiveImageInfo& rsImageInfo, const Drawing::SamplingOptions& sampling);
    void DrawPixelMapWithParm(const std::shared_ptr<Media::PixelMap>& pixelMap,
        const Drawing::AdaptiveImageInfo& rsImageInfo, const Drawing::SamplingOptions& sampling);
    void DrawImageNineWithPixelMap(const std::shared_ptr<Media::PixelMap>& pixelmap, const Drawing::RectI& center,
        const Drawing::Rect& dst, Drawing::FilterMode filter, const Drawing::Brush* brush);
    void DrawPixelMapRect(const std::shared_ptr<Media::PixelMap>& pixelMap, const Drawing::Rect& src,
        const Drawing::Rect& dst, const Drawing::SamplingOptions& sampling,
        Drawing::SrcRectConstraint constraint = Drawing::SrcRectConstraint::STRICT_SRC_RECT_CONSTRAINT);
    void DrawDrawFunc(Drawing::RecordingCanvas::DrawFunc&& drawFunc);
#ifdef ROSEN_OHOS
    void DrawSurfaceBuffer(const DrawingSurfaceBufferInfo& surfaceBufferInfo);
#endif
private:
    template<typename T, typename... Args>
    void AddDrawOpImmediate(Args&&... args);
    template<typename T, typename... Args>
    void AddDrawOpDeferred(Args&&... args);
};
} // namespace Rosen
} // namespace OHOS
#endif // USE_ROSEN_DRAWING
#endif // RENDER_SERVICE_CLIENT_CORE_PIPELINE_RS_RECORDING_CANVAS_H
