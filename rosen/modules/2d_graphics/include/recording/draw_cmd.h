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

#ifndef DRAW_CMD_H
#define DRAW_CMD_H

#include <unordered_map>

#include "draw/canvas.h"
#include "draw/pen.h"
#include "recording/cmd_list.h"
#include "recording/recording_canvas.h"

namespace OHOS {
namespace Rosen {
namespace Drawing {
struct BrushHandle {
    Color color;
    BlendMode mode;
    bool isAntiAlias;
    Filter::FilterQuality filterQuality;
    CmdListHandle colorSpaceHandle;
    CmdListHandle shaderEffectHandle;
    CmdListHandle colorFilterHandle;
    CmdListHandle imageFilterHandle;
    CmdListHandle maskFilterHandle;
};

struct PenHandle {
    Color color;
    scalar width;
    scalar miterLimit;
    Pen::CapStyle capStyle;
    Pen::JoinStyle joinStyle;
    BlendMode mode;
    bool isAntiAlias;
    Filter::FilterQuality filterQuality;
    CmdListHandle pathEffectHandle;
    CmdListHandle colorSpaceHandle;
    CmdListHandle shaderEffectHandle;
    CmdListHandle colorFilterHandle;
    CmdListHandle imageFilterHandle;
    CmdListHandle maskFilterHandle;
};

class CanvasPlayer {
public:
    CanvasPlayer(Canvas& canvas, const CmdList& cmdList, const Rect& rect);
    ~CanvasPlayer() = default;

    bool Playback(uint32_t type, const void* opItem);

    Canvas& canvas_;
    const CmdList& cmdList_;
    const Rect& rect_;

    using PlaybackFunc = void(*)(CanvasPlayer& palyer, const void* opItem);
private:
    static std::unordered_map<uint32_t, PlaybackFunc> opPlaybackFuncLUT_;
};

class DrawOpItem : public OpItem {
public:
    explicit DrawOpItem(uint32_t type) : OpItem(type) {}
    ~DrawOpItem() = default;

    enum Type : uint32_t {
        OPITEM_HEAD,
        POINT_OPITEM,
        POINTS_OPITEM,
        LINE_OPITEM,
        RECT_OPITEM,
        ROUND_RECT_OPITEM,
        NESTED_ROUND_RECT_OPITEM,
        ARC_OPITEM,
        PIE_OPITEM,
        OVAL_OPITEM,
        CIRCLE_OPITEM,
        COLOR_OPITEM,
        IMAGE_NINE_OPITEM,
        IMAGE_ANNOTATION_OPITEM,
        IMAGE_LATTICE_OPITEM,
        PATH_OPITEM,
        BACKGROUND_OPITEM,
        SHADOW_OPITEM,
        BITMAP_OPITEM,
        IMAGE_OPITEM,
        IMAGE_RECT_OPITEM,
        PICTURE_OPITEM,
        CLIP_RECT_OPITEM,
        CLIP_IRECT_OPITEM,
        CLIP_ROUND_RECT_OPITEM,
        CLIP_PATH_OPITEM,
        CLIP_REGION_OPITEM,
        SET_MATRIX_OPITEM,
        RESET_MATRIX_OPITEM,
        CONCAT_MATRIX_OPITEM,
        TRANSLATE_OPITEM,
        SCALE_OPITEM,
        ROTATE_OPITEM,
        SHEAR_OPITEM,
        FLUSH_OPITEM,
        CLEAR_OPITEM,
        SAVE_OPITEM,
        SAVE_LAYER_OPITEM,
        RESTORE_OPITEM,
        DISCARD_OPITEM,
        ATTACH_PEN_OPITEM,
        ATTACH_BRUSH_OPITEM,
        DETACH_PEN_OPITEM,
        DETACH_BRUSH_OPITEM,
        CLIP_ADAPTIVE_ROUND_RECT_OPITEM,
        ADAPTIVE_IMAGE_OPITEM,
        ADAPTIVE_PIXELMAP_OPITEM,
        REGION_OPITEM,
        PATCH_OPITEM,
        EDGEAAQUAD_OPITEM,
        VERTICES_OPITEM,
    };
};

class DrawPointOpItem : public DrawOpItem {
public:
    explicit DrawPointOpItem(const Point& point);
    ~DrawPointOpItem() = default;

    static void Playback(CanvasPlayer& player, const void* opItem);
    void Playback(Canvas& canvas) const;

private:
    Point point_;
};

class DrawPointsOpItem : public DrawOpItem {
public:
    explicit DrawPointsOpItem(PointMode mode, const std::pair<uint32_t, size_t> pts);
    ~DrawPointsOpItem() = default;

    static void Playback(CanvasPlayer& player, const void* opItem);
    void Playback(Canvas& canvas, const CmdList& cmdList) const;

private:
    PointMode mode_;
    const std::pair<uint32_t, size_t> pts_;
};

class DrawLineOpItem : public DrawOpItem {
public:
    DrawLineOpItem(const Point& startPt, const Point& endPt);
    ~DrawLineOpItem() = default;

    static void Playback(CanvasPlayer& player, const void* opItem);
    void Playback(Canvas& canvas) const;

private:
    Point startPt_;
    Point endPt_;
};

class DrawRectOpItem : public DrawOpItem {
public:
    explicit DrawRectOpItem(const Rect& rect);
    ~DrawRectOpItem() = default;

    static void Playback(CanvasPlayer& player, const void* opItem);
    void Playback(Canvas& canvas) const;

private:
    Rect rect_;
};

class DrawRoundRectOpItem : public DrawOpItem {
public:
    explicit DrawRoundRectOpItem(const std::pair<uint32_t, size_t> radiusXYData, const Rect& rect);
    ~DrawRoundRectOpItem()  = default;

    static void Playback(CanvasPlayer& player, const void* opItem);
    void Playback(Canvas& canvas, const CmdList& cmdList) const;

private:
    std::pair<uint32_t, size_t> radiusXYData_;
    Rect rect_;
};

class DrawNestedRoundRectOpItem : public DrawOpItem {
public:
    DrawNestedRoundRectOpItem(const std::pair<uint32_t, size_t> outerRadiusXYData, const Rect& outerRect,
        const std::pair<uint32_t, size_t> innerRadiusXYData, const Rect& innerRect);
    ~DrawNestedRoundRectOpItem() = default;

    static void Playback(CanvasPlayer& player, const void* opItem);
    void Playback(Canvas& canvas, const CmdList& cmdList) const;

private:
    std::pair<uint32_t, size_t> outerRadiusXYData_;
    std::pair<uint32_t, size_t> innerRadiusXYData_;
    Rect outerRect_;
    Rect innerRect_;
};

class DrawArcOpItem : public DrawOpItem {
public:
    DrawArcOpItem(const Rect& rect, scalar startAngle, scalar sweepAngle);
    ~DrawArcOpItem() = default;

    static void Playback(CanvasPlayer& player, const void* opItem);
    void Playback(Canvas& canvas) const;

private:
    Rect rect_;
    scalar startAngle_;
    scalar sweepAngle_;
};

class DrawPieOpItem : public DrawOpItem {
public:
    DrawPieOpItem(const Rect& rect, scalar startAngle, scalar sweepAngle);
    ~DrawPieOpItem() = default;

    static void Playback(CanvasPlayer& player, const void* opItem);
    void Playback(Canvas& canvas) const;

private:
    Rect rect_;
    scalar startAngle_;
    scalar sweepAngle_;
};

class DrawOvalOpItem : public DrawOpItem {
public:
    explicit DrawOvalOpItem(const Rect& rect);
    ~DrawOvalOpItem() = default;

    static void Playback(CanvasPlayer& player, const void* opItem);
    void Playback(Canvas& canvas) const;

private:
    Rect rect_;
};

class DrawCircleOpItem : public DrawOpItem {
public:
    DrawCircleOpItem(const Point& centerPt, scalar radius);
    ~DrawCircleOpItem() = default;

    static void Playback(CanvasPlayer& player, const void* opItem);
    void Playback(Canvas& canvas) const;

private:
    Point centerPt_;
    scalar radius_;
};

class DrawPathOpItem : public DrawOpItem {
public:
    explicit DrawPathOpItem(const CmdListHandle& path);
    ~DrawPathOpItem() = default;

    static void Playback(CanvasPlayer& player, const void* opItem);
    void Playback(Canvas& canvas, const CmdList& cmdList) const;

private:
    CmdListHandle path_;
};

class DrawBackgroundOpItem : public DrawOpItem {
public:
    DrawBackgroundOpItem(const BrushHandle& brushHandle);
    ~DrawBackgroundOpItem() = default;

    static void Playback(CanvasPlayer& player, const void* opItem);
    void Playback(Canvas& canvas, const CmdList& cmdList) const;

private:
    BrushHandle brushHandle_;
};

class DrawShadowOpItem : public DrawOpItem {
public:
    DrawShadowOpItem(const CmdListHandle& path, const Point3& planeParams, const Point3& devLightPos,
        scalar lightRadius, Color ambientColor, Color spotColor, ShadowFlags flag);
    ~DrawShadowOpItem() = default;

    static void Playback(CanvasPlayer& player, const void* opItem);
    void Playback(Canvas& canvas, const CmdList& cmdList) const;

private:
    CmdListHandle path_;
    Point3 planeParams_;
    Point3 devLightPos_;
    scalar lightRadius_;
    Color ambientColor_;
    Color spotColor_;
    ShadowFlags flag_;
};

class DrawRegionOpItem : public DrawOpItem {
public:
    DrawRegionOpItem(const CmdListHandle& path);
    ~DrawRegionOpItem() = default;

    static void Playback(CanvasPlayer& player, const void* opItem);
    void Playback(Canvas& canvas, const CmdList& cmdList) const;

private:
    CmdListHandle region_;
};

class DrawPatchOpItem : public DrawOpItem {
public:
    explicit DrawPatchOpItem(const std::pair<uint32_t, size_t> cubics, const std::pair<uint32_t, size_t> colors,
        const std::pair<uint32_t, size_t> texCoords, BlendMode mode);
    ~DrawPatchOpItem() = default;

    static void Playback(CanvasPlayer& player, const void* opItem);
    void Playback(Canvas& canvas, const CmdList& cmdList) const;

private:
    std::pair<uint32_t, size_t> cubics_;
    std::pair<uint32_t, size_t> colors_;
    std::pair<uint32_t, size_t> texCoords_;
    BlendMode mode_;
};

class DrawEdgeAAQuadOpItem : public DrawOpItem {
public:
    explicit DrawEdgeAAQuadOpItem(const Rect& rect, const std::pair<uint32_t, size_t> clipQuad,
        QuadAAFlags aaFlags, ColorQuad color, BlendMode mode);
    ~DrawEdgeAAQuadOpItem() = default;

    static void Playback(CanvasPlayer& player, const void* opItem);
    void Playback(Canvas& canvas, const CmdList& cmdList) const;

private:
    Rect rect_;
    std::pair<uint32_t, size_t> clipQuad_;
    QuadAAFlags aaFlags_;
    ColorQuad color_;
    BlendMode mode_;
};

class DrawImageNineOpItem : public DrawOpItem {
public:
    explicit DrawImageNineOpItem(const ImageHandle& image, const RectI& center, const Rect& dst,
        FilterMode filterMode, const BrushHandle& brushHandle, bool hasBrush);
    ~DrawImageNineOpItem() = default;

    static void Playback(CanvasPlayer& player, const void* opItem);
    void Playback(Canvas& canvas, const CmdList& cmdList) const;

private:
    ImageHandle image_;
    RectI center_;
    Rect dst_;
    FilterMode filter_;
    BrushHandle brushHandle_;
    bool hasBrush_;
};

class DrawAnnotationOpItem : public DrawOpItem {
public:
    explicit DrawAnnotationOpItem(const Rect& rect, const char* key, const ImageHandle& data);
    ~DrawAnnotationOpItem() = default;

    static void Playback(CanvasPlayer& player, const void* opItem);
    void Playback(Canvas& canvas, const CmdList& cmdList) const;

private:
    Rect rect_;
    const char* key_;
    const ImageHandle data_;
};

class DrawImageLatticeOpItem : public DrawOpItem {
public:
    explicit DrawImageLatticeOpItem(const ImageHandle& image, const Lattice& lattice, const Rect& dst,
        FilterMode filterMode, const BrushHandle& brushHandle, bool hasBrush);
    ~DrawImageLatticeOpItem() = default;

    static void Playback(CanvasPlayer& player, const void* opItem);
    void Playback(Canvas& canvas, const CmdList& cmdList) const;

private:
    ImageHandle image_;
    Lattice lattice_;
    Rect dst_;
    FilterMode filter_;
    BrushHandle brushHandle_;
    bool hasBrush_;
};

class DrawVerticesOpItem : public DrawOpItem {
public:
    DrawVerticesOpItem(const VerticesHandle& vertices, BlendMode mode);
    ~DrawVerticesOpItem() = default;

    static void Playback(CanvasPlayer& player, const void* opItem);
    void Playback(Canvas& canvas, const CmdList& cmdList) const;

private:
    VerticesHandle vertices_;
    BlendMode mode_;
};

class DrawBitmapOpItem : public DrawOpItem {
public:
    DrawBitmapOpItem(const ImageHandle& bitmap, scalar px, scalar py);
    ~DrawBitmapOpItem() = default;

    static void Playback(CanvasPlayer& player, const void* opItem);
    void Playback(Canvas& canvas, const CmdList& cmdList) const;

private:
    ImageHandle bitmap_;
    scalar px_;
    scalar py_;
};

class DrawImageOpItem : public DrawOpItem {
public:
    DrawImageOpItem(const ImageHandle& image, scalar px, scalar py, const SamplingOptions& samplingOptions);
    ~DrawImageOpItem() = default;

    static void Playback(CanvasPlayer& player, const void* opItem);
    void Playback(Canvas& canvas, const CmdList& cmdList) const;

private:
    ImageHandle image_;
    scalar px_;
    scalar py_;
    SamplingOptions samplingOptions_;
};

class DrawImageRectOpItem : public DrawOpItem {
public:
    DrawImageRectOpItem(const ImageHandle& image, const Rect& src, const Rect& dst,
        const SamplingOptions& sampling, SrcRectConstraint constraint);
    ~DrawImageRectOpItem() = default;

    static void Playback(CanvasPlayer& player, const void* opItem);
    void Playback(Canvas& canvas, const CmdList& cmdList) const;

private:
    ImageHandle image_;
    Rect src_;
    Rect dst_;
    SamplingOptions sampling_;
    SrcRectConstraint constraint_;
};

class DrawPictureOpItem : public DrawOpItem {
public:
    explicit DrawPictureOpItem(const ImageHandle& picture);
    ~DrawPictureOpItem() = default;

    static void Playback(CanvasPlayer& player, const void* opItem);
    void Playback(Canvas& canvas, const CmdList& cmdList) const;

private:
    ImageHandle picture_;
};

class DrawColorOpItem : public DrawOpItem {
public:
    explicit DrawColorOpItem(ColorQuad color, BlendMode mode);
    ~DrawColorOpItem() = default;

    static void Playback(CanvasPlayer& player, const void* opItem);
    void Playback(Canvas& canvas) const;

private:
    ColorQuad color_;
    BlendMode mode_;
};

class ClipRectOpItem : public DrawOpItem {
public:
    ClipRectOpItem(const Rect& rect, ClipOp op, bool doAntiAlias);
    ~ClipRectOpItem() = default;

    static void Playback(CanvasPlayer& player, const void* opItem);
    void Playback(Canvas& canvas) const;

private:
    Rect rect_;
    ClipOp clipOp_;
    bool doAntiAlias_;
};

class ClipIRectOpItem : public DrawOpItem {
public:
    ClipIRectOpItem(const RectI& rect, ClipOp op = ClipOp::INTERSECT);
    ~ClipIRectOpItem() = default;

    static void Playback(CanvasPlayer& player, const void* opItem);
    void Playback(Canvas& canvas) const;

private:
    RectI rect_;
    ClipOp clipOp_;
};

class ClipRoundRectOpItem : public DrawOpItem {
public:
    ClipRoundRectOpItem(const std::pair<uint32_t, size_t> radiusXYData, const Rect& rect, ClipOp op, bool doAntiAlias);
    ~ClipRoundRectOpItem() = default;

    static void Playback(CanvasPlayer& player, const void* opItem);
    void Playback(Canvas& canvas, const CmdList& cmdList) const;

private:
    std::pair<uint32_t, size_t> radiusXYData_;
    Rect rect_;
    ClipOp clipOp_;
    bool doAntiAlias_;
};

class ClipPathOpItem : public DrawOpItem {
public:
    ClipPathOpItem(const CmdListHandle& path, ClipOp clipOp, bool doAntiAlias);
    ~ClipPathOpItem() = default;

    static void Playback(CanvasPlayer& player, const void* opItem);
    void Playback(Canvas& canvas, const CmdList& cmdList) const;

private:
    CmdListHandle path_;
    ClipOp clipOp_;
    bool doAntiAlias_;
};

class ClipRegionOpItem : public DrawOpItem {
public:
    ClipRegionOpItem(const CmdListHandle& region, ClipOp clipOp = ClipOp::INTERSECT);
    ~ClipRegionOpItem() = default;

    static void Playback(CanvasPlayer& player, const void* opItem);
    void Playback(Canvas& canvas, const CmdList& cmdList) const;

private:
    CmdListHandle region_;
    ClipOp clipOp_;
};

class SetMatrixOpItem : public DrawOpItem {
public:
    explicit SetMatrixOpItem(const Matrix& matrix);
    ~SetMatrixOpItem() = default;

    static void Playback(CanvasPlayer& player, const void* opItem);
    void Playback(Canvas& canvas) const;

private:
    Matrix::Buffer matrixBuffer_;
};

class ResetMatrixOpItem : public DrawOpItem {
public:
    ResetMatrixOpItem();
    ~ResetMatrixOpItem() = default;

    static void Playback(CanvasPlayer& player, const void* opItem);
    void Playback(Canvas& canvas) const;
};

class ConcatMatrixOpItem : public DrawOpItem {
public:
    explicit ConcatMatrixOpItem(const Matrix& matrix);
    ~ConcatMatrixOpItem() = default;

    static void Playback(CanvasPlayer& player, const void* opItem);
    void Playback(Canvas& canvas) const;

private:
    Matrix::Buffer matrixBuffer_;
};

class TranslateOpItem : public DrawOpItem {
public:
    TranslateOpItem(scalar dx, scalar dy);
    ~TranslateOpItem() = default;

    static void Playback(CanvasPlayer& player, const void* opItem);
    void Playback(Canvas& canvas) const;

private:
    scalar dx_;
    scalar dy_;
};

class ScaleOpItem : public DrawOpItem {
public:
    ScaleOpItem(scalar sx, scalar sy);
    ~ScaleOpItem() = default;

    static void Playback(CanvasPlayer& player, const void* opItem);
    void Playback(Canvas& canvas) const;

private:
    scalar sx_;
    scalar sy_;
};

class RotateOpItem : public DrawOpItem {
public:
    RotateOpItem(scalar deg, scalar sx, scalar sy);
    ~RotateOpItem() = default;

    static void Playback(CanvasPlayer& player, const void* opItem);
    void Playback(Canvas& canvas) const;

private:
    scalar deg_;
    scalar sx_;
    scalar sy_;
};

class ShearOpItem : public DrawOpItem {
public:
    ShearOpItem(scalar sx, scalar sy);
    ~ShearOpItem() = default;

    static void Playback(CanvasPlayer& player, const void* opItem);
    void Playback(Canvas& canvas) const;

private:
    scalar sx_;
    scalar sy_;
};

class FlushOpItem : public DrawOpItem {
public:
    FlushOpItem();
    ~FlushOpItem() = default;

    static void Playback(CanvasPlayer& player, const void* opItem);
    void Playback(Canvas& canvas) const;
};

class ClearOpItem : public DrawOpItem {
public:
    explicit ClearOpItem(ColorQuad color);
    ~ClearOpItem() = default;

    static void Playback(CanvasPlayer& player, const void* opItem);
    void Playback(Canvas& canvas) const;

private:
    ColorQuad color_;
};

class SaveOpItem : public DrawOpItem {
public:
    SaveOpItem();
    ~SaveOpItem() = default;

    static void Playback(CanvasPlayer& player, const void* opItem);
    void Playback(Canvas& canvas) const;
};

class SaveLayerOpItem : public DrawOpItem {
public:
    SaveLayerOpItem(const Rect& rect, bool hasBrush, const BrushHandle& brushHandle, const CmdListHandle& imageFilter,
        uint32_t saveLayerFlags);
    ~SaveLayerOpItem() = default;

    static void Playback(CanvasPlayer& player, const void* opItem);
    void Playback(Canvas& canvas, const CmdList& cmdList) const;

private:
    Rect rect_;
    bool hasBrush_;
    BrushHandle brushHandle_;
    CmdListHandle imageFilter_;
    uint32_t saveLayerFlags_;
};

class RestoreOpItem : public DrawOpItem {
public:
    RestoreOpItem();
    ~RestoreOpItem() = default;

    static void Playback(CanvasPlayer& player, const void* opItem);
    void Playback(Canvas& canvas) const;
};

class DiscardOpItem : public DrawOpItem {
public:
    DiscardOpItem();
    ~DiscardOpItem() = default;

    static void Playback(CanvasPlayer& player, const void* opItem);
    void Playback(Canvas& canvas) const;
};

class AttachPenOpItem : public DrawOpItem {
public:
    AttachPenOpItem(const PenHandle& penHandle);
    ~AttachPenOpItem() = default;

    static void Playback(CanvasPlayer& player, const void* opItem);
    void Playback(Canvas& canvas, const CmdList& cmdList) const;

private:
    PenHandle penHandle_;
};

class AttachBrushOpItem : public DrawOpItem {
public:
    AttachBrushOpItem(const BrushHandle& brushHandle);
    ~AttachBrushOpItem() = default;

    static void Playback(CanvasPlayer& player, const void* opItem);
    void Playback(Canvas& canvas, const CmdList& cmdList) const;

private:
    BrushHandle brushHandle_;
};

class DetachPenOpItem : public DrawOpItem {
public:
    DetachPenOpItem();
    ~DetachPenOpItem() = default;

    static void Playback(CanvasPlayer& player, const void* opItem);
    void Playback(Canvas& canvas) const;
};

class DetachBrushOpItem : public DrawOpItem {
public:
    DetachBrushOpItem();
    ~DetachBrushOpItem() = default;

    static void Playback(CanvasPlayer& player, const void* opItem);
    void Playback(Canvas& canvas) const;
};

class ClipAdaptiveRoundRectOpItem : public DrawOpItem {
public:
    ClipAdaptiveRoundRectOpItem(const std::pair<uint32_t, size_t>& radiusData);
    ~ClipAdaptiveRoundRectOpItem() = default;

    static void Playback(CanvasPlayer& player, const void* opItem);
    void Playback(Canvas& canvas, const CmdList& cmdList, const Rect& rect) const;

private:
    std::pair<uint32_t, size_t> radiusData_;
};

class DrawAdaptiveImageOpItem : public DrawOpItem {
public:
    DrawAdaptiveImageOpItem(const ImageHandle& image, const AdaptiveImageInfo& rsImageInfo,
        const SamplingOptions& smapling, const bool isImage);
    ~DrawAdaptiveImageOpItem() = default;

    static void Playback(CanvasPlayer& player, const void* opItem);
    void Playback(Canvas& canvas, const CmdList& cmdList, const Rect& rect) const;

private:
    ImageHandle image_;
    AdaptiveImageInfo rsImageInfo_;
    SamplingOptions sampling_;
    bool isImage_;
};

class DrawAdaptivePixelMapOpItem : public DrawOpItem {
public:
    DrawAdaptivePixelMapOpItem(const ImageHandle& pixelMap, const AdaptiveImageInfo& imageInfo,
        const SamplingOptions& smapling);
    ~DrawAdaptivePixelMapOpItem() = default;

    static void Playback(CanvasPlayer& player, const void* opItem);
    void Playback(Canvas& canvas, const CmdList& cmdList, const Rect& rect) const;

private:
    ImageHandle pixelMap_;
    AdaptiveImageInfo imageInfo_;
    SamplingOptions smapling_;
};
} // namespace Drawing
} // namespace Rosen
} // namespace OHOS
#endif
