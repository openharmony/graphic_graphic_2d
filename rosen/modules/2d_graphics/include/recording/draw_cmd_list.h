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
class DrawOpItem;
class DRAWING_API DrawCmdList : public CmdList {
public:
    DrawCmdList() = default;
    DrawCmdList(int32_t width, int32_t height);
    ~DrawCmdList() override = default;

    uint32_t GetType() const override
    {
        return Type::DRAW_CMD_LIST;
    }

    /*
     * @brief         Clear Draw Ops Param
     */
    void ClearOp();

    size_t GetOpItemSize() const;

    std::string GetOpsWithDesc() const;

    /*
     * @brief       Creates a DrawCmdList with contiguous buffers.
     * @param data  A contiguous buffers.
     */
    static std::shared_ptr<DrawCmdList> CreateFromData(const CmdListData& data, bool isCopy = false);

    /*
     * @brief         Unmarshalling Draw Ops Param from contiguous buffers
     */
    void UnmarshallingOps();

    /*
     * @brief         Draw cmd is empty or not.
     */
    bool IsEmpty() const;

    /*
     * @brief         Calls the corresponding operations of all opitems in DrawCmdList to the canvas.
     * @param canvas  Implements the playback action of the DrawCmdList in the Canvas.
     */
    void Playback(Canvas& canvas, const Rect* rect = nullptr);

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

    void GenerateCache(Canvas* canvas = nullptr, const Rect* rect = nullptr);

    bool GetIsCache();

    void SetIsCache(bool isCached);

    bool GetCachedHighContrast();

    void SetCachedHighContrast(bool cachedHighContrast);

    std::vector<std::pair<uint32_t, uint32_t>> GetReplacedOpList();

    void SetReplacedOpList(std::vector<std::pair<uint32_t, uint32_t>> replacedOpList);

    std::vector<std::shared_ptr<DrawOpItem>> UnmarshallingCmdList();

    void UpdateNodeIdToPicture(NodeId nodeId);

private:
    void GenerateCacheInRenderService(Canvas* canvas, const Rect* rect);

    void ClearCache();

    void AddOpToCmdList(std::shared_ptr<DrawCmdList> cmdList);

private:
    MemAllocator largeObjectAllocator_;
    std::vector<std::shared_ptr<DrawOpItem>> unmarshalledOpItems_;
    size_t lastOpGenSize_ = 0;
    int32_t width_;
    int32_t height_;
    std::vector<std::pair<uint32_t, uint32_t>> replacedOpList_;
    std::vector<std::pair<uint32_t, std::shared_ptr<DrawOpItem>>> opReplacedByDrivenRender_;
    bool isCached_ = false;
    bool cachedHighContrast_ = false;
};

using DrawCmdListPtr = std::shared_ptr<DrawCmdList>;
} // namespace Drawing
} // namespace Rosen
} // namespace OHOS

#endif // DRAW_CMD_LIST_H
