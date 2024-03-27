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

#include "recording/draw_cmd_list.h"

#include <cstddef>
#include <memory>

#include "recording/draw_cmd.h"
#include "recording/recording_canvas.h"
#include "utils/log.h"

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
    { DrawOpItem::COLOR_OPITEM,             "COLOR_OPITEM" },
    { DrawOpItem::IMAGE_NINE_OPITEM,        "IMAGE_NINE_OPITEM" },
    { DrawOpItem::IMAGE_LATTICE_OPITEM,     "IMAGE_LATTICE_OPITEM" },
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
    { DrawOpItem::REGION_OPITEM,            "REGION_OPITEM" },
    { DrawOpItem::PATCH_OPITEM,             "PATCH_OPITEM" },
    { DrawOpItem::EDGEAAQUAD_OPITEM,        "EDGEAAQUAD_OPITEM" },
    { DrawOpItem::VERTICES_OPITEM,          "VERTICES_OPITEM" },
    { DrawOpItem::IMAGE_SNAPSHOT_OPITEM,    "IMAGE_SNAPSHOT_OPITEM" },
    { DrawOpItem::SURFACEBUFFER_OPITEM,     "SURFACEBUFFER_OPITEM"},
    { DrawOpItem::DRAW_FUNC_OPITEM,         "DRAW_FUNC_OPITEM"},
};

std::shared_ptr<DrawCmdList> DrawCmdList::CreateFromData(const CmdListData& data, bool isCopy)
{
    auto cmdList = std::make_shared<DrawCmdList>(DrawCmdList::UnmarshalMode::DEFERRED);
    if (isCopy) {
        cmdList->opAllocator_.BuildFromDataWithCopy(data.first, data.second);
    } else {
        cmdList->opAllocator_.BuildFromData(data.first, data.second);
    }

    int32_t* width = static_cast<int32_t*>(cmdList->opAllocator_.OffsetToAddr(0));
    int32_t* height = static_cast<int32_t*>(cmdList->opAllocator_.OffsetToAddr(sizeof(int32_t)));
    if (width && height) {
        cmdList->width_ = *width;
        cmdList->height_ = *height;
    } else {
        cmdList->width_ = 0;
        cmdList->height_ = 0;
    }
    return cmdList;
}

DrawCmdList::DrawCmdList(DrawCmdList::UnmarshalMode mode) : width_(0), height_(0), mode_(mode) {}

DrawCmdList::DrawCmdList(int32_t width, int32_t height, DrawCmdList::UnmarshalMode mode)
    : width_(width), height_(height), mode_(mode)
{
    opAllocator_.Add(&width_, sizeof(int32_t));
    opAllocator_.Add(&height_, sizeof(int32_t));
}

DrawCmdList::~DrawCmdList()
{
    ClearOp();
}

bool DrawCmdList::AddDrawOp(std::shared_ptr<DrawOpItem>&& drawOpItem)
{
    if (mode_ != DrawCmdList::UnmarshalMode::DEFERRED) {
        return false;
    }
    std::lock_guard<std::recursive_mutex> lock(mutex_);
    drawOpItems_.emplace_back(drawOpItem);
    return true;
}

void DrawCmdList::ClearOp()
{
    std::lock_guard<std::recursive_mutex> lock(mutex_);
    opAllocator_.ClearData();
    opAllocator_.Add(&width_, sizeof(int32_t));
    opAllocator_.Add(&height_, sizeof(int32_t));
    imageAllocator_.ClearData();
    bitmapAllocator_.ClearData();
    imageMap_.clear();
    imageHandleVec_.clear();
    drawOpItems_.clear();
    lastOpGenSize_ = 0;
    lastOpItemOffset_ = std::nullopt;
    opCnt_ = 0;
}

int32_t DrawCmdList::GetWidth() const
{
    return width_;
}

int32_t DrawCmdList::GetHeight() const
{
    return height_;
}

void DrawCmdList::SetWidth(int32_t width)
{
    width_ = width;
}

void DrawCmdList::SetHeight(int32_t height)
{
    height_ = height;
}

bool DrawCmdList::IsEmpty() const
{
    if (mode_ == DrawCmdList::UnmarshalMode::DEFERRED) {
        return drawOpItems_.empty();
    }
    uint32_t offset = 2 * sizeof(int32_t); // 2 is width and height.Offset of first OpItem is behind the w and h
    if (opAllocator_.GetSize() <= offset && drawOpItems_.size() == 0) {
        return true;
    }
    return false;
}

size_t DrawCmdList::GetOpItemSize() const
{
    return mode_ == DrawCmdList::UnmarshalMode::DEFERRED ? drawOpItems_.size() : opCnt_;
}

std::string DrawCmdList::GetOpsWithDesc() const
{
    std::string desc;
    for (auto& item : drawOpItems_) {
        if (item == nullptr) {
            continue;
        }
        desc += typeOpDes[item->GetType()];
        desc += "\n";
    }
    LOGI("DrawCmdList::GetOpsWithDesc %{public}s, opitem sz: %{public}u", desc.c_str(), drawOpItems_.size());
    return desc;
}

void DrawCmdList::MarshallingDrawOps()
{
    if (mode_ == DrawCmdList::UnmarshalMode::IMMEDIATE) {
        return;
    }
    std::lock_guard<std::recursive_mutex> lock(mutex_);
    if (replacedOpListForVector_.empty()) {
        for (auto& op : drawOpItems_) {
            op->Marshalling(*this);
        }
        return;
    }
    for (auto& [index, op] : replacedOpListForVector_) {
        op.swap(drawOpItems_[index]);
    }
    std::vector<uint32_t> opIndexForCache(replacedOpListForVector_.size());
    uint32_t opReplaceIndex = 0;
    for (size_t index = 0; index < drawOpItems_.size(); index++) {
        drawOpItems_[index]->Marshalling(*this);
        if (index == static_cast<size_t>(replacedOpListForVector_[opReplaceIndex].first)) {
            opIndexForCache[opReplaceIndex] = lastOpItemOffset_.value();
            ++opReplaceIndex;
        }
    }
    for (auto index = 0u; index < replacedOpListForVector_.size(); index++) {
        replacedOpListForVector_[index].second->Marshalling(*this);
        replacedOpListForBuffer_.emplace_back(opIndexForCache[index], lastOpItemOffset_.value());
    }
}

void DrawCmdList::UnmarshallingDrawOps()
{
    if (opAllocator_.GetSize() <= offset_) {
        return;
    }

    UnmarshallingPlayer player = { *this };
    drawOpItems_.clear();
    lastOpGenSize_ = 0;
    uint32_t opReplaceIndex = 0;
    uint32_t offset = offset_;
    do {
        void* itemPtr = opAllocator_.OffsetToAddr(offset);
        auto* curOpItemPtr = static_cast<OpItem*>(itemPtr);
        if (curOpItemPtr == nullptr) {
            LOGE("DrawCmdList::UnmarshallingOps failed, opItem is nullptr");
            break;
        }
        uint32_t type = curOpItemPtr->GetType();
        auto op = player.Unmarshalling(type, itemPtr);
        if (!op) {
            offset = curOpItemPtr->GetNextOpItemOffset();
            continue;
        }
        if (opReplaceIndex < replacedOpListForBuffer_.size() &&
            replacedOpListForBuffer_[opReplaceIndex].first == offset) {
            auto* replacePtr = opAllocator_.OffsetToAddr(replacedOpListForBuffer_[opReplaceIndex].second);
            if (replacePtr == nullptr) {
                LOGE("DrawCmdList::Unmarshalling replace Ops failed, replace op is nullptr");
                break;
            }
            auto* replaceOpItemPtr = static_cast<OpItem*>(replacePtr);
            auto replaceOp = player.Unmarshalling(replaceOpItemPtr->GetType(), replacePtr);
            if (replaceOp) {
                drawOpItems_.emplace_back(replaceOp);
                replacedOpListForVector_.emplace_back((drawOpItems_.size() - 1), op);
            } else {
                drawOpItems_.emplace_back(op);
            }
            opReplaceIndex++;
        } else {
            drawOpItems_.emplace_back(op);
        }
        offset = curOpItemPtr->GetNextOpItemOffset();
        if (!replacedOpListForBuffer_.empty() && offset >= replacedOpListForBuffer_[0].second) {
            LOGD("DrawCmdList::UnmarshallingOps seek end by cache textOps");
            break;
        }
    } while (offset != 0);
    lastOpGenSize_ = opAllocator_.GetSize();

    if ((int)imageAllocator_.GetSize() > 0) {
        imageAllocator_.ClearData();
    }
}

void DrawCmdList::Playback(Canvas& canvas, const Rect* rect)
{
    if (width_ <= 0 || height_ <= 0) {
        return;
    }
    if (canvas.GetDrawingType() == DrawingType::RECORDING) {
        PlaybackToDrawCmdList(static_cast<RecordingCanvas&>(canvas).GetDrawCmdList());
        return;
    }
    std::lock_guard<std::recursive_mutex> lock(mutex_);
#ifdef ROSEN_OHOS
    // invalidate cache if high contrast flag changed
    if (isCached_ && canvas.isHighContrastEnabled() != cachedHighContrast_) {
        ClearCache();
    }
    // Generate or clear cache if cache state changed
    if (canvas.GetCacheType() == Drawing::CacheType::ENABLED && !isCached_) {
        GenerateCache(&canvas, rect);
    } else if (canvas.GetCacheType() == Drawing::CacheType::DISABLED && isCached_) {
        ClearCache();
    }
#endif
    Rect tmpRect;
    if (rect != nullptr) {
        tmpRect = *rect;
    }
    if (mode_ == DrawCmdList::UnmarshalMode::IMMEDIATE) {
        PlaybackByBuffer(canvas, &tmpRect);
    }
    if (mode_ == DrawCmdList::UnmarshalMode::DEFERRED) {
        PlaybackByVector(canvas, &tmpRect);
    }
}

void DrawCmdList::GenerateCache(Canvas* canvas, const Rect* rect)
{
#ifdef ROSEN_OHOS
    if (isCached_) {
        LOGD("DrawCmdList::GenerateCache Invoke multiple times");
        return;
    }

    std::lock_guard<std::recursive_mutex> lock(mutex_);
    if (mode_ == DrawCmdList::UnmarshalMode::IMMEDIATE) {
        GenerateCacheByBuffer(canvas, rect);
    }
    if (mode_ == DrawCmdList::UnmarshalMode::DEFERRED) {
        GenerateCacheByVector(canvas, rect);
    }
#endif
}

bool DrawCmdList::GetIsCache()
{
    return isCached_;
}

void DrawCmdList::SetIsCache(bool isCached)
{
    isCached_ = isCached;
}

bool DrawCmdList::GetCachedHighContrast()
{
    return cachedHighContrast_;
}

void DrawCmdList::SetCachedHighContrast(bool cachedHighContrast)
{
    cachedHighContrast_ = cachedHighContrast;
}

std::vector<std::pair<uint32_t, uint32_t>> DrawCmdList::GetReplacedOpList()
{
    return replacedOpListForBuffer_;
}

void DrawCmdList::SetReplacedOpList(std::vector<std::pair<uint32_t, uint32_t>> replacedOpList)
{
    replacedOpListForBuffer_ = replacedOpList;
}

void DrawCmdList::UpdateNodeIdToPicture(NodeId nodeId)
{
    if (drawOpItems_.size() == 0) {
        return;
    }
    for (size_t i = 0; i < drawOpItems_.size(); ++i) {
        auto opItem = drawOpItems_[i];
        if (!opItem) {
            continue;
        }
        opItem->SetNodeId(nodeId);
    }
}

void DrawCmdList::ClearCache()
{
#ifdef ROSEN_OHOS
    // restore the original op
    for (auto& [index, op] : replacedOpListForVector_) {
        op.swap(drawOpItems_[index]);
    }
    replacedOpListForVector_.clear();
    replacedOpListForBuffer_.clear();
    isCached_ = false;
#endif
}

void DrawCmdList::GenerateCacheByVector(Canvas* canvas, const Rect* rect)
{
#ifdef ROSEN_OHOS
    if (drawOpItems_.size() == 0) {
        return;
    }
    uint32_t opSize = drawOpItems_.size();
    for (auto index = 0u; index < opSize; index++) {
        std::shared_ptr<DrawOpItem> op = drawOpItems_[index];
        if (!op || op->GetType() != DrawOpItem::TEXT_BLOB_OPITEM) {
            continue;
        }
        DrawTextBlobOpItem* textBlobOp = static_cast<DrawTextBlobOpItem*>(op.get());
        auto replaceCache = textBlobOp->GenerateCachedOpItem(canvas);
        if (replaceCache) {
            replacedOpListForVector_.emplace_back(index, op);
            drawOpItems_[index] = replaceCache;
        }
    }
    isCached_ = true;
    cachedHighContrast_ = canvas && canvas->isHighContrastEnabled();
#endif
}

void DrawCmdList::GenerateCacheByBuffer(Canvas* canvas, const Rect* rect)
{
#ifdef ROSEN_OHOS
    if (opAllocator_.GetSize() <= offset_) {
        return;
    }

    uint32_t offset = offset_;
    GenerateCachedOpItemPlayer player = { *this, canvas, rect };
    uint32_t maxOffset = opAllocator_.GetSize();
    do {
        void* itemPtr = opAllocator_.OffsetToAddr(offset);
        auto* curOpItemPtr = static_cast<OpItem*>(itemPtr);
        if (curOpItemPtr == nullptr) {
            LOGE("DrawCmdList::GenerateCacheByBuffer failed, opItem is nullptr");
            break;
        }
        bool replaceSuccess = player.GenerateCachedOpItem(curOpItemPtr->GetType(), itemPtr);
        if (replaceSuccess) {
            replacedOpListForBuffer_.push_back({offset, lastOpItemOffset_.value()});
        }
        offset = curOpItemPtr->GetNextOpItemOffset();
    } while (offset != 0 && offset < maxOffset);
    isCached_ = true;
    cachedHighContrast_ = canvas && canvas->isHighContrastEnabled();
#endif
}

void DrawCmdList::PlaybackToDrawCmdList(std::shared_ptr<DrawCmdList> drawCmdList)
{
    std::lock_guard<std::recursive_mutex> lock(mutex_);
    if (mode_ == DrawCmdList::UnmarshalMode::DEFERRED) {
        std::lock_guard<std::recursive_mutex> lock(drawCmdList->mutex_);
        drawCmdList->drawOpItems_.insert(drawCmdList->drawOpItems_.end(), drawOpItems_.begin(), drawOpItems_.end());
        return;
    }

    void* addr = opAllocator_.OffsetToAddr(offset_);
    if (addr == nullptr) {
        return;
    }

#ifdef SUPPORT_OHOS_PIXMAP
    {
        std::lock_guard<std::mutex> lock(drawCmdList->imageObjectMutex_);
        drawCmdList->imageObjectVec_.swap(imageObjectVec_);
    }
#endif
    {
        std::lock_guard<std::mutex> lock(drawCmdList->imageBaseObjMutex_);
        drawCmdList->imageBaseObjVec_.swap(imageBaseObjVec_);
    }
    size_t size = opAllocator_.GetSize() - offset_;
    auto imageData = GetAllImageData();
    auto bitmapData = GetAllBitmapData();
    drawCmdList->opAllocator_.Add(addr, size);
    if (imageData.first != nullptr && imageData.second != 0) {
        drawCmdList->AddImageData(imageData.first, imageData.second);
    }

    if (bitmapData.first != nullptr && bitmapData.second != 0) {
        drawCmdList->AddBitmapData(bitmapData.first, bitmapData.second);
    }
}

void DrawCmdList::PlaybackByVector(Canvas& canvas, const Rect* rect)
{
    if (drawOpItems_.empty()) {
        return;
    }
    for (auto op : drawOpItems_) {
        if (op) {
            op->Playback(&canvas, rect);
        }
    }
    canvas.DetachPaint();
}

void DrawCmdList::PlaybackByBuffer(Canvas& canvas, const Rect* rect)
{
    if (opAllocator_.GetSize() <= offset_) {
        return;
    }
    uint32_t offset = offset_;
    if (lastOpGenSize_ != opAllocator_.GetSize()) {
        UnmarshallingPlayer player = { *this };
        drawOpItems_.clear();
        do {
            void* itemPtr = opAllocator_.OffsetToAddr(offset);
            auto* curOpItemPtr = static_cast<OpItem*>(itemPtr);
            if (curOpItemPtr == nullptr) {
                break;
            }
            uint32_t type = curOpItemPtr->GetType();
            if (auto op = player.Unmarshalling(type, itemPtr)) {
                drawOpItems_.emplace_back(op);
            }
            offset = curOpItemPtr->GetNextOpItemOffset();
        } while (offset != 0);
        lastOpGenSize_ = opAllocator_.GetSize();
    }
    for (auto op : drawOpItems_) {
        if (op) {
            op->Playback(&canvas, rect);
        }
    }
    canvas.DetachPaint();
}
} // namespace Drawing
} // namespace Rosen
} // namespace OHOS
