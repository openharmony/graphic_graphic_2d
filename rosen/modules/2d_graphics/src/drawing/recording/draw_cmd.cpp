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

std::unordered_map<uint32_t, UnmarshallingPlayer::UnmarshallingFunc> UnmarshallingPlayer::opUnmarshallingFuncLUT_ = {
    { DrawOpItem::POINTS_OPITEM,            DrawPointsOpItem::Unmarshalling },
    { DrawOpItem::ROUND_RECT_OPITEM,        DrawRoundRectOpItem::Unmarshalling },
    { DrawOpItem::NESTED_ROUND_RECT_OPITEM, DrawNestedRoundRectOpItem::Unmarshalling },
    { DrawOpItem::PATH_OPITEM,              DrawPathOpItem::Unmarshalling },
    { DrawOpItem::BACKGROUND_OPITEM,        DrawBackgroundOpItem::Unmarshalling },
    { DrawOpItem::SHADOW_OPITEM,            DrawShadowOpItem::Unmarshalling },
    { DrawOpItem::IMAGE_NINE_OPITEM,        DrawImageNineOpItem::Unmarshalling },
    { DrawOpItem::IMAGE_ANNOTATION_OPITEM,  DrawAnnotationOpItem::Unmarshalling },
    { DrawOpItem::IMAGE_LATTICE_OPITEM,     DrawImageLatticeOpItem::Unmarshalling },
    { DrawOpItem::BITMAP_OPITEM,            DrawBitmapOpItem::Unmarshalling },
    { DrawOpItem::IMAGE_OPITEM,             DrawImageOpItem::Unmarshalling },
    { DrawOpItem::IMAGE_RECT_OPITEM,        DrawImageRectOpItem::Unmarshalling },
    { DrawOpItem::PICTURE_OPITEM,           DrawPictureOpItem::Unmarshalling },
    { DrawOpItem::TEXT_BLOB_OPITEM,         DrawTextBlobOpItem::Unmarshalling },
    { DrawOpItem::CLIP_ROUND_RECT_OPITEM,   ClipRoundRectOpItem::Unmarshalling },
    { DrawOpItem::CLIP_PATH_OPITEM,         ClipPathOpItem::Unmarshalling },
    { DrawOpItem::CLIP_REGION_OPITEM,       ClipRegionOpItem::Unmarshalling },
    { DrawOpItem::SAVE_LAYER_OPITEM,        SaveLayerOpItem::Unmarshalling },
    { DrawOpItem::ATTACH_PEN_OPITEM,        AttachPenOpItem::Unmarshalling },
    { DrawOpItem::ATTACH_BRUSH_OPITEM,      AttachBrushOpItem::Unmarshalling },
    { DrawOpItem::CLIP_ADAPTIVE_ROUND_RECT_OPITEM, ClipAdaptiveRoundRectOpItem::Unmarshalling},
    { DrawOpItem::ADAPTIVE_IMAGE_OPITEM,    DrawAdaptiveImageOpItem::Unmarshalling},
    { DrawOpItem::ADAPTIVE_PIXELMAP_OPITEM, DrawAdaptivePixelMapOpItem::Unmarshalling},
    { DrawOpItem::REGION_OPITEM,            DrawRegionOpItem::Unmarshalling },
    { DrawOpItem::PATCH_OPITEM,             DrawPatchOpItem::Unmarshalling },
    { DrawOpItem::EDGEAAQUAD_OPITEM, DrawEdgeAAQuadOpItem::Unmarshalling },
    { DrawOpItem::VERTICES_OPITEM,          DrawVerticesOpItem::Unmarshalling },
};

UnmarshallingPlayer::UnmarshallingPlayer(const CmdList& cmdList)
    : cmdList_(cmdList) {}

bool UnmarshallingPlayer::Unmarshalling(uint32_t type, void* opItem)
{
    if (type == DrawOpItem::OPITEM_HEAD) {
        return true;
    }

    auto it = opUnmarshallingFuncLUT_.find(type);
    if (it == opUnmarshallingFuncLUT_.end() || it->second == nullptr) {
        return false;
    }

    auto func = it->second;
    (*func)(this->cmdList_, opItem);

    return true;
}

/* OpItem */
DrawPointOpItem::DrawPointOpItem(const Point& point) : DrawOpItem(POINT_OPITEM), point_(point) {}

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

DrawPointsOpItem::DrawPointsOpItem(PointMode mode, const std::pair<uint32_t, size_t> pts)
    : DrawOpItem(POINTS_OPITEM), mode_(mode), pts_(pts) {}

void DrawPointsOpItem::Unmarshalling(const CmdList& cmdList, void* opItem)
{
    if (opItem != nullptr) {
        auto* op = static_cast<DrawPointsOpItem*>(opItem);
        op->Unmarshalling(cmdList);
    }
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

DrawLineOpItem::DrawLineOpItem(const Point& startPt, const Point& endPt) : DrawOpItem(LINE_OPITEM),
    startPt_(startPt), endPt_(endPt) {}

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

DrawRectOpItem::DrawRectOpItem(const Rect& rect) : DrawOpItem(RECT_OPITEM), rect_(rect) {}

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

DrawRoundRectOpItem::DrawRoundRectOpItem(std::pair<uint32_t, size_t> radiusXYData, const Rect& rect)
    : DrawOpItem(ROUND_RECT_OPITEM), radiusXYData_(radiusXYData), rect_(rect) {}

void DrawRoundRectOpItem::Unmarshalling(const CmdList& cmdList, void* opItem)
{
    if (opItem != nullptr) {
        auto* op = static_cast<DrawRoundRectOpItem*>(opItem);
        op->Unmarshalling(cmdList);
    }
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

DrawNestedRoundRectOpItem::DrawNestedRoundRectOpItem(const std::pair<uint32_t, size_t> outerRadiusXYData,
    const Rect& outerRect, const std::pair<uint32_t, size_t> innerRadiusXYData, const Rect& innerRect)
    : DrawOpItem(NESTED_ROUND_RECT_OPITEM), outerRadiusXYData_(outerRadiusXYData),
    innerRadiusXYData_(innerRadiusXYData), outerRect_(outerRect), innerRect_(innerRect) {}

void DrawNestedRoundRectOpItem::Unmarshalling(const CmdList& cmdList, void* opItem)
{
    if (opItem != nullptr) {
        auto* op = static_cast<DrawNestedRoundRectOpItem*>(opItem);
        op->Unmarshalling(cmdList);
    }
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

DrawArcOpItem::DrawArcOpItem(const Rect& rect, scalar startAngle, scalar sweepAngle)
    : DrawOpItem(ARC_OPITEM), rect_(rect), startAngle_(startAngle), sweepAngle_(sweepAngle) {}

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

DrawPieOpItem::DrawPieOpItem(const Rect& rect, scalar startAngle, scalar sweepAngle)
    : DrawOpItem(PIE_OPITEM), rect_(rect), startAngle_(startAngle), sweepAngle_(sweepAngle) {}

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

DrawOvalOpItem::DrawOvalOpItem(const Rect& rect) : DrawOpItem(OVAL_OPITEM), rect_(rect) {}

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

DrawCircleOpItem::DrawCircleOpItem(const Point& centerPt, scalar radius)
    : DrawOpItem(CIRCLE_OPITEM), centerPt_(centerPt), radius_(radius) {}

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

DrawPathOpItem::DrawPathOpItem(const CmdListHandle& path) : DrawOpItem(PATH_OPITEM), path_(path) {}

void DrawPathOpItem::Unmarshalling(const CmdList& cmdList, void* opItem)
{
    if (opItem != nullptr) {
        auto* op = static_cast<DrawPathOpItem*>(opItem);
        op->Unmarshalling(cmdList);
    }
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

DrawBackgroundOpItem::DrawBackgroundOpItem(const BrushHandle& brushHandle)
    : DrawOpItem(BACKGROUND_OPITEM), brushHandle_(brushHandle) {}

void DrawBackgroundOpItem::Unmarshalling(const CmdList& cmdList, void* opItem)
{
    if (opItem != nullptr) {
        auto* op = static_cast<DrawBackgroundOpItem*>(opItem);
        op->Unmarshalling(cmdList);
    }
}

void DrawBackgroundOpItem::Unmarshalling(const CmdList& cmdList, Canvas* canvas)
{
    auto colorSpace = CmdListHelper::GetFromCmdList<ColorSpaceCmdList, ColorSpace>(
        cmdList, brushHandle_.colorSpaceHandle);
    auto shaderEffect = CmdListHelper::GetFromCmdList<ShaderEffectCmdList, ShaderEffect>(
        cmdList, brushHandle_.shaderEffectHandle);
    auto colorFilter = CmdListHelper::GetFromCmdList<ColorFilterCmdList, ColorFilter>(
        cmdList, brushHandle_.colorFilterHandle);
    auto imageFilter = CmdListHelper::GetFromCmdList<ImageFilterCmdList, ImageFilter>(
        cmdList, brushHandle_.imageFilterHandle);
    auto maskFilter = CmdListHelper::GetFromCmdList<MaskFilterCmdList, MaskFilter>(
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

DrawShadowOpItem::DrawShadowOpItem(const CmdListHandle& path, const Point3& planeParams, const Point3& devLightPos,
    scalar lightRadius, Color ambientColor, Color spotColor, ShadowFlags flag)
    : DrawOpItem(SHADOW_OPITEM), path_(path), planeParams_(planeParams), devLightPos_(devLightPos),
    lightRadius_(lightRadius), ambientColor_(ambientColor), spotColor_(spotColor), flag_(flag) {}

void DrawShadowOpItem::Unmarshalling(const CmdList& cmdList, void* opItem)
{
    if (opItem != nullptr) {
        auto* op = static_cast<DrawShadowOpItem*>(opItem);
        op->Unmarshalling(cmdList);
    }
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

DrawRegionOpItem::DrawRegionOpItem(const CmdListHandle& region) : DrawOpItem(REGION_OPITEM), region_(region) {}

void DrawRegionOpItem::Unmarshalling(const CmdList& cmdList, void* opItem)
{
    if (opItem != nullptr) {
        auto* op = static_cast<DrawRegionOpItem*>(opItem);
        op->Unmarshalling(cmdList);
    }
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

DrawPatchOpItem::DrawPatchOpItem(const std::pair<uint32_t, size_t> cubics, const std::pair<uint32_t, size_t> colors,
    const std::pair<uint32_t, size_t> texCoords, BlendMode mode)
    : DrawOpItem(PATCH_OPITEM), cubics_(cubics), colors_(colors), texCoords_(texCoords), mode_(mode) {}

void DrawPatchOpItem::Unmarshalling(const CmdList& cmdList, void* opItem)
{
    if (opItem != nullptr) {
        auto* op = static_cast<DrawPatchOpItem*>(opItem);
        op->Unmarshalling(cmdList);
    }
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

DrawEdgeAAQuadOpItem::DrawEdgeAAQuadOpItem(const Rect& rect,
    const std::pair<uint32_t, size_t> clipQuad, QuadAAFlags aaFlags, ColorQuad color, BlendMode mode)
    : DrawOpItem(EDGEAAQUAD_OPITEM), rect_(rect), clipQuad_(clipQuad),
    aaFlags_(aaFlags), color_(color), mode_(mode) {}

void DrawEdgeAAQuadOpItem::Unmarshalling(const CmdList& cmdList, void* opItem)
{
    if (opItem != nullptr) {
        auto* op = static_cast<DrawEdgeAAQuadOpItem*>(opItem);
        op->Unmarshalling(cmdList);
    }
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

DrawVerticesOpItem::DrawVerticesOpItem(const VerticesHandle& vertices, BlendMode mode)
    : DrawOpItem(VERTICES_OPITEM), vertices_(vertices), mode_(mode) {}

void DrawVerticesOpItem::Unmarshalling(const CmdList& cmdList, void* opItem)
{
    if (opItem != nullptr) {
        auto* op = static_cast<DrawVerticesOpItem*>(opItem);
        op->Unmarshalling(cmdList);
    }
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

DrawColorOpItem::DrawColorOpItem(ColorQuad color, BlendMode mode) : DrawOpItem(COLOR_OPITEM),
    color_(color), mode_(mode) {}

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

DrawImageNineOpItem::DrawImageNineOpItem(const ImageHandle& image, const RectI& center, const Rect& dst,
    FilterMode filter, const BrushHandle& brushHandle, bool hasBrush) : DrawOpItem(IMAGE_NINE_OPITEM),
    image_(std::move(image)), center_(center), dst_(dst), filter_(filter),
    brushHandle_(brushHandle), hasBrush_(hasBrush) {}

void DrawImageNineOpItem::Unmarshalling(const CmdList& cmdList, void* opItem)
{
    if (opItem != nullptr) {
        auto* op = static_cast<DrawImageNineOpItem*>(opItem);
        op->Unmarshalling(cmdList);
    }
}

void DrawImageNineOpItem::Unmarshalling(const CmdList& cmdList, Canvas* canvas)
{
    auto image = CmdListHelper::GetImageFromCmdList(cmdList, image_);
    if (image == nullptr) {
        return;
    }

    std::shared_ptr<Brush> brush = nullptr;
    if (hasBrush_) {
        auto colorSpace = CmdListHelper::GetFromCmdList<ColorSpaceCmdList, ColorSpace>(
            cmdList, brushHandle_.colorSpaceHandle);
        auto shaderEffect = CmdListHelper::GetFromCmdList<ShaderEffectCmdList, ShaderEffect>(
            cmdList, brushHandle_.shaderEffectHandle);
        auto colorFilter = CmdListHelper::GetFromCmdList<ColorFilterCmdList, ColorFilter>(
            cmdList, brushHandle_.colorFilterHandle);
        auto imageFilter = CmdListHelper::GetFromCmdList<ImageFilterCmdList, ImageFilter>(
            cmdList, brushHandle_.imageFilterHandle);
        auto maskFilter = CmdListHelper::GetFromCmdList<MaskFilterCmdList, MaskFilter>(
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

void DrawAnnotationOpItem::Unmarshalling(const CmdList& cmdList, void* opItem)
{
    if (opItem != nullptr) {
        auto* op = static_cast<DrawAnnotationOpItem*>(opItem);
        op->Unmarshalling(cmdList);
    }
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

DrawImageLatticeOpItem::DrawImageLatticeOpItem(const ImageHandle& image, const Lattice& lattice, const Rect& dst,
    FilterMode filter, const BrushHandle& brushHandle, bool hasBrush) : DrawOpItem(IMAGE_LATTICE_OPITEM),
    image_(std::move(image)), lattice_(lattice), dst_(dst), filter_(filter),
    brushHandle_(brushHandle), hasBrush_(hasBrush) {}

void DrawImageLatticeOpItem::Unmarshalling(const CmdList& cmdList, void* opItem)
{
    if (opItem != nullptr) {
        auto* op = static_cast<DrawImageLatticeOpItem*>(opItem);
        op->Unmarshalling(cmdList);
    }
}

void DrawImageLatticeOpItem::Unmarshalling(const CmdList& cmdList, Canvas* canvas)
{
    auto image = CmdListHelper::GetImageFromCmdList(cmdList, image_);
    if (image == nullptr) {
        return;
    }

    std::shared_ptr<Brush> brush = nullptr;
    if (hasBrush_) {
        auto colorSpace = CmdListHelper::GetFromCmdList<ColorSpaceCmdList, ColorSpace>(
            cmdList, brushHandle_.colorSpaceHandle);
        auto shaderEffect = CmdListHelper::GetFromCmdList<ShaderEffectCmdList, ShaderEffect>(
            cmdList, brushHandle_.shaderEffectHandle);
        auto colorFilter = CmdListHelper::GetFromCmdList<ColorFilterCmdList, ColorFilter>(
            cmdList, brushHandle_.colorFilterHandle);
        auto imageFilter = CmdListHelper::GetFromCmdList<ImageFilterCmdList, ImageFilter>(
            cmdList, brushHandle_.imageFilterHandle);
        auto maskFilter = CmdListHelper::GetFromCmdList<MaskFilterCmdList, MaskFilter>(
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

DrawBitmapOpItem::DrawBitmapOpItem(const ImageHandle& bitmap, scalar px, scalar py)
    : DrawOpItem(BITMAP_OPITEM), bitmap_(bitmap), px_(px), py_(py) {}

void DrawBitmapOpItem::Unmarshalling(const CmdList& cmdList, void* opItem)
{
    if (opItem != nullptr) {
        auto* op = static_cast<DrawBitmapOpItem*>(opItem);
        op->Unmarshalling(cmdList);
    }
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

DrawImageOpItem::DrawImageOpItem(const ImageHandle& image, scalar px, scalar py,
    const SamplingOptions& samplingOptions) : DrawOpItem(IMAGE_OPITEM),
    image_(std::move(image)), px_(px), py_(py), samplingOptions_(samplingOptions) {}

void DrawImageOpItem::Unmarshalling(const CmdList& cmdList, void* opItem)
{
    if (opItem != nullptr) {
        auto* op = static_cast<DrawImageOpItem*>(opItem);
        op->Unmarshalling(cmdList);
    }
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

DrawImageRectOpItem::DrawImageRectOpItem(const ImageHandle& image, const Rect& src, const Rect& dst,
    const SamplingOptions& sampling, SrcRectConstraint constraint) : DrawOpItem(IMAGE_RECT_OPITEM),
    image_(image), src_(src), dst_(dst), sampling_(sampling), constraint_(constraint) {}

void DrawImageRectOpItem::Unmarshalling(const CmdList& cmdList, void* opItem)
{
    if (opItem != nullptr) {
        auto* op = static_cast<DrawImageRectOpItem*>(opItem);
        op->Unmarshalling(cmdList);
    }
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

DrawPictureOpItem::DrawPictureOpItem(const ImageHandle& picture) : DrawOpItem(PICTURE_OPITEM), picture_(picture) {}

void DrawPictureOpItem::Unmarshalling(const CmdList& cmdList, void* opItem)
{
    if (opItem != nullptr) {
        auto* op = static_cast<DrawPictureOpItem*>(opItem);
        op->Unmarshalling(cmdList);
    }
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

DrawTextBlobOpItem::DrawTextBlobOpItem(const ImageHandle& textBlob, const scalar x, const scalar y)
    : DrawOpItem(TEXT_BLOB_OPITEM), textBlob_(textBlob), x_(x), y_(y) {}

void DrawTextBlobOpItem::Unmarshalling(const CmdList& cmdList, void* opItem)
{
    if (opItem != nullptr) {
        auto* op = static_cast<DrawTextBlobOpItem*>(opItem);
        op->Unmarshalling(cmdList);
    }
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

ClipRectOpItem::ClipRectOpItem(const Rect& rect, ClipOp op, bool doAntiAlias)
    : DrawOpItem(CLIP_RECT_OPITEM), rect_(rect), clipOp_(op), doAntiAlias_(doAntiAlias) {}

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

ClipIRectOpItem::ClipIRectOpItem(const RectI& rect, ClipOp op)
    : DrawOpItem(CLIP_IRECT_OPITEM), rect_(rect), clipOp_(op) {}

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

ClipRoundRectOpItem::ClipRoundRectOpItem(std::pair<uint32_t, size_t> radiusXYData, const Rect& rect, ClipOp op,
    bool doAntiAlias) : DrawOpItem(CLIP_ROUND_RECT_OPITEM), radiusXYData_(radiusXYData), rect_(rect), clipOp_(op),
    doAntiAlias_(doAntiAlias) {}

void ClipRoundRectOpItem::Unmarshalling(const CmdList& cmdList, void* opItem)
{
    if (opItem != nullptr) {
        auto* op = static_cast<ClipRoundRectOpItem*>(opItem);
        op->Unmarshalling(cmdList);
    }
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

ClipPathOpItem::ClipPathOpItem(const CmdListHandle& path, ClipOp clipOp, bool doAntiAlias)
    : DrawOpItem(CLIP_PATH_OPITEM), path_(path), clipOp_(clipOp), doAntiAlias_(doAntiAlias) {}

void ClipPathOpItem::Unmarshalling(const CmdList& cmdList, void* opItem)
{
    if (opItem != nullptr) {
        auto* op = static_cast<ClipPathOpItem*>(opItem);
        op->Unmarshalling(cmdList);
    }
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

ClipRegionOpItem::ClipRegionOpItem(const CmdListHandle& region, ClipOp clipOp)
    : DrawOpItem(CLIP_REGION_OPITEM), region_(region), clipOp_(clipOp) {}

void ClipRegionOpItem::Unmarshalling(const CmdList& cmdList, void* opItem)
{
    if (opItem != nullptr) {
        auto* op = static_cast<ClipRegionOpItem*>(opItem);
        op->Unmarshalling(cmdList);
    }
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

SetMatrixOpItem::SetMatrixOpItem(const Matrix& matrix) : DrawOpItem(SET_MATRIX_OPITEM)
{
    matrix.GetAll(matrixBuffer_);
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

ConcatMatrixOpItem::ConcatMatrixOpItem(const Matrix& matrix) : DrawOpItem(CONCAT_MATRIX_OPITEM)
{
    matrix.GetAll(matrixBuffer_);
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

TranslateOpItem::TranslateOpItem(scalar dx, scalar dy) : DrawOpItem(TRANSLATE_OPITEM), dx_(dx), dy_(dy) {}

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

ScaleOpItem::ScaleOpItem(scalar sx, scalar sy) : DrawOpItem(SCALE_OPITEM), sx_(sx), sy_(sy) {}

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

RotateOpItem::RotateOpItem(scalar deg, scalar sx, scalar sy) : DrawOpItem(ROTATE_OPITEM), deg_(deg), sx_(sx), sy_(sy) {}

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

ShearOpItem::ShearOpItem(scalar sx, scalar sy) : DrawOpItem(SHEAR_OPITEM), sx_(sx), sy_(sy) {}

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

ClearOpItem::ClearOpItem(ColorQuad color) : DrawOpItem(CLEAR_OPITEM), color_(color) {}

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

SaveLayerOpItem::SaveLayerOpItem(const Rect& rect, bool hasBrush, const BrushHandle& brushHandle,
    const CmdListHandle& imageFilter, uint32_t saveLayerFlags) : DrawOpItem(SAVE_LAYER_OPITEM),
    rect_(rect), hasBrush_(hasBrush), brushHandle_(brushHandle), imageFilter_(imageFilter),
    saveLayerFlags_(saveLayerFlags) {}

void SaveLayerOpItem::Unmarshalling(const CmdList& cmdList, void* opItem)
{
    if (opItem != nullptr) {
        auto* op = static_cast<SaveLayerOpItem*>(opItem);
        op->Unmarshalling(cmdList);
    }
}

void SaveLayerOpItem::Unmarshalling(const CmdList& cmdList, Canvas* canvas)
{
    std::shared_ptr<Brush> brush = nullptr;
    if (hasBrush_) {
        auto colorSpace = CmdListHelper::GetFromCmdList<ColorSpaceCmdList, ColorSpace>(
            cmdList, brushHandle_.colorSpaceHandle);
        auto shaderEffect = CmdListHelper::GetFromCmdList<ShaderEffectCmdList, ShaderEffect>(
            cmdList, brushHandle_.shaderEffectHandle);
        auto colorFilter = CmdListHelper::GetFromCmdList<ColorFilterCmdList, ColorFilter>(
            cmdList, brushHandle_.colorFilterHandle);
        auto imageFilter = CmdListHelper::GetFromCmdList<ImageFilterCmdList, ImageFilter>(
            cmdList, brushHandle_.imageFilterHandle);
        auto maskFilter = CmdListHelper::GetFromCmdList<MaskFilterCmdList, MaskFilter>(
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

AttachPenOpItem::AttachPenOpItem(const PenHandle& penHandle) : DrawOpItem(ATTACH_PEN_OPITEM), penHandle_(penHandle) {}

void AttachPenOpItem::Unmarshalling(const CmdList& cmdList, void* opItem)
{
    if (opItem != nullptr) {
        auto* op = static_cast<AttachPenOpItem*>(opItem);
        op->Unmarshalling(cmdList);
    }
}

void AttachPenOpItem::Unmarshalling(const CmdList& cmdList, Canvas* canvas)
{
    auto pathEffect = CmdListHelper::GetFromCmdList<PathEffectCmdList, PathEffect>(
        cmdList, penHandle_.pathEffectHandle);
    auto colorSpace = CmdListHelper::GetFromCmdList<ColorSpaceCmdList, ColorSpace>(
        cmdList, penHandle_.colorSpaceHandle);
    auto shaderEffect = CmdListHelper::GetFromCmdList<ShaderEffectCmdList, ShaderEffect>(
        cmdList, penHandle_.shaderEffectHandle);
    auto colorFilter = CmdListHelper::GetFromCmdList<ColorFilterCmdList, ColorFilter>(
        cmdList, penHandle_.colorFilterHandle);
    auto imageFilter = CmdListHelper::GetFromCmdList<ImageFilterCmdList, ImageFilter>(
        cmdList, penHandle_.imageFilterHandle);
    auto maskFilter = CmdListHelper::GetFromCmdList<MaskFilterCmdList, MaskFilter>(
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

AttachBrushOpItem::AttachBrushOpItem(const BrushHandle& brushHandle)
    : DrawOpItem(ATTACH_BRUSH_OPITEM), brushHandle_(brushHandle) {}

void AttachBrushOpItem::Unmarshalling(const CmdList& cmdList, void* opItem)
{
    if (opItem != nullptr) {
        auto* op = static_cast<AttachBrushOpItem*>(opItem);
        op->Unmarshalling(cmdList);
    }
}

void AttachBrushOpItem::Unmarshalling(const CmdList& cmdList, Canvas* canvas)
{
    auto colorSpace = CmdListHelper::GetFromCmdList<ColorSpaceCmdList, ColorSpace>(
        cmdList, brushHandle_.colorSpaceHandle);
    auto shaderEffect = CmdListHelper::GetFromCmdList<ShaderEffectCmdList, ShaderEffect>(
        cmdList, brushHandle_.shaderEffectHandle);
    auto colorFilter = CmdListHelper::GetFromCmdList<ColorFilterCmdList, ColorFilter>(
        cmdList, brushHandle_.colorFilterHandle);
    auto imageFilter = CmdListHelper::GetFromCmdList<ImageFilterCmdList, ImageFilter>(
        cmdList, brushHandle_.imageFilterHandle);
    auto maskFilter = CmdListHelper::GetFromCmdList<MaskFilterCmdList, MaskFilter>(
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

ClipAdaptiveRoundRectOpItem::ClipAdaptiveRoundRectOpItem(const std::pair<uint32_t, size_t>& radiusData)
    : DrawOpItem(CLIP_ADAPTIVE_ROUND_RECT_OPITEM), radiusData_(radiusData) {}

void ClipAdaptiveRoundRectOpItem::Unmarshalling(const CmdList& cmdList, void* opItem)
{
    if (opItem != nullptr) {
        auto* op = static_cast<ClipAdaptiveRoundRectOpItem*>(opItem);
        op->Unmarshalling(cmdList);
    }
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

DrawAdaptiveImageOpItem::DrawAdaptiveImageOpItem(const ImageHandle& image, const AdaptiveImageInfo& rsImageInfo,
    const SamplingOptions& sampling, const bool isImage) : DrawOpItem(ADAPTIVE_IMAGE_OPITEM),
    image_(image), rsImageInfo_(rsImageInfo), sampling_(sampling), isImage_(isImage) {}

void DrawAdaptiveImageOpItem::Unmarshalling(const CmdList& cmdList, void* opItem)
{
    if (opItem != nullptr) {
        auto* op = static_cast<DrawAdaptiveImageOpItem*>(opItem);
        op->Unmarshalling(cmdList);
    }
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

DrawAdaptivePixelMapOpItem::DrawAdaptivePixelMapOpItem(const ImageHandle& pixelMap, const AdaptiveImageInfo& imageInfo,
    const SamplingOptions& smapling) : DrawOpItem(ADAPTIVE_PIXELMAP_OPITEM), pixelMap_(pixelMap),
    imageInfo_(imageInfo), smapling_(smapling) {}

void DrawAdaptivePixelMapOpItem::Unmarshalling(const CmdList& cmdList, void* opItem)
{
    if (opItem != nullptr) {
        auto* op = static_cast<DrawAdaptivePixelMapOpItem*>(opItem);
        op->Unmarshalling(cmdList);
    }
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
        auto* op = static_cast<DrawAdaptivePixelMapOpItem*>(opItem);
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

DrawImageWithParmOpItem::DrawImageWithParmOpItem(const ImageHandle& objectHandle, const SamplingOptions& sampling)
    : DrawOpItem(IMAGE_WITH_PARM_OPITEM), objectHandle_(objectHandle), sampling_(sampling) {}

void DrawImageWithParmOpItem::Unmarshalling(const CmdList& cmdList, void* opItem)
{
    if (opItem != nullptr) {
        auto* op = static_cast<DrawImageWithParmOpItem*>(opItem);
        op->Unmarshalling(cmdList);
    }
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
        auto* op = static_cast<const DrawImageWithParmOpItem*>(opItem);
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

DrawExtendPixelMapOpItem::DrawExtendPixelMapOpItem(const ImageHandle& objectHandle, const SamplingOptions& sampling)
    : DrawOpItem(EXTEND_PIXELMAP_OPITEM), objectHandle_(objectHandle), sampling_(sampling) {}

void DrawExtendPixelMapOpItem::Unmarshalling(const CmdList& cmdList, void* opItem)
{
    if (opItem != nullptr) {
        auto* op = static_cast<DrawExtendPixelMapOpItem*>(opItem);
        op->Unmarshalling(cmdList);
    }
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
        auto* op = static_cast<const DrawExtendPixelMapOpItem*>(opItem);
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
} // namespace Drawing
} // namespace Rosen
} // namespace OHOS
