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

#ifndef DRAW_CMD_LIST_H
#define DRAW_CMD_LIST_H

#include "draw/canvas.h"
#include "recording/cmd_list.h"

namespace OHOS {
namespace Rosen {
namespace Drawing {
class DrawCmdList : public CmdList {
public:
    DrawCmdList() = default;
    DrawCmdList(int32_t width, int32_t height);
    ~DrawCmdList() override = default;

    /*
     * @brief       Creates a DrawCmdList with contiguous buffers.
     * @param data  A contiguous buffers.
     */
    static std::shared_ptr<DrawCmdList> CreateFromData(const CmdListData& data);

    /*
     * @brief         Calls the corresponding operations of all opitems in DrawCmdList to the canvas.
     * @param canvas  Implements the playback action of the DrawCmdList in the Canvas.
     */
    void Playback(Canvas& canvas, const Rect* rect = nullptr) const;

    /*
     * @brief  Gets the width of the DrawCmdList.
     */
    int32_t GetWidth() const;

    /*
     * @brief  Gets the height of the DrawCmdList.
     */
    int32_t GetHeight() const;

    /*
     * @brief  Sets the width of the DrawCmdList.
     */
    void SetWidth(int32_t width);

    /*
     * @brief  Sets the height of the DrawCmdList.
     */
    void SetHeight(int32_t height);

private:
    MemAllocator largeObjectAllocator_;
    int32_t width_;
    int32_t height_;
};

using DrawCmdListPtr = std::shared_ptr<DrawCmdList>;
} // namespace Drawing
} // namespace Rosen
} // namespace OHOS

#endif // DRAW_CMD_LIST_H
