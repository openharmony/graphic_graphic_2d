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
    DrawCmdList(int width, int height);
    explicit DrawCmdList(const CmdListData& cmdListData);
    DrawCmdList(const CmdListData& cmdListData, const LargeObjectData& largeObjectData);
    ~DrawCmdList() override = default;

    /*
     * @brief       Add large object data to the buffers of DrawCmdList.
     * @param data  A large object data.
     * @return      Returns the offset of the contiguous buffers and DrawCmdList head point.
     * @note        using for recording, should to remove after using shared memory
     */
    int AddLargeObject(const LargeObjectData& data);

    /*
     * @brief  Gets the large object buffers of the DrawCmdList.
     */
    LargeObjectData GetLargeObjectData() const;

    /*
     * @brief         Calls the corresponding operations of all opitems in DrawCmdList to the canvas.
     * @param canvas  Implements the playback action of the DrawCmdList in the Canvas.
     */
    void Playback(Canvas& canvas, const Rect* rect = nullptr) const;

    /*
     * @brief  Gets the width of the DrawCmdList.
     */
    int GetWidth() const;

    /*
     * @brief  Gets the height of the DrawCmdList.
     */
    int GetHeight() const;

    /*
     * @brief  Sets the width of the DrawCmdList.
     */
    void SetWidth(int width);

    /*
     * @brief  Sets the height of the DrawCmdList.
     */
    void SetHeight(int height);

private:
    MemAllocator largeObjectAllocator_;
    int width_;
    int height_;
};

using DrawCmdListPtr = std::shared_ptr<DrawCmdList>;
} // namespace Drawing
} // namespace Rosen
} // namespace OHOS

#endif // DRAW_CMD_LIST_H
