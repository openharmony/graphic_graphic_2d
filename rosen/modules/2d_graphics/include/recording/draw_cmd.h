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
#include <functional>
#include <stack>

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
    ImageHandle colorSpaceHandle;
    FlattenableHandle shaderEffectHandle;
    FlattenableHandle colorFilterHandle;
    FlattenableHandle imageFilterHandle;
    FlattenableHandle maskFilterHandle;
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
    FlattenableHandle pathEffectHandle;
    ImageHandle colorSpaceHandle;
    FlattenableHandle shaderEffectHandle;
    FlattenableHandle colorFilterHandle;
    FlattenableHandle imageFilterHandle;
    FlattenableHandle maskFilterHandle;
};

class CanvasPlayer {
public:
    CanvasPlayer(Canvas& canvas, const CmdList& cmdList, const Rect& rect);
    ~CanvasPlayer() = default;

    bool Playback(uint32_t type, void* opItem);

    Canvas& canvas_;
    const CmdList& cmdList_;
    const Rect& rect_;

    using PlaybackFunc = void(*)(CanvasPlayer& palyer, void* opItem);
    static PlaybackFunc GetFuncFromType(uint32_t type);
private:
    static std::unordered_map<uint32_t, PlaybackFunc> opPlaybackFuncLUT_;
};

class UnmarshallingPlayer {
public:
    UnmarshallingPlayer(const CmdList& cmdList);
    ~UnmarshallingPlayer() = default;

    std::shared_ptr<OpItem> Unmarshalling(uint32_t type, void* opItem);

    const CmdList& cmdList_;

    using UnmarshallingFunc = std::shared_ptr<OpItem>(*)(const CmdList& cmdList, void* opItem);
private:
    static std::unordered_map<uint32_t, UnmarshallingFunc> opUnmarshallingFuncLUT_;
};

class NoIPCImageOpItem;

class GenerateCachedOpItemPlayer {
public:
    GenerateCachedOpItemPlayer(CmdList &cmdList, Canvas* canvas = nullptr, const Rect* rect = nullptr);
    ~GenerateCachedOpItemPlayer() = default;

    std::shared_ptr<NoIPCImageOpItem> GenerateCachedOpItem(uint32_t type, void* opItem, bool addCmdList,
        bool& replaceSuccess);
    
    Canvas* canvas_;
    const Rect* rect_;
    CmdList& cmdList_;
    std::stack<void*> brushOpItemStack;
    std::stack<void*> penOpItemStack;
};

class DrawOpItem : public OpItem {
public:
    explicit DrawOpItem(uint32_t type) : OpItem(type) {}
    ~DrawOpItem() override = default;

    enum Type : uint32_t {
        OPITEM_HEAD,
        CMD_LIST_OPITEM,
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
        TEXT_BLOB_OPITEM,
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
        EXTEND_PIXELMAP_OPITEM,
        IMAGE_WITH_PARM_OPITEM,
        REGION_OPITEM,
        PATCH_OPITEM,
        EDGEAAQUAD_OPITEM,
        VERTICES_OPITEM,
        NO_IPC_IMAGE_DRAW_OPITEM,
    };
};

class DrawCmdListOpItem : public DrawOpItem {
public:
    DrawCmdListOpItem();
    explicit DrawCmdListOpItem(const CmdListHandle& handle);
    ~DrawCmdListOpItem() override = default;

    static std::shared_ptr<OpItem> Unmarshalling(const CmdList& cmdList, void* opItem);
    void Unmarshalling(const CmdList& cmdList, Canvas* canvas = nullptr);

    static void Playback(CanvasPlayer& player, void* opItem);
    void Playback(Canvas& canvas, const CmdList& cmdList, const Rect& rect);

private:
    CmdListHandle handle_;
    std::function<void(Canvas&)> playbackTask_ = nullptr;
};

class DrawPointOpItem : public DrawOpItem {
public:
    DrawPointOpItem();
    explicit DrawPointOpItem(const Point& point);
    ~DrawPointOpItem() override = default;

    static std::shared_ptr<OpItem> Unmarshalling(const CmdList& cmdList, void* opItem);
    static void Playback(CanvasPlayer& player, void* opItem);
    void Playback(Canvas& canvas);

private:
    Point point_;
};

class DrawPointsOpItem : public DrawOpItem {
public:
    DrawPointsOpItem();
    explicit DrawPointsOpItem(PointMode mode, const std::pair<uint32_t, size_t> pts);
    ~DrawPointsOpItem() override = default;

    static std::shared_ptr<OpItem> Unmarshalling(const CmdList& cmdList, void* opItem);
    void Unmarshalling(const CmdList& cmdList, Canvas* canvas = nullptr);

    static void Playback(CanvasPlayer& player, void* opItem);
    void Playback(Canvas& canvas, const CmdList& cmdList);

private:
    PointMode mode_;
    const std::pair<uint32_t, size_t> pts_;
    std::function<void(Canvas&)> playbackTask_ = nullptr;
};

class DrawLineOpItem : public DrawOpItem {
public:
    DrawLineOpItem();
    DrawLineOpItem(const Point& startPt, const Point& endPt);
    ~DrawLineOpItem() override = default;

    static std::shared_ptr<OpItem> Unmarshalling(const CmdList& cmdList, void* opItem);
    static void Playback(CanvasPlayer& player, void* opItem);
    void Playback(Canvas& canvas);

private:
    Point startPt_;
    Point endPt_;
};

class DrawRectOpItem : public DrawOpItem {
public:
    DrawRectOpItem();
    explicit DrawRectOpItem(const Rect& rect);
    ~DrawRectOpItem() override = default;

    static std::shared_ptr<OpItem> Unmarshalling(const CmdList& cmdList, void* opItem);
    static void Playback(CanvasPlayer& player, void* opItem);
    void Playback(Canvas& canvas);

private:
    Rect rect_;
};

class DrawRoundRectOpItem : public DrawOpItem {
public:
    DrawRoundRectOpItem();
    explicit DrawRoundRectOpItem(const std::pair<uint32_t, size_t> radiusXYData, const Rect& rect);
    ~DrawRoundRectOpItem() override = default;

    static std::shared_ptr<OpItem> Unmarshalling(const CmdList& cmdList, void* opItem);
    void Unmarshalling(const CmdList& cmdList, Canvas* canvas = nullptr);

    static void Playback(CanvasPlayer& player, void* opItem);
    void Playback(Canvas& canvas, const CmdList& cmdList);

private:
    std::pair<uint32_t, size_t> radiusXYData_;
    Rect rect_;
    std::function<void(Canvas&)> playbackTask_ = nullptr;
};

class DrawNestedRoundRectOpItem : public DrawOpItem {
public:
    DrawNestedRoundRectOpItem();
    DrawNestedRoundRectOpItem(const std::pair<uint32_t, size_t> outerRadiusXYData, const Rect& outerRect,
        const std::pair<uint32_t, size_t> innerRadiusXYData, const Rect& innerRect);
    ~DrawNestedRoundRectOpItem() override = default;

    static std::shared_ptr<OpItem> Unmarshalling(const CmdList& cmdList, void* opItem);
    void Unmarshalling(const CmdList& cmdList, Canvas* canvas = nullptr);

    static void Playback(CanvasPlayer& player, void* opItem);
    void Playback(Canvas& canvas, const CmdList& cmdList);

private:
    std::pair<uint32_t, size_t> outerRadiusXYData_;
    std::pair<uint32_t, size_t> innerRadiusXYData_;
    Rect outerRect_;
    Rect innerRect_;
    std::function<void(Canvas&)> playbackTask_ = nullptr;
};

class DrawArcOpItem : public DrawOpItem {
public:
    DrawArcOpItem();
    DrawArcOpItem(const Rect& rect, scalar startAngle, scalar sweepAngle);
    ~DrawArcOpItem() override = default;

    static std::shared_ptr<OpItem> Unmarshalling(const CmdList& cmdList, void* opItem);
    static void Playback(CanvasPlayer& player, void* opItem);
    void Playback(Canvas& canvas);

private:
    Rect rect_;
    scalar startAngle_;
    scalar sweepAngle_;
};

class DrawPieOpItem : public DrawOpItem {
public:
    DrawPieOpItem();
    DrawPieOpItem(const Rect& rect, scalar startAngle, scalar sweepAngle);
    ~DrawPieOpItem() override = default;

    static std::shared_ptr<OpItem> Unmarshalling(const CmdList& cmdList, void* opItem);
    static void Playback(CanvasPlayer& player, void* opItem);
    void Playback(Canvas& canvas);

private:
    Rect rect_;
    scalar startAngle_;
    scalar sweepAngle_;
};

class DrawOvalOpItem : public DrawOpItem {
public:
    DrawOvalOpItem();
    explicit DrawOvalOpItem(const Rect& rect);
    ~DrawOvalOpItem() override = default;

    static std::shared_ptr<OpItem> Unmarshalling(const CmdList& cmdList, void* opItem);
    static void Playback(CanvasPlayer& player, void* opItem);
    void Playback(Canvas& canvas);

private:
    Rect rect_;
};

class DrawCircleOpItem : public DrawOpItem {
public:
    DrawCircleOpItem();
    DrawCircleOpItem(const Point& centerPt, scalar radius);
    ~DrawCircleOpItem() override = default;

    static std::shared_ptr<OpItem> Unmarshalling(const CmdList& cmdList, void* opItem);
    static void Playback(CanvasPlayer& player, void* opItem);
    void Playback(Canvas& canvas);

private:
    Point centerPt_;
    scalar radius_;
};

class DrawPathOpItem : public DrawOpItem {
public:
    DrawPathOpItem();
    explicit DrawPathOpItem(const ImageHandle& path);
    ~DrawPathOpItem() override = default;

    static std::shared_ptr<OpItem> Unmarshalling(const CmdList& cmdList, void* opItem);
    void Unmarshalling(const CmdList& cmdList, Canvas* canvas = nullptr);

    static void Playback(CanvasPlayer& player, void* opItem);
    void Playback(Canvas& canvas, const CmdList& cmdList);

private:
    ImageHandle path_;
    std::function<void(Canvas&)> playbackTask_ = nullptr;
};

class DrawBackgroundOpItem : public DrawOpItem {
public:
    DrawBackgroundOpItem();
    DrawBackgroundOpItem(const BrushHandle& brushHandle);
    ~DrawBackgroundOpItem() override = default;

    static std::shared_ptr<OpItem> Unmarshalling(const CmdList& cmdList, void* opItem);
    void Unmarshalling(const CmdList& cmdList, Canvas* canvas = nullptr);

    static void Playback(CanvasPlayer& player, void* opItem);
    void Playback(Canvas& canvas, const CmdList& cmdList);

private:
    BrushHandle brushHandle_;
    std::function<void(Canvas&)> playbackTask_ = nullptr;
};

class DrawShadowOpItem : public DrawOpItem {
public:
    DrawShadowOpItem();
    DrawShadowOpItem(const ImageHandle& path, const Point3& planeParams, const Point3& devLightPos,
        scalar lightRadius, Color ambientColor, Color spotColor, ShadowFlags flag);
    ~DrawShadowOpItem() override = default;

    static std::shared_ptr<OpItem> Unmarshalling(const CmdList& cmdList, void* opItem);
    void Unmarshalling(const CmdList& cmdList, Canvas* canvas = nullptr);

    static void Playback(CanvasPlayer& player, void* opItem);
    void Playback(Canvas& canvas, const CmdList& cmdList);

private:
    ImageHandle path_;
    Point3 planeParams_;
    Point3 devLightPos_;
    scalar lightRadius_;
    Color ambientColor_;
    Color spotColor_;
    ShadowFlags flag_;
    std::function<void(Canvas&)> playbackTask_ = nullptr;
};

class DrawRegionOpItem : public DrawOpItem {
public:
    DrawRegionOpItem();
    DrawRegionOpItem(const CmdListHandle& path);
    ~DrawRegionOpItem() override = default;

    static std::shared_ptr<OpItem> Unmarshalling(const CmdList& cmdList, void* opItem);
    void Unmarshalling(const CmdList& cmdList, Canvas* canvas = nullptr);

    static void Playback(CanvasPlayer& player, void* opItem);
    void Playback(Canvas& canvas, const CmdList& cmdList);

private:
    CmdListHandle region_;
    std::function<void(Canvas&)> playbackTask_ = nullptr;
};

class DrawPatchOpItem : public DrawOpItem {
public:
    DrawPatchOpItem();
    explicit DrawPatchOpItem(const std::pair<uint32_t, size_t> cubics, const std::pair<uint32_t, size_t> colors,
        const std::pair<uint32_t, size_t> texCoords, BlendMode mode);
    ~DrawPatchOpItem() override = default;

    static std::shared_ptr<OpItem> Unmarshalling(const CmdList& cmdList, void* opItem);
    void Unmarshalling(const CmdList& cmdList, Canvas* canvas = nullptr);

    static void Playback(CanvasPlayer& player, void* opItem);
    void Playback(Canvas& canvas, const CmdList& cmdList);

private:
    std::pair<uint32_t, size_t> cubics_;
    std::pair<uint32_t, size_t> colors_;
    std::pair<uint32_t, size_t> texCoords_;
    BlendMode mode_;
    std::function<void(Canvas&)> playbackTask_ = nullptr;
};

class DrawEdgeAAQuadOpItem : public DrawOpItem {
public:
    DrawEdgeAAQuadOpItem();
    explicit DrawEdgeAAQuadOpItem(const Rect& rect, const std::pair<uint32_t, size_t> clipQuad,
        QuadAAFlags aaFlags, ColorQuad color, BlendMode mode);
    ~DrawEdgeAAQuadOpItem() override = default;

    static std::shared_ptr<OpItem> Unmarshalling(const CmdList& cmdList, void* opItem);
    void Unmarshalling(const CmdList& cmdList, Canvas* canvas = nullptr);

    static void Playback(CanvasPlayer& player, void* opItem);
    void Playback(Canvas& canvas, const CmdList& cmdList);

private:
    Rect rect_;
    std::pair<uint32_t, size_t> clipQuad_;
    QuadAAFlags aaFlags_;
    ColorQuad color_;
    BlendMode mode_;
    std::function<void(Canvas&)> playbackTask_ = nullptr;
};

class DrawImageNineOpItem : public DrawOpItem {
public:
    DrawImageNineOpItem();
    explicit DrawImageNineOpItem(const ImageHandle& image, const RectI& center, const Rect& dst,
        FilterMode filterMode, const BrushHandle& brushHandle, bool hasBrush);
    ~DrawImageNineOpItem() override = default;

    static std::shared_ptr<OpItem> Unmarshalling(const CmdList& cmdList, void* opItem);
    void Unmarshalling(const CmdList& cmdList, Canvas* canvas = nullptr);

    static void Playback(CanvasPlayer& player, void* opItem);
    void Playback(Canvas& canvas, const CmdList& cmdList);

private:
    ImageHandle image_;
    RectI center_;
    Rect dst_;
    FilterMode filter_;
    BrushHandle brushHandle_;
    bool hasBrush_;
    std::function<void(Canvas&)> playbackTask_ = nullptr;
};

class DrawAnnotationOpItem : public DrawOpItem {
public:
    explicit DrawAnnotationOpItem(const Rect& rect, const char* key, const ImageHandle& data);
    ~DrawAnnotationOpItem() override = default;

    static std::shared_ptr<OpItem> Unmarshalling(const CmdList& cmdList, void* opItem);
    void Unmarshalling(const CmdList& cmdList, Canvas* canvas = nullptr);

    static void Playback(CanvasPlayer& player, void* opItem);
    void Playback(Canvas& canvas, const CmdList& cmdList);

private:
    Rect rect_;
    const char* key_;
    const ImageHandle data_;
    std::function<void(Canvas&)> playbackTask_ = nullptr;
};

class DrawImageLatticeOpItem : public DrawOpItem {
public:
    DrawImageLatticeOpItem();
    explicit DrawImageLatticeOpItem(const ImageHandle& image, const Lattice& lattice, const Rect& dst,
        FilterMode filterMode, const BrushHandle& brushHandle, bool hasBrush);
    ~DrawImageLatticeOpItem() override = default;

    static std::shared_ptr<OpItem> Unmarshalling(const CmdList& cmdList, void* opItem);
    void Unmarshalling(const CmdList& cmdList, Canvas* canvas = nullptr);

    static void Playback(CanvasPlayer& player, void* opItem);
    void Playback(Canvas& canvas, const CmdList& cmdList);

private:
    ImageHandle image_;
    Lattice lattice_;
    Rect dst_;
    FilterMode filter_;
    BrushHandle brushHandle_;
    bool hasBrush_;
    std::function<void(Canvas&)> playbackTask_ = nullptr;
};

class DrawVerticesOpItem : public DrawOpItem {
public:
    DrawVerticesOpItem();
    DrawVerticesOpItem(const VerticesHandle& vertices, BlendMode mode);
    ~DrawVerticesOpItem() override = default;

    static std::shared_ptr<OpItem> Unmarshalling(const CmdList& cmdList, void* opItem);
    void Unmarshalling(const CmdList& cmdList, Canvas* canvas = nullptr);

    static void Playback(CanvasPlayer& player, void* opItem);
    void Playback(Canvas& canvas, const CmdList& cmdList);

private:
    VerticesHandle vertices_;
    BlendMode mode_;
    std::function<void(Canvas&)> playbackTask_ = nullptr;
};

class DrawBitmapOpItem : public DrawOpItem {
public:
    DrawBitmapOpItem();
    DrawBitmapOpItem(const ImageHandle& bitmap, scalar px, scalar py);
    ~DrawBitmapOpItem() override = default;

    static std::shared_ptr<OpItem> Unmarshalling(const CmdList& cmdList, void* opItem);
    void Unmarshalling(const CmdList& cmdList, Canvas* canvas = nullptr);

    static void Playback(CanvasPlayer& player, void* opItem);
    void Playback(Canvas& canvas, const CmdList& cmdList);

private:
    ImageHandle bitmap_;
    scalar px_;
    scalar py_;
    std::function<void(Canvas&)> playbackTask_ = nullptr;
};

class DrawImageOpItem : public DrawOpItem {
public:
    DrawImageOpItem();
    DrawImageOpItem(const ImageHandle& image, scalar px, scalar py, const SamplingOptions& samplingOptions);
    ~DrawImageOpItem() override = default;

    static std::shared_ptr<OpItem> Unmarshalling(const CmdList& cmdList, void* opItem);
    void Unmarshalling(const CmdList& cmdList, Canvas* canvas = nullptr);

    static void Playback(CanvasPlayer& player, void* opItem);
    void Playback(Canvas& canvas, const CmdList& cmdList);

private:
    ImageHandle image_;
    scalar px_;
    scalar py_;
    SamplingOptions samplingOptions_;
    std::function<void(Canvas&)> playbackTask_ = nullptr;
};

class DrawImageRectOpItem : public DrawOpItem {
public:
    DrawImageRectOpItem();
    DrawImageRectOpItem(const ImageHandle& image, const Rect& src, const Rect& dst,
        const SamplingOptions& sampling, SrcRectConstraint constraint);
    ~DrawImageRectOpItem() override = default;

    static std::shared_ptr<OpItem> Unmarshalling(const CmdList& cmdList, void* opItem);
    void Unmarshalling(const CmdList& cmdList, Canvas* canvas = nullptr);

    static void Playback(CanvasPlayer& player, void* opItem);
    void Playback(Canvas& canvas, const CmdList& cmdList);

private:
    ImageHandle image_;
    Rect src_;
    Rect dst_;
    SamplingOptions sampling_;
    SrcRectConstraint constraint_;
    std::function<void(Canvas&)> playbackTask_ = nullptr;
};

class DrawPictureOpItem : public DrawOpItem {
public:
    DrawPictureOpItem();
    explicit DrawPictureOpItem(const ImageHandle& picture);
    ~DrawPictureOpItem() override = default;

    static std::shared_ptr<OpItem> Unmarshalling(const CmdList& cmdList, void* opItem);
    void Unmarshalling(const CmdList& cmdList, Canvas* canvas = nullptr);

    static void Playback(CanvasPlayer& player, void* opItem);
    void Playback(Canvas& canvas, const CmdList& cmdList);

private:
    ImageHandle picture_;
    std::function<void(Canvas&)> playbackTask_ = nullptr;
};

class DrawColorOpItem : public DrawOpItem {
public:
    DrawColorOpItem();
    explicit DrawColorOpItem(ColorQuad color, BlendMode mode);
    ~DrawColorOpItem() override = default;

    static std::shared_ptr<OpItem> Unmarshalling(const CmdList& cmdList, void* opItem);
    static void Playback(CanvasPlayer& player, void* opItem);
    void Playback(Canvas& canvas);

private:
    ColorQuad color_;
    BlendMode mode_;
};

class AttachPenOpItem;
class AttachBrushOpItem;
class DrawTextBlobOpItem : public DrawOpItem {
public:
    DrawTextBlobOpItem();
    explicit DrawTextBlobOpItem(const ImageHandle& textBlob, const scalar x, const scalar y);
    ~DrawTextBlobOpItem() override = default;

    static std::shared_ptr<OpItem> Unmarshalling(const CmdList& cmdList, void* opItem);
    void Unmarshalling(const CmdList& cmdList, Canvas* canvas = nullptr);

    static void Playback(CanvasPlayer& player, void* opItem);
    void Playback(Canvas& canvas, const CmdList& cmdList);

    std::shared_ptr<NoIPCImageOpItem> GenerateCachedOpItem(CmdList& cmdList, Canvas* canvas,
        AttachPenOpItem* penOpItem, AttachBrushOpItem* brushOpItem, bool addCmdList, bool& replaceSuccess);
    
    bool GenerateCachedOpItem(std::shared_ptr<CmdList> cacheCmdList, const TextBlob* textBlob);

private:
    ImageHandle textBlob_;
    scalar x_;
    scalar y_;
    std::function<void(Canvas&)> playbackTask_ = nullptr;
};

class ClipRectOpItem : public DrawOpItem {
public:
    ClipRectOpItem();
    ClipRectOpItem(const Rect& rect, ClipOp op, bool doAntiAlias);
    ~ClipRectOpItem() override = default;

    static std::shared_ptr<OpItem> Unmarshalling(const CmdList& cmdList, void* opItem);
    static void Playback(CanvasPlayer& player, void* opItem);
    void Playback(Canvas& canvas);

private:
    Rect rect_;
    ClipOp clipOp_;
    bool doAntiAlias_;
};

class ClipIRectOpItem : public DrawOpItem {
public:
    ClipIRectOpItem();
    ClipIRectOpItem(const RectI& rect, ClipOp op = ClipOp::INTERSECT);
    ~ClipIRectOpItem() override = default;

    static std::shared_ptr<OpItem> Unmarshalling(const CmdList& cmdList, void* opItem);
    static void Playback(CanvasPlayer& player, void* opItem);
    void Playback(Canvas& canvas);

private:
    RectI rect_;
    ClipOp clipOp_;
};

class ClipRoundRectOpItem : public DrawOpItem {
public:
    ClipRoundRectOpItem();
    ClipRoundRectOpItem(const std::pair<uint32_t, size_t> radiusXYData, const Rect& rect, ClipOp op, bool doAntiAlias);
    ~ClipRoundRectOpItem() override = default;

    static std::shared_ptr<OpItem> Unmarshalling(const CmdList& cmdList, void* opItem);
    void Unmarshalling(const CmdList& cmdList, Canvas* canvas = nullptr);

    static void Playback(CanvasPlayer& player, void* opItem);
    void Playback(Canvas& canvas, const CmdList& cmdList);

private:
    std::pair<uint32_t, size_t> radiusXYData_;
    Rect rect_;
    ClipOp clipOp_;
    bool doAntiAlias_;
    std::function<void(Canvas&)> playbackTask_ = nullptr;
};

class ClipPathOpItem : public DrawOpItem {
public:
    ClipPathOpItem();
    ClipPathOpItem(const ImageHandle& path, ClipOp clipOp, bool doAntiAlias);
    ~ClipPathOpItem() override = default;

    static std::shared_ptr<OpItem> Unmarshalling(const CmdList& cmdList, void* opItem);
    void Unmarshalling(const CmdList& cmdList, Canvas* canvas = nullptr);

    static void Playback(CanvasPlayer& player, void* opItem);
    void Playback(Canvas& canvas, const CmdList& cmdList);

private:
    ImageHandle path_;
    ClipOp clipOp_;
    bool doAntiAlias_;
    std::function<void(Canvas&)> playbackTask_ = nullptr;
};

class ClipRegionOpItem : public DrawOpItem {
public:
    ClipRegionOpItem();
    ClipRegionOpItem(const CmdListHandle& region, ClipOp clipOp = ClipOp::INTERSECT);
    ~ClipRegionOpItem() override = default;

    static std::shared_ptr<OpItem> Unmarshalling(const CmdList& cmdList, void* opItem);
    void Unmarshalling(const CmdList& cmdList, Canvas* canvas = nullptr);

    static void Playback(CanvasPlayer& player, void* opItem);
    void Playback(Canvas& canvas, const CmdList& cmdList);

private:
    CmdListHandle region_;
    ClipOp clipOp_;
    std::function<void(Canvas&)> playbackTask_ = nullptr;
};

class SetMatrixOpItem : public DrawOpItem {
public:
    SetMatrixOpItem();
    explicit SetMatrixOpItem(const Matrix& matrix);
    ~SetMatrixOpItem() override = default;

    static std::shared_ptr<OpItem> Unmarshalling(const CmdList& cmdList, void* opItem);
    static void Playback(CanvasPlayer& player, void* opItem);
    void Playback(Canvas& canvas);

private:
    Matrix::Buffer matrixBuffer_;
};

class ResetMatrixOpItem : public DrawOpItem {
public:
    ResetMatrixOpItem();
    ~ResetMatrixOpItem() override = default;

    static std::shared_ptr<OpItem> Unmarshalling(const CmdList& cmdList, void* opItem);
    static void Playback(CanvasPlayer& player, void* opItem);
    void Playback(Canvas& canvas);
};

class ConcatMatrixOpItem : public DrawOpItem {
public:
    ConcatMatrixOpItem();
    explicit ConcatMatrixOpItem(const Matrix& matrix);
    ~ConcatMatrixOpItem() override = default;

    static std::shared_ptr<OpItem> Unmarshalling(const CmdList& cmdList, void* opItem);
    static void Playback(CanvasPlayer& player, void* opItem);
    void Playback(Canvas& canvas);

private:
    Matrix::Buffer matrixBuffer_;
};

class TranslateOpItem : public DrawOpItem {
public:
    TranslateOpItem();
    TranslateOpItem(scalar dx, scalar dy);
    ~TranslateOpItem() override = default;

    static std::shared_ptr<OpItem> Unmarshalling(const CmdList& cmdList, void* opItem);
    static void Playback(CanvasPlayer& player, void* opItem);
    void Playback(Canvas& canvas);

private:
    scalar dx_;
    scalar dy_;
};

class ScaleOpItem : public DrawOpItem {
public:
    ScaleOpItem();
    ScaleOpItem(scalar sx, scalar sy);
    ~ScaleOpItem() override = default;

    static std::shared_ptr<OpItem> Unmarshalling(const CmdList& cmdList, void* opItem);
    static void Playback(CanvasPlayer& player, void* opItem);
    void Playback(Canvas& canvas);

private:
    scalar sx_;
    scalar sy_;
};

class RotateOpItem : public DrawOpItem {
public:
    RotateOpItem();
    RotateOpItem(scalar deg, scalar sx, scalar sy);
    ~RotateOpItem() override = default;

    static std::shared_ptr<OpItem> Unmarshalling(const CmdList& cmdList, void* opItem);
    static void Playback(CanvasPlayer& player, void* opItem);
    void Playback(Canvas& canvas);

private:
    scalar deg_;
    scalar sx_;
    scalar sy_;
};

class ShearOpItem : public DrawOpItem {
public:
    ShearOpItem();
    ShearOpItem(scalar sx, scalar sy);
    ~ShearOpItem() override = default;

    static std::shared_ptr<OpItem> Unmarshalling(const CmdList& cmdList, void* opItem);
    static void Playback(CanvasPlayer& player, void* opItem);
    void Playback(Canvas& canvas);

private:
    scalar sx_;
    scalar sy_;
};

class FlushOpItem : public DrawOpItem {
public:
    FlushOpItem();
    ~FlushOpItem() override = default;

    static std::shared_ptr<OpItem> Unmarshalling(const CmdList& cmdList, void* opItem);
    static void Playback(CanvasPlayer& player, void* opItem);
    void Playback(Canvas& canvas);
};

class ClearOpItem : public DrawOpItem {
public:
    ClearOpItem();
    explicit ClearOpItem(ColorQuad color);
    ~ClearOpItem() override = default;

    static std::shared_ptr<OpItem> Unmarshalling(const CmdList& cmdList, void* opItem);
    static void Playback(CanvasPlayer& player, void* opItem);
    void Playback(Canvas& canvas);

private:
    ColorQuad color_;
};

class SaveOpItem : public DrawOpItem {
public:
    SaveOpItem();
    ~SaveOpItem() override = default;

    static std::shared_ptr<OpItem> Unmarshalling(const CmdList& cmdList, void* opItem);
    static void Playback(CanvasPlayer& player, void* opItem);
    void Playback(Canvas& canvas);
};

class SaveLayerOpItem : public DrawOpItem {
public:
    SaveLayerOpItem();
    SaveLayerOpItem(const Rect& rect, bool hasBrush, const BrushHandle& brushHandle, const CmdListHandle& imageFilter,
        uint32_t saveLayerFlags);
    ~SaveLayerOpItem() override = default;

    static std::shared_ptr<OpItem> Unmarshalling(const CmdList& cmdList, void* opItem);
    void Unmarshalling(const CmdList& cmdList, Canvas* canvas = nullptr);

    static void Playback(CanvasPlayer& player, void* opItem);
    void Playback(Canvas& canvas, const CmdList& cmdList);

private:
    Rect rect_;
    bool hasBrush_;
    BrushHandle brushHandle_;
    CmdListHandle imageFilter_;
    uint32_t saveLayerFlags_;
    std::function<void(Canvas&)> playbackTask_ = nullptr;
};

class RestoreOpItem : public DrawOpItem {
public:
    RestoreOpItem();
    ~RestoreOpItem() override = default;

    static std::shared_ptr<OpItem> Unmarshalling(const CmdList& cmdList, void* opItem);
    static void Playback(CanvasPlayer& player, void* opItem);
    void Playback(Canvas& canvas);
};

class DiscardOpItem : public DrawOpItem {
public:
    DiscardOpItem();
    ~DiscardOpItem() override = default;

    static std::shared_ptr<OpItem> Unmarshalling(const CmdList& cmdList, void* opItem);
    static void Playback(CanvasPlayer& player, void* opItem);
    void Playback(Canvas& canvas);
};

class AttachPenOpItem : public DrawOpItem {
public:
    AttachPenOpItem();
    AttachPenOpItem(const PenHandle& penHandle);
    ~AttachPenOpItem() override = default;

    static std::shared_ptr<OpItem> Unmarshalling(const CmdList& cmdList, void* opItem);
    void Unmarshalling(const CmdList& cmdList, Canvas* canvas = nullptr);

    static void Playback(CanvasPlayer& player, void* opItem);
    void Playback(Canvas& canvas, const CmdList& cmdList);

private:
    PenHandle penHandle_;
    std::function<void(Canvas&)> playbackTask_ = nullptr;
};

class AttachBrushOpItem : public DrawOpItem {
public:
    AttachBrushOpItem();
    AttachBrushOpItem(const BrushHandle& brushHandle);
    ~AttachBrushOpItem() override = default;

    static std::shared_ptr<OpItem> Unmarshalling(const CmdList& cmdList, void* opItem);
    void Unmarshalling(const CmdList& cmdList, Canvas* canvas = nullptr);

    static void Playback(CanvasPlayer& player, void* opItem);
    void Playback(Canvas& canvas, const CmdList& cmdList);

private:
    BrushHandle brushHandle_;
    std::function<void(Canvas&)> playbackTask_ = nullptr;
};

class DetachPenOpItem : public DrawOpItem {
public:
    DetachPenOpItem();
    ~DetachPenOpItem() override = default;

    static std::shared_ptr<OpItem> Unmarshalling(const CmdList& cmdList, void* opItem);
    static void Playback(CanvasPlayer& player, void* opItem);
    void Playback(Canvas& canvas);
};

class DetachBrushOpItem : public DrawOpItem {
public:
    DetachBrushOpItem();
    ~DetachBrushOpItem() override = default;

    static std::shared_ptr<OpItem> Unmarshalling(const CmdList& cmdList, void* opItem);
    static void Playback(CanvasPlayer& player, void* opItem);
    void Playback(Canvas& canvas);
};

class ClipAdaptiveRoundRectOpItem : public DrawOpItem {
public:
    ClipAdaptiveRoundRectOpItem();
    ClipAdaptiveRoundRectOpItem(const std::pair<uint32_t, size_t>& radiusData);
    ~ClipAdaptiveRoundRectOpItem() override = default;

    static std::shared_ptr<OpItem> Unmarshalling(const CmdList& cmdList, void* opItem);
    void Unmarshalling(const CmdList& cmdList);

    static void Playback(CanvasPlayer& player, void* opItem);
    void Playback(Canvas& canvas, const CmdList& cmdList, const Rect& rect);

private:
    std::pair<uint32_t, size_t> radiusData_;
    std::function<void(Canvas&, const Rect&)> playbackTask_ = nullptr;
};

class DrawAdaptiveImageOpItem : public DrawOpItem {
public:
    DrawAdaptiveImageOpItem();
    DrawAdaptiveImageOpItem(const ImageHandle& image, const AdaptiveImageInfo& rsImageInfo,
        const SamplingOptions& smapling, const bool isImage);
    ~DrawAdaptiveImageOpItem() override = default;

    static std::shared_ptr<OpItem> Unmarshalling(const CmdList& cmdList, void* opItem);
    void Unmarshalling(const CmdList& cmdList);

    static void Playback(CanvasPlayer& player, void* opItem);
    void Playback(Canvas& canvas, const CmdList& cmdList, const Rect& rect);

private:
    ImageHandle image_;
    AdaptiveImageInfo rsImageInfo_;
    SamplingOptions sampling_;
    bool isImage_;
    std::function<void(Canvas&, const Rect&)> playbackTask_ = nullptr;
};

class DrawAdaptivePixelMapOpItem : public DrawOpItem {
public:
    DrawAdaptivePixelMapOpItem();
    DrawAdaptivePixelMapOpItem(const ImageHandle& pixelMap, const AdaptiveImageInfo& imageInfo,
        const SamplingOptions& smapling);
    ~DrawAdaptivePixelMapOpItem() override = default;

    static std::shared_ptr<OpItem> Unmarshalling(const CmdList& cmdList, void* opItem);
    void Unmarshalling(const CmdList& cmdList);

    static void Playback(CanvasPlayer& player, void* opItem);
    void Playback(Canvas& canvas, const CmdList& cmdList, const Rect& rect);

private:
    ImageHandle pixelMap_;
    AdaptiveImageInfo imageInfo_;
    SamplingOptions smapling_;
    std::function<void(Canvas&, const Rect&)> playbackTask_ = nullptr;
};

class DrawExtendPixelMapOpItem : public DrawOpItem {
public:
    DrawExtendPixelMapOpItem();
    DrawExtendPixelMapOpItem(const ImageHandle& objectHandle, const SamplingOptions& sampling);
    ~DrawExtendPixelMapOpItem() override = default;

    static std::shared_ptr<OpItem> Unmarshalling(const CmdList& cmdList, void* opItem);
    void Unmarshalling(const CmdList& cmdList);

    static void Playback(CanvasPlayer& player, void* opItem);
    void Playback(Canvas& canvas, const CmdList& cmdList, const Rect& rect) const;

private:
    ImageHandle objectHandle_;
    SamplingOptions sampling_;
    std::function<void(Canvas&, const Rect&)> playbackTask_ = nullptr;
};

class DrawImageWithParmOpItem : public DrawOpItem {
public:
    DrawImageWithParmOpItem();
    DrawImageWithParmOpItem(const ImageHandle& objectHandle, const SamplingOptions& sampling);
    ~DrawImageWithParmOpItem() override = default;

    static std::shared_ptr<OpItem> Unmarshalling(const CmdList& cmdList, void* opItem);
    void Unmarshalling(const CmdList& cmdList);

    static void Playback(CanvasPlayer& player, void* opItem);
    void Playback(Canvas& canvas, const CmdList& cmdList, const Rect& rect) const;

private:
    ImageHandle objectHandle_;
    SamplingOptions sampling_;
    std::function<void(Canvas&, const Rect&)> playbackTask_ = nullptr;
};

class NoIPCImageOpItem : public DrawOpItem {
public:
    NoIPCImageOpItem(std::shared_ptr<Image> image, const Rect& src, const Rect& dst,
        const SamplingOptions& sampling, SrcRectConstraint constraint);
    ~NoIPCImageOpItem() = default;

    static void Playback(CanvasPlayer& player, void* opItem);
    void Playback(Canvas& canvas, const CmdList& cmdList);

private:
    std::shared_ptr<Image> image_;
    Rect src_;
    Rect dst_;
    SamplingOptions sampling_;
    SrcRectConstraint constraint_;
};

} // namespace Drawing
} // namespace Rosen
} // namespace OHOS
#endif
