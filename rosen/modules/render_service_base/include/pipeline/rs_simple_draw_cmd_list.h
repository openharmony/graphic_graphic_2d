/*
 * Copyright (c) 2026 Huawei Device Co., Ltd.
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

#ifndef RENDER_SERVICE_BASE_PIPELINE_RS_SIMPLE_DRAW_CMD_LIST_H
#define RENDER_SERVICE_BASE_PIPELINE_RS_SIMPLE_DRAW_CMD_LIST_H

#include "recording/draw_cmd.h"
#include "recording/draw_cmd_list.h"

#include "common/rs_common_def.h"
#include "draw/canvas.h"
#include "utils/rect.h"

namespace OHOS {
namespace Rosen {
/**
 * @brief RSSimpleDrawCmdList stores fully unmarshalled drawing commands.
 *        All data is deserialized during construction, ready for direct use.
 *        This is the final state after IPC deserialization.
 */
class RSB_EXPORT RSSimpleDrawCmdList {
public:
    /**
     * @brief Create a RSSimpleDrawCmdList from Drawing::DrawCmdList
     */
    static std::shared_ptr<RSSimpleDrawCmdList> CreateFromDrawCmdList(Drawing::DrawCmdListPtr drawCmdList);
     
    /**
     * @brief Convert RSSimpleDrawCmdList to Drawing::DrawCmdList.
     */
    Drawing::DrawCmdListPtr ConvertToDrawCmdList() const;

    /**
     * @brief Default constructor.
     */
    RSSimpleDrawCmdList();

    /**
     * @brief Construct with op items.
     */
    RSSimpleDrawCmdList(int32_t width, int32_t height, std::vector<std::shared_ptr<Drawing::DrawOpItem>> opItems = {});

    /**
     * @brief Destroy a RSSimpleDrawCmdList
     */
    virtual ~RSSimpleDrawCmdList() = default;

    /**
     * @brief  Gets cmd list type.
     * @return Returns DRAW_CMD_LIST
     */
    virtual uint32_t GetType() const
    {
        return Drawing::CmdList::Type::DRAW_CMD_LIST;
    }

    /**
     * @brief Gets the width of the RSSimpleDrawCmdList.
     */
    virtual int32_t GetWidth() const
    {
        return width_;
    }

    /**
     * @brief Gets the height of the RSSimpleDrawCmdList.
     */
    virtual int32_t GetHeight() const
    {
        return height_;
    }

    /**
     * @brief Gets draw op items (thread-safe copy).
     */
    std::vector<std::shared_ptr<Drawing::DrawOpItem>> GetDrawOpItems() const;

    /**
     * @brief Check if empty (thread-safe).
     */
    virtual bool IsEmpty() const;

    /**
     * @brief Calls the corresponding operations of all opitems in RSSimpleDrawCmdList to the canvas.
     * @param canvas Implements the playback action of the RSSimpleDrawCmdList in the Canvas.
     * @param rect Rect used to playback, may be nullptr.
     */
    virtual void Playback(Drawing::Canvas& canvas, const Drawing::Rect* rect = nullptr);

    /**
     * @brief Dump for debugging.
     */
    void Dump(std::string& out);

    /**
     * @brief Get total memory size.
     */
    size_t GetSize() const;

    /**
     * @brief Get op vector size
     */
    size_t GetOpItemSize() const;

    /**
     * @brief Get cmdlist draw region from opItem.
     */
    Drawing::RectF GetCmdlistDrawRegion();

    /**
     * @brief Clear all DrawOpItem
     */
    void ClearOp();

    /**
     * @brief Update node id to picture for all draw op items.
     */
    void UpdateNodeIdToPicture(NodeId nodeId);

    /**
     * @brief Purge cached resources.
     */
    void Purge();

    /**
     * @brief Add DrawOpItem to RSSimpleDrawCmdList
     * @param drawOpItem A real DrawOpItem instance
     */
    void AddDrawOp(std::shared_ptr<Drawing::DrawOpItem>&& drawOpItem);

private:
    void PlaybackByVector(Drawing::Canvas& canvas, const Drawing::Rect* rect = nullptr);

    int32_t width_ = 0;

    int32_t height_ = 0;

    bool noNeedUICaptured_ = false;

    std::vector<std::shared_ptr<Drawing::DrawOpItem>> drawOpItems_;

    mutable std::recursive_mutex mutex_;
};

using SimpleDrawCmdListPtr = std::shared_ptr<RSSimpleDrawCmdList>;
} // namespace Rosen
} // namespace OHOS
#endif // RENDER_SERVICE_BASE_PIPELINE_RS_SIMPLE_DRAW_CMD_LIST_H