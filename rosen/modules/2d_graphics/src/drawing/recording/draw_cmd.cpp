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

namespace OHOS {
namespace Rosen {
namespace Drawing {
namespace {
void BrushHandleToBrush(const BrushHandle& brushHandle, const CmdList& cmdList, Brush& brush)
{
    brush.SetBlendMode(brushHandle.mode);
    brush.SetAntiAlias(brushHandle.isAntiAlias);

    if (brushHandle.colorSpaceHandle.size) {
        auto colorSpace = CmdListHelper::GetColorSpaceFromCmdList(cmdList, brushHandle.colorSpaceHandle);
        const Color4f color4f = { brushHandle.color.GetRedF(), brushHandle.color.GetGreenF(),
                                  brushHandle.color.GetBlueF(), brushHandle.color.GetAlphaF() };
        brush.SetColor(color4f, colorSpace);
    } else {
        brush.SetColor(brushHandle.color);
    }

    if (brushHandle.shaderEffectHandle.size) {
        auto shaderEffect = CmdListHelper::GetShaderEffectFromCmdList(cmdList, brushHandle.shaderEffectHandle);
        brush.SetShaderEffect(shaderEffect);
    }

    Filter filter;
    bool hasFilter = false;
    if (brushHandle.colorFilterHandle.size) {
        auto colorFilter = CmdListHelper::GetColorFilterFromCmdList(cmdList, brushHandle.colorFilterHandle);
        filter.SetColorFilter(colorFilter);
        hasFilter = true;
    }
    if (brushHandle.imageFilterHandle.size) {
        auto imageFilter = CmdListHelper::GetImageFilterFromCmdList(cmdList, brushHandle.imageFilterHandle);
        filter.SetImageFilter(imageFilter);
        hasFilter = true;
    }
    if (brushHandle.maskFilterHandle.size) {
        auto maskFilter = CmdListHelper::GetMaskFilterFromCmdList(cmdList, brushHandle.maskFilterHandle);
        filter.SetMaskFilter(maskFilter);
        hasFilter = true;
    }

    if (hasFilter) {
        filter.SetFilterQuality(brushHandle.filterQuality);
        brush.SetFilter(filter);
    }
}

void PenHandleToPen(const PenHandle& penHandle, const CmdList& cmdList, Pen& pen)
{
    pen.SetWidth(penHandle.width);
    pen.SetMiterLimit(penHandle.miterLimit);
    pen.SetCapStyle(penHandle.capStyle);
    pen.SetJoinStyle(penHandle.joinStyle);
    pen.SetBlendMode(penHandle.mode);
    pen.SetAntiAlias(penHandle.isAntiAlias);

    if (penHandle.colorSpaceHandle.size) {
        auto colorSpace = CmdListHelper::GetColorSpaceFromCmdList(cmdList, penHandle.colorSpaceHandle);
        const Color4f color4f = { penHandle.color.GetRedF(), penHandle.color.GetGreenF(),
                                  penHandle.color.GetBlueF(), penHandle.color.GetAlphaF() };
        pen.SetColor(color4f, colorSpace);
    } else {
        pen.SetColor(penHandle.color);
    }

    if (penHandle.shaderEffectHandle.size) {
        auto shaderEffect = CmdListHelper::GetShaderEffectFromCmdList(cmdList, penHandle.shaderEffectHandle);
        pen.SetShaderEffect(shaderEffect);
    }

    if (penHandle.pathEffectHandle.size) {
        auto pathEffect = CmdListHelper::GetPathEffectFromCmdList(cmdList, penHandle.pathEffectHandle);
        pen.SetPathEffect(pathEffect);
    }

    Filter filter;
    bool hasFilter = false;
    if (penHandle.colorFilterHandle.size) {
        auto colorFilter = CmdListHelper::GetColorFilterFromCmdList(cmdList, penHandle.colorFilterHandle);
        filter.SetColorFilter(colorFilter);
        hasFilter = true;
    }
    if (penHandle.imageFilterHandle.size) {
        auto imageFilter = CmdListHelper::GetImageFilterFromCmdList(cmdList, penHandle.imageFilterHandle);
        filter.SetImageFilter(imageFilter);
        hasFilter = true;
    }
    if (penHandle.maskFilterHandle.size) {
        auto maskFilter = CmdListHelper::GetMaskFilterFromCmdList(cmdList, penHandle.maskFilterHandle);
        filter.SetMaskFilter(maskFilter);
        hasFilter = true;
    }

    if (hasFilter) {
        filter.SetFilterQuality(penHandle.filterQuality);
        pen.SetFilter(filter);
    }
}
}

GenerateCachedOpItemPlayer::GenerateCachedOpItemPlayer(CmdList &cmdList, Canvas* canvas, const Rect* rect)
    : canvas_(canvas), rect_(rect), cmdList_(cmdList) {}

bool GenerateCachedOpItemPlayer::GenerateCachedOpItem(uint32_t type, void* handle)
{
    if (handle == nullptr) {
        return false;
    }

    if (type == DrawOpItem::TEXT_BLOB_OPITEM) {
        auto* op = static_cast<DrawTextBlobOpItem::ConstructorHandle*>(handle);
        return op->GenerateCachedOpItem(cmdList_, canvas_,
            static_cast<AttachPenOpItem::ConstructorHandle*>(penOpHandle_),
            static_cast<AttachBrushOpItem::ConstructorHandle*>(brushOpHandle_));
    }

    if (type == DrawOpItem::ATTACH_PEN_OPITEM) {
        penOpHandle_ = handle;
    } else if (type == DrawOpItem::ATTACH_BRUSH_OPITEM) {
        brushOpHandle_ = handle;
    } else if (type == DrawOpItem::DETACH_PEN_OPITEM) {
        penOpHandle_ = nullptr;
    } else if (type == DrawOpItem::DETACH_BRUSH_OPITEM) {
        brushOpHandle_ = nullptr;
    }
    return false;
}

std::unordered_map<uint32_t, UnmarshallingPlayer::UnmarshallingFunc> UnmarshallingPlayer::opUnmarshallingFuncLUT_ = {
    { DrawOpItem::CMD_LIST_OPITEM,          DrawCmdListOpItem::Unmarshalling },
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
    { DrawOpItem::IMAGE_WITH_PARM_OPITEM,   DrawImageWithParmOpItem::Unmarshalling},
    { DrawOpItem::PIXELMAP_WITH_PARM_OPITEM, DrawPixelMapWithParmOpItem::Unmarshalling},
    { DrawOpItem::PIXELMAP_RECT_OPITEM,     DrawPixelMapRectOpItem::Unmarshalling},
    { DrawOpItem::REGION_OPITEM,            DrawRegionOpItem::Unmarshalling },
    { DrawOpItem::PATCH_OPITEM,             DrawPatchOpItem::Unmarshalling },
    { DrawOpItem::EDGEAAQUAD_OPITEM, DrawEdgeAAQuadOpItem::Unmarshalling },
    { DrawOpItem::VERTICES_OPITEM,          DrawVerticesOpItem::Unmarshalling },
#ifdef ROSEN_OHOS
    { DrawOpItem::SURFACEBUFFER_OPITEM,          DrawSurfaceBufferOpItem::Unmarshalling },
#endif
};

UnmarshallingPlayer::UnmarshallingPlayer(const CmdList& cmdList) : cmdList_(cmdList) {}

std::shared_ptr<DrawOpItem> UnmarshallingPlayer::Unmarshalling(uint32_t type, void* handle)
{
    if (type == DrawOpItem::OPITEM_HEAD) {
        return nullptr;
    }

    auto it = opUnmarshallingFuncLUT_.find(type);
    if (it == opUnmarshallingFuncLUT_.end() || it->second == nullptr) {
        return nullptr;
    }

    auto func = it->second;
    return (*func)(this->cmdList_, handle);
}

DrawCmdListOpItem::DrawCmdListOpItem(const CmdList& cmdList, DrawCmdListOpItem::ConstructorHandle* handle)
    : DrawOpItem(CMD_LIST_OPITEM)
{
    opItems_ = CmdListHelper::GetDrawOpItemsFromHandle(cmdList, handle->handle);
}

std::shared_ptr<DrawOpItem> DrawCmdListOpItem::Unmarshalling(const CmdList& cmdList, void* handle)
{
    return std::make_shared<DrawCmdListOpItem>(cmdList, static_cast<DrawCmdListOpItem::ConstructorHandle*>(handle));
}

void DrawCmdListOpItem::Playback(Canvas* canvas, const Rect* rect)
{
    for (auto op : opItems_) {
        op->Playback(canvas, rect);
    }
}

/* DrawPointOpItem */
DrawPointOpItem::DrawPointOpItem(DrawPointOpItem::ConstructorHandle* handle)
    : DrawOpItem(POINT_OPITEM), point_(handle->point) {}

std::shared_ptr<DrawOpItem> DrawPointOpItem::Unmarshalling(const CmdList& cmdList, void* handle)
{
    return std::make_shared<DrawPointOpItem>(static_cast<DrawPointOpItem::ConstructorHandle*>(handle));
}

void DrawPointOpItem::Playback(Canvas* canvas, const Rect* rect)
{
    canvas->DrawPoint(point_);
}

/* DrawPointsOpItem */
DrawPointsOpItem::DrawPointsOpItem(const CmdList& cmdList, DrawPointsOpItem::ConstructorHandle* handle)
    : DrawOpItem(POINTS_OPITEM), mode_(handle->mode)
{
    pts_ = CmdListHelper::GetVectorFromCmdList<Point>(cmdList, handle->pts);
}

std::shared_ptr<DrawOpItem> DrawPointsOpItem::Unmarshalling(const CmdList& cmdList, void* handle)
{
    return std::make_shared<DrawPointsOpItem>(cmdList, static_cast<DrawPointsOpItem::ConstructorHandle*>(handle));
}

void DrawPointsOpItem::Playback(Canvas* canvas, const Rect* rect)
{
    canvas->DrawPoints(mode_, pts_.size(), pts_.data());
}

/* DrawLineOpItem */
DrawLineOpItem::DrawLineOpItem(DrawLineOpItem::ConstructorHandle* handle)
    : DrawOpItem(LINE_OPITEM), startPt_(handle->startPt), endPt_(handle->endPt) {}

std::shared_ptr<DrawOpItem> DrawLineOpItem::Unmarshalling(const CmdList& cmdList, void* handle)
{
    return std::make_shared<DrawLineOpItem>(static_cast<DrawLineOpItem::ConstructorHandle*>(handle));
}

void DrawLineOpItem::Playback(Canvas* canvas, const Rect* rect)
{
    canvas->DrawLine(startPt_, endPt_);
}

/* DrawRectOpItem */
DrawRectOpItem::DrawRectOpItem(DrawRectOpItem::ConstructorHandle* handle)
    : DrawOpItem(RECT_OPITEM), rect_(handle->rect) {}

std::shared_ptr<DrawOpItem> DrawRectOpItem::Unmarshalling(const CmdList& cmdList, void* handle)
{
    return std::make_shared<DrawRectOpItem>(static_cast<DrawRectOpItem::ConstructorHandle*>(handle));
}

void DrawRectOpItem::Playback(Canvas* canvas, const Rect* rect)
{
    canvas->DrawRect(rect_);
}

/* DrawRoundRectOpItem */
DrawRoundRectOpItem::DrawRoundRectOpItem(DrawRoundRectOpItem::ConstructorHandle* handle)
    : DrawOpItem(ROUND_RECT_OPITEM), rrect_(handle->rrect) {}

std::shared_ptr<DrawOpItem> DrawRoundRectOpItem::Unmarshalling(const CmdList& cmdList, void* handle)
{
    return std::make_shared<DrawRoundRectOpItem>(static_cast<DrawRoundRectOpItem::ConstructorHandle*>(handle));
}

void DrawRoundRectOpItem::Playback(Canvas* canvas, const Rect* rect)
{
    canvas->DrawRoundRect(rrect_);
}

/* DrawNestedRoundRectOpItem */
DrawNestedRoundRectOpItem::DrawNestedRoundRectOpItem(DrawNestedRoundRectOpItem::ConstructorHandle* handle)
    : DrawOpItem(NESTED_ROUND_RECT_OPITEM), outerRRect_(handle->outerRRect), innerRRect_(handle->innerRRect) {}

std::shared_ptr<DrawOpItem> DrawNestedRoundRectOpItem::Unmarshalling(const CmdList& cmdList, void* handle)
{
    return std::make_shared<DrawNestedRoundRectOpItem>(
        static_cast<DrawNestedRoundRectOpItem::ConstructorHandle*>(handle));
}

void DrawNestedRoundRectOpItem::Playback(Canvas* canvas, const Rect* rect)
{
    canvas->DrawNestedRoundRect(outerRRect_, innerRRect_);
}

/* DrawArcOpItem */
DrawArcOpItem::DrawArcOpItem(DrawArcOpItem::ConstructorHandle* handle)
    : DrawOpItem(ARC_OPITEM), rect_(handle->rect), startAngle_(handle->startAngle), sweepAngle_(handle->sweepAngle) {}

std::shared_ptr<DrawOpItem> DrawArcOpItem::Unmarshalling(const CmdList& cmdList, void* handle)
{
    return std::make_shared<DrawArcOpItem>(static_cast<DrawArcOpItem::ConstructorHandle*>(handle));
}

void DrawArcOpItem::Playback(Canvas* canvas, const Rect* rect)
{
    canvas->DrawArc(rect_, startAngle_, sweepAngle_);
}

/* DrawPieOpItem */
DrawPieOpItem::DrawPieOpItem(DrawPieOpItem::ConstructorHandle* handle)
    : DrawOpItem(PIE_OPITEM), rect_(handle->rect), startAngle_(handle->startAngle), sweepAngle_(handle->sweepAngle) {}

std::shared_ptr<DrawOpItem> DrawPieOpItem::Unmarshalling(const CmdList& cmdList, void* handle)
{
    return std::make_shared<DrawPieOpItem>(static_cast<DrawPieOpItem::ConstructorHandle*>(handle));
}

void DrawPieOpItem::Playback(Canvas* canvas, const Rect* rect)
{
    canvas->DrawPie(rect_, startAngle_, sweepAngle_);
}

/* DrawOvalOpItem */
DrawOvalOpItem::DrawOvalOpItem(DrawOvalOpItem::ConstructorHandle* handle)
    : DrawOpItem(OVAL_OPITEM), rect_(handle->rect) {}

std::shared_ptr<DrawOpItem> DrawOvalOpItem::Unmarshalling(const CmdList& cmdList, void* handle)
{
    return std::make_shared<DrawOvalOpItem>(static_cast<DrawOvalOpItem::ConstructorHandle*>(handle));
}

void DrawOvalOpItem::Playback(Canvas* canvas, const Rect* rect)
{
    canvas->DrawOval(rect_);
}

/* DrawCircleOpItem */
DrawCircleOpItem::DrawCircleOpItem(DrawCircleOpItem::ConstructorHandle* handle)
    : DrawOpItem(CIRCLE_OPITEM), centerPt_(handle->centerPt), radius_(handle->radius) {}

std::shared_ptr<DrawOpItem> DrawCircleOpItem::Unmarshalling(const CmdList& cmdList, void* handle)
{
    return std::make_shared<DrawCircleOpItem>(static_cast<DrawCircleOpItem::ConstructorHandle*>(handle));
}

void DrawCircleOpItem::Playback(Canvas* canvas, const Rect* rect)
{
    canvas->DrawCircle(centerPt_, radius_);
}

/* DrawPathOpItem */
DrawPathOpItem::DrawPathOpItem(const CmdList& cmdList, DrawPathOpItem::ConstructorHandle* handle)
    : DrawOpItem(PATH_OPITEM)
{
    path_ = CmdListHelper::GetPathFromCmdList(cmdList, handle->path);
}

std::shared_ptr<DrawOpItem> DrawPathOpItem::Unmarshalling(const CmdList& cmdList, void* handle)
{
    return std::make_shared<DrawPathOpItem>(cmdList, static_cast<DrawPathOpItem::ConstructorHandle*>(handle));
}

void DrawPathOpItem::Playback(Canvas* canvas, const Rect* rect)
{
    if (path_ == nullptr) {
        LOGE("DrawPathOpItem path is null!");
        return;
    }
    canvas->DrawPath(*path_);
}

/* DrawBackgroundOpItem */
DrawBackgroundOpItem::DrawBackgroundOpItem(const CmdList& cmdList, DrawBackgroundOpItem::ConstructorHandle* handle)
    : DrawOpItem(BACKGROUND_OPITEM)
{
    BrushHandleToBrush(handle->brushHandle, cmdList, brush_);
}

std::shared_ptr<DrawOpItem> DrawBackgroundOpItem::Unmarshalling(const CmdList& cmdList, void* handle)
{
    return std::make_shared<DrawBackgroundOpItem>(
        cmdList, static_cast<DrawBackgroundOpItem::ConstructorHandle*>(handle));
}

void DrawBackgroundOpItem::Playback(Canvas* canvas, const Rect* rect)
{
    canvas->DrawBackground(brush_);
}

/* DrawShadowOpItem */
DrawShadowOpItem::DrawShadowOpItem(const CmdList& cmdList, DrawShadowOpItem::ConstructorHandle* handle)
    : DrawOpItem(SHADOW_OPITEM), planeParams_(handle->planeParams), devLightPos_(handle->devLightPos),
    lightRadius_(handle->lightRadius), ambientColor_(handle->ambientColor),
    spotColor_(handle->spotColor), flag_(handle->flag)
{
    path_ = CmdListHelper::GetPathFromCmdList(cmdList, handle->path);
}

std::shared_ptr<DrawOpItem> DrawShadowOpItem::Unmarshalling(const CmdList& cmdList, void* handle)
{
    return std::make_shared<DrawShadowOpItem>(cmdList, static_cast<DrawShadowOpItem::ConstructorHandle*>(handle));
}

void DrawShadowOpItem::Playback(Canvas* canvas, const Rect* rect)
{
    if (path_ == nullptr) {
        LOGE("DrawShadowOpItem path is null!");
        return;
    }
    canvas->DrawShadow(*path_, planeParams_, devLightPos_, lightRadius_,
                       ambientColor_, spotColor_, flag_);
}

/* DrawRegionOpItem */
DrawRegionOpItem::DrawRegionOpItem(const CmdList& cmdList, DrawRegionOpItem::ConstructorHandle* handle)
    : DrawOpItem(REGION_OPITEM)
{
    region_ = CmdListHelper::GetFromCmdList<RegionCmdList, Region>(cmdList, handle->region);
}

std::shared_ptr<DrawOpItem> DrawRegionOpItem::Unmarshalling(const CmdList& cmdList, void* handle)
{
    return std::make_shared<DrawRegionOpItem>(cmdList, static_cast<DrawRegionOpItem::ConstructorHandle*>(handle));
}

void DrawRegionOpItem::Playback(Canvas* canvas, const Rect* rect)
{
    if (region_ == nullptr) {
        LOGE("DrawRegionOpItem region is nullptr!");
        return;
    }
    canvas->DrawRegion(*region_);
}

/* DrawPatchOpItem */
DrawPatchOpItem::DrawPatchOpItem(const CmdList& cmdList, DrawPatchOpItem::ConstructorHandle* handle)
    : DrawOpItem(PATCH_OPITEM), mode_(handle->mode)
{
    cubics_ = CmdListHelper::GetVectorFromCmdList<Point>(cmdList, handle->cubics);
    colors_ = CmdListHelper::GetVectorFromCmdList<ColorQuad>(cmdList, handle->colors);
    texCoords_ = CmdListHelper::GetVectorFromCmdList<Point>(cmdList, handle->texCoords);
}

std::shared_ptr<DrawOpItem> DrawPatchOpItem::Unmarshalling(const CmdList& cmdList, void* handle)
{
    return std::make_shared<DrawPatchOpItem>(cmdList, static_cast<DrawPatchOpItem::ConstructorHandle*>(handle));
}

void DrawPatchOpItem::Playback(Canvas* canvas, const Rect* rect)
{
    canvas->DrawPatch(cubics_.empty() ? nullptr : cubics_.data(),
                      colors_.empty() ? nullptr : colors_.data(),
                      texCoords_.empty() ? nullptr : texCoords_.data(),
                      mode_);
}

/* DrawEdgeAAQuadOpItem */
DrawEdgeAAQuadOpItem::DrawEdgeAAQuadOpItem(const CmdList& cmdList, DrawEdgeAAQuadOpItem::ConstructorHandle* handle)
    : DrawOpItem(EDGEAAQUAD_OPITEM), rect_(handle->rect),  aaFlags_(handle->aaFlags),
    color_(handle->color), mode_(handle->mode)
{
    clipQuad_ = CmdListHelper::GetVectorFromCmdList<Point>(cmdList, handle->clipQuad);
}

std::shared_ptr<DrawOpItem> DrawEdgeAAQuadOpItem::Unmarshalling(const CmdList& cmdList, void* handle)
{
    return std::make_shared<DrawEdgeAAQuadOpItem>(
        cmdList, static_cast<DrawEdgeAAQuadOpItem::ConstructorHandle*>(handle));
}

void DrawEdgeAAQuadOpItem::Playback(Canvas* canvas, const Rect* rect)
{
    canvas->DrawEdgeAAQuad(rect_, clipQuad_.empty() ? nullptr : clipQuad_.data(), aaFlags_, color_, mode_);
}

/* DrawVerticesOpItem */
DrawVerticesOpItem::DrawVerticesOpItem(const CmdList& cmdList, DrawVerticesOpItem::ConstructorHandle* handle)
    : DrawOpItem(VERTICES_OPITEM), mode_(handle->mode)
{
    vertices_ = CmdListHelper::GetVerticesFromCmdList(cmdList, handle->vertices);
}

std::shared_ptr<DrawOpItem> DrawVerticesOpItem::Unmarshalling(const CmdList& cmdList, void* handle)
{
    return std::make_shared<DrawVerticesOpItem>(cmdList, static_cast<DrawVerticesOpItem::ConstructorHandle*>(handle));
}

void DrawVerticesOpItem::Playback(Canvas* canvas, const Rect* rect)
{
    if (vertices_ == nullptr) {
        LOGE("DrawVerticesOpItem vertices is null");
        return;
    }
    canvas->DrawVertices(*vertices_, mode_);
}

/* DrawColorOpItem */
DrawColorOpItem::DrawColorOpItem(DrawColorOpItem::ConstructorHandle* handle)
    : DrawOpItem(COLOR_OPITEM), color_(handle->color), mode_(handle->mode) {}

std::shared_ptr<DrawOpItem> DrawColorOpItem::Unmarshalling(const CmdList& cmdList, void* handle)
{
    return std::make_shared<DrawColorOpItem>(static_cast<DrawColorOpItem::ConstructorHandle*>(handle));
}

void DrawColorOpItem::Playback(Canvas* canvas, const Rect* rect)
{
    canvas->DrawColor(color_, mode_);
}

/* DrawImageNineOpItem */
DrawImageNineOpItem::DrawImageNineOpItem(const CmdList& cmdList, DrawImageNineOpItem::ConstructorHandle* handle)
    : DrawOpItem(IMAGE_NINE_OPITEM), center_(handle->center), dst_(handle->dst), filter_(handle->filter),
    hasBrush_(handle->hasBrush)
{
    image_ = CmdListHelper::GetImageFromCmdList(cmdList, handle->image);
    if (hasBrush_) {
        BrushHandleToBrush(handle->brushHandle, cmdList, brush_);
    }
}

std::shared_ptr<DrawOpItem> DrawImageNineOpItem::Unmarshalling(const CmdList& cmdList, void* handle)
{
    return std::make_shared<DrawImageNineOpItem>(cmdList, static_cast<DrawImageNineOpItem::ConstructorHandle*>(handle));
}

void DrawImageNineOpItem::Playback(Canvas* canvas, const Rect* rect)
{
    if (image_ == nullptr) {
        LOGE("DrawImageNineOpItem image is null");
        return;
    }
    Brush* brushPtr = hasBrush_ ? &brush_ : nullptr;
    canvas->DrawImageNine(image_.get(), center_, dst_, filter_, brushPtr);
}

/* DrawAnnotationOpItem */
DrawAnnotationOpItem::DrawAnnotationOpItem(const CmdList& cmdList, DrawAnnotationOpItem::ConstructorHandle* handle)
    : DrawOpItem(IMAGE_ANNOTATION_OPITEM), rect_(handle->rect), key_(handle->key)
{
    data_ = CmdListHelper::GetDataFromCmdList(cmdList, handle->data);
}

std::shared_ptr<DrawOpItem> DrawAnnotationOpItem::Unmarshalling(const CmdList& cmdList, void* handle)
{
    return std::make_shared<DrawAnnotationOpItem>(
        cmdList, static_cast<DrawAnnotationOpItem::ConstructorHandle*>(handle));
}

void DrawAnnotationOpItem::Playback(Canvas* canvas, const Rect* rect)
{
    if (data_ == nullptr) {
        LOGE("DrawAnnotationOpItem data is null");
        return;
    }
    canvas->DrawAnnotation(rect_, key_, data_.get());
}

/* DrawImageLatticeOpItem */
DrawImageLatticeOpItem::DrawImageLatticeOpItem(
    const CmdList& cmdList, DrawImageLatticeOpItem::ConstructorHandle* handle)
    : DrawOpItem(IMAGE_LATTICE_OPITEM), lattice_(handle->lattice), dst_(handle->dst), filter_(handle->filter),
    hasBrush_(handle->hasBrush)
{
    image_ = CmdListHelper::GetImageFromCmdList(cmdList, handle->image);
    if (hasBrush_) {
        BrushHandleToBrush(handle->brushHandle, cmdList, brush_);
    }
}

std::shared_ptr<DrawOpItem> DrawImageLatticeOpItem::Unmarshalling(const CmdList& cmdList, void* handle)
{
    return std::make_shared<DrawImageLatticeOpItem>(
        cmdList, static_cast<DrawImageLatticeOpItem::ConstructorHandle*>(handle));
}

void DrawImageLatticeOpItem::Playback(Canvas* canvas, const Rect* rect)
{
    if (image_ == nullptr) {
        LOGE("DrawImageNineOpItem image is null");
        return;
    }
    Brush* brushPtr = hasBrush_ ? &brush_ : nullptr;
    canvas->DrawImageLattice(image_.get(), lattice_, dst_, filter_, brushPtr);
}

/* DrawBitmapOpItem */
DrawBitmapOpItem::DrawBitmapOpItem(const CmdList& cmdList, DrawBitmapOpItem::ConstructorHandle* handle)
    : DrawOpItem(BITMAP_OPITEM), px_(handle->px), py_(handle->py)
{
    bitmap_ = CmdListHelper::GetBitmapFromCmdList(cmdList, handle->bitmap);
}

std::shared_ptr<DrawOpItem> DrawBitmapOpItem::Unmarshalling(const CmdList& cmdList, void* handle)
{
    return std::make_shared<DrawBitmapOpItem>(cmdList, static_cast<DrawBitmapOpItem::ConstructorHandle*>(handle));
}

void DrawBitmapOpItem::Playback(Canvas* canvas, const Rect* rect)
{
    if (bitmap_ == nullptr) {
        LOGE("DrawBitmapOpItem bitmap is null");
        return;
    }
    canvas->DrawBitmap(*bitmap_, px_, py_);
}

/* DrawImageOpItem */
DrawImageOpItem::DrawImageOpItem(const CmdList& cmdList, DrawImageOpItem::ConstructorHandle* handle)
    : DrawOpItem(IMAGE_OPITEM), px_(handle->px), py_(handle->py), samplingOptions_(handle->samplingOptions)
{
    image_ = CmdListHelper::GetImageFromCmdList(cmdList, handle->image);
}

std::shared_ptr<DrawOpItem> DrawImageOpItem::Unmarshalling(const CmdList& cmdList, void* handle)
{
    return std::make_shared<DrawImageOpItem>(cmdList, static_cast<DrawImageOpItem::ConstructorHandle*>(handle));
}

void DrawImageOpItem::Playback(Canvas* canvas, const Rect* rect)
{
    if (image_ == nullptr) {
        LOGE("DrawImageOpItem image is null");
        return;
    }
    canvas->DrawImage(*image_, px_, py_, samplingOptions_);
}

/* DrawImageRectOpItem */
DrawImageRectOpItem::DrawImageRectOpItem(const CmdList& cmdList, DrawImageRectOpItem::ConstructorHandle* handle)
    : DrawOpItem(IMAGE_RECT_OPITEM), src_(handle->src), dst_(handle->dst), sampling_(handle->sampling),
    constraint_(handle->constraint)
{
    image_ = CmdListHelper::GetImageFromCmdList(cmdList, handle->image);
}

std::shared_ptr<DrawOpItem> DrawImageRectOpItem::Unmarshalling(const CmdList& cmdList, void* handle)
{
    return std::make_shared<DrawImageRectOpItem>(cmdList, static_cast<DrawImageRectOpItem::ConstructorHandle*>(handle));
}

void DrawImageRectOpItem::Playback(Canvas* canvas, const Rect* rect)
{
    if (image_ == nullptr) {
        LOGE("DrawImageRectOpItem image is null");
        return;
    }
    canvas->DrawImageRect(*image_, src_, dst_, sampling_, constraint_);
}

/* DrawPictureOpItem */
DrawPictureOpItem::DrawPictureOpItem(const CmdList& cmdList, DrawPictureOpItem::ConstructorHandle* handle)
    : DrawOpItem(PICTURE_OPITEM)
{
    picture_ = CmdListHelper::GetPictureFromCmdList(cmdList, handle->picture);
}

std::shared_ptr<DrawOpItem> DrawPictureOpItem::Unmarshalling(const CmdList& cmdList, void* handle)
{
    return std::make_shared<DrawPictureOpItem>(cmdList, static_cast<DrawPictureOpItem::ConstructorHandle*>(handle));
}

void DrawPictureOpItem::Playback(Canvas* canvas, const Rect* rect)
{
    if (picture_ == nullptr) {
        LOGE("DrawPictureOpItem picture is null");
        return;
    }
    canvas->DrawPicture(*picture_);
}

/* DrawTextBlobOpItem */
DrawTextBlobOpItem::DrawTextBlobOpItem(const CmdList& cmdList, DrawTextBlobOpItem::ConstructorHandle* handle)
    : DrawOpItem(TEXT_BLOB_OPITEM), x_(handle->x), y_(handle->y)
{
    textBlob_ = CmdListHelper::GetTextBlobFromCmdList(cmdList, handle->textBlob);
}

std::shared_ptr<DrawOpItem> DrawTextBlobOpItem::Unmarshalling(const CmdList& cmdList, void* handle)
{
    return std::make_shared<DrawTextBlobOpItem>(cmdList, static_cast<DrawTextBlobOpItem::ConstructorHandle*>(handle));
}

void DrawTextBlobOpItem::Playback(Canvas* canvas, const Rect* rect)
{
    if (textBlob_ == nullptr) {
        LOGE("DrawTextBlobOpItem textBlob is null");
        return;
    }
    canvas->DrawTextBlob(textBlob_.get(), x_, y_);
}

bool DrawTextBlobOpItem::ConstructorHandle::GenerateCachedOpItem(
    std::shared_ptr<CmdList> cacheCmdList, const TextBlob* textBlob)
{
    if (!textBlob) {
        LOGE("textBlob nullptr, %{public}s, %{public}d", __FUNCTION__, __LINE__);
        return false;
    }

    auto bounds = textBlob->Bounds();
    if (!bounds || !bounds->IsValid()) {
        return false;
    }
    bounds->Offset(x, y);

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

    offscreenCanvas->DrawTextBlob(textBlob, x, y);

    std::shared_ptr<Image> image = offscreenSurface->GetImageSnapshot();

    Drawing::Rect src(0, 0, image->GetWidth(), image->GetHeight());
    Drawing::Rect dst(bounds->GetLeft(), bounds->GetTop(), bounds->GetRight(), bounds->GetBottom());
    SamplingOptions sampling;
    auto imageHandle = CmdListHelper::AddImageToCmdList(*cacheCmdList, image);
    cacheCmdList->AddOp<DrawImageRectOpItem::ConstructorHandle>(
        imageHandle, src, dst, sampling, SrcRectConstraint::FAST_SRC_RECT_CONSTRAINT);
    return true;
}

bool DrawTextBlobOpItem::ConstructorHandle::GenerateCachedOpItem(CmdList& cmdList, Canvas* canvas,
    AttachPenOpItem::ConstructorHandle* penOpHandle, AttachBrushOpItem::ConstructorHandle* brushOpHandle)
{
    std::shared_ptr<TextBlob> textBlob_ = CmdListHelper::GetTextBlobFromCmdList(cmdList, textBlob);
    if (!textBlob_) {
        LOGE("textBlob nullptr, %{public}s, %{public}d", __FUNCTION__, __LINE__);
        return false;
    }

    auto bounds = textBlob_->Bounds();
    if (!bounds || !bounds->IsValid()) {
        return false;
    }
    bounds->Offset(x, y);

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
        return false;
    }

    Canvas* offscreenCanvas = offscreenSurface->GetCanvas().get();

    // align draw op to [0, 0]
    if (bounds->GetLeft() != 0 || bounds->GetTop() != 0) {
        offscreenCanvas->Translate(-bounds->GetLeft(), -bounds->GetTop());
    }

    if (penOpHandle) {
        AttachPenOpItem::Unmarshalling(cmdList, penOpHandle)->Playback(offscreenCanvas, nullptr);
    }

    if (brushOpHandle) {
        AttachBrushOpItem::Unmarshalling(cmdList, brushOpHandle)->Playback(offscreenCanvas, nullptr);
    }

    offscreenCanvas->DrawTextBlob(textBlob_.get(), x, y);

    std::shared_ptr<Image> image = offscreenSurface->GetImageSnapshot();
    Drawing::Rect src(0, 0, image->GetWidth(), image->GetHeight());
    Drawing::Rect dst(bounds->GetLeft(), bounds->GetTop(),
        bounds->GetLeft() + image->GetWidth(), bounds->GetTop() + image->GetHeight());
    SamplingOptions sampling;
    auto imageHandle = CmdListHelper::AddImageToCmdList(cmdList, image);
    cmdList.AddOp<DrawImageRectOpItem::ConstructorHandle>(imageHandle, src, dst, sampling,
        SrcRectConstraint::FAST_SRC_RECT_CONSTRAINT);
    return true;
}

std::shared_ptr<ImageSnapshotOpItem> DrawTextBlobOpItem::GenerateCachedOpItem(Canvas* canvas,
    AttachPenOpItem* penOpItem, AttachBrushOpItem* brushOpItem)
{
    if (!textBlob_) {
        LOGE("textBlob nullptr, %{public}s, %{public}d", __FUNCTION__, __LINE__);
        return nullptr;
    }

    auto bounds = textBlob_->Bounds();
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

    Canvas* offscreenCanvas = offscreenSurface->GetCanvas().get();

    // align draw op to [0, 0]
    if (bounds->GetLeft() != 0 || bounds->GetTop() != 0) {
        offscreenCanvas->Translate(-bounds->GetLeft(), -bounds->GetTop());
    }

    if (penOpItem) {
        penOpItem->Playback(offscreenCanvas, nullptr);
    }

    if (brushOpItem) {
        brushOpItem->Playback(offscreenCanvas, nullptr);
    }

    offscreenCanvas->DrawTextBlob(textBlob_.get(), x_, y_);

    std::shared_ptr<Image> image = offscreenSurface->GetImageSnapshot();
    Drawing::Rect src(0, 0, image->GetWidth(), image->GetHeight());
    Drawing::Rect dst(bounds->GetLeft(), bounds->GetTop(), bounds->GetRight(), bounds->GetBottom());
    SamplingOptions sampling;
    return std::make_shared<ImageSnapshotOpItem>(image, src, dst, sampling,
        SrcRectConstraint::FAST_SRC_RECT_CONSTRAINT);
}

/* ClipRectOpItem */
ClipRectOpItem::ClipRectOpItem(ClipRectOpItem::ConstructorHandle* handle)
    : DrawOpItem(CLIP_RECT_OPITEM), rect_(handle->rect), clipOp_(handle->clipOp), doAntiAlias_(handle->doAntiAlias) {}

std::shared_ptr<DrawOpItem> ClipRectOpItem::Unmarshalling(const CmdList& cmdList, void* handle)
{
    return std::make_shared<ClipRectOpItem>(static_cast<ClipRectOpItem::ConstructorHandle*>(handle));
}

void ClipRectOpItem::Playback(Canvas* canvas, const Rect* rect)
{
    canvas->ClipRect(rect_, clipOp_, doAntiAlias_);
}

/* ClipIRectOpItem */
ClipIRectOpItem::ClipIRectOpItem(ClipIRectOpItem::ConstructorHandle* handle)
    : DrawOpItem(CLIP_IRECT_OPITEM), rect_(handle->rect), clipOp_(handle->clipOp) {}

std::shared_ptr<DrawOpItem> ClipIRectOpItem::Unmarshalling(const CmdList& cmdList, void* handle)
{
    return std::make_shared<ClipIRectOpItem>(static_cast<ClipIRectOpItem::ConstructorHandle*>(handle));
}

void ClipIRectOpItem::Playback(Canvas* canvas, const Rect* rect)
{
    canvas->ClipIRect(rect_, clipOp_);
}

/* ClipRoundRectOpItem */
ClipRoundRectOpItem::ClipRoundRectOpItem(ClipRoundRectOpItem::ConstructorHandle* handle)
    : DrawOpItem(CLIP_ROUND_RECT_OPITEM), rrect_(handle->rrect), clipOp_(handle->clipOp),
    doAntiAlias_(handle->doAntiAlias) {}

std::shared_ptr<DrawOpItem> ClipRoundRectOpItem::Unmarshalling(const CmdList& cmdList, void* handle)
{
    return std::make_shared<ClipRoundRectOpItem>(static_cast<ClipRoundRectOpItem::ConstructorHandle*>(handle));
}

void ClipRoundRectOpItem::Playback(Canvas* canvas, const Rect* rect)
{
    canvas->ClipRoundRect(rrect_, clipOp_, doAntiAlias_);
}

/* ClipPathOpItem */
ClipPathOpItem::ClipPathOpItem(const CmdList& cmdList, ClipPathOpItem::ConstructorHandle* handle)
    : DrawOpItem(CLIP_PATH_OPITEM), clipOp_(handle->clipOp), doAntiAlias_(handle->doAntiAlias)
{
    path_ = CmdListHelper::GetPathFromCmdList(cmdList, handle->path);
}

std::shared_ptr<DrawOpItem> ClipPathOpItem::Unmarshalling(const CmdList& cmdList, void* handle)
{
    return std::make_shared<ClipPathOpItem>(cmdList, static_cast<ClipPathOpItem::ConstructorHandle*>(handle));
}

void ClipPathOpItem::Playback(Canvas* canvas, const Rect* rect)
{
    if (path_ == nullptr) {
        LOGE("ClipPathOpItem path is null!");
        return;
    }
    canvas->ClipPath(*path_, clipOp_, doAntiAlias_);
}

/* ClipRegionOpItem */
ClipRegionOpItem::ClipRegionOpItem(const CmdList& cmdList, ClipRegionOpItem::ConstructorHandle* handle)
    : DrawOpItem(CLIP_REGION_OPITEM), clipOp_(handle->clipOp)
{
    region_ = CmdListHelper::GetFromCmdList<RegionCmdList, Region>(cmdList, handle->region);
}

std::shared_ptr<DrawOpItem> ClipRegionOpItem::Unmarshalling(const CmdList& cmdList, void* handle)
{
    return std::make_shared<ClipRegionOpItem>(cmdList, static_cast<ClipRegionOpItem::ConstructorHandle*>(handle));
}

void ClipRegionOpItem::Playback(Canvas* canvas, const Rect* rect)
{
    if (region_ == nullptr) {
        LOGE("ClipRegionOpItem region is null!");
        return;
    }
    canvas->ClipRegion(*region_, clipOp_);
}

/* SetMatrixOpItem */
SetMatrixOpItem::SetMatrixOpItem(SetMatrixOpItem::ConstructorHandle* handle) : DrawOpItem(SET_MATRIX_OPITEM)
{
    matrix_.SetAll(handle->matrixBuffer);
}

std::shared_ptr<DrawOpItem> SetMatrixOpItem::Unmarshalling(const CmdList& cmdList, void* handle)
{
    return std::make_shared<SetMatrixOpItem>(static_cast<SetMatrixOpItem::ConstructorHandle*>(handle));
}

void SetMatrixOpItem::Playback(Canvas* canvas, const Rect* rect)
{
    canvas->SetMatrix(matrix_);
}

/* ResetMatrixOpItem */
ResetMatrixOpItem::ResetMatrixOpItem() : DrawOpItem(RESET_MATRIX_OPITEM) {}

std::shared_ptr<DrawOpItem> ResetMatrixOpItem::Unmarshalling(const CmdList& cmdList, void* handle)
{
    return std::make_shared<ResetMatrixOpItem>();
}

void ResetMatrixOpItem::Playback(Canvas* canvas, const Rect* rect)
{
    canvas->ResetMatrix();
}

/* ConcatMatrixOpItem */
ConcatMatrixOpItem::ConcatMatrixOpItem(ConcatMatrixOpItem::ConstructorHandle* handle) : DrawOpItem(CONCAT_MATRIX_OPITEM)
{
    matrix_.SetAll(handle->matrixBuffer);
}

std::shared_ptr<DrawOpItem> ConcatMatrixOpItem::Unmarshalling(const CmdList& cmdList, void* handle)
{
    return std::make_shared<ConcatMatrixOpItem>(static_cast<ConcatMatrixOpItem::ConstructorHandle*>(handle));
}

void ConcatMatrixOpItem::Playback(Canvas* canvas, const Rect* rect)
{
    canvas->ConcatMatrix(matrix_);
}

/* TranslateOpItem */
TranslateOpItem::TranslateOpItem(TranslateOpItem::ConstructorHandle* handle)
    : DrawOpItem(TRANSLATE_OPITEM), dx_(handle->dx), dy_(handle->dy) {}

std::shared_ptr<DrawOpItem> TranslateOpItem::Unmarshalling(const CmdList& cmdList, void* handle)
{
    return std::make_shared<TranslateOpItem>(static_cast<TranslateOpItem::ConstructorHandle*>(handle));
}

void TranslateOpItem::Playback(Canvas* canvas, const Rect* rect)
{
    canvas->Translate(dx_, dy_);
}

/* ScaleOpItem */
ScaleOpItem::ScaleOpItem(ScaleOpItem::ConstructorHandle* handle)
    : DrawOpItem(SCALE_OPITEM), sx_(handle->sx), sy_(handle->sy) {}

std::shared_ptr<DrawOpItem> ScaleOpItem::Unmarshalling(const CmdList& cmdList, void* handle)
{
    return std::make_shared<ScaleOpItem>(static_cast<ScaleOpItem::ConstructorHandle*>(handle));
}

void ScaleOpItem::Playback(Canvas* canvas, const Rect* rect)
{
    canvas->Scale(sx_, sy_);
}

/* RotateOpItem */
RotateOpItem::RotateOpItem(RotateOpItem::ConstructorHandle* handle)
    : DrawOpItem(ROTATE_OPITEM), deg_(handle->deg), sx_(handle->sx), sy_(handle->sy) {}

std::shared_ptr<DrawOpItem> RotateOpItem::Unmarshalling(const CmdList& cmdList, void* handle)
{
    return std::make_shared<RotateOpItem>(static_cast<RotateOpItem::ConstructorHandle*>(handle));
}

void RotateOpItem::Playback(Canvas* canvas, const Rect* rect)
{
    canvas->Rotate(deg_, sx_, sy_);
}

/* ShearOpItem */
ShearOpItem::ShearOpItem(ShearOpItem::ConstructorHandle* handle)
    : DrawOpItem(SHEAR_OPITEM), sx_(handle->sx), sy_(handle->sy) {}

std::shared_ptr<DrawOpItem> ShearOpItem::Unmarshalling(const CmdList& cmdList, void* handle)
{
    return std::make_shared<ShearOpItem>(static_cast<ShearOpItem::ConstructorHandle*>(handle));
}

void ShearOpItem::Playback(Canvas* canvas, const Rect* rect)
{
    canvas->Shear(sx_, sy_);
}

/* FlushOpItem */
FlushOpItem::FlushOpItem() : DrawOpItem(FLUSH_OPITEM) {}

std::shared_ptr<DrawOpItem> FlushOpItem::Unmarshalling(const CmdList& cmdList, void* handle)
{
    return std::make_shared<FlushOpItem>();
}

void FlushOpItem::Playback(Canvas* canvas, const Rect* rect)
{
    canvas->Flush();
}

/* ClearOpItem */
ClearOpItem::ClearOpItem(ClearOpItem::ConstructorHandle* handle)
    : DrawOpItem(CLEAR_OPITEM), color_(handle->color) {}

std::shared_ptr<DrawOpItem> ClearOpItem::Unmarshalling(const CmdList& cmdList, void* handle)
{
    return std::make_shared<ClearOpItem>(static_cast<ClearOpItem::ConstructorHandle*>(handle));
}

void ClearOpItem::Playback(Canvas* canvas, const Rect* rect)
{
    canvas->Clear(color_);
}

/* SaveOpItem */
SaveOpItem::SaveOpItem() : DrawOpItem(SAVE_OPITEM) {}

std::shared_ptr<DrawOpItem> SaveOpItem::Unmarshalling(const CmdList& cmdList, void* handle)
{
    return std::make_shared<SaveOpItem>();
}

void SaveOpItem::Playback(Canvas* canvas, const Rect* rect)
{
    canvas->Save();
}

/* SaveLayerOpItem */
SaveLayerOpItem::SaveLayerOpItem(const CmdList& cmdList, SaveLayerOpItem::ConstructorHandle* handle)
    : DrawOpItem(SAVE_LAYER_OPITEM), rect_(handle->rect), saveLayerFlags_(handle->saveLayerFlags),
    hasBrush_(handle->hasBrush)
{
    if (hasBrush_) {
        BrushHandleToBrush(handle->brushHandle, cmdList, brush_);
    }

    imageFilter_ = CmdListHelper::GetFromCmdList<ImageFilterCmdList, ImageFilter>(cmdList, handle->imageFilter);
}

std::shared_ptr<DrawOpItem> SaveLayerOpItem::Unmarshalling(const CmdList& cmdList, void* handle)
{
    return std::make_shared<SaveLayerOpItem>(cmdList, static_cast<SaveLayerOpItem::ConstructorHandle*>(handle));
}

void SaveLayerOpItem::Playback(Canvas* canvas, const Rect* rect)
{
    const Rect* rectPtr = nullptr;
    if (rect_.IsValid()) {
        rectPtr = &rect_;
    }
    Brush* brushPtr = hasBrush_ ? &brush_ : nullptr;
    SaveLayerOps slo(rectPtr, brushPtr, imageFilter_.get(), saveLayerFlags_);
    canvas->SaveLayer(slo);
}

/* RestoreOpItem */
RestoreOpItem::RestoreOpItem() : DrawOpItem(RESTORE_OPITEM) {}

std::shared_ptr<DrawOpItem> RestoreOpItem::Unmarshalling(const CmdList& cmdList, void* handle)
{
    return std::make_shared<RestoreOpItem>();
}

void RestoreOpItem::Playback(Canvas* canvas, const Rect* rect)
{
    canvas->Restore();
}

/* DiscardOpItem */
DiscardOpItem::DiscardOpItem() : DrawOpItem(DISCARD_OPITEM) {}

std::shared_ptr<DrawOpItem> DiscardOpItem::Unmarshalling(const CmdList& cmdList, void* handle)
{
    return std::make_shared<DiscardOpItem>();
}

void DiscardOpItem::Playback(Canvas* canvas, const Rect* rect)
{
    canvas->Discard();
}

/* AttachPenOpItem */
AttachPenOpItem::AttachPenOpItem(const CmdList& cmdList, AttachPenOpItem::ConstructorHandle* handle)
    : DrawOpItem(ATTACH_PEN_OPITEM)
{
    PenHandleToPen(handle->penHandle, cmdList, pen_);
}

std::shared_ptr<DrawOpItem> AttachPenOpItem::Unmarshalling(const CmdList& cmdList, void* handle)
{
    return std::make_shared<AttachPenOpItem>(cmdList, static_cast<AttachPenOpItem::ConstructorHandle*>(handle));
}

void AttachPenOpItem::Playback(Canvas* canvas, const Rect* rect)
{
    canvas->AttachPen(pen_);
}

/* AttachBrushOpItem */
AttachBrushOpItem::AttachBrushOpItem(const CmdList& cmdList, AttachBrushOpItem::ConstructorHandle* handle)
    : DrawOpItem(ATTACH_BRUSH_OPITEM)
{
    BrushHandleToBrush(handle->brushHandle, cmdList, brush_);
}

std::shared_ptr<DrawOpItem> AttachBrushOpItem::Unmarshalling(const CmdList& cmdList, void* handle)
{
    return std::make_shared<AttachBrushOpItem>(cmdList, static_cast<AttachBrushOpItem::ConstructorHandle*>(handle));
}

void AttachBrushOpItem::Playback(Canvas* canvas, const Rect* rect)
{
    canvas->AttachBrush(brush_);
}

/* DetachPenOpItem */
DetachPenOpItem::DetachPenOpItem() : DrawOpItem(DETACH_PEN_OPITEM) {}

std::shared_ptr<DrawOpItem> DetachPenOpItem::Unmarshalling(const CmdList& cmdList, void* handle)
{
    return std::make_shared<DetachPenOpItem>();
}

void DetachPenOpItem::Playback(Canvas* canvas, const Rect* rect)
{
    canvas->DetachPen();
}

/* DetachBrushOpItem */
DetachBrushOpItem::DetachBrushOpItem() : DrawOpItem(DETACH_BRUSH_OPITEM) {}

std::shared_ptr<DrawOpItem> DetachBrushOpItem::Unmarshalling(const CmdList& cmdList, void* handle)
{
    return std::make_shared<DetachBrushOpItem>();
}

void DetachBrushOpItem::Playback(Canvas* canvas, const Rect* rect)
{
    canvas->DetachBrush();
}

/* ClipAdaptiveRoundRectOpItem */
ClipAdaptiveRoundRectOpItem::ClipAdaptiveRoundRectOpItem(
    const CmdList& cmdList, ClipAdaptiveRoundRectOpItem::ConstructorHandle* handle)
    : DrawOpItem(CLIP_ADAPTIVE_ROUND_RECT_OPITEM)
{
    radiusData_ = CmdListHelper::GetVectorFromCmdList<Point>(cmdList, handle->radiusData);
}

std::shared_ptr<DrawOpItem> ClipAdaptiveRoundRectOpItem::Unmarshalling(const CmdList& cmdList, void* handle)
{
    return std::make_shared<ClipAdaptiveRoundRectOpItem>(
        cmdList, static_cast<ClipAdaptiveRoundRectOpItem::ConstructorHandle*>(handle));
}

void ClipAdaptiveRoundRectOpItem::Playback(Canvas* canvas, const Rect* rect)
{
    auto roundRect = RoundRect(*rect, radiusData_);
    canvas->ClipRoundRect(roundRect, ClipOp::INTERSECT, true);
}

/* DrawAdaptiveImageOpItem */
DrawAdaptiveImageOpItem::DrawAdaptiveImageOpItem(
    const CmdList& cmdList, DrawAdaptiveImageOpItem::ConstructorHandle* handle) : DrawOpItem(ADAPTIVE_IMAGE_OPITEM),
    rsImageInfo_(handle->rsImageInfo), sampling_(handle->sampling), isImage_(handle->isImage)
{
    if (isImage_) {
        image_ = CmdListHelper::GetImageFromCmdList(cmdList, handle->image);
    } else {
        data_ = CmdListHelper::GetCompressDataFromCmdList(cmdList, handle->image);
    }
}

std::shared_ptr<DrawOpItem> DrawAdaptiveImageOpItem::Unmarshalling(const CmdList& cmdList, void* handle)
{
    return std::make_shared<DrawAdaptiveImageOpItem>(
        cmdList, static_cast<DrawAdaptiveImageOpItem::ConstructorHandle*>(handle));
}

void DrawAdaptiveImageOpItem::Playback(Canvas* canvas, const Rect* rect)
{
    if (isImage_ && image_ != nullptr) {
        AdaptiveImageHelper::DrawImage(*canvas, *rect, image_, rsImageInfo_, sampling_);
        return;
    }
    if (!isImage_ && data_ != nullptr) {
        AdaptiveImageHelper::DrawImage(*canvas, *rect, data_, rsImageInfo_, sampling_);
    }
}

/* DrawAdaptivePixelMapOpItem */
DrawAdaptivePixelMapOpItem::DrawAdaptivePixelMapOpItem(
    const CmdList& cmdList, DrawAdaptivePixelMapOpItem::ConstructorHandle* handle)
    : DrawOpItem(ADAPTIVE_PIXELMAP_OPITEM), imageInfo_(handle->imageInfo), smapling_(handle->smapling)
{
    pixelMap_ = CmdListHelper::GetPixelMapFromCmdList(cmdList, handle->pixelMap);
}

std::shared_ptr<DrawOpItem> DrawAdaptivePixelMapOpItem::Unmarshalling(const CmdList& cmdList, void* handle)
{
    return std::make_shared<DrawAdaptivePixelMapOpItem>(
        cmdList, static_cast<DrawAdaptivePixelMapOpItem::ConstructorHandle*>(handle));
}

void DrawAdaptivePixelMapOpItem::Playback(Canvas* canvas, const Rect* rect)
{
    if (pixelMap_ == nullptr) {
        LOGE("DrawAdaptivePixelMapOpItem pixelMap is null!");
        return;
    }
    AdaptiveImageHelper::DrawPixelMap(*canvas, *rect, pixelMap_, imageInfo_, smapling_);
}

/* DrawImageWithParmOpItem */
DrawImageWithParmOpItem::DrawImageWithParmOpItem(
    const CmdList& cmdList, DrawImageWithParmOpItem::ConstructorHandle* handle)
    : DrawOpItem(IMAGE_WITH_PARM_OPITEM), sampling_(handle->sampling)
{
    objectHandle_ = CmdListHelper::GetImageObjectFromCmdList(cmdList, handle->objectHandle);
}

std::shared_ptr<DrawOpItem> DrawImageWithParmOpItem::Unmarshalling(const CmdList& cmdList, void* handle)
{
    return std::make_shared<DrawImageWithParmOpItem>(
        cmdList, static_cast<DrawImageWithParmOpItem::ConstructorHandle*>(handle));
}

void DrawImageWithParmOpItem::Playback(Canvas* canvas, const Rect* rect)
{
    if (objectHandle_ == nullptr) {
        LOGE("DrawImageWithParmOpItem objectHandle is nullptr!");
        return;
    }
    objectHandle_->Playback(*canvas, *rect, sampling_, false);
}

/* DrawPixelMapWithParmOpItem */
DrawPixelMapWithParmOpItem::DrawPixelMapWithParmOpItem(
    const CmdList& cmdList, DrawPixelMapWithParmOpItem::ConstructorHandle* handle)
    : DrawOpItem(PIXELMAP_WITH_PARM_OPITEM), sampling_(handle->sampling)
{
    objectHandle_ = CmdListHelper::GetImageObjectFromCmdList(cmdList, handle->objectHandle);
}

std::shared_ptr<DrawOpItem> DrawPixelMapWithParmOpItem::Unmarshalling(const CmdList& cmdList, void* handle)
{
    return std::make_shared<DrawPixelMapWithParmOpItem>(
        cmdList, static_cast<DrawPixelMapWithParmOpItem::ConstructorHandle*>(handle));
}

void DrawPixelMapWithParmOpItem::Playback(Canvas* canvas, const Rect* rect)
{
    if (objectHandle_ == nullptr) {
        LOGE("DrawPixelMapWithParmOpItem objectHandle is nullptr!");
        return;
    }
    objectHandle_->Playback(*canvas, *rect, sampling_, false);
}

/* DrawPixelMapRectOpItem */
DrawPixelMapRectOpItem::DrawPixelMapRectOpItem(
    const CmdList& cmdList, DrawPixelMapRectOpItem::ConstructorHandle* handle)
    : DrawOpItem(PIXELMAP_RECT_OPITEM), sampling_(handle->sampling)
{
    objectHandle_ = CmdListHelper::GetImageBaseOjFromCmdList(cmdList, handle->objectHandle);
}

std::shared_ptr<DrawOpItem> DrawPixelMapRectOpItem::Unmarshalling(const CmdList& cmdList, void* handle)
{
    return std::make_shared<DrawPixelMapRectOpItem>(
        cmdList, static_cast<DrawPixelMapRectOpItem::ConstructorHandle*>(handle));
}

void DrawPixelMapRectOpItem::Playback(Canvas* canvas, const Rect* rect)
{
    if (objectHandle_ == nullptr) {
        LOGE("DrawPixelMapWithParmOpItem objectHandle is nullptr!");
        return;
    }
    objectHandle_->Playback(*canvas, *rect, sampling_);
}

ImageSnapshotOpItem::ImageSnapshotOpItem(std::shared_ptr<Image> image, const Rect& src, const Rect& dst,
    const SamplingOptions& sampling, SrcRectConstraint constraint)
    : DrawOpItem(IMAGE_SNAPSHOT_OPITEM),
    image_(image),
    src_(src),
    dst_(dst),
    sampling_(sampling),
    constraint_(constraint)
{
}

void ImageSnapshotOpItem::Playback(Canvas* canvas, const Rect* rect)
{
    if (image_ == nullptr) {
        return;
    }
    canvas->DrawImageRect(*image_, src_, dst_, sampling_, constraint_);
}
#ifdef ROSEN_OHOS
DrawSurfaceBufferOpItem::DrawSurfaceBufferOpItem(const CmdList& cmdList,
    DrawSurfaceBufferOpItem::ConstructorHandle* handle)
    : DrawOpItem(SURFACEBUFFER_OPITEM), surfaceBufferInfo_(nullptr, handle->surfaceBufferInfo.offSetX_,
    handle->surfaceBufferInfo.offSetY_, handle->surfaceBufferInfo.width_, handle->surfaceBufferInfo.height_)
{
    surfaceBufferInfo_.surfaceBuffer_ = CmdListHelper::GetSurfaceBufferFromCmdList(cmdList, handle->surfaceBufferId);
}

DrawSurfaceBufferOpItem::~DrawSurfaceBufferOpItem()
{
    Clear();
}

std::shared_ptr<DrawOpItem> DrawSurfaceBufferOpItem::Unmarshalling(const CmdList& cmdList, void* handle)
{
    return std::make_shared<DrawSurfaceBufferOpItem>(cmdList,
        static_cast<DrawSurfaceBufferOpItem::ConstructorHandle*>(handle));
}

#ifdef RS_ENABLE_VK
std::function<Drawing::BackendTexture(NativeWindowBuffer* buffer, int width, int height)>
    DrawSurfaceBufferOpItem::makeBackendTextureFromNativeBuffer = nullptr;
std::function<void(void* context)> DrawSurfaceBufferOpItem::deleteVkImage = nullptr;
std::function<void*(VkImage image, VkDeviceMemory memory)> DrawSurfaceBufferOpItem::vulkanCleanupHelper = nullptr;
void DrawSurfaceBufferOpItem::SetBaseCallback(
    std::function<Drawing::BackendTexture(NativeWindowBuffer* buffer, int width, int height)> makeBackendTexture,
    std::function<void(void* context)> deleteImage,
    std::function<void*(VkImage image, VkDeviceMemory memory)> helper)
{
    DrawSurfaceBufferOpItem::makeBackendTextureFromNativeBuffer = makeBackendTexture;
    DrawSurfaceBufferOpItem::deleteVkImage = deleteImage;
    DrawSurfaceBufferOpItem::vulkanCleanupHelper = helper;
}
#endif

void DrawSurfaceBufferOpItem::Playback(Canvas* canvas, const Rect* rect)
{
    Clear();
#if defined(RS_ENABLE_GL) || defined(RS_ENABLE_VK)
    if (surfaceBufferInfo_.surfaceBuffer_ == nullptr) {
        LOGE("SurfaceBufferOpItem::Draw surfaceBuffer_ is nullptr");
        return;
    }
    nativeWindowBuffer_ = CreateNativeWindowBufferFromSurfaceBuffer(&(surfaceBufferInfo_.surfaceBuffer_));
    if (!nativeWindowBuffer_) {
        LOGE("create nativeWindowBuffer_ fail.");
        return;
    }
#endif
    Draw(canvas);
}

void DrawSurfaceBufferOpItem::Clear()
{
#ifdef RS_ENABLE_GL
    if (texId_ != 0U) {
        glDeleteTextures(1, &texId_);
    }
    if (eglImage_ != EGL_NO_IMAGE_KHR) {
        auto disp = eglGetDisplay(EGL_DEFAULT_DISPLAY);
        eglDestroyImageKHR(disp, eglImage_);
    }
#endif
#if defined(RS_ENABLE_GL) || defined(RS_ENABLE_VK)
    if (nativeWindowBuffer_ != nullptr) {
        DestroyNativeWindowBuffer(nativeWindowBuffer_);
        nativeWindowBuffer_ = nullptr;
    }
#endif
}

void DrawSurfaceBufferOpItem::Draw(Canvas* canvas)
{
#ifdef RS_ENABLE_VK
    if (!DrawSurfaceBufferOpItem::makeBackendTextureFromNativeBuffer ||
        !DrawSurfaceBufferOpItem::deleteVkImage ||
        !DrawSurfaceBufferOpItem::vulkanCleanupHelper ||
        !canvas) {
        return;
    }
    auto backendTexture = DrawSurfaceBufferOpItem::makeBackendTextureFromNativeBuffer(nativeWindowBuffer_,
        surfaceBufferInfo_.width_, surfaceBufferInfo_.height_);
    Drawing::BitmapFormat bitmapFormat = { Drawing::ColorType::COLORTYPE_RGBA_8888,
        Drawing::AlphaType::ALPHATYPE_PREMUL };
    auto ptr = [](void* context) {
        DrawSurfaceBufferOpItem::deleteVkImage(context);
    }
    auto image = std::make_shared<Drawing::Image>();
    auto vkTextureInfo = backendTexture.GetTextureInfo().GetVKTextureInfo();
    if (!vkTextureInfo && !image->BuildFromTexture(*canvas->GetGPUContext(), backendTexture.GetTextureInfo(),
        Drawing::TextureOrigin::TOP_LEFT, bitmapFormat, nullptr, ptr,
        DrawSurfaceBufferOpItem::vulkanCleanupHelper(vkTextureInfo->vkImage, vkTextureInfo->vkAlloc.memory))) {
        LOGE("DrawSurfaceBufferOpItem::Draw: image BuildFromTexture failed");
        return;
    }
    canvas->DrawImage(*image, surfaceBufferInfo_.offSetX_, surfaceBufferInfo_.offSetY_, Drawing::SamplingOptions());
#endif

#ifdef RS_ENABLE_GL
    EGLint attrs[] = {
        EGL_IMAGE_PRESERVED,
        EGL_TRUE,
        EGL_NONE,
    };

    auto disp = eglGetDisplay(EGL_DEFAULT_DISPLAY);
    eglImage_ = eglCreateImageKHR(disp, EGL_NO_CONTEXT, EGL_NATIVE_BUFFER_OHOS, nativeWindowBuffer_, attrs);
    if (eglImage_ == EGL_NO_IMAGE_KHR) {
        DestroyNativeWindowBuffer(nativeWindowBuffer_);
        LOGE("%{public}s create egl image fail %{public}d", __func__, eglGetError());
        return;
    }

    // save
    GLuint originTexture;
    glGetIntegerv(GL_TEXTURE_BINDING_2D, reinterpret_cast<GLint *>(&originTexture));
    GLint minFilter;
    glGetTexParameteriv(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, &minFilter);
    GLint magFilter;
    glGetTexParameteriv(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, &magFilter);
    GLint wrapS;
    glGetTexParameteriv(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, &wrapS);
    GLint wrapT;
    glGetTexParameteriv(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, &wrapT);

    // Create texture object
    texId_ = 0;
    glGenTextures(1, &texId_);
    glBindTexture(GL_TEXTURE_2D, texId_);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glEGLImageTargetTexture2DOES(GL_TEXTURE_2D, static_cast<GLeglImageOES>(eglImage_));

    // restore
    glBindTexture(GL_TEXTURE_2D, originTexture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, minFilter);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, magFilter);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, wrapS);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, wrapT);

    GrGLTextureInfo textureInfo = { GL_TEXTURE_2D, texId_, GL_RGBA8_OES };

    GrBackendTexture backendTexture(
        surfaceBufferInfo_.width_, surfaceBufferInfo_.height_, GrMipMapped::kNo, textureInfo);

    Drawing::TextureInfo externalTextureInfo;
                externalTextureInfo.SetWidth(surfaceBufferInfo_.width_);
                externalTextureInfo.SetHeight(surfaceBufferInfo_.height_);
                externalTextureInfo.SetIsMipMapped(false);
                externalTextureInfo.SetTarget(GL_TEXTURE_2D);
                externalTextureInfo.SetID(texId_);
                externalTextureInfo.SetFormat(GL_RGBA8_OES);
    Drawing::BitmapFormat bitmapFormat = { Drawing::ColorType::COLORTYPE_RGBA_8888,
        Drawing::AlphaType::ALPHATYPE_PREMUL };
    auto newImage = std::make_shared<Drawing::Image>();
    if (!newImage->BuildFromTexture(*canvas->GetGPUContext(), externalTextureInfo,
        Drawing::TextureOrigin::TOP_LEFT, bitmapFormat, nullptr)) {
        LOGD("RSFilterCacheManager::Render: cacheCanvas is null");
        return;
    }
    canvas->DrawImage(*newImage, surfaceBufferInfo_.offSetX_, surfaceBufferInfo_.offSetY_, Drawing::SamplingOptions());
#endif // RS_ENABLE_GL
}
#endif
} // namespace Drawing
} // namespace Rosen
} // namespace OHOS
