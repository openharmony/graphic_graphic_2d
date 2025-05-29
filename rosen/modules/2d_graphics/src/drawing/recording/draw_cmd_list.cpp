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
#include <unordered_set>

#include "recording/draw_cmd.h"
#include "recording/recording_canvas.h"
#include "utils/graphic_coretrace.h"
#include "utils/log.h"
#include "utils/performanceCaculate.h"

namespace OHOS {
namespace Rosen {
namespace Drawing {
namespace {
// WhiteList for hybridRender DrawOpItemTypes
const std::unordered_set<uint32_t> HYBRID_RENDER_DRAW_OPITEM_TYPES = {
    DrawOpItem::OPITEM_HEAD,
    DrawOpItem::PATH_OPITEM,
    DrawOpItem::TEXT_BLOB_OPITEM,
    DrawOpItem::SYMBOL_OPITEM,
    DrawOpItem::CLIP_RECT_OPITEM,
    DrawOpItem::CLIP_IRECT_OPITEM,
    DrawOpItem::CLIP_ROUND_RECT_OPITEM,
    DrawOpItem::CLIP_PATH_OPITEM,
    DrawOpItem::CLIP_REGION_OPITEM,
    DrawOpItem::SET_MATRIX_OPITEM,
    DrawOpItem::CONCAT_MATRIX_OPITEM,
    DrawOpItem::TRANSLATE_OPITEM,
    DrawOpItem::SCALE_OPITEM,
    DrawOpItem::ROTATE_OPITEM,
    DrawOpItem::SHEAR_OPITEM,
    DrawOpItem::FLUSH_OPITEM,
    DrawOpItem::CLEAR_OPITEM,
    DrawOpItem::SAVE_OPITEM,
    DrawOpItem::SAVE_LAYER_OPITEM,
    DrawOpItem::RESTORE_OPITEM,
    DrawOpItem::DISCARD_OPITEM,
    DrawOpItem::CLIP_ADAPTIVE_ROUND_RECT_OPITEM,
    DrawOpItem::HYBRID_RENDER_PIXELMAP_SIZE_OPITEM,
};
constexpr uint32_t DRAWCMDLIST_OPSIZE_COUNT_LIMIT = 50000;
}

std::shared_ptr<DrawCmdList> DrawCmdList::CreateFromData(const CmdListData& data, bool isCopy)
{
    auto cmdList = std::make_shared<DrawCmdList>(DrawCmdList::UnmarshalMode::DEFERRED);
    if (isCopy) {
        cmdList->opAllocator_.BuildFromDataWithCopy(data.first, data.second);
    } else {
        cmdList->opAllocator_.BuildFromData(data.first, data.second);
    }

    int32_t* width = static_cast<int32_t*>(cmdList->opAllocator_.OffsetToAddr(0, sizeof(int32_t)));
    int32_t* height = static_cast<int32_t*>(cmdList->opAllocator_.OffsetToAddr(sizeof(int32_t), sizeof(int32_t)));
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
    if (drawOpItems_.size() == 0 && isNeedUnmarshalOnDestruct_) {
        UnmarshallingDrawOps();
    }
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
    {
        std::lock_guard<std::mutex> lock(recordCmdMutex_);
        recordCmdVec_.clear();
    }
    {
        std::lock_guard<std::mutex> lock(imageObjectMutex_);
        imageObjectVec_.clear();
    }
    {
        std::lock_guard<std::mutex> lock(imageBaseObjMutex_);
        imageBaseObjVec_.clear();
    }
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

bool DrawCmdList::GetNoNeedUICaptured() const
{
    return noNeedUICaptured_;
}

void DrawCmdList::SetNoNeedUICaptured(bool noNeedUICaptured)
{
    noNeedUICaptured_ = noNeedUICaptured;
}

bool DrawCmdList::IsEmpty() const
{
    if (mode_ == DrawCmdList::UnmarshalMode::DEFERRED) {
        return drawOpItems_.empty();
    }
    size_t offset = 2 * sizeof(int32_t); // 2 is width and height.Offset of first OpItem is behind the w and h
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
        desc += item->GetOpDesc();
        desc += "\n";
    }
    LOGD("DrawCmdList::GetOpsWithDesc %{public}s, opitem sz: %{public}zu", desc.c_str(), drawOpItems_.size());
    return desc;
}

void DrawCmdList::Dump(std::string& out)
{
    bool found = false;
    std::lock_guard<std::recursive_mutex> lock(mutex_);
    for (auto& item : drawOpItems_) {
        if (item == nullptr) {
            continue;
        }
        found = true;
        item->Dump(out);
        out += ' ';
    }
    if (found) {
        out.pop_back();
    }
}

void DrawCmdList::MarshallingDrawOps()
{
    if (mode_ == DrawCmdList::UnmarshalMode::IMMEDIATE) {
        return;
    }
    std::lock_guard<std::recursive_mutex> lock(mutex_);
    if (replacedOpListForVector_.empty()) {
        for (auto& op : drawOpItems_) {
            if (op) {
                op->Marshalling(*this);
            }
        }
        return;
    }
    for (auto& [index, op] : replacedOpListForVector_) {
        op.swap(drawOpItems_[index]);
    }
    std::vector<uint32_t> opIndexForCache(replacedOpListForVector_.size());
    uint32_t opReplaceIndex = 0;
    for (auto index = 0u; index < drawOpItems_.size(); ++index) {
        if (drawOpItems_[index]) {
            drawOpItems_[index]->Marshalling(*this);
        }
        if (index == static_cast<size_t>(replacedOpListForVector_[opReplaceIndex].first)) {
            opIndexForCache[opReplaceIndex] = lastOpItemOffset_.value();
            ++opReplaceIndex;
        }
    }
    for (auto index = 0u; index < replacedOpListForVector_.size(); ++index) {
        if (replacedOpListForVector_[index].second) {
            replacedOpListForVector_[index].second->Marshalling(*this);
        }
        replacedOpListForBuffer_.emplace_back(opIndexForCache[index], lastOpItemOffset_.value());
    }
}

void DrawCmdList::ProfilerMarshallingDrawOps(Drawing::DrawCmdList *cmdlist)
{
    if (mode_ == DrawCmdList::UnmarshalMode::IMMEDIATE) {
        return;
    }
    if (!cmdlist) {
        return;
    }

    std::lock_guard<std::recursive_mutex> lock(mutex_);
    for (auto& op : drawOpItems_) {
        if (!op) {
            continue;
        }
        if (op->GetType() == DrawOpItem::IMAGE_WITH_PARM_OPITEM) {
            continue;
        }
        if (op->GetType() == DrawOpItem::IMAGE_OPITEM) {
            continue;
        }
        if (op->GetType() == DrawOpItem::IMAGE_RECT_OPITEM) {
            continue;
        }
        op->Marshalling(*cmdlist);
    }
}

void DrawCmdList::CaculatePerformanceOpType()
{
    size_t offset = offset_;
    const int caculatePerformaceCount = 500;    // 被测单接口用例至少出现500次以上
    std::map<uint32_t, uint32_t> opTypeCountMap;
    uint32_t count = 0;
    do {
        count++;
        void* itemPtr = opAllocator_.OffsetToAddr(offset, sizeof(OpItem));
        auto* curOpItemPtr = static_cast<OpItem*>(itemPtr);
        if (curOpItemPtr == nullptr) {
            break;
        }
        uint32_t type = curOpItemPtr->GetType();
        if (opTypeCountMap.find(type) != opTypeCountMap.end()) {
            if (++opTypeCountMap[type] > caculatePerformaceCount) {
                performanceCaculateOpType_ = type;
                DRAWING_PERFORMANCE_START_CACULATE;
                return;
            }
        } else {
            opTypeCountMap[type] = 1;   // 记录出现的第1次
        }
        if (curOpItemPtr->GetNextOpItemOffset() < offset + sizeof(OpItem)) {
            break;
        }
        offset = curOpItemPtr->GetNextOpItemOffset();
    } while (offset != 0 && count <= MAX_OPITEMSIZE);
}

void DrawCmdList::UnmarshallingDrawOps(uint32_t* opItemCount)
{
    if (PerformanceCaculate::GetDrawingTestRecordingEnabled()) {
        CaculatePerformanceOpType();
    }
    if (performanceCaculateOpType_ != 0) {
        LOGI("Drawing Performance UnmarshallingDrawOps begin %{public}lld", PerformanceCaculate::GetUpTime());
    }

    if (opAllocator_.GetSize() <= offset_ || width_ <= 0 || height_ <= 0) {
        return;
    }

    UnmarshallingPlayer player = { *this };
    drawOpItems_.clear();
    lastOpGenSize_ = 0;
    uint32_t opReplaceIndex = 0;
    size_t offset = offset_;
    uint32_t count = 0;
    do {
        count++;
        if (opItemCount && ++(*opItemCount) > MAX_OPITEMSIZE) {
            LOGE("DrawCmdList::UnmarshallingOps failed, opItem count exceed limit");
            break;
        }
        void* itemPtr = opAllocator_.OffsetToAddr(offset, sizeof(OpItem));
        auto* curOpItemPtr = static_cast<OpItem*>(itemPtr);
        if (curOpItemPtr == nullptr) {
            LOGE("DrawCmdList::UnmarshallingOps failed, opItem is nullptr");
            break;
        }
        uint32_t type = curOpItemPtr->GetType();
        auto op = player.Unmarshalling(type, itemPtr, opAllocator_.GetSize() - offset);
        if (!op) {
            if (curOpItemPtr->GetNextOpItemOffset() < offset + sizeof(OpItem)) {
                break;
            }
            offset = curOpItemPtr->GetNextOpItemOffset();
            continue;
        }
        if (opReplaceIndex < replacedOpListForBuffer_.size() &&
            replacedOpListForBuffer_[opReplaceIndex].first == offset) {
            auto* replacePtr = opAllocator_.OffsetToAddr(
                replacedOpListForBuffer_[opReplaceIndex].second, sizeof(OpItem));
            if (replacePtr == nullptr) {
                LOGE("DrawCmdList::Unmarshalling replace Ops failed, replace op is nullptr");
                break;
            }
            auto* replaceOpItemPtr = static_cast<OpItem*>(replacePtr);
            size_t avaliableSize = opAllocator_.GetSize() - replacedOpListForBuffer_[opReplaceIndex].second;
            auto replaceOp = player.Unmarshalling(replaceOpItemPtr->GetType(), replacePtr, avaliableSize);
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
        if (curOpItemPtr->GetNextOpItemOffset() < offset + sizeof(OpItem)) {
            break;
        }
        offset = curOpItemPtr->GetNextOpItemOffset();
        if (!replacedOpListForBuffer_.empty() && offset >= replacedOpListForBuffer_[0].second) {
            LOGD("DrawCmdList::UnmarshallingOps seek end by cache textOps");
            break;
        }
    } while (offset != 0 && count <= MAX_OPITEMSIZE);
    lastOpGenSize_ = opAllocator_.GetSize();

    opAllocator_.ClearData();
    imageAllocator_.ClearData();
    bitmapAllocator_.ClearData();
    opAllocator_.Add(&width_, sizeof(int32_t));
    opAllocator_.Add(&height_, sizeof(int32_t));

    if (performanceCaculateOpType_ != 0) {
        LOGI("Drawing Performance UnmarshallingDrawOps end %{public}lld", PerformanceCaculate::GetUpTime());
    }
}

void DrawCmdList::Playback(Canvas& canvas, const Rect* rect)
{
    RECORD_GPURESOURCE_CORETRACE_CALLER(Drawing::CoreFunction::
        GRAPHIC2D_DRAWCMDLIST_PLAYBACK);
    if (canvas.GetUICapture() && noNeedUICaptured_) {
        return;
    }

    if (width_ <= 0 || height_ <= 0) {
        return;
    }
    if (performanceCaculateOpType_ != 0) {
        LOGI("Drawing Performance Playback begin %{public}lld", PerformanceCaculate::GetUpTime());
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
    } else if (mode_ == DrawCmdList::UnmarshalMode::DEFERRED) {
        PlaybackByVector(canvas, &tmpRect);
    }
    if (performanceCaculateOpType_ != 0) {
        DRAWING_PERFORMANCE_STOP_CACULATE;
        performanceCaculateOpType_ = 0;
        LOGI("Drawing Performance Playback end %{public}lld", PerformanceCaculate::GetUpTime());
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
    } else if (mode_ == DrawCmdList::UnmarshalMode::DEFERRED) {
        GenerateCacheByVector(canvas, rect);
    }
#endif
}

bool DrawCmdList::GetIsCache() const
{
    return isCached_;
}

void DrawCmdList::SetIsCache(bool isCached)
{
    isCached_ = isCached;
}

bool DrawCmdList::GetCachedHighContrast() const
{
    return cachedHighContrast_;
}

void DrawCmdList::SetCachedHighContrast(bool cachedHighContrast)
{
    cachedHighContrast_ = cachedHighContrast;
}

std::vector<std::pair<size_t, size_t>> DrawCmdList::GetReplacedOpList()
{
    return replacedOpListForBuffer_;
}

void DrawCmdList::SetReplacedOpList(std::vector<std::pair<size_t, size_t>> replacedOpList)
{
    replacedOpListForBuffer_ = replacedOpList;
}

DrawCmdList::HybridRenderType DrawCmdList::GetHybridRenderType() const
{
    return hybridRenderType_;
}

void DrawCmdList::SetHybridRenderType(DrawCmdList::HybridRenderType hybridRenderType)
{
    hybridRenderType_ = hybridRenderType;
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
    for (auto index = 0u; index < opSize; ++index) {
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

    size_t offset = offset_;
    GenerateCachedOpItemPlayer player = { *this, canvas, rect };
    uint32_t maxOffset = opAllocator_.GetSize();
    uint32_t count = 0;
    do {
        count++;
        void* itemPtr = opAllocator_.OffsetToAddr(offset, sizeof(OpItem));
        auto* curOpItemPtr = static_cast<OpItem*>(itemPtr);
        if (curOpItemPtr == nullptr) {
            LOGE("DrawCmdList::GenerateCacheByBuffer failed, opItem is nullptr");
            break;
        }
        size_t avaliableSize = opAllocator_.GetSize() - offset;
        bool replaceSuccess = player.GenerateCachedOpItem(curOpItemPtr->GetType(), itemPtr, avaliableSize);
        if (replaceSuccess) {
            replacedOpListForBuffer_.push_back({offset, lastOpItemOffset_.value()});
            itemPtr = opAllocator_.OffsetToAddr(offset, sizeof(OpItem));
            curOpItemPtr = static_cast<OpItem*>(itemPtr);
            if (curOpItemPtr == nullptr) {
                LOGE("DrawCmdList::GenerateCache failed, opItem is nullptr");
                break;
            }
        }
        if (curOpItemPtr->GetNextOpItemOffset() < offset + sizeof(OpItem)) {
            break;
        }
        offset = curOpItemPtr->GetNextOpItemOffset();
    } while (offset != 0 && offset < maxOffset && count <= MAX_OPITEMSIZE);
    isCached_ = true;
    cachedHighContrast_ = canvas && canvas->isHighContrastEnabled();
#endif
}

void DrawCmdList::PlaybackToDrawCmdList(std::shared_ptr<DrawCmdList> drawCmdList)
{
    if (!drawCmdList) {
        return;
    }
    std::lock_guard<std::recursive_mutex> lock(mutex_);
    if (mode_ == DrawCmdList::UnmarshalMode::DEFERRED) {
        std::lock_guard<std::recursive_mutex> lock(drawCmdList->mutex_);
        drawCmdList->drawOpItems_.insert(drawCmdList->drawOpItems_.end(), drawOpItems_.begin(), drawOpItems_.end());
        return;
    }

    void* addr = opAllocator_.OffsetToAddr(offset_, 0);
    if (addr == nullptr) {
        return;
    }

    {
        std::lock_guard<std::mutex> lock(drawCmdList->recordCmdMutex_);
        drawCmdList->recordCmdVec_.swap(recordCmdVec_);
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
    uint32_t opCount = 0;
    for (auto op : drawOpItems_) {
        if (isCanvasDrawingOpLimitEnabled_ && opCount > DRAWCMDLIST_OPSIZE_COUNT_LIMIT) {
            LOGE("DrawCmdList::PlaybackByVector Out of DrawOp limit, DrawOpCount: %{public}d", opCount);
            break;
        }
        if (op) {
            op->Playback(&canvas, rect);
            ++opCount;
        }
    }
    canvas.DetachPaint();
}

bool DrawCmdList::UnmarshallingDrawOpsSimple()
{
    if (opAllocator_.GetSize() <= offset_) {
        return false;
    }
    size_t offset = offset_;
    if (lastOpGenSize_ != opAllocator_.GetSize()) {
        uint32_t count = 0;
        UnmarshallingPlayer player = { *this };
        drawOpItems_.clear();
        do {
            count++;
            void* itemPtr = opAllocator_.OffsetToAddr(offset, sizeof(OpItem));
            auto* curOpItemPtr = static_cast<OpItem*>(itemPtr);
            if (curOpItemPtr == nullptr) {
                break;
            }
            uint32_t type = curOpItemPtr->GetType();
            if (auto op = player.Unmarshalling(type, itemPtr, opAllocator_.GetSize() - offset)) {
                drawOpItems_.emplace_back(op);
            }
            if (curOpItemPtr->GetNextOpItemOffset() < offset + sizeof(OpItem)) {
                break;
            }
            offset = curOpItemPtr->GetNextOpItemOffset();
        } while (offset != 0 && count <= MAX_OPITEMSIZE);
        lastOpGenSize_ = opAllocator_.GetSize();
    }
    return true;
}

void DrawCmdList::PlaybackByBuffer(Canvas& canvas, const Rect* rect)
{
    if (!UnmarshallingDrawOpsSimple()) {
        return;
    }
    uint32_t opCount = 0;
    for (auto op : drawOpItems_) {
        if (isCanvasDrawingOpLimitEnabled_ && opCount > DRAWCMDLIST_OPSIZE_COUNT_LIMIT) {
            LOGE("DrawCmdList::PlaybackByBuffer Out of DrawOp limit, DrawOpCount: %{public}d", opCount);
            break;
        }
        if (op) {
            op->Playback(&canvas, rect);
            ++opCount;
        }
    }
    canvas.DetachPaint();
}

void DrawCmdList::GetBounds(Rect& rect)
{
    std::lock_guard<std::recursive_mutex> lock(mutex_);
    for (const auto& op : drawOpItems_) {
        if (op == nullptr) {
            continue;
        }
        switch (op->GetType()) {
            case DrawOpItem::HYBRID_RENDER_PIXELMAP_SIZE_OPITEM: {
                HybridRenderPixelMapSizeOpItem *sizeOp = static_cast<HybridRenderPixelMapSizeOpItem*>(op.get());
                rect.Join(Rect(0, 0, sizeOp->GetWidth(), sizeOp->GetHeight()));
                break;
            }
            default:
                break;
        }
    }
}

bool DrawCmdList::IsHybridRenderEnabled(uint32_t maxPixelMapWidth, uint32_t maxPixelMapHeight)
{
    std::lock_guard<std::recursive_mutex> lock(mutex_);
    // canvasdrawingnode does not support switching from enable to disable
    if (hybridRenderType_ == HybridRenderType::CANVAS) {
        return true;
    }
    if (!UnmarshallingDrawOpsSimple()) {
        return false;
    }
    // check whiteList
    for (const auto& op : drawOpItems_) {
        if (op == nullptr) {
            continue;
        }
        if (HYBRID_RENDER_DRAW_OPITEM_TYPES.find(op->GetType()) == HYBRID_RENDER_DRAW_OPITEM_TYPES.end()) {
            return false;
        }
    }
    // check size
    Drawing::Rect bounds;
    int32_t width = GetWidth();
    int32_t height = GetHeight();
    GetBounds(bounds);
    width = std::max(width, DrawingFloatSaturate2Int(ceilf(bounds.GetWidth())));
    height = std::max(height, DrawingFloatSaturate2Int(ceilf(bounds.GetHeight())));
    if (width < 0 || height < 0 ||
        static_cast<uint32_t>(width) > maxPixelMapWidth || static_cast<uint32_t>(height) > maxPixelMapHeight) {
        return false;
    }
    return true;
}

void DrawCmdList::ProfilerTextBlob(void* handle, uint32_t count, std::shared_ptr<Drawing::DrawCmdList> refDrawCmdList)
{
    if (!handle) {
        return;
    }
    DrawTextBlobOpItem::ConstructorHandle* constructorHandle =
        static_cast<DrawTextBlobOpItem::ConstructorHandle*>(handle);
    if (refDrawCmdList) {
        if (count > 0 && count - 1 < refDrawCmdList->drawOpItems_.size()) {
            auto drawOpItem = refDrawCmdList->drawOpItems_[count - 1];
            if (drawOpItem && drawOpItem->GetType() == DrawOpItem::TEXT_BLOB_OPITEM) {
                auto drawTextOpItem = static_cast<DrawTextBlobOpItem*>(drawOpItem.get());
                constructorHandle->globalUniqueId = drawTextOpItem->GetTypefaceId();
            }
        }
    } else if (constructorHandle->globalUniqueId) {
        constexpr int bitNumber = 30 + 32;
        uint64_t replayMask = (uint64_t)1 << bitNumber;
        constructorHandle->globalUniqueId |= replayMask;
    }
}

void DrawCmdList::PatchTypefaceIds(std::shared_ptr<Drawing::DrawCmdList> refDrawCmdList)
{
    size_t offset = offset_;
    size_t maxOffset = opAllocator_.GetSize();
    uint32_t count = 0;
    do {
        count++;
        void* itemPtr = opAllocator_.OffsetToAddr(offset, sizeof(OpItem));
        auto* curOpItemPtr = static_cast<OpItem*>(itemPtr);
        if (curOpItemPtr == nullptr) {
            break;
        }
        uint32_t type = curOpItemPtr->GetType();
        if (type == DrawOpItem::TEXT_BLOB_OPITEM) {
            DrawTextBlobOpItem::ConstructorHandle* handle =
                static_cast<DrawTextBlobOpItem::ConstructorHandle*>(curOpItemPtr);
            ProfilerTextBlob(handle, count, refDrawCmdList);
        }
        if (curOpItemPtr->GetNextOpItemOffset() < offset + sizeof(OpItem)) {
            break;
        }
        offset = curOpItemPtr->GetNextOpItemOffset();
    } while (offset != 0 && offset < maxOffset && count <= MAX_OPITEMSIZE);
}

void DrawCmdList::Purge()
{
    std::lock_guard<std::recursive_mutex> lock(mutex_);
    for (auto op : drawOpItems_) {
        if (!op) {
            continue;
        }
        auto type = op->GetType();
        if (type == DrawOpItem::PIXELMAP_RECT_OPITEM ||
            type == DrawOpItem::PIXELMAP_WITH_PARM_OPITEM ||
            type == DrawOpItem::PIXELMAP_NINE_OPITEM ||
            type == DrawOpItem::PIXELMAP_LATTICE_OPITEM) {
            op->Purge();
        }
    }
}

void DrawCmdList::SetIsNeedUnmarshalOnDestruct(bool isNeedUnmarshalOnDestruct)
{
    isNeedUnmarshalOnDestruct_ = isNeedUnmarshalOnDestruct;
}

size_t DrawCmdList::GetSize()
{
    size_t totoalSize = sizeof(*this);

    {
        std::lock_guard<std::recursive_mutex> lock(mutex_);
        totoalSize += opAllocator_.GetSize() + imageAllocator_.GetSize() + bitmapAllocator_.GetSize();
        for (auto op : drawOpItems_) {
            if (op) {
                totoalSize += op->GetOpSize();
            }
        }
    }
    return totoalSize;
}

void DrawCmdList::SetCanvasDrawingOpLimitEnable(bool isEnable)
{
    isCanvasDrawingOpLimitEnabled_ = isEnable;
}
} // namespace Drawing
} // namespace Rosen
} // namespace OHOS
