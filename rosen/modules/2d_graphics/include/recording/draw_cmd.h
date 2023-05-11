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
#include "recording/op_item.h"
#include "recording/mem_allocator.h"

namespace OHOS {
namespace Rosen {
namespace Drawing {

struct BrushSiteInfo {
    CmdListSiteInfo colorSpaceCmdListInfo;
    LargeObjectInfo colorSpaceLargeObjectInfo;
    CmdListSiteInfo shaderEffectCmdListInfo;
    LargeObjectInfo shaderEffectLargeObjectInfo;
    CmdListSiteInfo colorFilterCmdListInfo;
    CmdListSiteInfo imageFilterCmdListInfo;
    CmdListSiteInfo maskFilterCmdListInfo;
};

struct PenSiteInfo {
    CmdListSiteInfo pathEffectCmdListInfo;
    CmdListSiteInfo colorSpaceCmdListInfo;
    LargeObjectInfo colorSpaceLargeObjectInfo;
    CmdListSiteInfo shaderEffectCmdListInfo;
    LargeObjectInfo shaderEffectLargeObjectInfo;
    CmdListSiteInfo colorFilterCmdListInfo;
    CmdListSiteInfo imageFilterCmdListInfo;
    CmdListSiteInfo maskFilterCmdListInfo;
};

class CanvasPlayer {
public:
    CanvasPlayer(Canvas& canvas, const MemAllocator& opAllocator, const MemAllocator& largeObjectAllocator);
    ~CanvasPlayer() = default;

    bool Playback(uint32_t type, const void* opItem);

    Canvas& canvas_;
    const MemAllocator& opAllocator_;
    const MemAllocator& largeObjectAllocator_;

    using PlaybackFunc = void(*)(CanvasPlayer& palyer, const void* opItem);
private:
    static std::unordered_map<uint32_t, PlaybackFunc> opPlaybackFuncLUT_;
};

class DrawOpItem : public OpItem {
public:
    DrawOpItem(uint32_t type) : OpItem(type) {}
    ~DrawOpItem() = default;

    enum Type : uint32_t {
        OPITEM_HEAD,
        POINT_OPITEM,
        LINE_OPITEM,
        RECT_OPITEM,
        ROUND_RECT_OPITEM,
        NESTED_ROUND_RECT_OPITEM,
        ARC_OPITEM,
        PIE_OPITEM,
        OVAL_OPITEM,
        CIRCLE_OPITEM,
        PATH_OPITEM,
        BACKGROUND_OPITEM,
        SHADOW_OPITEM,
        BITMAP_OPITEM,
        IMAGE_OPITEM,
        IMAGE_RECT_OPITEM,
        PICTURE_OPITEM,
        CLIP_RECT_OPITEM,
        CLIP_ROUND_RECT_OPITEM,
        CLIP_PATH_OPITEM,
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
        ATTACH_PEN_OPITEM,
        ATTACH_BRUSH_OPITEM,
        DETACH_PEN_OPITEM,
        DETACH_BRUSH_OPITEM,
    };
};

class DrawPointOpItem : public DrawOpItem {
public:
    DrawPointOpItem(const Point& point);
    ~DrawPointOpItem() = default;

    static void Playback(CanvasPlayer& player, const void* opItem);
    void Playback(Canvas& canvas) const;

private:
    Point point_;
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
    DrawRectOpItem(const Rect& rect);
    ~DrawRectOpItem() = default;

    static void Playback(CanvasPlayer& player, const void* opItem);
    void Playback(Canvas& canvas) const;

private:
    Rect rect_;
};

class DrawRoundRectOpItem : public DrawOpItem {
public:
    DrawRoundRectOpItem(const RoundRect& rrect);
    ~DrawRoundRectOpItem()  = default;

    static void Playback(CanvasPlayer& player, const void* opItem);
    void Playback(Canvas& canvas) const;

private:
    RoundRect rrect_;
};

class DrawNestedRoundRectOpItem : public DrawOpItem {
public:
    DrawNestedRoundRectOpItem(const RoundRect& outer, const RoundRect& inner);
    ~DrawNestedRoundRectOpItem() = default;

    static void Playback(CanvasPlayer& player, const void* opItem);
    void Playback(Canvas& canvas) const;

private:
    RoundRect outer_;
    RoundRect inner_;
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
    DrawOvalOpItem(const Rect& rect);
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
    DrawPathOpItem(const CmdListSiteInfo info);
    ~DrawPathOpItem() = default;

    static void Playback(CanvasPlayer& player, const void* opItem);
    void Playback(Canvas& canvas, const MemAllocator& memAllocator) const;

private:
    CmdListSiteInfo info_;
};

class DrawBackgroundOpItem : public DrawOpItem {
public:
    DrawBackgroundOpItem(const Color& color, const BlendMode mode, const bool isAntiAlias,
        const Filter::FilterQuality filterQuality, const BrushHandle brushHandle);
    ~DrawBackgroundOpItem() = default;

    static void Playback(CanvasPlayer& player, const void* opItem);
    void Playback(Canvas& canvas, const CmdList& cmdList) const;

private:
    Color color_;
    BlendMode mode_;
    bool isAntiAlias_;
    Filter::FilterQuality filterQuality_;
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

class DrawBitmapOpItem : public DrawOpItem {
public:
    DrawBitmapOpItem(ImageHandle bitmap, const scalar px, const scalar py);
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
    DrawImageOpItem(ImageHandle image, const scalar px, const scalar py, const SamplingOptions& samplingOptions);
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
    DrawImageRectOpItem(ImageHandle image, const Rect& src, const Rect& dst,
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
    DrawPictureOpItem(ImageHandle picture);
    ~DrawPictureOpItem() = default;

    static void Playback(CanvasPlayer& player, const void* opItem);
    void Playback(Canvas& canvas, const CmdList& cmdList) const;

private:
    ImageHandle picture_;
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

class ClipRoundRectOpItem : public DrawOpItem {
public:
    ClipRoundRectOpItem(const RoundRect& roundRect, ClipOp op, bool doAntiAlias);
    ~ClipRoundRectOpItem() = default;

    static void Playback(CanvasPlayer& player, const void* opItem);
    void Playback(Canvas& canvas) const;

private:
    RoundRect roundRect_;
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

class SetMatrixOpItem : public DrawOpItem {
public:
    SetMatrixOpItem(const Matrix& matrix);
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
    ConcatMatrixOpItem(const Matrix& matrix);
    ~ConcatMatrixOpItem() = default;

    static void Playback(CanvasPlayer& player, const void* opItem);
    void Playback(Canvas& canvas) const;

private:
    Matrix::Buffer matrixBuffer_;
};
} // namespace Drawing
} // namespace Rosen
} // namespace OHOS

#endif // DRAW_CMD_H
