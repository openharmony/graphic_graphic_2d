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

#ifndef MASK_CMD_LIST_H
#define MASK_CMD_LIST_H

#include "recording/cmd_list.h"
#include "draw_cmd.h"
#include "recording/recording_handle.h"
#include "draw/path.h"
#include "draw/brush.h"

namespace OHOS {
namespace Rosen {
namespace Drawing {
class DRAWING_API MaskCmdList : public CmdList {
public:
    MaskCmdList() = default;
    ~MaskCmdList() override = default;

    uint32_t GetType() const override
    {
        return Type::MASK_CMD_LIST;
    }

    /**
     * @brief         Creates a MaskCmdList with contiguous buffers.
     * @param data    A contiguous buffers.
     * @param isCopy  Whether to copy data or not.
     */
    static std::shared_ptr<MaskCmdList> CreateFromData(const CmdListData& data, bool isCopy = false);

    /**
     * @brief  Calls the corresponding operations of all opitems in MaskCmdList to the mask.
     */
    bool Playback(std::shared_ptr<Path>& path, Brush& brush) const;

    bool Playback(std::shared_ptr<Path>& path, Pen& pen, Brush& brush) const;
};

/* OpItem */
/**
 * @brief  Helper class for mask playback.
 *         Contains the playback context and a static mapping table: { OpItemType， OpItemPlaybackFunc }.
 */
class MaskPlayer {
public:
    MaskPlayer(std::shared_ptr<Path>& path, Brush& brush, const CmdList& cmdList);
    MaskPlayer(std::shared_ptr<Path>& path, Brush& brush, Pen& pen, const CmdList& cmdList);
    ~MaskPlayer() = default;

    /**
     * @brief  Obtain the corresponding func according to the type lookup mapping table
     *         and then invoke the func to plays opItem back to mask which in context.
     */
    bool Playback(uint32_t type, const void* opItem);

    std::shared_ptr<Path>& path_;
    Brush& brush_;
    Pen& pen_;
    const CmdList& cmdList_;

    using MaskPlaybackFunc = void(*)(MaskPlayer& palyer, const void* opItem);
private:
    static std::unordered_map<uint32_t, MaskPlaybackFunc> opPlaybackFuncLUT_;
};

class MaskOpItem : public OpItem {
public:
    MaskOpItem(uint32_t type) : OpItem(type) {}
    ~MaskOpItem() = default;

    enum Type : uint32_t {
        OPITEM_HEAD,
        MASK_BRUSH_OPITEM,
        MASK_PATH_OPITEM,
        MASK_PEN_OPITEM
    };
};

class MaskBrushOpItem : public MaskOpItem {
public:
    explicit MaskBrushOpItem(const BrushHandle& brushHandle);
    ~MaskBrushOpItem() = default;

    static void Playback(MaskPlayer& player, const void* opItem);

    void Playback(Brush& brush, const CmdList& cmdList) const;
private:
    BrushHandle brushHandle_;
};

class MaskPenOpItem : public MaskOpItem {
public:
    explicit MaskPenOpItem(const PenHandle& penHandle);
    ~MaskPenOpItem() = default;

    static void Playback(MaskPlayer& player, const void* opItem);

    void Playback(Pen& pen, const CmdList& cmdList) const;
private:
    PenHandle penHandle_;
};

class MaskPathOpItem : public MaskOpItem {
public:
    explicit MaskPathOpItem(const OpDataHandle& pathHandle);
    ~MaskPathOpItem() = default;

    static void Playback(MaskPlayer& player, const void* opItem);

    void Playback(std::shared_ptr<Path>& path, const CmdList& cmdList) const;
private:
    OpDataHandle pathHandle_;
};
} // namespace Drawing
} // namespace Rosen
} // namespace OHOS

#endif
