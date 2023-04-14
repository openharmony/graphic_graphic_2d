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
#include "platform/common/rs_system_properties.h"
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

static OpUnmarshallingFunc GetOpUnmarshallingFunc(RSOpType type)
{
    auto it = opUnmarshallingFuncLUT.find(type);
    if (it == opUnmarshallingFuncLUT.end()) {
        return nullptr;
    }
    return it->second;
}

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

void DrawCmdList::Playback(SkCanvas& canvas, const SkRect* rect) const
{
    RSPaintFilterCanvas filterCanvas(&canvas);
    Playback(filterCanvas, rect);
}

void DrawCmdList::Playback(RSPaintFilterCanvas& canvas, const SkRect* rect) const
{
    if (width_ <= 0 || height_ <= 0) {
        return;
    }
    std::lock_guard<std::mutex> lock(mutex_);
    for (auto& it : ops_) {
        if (it == nullptr) {
            continue;
        }
        it->Draw(canvas, rect);
    }
}

int DrawCmdList::GetSize() const
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
    std::lock_guard<std::mutex> lock(mutex_);
    bool success = RSMarshallingHelper::Marshalling(parcel, width_) &&
                   RSMarshallingHelper::Marshalling(parcel, height_) &&
                   RSMarshallingHelper::Marshalling(parcel, GetSize());
    if (!success) {
        ROSEN_LOGE("DrawCmdList::Marshalling failed!");
        return false;
    }
    for (const auto& item : ops_) {
        auto type = item->GetType();
        success = success && RSMarshallingHelper::Marshalling(parcel, type);
        auto func = GetOpUnmarshallingFunc(type);
        if (!func) {
            ROSEN_LOGW("unirender: opItem Unmarshalling func not define, skip Marshalling, optype = %d", type);
            continue;
        }

        success = success && item->Marshalling(parcel);
        if (!success) {
            ROSEN_LOGE("unirender: failed opItem Marshalling, optype = %d", type);
            return success;
        }
    }
    return success;
}

DrawCmdList* DrawCmdList::Unmarshalling(Parcel& parcel)
{
    int width;
    int height;
    int size;
    if (!(RSMarshallingHelper::Unmarshalling(parcel, width) &&
            RSMarshallingHelper::Unmarshalling(parcel, height) &&
            RSMarshallingHelper::Unmarshalling(parcel, size))) {
        ROSEN_LOGE("DrawCmdList::Unmarshalling failed!");
        return nullptr;
    }
    std::unique_ptr<DrawCmdList> drawCmdList = std::make_unique<DrawCmdList>(width, height);
    for (int i = 0; i < size; ++i) {
        RSOpType type;
        if (!RSMarshallingHelper::Unmarshalling(parcel, type)) {
            ROSEN_LOGE("DrawCmdList::Unmarshalling failed, current processing:%d", i);
            return nullptr;
        }
        auto func = GetOpUnmarshallingFunc(type);
        if (!func) {
            ROSEN_LOGW("unirender: opItem Unmarshalling func not define, optype = %d", type);
            continue;
        }

        OpItem* item = (*func)(parcel);
        if (!item) {
            ROSEN_LOGE("unirender: failed opItem Unmarshalling, optype = %d", type);
            return nullptr;
        }

        drawCmdList->AddOp(std::unique_ptr<OpItem>(item));
    }
    return drawCmdList.release();
}

void DrawCmdList::GenerateCache(SkSurface* surface)
{
#ifdef ROSEN_OHOS
    if (isCached_) {
        return;
    }
    isCached_ = true;
    RS_TRACE_FUNC();
    std::lock_guard<std::mutex> lock(mutex_);

    for (auto index = 0u; index < ops_.size(); index++) {
        auto& op = ops_[index];
        if (auto cached_op = op->GenerateCachedOpItem(surface)) {
            // backup the original op and position
            opReplacedByCache_.emplace(index, op.release());
            // replace the original op with the cached op
            op.reset(cached_op.release());
        }
    }
#endif
}

void DrawCmdList::ClearCache()
{
#ifdef ROSEN_OHOS
    if (!isCached_) {
        return;
    }
    isCached_ = false;
    RS_TRACE_FUNC();
    std::lock_guard<std::mutex> lock(mutex_);

    // restore the original op
    for (auto& it : opReplacedByCache_) {
        ops_[it.first] = std::move(it.second);
    }
    opReplacedByCache_.clear();
#endif
}

// modify the DrawCmdListManager instance to global to extend life cycle, fix destructor crash
static DrawCmdListManager gDrawCmdListManagerInstance;

DrawCmdListManager& DrawCmdListManager::Instance()
{
    return gDrawCmdListManagerInstance;
}

DrawCmdListManager::~DrawCmdListManager()
{
    std::lock_guard<std::mutex> lock(listsMutex_);
    lists_.clear();
}

void DrawCmdListManager::RegisterDrawCmdList(NodeId id, std::shared_ptr<DrawCmdList> drawCmdList)
{
    std::lock_guard<std::mutex> lock(listsMutex_);
    static bool uniEnabled = RSSystemProperties::GetUniRenderEnabled();
    if (uniEnabled && drawCmdList) {
        lists_[id].emplace_back(drawCmdList);
    }
}

void DrawCmdListManager::ClearDrawCmdList(NodeId id)
{
    std::lock_guard<std::mutex> lock(listsMutex_);
    auto iterator = lists_.find(id);
    if (iterator == lists_.end()) {
        return;
    }
    if (forceClear_) {
        for (auto& weakPtr : iterator->second) {
            if (auto ptr = weakPtr.lock()) {
                ptr->ClearOp();
            }
        }
    }
    lists_.erase(iterator);
}

void DrawCmdListManager::MarkForceClear(bool flag)
{
    forceClear_ = flag;
}
} // namespace Rosen
} // namespace OHOS
