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

#include "recording/draw_cmd.h"

#include "recording/draw_cmd_list.h"
#include "recording/mem_allocator.h"
#include "recording/op_item.h"
#include "recording/path_cmd_list.h"
#include "recording/color_filter_cmd_list.h"
#include "recording/color_space_cmd_list.h"
#include "recording/image_filter_cmd_list.h"
#include "recording/mask_filter_cmd_list.h"
#include "recording/path_effect_cmd_list.h"
#include "recording/shader_effect_cmd_list.h"

#include "draw/path.h"
#include "draw/brush.h"
#include "effect/color_filter.h"
#include "effect/color_space.h"
#include "effect/image_filter.h"
#include "effect/mask_filter.h"
#include "effect/path_effect.h"
#include "effect/shader_effect.h"
#include "utils/scalar.h"

namespace OHOS {
namespace Rosen {
namespace Drawing {
std::unordered_map<uint32_t, CanvasPlayer::PlaybackFunc> CanvasPlayer::opPlaybackFuncLUT_ = {
    { DrawOpItem::POINT_OPITEM,             DrawPointOpItem::Playback },
    { DrawOpItem::LINE_OPITEM,              DrawLineOpItem::Playback },
    { DrawOpItem::RECT_OPITEM,              DrawRectOpItem::Playback },
    { DrawOpItem::ROUND_RECT_OPITEM,        DrawRoundRectOpItem::Playback },
    { DrawOpItem::NESTED_ROUND_RECT_OPITEM, DrawNestedRoundRectOpItem::Playback },
    { DrawOpItem::ARC_OPITEM,               DrawArcOpItem::Playback },
    { DrawOpItem::PIE_OPITEM,               DrawPieOpItem::Playback },
    { DrawOpItem::OVAL_OPITEM,              DrawOvalOpItem::Playback },
    { DrawOpItem::CIRCLE_OPITEM,            DrawCircleOpItem::Playback },
    { DrawOpItem::PATH_OPITEM,              DrawPathOpItem::Playback },
    { DrawOpItem::BACKGROUND_OPITEM,        DrawBackgroundOpItem::Playback },
    { DrawOpItem::SHADOW_OPITEM,            DrawShadowOpItem::Playback },
    { DrawOpItem::BITMAP_OPITEM,            DrawBitmapOpItem::Playback },
    { DrawOpItem::IMAGE_OPITEM,             DrawImageOpItem::Playback },
    { DrawOpItem::IMAGE_RECT_OPITEM,        DrawImageRectOpItem::Playback },
    { DrawOpItem::PICTURE_OPITEM,           DrawPictureOpItem::Playback },
    { DrawOpItem::CLIP_RECT_OPITEM,         ClipRectOpItem::Playback },
    { DrawOpItem::CLIP_ROUND_RECT_OPITEM,   ClipRoundRectOpItem::Playback },
    { DrawOpItem::CLIP_PATH_OPITEM,         ClipPathOpItem::Playback },
    { DrawOpItem::SET_MATRIX_OPITEM,        SetMatrixOpItem::Playback },
    { DrawOpItem::RESET_MATRIX_OPITEM,      ResetMatrixOpItem::Playback },
    { DrawOpItem::CONCAT_MATRIX_OPITEM,     ConcatMatrixOpItem::Playback },
    { DrawOpItem::TRANSLATE_OPITEM,         TranslateOpItem::Playback },
    { DrawOpItem::SCALE_OPITEM,             ScaleOpItem::Playback },
    { DrawOpItem::ROTATE_OPITEM,            RotateOpItem::Playback },
    { DrawOpItem::SHEAR_OPITEM,             ShearOpItem::Playback },
    { DrawOpItem::FLUSH_OPITEM,             FlushOpItem::Playback },
    { DrawOpItem::CLEAR_OPITEM,             ClearOpItem::Playback },
    { DrawOpItem::SAVE_OPITEM,              SaveOpItem::Playback },
    { DrawOpItem::SAVE_LAYER_OPITEM,        SaveLayerOpItem::Playback },
    { DrawOpItem::RESTORE_OPITEM,           RestoreOpItem::Playback },
    { DrawOpItem::ATTACH_PEN_OPITEM,        AttachPenOpItem::Playback },
    { DrawOpItem::ATTACH_BRUSH_OPITEM,      AttachBrushOpItem::Playback },
    { DrawOpItem::DETACH_PEN_OPITEM,        DetachPenOpItem::Playback },
    { DrawOpItem::DETACH_BRUSH_OPITEM,      DetachBrushOpItem::Playback },
};

CanvasPlayer::CanvasPlayer(Canvas& canvas, const MemAllocator& opAllocator, const MemAllocator& largeObjectAllocator)
    : canvas_(canvas), opAllocator_(opAllocator), largeObjectAllocator_(largeObjectAllocator) {}

bool CanvasPlayer::Playback(uint32_t type, const void* opItem)
{
    auto it = opPlaybackFuncLUT_.find(type);
    if (it == opPlaybackFuncLUT_.end() || it->second == nullptr) {
        return false;
    }

    auto func = it->second;
    (*func)(*this, opItem);

    return true;
}

/* OpItem */
DrawPointOpItem::DrawPointOpItem(const Point& point) : DrawOpItem(POINT_OPITEM), point_(point) {}

void DrawPointOpItem::Playback(CanvasPlayer& player, const void *opItem)
{
    if (opItem != nullptr) {
        const auto *op = static_cast<const DrawPointOpItem*>(opItem);
        op->Playback(player.canvas_);
    }
}

void DrawPointOpItem::Playback(Canvas& canvas) const
{
    canvas.DrawPoint(point_);
}

DrawLineOpItem::DrawLineOpItem(const Point& startPt, const Point& endPt) : DrawOpItem(LINE_OPITEM),
    startPt_(startPt), endPt_(endPt) {}

void DrawLineOpItem::Playback(CanvasPlayer& player, const void* opItem)
{
    if (opItem != nullptr) {
        const auto *op = static_cast<const DrawLineOpItem*>(opItem);
        op->Playback(player.canvas_);
    }
}

void DrawLineOpItem::Playback(Canvas& canvas) const
{
    canvas.DrawLine(startPt_, endPt_);
}

DrawRectOpItem::DrawRectOpItem(const Rect& rect) : DrawOpItem(RECT_OPITEM), rect_(rect) {}

void DrawRectOpItem::Playback(CanvasPlayer& player, const void* opItem)
{
    if (opItem != nullptr) {
        const auto *op = static_cast<const DrawRectOpItem*>(opItem);
        op->Playback(player.canvas_);
    }
}

void DrawRectOpItem::Playback(Canvas& canvas) const
{
    canvas.DrawRect(rect_);
}

DrawRoundRectOpItem::DrawRoundRectOpItem(const RoundRect& rrect) : DrawOpItem(ROUND_RECT_OPITEM), rrect_(rrect) {}

void DrawRoundRectOpItem::Playback(CanvasPlayer& player, const void* opItem)
{
    if (opItem != nullptr) {
        const auto *op = static_cast<const DrawRoundRectOpItem*>(opItem);
        op->Playback(player.canvas_);
    }
}

void DrawRoundRectOpItem::Playback(Canvas& canvas) const
{
    canvas.DrawRoundRect(rrect_);
}

DrawNestedRoundRectOpItem::DrawNestedRoundRectOpItem(const RoundRect& outer, const RoundRect& inner)
    : DrawOpItem(NESTED_ROUND_RECT_OPITEM), outer_(outer), inner_(inner) {}

void DrawNestedRoundRectOpItem::Playback(CanvasPlayer& player, const void* opItem)
{
    if (opItem != nullptr) {
        const auto *op = static_cast<const DrawNestedRoundRectOpItem*>(opItem);
        op->Playback(player.canvas_);
    }
}

void DrawNestedRoundRectOpItem::Playback(Canvas& canvas) const
{
    canvas.DrawNestedRoundRect(outer_, inner_);
}

DrawArcOpItem::DrawArcOpItem(const Rect& rect, scalar startAngle, scalar sweepAngle)
    : DrawOpItem(ARC_OPITEM), rect_(rect), startAngle_(startAngle), sweepAngle_(sweepAngle) {}

void DrawArcOpItem::Playback(CanvasPlayer& player, const void* opItem)
{
    if (opItem != nullptr) {
        const auto *op = static_cast<const DrawArcOpItem*>(opItem);
        op->Playback(player.canvas_);
    }
}

void DrawArcOpItem::Playback(Canvas& canvas) const
{
    canvas.DrawArc(rect_, startAngle_, sweepAngle_);
}

DrawPieOpItem::DrawPieOpItem(const Rect& rect, scalar startAngle, scalar sweepAngle)
    : DrawOpItem(PIE_OPITEM), rect_(rect), startAngle_(startAngle), sweepAngle_(sweepAngle) {}

void DrawPieOpItem::Playback(CanvasPlayer& player, const void* opItem)
{
    if (opItem != nullptr) {
        const auto *op = static_cast<const DrawPieOpItem*>(opItem);
        op->Playback(player.canvas_);
    }
}

void DrawPieOpItem::Playback(Canvas& canvas) const
{
    canvas.DrawPie(rect_, startAngle_, sweepAngle_);
}

DrawOvalOpItem::DrawOvalOpItem(const Rect& rect) : DrawOpItem(OVAL_OPITEM), rect_(rect) {}

void DrawOvalOpItem::Playback(CanvasPlayer& player, const void* opItem)
{
    if (opItem != nullptr) {
        const auto *op = static_cast<const DrawOvalOpItem*>(opItem);
        op->Playback(player.canvas_);
    }
}

void DrawOvalOpItem::Playback(Canvas& canvas) const
{
    canvas.DrawOval(rect_);
}

DrawCircleOpItem::DrawCircleOpItem(const Point& centerPt, scalar radius)
    : DrawOpItem(CIRCLE_OPITEM), centerPt_(centerPt), radius_(radius) {}

void DrawCircleOpItem::Playback(CanvasPlayer& player, const void* opItem)
{
    if (opItem != nullptr) {
        const auto *op = static_cast<const DrawCircleOpItem*>(opItem);
        op->Playback(player.canvas_);
    }
}

void DrawCircleOpItem::Playback(Canvas& canvas) const
{
    canvas.DrawCircle(centerPt_, radius_);
}

DrawPathOpItem::DrawPathOpItem(const CmdListSiteInfo info) : DrawOpItem(PATH_OPITEM), info_(info) {}

void DrawPathOpItem::Playback(CanvasPlayer& player, const void* opItem)
{
    if (opItem != nullptr) {
        const auto *op = static_cast<const DrawPathOpItem*>(opItem);
        op->Playback(player.canvas_, player.opAllocator_);
    }
}

void DrawPathOpItem::Playback(Canvas& canvas, const MemAllocator& memAllocator) const
{
    Path path;
    auto pathCmdList = std::make_shared<PathCmdList>(memAllocator.OffsetToAddr(info_.first), info_.second);
    if (pathCmdList) {
        pathCmdList->Playback(path);
    }
    canvas.DrawPath(path);
}

} // namespace Drawing
} // namespace Rosen
} // namespace OHOS
