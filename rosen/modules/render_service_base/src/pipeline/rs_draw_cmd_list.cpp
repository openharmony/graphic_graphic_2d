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

#ifndef USE_ROSEN_DRAWING
#include "pipeline/rs_draw_cmd_list.h"

#include <fstream>
#include <string>
#ifdef ROSEN_OHOS
#include <sys/mman.h>
#endif
#include <unordered_map>

#include "rs_trace.h"

#include "pipeline/rs_draw_cmd.h"
#include "pipeline/rs_paint_filter_canvas.h"
#include "platform/common/rs_log.h"
#include "transaction/rs_ashmem_helper.h"
#include "transaction/rs_marshalling_helper.h"

#ifdef ROSEN_OHOS
namespace {
constexpr size_t ASHMEMALLOCATOR_SIZE = 10 * 1024 * 1024; // 10M
constexpr size_t FDCOUNT_LIMIT = 100;
constexpr size_t OPSIZE_LIMIT = 100000;
}
#endif
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
    { HM_SYMBOL_OPITEM,            SymbolOpItem::Unmarshalling },
    { BITMAP_OPITEM,               BitmapOpItem::Unmarshalling },
    { COLOR_FILTER_BITMAP_OPITEM,  ColorFilterBitmapOpItem::Unmarshalling },
    { BITMAP_RECT_OPITEM,          BitmapRectOpItem::Unmarshalling },
    { BITMAP_NINE_OPITEM,          BitmapNineOpItem::Unmarshalling },
    { PIXELMAP_NINE_OPITEM,        PixelmapNineOpItem::Unmarshalling },
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
#ifdef ROSEN_OHOS
    { SURFACEBUFFER_OPITEM,        SurfaceBufferOpItem::Unmarshalling },
#endif
    { SCALE_OPITEM,                ScaleOpItem::Unmarshalling },
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
            RS_LOGE("DrawCmdList::Playback ops is null");
            continue;
        }
        if (it->GetType() == RSOpType::HM_SYMBOL_OPITEM) {
            it->SetSymbol();
        }
        it->Draw(canvas, rect);
    }
}

std::string DrawCmdList::PlayBackForRecord(SkCanvas& canvas, int startOpId, int endOpId, int descStartOpId,
    const SkRect* rect)
{
    RSPaintFilterCanvas filterCanvas(&canvas);
    return PlayBackForRecord(filterCanvas, startOpId, endOpId, descStartOpId, rect);
}

std::string DrawCmdList::PlayBackForRecord(RSPaintFilterCanvas& canvas, int startOpId, int endOpId,
    int descStartOpId, const SkRect* rect)
{
    std::string str;
    if (width_ <= 0 || height_ <= 0) {
        return str;
    }
    std::lock_guard<std::mutex> lock(mutex_);
    for (int i = startOpId; i < endOpId; ++i) {
        if (ops_[i] == nullptr) {
            continue;
        }
        if (i < descStartOpId) {
            ops_[i]->GetTypeWithDesc();
        } else {
            str += std::to_string(i) + ":";
            str += ops_[i]->GetTypeWithDesc();
        }
        ops_[i]->Draw(canvas, rect);
    }
    return str;
}

void DrawCmdList::SetWidth(int width)
{
    width_ = width;
}

void DrawCmdList::SetHeight(int height)
{
    height_ = height;
}

std::string DrawCmdList::GetOpsWithDesc() const
{
    std::string desc;
    for (auto& item : ops_) {
        if (item == nullptr) {
            continue;
        }
        desc += item->GetTypeWithDesc();
    }
    return desc + "\n";
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

void DrawCmdList::UpdateNodeIdToPicture(NodeId nodeId)
{
#ifdef ROSEN_OHOS
    if (imageIndexs_.empty()) {
        RS_LOGD("DrawCmdList::UpdateNodeIdToPicture no need update");
        return;
    }
    for (size_t i = 0; i < imageIndexs_.size(); i++) {
        auto index = imageIndexs_[i];
        if (index > ops_.size() - 1) {
            RS_LOGW("DrawCmdList::UpdateNodeIdToPicture index[%{public}d] error", index);
            continue;
        }
        ops_[index]->SetNodeId(nodeId);
    }
#endif
}

void DrawCmdList::DumpPicture(DfxString& info) const
{
    if (imageIndexs_.empty()) {
        RS_LOGW("DrawCmdList::DumpPicture no need update");
        return;
    }
    info.AppendFormat("Resources:%d\n", imageIndexs_.size());
    info.AppendFormat("Size  [width * height]    Addr\n");
    for (size_t i = 0; i < imageIndexs_.size(); i++) {
        auto index = imageIndexs_[i];
        if (index > ops_.size()) {
            RS_LOGW("DrawCmdList::DumpPicture index[%{public}d] error", index);
            continue;
        }
        ops_[index]->DumpPicture(info);
    }
}

void DrawCmdList::FindIndexOfImage() const
{
    for (size_t index = 0; index < ops_.size(); index++) {
        if (ops_[index]->IsImageOp()) {
            imageIndexs_.emplace_back(index);
        }
    }
}

bool DrawCmdList::Marshalling(Parcel& parcel) const
{
#ifdef ROSEN_OHOS
    std::lock_guard<std::mutex> lock(mutex_);
    FindIndexOfImage();
    bool success = RSMarshallingHelper::Marshalling(parcel, width_) &&
                   RSMarshallingHelper::Marshalling(parcel, height_) &&
                   RSMarshallingHelper::Marshalling(parcel, isCached_) &&
                   RSMarshallingHelper::Marshalling(parcel, cachedHighContrast_) &&
                   RSMarshallingHelper::Marshalling(parcel, opReplacedByCache_) &&
                   RSMarshallingHelper::Marshalling(parcel, imageIndexs_);
    if (!success) {
        ROSEN_LOGE("DrawCmdList::Marshalling failed!");
        return false;
    }

    if (ops_.size() > 1000 && RSMarshallingHelper::GetUseSharedMem(std::this_thread::get_id())) {  // OPsize > 1000
        parcel.WriteUint32(1); // 1: use shared mem
        auto position = parcel.GetWritePosition();
        parcel.WriteUint32(0); // shmem count
        parcel.WriteUint32(ops_.size());
        uint32_t index = 0;
        std::unique_ptr<AshmemAllocator> ashmemAllocator;
        std::shared_ptr<MessageParcel> newParcel;
        uint32_t fdCount = 0;
        uint32_t lastIndex = 0;
        while (index < ops_.size()) {
            if (newParcel == nullptr || newParcel->GetWritableBytes() < ASHMEMALLOCATOR_SIZE / 20) { // less than 5%
                if (index - lastIndex != 0) {
                    uint32_t size = index - lastIndex;
                    parcel.WriteUint32(size);
                }
                if (newParcel != nullptr) {
                    uint32_t offsetSize = newParcel->GetOffsetsSize();
                    parcel.WriteInt32(offsetSize);
                    if (offsetSize > 0) {
                        parcel.WriteBuffer(
                            reinterpret_cast<void*>(newParcel->GetObjectOffsets()), sizeof(binder_size_t) * offsetSize);
                        RSAshmemHelper::CopyFileDescriptor(static_cast<MessageParcel*>(&parcel), newParcel);
                    }
                }
                ashmemAllocator = AshmemAllocator::CreateAshmemAllocator(ASHMEMALLOCATOR_SIZE, PROT_READ | PROT_WRITE);
                if (!ashmemAllocator) {
                    RS_LOGE("RSMarshallingHelper::WriteToParcel CreateAshmemAllocator fail");
                    return false;
                }

                int fd = ashmemAllocator->GetFd();
                if (!(static_cast<MessageParcel*>(&parcel)->WriteFileDescriptor(fd))) {
                    RS_LOGE("RSMarshallingHelper::WriteToParcel WriteFileDescriptor fail");
                    return false;
                }
                lastIndex = index;
                newParcel = std::make_shared<MessageParcel>(ashmemAllocator.release());
                newParcel->SetDataCapacity(ASHMEMALLOCATOR_SIZE);
                ++fdCount;
            }
            if (!RSMarshallingHelper::Marshalling(*newParcel, ops_[index])) {
                RS_LOGE("RSMarshallingHelper::Marshalling, marshalling ops failed");
            }
            ++index;
        }
        parcel.WriteUint32(index - lastIndex);
        if (newParcel != nullptr) {
            uint32_t offsetSize = newParcel->GetOffsetsSize();
            parcel.WriteInt32(offsetSize);
            if (offsetSize > 0) {
                parcel.WriteBuffer(
                    reinterpret_cast<void*>(newParcel->GetObjectOffsets()), sizeof(binder_size_t) * offsetSize);
                RSAshmemHelper::CopyFileDescriptor(static_cast<MessageParcel*>(&parcel), newParcel);
            }
        }
        *reinterpret_cast<uint32_t*>(parcel.GetData() + position) = fdCount;
    } else {
        parcel.WriteUint32(0); // 0: not use shared mem
        success = RSMarshallingHelper::Marshalling(parcel, ops_);
    }
    if (!success) {
        RS_LOGE("DrawCmdList::Marshalling failed");
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
    if (!(RSMarshallingHelper::Unmarshalling(parcel, width) && RSMarshallingHelper::Unmarshalling(parcel, height))) {
        ROSEN_LOGE("DrawCmdList::Unmarshalling width&height failed!");
        return nullptr;
    }
    std::unique_ptr<DrawCmdList> drawCmdList = std::make_unique<DrawCmdList>(width, height);
    if (!(RSMarshallingHelper::Unmarshalling(parcel, drawCmdList->isCached_) &&
            RSMarshallingHelper::Unmarshalling(parcel, drawCmdList->cachedHighContrast_) &&
            RSMarshallingHelper::Unmarshalling(parcel, drawCmdList->opReplacedByCache_) &&
            RSMarshallingHelper::Unmarshalling(parcel, drawCmdList->imageIndexs_))) {
        ROSEN_LOGE("DrawCmdList::Unmarshalling contents failed!");
        return nullptr;
    }
    if (parcel.ReadUint32() == 0) {
        if (!RSMarshallingHelper::Unmarshalling(parcel, drawCmdList->ops_)) {
            return nullptr;
        }
    } else {
        auto fdCount = parcel.ReadUint32();
        auto totalOpSize = parcel.ReadUint32();
        if (fdCount > FDCOUNT_LIMIT || totalOpSize > OPSIZE_LIMIT) {
            ROSEN_LOGE("DrawCmdList::Unmarshalling FdCount or TotalOpSize is too large");
            return nullptr;
        }
        drawCmdList->ops_.resize(totalOpSize);
        uint32_t index = 0;
        for (uint32_t i = 0; i < fdCount; ++i) {
            int fd = static_cast<MessageParcel*>(&parcel)->ReadFileDescriptor();
            uint32_t opSize = parcel.ReadUint32();
            if (opSize > OPSIZE_LIMIT) {
                ROSEN_LOGE("DrawCmdList::Unmarshalling OpSize is too large");
                return nullptr;
            }
            auto ashmemAllocator =
                AshmemAllocator::CreateAshmemAllocatorWithFd(fd, ASHMEMALLOCATOR_SIZE, PROT_READ | PROT_WRITE);
            if (!ashmemAllocator) {
                return nullptr;
            }

            void* data = ashmemAllocator->GetData();
            auto dataSize = ashmemAllocator->GetSize();
            auto newParcel = std::make_shared<MessageParcel>(ashmemAllocator.release());
            newParcel->ParseFrom(reinterpret_cast<uintptr_t>(data), dataSize);

            int32_t offsetSize = parcel.ReadInt32();
            if (offsetSize > 0) {
                auto* offsets = parcel.ReadBuffer(sizeof(binder_size_t) * offsetSize);
                if (offsets == nullptr) {
                    ROSEN_LOGE("ParseFromAshmemParcel: read object offsets failed");
                    return nullptr;
                }
                newParcel->InjectOffsets(reinterpret_cast<binder_size_t>(offsets), offsetSize);
                RSAshmemHelper::InjectFileDescriptor(newParcel, static_cast<MessageParcel*>(&parcel));
            }

            for (uint32_t j = 0; j < opSize; ++j) {
                RSMarshallingHelper::Unmarshalling(*newParcel, drawCmdList->ops_[index]);
                ++index;
            }

            ashmemAllocator = nullptr;
            newParcel = nullptr;
        }
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
} // namespace Rosen
} // namespace OHOS
#endif // USE_ROSEN_DRAWING
