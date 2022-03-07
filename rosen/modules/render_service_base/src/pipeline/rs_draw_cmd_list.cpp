/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
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

#include "platform/common/rs_log.h"
#include "pipeline/rs_draw_cmd.h"
#include "pipeline/rs_paint_filter_canvas.h"
#include "transaction/rs_marshalling_helper.h"
#include <unordered_map>

namespace OHOS {
namespace Rosen {

using OpUnmarshallingFunc = OpItem* (*)(Parcel& parcel);

static std::unordered_map<RSOpType, OpUnmarshallingFunc> opUnmarshallingFuncLUT = {
    { RECTOPITEM,               RectOpItem::Unmarshalling },
    { ROUNDRECTOPITEM,          RoundRectOpItem::Unmarshalling },
    { IMAGEWITHPARMOPITEM,      ImageWithParmOpItem::Unmarshalling },
    { DRRECTOPITEM,             DRRectOpItem::Unmarshalling },
    { OVALOPITEM,               OvalOpItem::Unmarshalling },
    { REGIONOPITEM,             RegionOpItem::Unmarshalling },
    { ARCOPITEM,                ArcOpItem::Unmarshalling },
    { SAVEOPITEM,               SaveOpItem::Unmarshalling },
    { RESTOREOPITEM,            RestoreOpItem::Unmarshalling },
    { FLUSHOPITEM,              FlushOpItem::Unmarshalling },
    { MATRIXOPITEM,             MatrixOpItem::Unmarshalling },
    { CLIPRECTOPITEM,           ClipRectOpItem::Unmarshalling },
    { CLIPRRECTOPITEM,          ClipRRectOpItem::Unmarshalling },
    { CLIPREGIONOPITEM,         ClipRegionOpItem::Unmarshalling },
    { TRANSLATEOPITEM,          TranslateOpItem::Unmarshalling },
    { TEXTBLOBOPITEM,           TextBlobOpItem::Unmarshalling },
    { BITMAPOPITEM,             BitmapOpItem::Unmarshalling },
    { BITMAPRECTOPITEM,         BitmapRectOpItem::Unmarshalling },
    // { BITMAPLATTICEOPITEM,      BitmapLatticeOpItem::Unmarshalling },
    { BITMAPNINEOPITEM,         BitmapNineOpItem::Unmarshalling },
    { ADAPTIVERRECTOPITEM,      AdaptiveRRectOpItem::Unmarshalling },
    { CLIPADAPTIVERRECTOPITEM,  ClipAdaptiveRRectOpItem::Unmarshalling },
    { PATHOPITEM,               PathOpItem::Unmarshalling },
    { CLIPPATHOPITEM,           ClipPathOpItem::Unmarshalling },
    { PAINTOPITEM,              PaintOpItem::Unmarshalling },
    { CONCATOPITEM,             ConcatOpItem::Unmarshalling },
    { SAVELAYEROPITEM,          SaveLayerOpItem::Unmarshalling },
    { DRAWABLEOPITEM,           DrawableOpItem::Unmarshalling },
    { PICTUREOPITEM,            PictureOpItem::Unmarshalling },
    { POINTSOPITEM,             PointsOpItem::Unmarshalling },
    { VERTICESOPITEM,           VerticesOpItem::Unmarshalling },
    { MULTIPLYALPHAOPITEM,      MultiplyAlphaOpItem::Unmarshalling },
    { SAVEALPHAOPITEM,          SaveAlphaOpItem::Unmarshalling },
    { RESTOREALPHAOPITEM,       RestoreAlphaOpItem::Unmarshalling },
};

static OpUnmarshallingFunc GetOpUnmarshallingFunc(RSOpType type) {
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
    std::lock_guard<std::recursive_mutex> lock(mutex_);
    ops_.push_back(std::move(op));
}

void DrawCmdList::ClearOp()
{
    ops_.clear();
}

DrawCmdList& DrawCmdList::operator=(DrawCmdList&& that)
{
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
#ifdef ROSEN_OHOS
    if (width_ <= 0 || height_ <= 0) {
        return;
    }
    for (auto& it : ops_) {
        if (it == nullptr) {
            continue;
        }
        it->Draw(canvas, rect);
    }
#endif
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

#ifdef ROSEN_OHOS
bool DrawCmdList::Marshalling(Parcel& parcel) const
{
    bool success = true;
    success &= RSMarshallingHelper::Marshalling(parcel, width_);
    success &= RSMarshallingHelper::Marshalling(parcel, height_);
    success &= RSMarshallingHelper::Marshalling(parcel, GetSize());
    ROSEN_LOGE("unirender: DrawCmdList::Marshalling start, size = %d", GetSize());
    for (const auto& item : ops_) {
        auto type = item->GetType();
        success &= RSMarshallingHelper::Marshalling(parcel, type);
        bool result = item->Marshalling(parcel);
        success &= result;
        ROSEN_LOGE("unirender: opItem Marshalling, optype = %d, result = %d, funcDefine = %d",
                   type, result, GetOpUnmarshallingFunc(type) != nullptr);
    }
    ROSEN_LOGE("unirender: DrawCmdList::Marshalling end, sussess = %d", success);
    return success;
}

DrawCmdList* DrawCmdList::Unmarshalling(Parcel& parcel)
{
    int width;
    int height;
    int size;
    if (!RSMarshallingHelper::Unmarshalling(parcel, width)) {
        return nullptr;
    }
    if (!RSMarshallingHelper::Unmarshalling(parcel, height)) {
        return nullptr;
    }
    if (!RSMarshallingHelper::Unmarshalling(parcel, size)) {
        return nullptr;
    }

    ROSEN_LOGE("unirender: DrawCmdList::Unmarshalling start");
    DrawCmdList* drawCmdList = new DrawCmdList(width, height);
    for (int i = 0; i < size; ++i) {
        RSOpType type;
        if (!RSMarshallingHelper::Unmarshalling(parcel, type)) {
            return nullptr;
        }
        auto func = GetOpUnmarshallingFunc(type);
        if (!func) {
            ROSEN_LOGE("unirender: opItem Unmarshalling no funcDefine, optype = %d", type);
            continue;
        }
        OpItem* item = (*func)(parcel);
        if (item) {
            drawCmdList->AddOp(std::unique_ptr<OpItem>(item));
        }
        ROSEN_LOGE("unirender: opItem Unmarshalling, optype = %d, result = %d", type, item != nullptr);
    }
    ROSEN_LOGE("unirender: DrawCmdList::Unmarshalling finish.");
    return drawCmdList;
}
#endif

} // namespace Rosen
} // namespace OHOS
