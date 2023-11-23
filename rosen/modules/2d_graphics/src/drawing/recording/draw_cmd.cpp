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

#include "recording/cmd_list_helper.h"
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
#include "recording/region_cmd_list.h"

#include "draw/path.h"
#include "draw/brush.h"
#include "effect/color_filter.h"
#include "effect/color_space.h"
#include "effect/image_filter.h"
#include "effect/mask_filter.h"
#include "effect/path_effect.h"
#include "effect/shader_effect.h"
#include "utils/scalar.h"
#include "utils/log.h"
#include "draw/surface.h"

#define CHECK_AND_RETURN_RET_LOG(cond, ret, fmt, ...)  \
    do {                                               \
        if (!(cond)) {                                 \
            LOGE(fmt, ##__VA_ARGS__);                  \
            return ret;                                \
        }                                              \
    } while (0)

constexpr uint32_t ADDRSIZE = sizeof(uint32_t *);
#define COPY_OPITEN_AND_RETURN_RET_LOG(T, dest, from, ret, fmt, ...)  \
    do {                                                              \
        uint32_t size_ = sizeof(T) - ADDRSIZE;                        \
        if (memcpy_s(static_cast<char*>(static_cast<void*>(dest)) + ADDRSIZE, size_, \
                     static_cast<char*>(from) + ADDRSIZE, size_) != EOK) { \
            LOGE(fmt, ##__VA_ARGS__);                                 \
            return ret;                                               \
        }                                                             \
    } while (0)

namespace OHOS {
namespace Rosen {
namespace Drawing {
std::unordered_map<uint32_t, CanvasPlayer::PlaybackFunc> CanvasPlayer::opPlaybackFuncLUT_ = {
    { DrawOpItem::POINT_OPITEM,             DrawPointOpItem::Playback },
    { DrawOpItem::POINTS_OPITEM,            DrawPointsOpItem::Playback },
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
    { DrawOpItem::COLOR_OPITEM,             DrawColorOpItem::Playback },
    { DrawOpItem::IMAGE_NINE_OPITEM,        DrawImageNineOpItem::Playback },
    { DrawOpItem::IMAGE_ANNOTATION_OPITEM,  DrawAnnotationOpItem::Playback },
    { DrawOpItem::IMAGE_LATTICE_OPITEM,     DrawImageLatticeOpItem::Playback },
    { DrawOpItem::BITMAP_OPITEM,            DrawBitmapOpItem::Playback },
    { DrawOpItem::IMAGE_OPITEM,             DrawImageOpItem::Playback },
    { DrawOpItem::IMAGE_RECT_OPITEM,        DrawImageRectOpItem::Playback },
    { DrawOpItem::PICTURE_OPITEM,           DrawPictureOpItem::Playback },
    { DrawOpItem::TEXT_BLOB_OPITEM,         DrawTextBlobOpItem::Playback },
    { DrawOpItem::CLIP_RECT_OPITEM,         ClipRectOpItem::Playback },
    { DrawOpItem::CLIP_IRECT_OPITEM,        ClipIRectOpItem::Playback },
    { DrawOpItem::CLIP_ROUND_RECT_OPITEM,   ClipRoundRectOpItem::Playback },
    { DrawOpItem::CLIP_PATH_OPITEM,         ClipPathOpItem::Playback },
    { DrawOpItem::CLIP_REGION_OPITEM,       ClipRegionOpItem::Playback },
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
    { DrawOpItem::DISCARD_OPITEM,           DiscardOpItem::Playback },
    { DrawOpItem::ATTACH_PEN_OPITEM,        AttachPenOpItem::Playback },
    { DrawOpItem::ATTACH_BRUSH_OPITEM,      AttachBrushOpItem::Playback },
    { DrawOpItem::DETACH_PEN_OPITEM,        DetachPenOpItem::Playback },
    { DrawOpItem::DETACH_BRUSH_OPITEM,      DetachBrushOpItem::Playback },
    { DrawOpItem::CLIP_ADAPTIVE_ROUND_RECT_OPITEM, ClipAdaptiveRoundRectOpItem::Playback},
    { DrawOpItem::ADAPTIVE_IMAGE_OPITEM,    DrawAdaptiveImageOpItem::Playback},
    { DrawOpItem::ADAPTIVE_PIXELMAP_OPITEM, DrawAdaptivePixelMapOpItem::Playback},
    { DrawOpItem::EXTEND_PIXELMAP_OPITEM,   DrawExtendPixelMapOpItem::Playback},
    { DrawOpItem::IMAGE_WITH_PARM_OPITEM,   DrawImageWithParmOpItem::Playback},
    { DrawOpItem::REGION_OPITEM,            DrawRegionOpItem::Playback },
    { DrawOpItem::PATCH_OPITEM,             DrawPatchOpItem::Playback },
    { DrawOpItem::EDGEAAQUAD_OPITEM, DrawEdgeAAQuadOpItem::Playback },
    { DrawOpItem::VERTICES_OPITEM,          DrawVerticesOpItem::Playback },
    { DrawOpItem::NO_IPC_IMAGE_DRAW_OPITEM, NoIPCImageOpItem::Playback },
};

CanvasPlayer::CanvasPlayer(Canvas& canvas, const CmdList& cmdList, const Rect& rect)
    : canvas_(canvas), cmdList_(cmdList), rect_(rect) {}

bool CanvasPlayer::Playback(uint32_t type, void* opItem)
{
    if (type == DrawOpItem::OPITEM_HEAD) {
        return true;
    }

    auto it = opPlaybackFuncLUT_.find(type);
    if (it == opPlaybackFuncLUT_.end() || it->second == nullptr) {
        return false;
    }

    auto func = it->second;
    (*func)(*this, opItem);

    return true;
}

GenerateCachedOpItemPlayer::GenerateCachedOpItemPlayer(CmdList &cmdList, Canvas* canvas, const Rect* rect)
    : canvas_(canvas), rect_(rect), cmdList_(cmdList) {}

std::shared_ptr<NoIPCImageOpItem> GenerateCachedOpItemPlayer::GenerateCachedOpItem(uint32_t type, void* opItem,
    bool addCmdList, bool& replaceSuccess)
{
    if (opItem == nullptr) {
        return nullptr;
    } else if (type == DrawOpItem::ATTACH_PEN_OPITEM) {
        penOpItemStack.push(opItem);
    } else if (type == DrawOpItem::ATTACH_BRUSH_OPITEM) {
        brushOpItemStack.push(opItem);
    } else if (type == DrawOpItem::DETACH_PEN_OPITEM) {
        penOpItemStack.pop();
    } else if (type == DrawOpItem::DETACH_BRUSH_OPITEM) {
        brushOpItemStack.pop();
    } else if (type == DrawOpItem::TEXT_BLOB_OPITEM) {
        auto *op = static_cast<DrawTextBlobOpItem*>(opItem);
        if (op) {
            AttachPenOpItem* penOpItem = penOpItemStack.empty() ? nullptr :
                static_cast<AttachPenOpItem*>(penOpItemStack.top());
            AttachBrushOpItem* brushOpItem = brushOpItemStack.empty() ? nullptr :
                static_cast<AttachBrushOpItem*>(brushOpItemStack.top());
            return op->GenerateCachedOpItem(cmdList_, canvas_, penOpItem, brushOpItem, addCmdList, replaceSuccess);
        }
    }
    return nullptr;
}

std::unordered_map<uint32_t, UnmarshallingPlayer::UnmarshallingFunc> UnmarshallingPlayer::opUnmarshallingFuncLUT_ = {
    { DrawOpItem::POINT_OPITEM,             DrawPointOpItem::Unmarshalling },
    { DrawOpItem::POINTS_OPITEM,            DrawPointsOpItem::Unmarshalling },
    { DrawOpItem::LINE_OPITEM,              DrawLineOpItem::Unmarshalling },
    { DrawOpItem::RECT_OPITEM,              DrawRectOpItem::Unmarshalling },
    { DrawOpItem::ROUND_RECT_OPITEM,        DrawRoundRectOpItem::Unmarshalling },
    { DrawOpItem::NESTED_ROUND_RECT_OPITEM, DrawNestedRoundRectOpItem::Unmarshalling },
    { DrawOpItem::ARC_OPITEM,               DrawArcOpItem::Unmarshalling },
    { DrawOpItem::PIE_OPITEM,               DrawPieOpItem::Unmarshalling },
    { DrawOpItem::OVAL_OPITEM,              DrawOvalOpItem::Unmarshalling },
    { DrawOpItem::CIRCLE_OPITEM,            DrawCircleOpItem::Unmarshalling },
    { DrawOpItem::PATH_OPITEM,              DrawPathOpItem::Unmarshalling },
    { DrawOpItem::BACKGROUND_OPITEM,        DrawBackgroundOpItem::Unmarshalling },
    { DrawOpItem::SHADOW_OPITEM,            DrawShadowOpItem::Unmarshalling },
    { DrawOpItem::COLOR_OPITEM,             DrawColorOpItem::Unmarshalling },
    { DrawOpItem::IMAGE_NINE_OPITEM,        DrawImageNineOpItem::Unmarshalling },
    { DrawOpItem::IMAGE_ANNOTATION_OPITEM,  DrawAnnotationOpItem::Unmarshalling },
    { DrawOpItem::IMAGE_LATTICE_OPITEM,     DrawImageLatticeOpItem::Unmarshalling },
    { DrawOpItem::BITMAP_OPITEM,            DrawBitmapOpItem::Unmarshalling },
    { DrawOpItem::IMAGE_OPITEM,             DrawImageOpItem::Unmarshalling },
    { DrawOpItem::IMAGE_RECT_OPITEM,        DrawImageRectOpItem::Unmarshalling },
    { DrawOpItem::PICTURE_OPITEM,           DrawPictureOpItem::Unmarshalling },
    { DrawOpItem::TEXT_BLOB_OPITEM,         DrawTextBlobOpItem::Unmarshalling },
    { DrawOpItem::CLIP_RECT_OPITEM,         ClipRectOpItem::Unmarshalling },
    { DrawOpItem::CLIP_IRECT_OPITEM,        ClipIRectOpItem::Unmarshalling },
    { DrawOpItem::CLIP_ROUND_RECT_OPITEM,   ClipRoundRectOpItem::Unmarshalling },
    { DrawOpItem::CLIP_PATH_OPITEM,         ClipPathOpItem::Unmarshalling },
    { DrawOpItem::CLIP_REGION_OPITEM,       ClipRegionOpItem::Unmarshalling },
    { DrawOpItem::SET_MATRIX_OPITEM,        SetMatrixOpItem::Unmarshalling },
    { DrawOpItem::RESET_MATRIX_OPITEM,      ResetMatrixOpItem::Unmarshalling },
    { DrawOpItem::CONCAT_MATRIX_OPITEM,     ConcatMatrixOpItem::Unmarshalling },
    { DrawOpItem::TRANSLATE_OPITEM,         TranslateOpItem::Unmarshalling },
    { DrawOpItem::SCALE_OPITEM,             ScaleOpItem::Unmarshalling },
    { DrawOpItem::ROTATE_OPITEM,            RotateOpItem::Unmarshalling },
    { DrawOpItem::SHEAR_OPITEM,             ShearOpItem::Unmarshalling },
    { DrawOpItem::FLUSH_OPITEM,             FlushOpItem::Unmarshalling },
    { DrawOpItem::CLEAR_OPITEM,             ClearOpItem::Unmarshalling },
    { DrawOpItem::SAVE_OPITEM,              SaveOpItem::Unmarshalling },
    { DrawOpItem::SAVE_LAYER_OPITEM,        SaveLayerOpItem::Unmarshalling },
    { DrawOpItem::RESTORE_OPITEM,           RestoreOpItem::Unmarshalling },
    { DrawOpItem::DISCARD_OPITEM,           DiscardOpItem::Unmarshalling },
    { DrawOpItem::ATTACH_PEN_OPITEM,        AttachPenOpItem::Unmarshalling },
    { DrawOpItem::ATTACH_BRUSH_OPITEM,      AttachBrushOpItem::Unmarshalling },
    { DrawOpItem::DETACH_PEN_OPITEM,        DetachPenOpItem::Unmarshalling },
    { DrawOpItem::DETACH_BRUSH_OPITEM,      DetachBrushOpItem::Unmarshalling },
    { DrawOpItem::CLIP_ADAPTIVE_ROUND_RECT_OPITEM, ClipAdaptiveRoundRectOpItem::Unmarshalling},
    { DrawOpItem::ADAPTIVE_IMAGE_OPITEM,    DrawAdaptiveImageOpItem::Unmarshalling},
    { DrawOpItem::ADAPTIVE_PIXELMAP_OPITEM, DrawAdaptivePixelMapOpItem::Unmarshalling},
    { DrawOpItem::EXTEND_PIXELMAP_OPITEM,   DrawExtendPixelMapOpItem::Unmarshalling},
    { DrawOpItem::IMAGE_WITH_PARM_OPITEM,   DrawImageWithParmOpItem::Unmarshalling},
    { DrawOpItem::REGION_OPITEM,            DrawRegionOpItem::Unmarshalling },
    { DrawOpItem::PATCH_OPITEM,             DrawPatchOpItem::Unmarshalling },
    { DrawOpItem::EDGEAAQUAD_OPITEM, DrawEdgeAAQuadOpItem::Unmarshalling },
    { DrawOpItem::VERTICES_OPITEM,          DrawVerticesOpItem::Unmarshalling },
};

UnmarshallingPlayer::UnmarshallingPlayer(const CmdList& cmdList)
    : cmdList_(cmdList) {}

std::shared_ptr<OpItem> UnmarshallingPlayer::Unmarshalling(uint32_t type, void* opItem)
{
    if (type == DrawOpItem::OPITEM_HEAD) {
        return nullptr;
    }

    auto it = opUnmarshallingFuncLUT_.find(type);
    if (it == opUnmarshallingFuncLUT_.end() || it->second == nullptr) {
        return nullptr;
    }

    auto func = it->second;
    return (*func)(this->cmdList_, opItem);
}

/* OpItem */
DrawPointOpItem::DrawPointOpItem() : DrawOpItem(POINT_OPITEM) {}
DrawPointOpItem::DrawPointOpItem(const Point& point) : DrawOpItem(POINT_OPITEM), point_(point) {}

std::shared_ptr<OpItem> DrawPointOpItem::Unmarshalling(const CmdList& cmdList, void* opItem)
{
    CHECK_AND_RETURN_RET_LOG(opItem != nullptr, nullptr, "opItem is nullptr!");
    auto op = std::make_shared<DrawPointOpItem>();
    COPY_OPITEN_AND_RETURN_RET_LOG(DrawPointOpItem, op.get(), opItem, nullptr, "failed to memcpy_s");
    return op;
}

void DrawPointOpItem::Playback(CanvasPlayer& player, void* opItem)
{
    if (opItem != nullptr) {
        auto* op = static_cast<DrawPointOpItem*>(opItem);
        op->Playback(player.canvas_);
    }
}

void DrawPointOpItem::Playback(Canvas& canvas)
{
    canvas.DrawPoint(point_);
}

DrawPointsOpItem::DrawPointsOpItem() : DrawOpItem(POINTS_OPITEM) {}
DrawPointsOpItem::DrawPointsOpItem(PointMode mode, const std::pair<uint32_t, size_t> pts)
    : DrawOpItem(POINTS_OPITEM), mode_(mode), pts_(pts) {}

std::shared_ptr<OpItem> DrawPointsOpItem::Unmarshalling(const CmdList& cmdList, void* opItem)
{
    CHECK_AND_RETURN_RET_LOG(opItem != nullptr, nullptr, "opItem is nullptr!");
    auto op = std::make_shared<DrawPointsOpItem>();
    COPY_OPITEN_AND_RETURN_RET_LOG(DrawPointsOpItem, op.get(), opItem, nullptr, "failed to memcpy_s");
    op->Unmarshalling(cmdList);
    return op;
}

void DrawPointsOpItem::Unmarshalling(const CmdList& cmdList, Canvas* canvas)
{
    auto pts = CmdListHelper::GetVectorFromCmdList<Point>(cmdList, pts_);
    if (canvas) {
        canvas->DrawPoints(mode_, pts.size(), pts.data());
    } else {
        playbackTask_ = [this, pts](Canvas& canvas) {
            canvas.DrawPoints(mode_, pts.size(), pts.data());
        };
    }
}

void DrawPointsOpItem::Playback(CanvasPlayer& player, void* opItem)
{
    if (opItem != nullptr) {
        auto* op = static_cast<DrawPointsOpItem*>(opItem);
        op->Playback(player.canvas_, player.cmdList_);
    }
}

void DrawPointsOpItem::Playback(Canvas& canvas, const CmdList& cmdList)
{
    if (playbackTask_) {
        playbackTask_(canvas);
    } else {
        Unmarshalling(cmdList, &canvas);
    }
}

DrawLineOpItem::DrawLineOpItem() : DrawOpItem(LINE_OPITEM) {}
DrawLineOpItem::DrawLineOpItem(const Point& startPt, const Point& endPt) : DrawOpItem(LINE_OPITEM),
    startPt_(startPt), endPt_(endPt) {}

std::shared_ptr<OpItem> DrawLineOpItem::Unmarshalling(const CmdList& cmdList, void* opItem)
{
    CHECK_AND_RETURN_RET_LOG(opItem != nullptr, nullptr, "opItem is nullptr!");
    auto op = std::make_shared<DrawLineOpItem>();
    COPY_OPITEN_AND_RETURN_RET_LOG(DrawLineOpItem, op.get(), opItem, nullptr, "failed to memcpy_s");
    return op;
}

void DrawLineOpItem::Playback(CanvasPlayer& player, void* opItem)
{
    if (opItem != nullptr) {
        auto* op = static_cast<DrawLineOpItem*>(opItem);
        op->Playback(player.canvas_);
    }
}

void DrawLineOpItem::Playback(Canvas& canvas)
{
    canvas.DrawLine(startPt_, endPt_);
}

DrawRectOpItem::DrawRectOpItem() : DrawOpItem(RECT_OPITEM) {}
DrawRectOpItem::DrawRectOpItem(const Rect& rect) : DrawOpItem(RECT_OPITEM), rect_(rect) {}

std::shared_ptr<OpItem> DrawRectOpItem::Unmarshalling(const CmdList& cmdList, void* opItem)
{
    CHECK_AND_RETURN_RET_LOG(opItem != nullptr, nullptr, "opItem is nullptr!");
    auto op = std::make_shared<DrawRectOpItem>();
    COPY_OPITEN_AND_RETURN_RET_LOG(DrawRectOpItem, op.get(), opItem, nullptr, "failed to memcpy_s");
    return op;
}

void DrawRectOpItem::Playback(CanvasPlayer& player, void* opItem)
{
    if (opItem != nullptr) {
        auto* op = static_cast<DrawRectOpItem*>(opItem);
        op->Playback(player.canvas_);
    }
}

void DrawRectOpItem::Playback(Canvas& canvas)
{
    canvas.DrawRect(rect_);
}

DrawRoundRectOpItem::DrawRoundRectOpItem() : DrawOpItem(ROUND_RECT_OPITEM) {}
DrawRoundRectOpItem::DrawRoundRectOpItem(std::pair<uint32_t, size_t> radiusXYData, const Rect& rect)
    : DrawOpItem(ROUND_RECT_OPITEM), radiusXYData_(radiusXYData), rect_(rect) {}

std::shared_ptr<OpItem> DrawRoundRectOpItem::Unmarshalling(const CmdList& cmdList, void* opItem)
{
    CHECK_AND_RETURN_RET_LOG(opItem != nullptr, nullptr, "opItem is nullptr!");
    auto op = std::make_shared<DrawRoundRectOpItem>();
    COPY_OPITEN_AND_RETURN_RET_LOG(DrawRoundRectOpItem, op.get(), opItem, nullptr, "failed to memcpy_s");
    op->Unmarshalling(cmdList);
    return op;
}

void DrawRoundRectOpItem::Unmarshalling(const CmdList& cmdList, Canvas* canvas)
{
    auto radiusXYData = CmdListHelper::GetVectorFromCmdList<Point>(cmdList, radiusXYData_);
    RoundRect roundRect(rect_, radiusXYData);

    if (canvas) {
        canvas->DrawRoundRect(roundRect);
    } else {
        playbackTask_ = [roundRect](Canvas& canvas) {
            canvas.DrawRoundRect(roundRect);
        };
    }
}

void DrawRoundRectOpItem::Playback(CanvasPlayer& player, void* opItem)
{
    if (opItem != nullptr) {
        auto* op = static_cast<DrawRoundRectOpItem*>(opItem);
        op->Playback(player.canvas_, player.cmdList_);
    }
}

void DrawRoundRectOpItem::Playback(Canvas& canvas, const CmdList& cmdList)
{
    if (playbackTask_) {
        playbackTask_(canvas);
    } else {
        Unmarshalling(cmdList, &canvas);
    }
}

DrawNestedRoundRectOpItem::DrawNestedRoundRectOpItem() : DrawOpItem(NESTED_ROUND_RECT_OPITEM) {}
DrawNestedRoundRectOpItem::DrawNestedRoundRectOpItem(const std::pair<uint32_t, size_t> outerRadiusXYData,
    const Rect& outerRect, const std::pair<uint32_t, size_t> innerRadiusXYData, const Rect& innerRect)
    : DrawOpItem(NESTED_ROUND_RECT_OPITEM), outerRadiusXYData_(outerRadiusXYData),
    innerRadiusXYData_(innerRadiusXYData), outerRect_(outerRect), innerRect_(innerRect) {}

std::shared_ptr<OpItem> DrawNestedRoundRectOpItem::Unmarshalling(const CmdList& cmdList, void* opItem)
{
    CHECK_AND_RETURN_RET_LOG(opItem != nullptr, nullptr, "opItem is nullptr!");
    auto op = std::make_shared<DrawNestedRoundRectOpItem>();
    COPY_OPITEN_AND_RETURN_RET_LOG(DrawNestedRoundRectOpItem, op.get(), opItem, nullptr, "failed to memcpy_s");
    op->Unmarshalling(cmdList);
    return op;
}

void DrawNestedRoundRectOpItem::Unmarshalling(const CmdList& cmdList, Canvas* canvas)
{
    auto outerRadiusXYData = CmdListHelper::GetVectorFromCmdList<Point>(cmdList, outerRadiusXYData_);
    RoundRect outerRect(outerRect_, outerRadiusXYData);
    auto innerRadiusXYData = CmdListHelper::GetVectorFromCmdList<Point>(cmdList, innerRadiusXYData_);
    RoundRect innerRect(innerRect_, innerRadiusXYData);

    if (canvas) {
        canvas->DrawNestedRoundRect(outerRect, innerRect);
    } else {
        playbackTask_ = [this, outerRect, innerRect](Canvas& canvas) {
            canvas.DrawNestedRoundRect(outerRect, innerRect);
        };
    }
}

void DrawNestedRoundRectOpItem::Playback(CanvasPlayer& player, void* opItem)
{
    if (opItem != nullptr) {
        auto* op = static_cast<DrawNestedRoundRectOpItem*>(opItem);
        op->Playback(player.canvas_, player.cmdList_);
    }
}

void DrawNestedRoundRectOpItem::Playback(Canvas& canvas, const CmdList& cmdList)
{
    if (playbackTask_) {
        playbackTask_(canvas);
    } else {
        Unmarshalling(cmdList, &canvas);
    }
}

DrawArcOpItem::DrawArcOpItem() : DrawOpItem(ARC_OPITEM) {}
DrawArcOpItem::DrawArcOpItem(const Rect& rect, scalar startAngle, scalar sweepAngle)
    : DrawOpItem(ARC_OPITEM), rect_(rect), startAngle_(startAngle), sweepAngle_(sweepAngle) {}

std::shared_ptr<OpItem> DrawArcOpItem::Unmarshalling(const CmdList& cmdList, void* opItem)
{
    CHECK_AND_RETURN_RET_LOG(opItem != nullptr, nullptr, "opItem is nullptr!");
    auto op = std::make_shared<DrawArcOpItem>();
    COPY_OPITEN_AND_RETURN_RET_LOG(DrawArcOpItem, op.get(), opItem, nullptr, "failed to memcpy_s");
    return op;
}

void DrawArcOpItem::Playback(CanvasPlayer& player, void* opItem)
{
    if (opItem != nullptr) {
        auto* op = static_cast<DrawArcOpItem*>(opItem);
        op->Playback(player.canvas_);
    }
}

void DrawArcOpItem::Playback(Canvas& canvas)
{
    canvas.DrawArc(rect_, startAngle_, sweepAngle_);
}

DrawPieOpItem::DrawPieOpItem() : DrawOpItem(PIE_OPITEM) {}
DrawPieOpItem::DrawPieOpItem(const Rect& rect, scalar startAngle, scalar sweepAngle)
    : DrawOpItem(PIE_OPITEM), rect_(rect), startAngle_(startAngle), sweepAngle_(sweepAngle) {}

std::shared_ptr<OpItem> DrawPieOpItem::Unmarshalling(const CmdList& cmdList, void* opItem)
{
    CHECK_AND_RETURN_RET_LOG(opItem != nullptr, nullptr, "opItem is nullptr!");
    auto op = std::make_shared<DrawPieOpItem>();
    COPY_OPITEN_AND_RETURN_RET_LOG(DrawPieOpItem, op.get(), opItem, nullptr, "failed to memcpy_s");
    return op;
}

void DrawPieOpItem::Playback(CanvasPlayer& player, void* opItem)
{
    if (opItem != nullptr) {
        auto* op = static_cast<DrawPieOpItem*>(opItem);
        op->Playback(player.canvas_);
    }
}

void DrawPieOpItem::Playback(Canvas& canvas)
{
    canvas.DrawPie(rect_, startAngle_, sweepAngle_);
}

DrawOvalOpItem::DrawOvalOpItem() : DrawOpItem(OVAL_OPITEM) {}
DrawOvalOpItem::DrawOvalOpItem(const Rect& rect) : DrawOpItem(OVAL_OPITEM), rect_(rect) {}

std::shared_ptr<OpItem> DrawOvalOpItem::Unmarshalling(const CmdList& cmdList, void* opItem)
{
    CHECK_AND_RETURN_RET_LOG(opItem != nullptr, nullptr, "opItem is nullptr!");
    auto op = std::make_shared<DrawOvalOpItem>();
    COPY_OPITEN_AND_RETURN_RET_LOG(DrawOvalOpItem, op.get(), opItem, nullptr, "failed to memcpy_s");
    return op;
}

void DrawOvalOpItem::Playback(CanvasPlayer& player, void* opItem)
{
    if (opItem != nullptr) {
        auto* op = static_cast<DrawOvalOpItem*>(opItem);
        op->Playback(player.canvas_);
    }
}

void DrawOvalOpItem::Playback(Canvas& canvas)
{
    canvas.DrawOval(rect_);
}

DrawCircleOpItem::DrawCircleOpItem() : DrawOpItem(CIRCLE_OPITEM) {}
DrawCircleOpItem::DrawCircleOpItem(const Point& centerPt, scalar radius)
    : DrawOpItem(CIRCLE_OPITEM), centerPt_(centerPt), radius_(radius) {}

std::shared_ptr<OpItem> DrawCircleOpItem::Unmarshalling(const CmdList& cmdList, void* opItem)
{
    CHECK_AND_RETURN_RET_LOG(opItem != nullptr, nullptr, "opItem is nullptr!");
    auto op = std::make_shared<DrawCircleOpItem>();
    COPY_OPITEN_AND_RETURN_RET_LOG(DrawCircleOpItem, op.get(), opItem, nullptr, "failed to memcpy_s");
    return op;
}

void DrawCircleOpItem::Playback(CanvasPlayer& player, void* opItem)
{
    if (opItem != nullptr) {
        auto* op = static_cast<DrawCircleOpItem*>(opItem);
        op->Playback(player.canvas_);
    }
}

void DrawCircleOpItem::Playback(Canvas& canvas)
{
    canvas.DrawCircle(centerPt_, radius_);
}

DrawPathOpItem::DrawPathOpItem() : DrawOpItem(PATH_OPITEM) {}
DrawPathOpItem::DrawPathOpItem(const CmdListHandle& path) : DrawOpItem(PATH_OPITEM), path_(path) {}

std::shared_ptr<OpItem> DrawPathOpItem::Unmarshalling(const CmdList& cmdList, void* opItem)
{
    CHECK_AND_RETURN_RET_LOG(opItem != nullptr, nullptr, "opItem is nullptr!");
    auto op = std::make_shared<DrawPathOpItem>();
    COPY_OPITEN_AND_RETURN_RET_LOG(DrawPathOpItem, op.get(), opItem, nullptr, "failed to memcpy_s");
    op->Unmarshalling(cmdList);
    return op;
}

void DrawPathOpItem::Unmarshalling(const CmdList& cmdList, Canvas* canvas)
{
    auto path = CmdListHelper::GetFromCmdList<PathCmdList, Path>(cmdList, path_);
    if (path == nullptr) {
        LOGE("path is nullptr!");
        return;
    }

    if (canvas) {
        canvas->DrawPath(*path);
    } else {
        playbackTask_ = [this, path](Canvas& canvas) {
            canvas.DrawPath(*path);
        };
    }
}

void DrawPathOpItem::Playback(CanvasPlayer& player, void* opItem)
{
    if (opItem != nullptr) {
        auto* op = static_cast<DrawPathOpItem*>(opItem);
        op->Playback(player.canvas_, player.cmdList_);
    }
}

void DrawPathOpItem::Playback(Canvas& canvas, const CmdList& cmdList)
{
    if (playbackTask_) {
        playbackTask_(canvas);
    } else {
        Unmarshalling(cmdList, &canvas);
    }
}

DrawBackgroundOpItem::DrawBackgroundOpItem() : DrawOpItem(BACKGROUND_OPITEM) {}
DrawBackgroundOpItem::DrawBackgroundOpItem(const BrushHandle& brushHandle)
    : DrawOpItem(BACKGROUND_OPITEM), brushHandle_(brushHandle) {}

std::shared_ptr<OpItem> DrawBackgroundOpItem::Unmarshalling(const CmdList& cmdList, void* opItem)
{
    CHECK_AND_RETURN_RET_LOG(opItem != nullptr, nullptr, "opItem is nullptr!");
    auto op = std::make_shared<DrawBackgroundOpItem>();
    COPY_OPITEN_AND_RETURN_RET_LOG(DrawBackgroundOpItem, op.get(), opItem, nullptr, "failed to memcpy_s");
    op->Unmarshalling(cmdList);
    return op;
}

void DrawBackgroundOpItem::Unmarshalling(const CmdList& cmdList, Canvas* canvas)
{
    auto colorSpace = CmdListHelper::GetColorSpaceFromCmdList(
        cmdList, brushHandle_.colorSpaceHandle);
    auto shaderEffect = CmdListHelper::GetShaderEffectFromCmdList(
        cmdList, brushHandle_.shaderEffectHandle);
    auto colorFilter = CmdListHelper::GetColorFilterFromCmdList(
        cmdList, brushHandle_.colorFilterHandle);
    auto imageFilter = CmdListHelper::GetImageFilterFromCmdList(
        cmdList, brushHandle_.imageFilterHandle);
    auto maskFilter = CmdListHelper::GetMaskFilterFromCmdList(
        cmdList, brushHandle_.maskFilterHandle);

    Filter filter;
    filter.SetColorFilter(colorFilter);
    filter.SetImageFilter(imageFilter);
    filter.SetMaskFilter(maskFilter);
    filter.SetFilterQuality(brushHandle_.filterQuality);

    const Color4f color4f = { brushHandle_.color.GetRedF(), brushHandle_.color.GetGreenF(),
        brushHandle_.color.GetBlueF(), brushHandle_.color.GetAlphaF() };

    Brush brush;
    brush.SetColor(color4f, colorSpace);
    brush.SetShaderEffect(shaderEffect);
    brush.SetBlendMode(brushHandle_.mode);
    brush.SetAntiAlias(brushHandle_.isAntiAlias);
    brush.SetFilter(filter);

    if (canvas) {
        canvas->DrawBackground(brush);
    } else {
        playbackTask_ = [this, brush](Canvas& canvas) {
            canvas.DrawBackground(brush);
        };
    }
}

void DrawBackgroundOpItem::Playback(CanvasPlayer& player, void* opItem)
{
    if (opItem != nullptr) {
        auto* op = static_cast<DrawBackgroundOpItem*>(opItem);
        op->Playback(player.canvas_, player.cmdList_);
    }
}

void DrawBackgroundOpItem::Playback(Canvas& canvas, const CmdList& cmdList)
{
    if (playbackTask_) {
        playbackTask_(canvas);
    } else {
        Unmarshalling(cmdList, &canvas);
    }
}

DrawShadowOpItem::DrawShadowOpItem() : DrawOpItem(SHADOW_OPITEM) {}
DrawShadowOpItem::DrawShadowOpItem(const CmdListHandle& path, const Point3& planeParams, const Point3& devLightPos,
    scalar lightRadius, Color ambientColor, Color spotColor, ShadowFlags flag)
    : DrawOpItem(SHADOW_OPITEM), path_(path), planeParams_(planeParams), devLightPos_(devLightPos),
    lightRadius_(lightRadius), ambientColor_(ambientColor), spotColor_(spotColor), flag_(flag) {}

std::shared_ptr<OpItem> DrawShadowOpItem::Unmarshalling(const CmdList& cmdList, void* opItem)
{
    CHECK_AND_RETURN_RET_LOG(opItem != nullptr, nullptr, "opItem is nullptr!");
    auto op = std::make_shared<DrawShadowOpItem>();
    COPY_OPITEN_AND_RETURN_RET_LOG(DrawShadowOpItem, op.get(), opItem, nullptr, "failed to memcpy_s");
    op->Unmarshalling(cmdList);
    return op;
}

void DrawShadowOpItem::Unmarshalling(const CmdList& cmdList, Canvas* canvas)
{
    auto path = CmdListHelper::GetFromCmdList<PathCmdList, Path>(cmdList, path_);
    if (path == nullptr) {
        LOGE("path is nullptr!");
        return;
    }

    if (canvas) {
        canvas->DrawShadow(*path, planeParams_, devLightPos_, lightRadius_,
                           ambientColor_, spotColor_, flag_);
    } else {
        playbackTask_ = [this, path](Canvas& canvas) {
            canvas.DrawShadow(*path, planeParams_, devLightPos_, lightRadius_,
                              ambientColor_, spotColor_, flag_);
        };
    }
}

void DrawShadowOpItem::Playback(CanvasPlayer& player, void* opItem)
{
    if (opItem != nullptr) {
        auto* op = static_cast<DrawShadowOpItem*>(opItem);
        op->Playback(player.canvas_, player.cmdList_);
    }
}

void DrawShadowOpItem::Playback(Canvas& canvas, const CmdList& cmdList)
{
    if (playbackTask_) {
        playbackTask_(canvas);
    } else {
        Unmarshalling(cmdList, &canvas);
    }
}

DrawRegionOpItem::DrawRegionOpItem() : DrawOpItem(REGION_OPITEM) {}
DrawRegionOpItem::DrawRegionOpItem(const CmdListHandle& region) : DrawOpItem(REGION_OPITEM), region_(region) {}

std::shared_ptr<OpItem> DrawRegionOpItem::Unmarshalling(const CmdList& cmdList, void* opItem)
{
    CHECK_AND_RETURN_RET_LOG(opItem != nullptr, nullptr, "opItem is nullptr!");
    auto op = std::make_shared<DrawRegionOpItem>();
    COPY_OPITEN_AND_RETURN_RET_LOG(DrawRegionOpItem, op.get(), opItem, nullptr, "failed to memcpy_s");
    op->Unmarshalling(cmdList);
    return op;
}

void DrawRegionOpItem::Unmarshalling(const CmdList& cmdList, Canvas* canvas)
{
    auto region = CmdListHelper::GetFromCmdList<RegionCmdList, Region>(cmdList, region_);
    if (region == nullptr) {
        LOGE("region is nullptr!");
        return;
    }

    if (canvas) {
        canvas->DrawRegion(*region);
    } else {
        playbackTask_ = [this, region](Canvas& canvas) {
            canvas.DrawRegion(*region);
        };
    }
}

void DrawRegionOpItem::Playback(CanvasPlayer& player, void* opItem)
{
    if (opItem != nullptr) {
        auto* op = static_cast<DrawRegionOpItem*>(opItem);
        op->Playback(player.canvas_, player.cmdList_);
    }
}

void DrawRegionOpItem::Playback(Canvas& canvas, const CmdList& cmdList)
{
    if (playbackTask_) {
        playbackTask_(canvas);
    } else {
        Unmarshalling(cmdList, &canvas);
    }
}

DrawPatchOpItem::DrawPatchOpItem() : DrawOpItem(PATCH_OPITEM) {}
DrawPatchOpItem::DrawPatchOpItem(const std::pair<uint32_t, size_t> cubics, const std::pair<uint32_t, size_t> colors,
    const std::pair<uint32_t, size_t> texCoords, BlendMode mode)
    : DrawOpItem(PATCH_OPITEM), cubics_(cubics), colors_(colors), texCoords_(texCoords), mode_(mode) {}

std::shared_ptr<OpItem> DrawPatchOpItem::Unmarshalling(const CmdList& cmdList, void* opItem)
{
    CHECK_AND_RETURN_RET_LOG(opItem != nullptr, nullptr, "opItem is nullptr!");
    auto op = std::make_shared<DrawPatchOpItem>();
    COPY_OPITEN_AND_RETURN_RET_LOG(DrawPatchOpItem, op.get(), opItem, nullptr, "failed to memcpy_s");
    op->Unmarshalling(cmdList);
    return op;
}

void DrawPatchOpItem::Unmarshalling(const CmdList& cmdList, Canvas* canvas)
{
    auto cubics = CmdListHelper::GetVectorFromCmdList<Point>(cmdList, cubics_);
    auto colors = CmdListHelper::GetVectorFromCmdList<ColorQuad>(cmdList, colors_);
    auto texCoords = CmdListHelper::GetVectorFromCmdList<Point>(cmdList, texCoords_);

    if (canvas) {
        canvas->DrawPatch(cubics.empty() ? nullptr : cubics.data(),
                          colors.empty() ? nullptr : colors.data(),
                          texCoords.empty() ? nullptr : texCoords.data(),
                          mode_);
    } else {
        playbackTask_ = [this, cubics, colors, texCoords](Canvas& canvas) {
            canvas.DrawPatch(cubics.empty() ? nullptr : cubics.data(),
                             colors.empty() ? nullptr : colors.data(),
                             texCoords.empty() ? nullptr : texCoords.data(),
                             mode_);
        };
    }
}

void DrawPatchOpItem::Playback(CanvasPlayer& player, void* opItem)
{
    if (opItem != nullptr) {
        auto* op = static_cast<DrawPatchOpItem*>(opItem);
        op->Playback(player.canvas_, player.cmdList_);
    }
}

void DrawPatchOpItem::Playback(Canvas& canvas, const CmdList& cmdList)
{
    if (playbackTask_) {
        playbackTask_(canvas);
    } else {
        Unmarshalling(cmdList, &canvas);
    }
}

DrawEdgeAAQuadOpItem::DrawEdgeAAQuadOpItem() : DrawOpItem(EDGEAAQUAD_OPITEM) {}
DrawEdgeAAQuadOpItem::DrawEdgeAAQuadOpItem(const Rect& rect,
    const std::pair<uint32_t, size_t> clipQuad, QuadAAFlags aaFlags, ColorQuad color, BlendMode mode)
    : DrawOpItem(EDGEAAQUAD_OPITEM), rect_(rect), clipQuad_(clipQuad),
    aaFlags_(aaFlags), color_(color), mode_(mode) {}

std::shared_ptr<OpItem> DrawEdgeAAQuadOpItem::Unmarshalling(const CmdList& cmdList, void* opItem)
{
    CHECK_AND_RETURN_RET_LOG(opItem != nullptr, nullptr, "opItem is nullptr!");
    auto op = std::make_shared<DrawEdgeAAQuadOpItem>();
    COPY_OPITEN_AND_RETURN_RET_LOG(DrawEdgeAAQuadOpItem, op.get(), opItem, nullptr, "failed to memcpy_s");
    op->Unmarshalling(cmdList);
    return op;
}

void DrawEdgeAAQuadOpItem::Unmarshalling(const CmdList& cmdList, Canvas* canvas)
{
    auto clip = CmdListHelper::GetVectorFromCmdList<Point>(cmdList, clipQuad_);

    if (canvas) {
        canvas->DrawEdgeAAQuad(rect_, clip.empty() ? nullptr : clip.data(), aaFlags_, color_, mode_);
    } else {
        playbackTask_ = [this, clip](Canvas& canvas) {
            canvas.DrawEdgeAAQuad(rect_, clip.empty() ? nullptr : clip.data(), aaFlags_, color_, mode_);
        };
    }
}

void DrawEdgeAAQuadOpItem::Playback(CanvasPlayer& player, void* opItem)
{
    if (opItem != nullptr) {
        auto* op = static_cast<DrawEdgeAAQuadOpItem*>(opItem);
        op->Playback(player.canvas_, player.cmdList_);
    }
}

void DrawEdgeAAQuadOpItem::Playback(Canvas& canvas, const CmdList& cmdList)
{
    if (playbackTask_) {
        playbackTask_(canvas);
    } else {
        Unmarshalling(cmdList, &canvas);
    }
}

DrawVerticesOpItem::DrawVerticesOpItem() : DrawOpItem(VERTICES_OPITEM) {}
DrawVerticesOpItem::DrawVerticesOpItem(const VerticesHandle& vertices, BlendMode mode)
    : DrawOpItem(VERTICES_OPITEM), vertices_(vertices), mode_(mode) {}

std::shared_ptr<OpItem> DrawVerticesOpItem::Unmarshalling(const CmdList& cmdList, void* opItem)
{
    CHECK_AND_RETURN_RET_LOG(opItem != nullptr, nullptr, "opItem is nullptr!");
    auto op = std::make_shared<DrawVerticesOpItem>();
    COPY_OPITEN_AND_RETURN_RET_LOG(DrawVerticesOpItem, op.get(), opItem, nullptr, "failed to memcpy_s");
    op->Unmarshalling(cmdList);
    return op;
}

void DrawVerticesOpItem::Unmarshalling(const CmdList& cmdList, Canvas* canvas)
{
    auto vertices = CmdListHelper::GetVerticesFromCmdList(cmdList, vertices_);
    if (vertices == nullptr) {
        return;
    }

    if (canvas) {
        canvas->DrawVertices(*vertices, mode_);
    } else {
        playbackTask_ = [this, vertices](Canvas& canvas) {
            canvas.DrawVertices(*vertices, mode_);
        };
    }
}

void DrawVerticesOpItem::Playback(CanvasPlayer& player, void* opItem)
{
    if (opItem != nullptr) {
        auto* op = static_cast<DrawVerticesOpItem*>(opItem);
        op->Playback(player.canvas_, player.cmdList_);
    }
}

void DrawVerticesOpItem::Playback(Canvas& canvas, const CmdList& cmdList)
{
    if (playbackTask_) {
        playbackTask_(canvas);
    } else {
        Unmarshalling(cmdList, &canvas);
    }
}

DrawColorOpItem::DrawColorOpItem() : DrawOpItem(COLOR_OPITEM) {}
DrawColorOpItem::DrawColorOpItem(ColorQuad color, BlendMode mode) : DrawOpItem(COLOR_OPITEM),
    color_(color), mode_(mode) {}

std::shared_ptr<OpItem> DrawColorOpItem::Unmarshalling(const CmdList& cmdList, void* opItem)
{
    CHECK_AND_RETURN_RET_LOG(opItem != nullptr, nullptr, "opItem is nullptr!");
    auto op = std::make_shared<DrawColorOpItem>();
    COPY_OPITEN_AND_RETURN_RET_LOG(DrawColorOpItem, op.get(), opItem, nullptr, "failed to memcpy_s");
    return op;
}

void DrawColorOpItem::Playback(CanvasPlayer& player, void* opItem)
{
    if (opItem != nullptr) {
        auto* op = static_cast<DrawColorOpItem*>(opItem);
        op->Playback(player.canvas_);
    }
}

void DrawColorOpItem::Playback(Canvas& canvas)
{
    canvas.DrawColor(color_, mode_);
}

DrawImageNineOpItem::DrawImageNineOpItem() : DrawOpItem(IMAGE_NINE_OPITEM) {}
DrawImageNineOpItem::DrawImageNineOpItem(const ImageHandle& image, const RectI& center, const Rect& dst,
    FilterMode filter, const BrushHandle& brushHandle, bool hasBrush) : DrawOpItem(IMAGE_NINE_OPITEM),
    image_(std::move(image)), center_(center), dst_(dst), filter_(filter),
    brushHandle_(brushHandle), hasBrush_(hasBrush) {}

std::shared_ptr<OpItem> DrawImageNineOpItem::Unmarshalling(const CmdList& cmdList, void* opItem)
{
    CHECK_AND_RETURN_RET_LOG(opItem != nullptr, nullptr, "opItem is nullptr!");
    auto op = std::make_shared<DrawImageNineOpItem>();
    COPY_OPITEN_AND_RETURN_RET_LOG(DrawImageNineOpItem, op.get(), opItem, nullptr, "failed to memcpy_s");
    op->Unmarshalling(cmdList);
    return op;
}

void DrawImageNineOpItem::Unmarshalling(const CmdList& cmdList, Canvas* canvas)
{
    auto image = CmdListHelper::GetImageFromCmdList(cmdList, image_);
    if (image == nullptr) {
        return;
    }

    std::shared_ptr<Brush> brush = nullptr;
    if (hasBrush_) {
        auto colorSpace = CmdListHelper::GetColorSpaceFromCmdList(
            cmdList, brushHandle_.colorSpaceHandle);
        auto shaderEffect = CmdListHelper::GetShaderEffectFromCmdList(
            cmdList, brushHandle_.shaderEffectHandle);
        auto colorFilter = CmdListHelper::GetColorFilterFromCmdList(
            cmdList, brushHandle_.colorFilterHandle);
        auto imageFilter = CmdListHelper::GetImageFilterFromCmdList(
            cmdList, brushHandle_.imageFilterHandle);
        auto maskFilter = CmdListHelper::GetMaskFilterFromCmdList(
            cmdList, brushHandle_.maskFilterHandle);

        Filter filter;
        filter.SetColorFilter(colorFilter);
        filter.SetImageFilter(imageFilter);
        filter.SetMaskFilter(maskFilter);
        filter.SetFilterQuality(brushHandle_.filterQuality);

        const Color4f color4f = { brushHandle_.color.GetRedF(), brushHandle_.color.GetGreenF(),
            brushHandle_.color.GetBlueF(), brushHandle_.color.GetAlphaF() };

        brush = std::make_shared<Brush>();
        brush->SetColor(color4f, colorSpace);
        brush->SetShaderEffect(shaderEffect);
        brush->SetBlendMode(brushHandle_.mode);
        brush->SetAntiAlias(brushHandle_.isAntiAlias);
        brush->SetFilter(filter);
    }

    if (canvas) {
        canvas->DrawImageNine(image.get(), center_, dst_, filter_, brush.get());
    } else {
        playbackTask_ = [this, image, brush](Canvas& canvas) {
            canvas.DrawImageNine(image.get(), center_, dst_, filter_, brush.get());
        };
    }
}

void DrawImageNineOpItem::Playback(CanvasPlayer& player, void* opItem)
{
    if (opItem != nullptr) {
        auto* op = static_cast<DrawImageNineOpItem*>(opItem);
        op->Playback(player.canvas_, player.cmdList_);
    }
}

void DrawImageNineOpItem::Playback(Canvas& canvas, const CmdList& cmdList)
{
    if (playbackTask_) {
        playbackTask_(canvas);
    } else {
        Unmarshalling(cmdList, &canvas);
    }
}

DrawAnnotationOpItem::DrawAnnotationOpItem(const Rect& rect, const char* key, const ImageHandle& data)
    : DrawOpItem(IMAGE_ANNOTATION_OPITEM),
    rect_(rect), key_(key), data_(data) {}

std::shared_ptr<OpItem> DrawAnnotationOpItem::Unmarshalling(const CmdList& cmdList, void* opItem)
{
    CHECK_AND_RETURN_RET_LOG(opItem != nullptr, nullptr, "opItem is nullptr!");
    
    auto* opPtr = static_cast<DrawAnnotationOpItem*>(opItem);
    auto op = std::make_shared<DrawAnnotationOpItem>(opPtr->rect_, opPtr->key_, opPtr->data_);
    COPY_OPITEN_AND_RETURN_RET_LOG(DrawAnnotationOpItem, op.get(), opItem, nullptr, "failed to memcpy_s");
    op->Unmarshalling(cmdList);
    return op;
}

void DrawAnnotationOpItem::Unmarshalling(const CmdList& cmdList, Canvas* canvas)
{
    auto data = CmdListHelper::GetDataFromCmdList(cmdList, data_);
    if (data == nullptr) {
        return;
    }

    if (canvas) {
        canvas->DrawAnnotation(rect_, key_, data.get());
    } else {
        playbackTask_ = [this, data](Canvas& canvas) {
            canvas.DrawAnnotation(rect_, key_, data.get());
        };
    }
}

void DrawAnnotationOpItem::Playback(CanvasPlayer& player, void* opItem)
{
    if (opItem != nullptr) {
        auto* op = static_cast<DrawAnnotationOpItem*>(opItem);
        op->Playback(player.canvas_, player.cmdList_);
    }
}

void DrawAnnotationOpItem::Playback(Canvas& canvas, const CmdList& cmdList)
{
    if (playbackTask_) {
        playbackTask_(canvas);
    } else {
        Unmarshalling(cmdList, &canvas);
    }
}

DrawImageLatticeOpItem::DrawImageLatticeOpItem() : DrawOpItem(IMAGE_LATTICE_OPITEM) {}
DrawImageLatticeOpItem::DrawImageLatticeOpItem(const ImageHandle& image, const Lattice& lattice, const Rect& dst,
    FilterMode filter, const BrushHandle& brushHandle, bool hasBrush) : DrawOpItem(IMAGE_LATTICE_OPITEM),
    image_(std::move(image)), lattice_(lattice), dst_(dst), filter_(filter),
    brushHandle_(brushHandle), hasBrush_(hasBrush) {}

std::shared_ptr<OpItem> DrawImageLatticeOpItem::Unmarshalling(const CmdList& cmdList, void* opItem)
{
    CHECK_AND_RETURN_RET_LOG(opItem != nullptr, nullptr, "opItem is nullptr!");
    auto op = std::make_shared<DrawImageLatticeOpItem>();
    COPY_OPITEN_AND_RETURN_RET_LOG(DrawImageLatticeOpItem, op.get(), opItem, nullptr, "failed to memcpy_s");
    op->Unmarshalling(cmdList);
    return op;
}

void DrawImageLatticeOpItem::Unmarshalling(const CmdList& cmdList, Canvas* canvas)
{
    auto image = CmdListHelper::GetImageFromCmdList(cmdList, image_);
    if (image == nullptr) {
        return;
    }

    std::shared_ptr<Brush> brush = nullptr;
    if (hasBrush_) {
        auto colorSpace = CmdListHelper::GetColorSpaceFromCmdList(
            cmdList, brushHandle_.colorSpaceHandle);
        auto shaderEffect = CmdListHelper::GetShaderEffectFromCmdList(
            cmdList, brushHandle_.shaderEffectHandle);
        auto colorFilter = CmdListHelper::GetColorFilterFromCmdList(
            cmdList, brushHandle_.colorFilterHandle);
        auto imageFilter = CmdListHelper::GetImageFilterFromCmdList(
            cmdList, brushHandle_.imageFilterHandle);
        auto maskFilter = CmdListHelper::GetMaskFilterFromCmdList(
            cmdList, brushHandle_.maskFilterHandle);

        Filter filter;
        filter.SetColorFilter(colorFilter);
        filter.SetImageFilter(imageFilter);
        filter.SetMaskFilter(maskFilter);
        filter.SetFilterQuality(brushHandle_.filterQuality);

        const Color4f color4f = { brushHandle_.color.GetRedF(), brushHandle_.color.GetGreenF(),
            brushHandle_.color.GetBlueF(), brushHandle_.color.GetAlphaF() };

        brush = std::make_shared<Brush>();
        brush->SetColor(color4f, colorSpace);
        brush->SetShaderEffect(shaderEffect);
        brush->SetBlendMode(brushHandle_.mode);
        brush->SetAntiAlias(brushHandle_.isAntiAlias);
        brush->SetFilter(filter);
    }

    if (canvas) {
        canvas->DrawImageLattice(image.get(), lattice_, dst_, filter_, brush.get());
    } else {
        playbackTask_ = [this, image, brush](Canvas& canvas) {
            canvas.DrawImageLattice(image.get(), lattice_, dst_, filter_, brush.get());
        };
    }
}

void DrawImageLatticeOpItem::Playback(CanvasPlayer& player, void* opItem)
{
    if (opItem != nullptr) {
        auto* op = static_cast<DrawImageLatticeOpItem*>(opItem);
        op->Playback(player.canvas_, player.cmdList_);
    }
}

void DrawImageLatticeOpItem::Playback(Canvas& canvas, const CmdList& cmdList)
{
    if (playbackTask_) {
        playbackTask_(canvas);
    } else {
        Unmarshalling(cmdList, &canvas);
    }
}

DrawBitmapOpItem::DrawBitmapOpItem() : DrawOpItem(BITMAP_OPITEM) {}
DrawBitmapOpItem::DrawBitmapOpItem(const ImageHandle& bitmap, scalar px, scalar py)
    : DrawOpItem(BITMAP_OPITEM), bitmap_(bitmap), px_(px), py_(py) {}

std::shared_ptr<OpItem> DrawBitmapOpItem::Unmarshalling(const CmdList& cmdList, void* opItem)
{
    CHECK_AND_RETURN_RET_LOG(opItem != nullptr, nullptr, "opItem is nullptr!");
    auto op = std::make_shared<DrawBitmapOpItem>();
    COPY_OPITEN_AND_RETURN_RET_LOG(DrawBitmapOpItem, op.get(), opItem, nullptr, "failed to memcpy_s");
    op->Unmarshalling(cmdList);
    return op;
}

void DrawBitmapOpItem::Unmarshalling(const CmdList& cmdList, Canvas* canvas)
{
    auto bitmap = CmdListHelper::GetBitmapFromCmdList(cmdList, bitmap_);
    if (bitmap == nullptr) {
        return;
    }

    if (canvas) {
        canvas->DrawBitmap(*bitmap, px_, py_);
    } else {
        playbackTask_ = [this, bitmap](Canvas& canvas) {
            canvas.DrawBitmap(*bitmap, px_, py_);
        };
    }
}

void DrawBitmapOpItem::Playback(CanvasPlayer& player, void* opItem)
{
    if (opItem != nullptr) {
        auto* op = static_cast<DrawBitmapOpItem*>(opItem);
        op->Playback(player.canvas_, player.cmdList_);
    }
}

void DrawBitmapOpItem::Playback(Canvas& canvas, const CmdList& cmdList)
{
    if (playbackTask_) {
        playbackTask_(canvas);
    } else {
        Unmarshalling(cmdList, &canvas);
    }
}

DrawImageOpItem::DrawImageOpItem() : DrawOpItem(IMAGE_OPITEM) {}
DrawImageOpItem::DrawImageOpItem(const ImageHandle& image, scalar px, scalar py,
    const SamplingOptions& samplingOptions) : DrawOpItem(IMAGE_OPITEM),
    image_(std::move(image)), px_(px), py_(py), samplingOptions_(samplingOptions) {}

std::shared_ptr<OpItem> DrawImageOpItem::Unmarshalling(const CmdList& cmdList, void* opItem)
{
    CHECK_AND_RETURN_RET_LOG(opItem != nullptr, nullptr, "opItem is nullptr!");
    auto op = std::make_shared<DrawImageOpItem>();
    COPY_OPITEN_AND_RETURN_RET_LOG(DrawImageOpItem, op.get(), opItem, nullptr, "failed to memcpy_s");
    op->Unmarshalling(cmdList);
    return op;
}

void DrawImageOpItem::Unmarshalling(const CmdList& cmdList, Canvas* canvas)
{
    auto image = CmdListHelper::GetImageFromCmdList(cmdList, image_);
    if (image == nullptr) {
        return;
    }

    if (canvas) {
        canvas->DrawImage(*image, px_, py_, samplingOptions_);
    } else {
        playbackTask_ = [this, image](Canvas& canvas) {
            canvas.DrawImage(*image, px_, py_, samplingOptions_);
        };
    }
}

void DrawImageOpItem::Playback(CanvasPlayer& player, void* opItem)
{
    if (opItem != nullptr) {
        auto* op = static_cast<DrawImageOpItem*>(opItem);
        op->Playback(player.canvas_, player.cmdList_);
    }
}

void DrawImageOpItem::Playback(Canvas& canvas, const CmdList& cmdList)
{
    if (playbackTask_) {
        playbackTask_(canvas);
    } else {
        Unmarshalling(cmdList, &canvas);
    }
}

DrawImageRectOpItem::DrawImageRectOpItem() : DrawOpItem(IMAGE_RECT_OPITEM) {}
DrawImageRectOpItem::DrawImageRectOpItem(const ImageHandle& image, const Rect& src, const Rect& dst,
    const SamplingOptions& sampling, SrcRectConstraint constraint) : DrawOpItem(IMAGE_RECT_OPITEM),
    image_(image), src_(src), dst_(dst), sampling_(sampling), constraint_(constraint) {}

std::shared_ptr<OpItem> DrawImageRectOpItem::Unmarshalling(const CmdList& cmdList, void* opItem)
{
    CHECK_AND_RETURN_RET_LOG(opItem != nullptr, nullptr, "opItem is nullptr!");
    auto op = std::make_shared<DrawImageRectOpItem>();
    COPY_OPITEN_AND_RETURN_RET_LOG(DrawImageRectOpItem, op.get(), opItem, nullptr, "failed to memcpy_s");
    op->Unmarshalling(cmdList);
    return op;
}

void DrawImageRectOpItem::Unmarshalling(const CmdList& cmdList, Canvas* canvas)
{
    auto image = CmdListHelper::GetImageFromCmdList(cmdList, image_);
    if (image == nullptr) {
        return;
    }

    if (canvas) {
        canvas->DrawImageRect(*image, src_, dst_, sampling_, constraint_);
    } else {
        playbackTask_ = [this, image](Canvas& canvas) {
            canvas.DrawImageRect(*image, src_, dst_, sampling_, constraint_);
        };
    }
}

void DrawImageRectOpItem::Playback(CanvasPlayer& player, void* opItem)
{
    if (opItem != nullptr) {
        auto* op = static_cast<DrawImageRectOpItem*>(opItem);
        op->Playback(player.canvas_, player.cmdList_);
    }
}

void DrawImageRectOpItem::Playback(Canvas& canvas, const CmdList& cmdList)
{
    if (playbackTask_) {
        playbackTask_(canvas);
    } else {
        Unmarshalling(cmdList, &canvas);
    }
}

DrawPictureOpItem::DrawPictureOpItem() : DrawOpItem(PICTURE_OPITEM) {}
DrawPictureOpItem::DrawPictureOpItem(const ImageHandle& picture) : DrawOpItem(PICTURE_OPITEM), picture_(picture) {}

std::shared_ptr<OpItem> DrawPictureOpItem::Unmarshalling(const CmdList& cmdList, void* opItem)
{
    CHECK_AND_RETURN_RET_LOG(opItem != nullptr, nullptr, "opItem is nullptr!");
    auto op = std::make_shared<DrawPictureOpItem>();
    COPY_OPITEN_AND_RETURN_RET_LOG(DrawPictureOpItem, op.get(), opItem, nullptr, "failed to memcpy_s");
    op->Unmarshalling(cmdList);
    return op;
}

void DrawPictureOpItem::Unmarshalling(const CmdList& cmdList, Canvas* canvas)
{
    auto picture = CmdListHelper::GetPictureFromCmdList(cmdList, picture_);
    if (picture == nullptr) {
        return;
    }

    if (canvas) {
        canvas->DrawPicture(*picture);
    } else {
        playbackTask_ = [this, picture](Canvas& canvas) {
            canvas.DrawPicture(*picture);
        };
    }
}

void DrawPictureOpItem::Playback(CanvasPlayer& player, void* opItem)
{
    if (opItem != nullptr) {
        auto* op = static_cast<DrawPictureOpItem*>(opItem);
        op->Playback(player.canvas_, player.cmdList_);
    }
}

void DrawPictureOpItem::Playback(Canvas& canvas, const CmdList& cmdList)
{
    if (playbackTask_) {
        playbackTask_(canvas);
    } else {
        Unmarshalling(cmdList, &canvas);
    }
}

DrawTextBlobOpItem::DrawTextBlobOpItem() : DrawOpItem(TEXT_BLOB_OPITEM) {}
DrawTextBlobOpItem::DrawTextBlobOpItem(const ImageHandle& textBlob, const scalar x, const scalar y)
    : DrawOpItem(TEXT_BLOB_OPITEM), textBlob_(textBlob), x_(x), y_(y) {}

std::shared_ptr<OpItem> DrawTextBlobOpItem::Unmarshalling(const CmdList& cmdList, void* opItem)
{
    CHECK_AND_RETURN_RET_LOG(opItem != nullptr, nullptr, "opItem is nullptr!");
    auto op = std::make_shared<DrawTextBlobOpItem>();
    COPY_OPITEN_AND_RETURN_RET_LOG(DrawTextBlobOpItem, op.get(), opItem, nullptr, "failed to memcpy_s");
    op->Unmarshalling(cmdList);
    return op;
}

void DrawTextBlobOpItem::Unmarshalling(const CmdList& cmdList, Canvas* canvas)
{
    std::shared_ptr<TextBlob> textBlob = CmdListHelper::GetTextBlobFromCmdList(cmdList, textBlob_);
    if (!textBlob) {
        LOGE("textBlob nullptr, %{public}s, %{public}d", __FUNCTION__, __LINE__);
        return;
    }

    if (canvas) {
        canvas->DrawTextBlob(textBlob.get(), x_, y_);
    } else {
        playbackTask_ = [this, textBlob](Canvas& canvas) {
            canvas.DrawTextBlob(textBlob.get(), x_, y_);
        };
    }
}

void DrawTextBlobOpItem::Playback(CanvasPlayer& player, void* opItem)
{
    if (opItem != nullptr) {
        auto* op = static_cast<DrawTextBlobOpItem*>(opItem);
        op->Playback(player.canvas_, player.cmdList_);
    }
}

void DrawTextBlobOpItem::Playback(Canvas& canvas, const CmdList& cmdList)
{
    if (playbackTask_) {
        playbackTask_(canvas);
    } else {
        Unmarshalling(cmdList, &canvas);
    }
}

bool DrawTextBlobOpItem::GenerateCachedOpItem(std::shared_ptr<CmdList> cacheCmdList, const TextBlob* textBlob)
{
    if (!textBlob) {
        LOGE("textBlob nullptr, %{public}s, %{public}d", __FUNCTION__, __LINE__);
        return false;
    }

    auto bounds = textBlob->Bounds();
    if (!bounds || !bounds->IsValid()) {
        return false;
    }
    bounds->Offset(x_, y_);

    // create CPU raster surface
    Drawing::ImageInfo offscreenInfo { bounds->GetWidth(), bounds->GetHeight(),
        Drawing::COLORTYPE_RGBA_8888, Drawing::ALPHATYPE_PREMUL, nullptr};
    
    std::shared_ptr<Surface> offscreenSurface = Surface::MakeRaster(offscreenInfo);

    if (offscreenSurface == nullptr) {
        return false;
    }

    auto offscreenCanvas = offscreenSurface->GetCanvas();

    if (bounds->GetLeft() != 0 || bounds->GetTop() != 0) {
        offscreenCanvas->Translate(-bounds->GetLeft(), -bounds->GetTop());
    }

    offscreenCanvas->DrawTextBlob(textBlob, x_, y_);

    std::shared_ptr<Image> image = offscreenSurface->GetImageSnapshot();

    Drawing::Rect src(0, 0, image->GetWidth(), image->GetHeight());
    Drawing::Rect dst(bounds->GetLeft(), bounds->GetTop(), bounds->GetRight(), bounds->GetBottom());
    SamplingOptions sampling;
    auto imageHandle = CmdListHelper::AddImageToCmdList(*cacheCmdList, image);
    cacheCmdList->AddOp<DrawImageRectOpItem>(imageHandle, src, dst, sampling,
        SrcRectConstraint::FAST_SRC_RECT_CONSTRAINT);
    return true;
}

std::shared_ptr<NoIPCImageOpItem> DrawTextBlobOpItem::GenerateCachedOpItem(CmdList& cmdList, Canvas* canvas,
    AttachPenOpItem* penOpItem, AttachBrushOpItem* brushOpItem, bool addCmdList, bool& replaceSuccess)
{
    std::shared_ptr<TextBlob> textBlob = CmdListHelper::GetTextBlobFromCmdList(cmdList, textBlob_);
    if (!textBlob) {
        LOGE("textBlob nullptr, %{public}s, %{public}d", __FUNCTION__, __LINE__);
        return nullptr;
    }

    auto bounds = textBlob->Bounds();
    if (!bounds || !bounds->IsValid()) {
        return nullptr;
    }
    bounds->Offset(x_, y_);

    std::shared_ptr<Surface> offscreenSurface = nullptr;

    if (auto surface = canvas != nullptr ? canvas->GetSurface() : nullptr) {
        // create GPU accelerated surface if possible
        offscreenSurface = surface->MakeSurface(bounds->GetWidth(), bounds->GetHeight());
    } else {
        // create CPU raster surface
        Drawing::ImageInfo offscreenInfo { bounds->GetWidth(), bounds->GetHeight(),
            Drawing::COLORTYPE_RGBA_8888, Drawing::ALPHATYPE_PREMUL, nullptr};
        offscreenSurface = Surface::MakeRaster(offscreenInfo);
    }
    if (offscreenSurface == nullptr) {
        return nullptr;
    }

    auto offscreenCanvas = offscreenSurface->GetCanvas();

    // align draw op to [0, 0]
    if (bounds->GetLeft() != 0 || bounds->GetTop() != 0) {
        offscreenCanvas->Translate(-bounds->GetLeft(), -bounds->GetTop());
    }

    if (penOpItem) {
        penOpItem->Unmarshalling(cmdList, offscreenCanvas.get());
    }

    if (brushOpItem) {
        brushOpItem->Unmarshalling(cmdList, offscreenCanvas.get());
    }

    offscreenCanvas->DrawTextBlob(textBlob.get(), x_, y_);

    std::shared_ptr<Image> image = offscreenSurface->GetImageSnapshot();
    Drawing::Rect src(0, 0, image->GetWidth(), image->GetHeight());
    Drawing::Rect dst(bounds->GetLeft(), bounds->GetTop(), bounds->GetRight(), bounds->GetBottom());
    SamplingOptions sampling;
    std::shared_ptr<NoIPCImageOpItem> ipcImageOpItem = nullptr;
    if (!addCmdList) {
        ipcImageOpItem = std::make_shared<NoIPCImageOpItem>(image, src, dst, sampling,
            SrcRectConstraint::FAST_SRC_RECT_CONSTRAINT);
    } else {
        auto imageHandle = CmdListHelper::AddImageToCmdList(cmdList, image);
        cmdList.AddOp<DrawImageRectOpItem>(imageHandle, src, dst, sampling,
            SrcRectConstraint::FAST_SRC_RECT_CONSTRAINT);
        replaceSuccess = true;
    }
    
    if (penOpItem) {
        offscreenCanvas->DetachPen();
    }

    if (brushOpItem) {
        offscreenCanvas->DetachBrush();
    }

    return nullptr;
}

ClipRectOpItem::ClipRectOpItem() : DrawOpItem(CLIP_RECT_OPITEM) {}

ClipRectOpItem::ClipRectOpItem(const Rect& rect, ClipOp op, bool doAntiAlias)
    : DrawOpItem(CLIP_RECT_OPITEM), rect_(rect), clipOp_(op), doAntiAlias_(doAntiAlias) {}

std::shared_ptr<OpItem> ClipRectOpItem::Unmarshalling(const CmdList& cmdList, void* opItem)
{
    CHECK_AND_RETURN_RET_LOG(opItem != nullptr, nullptr, "opItem is nullptr!");
    auto op = std::make_shared<ClipRectOpItem>();
    COPY_OPITEN_AND_RETURN_RET_LOG(ClipRectOpItem, op.get(), opItem, nullptr, "failed to memcpy_s");
    return op;
}

void ClipRectOpItem::Playback(CanvasPlayer& player, void* opItem)
{
    if (opItem != nullptr) {
        auto* op = static_cast<ClipRectOpItem*>(opItem);
        op->Playback(player.canvas_);
    }
}

void ClipRectOpItem::Playback(Canvas& canvas)
{
    canvas.ClipRect(rect_, clipOp_, doAntiAlias_);
}

ClipIRectOpItem::ClipIRectOpItem() : DrawOpItem(CLIP_IRECT_OPITEM) {}
ClipIRectOpItem::ClipIRectOpItem(const RectI& rect, ClipOp op)
    : DrawOpItem(CLIP_IRECT_OPITEM), rect_(rect), clipOp_(op) {}

std::shared_ptr<OpItem> ClipIRectOpItem::Unmarshalling(const CmdList& cmdList, void* opItem)
{
    CHECK_AND_RETURN_RET_LOG(opItem != nullptr, nullptr, "opItem is nullptr!");
    auto op = std::make_shared<ClipIRectOpItem>();
    COPY_OPITEN_AND_RETURN_RET_LOG(ClipIRectOpItem, op.get(), opItem, nullptr, "failed to memcpy_s");
    return op;
}

void ClipIRectOpItem::Playback(CanvasPlayer& player, void* opItem)
{
    if (opItem != nullptr) {
        auto* op = static_cast<ClipIRectOpItem*>(opItem);
        op->Playback(player.canvas_);
    }
}

void ClipIRectOpItem::Playback(Canvas& canvas)
{
    canvas.ClipIRect(rect_, clipOp_);
}

ClipRoundRectOpItem::ClipRoundRectOpItem() : DrawOpItem(CLIP_ROUND_RECT_OPITEM) {}
ClipRoundRectOpItem::ClipRoundRectOpItem(std::pair<uint32_t, size_t> radiusXYData, const Rect& rect, ClipOp op,
    bool doAntiAlias) : DrawOpItem(CLIP_ROUND_RECT_OPITEM), radiusXYData_(radiusXYData), rect_(rect), clipOp_(op),
    doAntiAlias_(doAntiAlias) {}

std::shared_ptr<OpItem> ClipRoundRectOpItem::Unmarshalling(const CmdList& cmdList, void* opItem)
{
    CHECK_AND_RETURN_RET_LOG(opItem != nullptr, nullptr, "opItem is nullptr!");
    auto op = std::make_shared<ClipRoundRectOpItem>();
    COPY_OPITEN_AND_RETURN_RET_LOG(ClipRoundRectOpItem, op.get(), opItem, nullptr, "failed to memcpy_s");
    op->Unmarshalling(cmdList);
    return op;
}

void ClipRoundRectOpItem::Unmarshalling(const CmdList& cmdList, Canvas* canvas)
{
    auto radiusXYData = CmdListHelper::GetVectorFromCmdList<Point>(cmdList, radiusXYData_);
    RoundRect roundRect(rect_, radiusXYData);

    if (canvas) {
        canvas->ClipRoundRect(roundRect, clipOp_, doAntiAlias_);
    } else {
        playbackTask_ = [this, roundRect](Canvas& canvas) {
            canvas.ClipRoundRect(roundRect, clipOp_, doAntiAlias_);
        };
    }
}

void ClipRoundRectOpItem::Playback(CanvasPlayer& player, void* opItem)
{
    if (opItem != nullptr) {
        auto* op = static_cast<ClipRoundRectOpItem*>(opItem);
        op->Playback(player.canvas_, player.cmdList_);
    }
}

void ClipRoundRectOpItem::Playback(Canvas& canvas, const CmdList& cmdList)
{
    if (playbackTask_) {
        playbackTask_(canvas);
    } else {
        Unmarshalling(cmdList, &canvas);
    }
}

ClipPathOpItem::ClipPathOpItem() : DrawOpItem(CLIP_PATH_OPITEM) {}
ClipPathOpItem::ClipPathOpItem(const CmdListHandle& path, ClipOp clipOp, bool doAntiAlias)
    : DrawOpItem(CLIP_PATH_OPITEM), path_(path), clipOp_(clipOp), doAntiAlias_(doAntiAlias) {}

std::shared_ptr<OpItem> ClipPathOpItem::Unmarshalling(const CmdList& cmdList, void* opItem)
{
    CHECK_AND_RETURN_RET_LOG(opItem != nullptr, nullptr, "opItem is nullptr!");
    auto op = std::make_shared<ClipPathOpItem>();
    COPY_OPITEN_AND_RETURN_RET_LOG(ClipPathOpItem, op.get(), opItem, nullptr, "failed to memcpy_s");
    op->Unmarshalling(cmdList);
    return op;
}

void ClipPathOpItem::Unmarshalling(const CmdList& cmdList, Canvas* canvas)
{
    auto path = CmdListHelper::GetFromCmdList<PathCmdList, Path>(cmdList, path_);
    if (path == nullptr) {
        LOGE("path is nullptr!");
        return;
    }

    if (canvas) {
        canvas->ClipPath(*path, clipOp_, doAntiAlias_);
    } else {
        playbackTask_ = [this, path](Canvas& canvas) {
            canvas.ClipPath(*path, clipOp_, doAntiAlias_);
        };
    }
}

void ClipPathOpItem::Playback(CanvasPlayer& player, void* opItem)
{
    if (opItem != nullptr) {
        auto* op = static_cast<ClipPathOpItem*>(opItem);
        op->Playback(player.canvas_, player.cmdList_);
    }
}

void ClipPathOpItem::Playback(Canvas& canvas, const CmdList& cmdList)
{
    if (playbackTask_) {
        playbackTask_(canvas);
    } else {
        Unmarshalling(cmdList, &canvas);
    }
}

ClipRegionOpItem::ClipRegionOpItem() : DrawOpItem(CLIP_REGION_OPITEM) {}
ClipRegionOpItem::ClipRegionOpItem(const CmdListHandle& region, ClipOp clipOp)
    : DrawOpItem(CLIP_REGION_OPITEM), region_(region), clipOp_(clipOp) {}

std::shared_ptr<OpItem> ClipRegionOpItem::Unmarshalling(const CmdList& cmdList, void* opItem)
{
    CHECK_AND_RETURN_RET_LOG(opItem != nullptr, nullptr, "opItem is nullptr!");
    auto op = std::make_shared<ClipRegionOpItem>();
    COPY_OPITEN_AND_RETURN_RET_LOG(ClipRegionOpItem, op.get(), opItem, nullptr, "failed to memcpy_s");
    op->Unmarshalling(cmdList);
    return op;
}

void ClipRegionOpItem::Unmarshalling(const CmdList& cmdList, Canvas* canvas)
{
    auto region = CmdListHelper::GetFromCmdList<RegionCmdList, Region>(cmdList, region_);
    if (region == nullptr) {
        LOGE("region is nullptr!");
        return;
    }

    if (canvas) {
        canvas->ClipRegion(*region, clipOp_);
    } else {
        playbackTask_ = [this, region](Canvas& canvas) {
            canvas.ClipRegion(*region, clipOp_);
        };
    }
}

void ClipRegionOpItem::Playback(CanvasPlayer& player, void* opItem)
{
    if (opItem != nullptr) {
        auto* op = static_cast<ClipRegionOpItem*>(opItem);
        op->Playback(player.canvas_, player.cmdList_);
    }
}

void ClipRegionOpItem::Playback(Canvas& canvas, const CmdList& cmdList)
{
    if (playbackTask_) {
        playbackTask_(canvas);
    } else {
        Unmarshalling(cmdList, &canvas);
    }
}

SetMatrixOpItem::SetMatrixOpItem() : DrawOpItem(SET_MATRIX_OPITEM) {}
SetMatrixOpItem::SetMatrixOpItem(const Matrix& matrix) : DrawOpItem(SET_MATRIX_OPITEM)
{
    matrix.GetAll(matrixBuffer_);
}

std::shared_ptr<OpItem> SetMatrixOpItem::Unmarshalling(const CmdList& cmdList, void* opItem)
{
    CHECK_AND_RETURN_RET_LOG(opItem != nullptr, nullptr, "opItem is nullptr!");
    auto op = std::make_shared<SetMatrixOpItem>();
    COPY_OPITEN_AND_RETURN_RET_LOG(SetMatrixOpItem, op.get(), opItem, nullptr, "failed to memcpy_s");
    return op;
}

void SetMatrixOpItem::Playback(CanvasPlayer& player, void* opItem)
{
    if (opItem != nullptr) {
        auto* op = static_cast<SetMatrixOpItem*>(opItem);
        op->Playback(player.canvas_);
    }
}

void SetMatrixOpItem::Playback(Canvas& canvas)
{
    Matrix matrix;
    for (uint32_t i = 0; i < matrixBuffer_.size(); i++) {
        matrix.Set(static_cast<Matrix::Index>(i), matrixBuffer_[i]);
    }

    canvas.SetMatrix(matrix);
}

ResetMatrixOpItem::ResetMatrixOpItem() : DrawOpItem(RESET_MATRIX_OPITEM) {}

std::shared_ptr<OpItem> ResetMatrixOpItem::Unmarshalling(const CmdList& cmdList, void* opItem)
{
    CHECK_AND_RETURN_RET_LOG(opItem != nullptr, nullptr, "opItem is nullptr!");
    auto op = std::make_shared<ResetMatrixOpItem>();
    COPY_OPITEN_AND_RETURN_RET_LOG(ResetMatrixOpItem, op.get(), opItem, nullptr, "failed to memcpy_s");
    return op;
}

void ResetMatrixOpItem::Playback(CanvasPlayer& player, void* opItem)
{
    if (opItem != nullptr) {
        auto* op = static_cast<ResetMatrixOpItem*>(opItem);
        op->Playback(player.canvas_);
    }
}

void ResetMatrixOpItem::Playback(Canvas& canvas)
{
    canvas.ResetMatrix();
}

ConcatMatrixOpItem::ConcatMatrixOpItem() : DrawOpItem(CONCAT_MATRIX_OPITEM) {}
ConcatMatrixOpItem::ConcatMatrixOpItem(const Matrix& matrix) : DrawOpItem(CONCAT_MATRIX_OPITEM)
{
    matrix.GetAll(matrixBuffer_);
}

std::shared_ptr<OpItem> ConcatMatrixOpItem::Unmarshalling(const CmdList& cmdList, void* opItem)
{
    CHECK_AND_RETURN_RET_LOG(opItem != nullptr, nullptr, "opItem is nullptr!");
    auto op = std::make_shared<ConcatMatrixOpItem>();
    COPY_OPITEN_AND_RETURN_RET_LOG(ConcatMatrixOpItem, op.get(), opItem, nullptr, "failed to memcpy_s");
    return op;
}

void ConcatMatrixOpItem::Playback(CanvasPlayer& player, void* opItem)
{
    if (opItem != nullptr) {
        auto* op = static_cast<ConcatMatrixOpItem*>(opItem);
        op->Playback(player.canvas_);
    }
}

void ConcatMatrixOpItem::Playback(Canvas& canvas)
{
    Matrix matrix;
    for (uint32_t i = 0; i < matrixBuffer_.size(); i++) {
        matrix.Set(static_cast<Matrix::Index>(i), matrixBuffer_[i]);
    }

    canvas.ConcatMatrix(matrix);
}

TranslateOpItem::TranslateOpItem() : DrawOpItem(TRANSLATE_OPITEM) {}
TranslateOpItem::TranslateOpItem(scalar dx, scalar dy) : DrawOpItem(TRANSLATE_OPITEM), dx_(dx), dy_(dy) {}

std::shared_ptr<OpItem> TranslateOpItem::Unmarshalling(const CmdList& cmdList, void* opItem)
{
    CHECK_AND_RETURN_RET_LOG(opItem != nullptr, nullptr, "opItem is nullptr!");
    auto op = std::make_shared<TranslateOpItem>();
    COPY_OPITEN_AND_RETURN_RET_LOG(TranslateOpItem, op.get(), opItem, nullptr, "failed to memcpy_s");
    return op;
}

void TranslateOpItem::Playback(CanvasPlayer& player, void* opItem)
{
    if (opItem != nullptr) {
        auto* op = static_cast<TranslateOpItem*>(opItem);
        op->Playback(player.canvas_);
    }
}

void TranslateOpItem::Playback(Canvas& canvas)
{
    canvas.Translate(dx_, dy_);
}

ScaleOpItem::ScaleOpItem() : DrawOpItem(SCALE_OPITEM) {}
ScaleOpItem::ScaleOpItem(scalar sx, scalar sy) : DrawOpItem(SCALE_OPITEM), sx_(sx), sy_(sy) {}

std::shared_ptr<OpItem> ScaleOpItem::Unmarshalling(const CmdList& cmdList, void* opItem)
{
    CHECK_AND_RETURN_RET_LOG(opItem != nullptr, nullptr, "opItem is nullptr!");
    auto op = std::make_shared<ScaleOpItem>();
    COPY_OPITEN_AND_RETURN_RET_LOG(ScaleOpItem, op.get(), opItem, nullptr, "failed to memcpy_s");
    return op;
}

void ScaleOpItem::Playback(CanvasPlayer& player, void* opItem)
{
    if (opItem != nullptr) {
        auto* op = static_cast<ScaleOpItem*>(opItem);
        op->Playback(player.canvas_);
    }
}

void ScaleOpItem::Playback(Canvas& canvas)
{
    canvas.Scale(sx_, sy_);
}

RotateOpItem::RotateOpItem() : DrawOpItem(ROTATE_OPITEM) {}
RotateOpItem::RotateOpItem(scalar deg, scalar sx, scalar sy) : DrawOpItem(ROTATE_OPITEM), deg_(deg), sx_(sx), sy_(sy) {}

std::shared_ptr<OpItem> RotateOpItem::Unmarshalling(const CmdList& cmdList, void* opItem)
{
    CHECK_AND_RETURN_RET_LOG(opItem != nullptr, nullptr, "opItem is nullptr!");
    auto op = std::make_shared<RotateOpItem>();
    COPY_OPITEN_AND_RETURN_RET_LOG(RotateOpItem, op.get(), opItem, nullptr, "failed to memcpy_s");
    return op;
}

void RotateOpItem::Playback(CanvasPlayer& player, void* opItem)
{
    if (opItem != nullptr) {
        auto* op = static_cast<RotateOpItem*>(opItem);
        op->Playback(player.canvas_);
    }
}

void RotateOpItem::Playback(Canvas& canvas)
{
    canvas.Rotate(deg_, sx_, sy_);
}

ShearOpItem::ShearOpItem() : DrawOpItem(SHEAR_OPITEM) {}
ShearOpItem::ShearOpItem(scalar sx, scalar sy) : DrawOpItem(SHEAR_OPITEM), sx_(sx), sy_(sy) {}

std::shared_ptr<OpItem> ShearOpItem::Unmarshalling(const CmdList& cmdList, void* opItem)
{
    CHECK_AND_RETURN_RET_LOG(opItem != nullptr, nullptr, "opItem is nullptr!");
    auto op = std::make_shared<ShearOpItem>();
    COPY_OPITEN_AND_RETURN_RET_LOG(ShearOpItem, op.get(), opItem, nullptr, "failed to memcpy_s");
    return op;
}

void ShearOpItem::Playback(CanvasPlayer& player, void* opItem)
{
    if (opItem != nullptr) {
        auto* op = static_cast<ShearOpItem*>(opItem);
        op->Playback(player.canvas_);
    }
}

void ShearOpItem::Playback(Canvas& canvas)
{
    canvas.Shear(sx_, sy_);
}

FlushOpItem::FlushOpItem() : DrawOpItem(FLUSH_OPITEM) {}

std::shared_ptr<OpItem> FlushOpItem::Unmarshalling(const CmdList& cmdList, void* opItem)
{
    CHECK_AND_RETURN_RET_LOG(opItem != nullptr, nullptr, "opItem is nullptr!");
    auto op = std::make_shared<FlushOpItem>();
    COPY_OPITEN_AND_RETURN_RET_LOG(FlushOpItem, op.get(), opItem, nullptr, "failed to memcpy_s");
    return op;
}

void FlushOpItem::Playback(CanvasPlayer& player, void* opItem)
{
    if (opItem != nullptr) {
        auto* op = static_cast<FlushOpItem*>(opItem);
        op->Playback(player.canvas_);
    }
}

void FlushOpItem::Playback(Canvas& canvas)
{
    canvas.Flush();
}

ClearOpItem::ClearOpItem() : DrawOpItem(CLEAR_OPITEM) {}
ClearOpItem::ClearOpItem(ColorQuad color) : DrawOpItem(CLEAR_OPITEM), color_(color) {}

std::shared_ptr<OpItem> ClearOpItem::Unmarshalling(const CmdList& cmdList, void* opItem)
{
    CHECK_AND_RETURN_RET_LOG(opItem != nullptr, nullptr, "opItem is nullptr!");
    auto op = std::make_shared<ClearOpItem>();
    COPY_OPITEN_AND_RETURN_RET_LOG(ClearOpItem, op.get(), opItem, nullptr, "failed to memcpy_s");
    return op;
}

void ClearOpItem::Playback(CanvasPlayer& player, void* opItem)
{
    if (opItem != nullptr) {
        auto* op = static_cast<ClearOpItem*>(opItem);
        op->Playback(player.canvas_);
    }
}

void ClearOpItem::Playback(Canvas& canvas)
{
    canvas.Clear(color_);
}

SaveOpItem::SaveOpItem() : DrawOpItem(SAVE_OPITEM) {}

std::shared_ptr<OpItem> SaveOpItem::Unmarshalling(const CmdList& cmdList, void* opItem)
{
    CHECK_AND_RETURN_RET_LOG(opItem != nullptr, nullptr, "opItem is nullptr!");
    auto op = std::make_shared<SaveOpItem>();
    COPY_OPITEN_AND_RETURN_RET_LOG(SaveOpItem, op.get(), opItem, nullptr, "failed to memcpy_s");
    return op;
}

void SaveOpItem::Playback(CanvasPlayer& player, void* opItem)
{
    if (opItem != nullptr) {
        auto* op = static_cast<SaveOpItem*>(opItem);
        op->Playback(player.canvas_);
    }
}

void SaveOpItem::Playback(Canvas& canvas)
{
    canvas.Save();
}

SaveLayerOpItem::SaveLayerOpItem() : DrawOpItem(SAVE_LAYER_OPITEM) {}
SaveLayerOpItem::SaveLayerOpItem(const Rect& rect, bool hasBrush, const BrushHandle& brushHandle,
    const CmdListHandle& imageFilter, uint32_t saveLayerFlags) : DrawOpItem(SAVE_LAYER_OPITEM),
    rect_(rect), hasBrush_(hasBrush), brushHandle_(brushHandle), imageFilter_(imageFilter),
    saveLayerFlags_(saveLayerFlags) {}

std::shared_ptr<OpItem> SaveLayerOpItem::Unmarshalling(const CmdList& cmdList, void* opItem)
{
    CHECK_AND_RETURN_RET_LOG(opItem != nullptr, nullptr, "opItem is nullptr!");
    auto op = std::make_shared<SaveLayerOpItem>();
    COPY_OPITEN_AND_RETURN_RET_LOG(SaveLayerOpItem, op.get(), opItem, nullptr, "failed to memcpy_s");
    op->Unmarshalling(cmdList);
    return op;
}

void SaveLayerOpItem::Unmarshalling(const CmdList& cmdList, Canvas* canvas)
{
    std::shared_ptr<Brush> brush = nullptr;
    if (hasBrush_) {
        auto colorSpace = CmdListHelper::GetColorSpaceFromCmdList(
            cmdList, brushHandle_.colorSpaceHandle);
        auto shaderEffect = CmdListHelper::GetShaderEffectFromCmdList(
            cmdList, brushHandle_.shaderEffectHandle);
        auto colorFilter = CmdListHelper::GetColorFilterFromCmdList(
            cmdList, brushHandle_.colorFilterHandle);
        auto imageFilter = CmdListHelper::GetImageFilterFromCmdList(
            cmdList, brushHandle_.imageFilterHandle);
        auto maskFilter = CmdListHelper::GetMaskFilterFromCmdList(
            cmdList, brushHandle_.maskFilterHandle);

        Filter filter;
        filter.SetColorFilter(colorFilter);
        filter.SetImageFilter(imageFilter);
        filter.SetMaskFilter(maskFilter);
        filter.SetFilterQuality(brushHandle_.filterQuality);

        const Color4f color4f = { brushHandle_.color.GetRedF(), brushHandle_.color.GetGreenF(),
            brushHandle_.color.GetBlueF(), brushHandle_.color.GetAlphaF() };

        brush = std::make_shared<Brush>();
        brush->SetColor(color4f, colorSpace);
        brush->SetShaderEffect(shaderEffect);
        brush->SetBlendMode(brushHandle_.mode);
        brush->SetAntiAlias(brushHandle_.isAntiAlias);
        brush->SetFilter(filter);
    }

    auto saveImageFilter = CmdListHelper::GetFromCmdList<ImageFilterCmdList, ImageFilter>(cmdList, imageFilter_);

    if (canvas) {
        const Rect* rectPtr = nullptr;
        if (rect_.IsValid()) {
            rectPtr = &rect_;
        }
        SaveLayerOps slo(rectPtr, brush.get(), saveImageFilter.get(), saveLayerFlags_);
        canvas->SaveLayer(slo);
    } else {
        playbackTask_ = [this, brush, saveImageFilter](Canvas& canvas) {
            const Rect* rectPtr = nullptr;
            if (rect_.IsValid()) {
                rectPtr = &rect_;
            }
            SaveLayerOps slo(rectPtr, brush.get(), saveImageFilter.get(), saveLayerFlags_);
            canvas.SaveLayer(slo);
        };
    }
}

void SaveLayerOpItem::Playback(CanvasPlayer& player, void* opItem)
{
    if (opItem != nullptr) {
        auto* op = static_cast<SaveLayerOpItem*>(opItem);
        op->Playback(player.canvas_, player.cmdList_);
    }
}

void SaveLayerOpItem::Playback(Canvas& canvas, const CmdList& cmdList)
{
    if (playbackTask_) {
        playbackTask_(canvas);
    } else {
        Unmarshalling(cmdList, &canvas);
    }
}

RestoreOpItem::RestoreOpItem() : DrawOpItem(RESTORE_OPITEM) {}

std::shared_ptr<OpItem> RestoreOpItem::Unmarshalling(const CmdList& cmdList, void* opItem)
{
    CHECK_AND_RETURN_RET_LOG(opItem != nullptr, nullptr, "opItem is nullptr!");
    auto op = std::make_shared<RestoreOpItem>();
    COPY_OPITEN_AND_RETURN_RET_LOG(RestoreOpItem, op.get(), opItem, nullptr, "failed to memcpy_s");
    return op;
}

void RestoreOpItem::Playback(CanvasPlayer& player, void* opItem)
{
    if (opItem != nullptr) {
        auto* op = static_cast<RestoreOpItem*>(opItem);
        op->Playback(player.canvas_);
    }
}

void RestoreOpItem::Playback(Canvas& canvas)
{
    canvas.Restore();
}

DiscardOpItem::DiscardOpItem() : DrawOpItem(DISCARD_OPITEM) {}

std::shared_ptr<OpItem> DiscardOpItem::Unmarshalling(const CmdList& cmdList, void* opItem)
{
    CHECK_AND_RETURN_RET_LOG(opItem != nullptr, nullptr, "opItem is nullptr!");
    auto op = std::make_shared<DiscardOpItem>();
    COPY_OPITEN_AND_RETURN_RET_LOG(DiscardOpItem, op.get(), opItem, nullptr, "failed to memcpy_s");
    return op;
}

void DiscardOpItem::Playback(CanvasPlayer& player, void* opItem)
{
    if (opItem != nullptr) {
        auto* op = static_cast<DiscardOpItem*>(opItem);
        op->Playback(player.canvas_);
    }
}

void DiscardOpItem::Playback(Canvas& canvas)
{
    canvas.Discard();
}

AttachPenOpItem::AttachPenOpItem() : DrawOpItem(ATTACH_PEN_OPITEM) {}
AttachPenOpItem::AttachPenOpItem(const PenHandle& penHandle) : DrawOpItem(ATTACH_PEN_OPITEM), penHandle_(penHandle) {}

std::shared_ptr<OpItem> AttachPenOpItem::Unmarshalling(const CmdList& cmdList, void* opItem)
{
    CHECK_AND_RETURN_RET_LOG(opItem != nullptr, nullptr, "opItem is nullptr!");
    auto op = std::make_shared<AttachPenOpItem>();
    COPY_OPITEN_AND_RETURN_RET_LOG(AttachPenOpItem, op.get(), opItem, nullptr, "failed to memcpy_s");
    op->Unmarshalling(cmdList);
    return op;
}

void AttachPenOpItem::Unmarshalling(const CmdList& cmdList, Canvas* canvas)
{
    auto pathEffect = CmdListHelper::GetPathEffectFromCmdList(
        cmdList, penHandle_.pathEffectHandle);
    auto colorSpace = CmdListHelper::GetColorSpaceFromCmdList(
        cmdList, penHandle_.colorSpaceHandle);
    auto shaderEffect = CmdListHelper::GetShaderEffectFromCmdList(
        cmdList, penHandle_.shaderEffectHandle);
    auto colorFilter = CmdListHelper::GetColorFilterFromCmdList(
        cmdList, penHandle_.colorFilterHandle);
    auto imageFilter = CmdListHelper::GetImageFilterFromCmdList(
        cmdList, penHandle_.imageFilterHandle);
    auto maskFilter = CmdListHelper::GetMaskFilterFromCmdList(
        cmdList, penHandle_.maskFilterHandle);

    Filter filter;
    filter.SetColorFilter(colorFilter);
    filter.SetImageFilter(imageFilter);
    filter.SetMaskFilter(maskFilter);
    filter.SetFilterQuality(penHandle_.filterQuality);

    const Color4f color4f = { penHandle_.color.GetRedF(), penHandle_.color.GetGreenF(),
        penHandle_.color.GetBlueF(), penHandle_.color.GetAlphaF() };
    Pen pen;
    pen.SetPathEffect(pathEffect);
    pen.SetColor(color4f, colorSpace);
    pen.SetShaderEffect(shaderEffect);
    pen.SetWidth(penHandle_.width);
    pen.SetMiterLimit(penHandle_.miterLimit);
    pen.SetCapStyle(penHandle_.capStyle);
    pen.SetJoinStyle(penHandle_.joinStyle);
    pen.SetColor(penHandle_.color);
    pen.SetBlendMode(penHandle_.mode);
    pen.SetAntiAlias(penHandle_.isAntiAlias);
    pen.SetFilter(filter);

    if (canvas) {
        canvas->AttachPen(pen);
    } else {
        playbackTask_ = [this, pen](Canvas& canvas) {
            canvas.AttachPen(pen);
        };
    }
}

void AttachPenOpItem::Playback(CanvasPlayer& player, void* opItem)
{
    if (opItem != nullptr) {
        auto* op = static_cast<AttachPenOpItem*>(opItem);
        op->Playback(player.canvas_, player.cmdList_);
    }
}

void AttachPenOpItem::Playback(Canvas& canvas, const CmdList& cmdList)
{
    if (playbackTask_) {
        playbackTask_(canvas);
    } else {
        Unmarshalling(cmdList, &canvas);
    }
}

AttachBrushOpItem::AttachBrushOpItem() : DrawOpItem(ATTACH_BRUSH_OPITEM) {}
AttachBrushOpItem::AttachBrushOpItem(const BrushHandle& brushHandle)
    : DrawOpItem(ATTACH_BRUSH_OPITEM), brushHandle_(brushHandle) {}

std::shared_ptr<OpItem> AttachBrushOpItem::Unmarshalling(const CmdList& cmdList, void* opItem)
{
    CHECK_AND_RETURN_RET_LOG(opItem != nullptr, nullptr, "opItem is nullptr!");
    auto op = std::make_shared<AttachBrushOpItem>();
    COPY_OPITEN_AND_RETURN_RET_LOG(AttachBrushOpItem, op.get(), opItem, nullptr, "failed to memcpy_s");
    op->Unmarshalling(cmdList);
    return op;
}

void AttachBrushOpItem::Unmarshalling(const CmdList& cmdList, Canvas* canvas)
{
    auto colorSpace = CmdListHelper::GetColorSpaceFromCmdList(
        cmdList, brushHandle_.colorSpaceHandle);
    auto shaderEffect = CmdListHelper::GetShaderEffectFromCmdList(
        cmdList, brushHandle_.shaderEffectHandle);
    auto colorFilter = CmdListHelper::GetColorFilterFromCmdList(
        cmdList, brushHandle_.colorFilterHandle);
    auto imageFilter = CmdListHelper::GetImageFilterFromCmdList(
        cmdList, brushHandle_.imageFilterHandle);
    auto maskFilter = CmdListHelper::GetMaskFilterFromCmdList(
        cmdList, brushHandle_.maskFilterHandle);

    Filter filter;
    filter.SetColorFilter(colorFilter);
    filter.SetImageFilter(imageFilter);
    filter.SetMaskFilter(maskFilter);
    filter.SetFilterQuality(brushHandle_.filterQuality);

    const Color4f color4f = { brushHandle_.color.GetRedF(), brushHandle_.color.GetGreenF(),
        brushHandle_.color.GetBlueF(), brushHandle_.color.GetAlphaF() };
    Brush brush;
    brush.SetColor(color4f, colorSpace);
    brush.SetShaderEffect(shaderEffect);
    brush.SetBlendMode(brushHandle_.mode);
    brush.SetAntiAlias(brushHandle_.isAntiAlias);
    brush.SetFilter(filter);

    if (canvas) {
        canvas->AttachBrush(brush);
    } else {
        playbackTask_ = [this, brush](Canvas& canvas) {
            canvas.AttachBrush(brush);
        };
    }
}

void AttachBrushOpItem::Playback(CanvasPlayer& player, void* opItem)
{
    if (opItem != nullptr) {
        auto* op = static_cast<AttachBrushOpItem*>(opItem);
        op->Playback(player.canvas_, player.cmdList_);
    }
}

void AttachBrushOpItem::Playback(Canvas& canvas, const CmdList& cmdList)
{
    if (playbackTask_) {
        playbackTask_(canvas);
    } else {
        Unmarshalling(cmdList, &canvas);
    }
}

DetachPenOpItem::DetachPenOpItem() : DrawOpItem(DETACH_PEN_OPITEM) {}

std::shared_ptr<OpItem> DetachPenOpItem::Unmarshalling(const CmdList& cmdList, void* opItem)
{
    CHECK_AND_RETURN_RET_LOG(opItem != nullptr, nullptr, "opItem is nullptr!");
    auto op = std::make_shared<DetachPenOpItem>();
    COPY_OPITEN_AND_RETURN_RET_LOG(DetachPenOpItem, op.get(), opItem, nullptr, "failed to memcpy_s");
    return op;
}

void DetachPenOpItem::Playback(CanvasPlayer& player, void* opItem)
{
    if (opItem != nullptr) {
        auto* op = static_cast<DetachPenOpItem*>(opItem);
        op->Playback(player.canvas_);
    }
}

void DetachPenOpItem::Playback(Canvas& canvas)
{
    canvas.DetachPen();
}

DetachBrushOpItem::DetachBrushOpItem() : DrawOpItem(DETACH_BRUSH_OPITEM) {}

std::shared_ptr<OpItem> DetachBrushOpItem::Unmarshalling(const CmdList& cmdList, void* opItem)
{
    CHECK_AND_RETURN_RET_LOG(opItem != nullptr, nullptr, "opItem is nullptr!");
    auto op = std::make_shared<DetachBrushOpItem>();
    COPY_OPITEN_AND_RETURN_RET_LOG(DetachBrushOpItem, op.get(), opItem, nullptr, "failed to memcpy_s");
    return op;
}

void DetachBrushOpItem::Playback(CanvasPlayer& player, void* opItem)
{
    if (opItem != nullptr) {
        auto* op = static_cast<DetachBrushOpItem*>(opItem);
        op->Playback(player.canvas_);
    }
}

void DetachBrushOpItem::Playback(Canvas& canvas)
{
    canvas.DetachBrush();
}

ClipAdaptiveRoundRectOpItem::ClipAdaptiveRoundRectOpItem() : DrawOpItem(CLIP_ADAPTIVE_ROUND_RECT_OPITEM) {}
ClipAdaptiveRoundRectOpItem::ClipAdaptiveRoundRectOpItem(const std::pair<uint32_t, size_t>& radiusData)
    : DrawOpItem(CLIP_ADAPTIVE_ROUND_RECT_OPITEM), radiusData_(radiusData) {}

std::shared_ptr<OpItem> ClipAdaptiveRoundRectOpItem::Unmarshalling(const CmdList& cmdList, void* opItem)
{
    CHECK_AND_RETURN_RET_LOG(opItem != nullptr, nullptr, "opItem is nullptr!");
    auto op = std::make_shared<ClipAdaptiveRoundRectOpItem>();
    COPY_OPITEN_AND_RETURN_RET_LOG(ClipAdaptiveRoundRectOpItem, op.get(), opItem, nullptr, "failed to memcpy_s");
    op->Unmarshalling(cmdList);
    return op;
}

void ClipAdaptiveRoundRectOpItem::Unmarshalling(const CmdList& cmdList)
{
    auto radius = CmdListHelper::GetVectorFromCmdList<Point>(cmdList, radiusData_);

    playbackTask_ = [this, radius](Canvas& canvas, const Rect& rect) {
        auto roundRect = RoundRect(rect, radius);
        canvas.ClipRoundRect(roundRect, ClipOp::INTERSECT, true);
    };
}

void ClipAdaptiveRoundRectOpItem::Playback(CanvasPlayer& player, void* opItem)
{
    if (opItem != nullptr) {
        auto* op = static_cast<ClipAdaptiveRoundRectOpItem*>(opItem);
        op->Playback(player.canvas_, player.cmdList_, player.rect_);
    }
}

void ClipAdaptiveRoundRectOpItem::Playback(Canvas& canvas, const CmdList& cmdList, const Rect& rect)
{
    if (playbackTask_) {
        playbackTask_(canvas, rect);
    } else {
        auto radius = CmdListHelper::GetVectorFromCmdList<Point>(cmdList, radiusData_);
        auto roundRect = RoundRect(rect, radius);
        canvas.ClipRoundRect(roundRect, ClipOp::INTERSECT, true);
    }
}

DrawAdaptiveImageOpItem::DrawAdaptiveImageOpItem() : DrawOpItem(ADAPTIVE_IMAGE_OPITEM) {}
DrawAdaptiveImageOpItem::DrawAdaptiveImageOpItem(const ImageHandle& image, const AdaptiveImageInfo& rsImageInfo,
    const SamplingOptions& sampling, const bool isImage) : DrawOpItem(ADAPTIVE_IMAGE_OPITEM),
    image_(image), rsImageInfo_(rsImageInfo), sampling_(sampling), isImage_(isImage) {}

std::shared_ptr<OpItem> DrawAdaptiveImageOpItem::Unmarshalling(const CmdList& cmdList, void* opItem)
{
    CHECK_AND_RETURN_RET_LOG(opItem != nullptr, nullptr, "opItem is nullptr!");
    auto op = std::make_shared<DrawAdaptiveImageOpItem>();
    COPY_OPITEN_AND_RETURN_RET_LOG(DrawAdaptiveImageOpItem, op.get(), opItem, nullptr, "failed to memcpy_s");
    op->Unmarshalling(cmdList);
    return op;
}

void DrawAdaptiveImageOpItem::Unmarshalling(const CmdList& cmdList)
{
    if (isImage_) {
        auto image = CmdListHelper::GetImageFromCmdList(cmdList, image_);
        if (image == nullptr) {
        LOGE("image is nullptr!");
            return;
        }
        playbackTask_ = [this, image](Canvas& canvas, const Rect& rect) {
            AdaptiveImageHelper::DrawImage(canvas, rect, image, rsImageInfo_, sampling_);
        };
        return;
    }
    auto data = CmdListHelper::GetCompressDataFromCmdList(cmdList, image_);
    if (data == nullptr) {
        LOGE("compress data is nullptr!");
        return;
    }

    playbackTask_ = [this, data](Canvas& canvas, const Rect& rect) {
        AdaptiveImageHelper::DrawImage(canvas, rect, data, rsImageInfo_, sampling_);
    };
}

void DrawAdaptiveImageOpItem::Playback(CanvasPlayer& player, void* opItem)
{
    if (opItem != nullptr) {
        auto* op = static_cast<DrawAdaptiveImageOpItem*>(opItem);
        op->Playback(player.canvas_, player.cmdList_, player.rect_);
    }
}

void DrawAdaptiveImageOpItem::Playback(Canvas& canvas, const CmdList& cmdList, const Rect& rect)
{
    if (playbackTask_) {
        playbackTask_(canvas, rect);
        return;
    }

    if (isImage_) {
        auto image = CmdListHelper::GetImageFromCmdList(cmdList, image_);
        if (image == nullptr) {
        LOGE("image is nullptr!");
            return;
        }
        AdaptiveImageHelper::DrawImage(canvas, rect, image, rsImageInfo_, sampling_);
        return;
    }
    auto data = CmdListHelper::GetCompressDataFromCmdList(cmdList, image_);
    if (data == nullptr) {
        LOGE("compress data is nullptr!");
        return;
    }

    AdaptiveImageHelper::DrawImage(canvas, rect, data, rsImageInfo_, sampling_);
}

DrawAdaptivePixelMapOpItem::DrawAdaptivePixelMapOpItem() : DrawOpItem(ADAPTIVE_PIXELMAP_OPITEM) {}
DrawAdaptivePixelMapOpItem::DrawAdaptivePixelMapOpItem(const ImageHandle& pixelMap, const AdaptiveImageInfo& imageInfo,
    const SamplingOptions& smapling) : DrawOpItem(ADAPTIVE_PIXELMAP_OPITEM), pixelMap_(pixelMap),
    imageInfo_(imageInfo), smapling_(smapling) {}

std::shared_ptr<OpItem> DrawAdaptivePixelMapOpItem::Unmarshalling(const CmdList& cmdList, void* opItem)
{
    CHECK_AND_RETURN_RET_LOG(opItem != nullptr, nullptr, "opItem is nullptr!");
    auto op = std::make_shared<DrawAdaptivePixelMapOpItem>();
    COPY_OPITEN_AND_RETURN_RET_LOG(DrawAdaptivePixelMapOpItem, op.get(), opItem, nullptr, "failed to memcpy_s");
    op->Unmarshalling(cmdList);
    return op;
}

void DrawAdaptivePixelMapOpItem::Unmarshalling(const CmdList& cmdList)
{
    auto pixelMap = CmdListHelper::GetPixelMapFromCmdList(cmdList, pixelMap_);
    if (pixelMap == nullptr) {
        LOGE("pixelMap is nullptr!");
        return;
    }

    playbackTask_ = [this, pixelMap](Canvas& canvas, const Rect& rect) {
        AdaptiveImageHelper::DrawPixelMap(canvas, rect, pixelMap, imageInfo_, smapling_);
    };
}

void DrawAdaptivePixelMapOpItem::Playback(CanvasPlayer& player, void* opItem)
{
    if (opItem != nullptr) {
        auto* op = static_cast<DrawImageWithParmOpItem*>(opItem);
        op->Playback(player.canvas_, player.cmdList_, player.rect_);
    }
}

void DrawAdaptivePixelMapOpItem::Playback(Canvas& canvas, const CmdList& cmdList, const Rect& rect)
{
    if (playbackTask_) {
        playbackTask_(canvas, rect);
        return;
    }

    auto pixelMap = CmdListHelper::GetPixelMapFromCmdList(cmdList, pixelMap_);
    if (pixelMap == nullptr) {
        LOGE("pixelMap is nullptr!");
        return;
    }

    AdaptiveImageHelper::DrawPixelMap(canvas, rect, pixelMap, imageInfo_, smapling_);
}

DrawImageWithParmOpItem::DrawImageWithParmOpItem() : DrawOpItem(IMAGE_WITH_PARM_OPITEM) {}
DrawImageWithParmOpItem::DrawImageWithParmOpItem(const ImageHandle& objectHandle, const SamplingOptions& sampling)
    : DrawOpItem(IMAGE_WITH_PARM_OPITEM), objectHandle_(objectHandle), sampling_(sampling) {}

std::shared_ptr<OpItem> DrawImageWithParmOpItem::Unmarshalling(const CmdList& cmdList, void* opItem)
{
    CHECK_AND_RETURN_RET_LOG(opItem != nullptr, nullptr, "opItem is nullptr!");
    auto op = std::make_shared<DrawImageWithParmOpItem>();
    COPY_OPITEN_AND_RETURN_RET_LOG(DrawImageWithParmOpItem, op.get(), opItem, nullptr, "failed to memcpy_s");
    op->Unmarshalling(cmdList);
    return op;
}

void DrawImageWithParmOpItem::Unmarshalling(const CmdList& cmdList)
{
    auto extendObject = CmdListHelper::GetImageObjectFromCmdList(cmdList, objectHandle_);
    if (extendObject == nullptr) {
        LOGE("extendObject is nullptr!");
        return;
    }

    playbackTask_ = [this, extendObject](Canvas& canvas, const Rect& rect) {
        extendObject->Playback(canvas, rect, sampling_, false);
    };
}

void DrawImageWithParmOpItem::Playback(CanvasPlayer& player, void* opItem)
{
    if (opItem != nullptr) {
        auto* op = static_cast<DrawImageWithParmOpItem*>(opItem);
        op->Playback(player.canvas_, player.cmdList_, player.rect_);
    }
}

void DrawImageWithParmOpItem::Playback(Canvas& canvas, const CmdList& cmdList, const Rect& rect) const
{
    if (playbackTask_) {
        playbackTask_(canvas, rect);
        return;
    }

    auto extendObject = CmdListHelper::GetImageObjectFromCmdList(cmdList, objectHandle_);
    if (extendObject == nullptr) {
        LOGE("extendObject is nullptr!");
        return;
    }
    extendObject->Playback(canvas, rect, sampling_, false);
}

DrawExtendPixelMapOpItem::DrawExtendPixelMapOpItem() : DrawOpItem(EXTEND_PIXELMAP_OPITEM) {}
DrawExtendPixelMapOpItem::DrawExtendPixelMapOpItem(const ImageHandle& objectHandle, const SamplingOptions& sampling)
    : DrawOpItem(EXTEND_PIXELMAP_OPITEM), objectHandle_(objectHandle), sampling_(sampling) {}

std::shared_ptr<OpItem> DrawExtendPixelMapOpItem::Unmarshalling(const CmdList& cmdList, void* opItem)
{
    CHECK_AND_RETURN_RET_LOG(opItem != nullptr, nullptr, "opItem is nullptr!");
    auto op = std::make_shared<DrawExtendPixelMapOpItem>();
    COPY_OPITEN_AND_RETURN_RET_LOG(DrawExtendPixelMapOpItem, op.get(), opItem, nullptr, "failed to memcpy_s");
    op->Unmarshalling(cmdList);
    return op;
}

void DrawExtendPixelMapOpItem::Unmarshalling(const CmdList& cmdList)
{
    auto extendObject = CmdListHelper::GetImageObjectFromCmdList(cmdList, objectHandle_);
    if (extendObject == nullptr) {
        LOGE("extendObject is nullptr!");
        return;
    }

    playbackTask_ = [this, extendObject](Canvas& canvas, const Rect& rect) {
        extendObject->Playback(canvas, rect, sampling_, false);
    };
}


void DrawExtendPixelMapOpItem::Playback(CanvasPlayer& player, void* opItem)
{
    if (opItem != nullptr) {
        auto* op = static_cast<DrawExtendPixelMapOpItem*>(opItem);
        op->Playback(player.canvas_, player.cmdList_, player.rect_);
    }
}

void DrawExtendPixelMapOpItem::Playback(Canvas& canvas, const CmdList& cmdList, const Rect& rect) const
{
    if (playbackTask_) {
        playbackTask_(canvas, rect);
        return;
    }

    auto extendObject = CmdListHelper::GetImageObjectFromCmdList(cmdList, objectHandle_);
    if (extendObject == nullptr) {
        LOGE("extendObject is nullptr!");
        return;
    }
    extendObject->Playback(canvas, rect, sampling_, false);
}

NoIPCImageOpItem::NoIPCImageOpItem(std::shared_ptr<Image> image, const Rect& src, const Rect& dst,
    const SamplingOptions& sampling, SrcRectConstraint constraint)
    : DrawOpItem(NO_IPC_IMAGE_DRAW_OPITEM),
    image_(image),
    src_(src),
    dst_(dst),
    sampling_(sampling),
    constraint_(constraint)
{
}

void NoIPCImageOpItem::Playback(CanvasPlayer& player, void* opItem)
{
    if (opItem != nullptr) {
        auto* op = static_cast<NoIPCImageOpItem*>(opItem);
        op->Playback(player.canvas_, player.cmdList_);
    }
}

void NoIPCImageOpItem::Playback(Canvas& canvas, const CmdList& cmdList)
{
    if (image_ == nullptr) {
        return;
    }
    canvas.DrawImageRect(*image_, src_, dst_, sampling_, constraint_);
}

} // namespace Drawing
} // namespace Rosen
} // namespace OHOS
