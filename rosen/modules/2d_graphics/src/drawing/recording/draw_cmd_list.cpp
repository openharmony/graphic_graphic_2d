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

#include <memory>

#include "recording/draw_cmd.h"
#include "utils/log.h"

namespace OHOS {
namespace Rosen {
namespace Drawing {

DrawCmdList::DrawCmdList(int32_t width, int32_t height) : width_(width), height_(height)
{
    opAllocator_.Add(&width_, sizeof(int32_t));
    opAllocator_.Add(&height_, sizeof(int32_t));
}

void DrawCmdList::ClearOp()
{
    opAllocator_.ClearData();
    opAllocator_.Add(&width_, sizeof(int32_t));
    opAllocator_.Add(&height_, sizeof(int32_t));
    imageAllocator_.ClearData();
    lastOpItemOffset_ = std::nullopt;
}

std::shared_ptr<DrawCmdList> DrawCmdList::CreateFromData(const CmdListData& data, bool isCopy)
{
    auto cmdList = std::make_shared<DrawCmdList>();
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
    uint32_t offset = 2 * sizeof(int32_t); // 2 is width and height.Offset of first OpItem is behind the w and h
    if (opAllocator_.GetSize() <= offset && unmarshalledOpItems_.size() == 0) {
        return true;
    }
    return false;
}

void DrawCmdList::UnmarshallingOps()
{
    uint32_t offset = 2 * sizeof(int32_t); // 2 is width and height.Offset of first OpItem is behind the w and h
    if (opAllocator_.GetSize() <= offset) {
        return;
    }

    UnmarshallingPlayer player = { *this };

    uint32_t opReplaceIndex = 0;
    do {
        void* itemPtr = opAllocator_.OffsetToAddr(offset);
        auto* curOpItemPtr = static_cast<OpItem*>(itemPtr);
        if (curOpItemPtr != nullptr) {
            uint32_t type = curOpItemPtr->GetType();
            if (type == DrawOpItem::CMD_LIST_OPITEM) {
                return;
            }
            if (opReplaceIndex < replacedOpList_.size() &&
                replacedOpList_[opReplaceIndex].first == offset) {
                auto* replacePtr = opAllocator_.OffsetToAddr(replacedOpList_[opReplaceIndex].second);
                auto* replaceOpItemPtr = static_cast<OpItem*>(replacePtr);
                unmarshalledOpItems_.emplace_back(player.Unmarshalling(replaceOpItemPtr->GetType(), replacePtr));
                opReplacedByDrivenRender_.emplace_back((unmarshalledOpItems_.size() - 1),
                    player.Unmarshalling(type, itemPtr));
                opReplaceIndex++;
            } else {
                unmarshalledOpItems_.emplace_back(player.Unmarshalling(type, itemPtr));
            }
            offset = curOpItemPtr->GetNextOpItemOffset();
        } else {
            LOGE("DrawCmdList::UnmarshallingOps failed, opItem is nullptr");
            break;
        }
        if (!replacedOpList_.empty() && offset >= replacedOpList_[0].second) {
            LOGD("DrawCmdList::UnmarshallingOps seek end by cache textOps");
            break;
        }
    } while (offset != 0);
}

std::vector<std::pair<OpItem*, void*>> DrawCmdList::UnmarshallingCmdList()
{
    std::vector<std::pair<OpItem*, void*>> opItems;
    uint32_t offset = 2 * sizeof(int32_t); // 2 is width and height.Offset of first OpItem is behind the w and h
    if (width_ <= 0 || height_ <= 0 || opAllocator_.GetSize() <= offset) {
        return opItems;
    }

    UnmarshallingPlayer player = { *this };
    do {
        void* itemPtr = opAllocator_.OffsetToAddr(offset);
        auto* curOpItemPtr = static_cast<OpItem*>(itemPtr);
        if (curOpItemPtr != nullptr) {
            player.Unmarshalling(curOpItemPtr->GetType(), itemPtr);
            offset = curOpItemPtr->GetNextOpItemOffset();
            auto playFunc = CanvasPlayer::GetFuncFromType(curOpItemPtr->GetType());
            if (playFunc == nullptr) {
                continue;
            }
            opItems.push_back({curOpItemPtr, (void*)playFunc});
        } else {
            LOGE("DrawCmdList::UnmarshallingCmdList failed, opItem is nullptr");
            break;
        }
    } while (offset != 0);
    return opItems;
}

void DrawCmdList::Playback(Canvas& canvas, const Rect* rect)
{
    if (canvas.GetDrawingType() == DrawingType::RECORDING) {
        AddOpToCmdList(static_cast<RecordingCanvas&>(canvas).GetDrawCmdList());
        return;
    }
    uint32_t offset = 2 * sizeof(int32_t); // 2 is width and height.Offset of first OpItem is behind the w and h
    if (opAllocator_.GetSize() <= offset && unmarshalledOpItems_.size() == 0) {
        return;
    }

#ifdef ROSEN_OHOS
    // invalidate cache if high contrast flag changed
    if (isCached_ && canvas.isHighContrastEnabled() != cachedHighContrast_) {
        ClearCache();
    }
    // Generate or clear cache if cache state changed
    if (canvas.GetCacheType() == Drawing::CacheType::ENABLED && !isCached_) {
        GenerateCacheInRenderService(&canvas, rect);
    } else if (canvas.GetCacheType() == Drawing::CacheType::DISABLED && isCached_) {
        ClearCache();
    }
#endif

    Rect tmpRect;
    if (rect != nullptr) {
        tmpRect = *rect;
    }
    CanvasPlayer player = { canvas, *this, tmpRect};
    if (unmarshalledOpItems_.size() == 0) {
        do {
            void* itemPtr = opAllocator_.OffsetToAddr(offset);
            auto* curOpItemPtr = static_cast<OpItem*>(itemPtr);
            if (curOpItemPtr != nullptr) {
                if (!player.Playback(curOpItemPtr->GetType(), itemPtr)) {
                    LOGE("DrawCmdList::Playback failed!");
                    break;
                }
                offset = curOpItemPtr->GetNextOpItemOffset();
            } else {
                LOGE("DrawCmdList::Playback failed, opItem is nullptr");
                break;
            }
        } while (offset != 0);
    } else {
        for (auto iter = unmarshalledOpItems_.begin(); iter != unmarshalledOpItems_.end(); ++iter) {
            if ((*iter) != nullptr) {
                if (!player.Playback((*iter)->GetType(), (*iter).get())) {
                    LOGE("DrawCmdList::Playback failed!");
                    break;
                }
            } else {
                LOGE("DrawCmdList::Playback failed, opItem is nullptr");
                break;
            }
        }
    }
}

void DrawCmdList::GenerateCache(Canvas* canvas, const Rect* rect)
{
#ifdef ROSEN_OHOS
    uint32_t offset = 2 * sizeof(int32_t);
    if (opAllocator_.GetSize() <= offset) {
        return;
    }

    if (isCached_) {
        LOGD("DrawCmdList::GenerateCache Invoke multiple times");
        return;
    }
    GenerateCachedOpItemPlayer player = {*this, canvas, rect};

    uint32_t maxOffset = opAllocator_.GetSize();
    do {
        void* itemPtr = opAllocator_.OffsetToAddr(offset);
        auto* curOpItemPtr = static_cast<OpItem*>(itemPtr);
        if (curOpItemPtr != nullptr) {
            bool replaceSuccess = false;
            player.GenerateCachedOpItem(curOpItemPtr->GetType(), itemPtr, true, replaceSuccess);
            if (replaceSuccess) {
                replacedOpList_.push_back({offset, lastOpItemOffset_.value()});
            }
            offset = curOpItemPtr->GetNextOpItemOffset();
        } else {
            LOGE("DrawCmdList::GenerateCache failed, opItem is nullptr");
            break;
        }
    } while (offset != 0 && offset < maxOffset);

    isCached_ = true;
    cachedHighContrast_ = canvas && canvas->isHighContrastEnabled();
#endif
}

void DrawCmdList::GenerateCacheInRenderService(Canvas* canvas, const Rect* rect)
{
#ifdef ROSEN_OHOS
    if (unmarshalledOpItems_.size() == 0) {
        return;
    }

    GenerateCachedOpItemPlayer player = {*this, canvas, rect};
    for (int i = 0; i < unmarshalledOpItems_.size(); ++i) {
        auto opItem = unmarshalledOpItems_[i];
        if (opItem != nullptr) {
            bool replaceSuccess = false;
            auto replaceCache = player.GenerateCachedOpItem(opItem->GetType(), opItem.get(), false, replaceSuccess);
            if (replaceCache) {
                opReplacedByDrivenRender_.emplace_back(i, unmarshalledOpItems_[i]);
                unmarshalledOpItems_[i] = replaceCache;
            }
        }
    }

    isCached_ = true;
    cachedHighContrast_ = canvas && canvas->isHighContrastEnabled();
#endif
}

void DrawCmdList::ClearCache()
{
    // restore the original op
    for (auto& [index, op] : opReplacedByDrivenRender_) {
        unmarshalledOpItems_[index] = op;
    }
    opReplacedByDrivenRender_.clear();
    replacedOpList_.clear();
    isCached_ = false;
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
    return replacedOpList_;
}

void DrawCmdList::SetReplacedOpList(std::vector<std::pair<uint32_t, uint32_t>> replacedOpList)
{
    replacedOpList_ = replacedOpList;
}

void DrawCmdList::AddOpToCmdList(std::shared_ptr<DrawCmdList> cmdList)
{
    CmdListHandle handle = { 0 };
    handle.type = GetType();

    auto data = GetData();
    if (data.first != nullptr && data.second != 0) {
        handle.offset = cmdList->AddCmdListData(data);
        handle.size = data.second;
    } else {
        return;
    }

    auto imageData = GetAllImageData();
    if (imageData.first != nullptr && imageData.second != 0) {
        handle.imageOffset = cmdList->AddImageData(imageData.first, imageData.second);
        handle.imageSize = imageData.second;
    }

    cmdList->AddOp<DrawCmdListOpItem>(handle);
    return;
}

} // namespace Drawing
} // namespace Rosen
} // namespace OHOS
