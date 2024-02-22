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

#ifndef RECORDING_CANVAS_H
#define RECORDING_CANVAS_H

#include <optional>
#include <stack>

#include "draw/canvas.h"
#include "image/gpu_context.h"
#include "recording/adaptive_image_helper.h"
#include "recording/draw_cmd_list.h"
#include "recording/recording_handle.h"
#ifdef ROSEN_OHOS
#include "surface_buffer.h"
#endif

namespace OHOS {
namespace Rosen {
namespace Drawing {
/*
 * @brief  RecordingCanvas is an empty canvas, which does not act on any surface,
 * and is used to record the sequence of draw calls for the canvas.
 * Draw calls are kept in linear memory in DrawCmdList, Subsequent playback can be performed through DrawCmdList.
 */
class DRAWING_API RecordingCanvas : public NoDrawCanvas {
public:
    RecordingCanvas(int32_t width, int32_t height, bool addDrawOpImmediate = true);
    ~RecordingCanvas() override = default;

    std::shared_ptr<DrawCmdList> GetDrawCmdList() const;

    DrawingType GetDrawingType() const override
    {
        return DrawingType::RECORDING;
    }

    void SetGrRecordingContext(std::shared_ptr<GPUContext> gpuContext)
    {
        gpuContext_ = gpuContext;
    }

#ifdef ACE_ENABLE_GPU
    std::shared_ptr<GPUContext> GetGPUContext() override
    {
        return gpuContext_;
    }
#endif

    void Clear() const;

    void ResetCanvas(int32_t width, int32_t height);

    void DrawPoint(const Point& point) override;
    void DrawPoints(PointMode mode, size_t count, const Point pts[]) override;
    void DrawLine(const Point& startPt, const Point& endPt) override;
    void DrawRect(const Rect& rect) override;
    void DrawRoundRect(const RoundRect& roundRect) override;
    void DrawNestedRoundRect(const RoundRect& outer, const RoundRect& inner) override;
    void DrawArc(const Rect& oval, scalar startAngle, scalar sweepAngle) override;
    void DrawPie(const Rect& oval, scalar startAngle, scalar sweepAngle) override;
    void DrawOval(const Rect& oval) override;
    void DrawCircle(const Point& centerPt, scalar radius) override;
    void DrawPath(const Path& path) override;
    void DrawBackground(const Brush& brush) override;
    void DrawShadow(const Path& path, const Point3& planeParams, const Point3& devLightPos, scalar lightRadius,
        Color ambientColor, Color spotColor, ShadowFlags flag) override;
    void DrawRegion(const Region& region) override;
    void DrawPatch(const Point cubics[12], const ColorQuad colors[4],
        const Point texCoords[4], BlendMode mode) override;
    void DrawVertices(const Vertices& vertices, BlendMode mode) override;

    void DrawImageNine(const Image* image, const RectI& center, const Rect& dst,
        FilterMode filterMode, const Brush* brush = nullptr) override;
    void DrawImageLattice(const Image* image, const Lattice& lattice, const Rect& dst,
        FilterMode filterMode, const Brush* brush = nullptr) override;

    void DrawColor(ColorQuad color, BlendMode mode = BlendMode::SRC_OVER) override;

    void DrawBitmap(const Bitmap& bitmap, const scalar px, const scalar py) override;
    void DrawImage(const Image& image, const scalar px, const scalar py, const SamplingOptions& sampling) override;
    void DrawImageRect(const Image& image, const Rect& src, const Rect& dst, const SamplingOptions& sampling,
        SrcRectConstraint constraint = SrcRectConstraint::STRICT_SRC_RECT_CONSTRAINT) override;
    void DrawImageRect(const Image& image, const Rect& dst, const SamplingOptions& sampling) override;
    void DrawPicture(const Picture& picture) override;
    void DrawTextBlob(const TextBlob* blob, const scalar x, const scalar y) override;
    void DrawSymbol(const DrawingHMSymbolData& symbol, Point locate) override;

    void ClipRect(const Rect& rect, ClipOp op, bool doAntiAlias) override;
    void ClipIRect(const RectI& rect, ClipOp op = ClipOp::INTERSECT) override;
    void ClipRoundRect(const RoundRect& roundRect, ClipOp op, bool doAntiAlias) override;
    void ClipRoundRect(const Rect& rect, std::vector<Point>& pts, bool doAntiAlias) override;
    void ClipPath(const Path& path, ClipOp op, bool doAntiAlias) override;
    void ClipRegion(const Region& region, ClipOp op = ClipOp::INTERSECT) override;

    void SetMatrix(const Matrix& matrix) override;
    void ResetMatrix() override;
    void ConcatMatrix(const Matrix& matrix) override;
    void Translate(scalar dx, scalar dy) override;
    void Scale(scalar sx, scalar sy) override;
    void Rotate(scalar deg, scalar sx, scalar sy) override;
    void Shear(scalar sx, scalar sy) override;

    void Flush() override;
    void Clear(ColorQuad color) override;
    uint32_t Save() override;
    void SaveLayer(const SaveLayerOps& saveLayerOps) override;
    void Restore() override;
    uint32_t GetSaveCount() const override;
    void Discard() override;

    void ClipAdaptiveRoundRect(const std::vector<Point>& radius);
    void DrawImage(const std::shared_ptr<Image>& image, const std::shared_ptr<Data>& data,
        const AdaptiveImageInfo& rsImageInfo, const SamplingOptions& sampling);
    void DrawPixelMap(const std::shared_ptr<Media::PixelMap>& pixelMap,
        const AdaptiveImageInfo& rsImageInfo, const SamplingOptions& sampling);

    void SetIsCustomTextType(bool isCustomTextType);
    bool IsCustomTextType() const;

    using DrawFunc = std::function<void(Drawing::Canvas* canvas, const Drawing::Rect* rect)>;
protected:
    bool addDrawOpImmediate_ = true;
    std::shared_ptr<DrawCmdList> cmdList_ = nullptr;

private:
    template<typename T, typename... Args>
    void AddDrawOpImmediate(Args&&... args);
    template<typename T, typename... Args>
    void AddDrawOpDeferred(Args&&... args);

    enum SaveOpState { LazySaveOp, RealSaveOp };
    void CheckForLazySave();
    void GenerateCachedOpForTextblob(const TextBlob* blob, const scalar x, const scalar y);
    void GenerateCachedOpForTextblob(const TextBlob* blob, const scalar x, const scalar y, Paint& paint);
    bool isCustomTextType_ = false;
    std::optional<Brush> customTextBrush_ = std::nullopt;
    std::optional<Pen> customTextPen_ = std::nullopt;
    std::stack<SaveOpState> saveOpStateStack_;
    std::shared_ptr<GPUContext> gpuContext_ = nullptr;
};
} // namespace Drawing
} // namespace Rosen
} // namespace OHOS
#endif
