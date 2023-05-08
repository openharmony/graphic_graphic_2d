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

} // namespace Drawing
} // namespace Rosen
} // namespace OHOS

#endif // DRAW_CMD_H
