/*
 * Copyright (c) 2021-2023 Huawei Device Co., Ltd.
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

#include "pipeline/rs_draw_cmd_list.h"

#include <unordered_map>

#include "rs_trace.h"

#include "pipeline/rs_draw_cmd.h"
#include "pipeline/rs_paint_filter_canvas.h"
#include "platform/common/rs_log.h"
#include "transaction/rs_marshalling_helper.h"

namespace OHOS {
namespace Rosen {
using OpUnmarshallingFunc = OpItem* (*)(Parcel& parcel);

static std::unordered_map<RSOpType, OpUnmarshallingFunc> opUnmarshallingFuncLUT = {
    { RECT_OPITEM,                 RectOpItem::Unmarshalling },
    { ROUND_RECT_OPITEM,           RoundRectOpItem::Unmarshalling },
    { IMAGE_WITH_PARM_OPITEM,      ImageWithParmOpItem::Unmarshalling },
    { DRRECT_OPITEM,               DRRectOpItem::Unmarshalling },
    { OVAL_OPITEM,                 OvalOpItem::Unmarshalling },
    { REGION_OPITEM,               RegionOpItem::Unmarshalling },
    { ARC_OPITEM,                  ArcOpItem::Unmarshalling },
    { SAVE_OPITEM,                 SaveOpItem::Unmarshalling },
    { RESTORE_OPITEM,              RestoreOpItem::Unmarshalling },
    { FLUSH_OPITEM,                FlushOpItem::Unmarshalling },
    { MATRIX_OPITEM,               MatrixOpItem::Unmarshalling },
    { CLIP_RECT_OPITEM,            ClipRectOpItem::Unmarshalling },
    { CLIP_RRECT_OPITEM,           ClipRRectOpItem::Unmarshalling },
    { CLIP_REGION_OPITEM,          ClipRegionOpItem::Unmarshalling },
    { TRANSLATE_OPITEM,            TranslateOpItem::Unmarshalling },
    { TEXTBLOB_OPITEM,             TextBlobOpItem::Unmarshalling },
    { BITMAP_OPITEM,               BitmapOpItem::Unmarshalling },
    { COLOR_FILTER_BITMAP_OPITEM,  ColorFilterBitmapOpItem::Unmarshalling },
    { BITMAP_RECT_OPITEM,          BitmapRectOpItem::Unmarshalling },
    { BITMAP_NINE_OPITEM,          BitmapNineOpItem::Unmarshalling },
    { PIXELMAP_OPITEM,             PixelMapOpItem::Unmarshalling },
    { PIXELMAP_RECT_OPITEM,        PixelMapRectOpItem::Unmarshalling },
    { ADAPTIVE_RRECT_OPITEM,       AdaptiveRRectOpItem::Unmarshalling },
    { ADAPTIVE_RRECT_SCALE_OPITEM, AdaptiveRRectScaleOpItem::Unmarshalling },
    { CLIP_ADAPTIVE_RRECT_OPITEM,  ClipAdaptiveRRectOpItem::Unmarshalling },
    { CLIP_OUTSET_RECT_OPITEM,     ClipOutsetRectOpItem::Unmarshalling },
    { PATH_OPITEM,                 PathOpItem::Unmarshalling },
    { CLIP_PATH_OPITEM,            ClipPathOpItem::Unmarshalling },
    { PAINT_OPITEM,                PaintOpItem::Unmarshalling },
    { CONCAT_OPITEM,               ConcatOpItem::Unmarshalling },
    { SAVE_LAYER_OPITEM,           SaveLayerOpItem::Unmarshalling },
    { DRAWABLE_OPITEM,             DrawableOpItem::Unmarshalling },
    { PICTURE_OPITEM,              PictureOpItem::Unmarshalling },
    { POINTS_OPITEM,               PointsOpItem::Unmarshalling },
    { VERTICES_OPITEM,             VerticesOpItem::Unmarshalling },
    { SHADOW_REC_OPITEM,           ShadowRecOpItem::Unmarshalling },
    { MULTIPLY_ALPHA_OPITEM,       MultiplyAlphaOpItem::Unmarshalling },
    { SAVE_ALPHA_OPITEM,           SaveAlphaOpItem::Unmarshalling },
    { RESTORE_ALPHA_OPITEM,        RestoreAlphaOpItem::Unmarshalling },
};

#ifdef ROSEN_OHOS
OpUnmarshallingFunc DrawCmdList::GetOpUnmarshallingFunc(RSOpType type)
{
    auto it = opUnmarshallingFuncLUT.find(type);
    if (it == opUnmarshallingFuncLUT.end()) {
        return nullptr;
    }
    return it->second;
}
#endif

DrawCmdList::DrawCmdList(int w, int h) : width_(w), height_(h) {}

DrawCmdList::~DrawCmdList()
{
    ClearOp();
}

void DrawCmdList::AddOp(std::unique_ptr<OpItem>&& op)
{
    std::lock_guard<std::mutex> lock(mutex_);
    ops_.push_back(std::move(op));
}

void DrawCmdList::ClearOp()
{
    std::lock_guard<std::mutex> lock(mutex_);
    ops_.clear();
}

DrawCmdList& DrawCmdList::operator=(DrawCmdList&& that)
{
    std::lock_guard<std::mutex> lock(mutex_);
    ops_.swap(that.ops_);
    return *this;
}

void DrawCmdList::Playback(SkCanvas& canvas, const SkRect* rect)
{
    RSPaintFilterCanvas filterCanvas(&canvas);
    Playback(filterCanvas, rect);
}

void DrawCmdList::Playback(RSPaintFilterCanvas& canvas, const SkRect* rect)
{
    if (width_ <= 0 || height_ <= 0) {
        return;
    }
    std::lock_guard<std::mutex> lock(mutex_);
#ifdef ROSEN_OHOS
    // invalidate cache if high contrast flag changed
    if (isCached_ && canvas.isHighContrastEnabled() != cachedHighContrast_) {
        ClearCache();
    }
    // Generate or clear cache if cache state changed.
    if (canvas.GetCacheType() == RSPaintFilterCanvas::CacheType::ENABLED && !isCached_) {
        GenerateCache(&canvas, rect);
    } else if (canvas.GetCacheType() == RSPaintFilterCanvas::CacheType::DISABLED && isCached_) {
        ClearCache();
    }
#endif
    for (auto& it : ops_) {
        if (it == nullptr) {
            continue;
        }
        it->Draw(canvas, rect);
    }
}

size_t DrawCmdList::GetSize() const
{
    return ops_.size();
}

int DrawCmdList::GetWidth() const
{
    return width_;
}

int DrawCmdList::GetHeight() const
{
    return height_;
}

bool DrawCmdList::Marshalling(Parcel& parcel) const
{
#ifdef ROSEN_OHOS
    std::lock_guard<std::mutex> lock(mutex_);
    bool success = RSMarshallingHelper::Marshalling(parcel, width_) &&
                   RSMarshallingHelper::Marshalling(parcel, height_) &&
                   RSMarshallingHelper::Marshalling(parcel, ops_) &&
                   RSMarshallingHelper::Marshalling(parcel, isCached_) &&
                   RSMarshallingHelper::Marshalling(parcel, cachedHighContrast_) &&
                   RSMarshallingHelper::Marshalling(parcel, opReplacedByCache_);
    if (!success) {
        ROSEN_LOGE("DrawCmdList::Marshalling failed!");
        return false;
    }
    return success;
#else
    return true;
#endif
}

DrawCmdList* DrawCmdList::Unmarshalling(Parcel& parcel)
{
#ifdef ROSEN_OHOS
    int width;
    int height;
    if (!(RSMarshallingHelper::Unmarshalling(parcel, width) &&
            RSMarshallingHelper::Unmarshalling(parcel, height))) {
        ROSEN_LOGE("DrawCmdList::Unmarshalling width&height failed!");
        return nullptr;
    }
    std::unique_ptr<DrawCmdList> drawCmdList = std::make_unique<DrawCmdList>(width, height);
    if (!(RSMarshallingHelper::Unmarshalling(parcel, drawCmdList->ops_) &&
            RSMarshallingHelper::Unmarshalling(parcel, drawCmdList->isCached_) &&
            RSMarshallingHelper::Unmarshalling(parcel, drawCmdList->cachedHighContrast_) &&
            RSMarshallingHelper::Unmarshalling(parcel, drawCmdList->opReplacedByCache_))) {
        ROSEN_LOGE("DrawCmdList::Unmarshalling contents failed!");
        return nullptr;
    }
    return drawCmdList.release();
#else
    return nullptr;
#endif
}

void DrawCmdList::GenerateCache(const RSPaintFilterCanvas* canvas, const SkRect* rect)
{
#ifdef ROSEN_OHOS
    RS_TRACE_FUNC();
    for (auto index = 0u; index < ops_.size(); index++) {
        auto& op = ops_[index];
        if (op == nullptr) {
            continue;
        }
        if (auto cached_op = op->GenerateCachedOpItem(canvas, rect)) {
            // backup the original op and position
            opReplacedByCache_.emplace_back(index, op.release());
            // replace the original op with the cached op
            op.reset(cached_op.release());
        }
    }
    isCached_ = true;
    cachedHighContrast_ = canvas && canvas->isHighContrastEnabled();
#endif
}

void DrawCmdList::ClearCache()
{
#ifdef ROSEN_OHOS
    RS_TRACE_FUNC();
    // restore the original op
    for (auto& [index, op] : opReplacedByCache_) {
        ops_[index].reset(op.release());
    }
    opReplacedByCache_.clear();
    isCached_ = false;
#endif
}

#if defined(RS_ENABLE_DRIVEN_RENDER) && defined(RS_ENABLE_GL)
void DrawCmdList::CheckClipRect(SkRect& rect)
{
    std::lock_guard<std::mutex> lock(mutex_);
    for (const auto& it : ops_) {
        if (it == nullptr) {
            continue;
        }
        if (it->GetType() == CLIP_RECT_OPITEM) {
            ClipRectOpItem* clipRectOpItem = static_cast<ClipRectOpItem*>(it.get());
            auto optionalRect = clipRectOpItem->GetClipRect();
            if (optionalRect.has_value() && !optionalRect.value().isEmpty()) {
                rect = optionalRect.value();
            }
            break;
        }
    }
}

void DrawCmdList::ReplaceDrivenCmds()
{
    RS_TRACE_FUNC();
    std::lock_guard<std::mutex> lock(mutex_);

    for (auto index = 0u; index < ops_.size(); index++) {
        auto& op = ops_[index];
        if (op != nullptr && op->GetType() == CLIP_RECT_OPITEM) {
            // backup the original op and position, replace the original op with nullptr
            opReplacedByDrivenRender_.emplace(index, op.release());
            break;
        }
    }
}

void DrawCmdList::RestoreOriginCmdsForDriven()
{
    RS_TRACE_FUNC();
    std::lock_guard<std::mutex> lock(mutex_);

    // restore the original op
    for (auto& it : opReplacedByDrivenRender_) {
        ops_[it.first] = std::move(it.second);
    }
    opReplacedByDrivenRender_.clear();
}
#endif
} // namespace Rosen
} // namespace OHOS
