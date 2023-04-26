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

#include "recording/draw_cmd.h"

namespace OHOS {
namespace Rosen {
namespace Drawing {

DrawCmdList::DrawCmdList(int width, int height) : width_(width), height_(height)
{
    opAllocator_.Add(&width_, sizeof(int));
    opAllocator_.Add(&height_, sizeof(int));
}

DrawCmdList::DrawCmdList(const CmdListData& cmdListData) : CmdList(cmdListData)
{
    int* width = static_cast<int*>(opAllocator_.OffsetToAddr(0));
    int* height = static_cast<int*>(opAllocator_.OffsetToAddr(sizeof(int)));
    if (width && height) {
        width_ = *width;
        height_ = *height;
    } else {
        width_ = 0;
        height_ = 0;
    }
}

DrawCmdList::DrawCmdList(const CmdListData& cmdListData, const LargeObjectData& largeObjectData)
    : DrawCmdList(cmdListData)
{
    largeObjectAllocator_.BuildFromData(largeObjectData.first, largeObjectData.second);
}

int DrawCmdList::AddLargeObject(const LargeObjectData& data)
{
    std::lock_guard<std::mutex> lock(mutex_);
    void* addr = largeObjectAllocator_.Add(data.first, data.second);
    if (addr == nullptr) {
        LOGE("DrawCmdList AddLargeObject failed!");
        return 0;
    }
    return largeObjectAllocator_.AddrToOffset(addr);
}

LargeObjectData DrawCmdList::GetLargeObjectData() const
{
    return std::make_pair(largeObjectAllocator_.GetData(), largeObjectAllocator_.GetSize());
}

int DrawCmdList::GetWidth() const
{
    return width_;
}

int DrawCmdList::GetHeight() const
{
    return height_;
}

void DrawCmdList::SetWidth(int width)
{
    width_ = width;
}

void DrawCmdList::SetHeight(int height)
{
    height_ = height;
}

void DrawCmdList::Playback(Canvas& canvas, const Rect* rect) const
{
    if (width_ <= 0 || height_ <= 0) {
        return;
    }

    int offset = 2 * sizeof(int);   // 2 is width and height. Offset of first OpItem is behind the width and height

    CanvasPlayer player = { canvas, opAllocator_, largeObjectAllocator_ };
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
}

} // namespace Drawing
} // namespace Rosen
} // namespace OHOS
