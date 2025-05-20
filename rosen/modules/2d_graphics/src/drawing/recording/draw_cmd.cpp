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

#include <cstdint>
#include <sstream>

#include "recording/cmd_list_helper.h"
#include "recording/draw_cmd_list.h"
#include "recording/mem_allocator.h"
#include "recording/op_item.h"

#include "draw/brush.h"
#include "draw/color.h"
#include "draw/path.h"
#include "draw/surface.h"
#include "effect/color_filter.h"
#include "effect/color_space.h"
#include "effect/image_filter.h"
#include "effect/mask_filter.h"
#include "effect/path_effect.h"
#include "effect/shader_effect.h"
#include "utils/log.h"
#include "utils/scalar.h"
#include "utils/system_properties.h"
#include "sandbox_utils.h"

namespace OHOS {
namespace Rosen {
namespace Drawing {
std::unordered_map<uint32_t, std::string> typeOpDes = {
    { DrawOpItem::OPITEM_HEAD,              "OPITEM_HEAD"},
    { DrawOpItem::POINT_OPITEM,             "POINT_OPITEM" },
    { DrawOpItem::POINTS_OPITEM,            "POINTS_OPITEM" },
    { DrawOpItem::LINE_OPITEM,              "LINE_OPITEM" },
    { DrawOpItem::RECT_OPITEM,              "RECT_OPITEM" },
    { DrawOpItem::ROUND_RECT_OPITEM,        "ROUND_RECT_OPITEM" },
    { DrawOpItem::NESTED_ROUND_RECT_OPITEM, "NESTED_ROUND_RECT_OPITEM" },
    { DrawOpItem::ARC_OPITEM,               "ARC_OPITEM" },
    { DrawOpItem::PIE_OPITEM,               "PIE_OPITEM" },
    { DrawOpItem::OVAL_OPITEM,              "OVAL_OPITEM" },
    { DrawOpItem::CIRCLE_OPITEM,            "CIRCLE_OPITEM" },
    { DrawOpItem::PATH_OPITEM,              "PATH_OPITEM" },
    { DrawOpItem::BACKGROUND_OPITEM,        "BACKGROUND_OPITEM" },
    { DrawOpItem::SHADOW_OPITEM,            "SHADOW_OPITEM" },
    { DrawOpItem::SHADOW_STYLE_OPITEM,      "SHADOW_STYLE_OPITEM" },
    { DrawOpItem::COLOR_OPITEM,             "COLOR_OPITEM" },
    { DrawOpItem::IMAGE_NINE_OPITEM,        "IMAGE_NINE_OPITEM" },
    { DrawOpItem::IMAGE_LATTICE_OPITEM,     "IMAGE_LATTICE_OPITEM" },
    { DrawOpItem::ATLAS_OPITEM,             "ATLAS_OPITEM" },
    { DrawOpItem::BITMAP_OPITEM,            "BITMAP_OPITEM" },
    { DrawOpItem::IMAGE_OPITEM,             "IMAGE_OPITEM" },
    { DrawOpItem::IMAGE_RECT_OPITEM,        "IMAGE_RECT_OPITEM" },
    { DrawOpItem::PICTURE_OPITEM,           "PICTURE_OPITEM" },
    { DrawOpItem::TEXT_BLOB_OPITEM,         "TEXT_BLOB_OPITEM" },
    { DrawOpItem::SYMBOL_OPITEM,            "SYMBOL_OPITEM" },
    { DrawOpItem::CLIP_RECT_OPITEM,         "CLIP_RECT_OPITEM" },
    { DrawOpItem::CLIP_IRECT_OPITEM,        "CLIP_IRECT_OPITEM" },
    { DrawOpItem::CLIP_ROUND_RECT_OPITEM,   "CLIP_ROUND_RECT_OPITEM" },
    { DrawOpItem::CLIP_PATH_OPITEM,         "CLIP_PATH_OPITEM" },
    { DrawOpItem::CLIP_REGION_OPITEM,       "CLIP_REGION_OPITEM" },
    { DrawOpItem::SET_MATRIX_OPITEM,        "SET_MATRIX_OPITEM" },
    { DrawOpItem::RESET_MATRIX_OPITEM,      "RESET_MATRIX_OPITEM" },
    { DrawOpItem::CONCAT_MATRIX_OPITEM,     "CONCAT_MATRIX_OPITEM" },
    { DrawOpItem::TRANSLATE_OPITEM,         "TRANSLATE_OPITEM" },
    { DrawOpItem::SCALE_OPITEM,             "SCALE_OPITEM" },
    { DrawOpItem::ROTATE_OPITEM,            "ROTATE_OPITEM" },
    { DrawOpItem::SHEAR_OPITEM,             "SHEAR_OPITEM" },
    { DrawOpItem::FLUSH_OPITEM,             "FLUSH_OPITEM" },
    { DrawOpItem::CLEAR_OPITEM,             "CLEAR_OPITEM" },
    { DrawOpItem::SAVE_OPITEM,              "SAVE_OPITEM" },
    { DrawOpItem::SAVE_LAYER_OPITEM,        "SAVE_LAYER_OPITEM" },
    { DrawOpItem::RESTORE_OPITEM,           "RESTORE_OPITEM" },
    { DrawOpItem::DISCARD_OPITEM,           "DISCARD_OPITEM" },
    { DrawOpItem::CLIP_ADAPTIVE_ROUND_RECT_OPITEM,  "CLIP_ADAPTIVE_ROUND_RECT_OPITEM" },
    { DrawOpItem::IMAGE_WITH_PARM_OPITEM,   "IMAGE_WITH_PARM_OPITEM" },
    { DrawOpItem::PIXELMAP_WITH_PARM_OPITEM, "PIXELMAP_WITH_PARM_OPITEM" },
    { DrawOpItem::PIXELMAP_RECT_OPITEM,     "PIXELMAP_RECT_OPITEM" },
    { DrawOpItem::PIXELMAP_NINE_OPITEM,     "PIXELMAP_NINE_OPITEM" },
    { DrawOpItem::PIXELMAP_LATTICE_OPITEM,  "PIXELMAP_LATTICE_OPITEM" },
    { DrawOpItem::REGION_OPITEM,            "REGION_OPITEM" },
    { DrawOpItem::PATCH_OPITEM,             "PATCH_OPITEM" },
    { DrawOpItem::EDGEAAQUAD_OPITEM,        "EDGEAAQUAD_OPITEM" },
    { DrawOpItem::VERTICES_OPITEM,          "VERTICES_OPITEM" },
    { DrawOpItem::IMAGE_SNAPSHOT_OPITEM,    "IMAGE_SNAPSHOT_OPITEM" },
    { DrawOpItem::SURFACEBUFFER_OPITEM,     "SURFACEBUFFER_OPITEM"},
    { DrawOpItem::DRAW_FUNC_OPITEM,         "DRAW_FUNC_OPITEM"},
    { DrawOpItem::HYBRID_RENDER_PIXELMAP_OPITEM, "HYBRID_RENDER_PIXELMAP_OPITEM"},
    { DrawOpItem::HYBRID_RENDER_PIXELMAP_SIZE_OPITEM, "HYBRID_RENDER_PIXELMAP_SIZE_OPITEM"},
};

namespace {
constexpr int TEXT_BLOB_CACHE_MARGIN = 10;
constexpr float HIGH_CONTRAST_OFFSCREEN_THREASHOLD = 0.99f;
constexpr size_t MAX_GLYPH_ID_COUNT = 20;

template<class A, class F>
void DumpArray(std::string& out, const A& array, F func)
{
    out += "[";
    auto end = std::end(array);
    for (auto begin = std::begin(array); begin != end; begin++) {
        func(out, *begin);
        out += " ";
    }
    if (std::begin(array) != end) {
        out.pop_back();
    }
    out += "]";
}
}

std::function<void (std::shared_ptr<Drawing::Image> image)> DrawOpItem::holdDrawingImagefunc_ = nullptr;
void DrawOpItem::SetBaseCallback(
    std::function<void (std::shared_ptr<Drawing::Image> image)> holdDrawingImagefunc)
{
    holdDrawingImagefunc_ = holdDrawingImagefunc;
}

std::function<std::shared_ptr<Drawing::Typeface>(uint64_t)> DrawOpItem::customTypefaceQueryfunc_ = nullptr;
void DrawOpItem::SetTypefaceQueryCallBack(
    std::function<std::shared_ptr<Drawing::Typeface>(uint64_t)> customTypefaceQueryfunc)
{
    customTypefaceQueryfunc_ = customTypefaceQueryfunc;
}

void DrawOpItem::BrushHandleToBrush(const BrushHandle& brushHandle, const DrawCmdList& cmdList, Brush& brush)
{
    brush.SetBlendMode(brushHandle.mode);
    brush.SetAntiAlias(brushHandle.isAntiAlias);
    brush.SetBlenderEnabled(brushHandle.blenderEnabled);

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

    if (brushHandle.blenderHandle.size) {
        auto blender = CmdListHelper::GetBlenderFromCmdList(cmdList, brushHandle.blenderHandle);
        brush.SetBlender(blender);
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

void DrawOpItem::BrushToBrushHandle(const Brush& brush, DrawCmdList& cmdList, BrushHandle& brushHandle)
{
    const Filter& filter = brush.GetFilter();
    brushHandle.color = brush.GetColor();
    brushHandle.mode = brush.GetBlendMode();
    brushHandle.isAntiAlias = brush.IsAntiAlias();
    brushHandle.blenderEnabled = brush.GetBlenderEnabled();
    brushHandle.filterQuality = filter.GetFilterQuality();
    brushHandle.colorSpaceHandle = CmdListHelper::AddColorSpaceToCmdList(cmdList, brush.GetColorSpace());
    brushHandle.shaderEffectHandle = CmdListHelper::AddShaderEffectToCmdList(cmdList, brush.GetShaderEffect());
    brushHandle.colorFilterHandle = CmdListHelper::AddColorFilterToCmdList(cmdList, filter.GetColorFilter());
    brushHandle.imageFilterHandle = CmdListHelper::AddImageFilterToCmdList(cmdList, filter.GetImageFilter());
    brushHandle.maskFilterHandle = CmdListHelper::AddMaskFilterToCmdList(cmdList, filter.GetMaskFilter());
}

void DrawOpItem::GeneratePaintFromHandle(const PaintHandle& paintHandle, const DrawCmdList& cmdList, Paint& paint)
{
    paint.SetBlendMode(paintHandle.mode);
    paint.SetAntiAlias(paintHandle.isAntiAlias);
    paint.SetBlenderEnabled(paintHandle.blenderEnabled);
    paint.SetStyle(paintHandle.style);

    if (paintHandle.colorSpaceHandle.size) {
        auto colorSpace = CmdListHelper::GetColorSpaceFromCmdList(cmdList, paintHandle.colorSpaceHandle);
        const Color4f color4f = { paintHandle.color.GetRedF(), paintHandle.color.GetGreenF(),
                                  paintHandle.color.GetBlueF(), paintHandle.color.GetAlphaF() };
        paint.SetColor(color4f, colorSpace);
    } else {
        paint.SetColor(paintHandle.color);
    }

    if (paintHandle.shaderEffectHandle.size) {
        auto shaderEffect = CmdListHelper::GetShaderEffectFromCmdList(cmdList, paintHandle.shaderEffectHandle);
        paint.SetShaderEffect(shaderEffect);
    }

    if (paintHandle.blenderHandle.size) {
        auto blender = CmdListHelper::GetBlenderFromCmdList(cmdList, paintHandle.blenderHandle);
        paint.SetBlender(blender);
    }

    Filter filter;
    bool hasFilter = false;
    if (paintHandle.colorFilterHandle.size) {
        auto colorFilter = CmdListHelper::GetColorFilterFromCmdList(cmdList, paintHandle.colorFilterHandle);
        filter.SetColorFilter(colorFilter);
        hasFilter = true;
    }
    if (paintHandle.imageFilterHandle.size) {
        auto imageFilter = CmdListHelper::GetImageFilterFromCmdList(cmdList, paintHandle.imageFilterHandle);
        filter.SetImageFilter(imageFilter);
        hasFilter = true;
    }
    if (paintHandle.maskFilterHandle.size) {
        auto maskFilter = CmdListHelper::GetMaskFilterFromCmdList(cmdList, paintHandle.maskFilterHandle);
        filter.SetMaskFilter(maskFilter);
        hasFilter = true;
    }

    if (hasFilter) {
        filter.SetFilterQuality(paintHandle.filterQuality);
        paint.SetFilter(filter);
    }

    if (paintHandle.blurDrawLooperHandle.size) {
        auto blurDrawLooper = CmdListHelper::GetBlurDrawLooperFromCmdList(cmdList,
            paintHandle.blurDrawLooperHandle);
        paint.SetLooper(blurDrawLooper);
    }

    if (!paint.HasStrokeStyle()) {
        return;
    }

    paint.SetWidth(paintHandle.width);
    paint.SetMiterLimit(paintHandle.miterLimit);
    paint.SetCapStyle(paintHandle.capStyle);
    paint.SetJoinStyle(paintHandle.joinStyle);
    if (paintHandle.pathEffectHandle.size) {
        auto pathEffect = CmdListHelper::GetPathEffectFromCmdList(cmdList, paintHandle.pathEffectHandle);
        paint.SetPathEffect(pathEffect);
    }
}

void DrawOpItem::GenerateHandleFromPaint(CmdList& cmdList, const Paint& paint, PaintHandle& paintHandle)
{
    paintHandle.isAntiAlias = paint.IsAntiAlias();
    paintHandle.blenderEnabled = paint.GetBlenderEnabled();
    paintHandle.style = paint.GetStyle();
    paintHandle.color = paint.GetColor();
    paintHandle.mode = paint.GetBlendMode();

    if (paint.HasFilter()) {
        const Filter& filter = paint.GetFilter();
        paintHandle.filterQuality = filter.GetFilterQuality();
        paintHandle.colorFilterHandle = CmdListHelper::AddColorFilterToCmdList(cmdList, filter.GetColorFilter());
        paintHandle.imageFilterHandle = CmdListHelper::AddImageFilterToCmdList(cmdList, filter.GetImageFilter());
        paintHandle.maskFilterHandle = CmdListHelper::AddMaskFilterToCmdList(cmdList, filter.GetMaskFilter());
    }

    if (paint.GetColorSpace()) {
        paintHandle.colorSpaceHandle = CmdListHelper::AddColorSpaceToCmdList(cmdList, paint.GetColorSpace());
    }

    if (paint.GetShaderEffect()) {
        paintHandle.shaderEffectHandle = CmdListHelper::AddShaderEffectToCmdList(cmdList, paint.GetShaderEffect());
    }

    if (paint.GetBlender()) {
        paintHandle.blenderHandle = CmdListHelper::AddBlenderToCmdList(cmdList, paint.GetBlender());
    }

    if (paint.GetLooper()) {
        paintHandle.blurDrawLooperHandle = CmdListHelper::AddBlurDrawLooperToCmdList(cmdList,
            paint.GetLooper());
    }

    if (!paint.HasStrokeStyle()) {
        return;
    }

    paintHandle.width = paint.GetWidth();
    paintHandle.miterLimit = paint.GetMiterLimit();
    paintHandle.capStyle = paint.GetCapStyle();
    paintHandle.joinStyle = paint.GetJoinStyle();
    if (paint.GetPathEffect()) {
        paintHandle.pathEffectHandle = CmdListHelper::AddPathEffectToCmdList(cmdList, paint.GetPathEffect());
    }
}

std::string DrawOpItem::GetOpDesc() const
{
    return typeOpDes[GetType()];
}

void DrawOpItem::Dump(std::string& out) const
{
    out += typeOpDes[GetType()];
}

size_t DrawOpItem::GetOpSize()
{
    size_t totoalSize = sizeof(*this);
    const auto unmarshallingPair = UnmarshallingHelper::Instance().GetFuncAndSize(GetType());
    totoalSize += unmarshallingPair.second;
    return totoalSize;
}

GenerateCachedOpItemPlayer::GenerateCachedOpItemPlayer(DrawCmdList &cmdList, Canvas* canvas, const Rect* rect)
    : canvas_(canvas), rect_(rect), cmdList_(cmdList) {}

bool GenerateCachedOpItemPlayer::GenerateCachedOpItem(uint32_t type, void* handle, size_t avaliableSize)
{
    if (handle == nullptr) {
        return false;
    }

    if (type == DrawOpItem::TEXT_BLOB_OPITEM && avaliableSize >= sizeof(DrawTextBlobOpItem::ConstructorHandle)) {
        auto* op = static_cast<DrawTextBlobOpItem::ConstructorHandle*>(handle);
        return op->GenerateCachedOpItem(cmdList_, canvas_);
    }
    return false;
}

/* UnmarshallingPlayer */
UnmarshallingHelper& UnmarshallingHelper::Instance()
{
    static UnmarshallingHelper instance;
    return instance;
}

bool UnmarshallingHelper::Register(uint32_t type, UnmarshallingHelper::UnmarshallingFunc func, size_t unmarshallingSize)
{
    std::unique_lock lck(mtx_);
    opUnmarshallingFuncLUT_.emplace(type, func);
    opUnmarshallingSize_.emplace(type, unmarshallingSize);
    return true;
}

std::pair<UnmarshallingHelper::UnmarshallingFunc, size_t> UnmarshallingHelper::GetFuncAndSize(uint32_t type)
{
    std::shared_lock lck(mtx_);
    auto funcIt = opUnmarshallingFuncLUT_.find(type);
    auto sizeIt = opUnmarshallingSize_.find(type);
    if (funcIt == opUnmarshallingFuncLUT_.end() || sizeIt == opUnmarshallingSize_.end()) {
        return { nullptr, 0 };
    }
    /* unmarshalling func, desirable size for unmarshalling*/
    return { funcIt->second, sizeIt->second };
}

UnmarshallingPlayer::UnmarshallingPlayer(const DrawCmdList& cmdList) : cmdList_(cmdList) {}

std::shared_ptr<DrawOpItem> UnmarshallingPlayer::Unmarshalling(uint32_t type, void* handle, size_t avaliableSize)
{
    if (type == DrawOpItem::OPITEM_HEAD) {
        return nullptr;
    }

    const auto unmarshallingPair = UnmarshallingHelper::Instance().GetFuncAndSize(type);
    /* if unmarshalling func is null or avaliable size < desirable unmarshalling size, then return nullptr*/
    if (unmarshallingPair.first == nullptr || unmarshallingPair.second > avaliableSize) {
        return nullptr;
    }
    return (*unmarshallingPair.first)(this->cmdList_, handle);
}

/* DrawWithPaintOpItem */
DrawWithPaintOpItem::DrawWithPaintOpItem(const DrawCmdList& cmdList, const PaintHandle& paintHandle, uint32_t type)
    : DrawOpItem(type)
{
    GeneratePaintFromHandle(paintHandle, cmdList, paint_);
}

void DrawWithPaintOpItem::Dump(std::string& out) const
{
    DrawOpItem::Dump(out);
    out += "[paint";
    paint_.Dump(out);
    DumpItems(out);
    out += ']';
}

/* DrawPointOpItem */
UNMARSHALLING_REGISTER(DrawPoint, DrawOpItem::POINT_OPITEM,
    DrawPointOpItem::Unmarshalling, sizeof(DrawPointOpItem::ConstructorHandle));

DrawPointOpItem::DrawPointOpItem(const DrawCmdList& cmdList, DrawPointOpItem::ConstructorHandle* handle)
    : DrawWithPaintOpItem(cmdList, handle->paintHandle, POINT_OPITEM), point_(handle->point) {}

std::shared_ptr<DrawOpItem> DrawPointOpItem::Unmarshalling(const DrawCmdList& cmdList, void* handle)
{
    return std::make_shared<DrawPointOpItem>(cmdList, static_cast<DrawPointOpItem::ConstructorHandle*>(handle));
}

void DrawPointOpItem::Marshalling(DrawCmdList& cmdList)
{
    PaintHandle paintHandle;
    GenerateHandleFromPaint(cmdList, paint_, paintHandle);
    cmdList.AddOp<ConstructorHandle>(point_, paintHandle);
}

void DrawPointOpItem::Playback(Canvas* canvas, const Rect* rect)
{
    canvas->AttachPaint(paint_);
    canvas->DrawPoint(point_);
}

void DrawPointOpItem::DumpItems(std::string& out) const
{
    out += " point";
    point_.Dump(out);
}

/* DrawPointsOpItem */
UNMARSHALLING_REGISTER(DrawPoints, DrawOpItem::POINTS_OPITEM,
    DrawPointsOpItem::Unmarshalling, sizeof(DrawPointsOpItem::ConstructorHandle));

DrawPointsOpItem::DrawPointsOpItem(const DrawCmdList& cmdList, DrawPointsOpItem::ConstructorHandle* handle)
    : DrawWithPaintOpItem(cmdList, handle->paintHandle, POINTS_OPITEM), mode_(handle->mode)
{
    pts_ = CmdListHelper::GetVectorFromCmdList<Point>(cmdList, handle->pts);
}

std::shared_ptr<DrawOpItem> DrawPointsOpItem::Unmarshalling(const DrawCmdList& cmdList, void* handle)
{
    return std::make_shared<DrawPointsOpItem>(cmdList, static_cast<DrawPointsOpItem::ConstructorHandle*>(handle));
}

void DrawPointsOpItem::Marshalling(DrawCmdList& cmdList)
{
    PaintHandle paintHandle;
    GenerateHandleFromPaint(cmdList, paint_, paintHandle);
    auto pointsData = CmdListHelper::AddVectorToCmdList<Point>(cmdList, pts_);
    cmdList.AddOp<ConstructorHandle>(mode_, pointsData, paintHandle);
}

void DrawPointsOpItem::Playback(Canvas* canvas, const Rect* rect)
{
    canvas->AttachPaint(paint_);
    canvas->DrawPoints(mode_, pts_.size(), pts_.data());
}

/* DrawLineOpItem */
UNMARSHALLING_REGISTER(DrawLine, DrawOpItem::LINE_OPITEM,
    DrawLineOpItem::Unmarshalling, sizeof(DrawLineOpItem::ConstructorHandle));

DrawLineOpItem::DrawLineOpItem(const DrawCmdList& cmdList, DrawLineOpItem::ConstructorHandle* handle)
    : DrawWithPaintOpItem(cmdList, handle->paintHandle, LINE_OPITEM),
      startPt_(handle->startPt), endPt_(handle->endPt) {}

std::shared_ptr<DrawOpItem> DrawLineOpItem::Unmarshalling(const DrawCmdList& cmdList, void* handle)
{
    return std::make_shared<DrawLineOpItem>(cmdList, static_cast<DrawLineOpItem::ConstructorHandle*>(handle));
}

void DrawLineOpItem::Marshalling(DrawCmdList& cmdList)
{
    PaintHandle paintHandle;
    GenerateHandleFromPaint(cmdList, paint_, paintHandle);
    cmdList.AddOp<ConstructorHandle>(startPt_, endPt_, paintHandle);
}

void DrawLineOpItem::Playback(Canvas* canvas, const Rect* rect)
{
    canvas->AttachPaint(paint_);
    canvas->DrawLine(startPt_, endPt_);
}

void DrawLineOpItem::DumpItems(std::string& out) const
{
    out += " startPt";
    startPt_.Dump(out);
    out += " endPt";
    endPt_.Dump(out);
}

/* DrawRectOpItem */
UNMARSHALLING_REGISTER(DrawRect, DrawOpItem::RECT_OPITEM,
    DrawRectOpItem::Unmarshalling, sizeof(DrawRectOpItem::ConstructorHandle));

DrawRectOpItem::DrawRectOpItem(const DrawCmdList& cmdList, DrawRectOpItem::ConstructorHandle* handle)
    : DrawWithPaintOpItem(cmdList, handle->paintHandle, RECT_OPITEM), rect_(handle->rect) {}

std::shared_ptr<DrawOpItem> DrawRectOpItem::Unmarshalling(const DrawCmdList& cmdList, void* handle)
{
    return std::make_shared<DrawRectOpItem>(cmdList, static_cast<DrawRectOpItem::ConstructorHandle*>(handle));
}

void DrawRectOpItem::Marshalling(DrawCmdList& cmdList)
{
    PaintHandle paintHandle;
    GenerateHandleFromPaint(cmdList, paint_, paintHandle);
    cmdList.AddOp<ConstructorHandle>(rect_, paintHandle);
}

void DrawRectOpItem::Playback(Canvas* canvas, const Rect* rect)
{
    canvas->AttachPaint(paint_);
    canvas->DrawRect(rect_);
}

void DrawRectOpItem::DumpItems(std::string& out) const
{
    out += " rect";
    rect_.Dump(out);
}

/* DrawRoundRectOpItem */
UNMARSHALLING_REGISTER(DrawRoundRect, DrawOpItem::ROUND_RECT_OPITEM,
    DrawRoundRectOpItem::Unmarshalling, sizeof(DrawRoundRectOpItem::ConstructorHandle));

DrawRoundRectOpItem::DrawRoundRectOpItem(const DrawCmdList& cmdList, DrawRoundRectOpItem::ConstructorHandle* handle)
    : DrawWithPaintOpItem(cmdList, handle->paintHandle, ROUND_RECT_OPITEM), rrect_(handle->rrect) {}

std::shared_ptr<DrawOpItem> DrawRoundRectOpItem::Unmarshalling(const DrawCmdList& cmdList, void* handle)
{
    return std::make_shared<DrawRoundRectOpItem>(cmdList, static_cast<DrawRoundRectOpItem::ConstructorHandle*>(handle));
}

void DrawRoundRectOpItem::Marshalling(DrawCmdList& cmdList)
{
    PaintHandle paintHandle;
    GenerateHandleFromPaint(cmdList, paint_, paintHandle);
    cmdList.AddOp<ConstructorHandle>(rrect_, paintHandle);
}

void DrawRoundRectOpItem::Playback(Canvas* canvas, const Rect* rect)
{
    canvas->AttachPaint(paint_);
    canvas->DrawRoundRect(rrect_);
}

void DrawRoundRectOpItem::DumpItems(std::string& out) const
{
    out += " rrect";
    rrect_.Dump(out);
}

/* DrawNestedRoundRectOpItem */
UNMARSHALLING_REGISTER(DrawNestedRoundRect, DrawOpItem::NESTED_ROUND_RECT_OPITEM,
    DrawNestedRoundRectOpItem::Unmarshalling, sizeof(DrawNestedRoundRectOpItem::ConstructorHandle));

DrawNestedRoundRectOpItem::DrawNestedRoundRectOpItem(
    const DrawCmdList& cmdList, DrawNestedRoundRectOpItem::ConstructorHandle* handle)
    : DrawWithPaintOpItem(cmdList, handle->paintHandle, NESTED_ROUND_RECT_OPITEM),
      outerRRect_(handle->outerRRect), innerRRect_(handle->innerRRect) {}

std::shared_ptr<DrawOpItem> DrawNestedRoundRectOpItem::Unmarshalling(const DrawCmdList& cmdList, void* handle)
{
    return std::make_shared<DrawNestedRoundRectOpItem>(
        cmdList, static_cast<DrawNestedRoundRectOpItem::ConstructorHandle*>(handle));
}

void DrawNestedRoundRectOpItem::Marshalling(DrawCmdList& cmdList)
{
    PaintHandle paintHandle;
    GenerateHandleFromPaint(cmdList, paint_, paintHandle);
    cmdList.AddOp<ConstructorHandle>(outerRRect_, innerRRect_, paintHandle);
}

void DrawNestedRoundRectOpItem::Playback(Canvas* canvas, const Rect* rect)
{
    canvas->AttachPaint(paint_);
    canvas->DrawNestedRoundRect(outerRRect_, innerRRect_);
}

void DrawNestedRoundRectOpItem::DumpItems(std::string& out) const
{
    out += " outerRRect";
    outerRRect_.Dump(out);
    out += " innerRRect";
    innerRRect_.Dump(out);
}

/* DrawArcOpItem */
UNMARSHALLING_REGISTER(DrawArc, DrawOpItem::ARC_OPITEM,
    DrawArcOpItem::Unmarshalling, sizeof(DrawArcOpItem::ConstructorHandle));

DrawArcOpItem::DrawArcOpItem(const DrawCmdList& cmdList, DrawArcOpItem::ConstructorHandle* handle)
    : DrawWithPaintOpItem(cmdList, handle->paintHandle, ARC_OPITEM), rect_(handle->rect),
      startAngle_(handle->startAngle), sweepAngle_(handle->sweepAngle) {}

std::shared_ptr<DrawOpItem> DrawArcOpItem::Unmarshalling(const DrawCmdList& cmdList, void* handle)
{
    return std::make_shared<DrawArcOpItem>(cmdList, static_cast<DrawArcOpItem::ConstructorHandle*>(handle));
}

void DrawArcOpItem::Marshalling(DrawCmdList& cmdList)
{
    PaintHandle paintHandle;
    GenerateHandleFromPaint(cmdList, paint_, paintHandle);
    cmdList.AddOp<ConstructorHandle>(rect_, startAngle_, sweepAngle_, paintHandle);
}

void DrawArcOpItem::Playback(Canvas* canvas, const Rect* rect)
{
    canvas->AttachPaint(paint_);
    canvas->DrawArc(rect_, startAngle_, sweepAngle_);
}

void DrawArcOpItem::DumpItems(std::string& out) const
{
    out += " rect";
    rect_.Dump(out);
    out += " startAngle:" + std::to_string(startAngle_);
    out += " sweepAngle:" + std::to_string(sweepAngle_);
}

/* DrawPieOpItem */
UNMARSHALLING_REGISTER(DrawPie, DrawOpItem::PIE_OPITEM,
    DrawPieOpItem::Unmarshalling, sizeof(DrawPieOpItem::ConstructorHandle));

DrawPieOpItem::DrawPieOpItem(const DrawCmdList& cmdList, DrawPieOpItem::ConstructorHandle* handle)
    : DrawWithPaintOpItem(cmdList, handle->paintHandle, PIE_OPITEM), rect_(handle->rect),
      startAngle_(handle->startAngle), sweepAngle_(handle->sweepAngle) {}

std::shared_ptr<DrawOpItem> DrawPieOpItem::Unmarshalling(const DrawCmdList& cmdList, void* handle)
{
    return std::make_shared<DrawPieOpItem>(cmdList, static_cast<DrawPieOpItem::ConstructorHandle*>(handle));
}

void DrawPieOpItem::Marshalling(DrawCmdList& cmdList)
{
    PaintHandle paintHandle;
    GenerateHandleFromPaint(cmdList, paint_, paintHandle);
    cmdList.AddOp<ConstructorHandle>(rect_, startAngle_, sweepAngle_, paintHandle);
}

void DrawPieOpItem::Playback(Canvas* canvas, const Rect* rect)
{
    canvas->AttachPaint(paint_);
    canvas->DrawPie(rect_, startAngle_, sweepAngle_);
}

void DrawPieOpItem::DumpItems(std::string& out) const
{
    out += " rect";
    rect_.Dump(out);
    out += " startAngle:" + std::to_string(startAngle_);
    out += " sweepAngle:" + std::to_string(sweepAngle_);
}

/* DrawOvalOpItem */
UNMARSHALLING_REGISTER(DrawOval, DrawOpItem::OVAL_OPITEM,
    DrawOvalOpItem::Unmarshalling, sizeof(DrawOvalOpItem::ConstructorHandle));

DrawOvalOpItem::DrawOvalOpItem(const DrawCmdList& cmdList, DrawOvalOpItem::ConstructorHandle* handle)
    : DrawWithPaintOpItem(cmdList, handle->paintHandle, OVAL_OPITEM), rect_(handle->rect) {}

std::shared_ptr<DrawOpItem> DrawOvalOpItem::Unmarshalling(const DrawCmdList& cmdList, void* handle)
{
    return std::make_shared<DrawOvalOpItem>(cmdList, static_cast<DrawOvalOpItem::ConstructorHandle*>(handle));
}

void DrawOvalOpItem::Marshalling(DrawCmdList& cmdList)
{
    PaintHandle paintHandle;
    GenerateHandleFromPaint(cmdList, paint_, paintHandle);
    cmdList.AddOp<ConstructorHandle>(rect_, paintHandle);
}

void DrawOvalOpItem::Playback(Canvas* canvas, const Rect* rect)
{
    canvas->AttachPaint(paint_);
    canvas->DrawOval(rect_);
}

void DrawOvalOpItem::DumpItems(std::string& out) const
{
    out += " rect";
    rect_.Dump(out);
}

/* DrawCircleOpItem */
UNMARSHALLING_REGISTER(DrawCircle, DrawOpItem::CIRCLE_OPITEM,
    DrawCircleOpItem::Unmarshalling, sizeof(DrawCircleOpItem::ConstructorHandle));

DrawCircleOpItem::DrawCircleOpItem(const DrawCmdList& cmdList, DrawCircleOpItem::ConstructorHandle* handle)
    : DrawWithPaintOpItem(cmdList, handle->paintHandle, CIRCLE_OPITEM),
      centerPt_(handle->centerPt), radius_(handle->radius) {}

std::shared_ptr<DrawOpItem> DrawCircleOpItem::Unmarshalling(const DrawCmdList& cmdList, void* handle)
{
    return std::make_shared<DrawCircleOpItem>(cmdList, static_cast<DrawCircleOpItem::ConstructorHandle*>(handle));
}

void DrawCircleOpItem::Marshalling(DrawCmdList& cmdList)
{
    PaintHandle paintHandle;
    GenerateHandleFromPaint(cmdList, paint_, paintHandle);
    cmdList.AddOp<ConstructorHandle>(centerPt_, radius_, paintHandle);
}

void DrawCircleOpItem::Playback(Canvas* canvas, const Rect* rect)
{
    canvas->AttachPaint(paint_);
    canvas->DrawCircle(centerPt_, radius_);
}

void DrawCircleOpItem::DumpItems(std::string& out) const
{
    out += " centerPt";
    centerPt_.Dump(out);
    out += " radius:" + std::to_string(radius_);
}

/* DrawPathOpItem */
UNMARSHALLING_REGISTER(DrawPath, DrawOpItem::PATH_OPITEM,
    DrawPathOpItem::Unmarshalling, sizeof(DrawPathOpItem::ConstructorHandle));

DrawPathOpItem::DrawPathOpItem(const DrawCmdList& cmdList, DrawPathOpItem::ConstructorHandle* handle)
    : DrawWithPaintOpItem(cmdList, handle->paintHandle, PATH_OPITEM)
{
    path_ = CmdListHelper::GetPathFromCmdList(cmdList, handle->path);
}

std::shared_ptr<DrawOpItem> DrawPathOpItem::Unmarshalling(const DrawCmdList& cmdList, void* handle)
{
    return std::make_shared<DrawPathOpItem>(cmdList, static_cast<DrawPathOpItem::ConstructorHandle*>(handle));
}

void DrawPathOpItem::Marshalling(DrawCmdList& cmdList)
{
    PaintHandle paintHandle;
    GenerateHandleFromPaint(cmdList, paint_, paintHandle);
    OpDataHandle pathHandle;
    if (path_) {
        pathHandle = CmdListHelper::AddPathToCmdList(cmdList, *path_);
    }
    cmdList.AddOp<ConstructorHandle>(pathHandle, paintHandle);
}

void DrawPathOpItem::Playback(Canvas* canvas, const Rect* rect)
{
    if (path_ == nullptr) {
        LOGD("DrawPathOpItem path is null!");
        return;
    }
    canvas->AttachPaint(paint_);
    canvas->DrawPath(*path_);
}

void DrawPathOpItem::DumpItems(std::string& out) const
{
    if (path_ != nullptr) {
        out += " Path";
        path_->Dump(out);
    }
}

/* DrawBackgroundOpItem */
UNMARSHALLING_REGISTER(DrawBackground, DrawOpItem::BACKGROUND_OPITEM,
    DrawBackgroundOpItem::Unmarshalling, sizeof(DrawBackgroundOpItem::ConstructorHandle));

DrawBackgroundOpItem::DrawBackgroundOpItem(const DrawCmdList& cmdList, DrawBackgroundOpItem::ConstructorHandle* handle)
    : DrawOpItem(BACKGROUND_OPITEM)
{
    BrushHandleToBrush(handle->brushHandle, cmdList, brush_);
}

std::shared_ptr<DrawOpItem> DrawBackgroundOpItem::Unmarshalling(const DrawCmdList& cmdList, void* handle)
{
    return std::make_shared<DrawBackgroundOpItem>(
        cmdList, static_cast<DrawBackgroundOpItem::ConstructorHandle*>(handle));
}

void DrawBackgroundOpItem::Marshalling(DrawCmdList& cmdList)
{
    BrushHandle brushHandle;
    BrushToBrushHandle(brush_, cmdList, brushHandle);
    cmdList.AddOp<ConstructorHandle>(brushHandle);
}

void DrawBackgroundOpItem::Playback(Canvas* canvas, const Rect* rect)
{
    canvas->DrawBackground(brush_);
}

/* DrawShadowStyleOpItem */
UNMARSHALLING_REGISTER(DrawShadowStyle, DrawOpItem::SHADOW_STYLE_OPITEM,
    DrawShadowStyleOpItem::Unmarshalling, sizeof(DrawShadowStyleOpItem::ConstructorHandle));

DrawShadowStyleOpItem::DrawShadowStyleOpItem(
    const DrawCmdList& cmdList, DrawShadowStyleOpItem::ConstructorHandle* handle)
    : DrawOpItem(SHADOW_STYLE_OPITEM),
      planeParams_(handle->planeParams),
      devLightPos_(handle->devLightPos),
      lightRadius_(handle->lightRadius),
      ambientColor_(handle->ambientColor),
      spotColor_(handle->spotColor),
      flag_(handle->flag),
      isLimitElevation_(handle->isLimitElevation)
{
    path_ = CmdListHelper::GetPathFromCmdList(cmdList, handle->path);
}

std::shared_ptr<DrawOpItem> DrawShadowStyleOpItem::Unmarshalling(const DrawCmdList& cmdList, void* handle)
{
    return std::make_shared<DrawShadowStyleOpItem>(
        cmdList, static_cast<DrawShadowStyleOpItem::ConstructorHandle*>(handle));
}

void DrawShadowStyleOpItem::Marshalling(DrawCmdList& cmdList)
{
    OpDataHandle pathHandle;
    if (path_) {
        pathHandle = CmdListHelper::AddPathToCmdList(cmdList, *path_);
    }
    cmdList.AddOp<ConstructorHandle>(
        pathHandle, planeParams_, devLightPos_, lightRadius_, ambientColor_, spotColor_, flag_, isLimitElevation_);
}

void DrawShadowStyleOpItem::Playback(Canvas* canvas, const Rect* rect)
{
    if (path_ == nullptr) {
        LOGD("DrawShadowStyleOpItem path is null!");
        return;
    }
    canvas->DrawShadowStyle(
        *path_, planeParams_, devLightPos_, lightRadius_, ambientColor_, spotColor_, flag_, isLimitElevation_);
}

void DrawShadowStyleOpItem::Dump(std::string& out) const
{
    out += GetOpDesc() + "[plane";
    planeParams_.Dump(out);
    out += " lightPos";
    devLightPos_.Dump(out);
    out += " lightRadius:" + std::to_string(lightRadius_);
    out += " ambientColor";
    ambientColor_.Dump(out);
    out += " spotColor_";
    spotColor_.Dump(out);
    out += " shadowFlags:" + std::to_string(static_cast<int>(flag_));
    out += " isLimitElevation:" + std::string(isLimitElevation_ ? "true" : "false");
    out += "]";
}

/* DrawShadowOpItem */
UNMARSHALLING_REGISTER(DrawShadow, DrawOpItem::SHADOW_OPITEM,
    DrawShadowOpItem::Unmarshalling, sizeof(DrawShadowOpItem::ConstructorHandle));

DrawShadowOpItem::DrawShadowOpItem(const DrawCmdList& cmdList, DrawShadowOpItem::ConstructorHandle* handle)
    : DrawOpItem(SHADOW_OPITEM), planeParams_(handle->planeParams), devLightPos_(handle->devLightPos),
    lightRadius_(handle->lightRadius), ambientColor_(handle->ambientColor),
    spotColor_(handle->spotColor), flag_(handle->flag)
{
    path_ = CmdListHelper::GetPathFromCmdList(cmdList, handle->path);
}

std::shared_ptr<DrawOpItem> DrawShadowOpItem::Unmarshalling(const DrawCmdList& cmdList, void* handle)
{
    return std::make_shared<DrawShadowOpItem>(cmdList, static_cast<DrawShadowOpItem::ConstructorHandle*>(handle));
}

void DrawShadowOpItem::Marshalling(DrawCmdList& cmdList)
{
    OpDataHandle pathHandle;
    if (path_) {
        pathHandle = CmdListHelper::AddPathToCmdList(cmdList, *path_);
    }
    cmdList.AddOp<ConstructorHandle>(
        pathHandle, planeParams_, devLightPos_, lightRadius_, ambientColor_, spotColor_, flag_);
}

void DrawShadowOpItem::Playback(Canvas* canvas, const Rect* rect)
{
    if (path_ == nullptr) {
        LOGD("DrawShadowOpItem path is null!");
        return;
    }
    canvas->DrawShadow(*path_, planeParams_, devLightPos_, lightRadius_,
                       ambientColor_, spotColor_, flag_);
}

void DrawShadowOpItem::Dump(std::string& out) const
{
    out += GetOpDesc() + "[plane";
    planeParams_.Dump(out);
    out += " lightPos";
    devLightPos_.Dump(out);
    out += " lightRadius:" + std::to_string(lightRadius_);
    out += " ambientColor";
    ambientColor_.Dump(out);
    out += " spotColor_";
    spotColor_.Dump(out);
    out += " shadowFlags:" + std::to_string(static_cast<int>(flag_));
    out += " path";
    if (path_) {
        path_->Dump(out);
    } else {
        out += "[null]";
    }
    out += "]";
}

/* DrawRegionOpItem */
UNMARSHALLING_REGISTER(DrawRegion, DrawOpItem::REGION_OPITEM,
    DrawRegionOpItem::Unmarshalling, sizeof(DrawRegionOpItem::ConstructorHandle));

DrawRegionOpItem::DrawRegionOpItem(const DrawCmdList& cmdList, DrawRegionOpItem::ConstructorHandle* handle)
    : DrawWithPaintOpItem(cmdList, handle->paintHandle, REGION_OPITEM)
{
    region_ = CmdListHelper::GetRegionFromCmdList(cmdList, handle->region);
}

std::shared_ptr<DrawOpItem> DrawRegionOpItem::Unmarshalling(const DrawCmdList& cmdList, void* handle)
{
    return std::make_shared<DrawRegionOpItem>(cmdList, static_cast<DrawRegionOpItem::ConstructorHandle*>(handle));
}

void DrawRegionOpItem::Marshalling(DrawCmdList& cmdList)
{
    PaintHandle paintHandle;
    GenerateHandleFromPaint(cmdList, paint_, paintHandle);
    OpDataHandle regionHandle;
    if (region_) {
        regionHandle = CmdListHelper::AddRegionToCmdList(cmdList, *region_);
    }
    cmdList.AddOp<ConstructorHandle>(regionHandle, paintHandle);
}

void DrawRegionOpItem::Playback(Canvas* canvas, const Rect* rect)
{
    if (region_ == nullptr) {
        LOGD("DrawRegionOpItem region is nullptr!");
        return;
    }
    canvas->AttachPaint(paint_);
    canvas->DrawRegion(*region_);
}

void DrawRegionOpItem::DumpItems(std::string& out) const
{
    if (region_ != nullptr) {
        out += " Region";
        region_->Dump(out);
    }
}

/* DrawVerticesOpItem */
UNMARSHALLING_REGISTER(DrawVertices, DrawOpItem::VERTICES_OPITEM,
    DrawVerticesOpItem::Unmarshalling, sizeof(DrawVerticesOpItem::ConstructorHandle));

DrawVerticesOpItem::DrawVerticesOpItem(const DrawCmdList& cmdList, DrawVerticesOpItem::ConstructorHandle* handle)
    : DrawWithPaintOpItem(cmdList, handle->paintHandle, VERTICES_OPITEM), mode_(handle->mode)
{
    vertices_ = CmdListHelper::GetVerticesFromCmdList(cmdList, handle->vertices);
}

std::shared_ptr<DrawOpItem> DrawVerticesOpItem::Unmarshalling(const DrawCmdList& cmdList, void* handle)
{
    return std::make_shared<DrawVerticesOpItem>(cmdList, static_cast<DrawVerticesOpItem::ConstructorHandle*>(handle));
}

void DrawVerticesOpItem::Marshalling(DrawCmdList& cmdList)
{
    PaintHandle paintHandle;
    GenerateHandleFromPaint(cmdList, paint_, paintHandle);
    OpDataHandle opDataHandle;
    if (vertices_) {
        opDataHandle = CmdListHelper::AddVerticesToCmdList(cmdList, *vertices_);
    }
    cmdList.AddOp<ConstructorHandle>(opDataHandle, mode_, paintHandle);
}

void DrawVerticesOpItem::Playback(Canvas* canvas, const Rect* rect)
{
    if (vertices_ == nullptr) {
        LOGD("DrawVerticesOpItem vertices is null");
        return;
    }
    canvas->AttachPaint(paint_);
    canvas->DrawVertices(*vertices_, mode_);
}

void DrawVerticesOpItem::DumpItems(std::string& out) const
{
    out += " blend_mode:" + std::to_string(static_cast<int>(mode_));
    out += " vertices:";
    std::stringstream stream;
    stream << std::hex << vertices_.get();
    out += std::string(stream.str());
}

/* DrawColorOpItem */
UNMARSHALLING_REGISTER(DrawColor, DrawOpItem::COLOR_OPITEM,
    DrawColorOpItem::Unmarshalling, sizeof(DrawColorOpItem::ConstructorHandle));

DrawColorOpItem::DrawColorOpItem(DrawColorOpItem::ConstructorHandle* handle)
    : DrawOpItem(COLOR_OPITEM), color_(handle->color), mode_(handle->mode) {}

std::shared_ptr<DrawOpItem> DrawColorOpItem::Unmarshalling(const DrawCmdList& cmdList, void* handle)
{
    return std::make_shared<DrawColorOpItem>(static_cast<DrawColorOpItem::ConstructorHandle*>(handle));
}

void DrawColorOpItem::Marshalling(DrawCmdList& cmdList)
{
    cmdList.AddOp<ConstructorHandle>(color_, mode_);
}

void DrawColorOpItem::Playback(Canvas* canvas, const Rect* rect)
{
    canvas->DrawColor(color_, mode_);
}

void DrawColorOpItem::Dump(std::string& out) const
{
    out += GetOpDesc() + "[color";
    Color(color_).Dump(out);
    out += " blendMode:" + std::to_string(static_cast<int>(mode_));
    out += "]";
}

/* DrawImageNineOpItem */
UNMARSHALLING_REGISTER(DrawImageNine, DrawOpItem::IMAGE_NINE_OPITEM,
    DrawImageNineOpItem::Unmarshalling, sizeof(DrawImageNineOpItem::ConstructorHandle));

DrawImageNineOpItem::DrawImageNineOpItem(const DrawCmdList& cmdList, DrawImageNineOpItem::ConstructorHandle* handle)
    : DrawOpItem(IMAGE_NINE_OPITEM), center_(handle->center), dst_(handle->dst), filter_(handle->filter),
    hasBrush_(handle->hasBrush)
{
    image_ = CmdListHelper::GetImageFromCmdList(cmdList, handle->image);
    if (DrawOpItem::holdDrawingImagefunc_) {
        DrawOpItem::holdDrawingImagefunc_(image_);
    }
    if (hasBrush_) {
        BrushHandleToBrush(handle->brushHandle, cmdList, brush_);
    }
}

std::shared_ptr<DrawOpItem> DrawImageNineOpItem::Unmarshalling(const DrawCmdList& cmdList, void* handle)
{
    return std::make_shared<DrawImageNineOpItem>(cmdList, static_cast<DrawImageNineOpItem::ConstructorHandle*>(handle));
}

void DrawImageNineOpItem::Marshalling(DrawCmdList& cmdList)
{
    OpDataHandle imageHandle;
    if (image_) {
        imageHandle = CmdListHelper::AddImageToCmdList(cmdList, *image_);
    }
    BrushHandle brushHandle;
    if (hasBrush_) {
        BrushToBrushHandle(brush_, cmdList, brushHandle);
    }

    cmdList.AddOp<ConstructorHandle>(imageHandle, center_, dst_, filter_, brushHandle, hasBrush_);
}

void DrawImageNineOpItem::Playback(Canvas* canvas, const Rect* rect)
{
    if (image_ == nullptr) {
        LOGD("DrawImageNineOpItem image is null");
        return;
    }
    Brush* brushPtr = hasBrush_ ? &brush_ : nullptr;
    canvas->DrawImageNine(image_.get(), center_, dst_, filter_, brushPtr);
}

void DrawImageNineOpItem::Dump(std::string& out) const
{
    out += GetOpDesc() + "[center";
    center_.Dump(out);
    out += " dst";
    dst_.Dump(out);
    out += " filterMode:" + std::to_string(static_cast<int>(filter_));
    out += " brush";
    if (hasBrush_) {
        brush_.Dump(out);
    } else {
        out += "[null]";
    }
    out += " image";
    if (image_) {
        image_->Dump(out);
    } else {
        out += "[null]";
    }
    out += "]";
}

/* DrawImageLatticeOpItem */
UNMARSHALLING_REGISTER(DrawImageLattice, DrawOpItem::IMAGE_LATTICE_OPITEM,
    DrawImageLatticeOpItem::Unmarshalling, sizeof(DrawImageLatticeOpItem::ConstructorHandle));

DrawImageLatticeOpItem::DrawImageLatticeOpItem(
    const DrawCmdList& cmdList, DrawImageLatticeOpItem::ConstructorHandle* handle)
    : DrawWithPaintOpItem(cmdList, handle->paintHandle, IMAGE_LATTICE_OPITEM),
    dst_(handle->dst), filter_(handle->filter)
{
    image_ = CmdListHelper::GetImageFromCmdList(cmdList, handle->image);
    if (DrawOpItem::holdDrawingImagefunc_) {
        DrawOpItem::holdDrawingImagefunc_(image_);
    }
    lattice_ = CmdListHelper::GetLatticeFromCmdList(cmdList, handle->latticeHandle);
}

std::shared_ptr<DrawOpItem> DrawImageLatticeOpItem::Unmarshalling(const DrawCmdList& cmdList, void* handle)
{
    return std::make_shared<DrawImageLatticeOpItem>(
        cmdList, static_cast<DrawImageLatticeOpItem::ConstructorHandle*>(handle));
}

void DrawImageLatticeOpItem::Marshalling(DrawCmdList& cmdList)
{
    PaintHandle paintHandle;
    GenerateHandleFromPaint(cmdList, paint_, paintHandle);
    OpDataHandle imageHandle;
    if (image_) {
        imageHandle = CmdListHelper::AddImageToCmdList(cmdList, *image_);
    }
    auto latticeHandle =  CmdListHelper::AddLatticeToCmdList(cmdList, lattice_);
    cmdList.AddOp<ConstructorHandle>(imageHandle, latticeHandle, dst_, filter_, paintHandle);
}

void DrawImageLatticeOpItem::Playback(Canvas* canvas, const Rect* rect)
{
    if (image_ == nullptr) {
        LOGD("DrawImageNineOpItem image is null");
        return;
    }
    canvas->AttachPaint(paint_);
    canvas->DrawImageLattice(image_.get(), lattice_, dst_, filter_);
}

void DrawImageLatticeOpItem::Dump(std::string& out) const
{
    out += GetOpDesc() + "[lattice[";
    out += "xCount:" + std::to_string(lattice_.fXCount);
    out += " yCount:" + std::to_string(lattice_.fYCount);
    out += " xDivs";
    DumpArray(out, lattice_.fXDivs, [](std::string& out, int n) {
        out += std::to_string(n);
    });
    out += " yDivs";
    DumpArray(out, lattice_.fYDivs, [](std::string& out, int n) {
        out += std::to_string(n);
    });
    out += " rectTypes";
    DumpArray(out, lattice_.fRectTypes, [](std::string& out, Lattice::RectType n) {
        out += std::to_string(static_cast<int>(n));
    });
    out += " bounds";
    DumpArray(out, lattice_.fBounds, [](std::string& out, const RectI& n) {
        n.Dump(out);
    });
    out += " colors";
    DumpArray(out, lattice_.fColors, [](std::string& out, const Color& c) {
        c.Dump(out);
    });
    out += " dst";
    dst_.Dump(out);
    out += " filterMode:" + std::to_string(static_cast<int>(filter_));
    out += " image";
    if (image_) {
        image_->Dump(out);
    } else {
        out += "[null]";
    }
    out += "]";
}

/* DrawAtlasOpItem */
UNMARSHALLING_REGISTER(DrawAtlas, DrawOpItem::ATLAS_OPITEM,
    DrawAtlasOpItem::Unmarshalling, sizeof(DrawAtlasOpItem::ConstructorHandle));

DrawAtlasOpItem::DrawAtlasOpItem(const DrawCmdList& cmdList, DrawAtlasOpItem::ConstructorHandle* handle)
    : DrawWithPaintOpItem(cmdList, handle->paintHandle, ATLAS_OPITEM), mode_(handle->mode),
      samplingOptions_(handle->samplingOptions), hasCullRect_(handle->hasCullRect), cullRect_(handle->cullRect)
{
    atlas_ = CmdListHelper::GetImageFromCmdList(cmdList, handle->atlas);
    if (DrawOpItem::holdDrawingImagefunc_) {
        DrawOpItem::holdDrawingImagefunc_(atlas_);
    }
    xform_ = CmdListHelper::GetVectorFromCmdList<RSXform>(cmdList, handle->xform);
    tex_ = CmdListHelper::GetVectorFromCmdList<Rect>(cmdList, handle->tex);
    colors_ = CmdListHelper::GetVectorFromCmdList<ColorQuad>(cmdList, handle->colors);
}

std::shared_ptr<DrawOpItem> DrawAtlasOpItem::Unmarshalling(const DrawCmdList& cmdList, void* handle)
{
    return std::make_shared<DrawAtlasOpItem>(cmdList, static_cast<DrawAtlasOpItem::ConstructorHandle*>(handle));
}

void DrawAtlasOpItem::Marshalling(DrawCmdList& cmdList)
{
    PaintHandle paintHandle;
    GenerateHandleFromPaint(cmdList, paint_, paintHandle);
    OpDataHandle imageHandle;
    if (atlas_) {
        imageHandle = CmdListHelper::AddImageToCmdList(cmdList, *atlas_);
    }
    auto xformData = CmdListHelper::AddVectorToCmdList<RSXform>(cmdList, xform_);
    auto texData = CmdListHelper::AddVectorToCmdList<Rect>(cmdList, tex_);
    auto colorData = CmdListHelper::AddVectorToCmdList<ColorQuad>(cmdList, colors_);
    cmdList.AddOp<ConstructorHandle>(imageHandle, xformData, texData, colorData, mode_, samplingOptions_,
        hasCullRect_, cullRect_, paintHandle);
}

void DrawAtlasOpItem::Playback(Canvas* canvas, const Rect* rect)
{
    if (atlas_ == nullptr) {
        LOGD("DrawAtlasOpItem atlas is null");
        return;
    }
    canvas->AttachPaint(paint_);
    Rect* rectPtr = hasCullRect_ ? &cullRect_ : nullptr;
    canvas->DrawAtlas(atlas_.get(), xform_.data(), tex_.data(), colors_.data(), xform_.size(), mode_,
        samplingOptions_, rectPtr);
}

void DrawAtlasOpItem::DumpItems(std::string& out) const
{
    out += " Xform[";
    for (auto& e: xform_) {
        e.Dump(out);
        out += ' ';
    }
    if (xform_.size() > 0) {
        out.pop_back();
    }
    out += ']';

    out += " Tex[";
    for (auto& e: tex_) {
        e.Dump(out);
        out += ' ';
    }
    if (tex_.size() > 0) {
        out.pop_back();
    }
    out += ']';

    out += " Colors[";
    for (auto e: colors_) {
        Color color(e);
        color.Dump(out);
        out += ' ';
    }
    if (colors_.size() > 0) {
        out.pop_back();
    }
    out += ']';

    out += " mode:" + std::to_string(static_cast<int>(mode_));
    out += " SamplingOption";
    samplingOptions_.Dump(out);
    out += " hasCullRect:" + std::string(hasCullRect_ ? "true" : "false");
    out += " CullRect";
    cullRect_.Dump(out);
    if (atlas_ != nullptr) {
        out += " Atlas";
        atlas_->Dump(out);
    }
}

/* DrawBitmapOpItem */
UNMARSHALLING_REGISTER(DrawBitmap, DrawOpItem::BITMAP_OPITEM,
    DrawBitmapOpItem::Unmarshalling, sizeof(DrawBitmapOpItem::ConstructorHandle));

DrawBitmapOpItem::DrawBitmapOpItem(const DrawCmdList& cmdList, DrawBitmapOpItem::ConstructorHandle* handle)
    : DrawWithPaintOpItem(cmdList, handle->paintHandle, BITMAP_OPITEM), px_(handle->px), py_(handle->py)
{
    bitmap_ = CmdListHelper::GetBitmapFromCmdList(cmdList, handle->bitmap);
}

std::shared_ptr<DrawOpItem> DrawBitmapOpItem::Unmarshalling(const DrawCmdList& cmdList, void* handle)
{
    return std::make_shared<DrawBitmapOpItem>(cmdList, static_cast<DrawBitmapOpItem::ConstructorHandle*>(handle));
}

void DrawBitmapOpItem::Marshalling(DrawCmdList& cmdList)
{
    PaintHandle paintHandle;
    GenerateHandleFromPaint(cmdList, paint_, paintHandle);
    ImageHandle bitmapHandle;
    if (bitmap_) {
        bitmapHandle = CmdListHelper::AddBitmapToCmdList(cmdList, *bitmap_);
    }
    cmdList.AddOp<ConstructorHandle>(bitmapHandle, px_, py_, paintHandle);
}

void DrawBitmapOpItem::Playback(Canvas* canvas, const Rect* rect)
{
    if (bitmap_ == nullptr) {
        LOGD("DrawBitmapOpItem bitmap is null");
        return;
    }
    canvas->AttachPaint(paint_);
    canvas->DrawBitmap(*bitmap_, px_, py_);
}

void DrawBitmapOpItem::DumpItems(std::string& out) const
{
    out += " px:" + std::to_string(px_) + " py:" + std::to_string(py_);
    if (bitmap_ != nullptr) {
        out += " Bitmap";
        bitmap_->Dump(out);
    }
}

/* DrawImageOpItem */
UNMARSHALLING_REGISTER(DrawImage, DrawOpItem::IMAGE_OPITEM,
    DrawImageOpItem::Unmarshalling, sizeof(DrawImageOpItem::ConstructorHandle));

DrawImageOpItem::DrawImageOpItem(const DrawCmdList& cmdList, DrawImageOpItem::ConstructorHandle* handle)
    : DrawWithPaintOpItem(cmdList, handle->paintHandle, IMAGE_OPITEM), px_(handle->px), py_(handle->py),
      samplingOptions_(handle->samplingOptions)
{
    image_ = CmdListHelper::GetImageFromCmdList(cmdList, handle->image);
    if (DrawOpItem::holdDrawingImagefunc_) {
        DrawOpItem::holdDrawingImagefunc_(image_);
    }
}

std::shared_ptr<DrawOpItem> DrawImageOpItem::Unmarshalling(const DrawCmdList& cmdList, void* handle)
{
    return std::make_shared<DrawImageOpItem>(cmdList, static_cast<DrawImageOpItem::ConstructorHandle*>(handle));
}

void DrawImageOpItem::Marshalling(DrawCmdList& cmdList)
{
    PaintHandle paintHandle;
    GenerateHandleFromPaint(cmdList, paint_, paintHandle);
    OpDataHandle imageHandle;
    if (image_) {
        imageHandle = CmdListHelper::AddImageToCmdList(cmdList, *image_);
    }
    cmdList.AddOp<ConstructorHandle>(imageHandle, px_, py_, samplingOptions_, paintHandle);
}

void DrawImageOpItem::Playback(Canvas* canvas, const Rect* rect)
{
    if (image_ == nullptr) {
        LOGD("DrawImageOpItem image is null");
        return;
    }
    canvas->AttachPaint(paint_);
    canvas->DrawImage(*image_, px_, py_, samplingOptions_);
}

void DrawImageOpItem::DumpItems(std::string& out) const
{
    out += " px:" + std::to_string(px_) + " py:" + std::to_string(py_);
    out += " SamplingOptions";
    samplingOptions_.Dump(out);
    if (image_ != nullptr) {
        out += " Image";
        image_->Dump(out);
    }
}

/* DrawImageRectOpItem */
UNMARSHALLING_REGISTER(DrawImageRect, DrawOpItem::IMAGE_RECT_OPITEM,
    DrawImageRectOpItem::Unmarshalling, sizeof(DrawImageRectOpItem::ConstructorHandle));

DrawImageRectOpItem::DrawImageRectOpItem(const DrawCmdList& cmdList, DrawImageRectOpItem::ConstructorHandle* handle)
    : DrawWithPaintOpItem(cmdList, handle->paintHandle, IMAGE_RECT_OPITEM), src_(handle->src), dst_(handle->dst),
      sampling_(handle->sampling), constraint_(handle->constraint), isForeground_(handle->isForeground)
{
    image_ = CmdListHelper::GetImageFromCmdList(cmdList, handle->image);
    if (DrawOpItem::holdDrawingImagefunc_) {
        DrawOpItem::holdDrawingImagefunc_(image_);
    }
}

DrawImageRectOpItem::DrawImageRectOpItem(const Image& image, const Rect& src,
    const Rect& dst, const SamplingOptions& sampling,
    SrcRectConstraint constraint, const Paint& paint, bool isForeground)
    : DrawWithPaintOpItem(paint, DrawOpItem::IMAGE_RECT_OPITEM), src_(src), dst_(dst), sampling_(sampling),
    constraint_(constraint), image_(std::make_shared<Image>(image)), isForeground_(isForeground)
{
    if (DrawOpItem::holdDrawingImagefunc_) {
        DrawOpItem::holdDrawingImagefunc_(image_);
    }
}

std::shared_ptr<DrawOpItem> DrawImageRectOpItem::Unmarshalling(const DrawCmdList& cmdList, void* handle)
{
    return std::make_shared<DrawImageRectOpItem>(cmdList, static_cast<DrawImageRectOpItem::ConstructorHandle*>(handle));
}

void DrawImageRectOpItem::Marshalling(DrawCmdList& cmdList)
{
    PaintHandle paintHandle;
    GenerateHandleFromPaint(cmdList, paint_, paintHandle);
    OpDataHandle imageHandle;
    if (image_) {
        imageHandle = CmdListHelper::AddImageToCmdList(cmdList, *image_);
    }
    cmdList.AddOp<ConstructorHandle>(imageHandle, src_, dst_, sampling_, constraint_, paintHandle);
}

void DrawImageRectOpItem::Playback(Canvas* canvas, const Rect* rect)
{
    if (image_ == nullptr) {
        LOGD("DrawImageRectOpItem image is null");
        return;
    }
    if (isForeground_) {
        AutoCanvasRestore acr(*canvas, false);
        SaveLayerOps ops;
        canvas->SaveLayer(ops);
        canvas->AttachPaint(paint_);
        canvas->DrawImageRect(*image_, src_, dst_, sampling_, constraint_);
        Brush brush;
        brush.SetColor(canvas->GetEnvForegroundColor());
        brush.SetBlendMode(Drawing::BlendMode::SRC_IN);
        canvas->DrawBackground(brush);
        return;
    }
    canvas->AttachPaint(paint_);
    canvas->DrawImageRect(*image_, src_, dst_, sampling_, constraint_);
}

void DrawImageRectOpItem::DumpItems(std::string& out) const
{
    out += " Src";
    src_.Dump(out);
    out += " Dst";
    src_.Dump(out);
    out += " Sampling";
    sampling_.Dump(out);
    out += " constraint:" + std::to_string(static_cast<int>(constraint_));
    if (image_ != nullptr) {
        out += " Image";
        image_->Dump(out);
    }
    out += " isForeground:" + std::string(isForeground_ ? "true" : "false");
}

/* DrawRecordCmdOpItem */
UNMARSHALLING_REGISTER(DrawRecordCmd, DrawOpItem::RECORD_CMD_OPITEM,
    DrawRecordCmdOpItem::Unmarshalling, sizeof(DrawRecordCmdOpItem::ConstructorHandle));

DrawRecordCmdOpItem::DrawRecordCmdOpItem(
    const DrawCmdList& cmdList, DrawRecordCmdOpItem::ConstructorHandle* handle)
    : DrawOpItem(RECORD_CMD_OPITEM), hasBrush_(handle->hasBrush)
{
    recordCmd_ = CmdListHelper::GetRecordCmdFromCmdList(cmdList, handle->recordCmdHandle);
    matrix_.SetAll(handle->matrixBuffer);
    if (hasBrush_) {
        BrushHandleToBrush(handle->brushHandle, cmdList, brush_);
    }
}

DrawRecordCmdOpItem::DrawRecordCmdOpItem(const std::shared_ptr<RecordCmd>& recordCmd,
    const Matrix* matrix, const Brush* brush)
    : DrawOpItem(RECORD_CMD_OPITEM), recordCmd_(recordCmd)
{
    if (matrix != nullptr) {
        matrix_ = *matrix;
    }
    if (brush != nullptr) {
        hasBrush_ = true;
        brush_ = *brush;
    }
}

std::shared_ptr<DrawOpItem> DrawRecordCmdOpItem::Unmarshalling(const DrawCmdList& cmdList, void* handle)
{
    return std::make_shared<DrawRecordCmdOpItem>(
        cmdList, static_cast<DrawRecordCmdOpItem::ConstructorHandle*>(handle));
}

void DrawRecordCmdOpItem::Marshalling(DrawCmdList& cmdList)
{
    auto recordCmdHandle = CmdListHelper::AddRecordCmdToCmdList(cmdList, recordCmd_);
    Matrix::Buffer matrixBuffer;
    matrix_.GetAll(matrixBuffer);
    BrushHandle brushHandle;
    if (hasBrush_) {
        BrushToBrushHandle(brush_, cmdList, brushHandle);
    }
    cmdList.AddOp<ConstructorHandle>(recordCmdHandle,
        matrixBuffer, hasBrush_, brushHandle);
}

void DrawRecordCmdOpItem::Playback(Canvas* canvas, const Rect* rect)
{
    Brush* brushPtr = hasBrush_ ? &brush_ : nullptr;
    canvas->DrawRecordCmd(recordCmd_, &matrix_, brushPtr);
}

/* DrawPictureOpItem */
UNMARSHALLING_REGISTER(DrawPicture, DrawOpItem::PICTURE_OPITEM,
    DrawPictureOpItem::Unmarshalling, sizeof(DrawPictureOpItem::ConstructorHandle));

DrawPictureOpItem::DrawPictureOpItem(const DrawCmdList& cmdList, DrawPictureOpItem::ConstructorHandle* handle)
    : DrawOpItem(PICTURE_OPITEM)
{
    picture_ = CmdListHelper::GetPictureFromCmdList(cmdList, handle->picture);
}

std::shared_ptr<DrawOpItem> DrawPictureOpItem::Unmarshalling(const DrawCmdList& cmdList, void* handle)
{
    return std::make_shared<DrawPictureOpItem>(cmdList, static_cast<DrawPictureOpItem::ConstructorHandle*>(handle));
}

void DrawPictureOpItem::Marshalling(DrawCmdList& cmdList)
{
    OpDataHandle pictureHandle;
    if (picture_) {
        pictureHandle = CmdListHelper::AddPictureToCmdList(cmdList, *picture_);
    }
    cmdList.AddOp<ConstructorHandle>(pictureHandle);
}

void DrawPictureOpItem::Playback(Canvas* canvas, const Rect* rect)
{
    if (picture_ == nullptr) {
        LOGD("DrawPictureOpItem picture is null");
        return;
    }
    canvas->DrawPicture(*picture_);
}

/* DrawTextBlobOpItem */
UNMARSHALLING_REGISTER(DrawTextBlob, DrawOpItem::TEXT_BLOB_OPITEM,
    DrawTextBlobOpItem::Unmarshalling, sizeof(DrawTextBlobOpItem::ConstructorHandle));

void SimplifyPaint(ColorQuad colorQuad, Paint& paint)
{
    Color color{colorQuad};
    paint.SetColor(color);
    paint.SetShaderEffect(nullptr);
    if (paint.HasFilter()) {
        Filter filter = paint.GetFilter();
        if (filter.GetColorFilter() != nullptr) {
            filter.SetColorFilter(nullptr);
            paint.SetFilter(filter);
        }
    }
    paint.SetWidth(1.04); // 1.04 is empirical value
    paint.SetJoinStyle(Pen::JoinStyle::ROUND_JOIN);
}

DrawTextBlobOpItem::DrawTextBlobOpItem(const DrawCmdList& cmdList, DrawTextBlobOpItem::ConstructorHandle* handle)
    : DrawWithPaintOpItem(cmdList, handle->paintHandle, TEXT_BLOB_OPITEM), x_(handle->x), y_(handle->y)
{
    globalUniqueId_ = handle->globalUniqueId;
    textContrast_ = handle->textContrast;
    textBlob_ = CmdListHelper::GetTextBlobFromCmdList(cmdList, handle->textBlob, handle->globalUniqueId);
    textBlob_->SetTextContrast(textContrast_);
}

std::shared_ptr<DrawOpItem> DrawTextBlobOpItem::Unmarshalling(const DrawCmdList& cmdList, void* handle)
{
    return std::make_shared<DrawTextBlobOpItem>(cmdList, static_cast<DrawTextBlobOpItem::ConstructorHandle*>(handle));
}

void DrawTextBlobOpItem::Marshalling(DrawCmdList& cmdList)
{
    static uint64_t shiftedPid = static_cast<uint64_t>(GetRealPid()) << 32; // 32 for 64-bit unsignd number shift
    PaintHandle paintHandle;
    GenerateHandleFromPaint(cmdList, paint_, paintHandle);
    TextBlob::Context ctx {nullptr, false};
    auto textBlobHandle = CmdListHelper::AddTextBlobToCmdList(cmdList, textBlob_.get(), &ctx);
    uint64_t globalUniqueId = 0;
    if (ctx.GetTypeface() != nullptr) {
        uint32_t typefaceId = ctx.GetTypeface()->GetUniqueID();
        globalUniqueId = (shiftedPid | typefaceId);
    }

    cmdList.AddOp<ConstructorHandle>(textBlobHandle,
        globalUniqueId, textBlob_->GetTextContrast(), x_, y_, paintHandle);
}

uint64_t DrawTextBlobOpItem::GetTypefaceId()
{
    return globalUniqueId_;
}

void DrawTextBlobOpItem::Playback(Canvas* canvas, const Rect* rect)
{
    if (textBlob_ == nullptr) {
        LOGD("DrawTextBlobOpItem textBlob is null");
        return;
    }
    RectI globalClipBounds = canvas->GetDeviceClipBounds();
    bool saveFlag = false;
    if (globalClipBounds.GetWidth() == 1 || globalClipBounds.GetHeight() == 1) {
        // In this case, the clip area for textblob must have AA.
        Matrix m = canvas->GetTotalMatrix();
        canvas->Save();
        canvas->SetMatrix(m);
        auto bounds = textBlob_->Bounds();
        if (bounds && bounds->IsValid()) {
            canvas->ClipRect(*bounds, ClipOp::INTERSECT, true);
        }
        saveFlag = true;
    }
    TextContrast customerEnableValue = textBlob_->GetTextContrast();
    if (IsHighContrastEnable(canvas, customerEnableValue)) {
        LOGD("DrawTextBlobOpItem::Playback highContrastEnabled, %{public}s, %{public}d", __FUNCTION__, __LINE__);
        DrawHighContrastEnabled(canvas);
    } else {
        canvas->AttachPaint(paint_);
        canvas->DrawTextBlob(textBlob_.get(), x_, y_);
    }
    if (saveFlag) {
        canvas->Restore();
    }
}

bool DrawTextBlobOpItem::IsHighContrastEnable(Canvas* canvas, TextContrast value) const
{
    bool canvasHighContrastEnabled = canvas->isHighContrastEnabled();
    switch (value) {
        case TextContrast::FOLLOW_SYSTEM:
            return canvasHighContrastEnabled;
        case TextContrast::DISABLE_CONTRAST:
            return false;
        case TextContrast::ENABLE_CONTRAST:
            return true;
        default:
            return canvasHighContrastEnabled;
    }
}

bool DrawTextBlobOpItem::GetOffScreenSurfaceAndCanvas(const Canvas& canvas,
    std::shared_ptr<Drawing::Surface>& offScreenSurface, std::shared_ptr<Canvas>& offScreenCanvas) const
{
    auto surface = canvas.GetSurface();
    auto textBlobBounds = textBlob_->Bounds();
    if (!surface || !textBlobBounds) {
        return false;
    }
    offScreenSurface = surface->MakeSurface(textBlobBounds->GetWidth(), textBlobBounds->GetHeight());
    if (!offScreenSurface) {
        return false;
    }
    offScreenCanvas = offScreenSurface->GetCanvas();
    if (!offScreenCanvas) {
        return false;
    }
    offScreenCanvas->Translate(-textBlobBounds->GetLeft(), -textBlobBounds->GetTop());
    return true;
}

void DrawTextBlobOpItem::DrawHighContrastEnabled(Canvas* canvas) const
{
    ColorQuad colorQuad = paint_.GetColor().CastToColorQuad();
    if (Color::ColorQuadGetA(colorQuad) == 0 || paint_.HasFilter()) {
        canvas->AttachPaint(paint_);
        canvas->DrawTextBlob(textBlob_.get(), x_, y_);
        return;
    }
    // in case of perceptible transparent, text should be drawn offscreen to avoid stroke and content overlap.
    if (canvas->GetAlphaSaveCount() > 0 && canvas->GetAlpha() < HIGH_CONTRAST_OFFSCREEN_THREASHOLD) {
        std::shared_ptr<Drawing::Surface> offScreenSurface;
        std::shared_ptr<Canvas> offScreenCanvas;
        if (GetOffScreenSurfaceAndCanvas(*canvas, offScreenSurface, offScreenCanvas)) {
            DrawHighContrast(offScreenCanvas.get(), true);
            offScreenCanvas->Flush();
            auto image = offScreenSurface->GetImageSnapshot();
            if (image == nullptr) {
                return;
            }
            Drawing::Brush paint;
            paint.SetAntiAlias(true);
            canvas->AttachBrush(paint);
            Drawing::SamplingOptions sampling =
                Drawing::SamplingOptions(Drawing::FilterMode::NEAREST, Drawing::MipmapMode::NEAREST);
            canvas->DrawImage(*image, x_ + textBlob_->Bounds()->GetLeft(),
                y_ + textBlob_->Bounds()->GetTop(), sampling);
            canvas->DetachBrush();
            return;
        }
    }
    DrawHighContrast(canvas);
}

void DrawTextBlobOpItem::DrawHighContrast(Canvas* canvas, bool offScreen) const
{
    ColorQuad colorQuad = paint_.GetColor().CastToColorQuad();
    uint32_t channelSum = Color::ColorQuadGetR(colorQuad) + Color::ColorQuadGetG(colorQuad) +
        Color::ColorQuadGetB(colorQuad);
    bool flag = channelSum < 594; // 594 is empirical value

    // draw outline stroke with pen
    Drawing::Pen outlinePen;
    outlinePen.SetColor(flag ? Color::COLOR_WHITE : Color::COLOR_BLACK);
    outlinePen.SetAntiAlias(true);
    outlinePen.SetBlenderEnabled(false);
    canvas->AttachPen(outlinePen);
    offScreen ? canvas->DrawTextBlob(textBlob_.get(), 0, 0) : canvas->DrawTextBlob(textBlob_.get(), x_, y_);

    // draw inner content with brush
    Drawing::Brush innerBrush;
    innerBrush.SetColor(flag ? Color::COLOR_BLACK : Color::COLOR_WHITE);
    canvas->DetachPen();
    canvas->AttachBrush(innerBrush);
    offScreen ? canvas->DrawTextBlob(textBlob_.get(), 0, 0) : canvas->DrawTextBlob(textBlob_.get(), x_, y_);
    canvas->DetachBrush();
}

bool DrawTextBlobOpItem::ConstructorHandle::GenerateCachedOpItem(
    DrawCmdList& cmdList, const TextBlob* textBlob, scalar x, scalar y, Paint& p)
{
    if (!textBlob) {
        LOGD("textBlob nullptr, %{public}s, %{public}d", __FUNCTION__, __LINE__);
        return false;
    }

    auto bounds = textBlob->Bounds();
    if (!bounds || !bounds->IsValid()) {
        return false;
    }
    bounds->Offset(x, y);
    bounds->MakeOutset(TEXT_BLOB_CACHE_MARGIN, TEXT_BLOB_CACHE_MARGIN);
    // create CPU raster surface
    Drawing::ImageInfo offscreenInfo { bounds->GetWidth(), bounds->GetHeight(),
        Drawing::COLORTYPE_RGBA_8888, Drawing::ALPHATYPE_PREMUL, nullptr};
    std::shared_ptr<Surface> offscreenSurface = Surface::MakeRaster(offscreenInfo);
    if (offscreenSurface == nullptr) {
        return false;
    }
    auto offscreenCanvas = offscreenSurface->GetCanvas();
    if (offscreenCanvas == nullptr) {
        return false;
    }
    if (bounds->GetLeft() != 0 || bounds->GetTop() != 0) {
        offscreenCanvas->Translate(-bounds->GetLeft(), -bounds->GetTop());
    }

    //OffscreenCanvas used once, detach is unnecessary, FakeBrush/Pen avoid affecting ImageRectOp, attach is necessary.
    bool isForeground = false;
    if (p.GetColor() == Drawing::Color::COLOR_FOREGROUND) {
        isForeground = true;
        p.SetColor(Drawing::Color::COLOR_BLACK);
    }
    offscreenCanvas->AttachPaint(p);
    offscreenCanvas->DrawTextBlob(textBlob, x, y);

    std::shared_ptr<Image> image = offscreenSurface->GetImageSnapshot();
    if (image == nullptr) {
        return false;
    }
    Drawing::Rect src(0, 0, image->GetWidth(), image->GetHeight());
    Drawing::Rect dst(bounds->GetLeft(), bounds->GetTop(),
        bounds->GetLeft() + image->GetWidth(), bounds->GetTop() + image->GetHeight());
    SamplingOptions sampling(FilterMode::LINEAR, MipmapMode::LINEAR);
    auto imageHandle = CmdListHelper::AddImageToCmdList(cmdList, image);
    PaintHandle fakePaintHandle;
    fakePaintHandle.isAntiAlias = true;
    fakePaintHandle.style = Paint::PaintStyle::PAINT_FILL;
    cmdList.AddOp<DrawImageRectOpItem::ConstructorHandle>(
        imageHandle, src, dst, sampling, SrcRectConstraint::STRICT_SRC_RECT_CONSTRAINT, fakePaintHandle, isForeground);
    return true;
}

bool DrawTextBlobOpItem::ConstructorHandle::GenerateCachedOpItem(DrawCmdList& cmdList, Canvas* canvas)
{
    std::shared_ptr<TextBlob> textBlob_ = CmdListHelper::GetTextBlobFromCmdList(cmdList, textBlob, globalUniqueId);
    if (!textBlob_) {
        LOGD("textBlob nullptr, %{public}s, %{public}d", __FUNCTION__, __LINE__);
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
    if (!offscreenCanvas) {
        return false;
    }

    // align draw op to [0, 0]
    if (bounds->GetLeft() != 0 || bounds->GetTop() != 0) {
        offscreenCanvas->Translate(-bounds->GetLeft(), -bounds->GetTop());
    }

    Paint p;
    GeneratePaintFromHandle(paintHandle, cmdList, p);
    offscreenCanvas->AttachPaint(p);
    offscreenCanvas->DrawTextBlob(textBlob_.get(), x, y);

    std::shared_ptr<Image> image = offscreenSurface->GetImageSnapshot();
    if (image == nullptr) {
        return false;
    }
    Drawing::Rect src(0, 0, image->GetWidth(), image->GetHeight());
    Drawing::Rect dst(bounds->GetLeft(), bounds->GetTop(),
        bounds->GetLeft() + image->GetWidth(), bounds->GetTop() + image->GetHeight());
    SamplingOptions sampling;
    auto imageHandle = CmdListHelper::AddImageToCmdList(cmdList, image);
    PaintHandle fakePaintHandle;
    fakePaintHandle.isAntiAlias = true;
    fakePaintHandle.style = Paint::PaintStyle::PAINT_FILL;
    cmdList.AddOp<DrawImageRectOpItem::ConstructorHandle>(imageHandle, src, dst, sampling,
        SrcRectConstraint::FAST_SRC_RECT_CONSTRAINT, fakePaintHandle);
    return true;
}

std::shared_ptr<DrawImageRectOpItem> DrawTextBlobOpItem::GenerateCachedOpItem(Canvas* canvas)
{
    if (!textBlob_) {
        LOGD("textBlob nullptr, %{public}s, %{public}d", __FUNCTION__, __LINE__);
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
    if (!offscreenCanvas) {
        return nullptr;
    }

    // align draw op to [0, 0]
    if (bounds->GetLeft() != 0 || bounds->GetTop() != 0) {
        offscreenCanvas->Translate(-bounds->GetLeft(), -bounds->GetTop());
    }

    Playback(offscreenCanvas, nullptr);

    std::shared_ptr<Image> image = offscreenSurface->GetImageSnapshot();
    if (image == nullptr) {
        return nullptr;
    }
    Drawing::Rect src(0, 0, image->GetWidth(), image->GetHeight());
    Drawing::Rect dst(bounds->GetLeft(), bounds->GetTop(), bounds->GetRight(), bounds->GetBottom());
    Paint fakePaint;
    fakePaint.SetStyle(Paint::PaintStyle::PAINT_FILL);
    fakePaint.SetAntiAlias(true);
    return std::make_shared<DrawImageRectOpItem>(*image, src, dst, SamplingOptions(),
        SrcRectConstraint::FAST_SRC_RECT_CONSTRAINT, fakePaint);
}

void DrawTextBlobOpItem::DumpItems(std::string& out) const
{
    out += " scalarX:" + std::to_string(x_) + " scalarY:" + std::to_string(y_);
    if (textBlob_ != nullptr) {
        out += " TextBlob[";
        out += "UniqueID:" + std::to_string(textBlob_->UniqueID());
        std::vector<uint16_t> glyphIds;
        TextBlob::GetDrawingGlyphIDforTextBlob(textBlob_.get(), glyphIds);
        out += " GlyphID[";
        for (size_t index = 0; index < std::min(glyphIds.size(), MAX_GLYPH_ID_COUNT); index++) {
            out += " " + std::to_string(glyphIds[index]);
        }
        out += "]";
        switch (textBlob_ -> GetTextContrast()) {
            case TextContrast::FOLLOW_SYSTEM:
                out += " TextContrast: FOLLOW_SYSTEM";
                break;
            case TextContrast::DISABLE_CONTRAST:
                out += " TextContrast: DISABLE_CONTRAST";
                break;
            case TextContrast::ENABLE_CONTRAST:
                out += " TextContrast: ENABLE_CONTRAST";
                break;
            default:
                break;
        }
        auto bounds = textBlob_->Bounds();
        if (bounds != nullptr) {
            out += " Bounds";
            bounds->Dump(out);
        }
        out += " isEmoji:" + std::string(textBlob_->IsEmoji() ? "true" : "false");
        out += ']';
    }
}

/* DrawSymbolOpItem */
UNMARSHALLING_REGISTER(DrawSymbol, DrawOpItem::SYMBOL_OPITEM,
    DrawSymbolOpItem::Unmarshalling, sizeof(DrawSymbolOpItem::ConstructorHandle));

DrawSymbolOpItem::DrawSymbolOpItem(const DrawCmdList& cmdList, DrawSymbolOpItem::ConstructorHandle* handle)
    : DrawWithPaintOpItem(cmdList, handle->paintHandle, SYMBOL_OPITEM), locate_(handle->locate)
{
    symbol_ = CmdListHelper::GetSymbolFromCmdList(cmdList, handle->symbolHandle);
}

std::shared_ptr<DrawOpItem> DrawSymbolOpItem::Unmarshalling(const DrawCmdList& cmdList, void* handle)
{
    return std::make_shared<DrawSymbolOpItem>(cmdList, static_cast<DrawSymbolOpItem::ConstructorHandle*>(handle));
}

void DrawSymbolOpItem::Marshalling(DrawCmdList& cmdList)
{
    PaintHandle paintHandle;
    GenerateHandleFromPaint(cmdList, paint_, paintHandle);
    auto symbolHandle = CmdListHelper::AddSymbolToCmdList(cmdList, symbol_);
    cmdList.AddOp<ConstructorHandle>(symbolHandle, locate_, paintHandle);
}

void DrawSymbolOpItem::Playback(Canvas* canvas, const Rect* rect)
{
    if (!canvas) {
        LOGD("SymbolOpItem::Playback failed cause by canvas is nullptr");
        return;
    }
    Path path(symbol_.path_);

    // 1.0 move path
    path.Offset(locate_.GetX(), locate_.GetY());

    // 2.0 split path
    std::vector<Path> paths;
    DrawingHMSymbol::PathOutlineDecompose(path, paths);
    std::vector<Path> pathLayers;
    DrawingHMSymbol::MultilayerPath(symbol_.symbolInfo_.layers, paths, pathLayers);

    // 3.0 set paint
    Paint paintCopy = paint_;
    paintCopy.SetAntiAlias(true);
    paintCopy.SetStyle(Paint::PaintStyle::PAINT_FILL_STROKE);
    paintCopy.SetWidth(0.0f);
    paintCopy.SetJoinStyle(Pen::JoinStyle::ROUND_JOIN);

    // draw path
    std::vector<DrawingRenderGroup> groups = symbol_.symbolInfo_.renderGroups;
    LOGD("SymbolOpItem::Draw RenderGroup size %{public}d", static_cast<int>(groups.size()));
    if (groups.size() == 0) {
        canvas->AttachPaint(paintCopy);
        canvas->DrawPath(path);
    }
    for (auto group : groups) {
        Path multPath;
        MergeDrawingPath(multPath, group, pathLayers);
        // color
        paintCopy.SetColor(Color::ColorQuadSetARGB(0xFF, group.color.r, group.color.g, group.color.b));
        paintCopy.SetAlphaF(group.color.a);
        canvas->AttachPaint(paintCopy);
        canvas->DrawPath(multPath);
    }
}

void DrawSymbolOpItem::MergeDrawingPath(
    Drawing::Path& multPath, Drawing::DrawingRenderGroup& group, std::vector<Drawing::Path>& pathLayers)
{
    for (auto groupInfo : group.groupInfos) {
        Drawing::Path pathTemp;
        for (auto k : groupInfo.layerIndexes) {
            if (k >= pathLayers.size()) {
                continue;
            }
            pathTemp.AddPath(pathLayers[k]);
        }
        for (size_t h : groupInfo.maskIndexes) {
            if (h >= pathLayers.size()) {
                continue;
            }
            Drawing::Path outPath;
            auto isOk = outPath.Op(pathTemp, pathLayers[h], Drawing::PathOp::DIFFERENCE);
            if (isOk) {
                pathTemp = outPath;
            }
        }
        multPath.AddPath(pathTemp);
    }
}

void DrawSymbolOpItem::DumpItems(std::string& out) const
{
    out += " symbol[symbolId:" + std::to_string(symbol_.symbolId);
    out += " DrawingType:" + std::to_string(static_cast<int>(symbol_.path_.GetDrawingType()));
    auto rect = symbol_.path_.GetBounds();
    out += " path";
    rect.Dump(out);
    out += " symbolGlyphId:" + std::to_string(symbol_.symbolInfo_.symbolGlyphId);
    out += "]";
    out += " locate";
    locate_.Dump(out);
}

/* ClipRectOpItem */
UNMARSHALLING_REGISTER(ClipRect, DrawOpItem::CLIP_RECT_OPITEM,
    ClipRectOpItem::Unmarshalling, sizeof(ClipRectOpItem::ConstructorHandle));

ClipRectOpItem::ClipRectOpItem(ClipRectOpItem::ConstructorHandle* handle)
    : DrawOpItem(CLIP_RECT_OPITEM), rect_(handle->rect), clipOp_(handle->clipOp), doAntiAlias_(handle->doAntiAlias) {}

std::shared_ptr<DrawOpItem> ClipRectOpItem::Unmarshalling(const DrawCmdList& cmdList, void* handle)
{
    return std::make_shared<ClipRectOpItem>(static_cast<ClipRectOpItem::ConstructorHandle*>(handle));
}

void ClipRectOpItem::Marshalling(DrawCmdList& cmdList)
{
    cmdList.AddOp<ConstructorHandle>(rect_, clipOp_, doAntiAlias_);
}

void ClipRectOpItem::Playback(Canvas* canvas, const Rect* rect)
{
    canvas->ClipRect(rect_, clipOp_, doAntiAlias_);
}

void ClipRectOpItem::Dump(std::string& out) const
{
    out += GetOpDesc() + "[rect";
    rect_.Dump(out);
    out += " clipOp:" + std::to_string(static_cast<int>(clipOp_));
    out += " antiAlias:" + std::string(doAntiAlias_ ? "true" : "false");
    out += "]";
}

/* ClipIRectOpItem */
UNMARSHALLING_REGISTER(ClipIRect, DrawOpItem::CLIP_IRECT_OPITEM,
    ClipIRectOpItem::Unmarshalling, sizeof(ClipIRectOpItem::ConstructorHandle));

ClipIRectOpItem::ClipIRectOpItem(ClipIRectOpItem::ConstructorHandle* handle)
    : DrawOpItem(CLIP_IRECT_OPITEM), rect_(handle->rect), clipOp_(handle->clipOp) {}

std::shared_ptr<DrawOpItem> ClipIRectOpItem::Unmarshalling(const DrawCmdList& cmdList, void* handle)
{
    return std::make_shared<ClipIRectOpItem>(static_cast<ClipIRectOpItem::ConstructorHandle*>(handle));
}

void ClipIRectOpItem::Marshalling(DrawCmdList& cmdList)
{
    cmdList.AddOp<ConstructorHandle>(rect_, clipOp_);
}

void ClipIRectOpItem::Playback(Canvas* canvas, const Rect* rect)
{
    canvas->ClipIRect(rect_, clipOp_);
}

void ClipIRectOpItem::Dump(std::string& out) const
{
    out += GetOpDesc() + "[rect";
    rect_.Dump(out);
    out += " clipOp:" + std::to_string(static_cast<int>(clipOp_));
    out += "]";
}

/* ClipRoundRectOpItem */
UNMARSHALLING_REGISTER(ClipRoundRect, DrawOpItem::CLIP_ROUND_RECT_OPITEM,
    ClipRoundRectOpItem::Unmarshalling, sizeof(ClipRoundRectOpItem::ConstructorHandle));

ClipRoundRectOpItem::ClipRoundRectOpItem(ClipRoundRectOpItem::ConstructorHandle* handle)
    : DrawOpItem(CLIP_ROUND_RECT_OPITEM), rrect_(handle->rrect), clipOp_(handle->clipOp),
    doAntiAlias_(handle->doAntiAlias) {}

std::shared_ptr<DrawOpItem> ClipRoundRectOpItem::Unmarshalling(const DrawCmdList& cmdList, void* handle)
{
    return std::make_shared<ClipRoundRectOpItem>(static_cast<ClipRoundRectOpItem::ConstructorHandle*>(handle));
}

void ClipRoundRectOpItem::Marshalling(DrawCmdList& cmdList)
{
    cmdList.AddOp<ConstructorHandle>(rrect_, clipOp_, doAntiAlias_);
}

void ClipRoundRectOpItem::Playback(Canvas* canvas, const Rect* rect)
{
    canvas->ClipRoundRect(rrect_, clipOp_, doAntiAlias_);
}

void ClipRoundRectOpItem::Dump(std::string& out) const
{
    out += GetOpDesc() + "[rrect";
    rrect_.Dump(out);
    out += " clipOp:" + std::to_string(static_cast<int>(clipOp_));
    out += " antiAlias:" + std::string(doAntiAlias_ ? "true" : "false");
    out += "]";
}

/* ClipPathOpItem */
UNMARSHALLING_REGISTER(ClipPath, DrawOpItem::CLIP_PATH_OPITEM,
    ClipPathOpItem::Unmarshalling, sizeof(ClipPathOpItem::ConstructorHandle));

ClipPathOpItem::ClipPathOpItem(const DrawCmdList& cmdList, ClipPathOpItem::ConstructorHandle* handle)
    : DrawOpItem(CLIP_PATH_OPITEM), clipOp_(handle->clipOp), doAntiAlias_(handle->doAntiAlias)
{
    path_ = CmdListHelper::GetPathFromCmdList(cmdList, handle->path);
}

std::shared_ptr<DrawOpItem> ClipPathOpItem::Unmarshalling(const DrawCmdList& cmdList, void* handle)
{
    return std::make_shared<ClipPathOpItem>(cmdList, static_cast<ClipPathOpItem::ConstructorHandle*>(handle));
}

void ClipPathOpItem::Marshalling(DrawCmdList& cmdList)
{
    OpDataHandle pathHandle;
    if (path_) {
        pathHandle = CmdListHelper::AddPathToCmdList(cmdList, *path_);
    }
    cmdList.AddOp<ConstructorHandle>(pathHandle, clipOp_, doAntiAlias_);
}

void ClipPathOpItem::Playback(Canvas* canvas, const Rect* rect)
{
    if (path_ == nullptr) {
        LOGD("ClipPathOpItem path is null!");
        return;
    }
    canvas->ClipPath(*path_, clipOp_, doAntiAlias_);
}

/* ClipRegionOpItem */
UNMARSHALLING_REGISTER(ClipRegion, DrawOpItem::CLIP_REGION_OPITEM,
    ClipRegionOpItem::Unmarshalling, sizeof(ClipRegionOpItem::ConstructorHandle));

ClipRegionOpItem::ClipRegionOpItem(const DrawCmdList& cmdList, ClipRegionOpItem::ConstructorHandle* handle)
    : DrawOpItem(CLIP_REGION_OPITEM), clipOp_(handle->clipOp)
{
    region_ = CmdListHelper::GetRegionFromCmdList(cmdList, handle->region);
}

std::shared_ptr<DrawOpItem> ClipRegionOpItem::Unmarshalling(const DrawCmdList& cmdList, void* handle)
{
    return std::make_shared<ClipRegionOpItem>(cmdList, static_cast<ClipRegionOpItem::ConstructorHandle*>(handle));
}

void ClipRegionOpItem::Marshalling(DrawCmdList& cmdList)
{
    OpDataHandle regionHandle;
    if (region_) {
        regionHandle = CmdListHelper::AddRegionToCmdList(cmdList, *region_);
    }
    cmdList.AddOp<ConstructorHandle>(regionHandle, clipOp_);
}

void ClipRegionOpItem::Playback(Canvas* canvas, const Rect* rect)
{
    if (region_ == nullptr) {
        LOGD("ClipRegionOpItem region is null!");
        return;
    }
    canvas->ClipRegion(*region_, clipOp_);
}

void ClipRegionOpItem::Dump(std::string& out) const
{
    out += GetOpDesc() + "[clipOp:";
    out += std::to_string(static_cast<int>(clipOp_)) + " region";
    if (region_) {
        region_->Dump(out);
    } else {
        out += "[null]";
    }
    out += "]";
}

/* SetMatrixOpItem */
UNMARSHALLING_REGISTER(SetMatrix, DrawOpItem::SET_MATRIX_OPITEM,
    SetMatrixOpItem::Unmarshalling, sizeof(SetMatrixOpItem::ConstructorHandle));

SetMatrixOpItem::SetMatrixOpItem(SetMatrixOpItem::ConstructorHandle* handle) : DrawOpItem(SET_MATRIX_OPITEM)
{
    matrix_.SetAll(handle->matrixBuffer);
}

std::shared_ptr<DrawOpItem> SetMatrixOpItem::Unmarshalling(const DrawCmdList& cmdList, void* handle)
{
    return std::make_shared<SetMatrixOpItem>(static_cast<SetMatrixOpItem::ConstructorHandle*>(handle));
}

void SetMatrixOpItem::Marshalling(DrawCmdList& cmdList)
{
    Matrix::Buffer matrixBuffer;
    matrix_.GetAll(matrixBuffer);
    cmdList.AddOp<ConstructorHandle>(matrixBuffer);
}

void SetMatrixOpItem::Playback(Canvas* canvas, const Rect* rect)
{
    canvas->SetMatrix(matrix_);
}

void SetMatrixOpItem::Dump(std::string& out) const
{
    out += GetOpDesc() + "[matrix";
    Matrix::Buffer buffer;
    matrix_.GetAll(buffer);
    DumpArray(out, buffer, [](std::string& out, float v) {
        out += std::to_string(v);
    });
    out += "]";
}

/* ResetMatrixOpItem */
UNMARSHALLING_REGISTER(ResetMatrix, DrawOpItem::RESET_MATRIX_OPITEM,
    ResetMatrixOpItem::Unmarshalling, sizeof(ResetMatrixOpItem::ConstructorHandle));

ResetMatrixOpItem::ResetMatrixOpItem() : DrawOpItem(RESET_MATRIX_OPITEM) {}

std::shared_ptr<DrawOpItem> ResetMatrixOpItem::Unmarshalling(const DrawCmdList& cmdList, void* handle)
{
    return std::make_shared<ResetMatrixOpItem>();
}

void ResetMatrixOpItem::Marshalling(DrawCmdList& cmdList)
{
    cmdList.AddOp<ConstructorHandle>();
}

void ResetMatrixOpItem::Playback(Canvas* canvas, const Rect* rect)
{
    canvas->ResetMatrix();
}

/* ConcatMatrixOpItem */
UNMARSHALLING_REGISTER(ConcatMatrix, DrawOpItem::CONCAT_MATRIX_OPITEM,
    ConcatMatrixOpItem::Unmarshalling, sizeof(ConcatMatrixOpItem::ConstructorHandle));

ConcatMatrixOpItem::ConcatMatrixOpItem(ConcatMatrixOpItem::ConstructorHandle* handle) : DrawOpItem(CONCAT_MATRIX_OPITEM)
{
    matrix_.SetAll(handle->matrixBuffer);
}

std::shared_ptr<DrawOpItem> ConcatMatrixOpItem::Unmarshalling(const DrawCmdList& cmdList, void* handle)
{
    return std::make_shared<ConcatMatrixOpItem>(static_cast<ConcatMatrixOpItem::ConstructorHandle*>(handle));
}

void ConcatMatrixOpItem::Marshalling(DrawCmdList& cmdList)
{
    Matrix::Buffer matrixBuffer;
    matrix_.GetAll(matrixBuffer);
    cmdList.AddOp<ConstructorHandle>(matrixBuffer);
}

void ConcatMatrixOpItem::Playback(Canvas* canvas, const Rect* rect)
{
    canvas->ConcatMatrix(matrix_);
}

void ConcatMatrixOpItem::Dump(std::string& out) const
{
    out += GetOpDesc() + "[matrix";
    Matrix::Buffer buffer;
    matrix_.GetAll(buffer);
    DumpArray(out, buffer, [](std::string& out, float v) {
        out += std::to_string(v);
    });
    out += "]";
}

/* TranslateOpItem */
UNMARSHALLING_REGISTER(Translate, DrawOpItem::TRANSLATE_OPITEM,
    TranslateOpItem::Unmarshalling, sizeof(TranslateOpItem::ConstructorHandle));

TranslateOpItem::TranslateOpItem(TranslateOpItem::ConstructorHandle* handle)
    : DrawOpItem(TRANSLATE_OPITEM), dx_(handle->dx), dy_(handle->dy) {}

std::shared_ptr<DrawOpItem> TranslateOpItem::Unmarshalling(const DrawCmdList& cmdList, void* handle)
{
    return std::make_shared<TranslateOpItem>(static_cast<TranslateOpItem::ConstructorHandle*>(handle));
}

void TranslateOpItem::Marshalling(DrawCmdList& cmdList)
{
    cmdList.AddOp<ConstructorHandle>(dx_, dy_);
}

void TranslateOpItem::Playback(Canvas* canvas, const Rect* rect)
{
    canvas->Translate(dx_, dy_);
}

void TranslateOpItem::Dump(std::string& out) const
{
    out += GetOpDesc() + "[x:";
    out += std::to_string(dx_) + " y:";
    out += std::to_string(dy_);
    out += "]";
}

/* ScaleOpItem */
UNMARSHALLING_REGISTER(Scale, DrawOpItem::SCALE_OPITEM,
    ScaleOpItem::Unmarshalling, sizeof(ScaleOpItem::ConstructorHandle));

ScaleOpItem::ScaleOpItem(ScaleOpItem::ConstructorHandle* handle)
    : DrawOpItem(SCALE_OPITEM), sx_(handle->sx), sy_(handle->sy) {}

std::shared_ptr<DrawOpItem> ScaleOpItem::Unmarshalling(const DrawCmdList& cmdList, void* handle)
{
    return std::make_shared<ScaleOpItem>(static_cast<ScaleOpItem::ConstructorHandle*>(handle));
}

void ScaleOpItem::Marshalling(DrawCmdList& cmdList)
{
    cmdList.AddOp<ConstructorHandle>(sx_, sy_);
}

void ScaleOpItem::Playback(Canvas* canvas, const Rect* rect)
{
    canvas->Scale(sx_, sy_);
}

void ScaleOpItem::Dump(std::string& out) const
{
    out += GetOpDesc() + "[x:";
    out += std::to_string(sx_) + " y:";
    out += std::to_string(sy_);
    out += "]";
}

/* RotateOpItem */
UNMARSHALLING_REGISTER(Rotate, DrawOpItem::ROTATE_OPITEM,
    RotateOpItem::Unmarshalling, sizeof(RotateOpItem::ConstructorHandle));

RotateOpItem::RotateOpItem(RotateOpItem::ConstructorHandle* handle)
    : DrawOpItem(ROTATE_OPITEM), deg_(handle->deg), sx_(handle->sx), sy_(handle->sy) {}

std::shared_ptr<DrawOpItem> RotateOpItem::Unmarshalling(const DrawCmdList& cmdList, void* handle)
{
    return std::make_shared<RotateOpItem>(static_cast<RotateOpItem::ConstructorHandle*>(handle));
}

void RotateOpItem::Marshalling(DrawCmdList& cmdList)
{
    cmdList.AddOp<ConstructorHandle>(deg_, sx_, sy_);
}

void RotateOpItem::Playback(Canvas* canvas, const Rect* rect)
{
    canvas->Rotate(deg_, sx_, sy_);
}

void RotateOpItem::Dump(std::string& out) const
{
    out += GetOpDesc() + "[degree:";
    out += std::to_string(deg_) + " x:";
    out += std::to_string(sx_) + " y:";
    out += std::to_string(sy_);
    out += "]";
}

/* ShearOpItem */
UNMARSHALLING_REGISTER(Shear, DrawOpItem::SHEAR_OPITEM,
    ShearOpItem::Unmarshalling, sizeof(ShearOpItem::ConstructorHandle));

ShearOpItem::ShearOpItem(ShearOpItem::ConstructorHandle* handle)
    : DrawOpItem(SHEAR_OPITEM), sx_(handle->sx), sy_(handle->sy) {}

std::shared_ptr<DrawOpItem> ShearOpItem::Unmarshalling(const DrawCmdList& cmdList, void* handle)
{
    return std::make_shared<ShearOpItem>(static_cast<ShearOpItem::ConstructorHandle*>(handle));
}

void ShearOpItem::Marshalling(DrawCmdList& cmdList)
{
    cmdList.AddOp<ConstructorHandle>(sx_, sy_);
}

void ShearOpItem::Playback(Canvas* canvas, const Rect* rect)
{
    canvas->Shear(sx_, sy_);
}

void ShearOpItem::Dump(std::string& out) const
{
    out += GetOpDesc() + "[x:";
    out += std::to_string(sx_) + " y:";
    out += std::to_string(sy_);
    out += "]";
}

/* FlushOpItem */
UNMARSHALLING_REGISTER(Flush, DrawOpItem::FLUSH_OPITEM,
    FlushOpItem::Unmarshalling, sizeof(FlushOpItem::ConstructorHandle));

FlushOpItem::FlushOpItem() : DrawOpItem(FLUSH_OPITEM) {}

std::shared_ptr<DrawOpItem> FlushOpItem::Unmarshalling(const DrawCmdList& cmdList, void* handle)
{
    return std::make_shared<FlushOpItem>();
}

void FlushOpItem::Marshalling(DrawCmdList& cmdList)
{
    cmdList.AddOp<ConstructorHandle>();
}

void FlushOpItem::Playback(Canvas* canvas, const Rect* rect)
{
    canvas->Flush();
}

/* ClearOpItem */
UNMARSHALLING_REGISTER(Clear, DrawOpItem::CLEAR_OPITEM,
    ClearOpItem::Unmarshalling, sizeof(ClearOpItem::ConstructorHandle));

ClearOpItem::ClearOpItem(ClearOpItem::ConstructorHandle* handle)
    : DrawOpItem(CLEAR_OPITEM), color_(handle->color) {}

std::shared_ptr<DrawOpItem> ClearOpItem::Unmarshalling(const DrawCmdList& cmdList, void* handle)
{
    return std::make_shared<ClearOpItem>(static_cast<ClearOpItem::ConstructorHandle*>(handle));
}

void ClearOpItem::Marshalling(DrawCmdList& cmdList)
{
    cmdList.AddOp<ConstructorHandle>(color_);
}

void ClearOpItem::Playback(Canvas* canvas, const Rect* rect)
{
    canvas->Clear(color_);
}

void ClearOpItem::Dump(std::string& out) const
{
    out += GetOpDesc();
    Color(color_).Dump(out);
}

/* SaveOpItem */
UNMARSHALLING_REGISTER(Save, DrawOpItem::SAVE_OPITEM,
    SaveOpItem::Unmarshalling, sizeof(SaveOpItem::ConstructorHandle));

SaveOpItem::SaveOpItem() : DrawOpItem(SAVE_OPITEM) {}

std::shared_ptr<DrawOpItem> SaveOpItem::Unmarshalling(const DrawCmdList& cmdList, void* handle)
{
    return std::make_shared<SaveOpItem>();
}

void SaveOpItem::Marshalling(DrawCmdList& cmdList)
{
    cmdList.AddOp<ConstructorHandle>();
}

void SaveOpItem::Playback(Canvas* canvas, const Rect* rect)
{
    canvas->Save();
}

/* SaveLayerOpItem */
UNMARSHALLING_REGISTER(SaveLayer, DrawOpItem::SAVE_LAYER_OPITEM,
    SaveLayerOpItem::Unmarshalling, sizeof(SaveLayerOpItem::ConstructorHandle));

SaveLayerOpItem::SaveLayerOpItem(const DrawCmdList& cmdList, SaveLayerOpItem::ConstructorHandle* handle)
    : DrawOpItem(SAVE_LAYER_OPITEM), saveLayerFlags_(handle->saveLayerFlags), rect_(handle->rect),
    hasBrush_(handle->hasBrush)
{
    if (hasBrush_) {
        BrushHandleToBrush(handle->brushHandle, cmdList, brush_);
    }
}

std::shared_ptr<DrawOpItem> SaveLayerOpItem::Unmarshalling(const DrawCmdList& cmdList, void* handle)
{
    return std::make_shared<SaveLayerOpItem>(cmdList, static_cast<SaveLayerOpItem::ConstructorHandle*>(handle));
}

void SaveLayerOpItem::Marshalling(DrawCmdList& cmdList)
{
    BrushHandle brushHandle;
    if (hasBrush_) {
        BrushToBrushHandle(brush_, cmdList, brushHandle);
    }
    cmdList.AddOp<ConstructorHandle>(rect_, hasBrush_, brushHandle, saveLayerFlags_);
}

void SaveLayerOpItem::Playback(Canvas* canvas, const Rect* rect)
{
    const Rect* rectPtr = nullptr;
    if (rect_.IsValid()) {
        rectPtr = &rect_;
    }
    Brush* brushPtr = hasBrush_ ? &brush_ : nullptr;
    SaveLayerOps slo(rectPtr, brushPtr, saveLayerFlags_);
    canvas->SaveLayer(slo);
}

void SaveLayerOpItem::Dump(std::string& out) const
{
    out += GetOpDesc() + "[flags:";
    out += std::to_string(saveLayerFlags_) + " rect";
    rect_.Dump(out);
    out += " brush";
    if (hasBrush_) {
        brush_.Dump(out);
    } else {
        out += "[null]";
    }
    out += "]";
}

/* RestoreOpItem */
UNMARSHALLING_REGISTER(Restore, DrawOpItem::RESTORE_OPITEM,
    RestoreOpItem::Unmarshalling, sizeof(RestoreOpItem::ConstructorHandle));

RestoreOpItem::RestoreOpItem() : DrawOpItem(RESTORE_OPITEM) {}

std::shared_ptr<DrawOpItem> RestoreOpItem::Unmarshalling(const DrawCmdList& cmdList, void* handle)
{
    return std::make_shared<RestoreOpItem>();
}

void RestoreOpItem::Marshalling(DrawCmdList& cmdList)
{
    cmdList.AddOp<ConstructorHandle>();
}

void RestoreOpItem::Playback(Canvas* canvas, const Rect* rect)
{
    canvas->Restore();
}

/* DiscardOpItem */
UNMARSHALLING_REGISTER(Discard, DrawOpItem::DISCARD_OPITEM,
    DiscardOpItem::Unmarshalling, sizeof(DiscardOpItem::ConstructorHandle));

DiscardOpItem::DiscardOpItem() : DrawOpItem(DISCARD_OPITEM) {}

std::shared_ptr<DrawOpItem> DiscardOpItem::Unmarshalling(const DrawCmdList& cmdList, void* handle)
{
    return std::make_shared<DiscardOpItem>();
}

void DiscardOpItem::Marshalling(DrawCmdList& cmdList)
{
    cmdList.AddOp<ConstructorHandle>();
}

void DiscardOpItem::Playback(Canvas* canvas, const Rect* rect)
{
    canvas->Discard();
}

/* ClipAdaptiveRoundRectOpItem */
UNMARSHALLING_REGISTER(ClipAdaptiveRoundRect, DrawOpItem::CLIP_ADAPTIVE_ROUND_RECT_OPITEM,
    ClipAdaptiveRoundRectOpItem::Unmarshalling, sizeof(ClipAdaptiveRoundRectOpItem::ConstructorHandle));

ClipAdaptiveRoundRectOpItem::ClipAdaptiveRoundRectOpItem(
    const DrawCmdList& cmdList, ClipAdaptiveRoundRectOpItem::ConstructorHandle* handle)
    : DrawOpItem(CLIP_ADAPTIVE_ROUND_RECT_OPITEM)
{
    radiusData_ = CmdListHelper::GetVectorFromCmdList<Point>(cmdList, handle->radiusData);
}

std::shared_ptr<DrawOpItem> ClipAdaptiveRoundRectOpItem::Unmarshalling(const DrawCmdList& cmdList, void* handle)
{
    return std::make_shared<ClipAdaptiveRoundRectOpItem>(
        cmdList, static_cast<ClipAdaptiveRoundRectOpItem::ConstructorHandle*>(handle));
}

void ClipAdaptiveRoundRectOpItem::Marshalling(DrawCmdList& cmdList)
{
    auto radiusData = CmdListHelper::AddVectorToCmdList<Point>(cmdList, radiusData_);
    cmdList.AddOp<ConstructorHandle>(radiusData);
}

void ClipAdaptiveRoundRectOpItem::Playback(Canvas* canvas, const Rect* rect)
{
    canvas->ClipRoundRect(*rect, radiusData_, true);
}

void ClipAdaptiveRoundRectOpItem::Dump(std::string& out) const
{
    out += GetOpDesc() + "[radius";
    DumpArray(out, radiusData_, [](std::string& out, const Point& p) {
        out += "[";
        out += std::to_string(p.GetX()) + " ";
        out += std::to_string(p.GetY());
        out += "]";
    });
    out += "]";
}

/* HybridRenderPixelMapSizeOpItem */
UNMARSHALLING_REGISTER(ResetHybridRenderSize, DrawOpItem::HYBRID_RENDER_PIXELMAP_SIZE_OPITEM,
    HybridRenderPixelMapSizeOpItem::Unmarshalling, sizeof(HybridRenderPixelMapSizeOpItem::ConstructorHandle));

HybridRenderPixelMapSizeOpItem::HybridRenderPixelMapSizeOpItem(
    HybridRenderPixelMapSizeOpItem::ConstructorHandle* handle)
    : DrawOpItem(HYBRID_RENDER_PIXELMAP_SIZE_OPITEM), width_(handle->width), height_(handle->height) {}

std::shared_ptr<DrawOpItem> HybridRenderPixelMapSizeOpItem::Unmarshalling(const DrawCmdList& cmdList, void* handle)
{
    return std::make_shared<HybridRenderPixelMapSizeOpItem>(
        static_cast<HybridRenderPixelMapSizeOpItem::ConstructorHandle*>(handle));
}

void HybridRenderPixelMapSizeOpItem::Marshalling(DrawCmdList& cmdList)
{
    cmdList.AddOp<ConstructorHandle>(width_, height_);
}

void HybridRenderPixelMapSizeOpItem::Playback(Canvas* canvas, const Rect* rect)
{
    return;
}

void HybridRenderPixelMapSizeOpItem::Dump(std::string& out) const
{
    out += GetOpDesc();
}

float HybridRenderPixelMapSizeOpItem::GetWidth() const
{
    return width_;
}

float HybridRenderPixelMapSizeOpItem::GetHeight() const
{
    return height_;
}
} // namespace Drawing
} // namespace Rosen
} // namespace OHOS
