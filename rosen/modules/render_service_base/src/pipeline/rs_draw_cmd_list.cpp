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

namespace OHOS {
namespace Rosen {
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

bool DrawCmdList::Marshalling(Parcel& parcel) const
{
    bool success = true;
    success &= RSMarshallingHelper::Marshalling(parcel, width_);
    success &= RSMarshallingHelper::Marshalling(parcel, height_);
    success &= RSMarshallingHelper::Marshalling(parcel, GetSize());
    ROSEN_LOGE("unirender: DrawCmdList::Marshalling start, sussess = %d", success);
    for (const auto& item : ops_) {
        auto type = item->GetType();
        bool result = false;
        switch (type) {
            case RSOpType::TEXTBLOBOPITEM :
            case RSOpType::SAVEALPHAOPITEM :
            case RSOpType::RESTOREALPHAOPITEM :
            case RSOpType::SAVEOPITEM :
            case RSOpType::RESTOREOPITEM :
            case RSOpType::MULTIPLYALPHAOPITEM :
                ROSEN_LOGE("unirender: opItem Marshalling, optype = %d", type);
                result = item->Marshalling(parcel);
                success &= result;
                ROSEN_LOGE("unirender: opItem Marshalling end, optype = %d, sussess = %d", type, result);
                break;
            default :
                ROSEN_LOGE("unirender: opItem no Marshalling, optype = %d", type);
        }
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
    
    DrawCmdList* drawCmdList = new DrawCmdList(width, height);
    for (int i = 0; i < size; ++i) {
        RSOpType type;
        if (!RSMarshallingHelper::Unmarshalling(parcel, type)) {
            return nullptr;
        }
        OpItem* item = nullptr;
        switch (type) {
            case RSOpType::TEXTBLOBOPITEM :
                item = TextBlobOpItem::Unmarshalling(parcel);
                break;
            case RSOpType::SAVEALPHAOPITEM : 
                item = SaveAlphaOpItem::Unmarshalling(parcel);
                break;
            case RSOpType::RESTOREALPHAOPITEM : 
                item = RestoreAlphaOpItem::Unmarshalling(parcel);
                break;
            case RSOpType::SAVEOPITEM :
                item = SaveOpItem::Unmarshalling(parcel);
                break;
            case RSOpType::RESTOREOPITEM :
                item = RestoreOpItem::Unmarshalling(parcel);
                break;
            case RSOpType::MULTIPLYALPHAOPITEM :
                item = MultiplyAlphaOpItem::Unmarshalling(parcel);
                break;
            default :
                item = nullptr;
                ROSEN_LOGE("unirender: no Unmarshalling func, RSOpType = %d", type);
        }
        if (item) {
            drawCmdList->AddOp(std::unique_ptr<OpItem>(item));
        }
    }
    return drawCmdList;
}

} // namespace Rosen
} // namespace OHOS
