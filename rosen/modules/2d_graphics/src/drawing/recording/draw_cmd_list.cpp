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
    if (width_ <= 0 || height_ <= 0 ||
        (opAllocator_.GetSize() <= offset && opItems_.size() == 0)) {
        return true;
    }
    return false;
}

void DrawCmdList::UnmarshallingOps()
{
    uint32_t offset = 2 * sizeof(int32_t); // 2 is width and height.Offset of first OpItem is behind the w and h
    if (width_ <= 0 || height_ <= 0 || opAllocator_.GetSize() <= offset) {
        return;
    }

    UnmarshallingPlayer player = { *this };
    do {
        void* itemPtr = opAllocator_.OffsetToAddr(offset);
        auto* curOpItemPtr = static_cast<OpItem*>(itemPtr);
        if (curOpItemPtr != nullptr) {
            uint32_t type = curOpItemPtr->GetType();
            opItems_.emplace_back(player.Unmarshalling(type, itemPtr));
            offset = curOpItemPtr->GetNextOpItemOffset();
        } else {
            LOGE("DrawCmdList::UnmarshallingOps failed, opItem is nullptr");
            break;
        }
    } while (offset != 0);
}

void DrawCmdList::Playback(Canvas& canvas, const Rect* rect) const
{
    uint32_t offset = 2 * sizeof(int32_t); // 2 is width and height.Offset of first OpItem is behind the w and h
    if (width_ <= 0 || height_ <= 0 ||
        (opAllocator_.GetSize() <= offset && opItems_.size() == 0)) {
        return;
    }

    Rect tmpRect;
    if (rect != nullptr) {
        tmpRect = *rect;
    }
    CanvasPlayer player = { canvas, *this, tmpRect};

    for (auto iter = opItems_.begin(); iter != opItems_.end(); ++iter) {
        if ((*iter) != nullptr) {
            if (!player.Playback((*iter)->GetType(), (*iter))) {
                LOGE("DrawCmdList::Playback failed!");
                break;
            }
        } else {
            LOGE("DrawCmdList::Playback failed, opItem is nullptr");
            break;
        }
    }
}

} // namespace Drawing
} // namespace Rosen
} // namespace OHOS
