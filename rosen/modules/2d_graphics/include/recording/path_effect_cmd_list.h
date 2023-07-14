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

#ifndef PATH_EFFECT_CMD_LILST_H
#define PATH_EFFECT_CMD_LILST_H

#include "effect/path_effect.h"
#include "recording/cmd_list.h"

namespace OHOS {
namespace Rosen {
namespace Drawing {
class PathEffectCmdList : public CmdList {
public:
    PathEffectCmdList() = default;
    ~PathEffectCmdList() override = default;

    uint32_t GetType() const override
    {
        return Type::PATH_EFFECT_CMD_LIST;
    }

    /*
     * @brief       Creates a PathEffectCmdList with contiguous buffers.
     * @param data  A contiguous buffsers.
     */
    static std::shared_ptr<PathEffectCmdList> CreateFromData(const CmdListData& data, bool isCopy = false);

    /*
     * @brief  Creates a PathEffect by the ShaderEffectCmdList playback operation.
     */
    std::shared_ptr<PathEffect> Playback() const;
};

/* OpItem */
class PathEffectOpItem : public OpItem {
public:
    explicit PathEffectOpItem(uint32_t type) : OpItem(type) {}
    ~PathEffectOpItem() = default;

    enum Type : uint32_t {
        OPITEM_HEAD = 0, // OPITEM_HEAD must be 0
        CREATE_DASH,
        CREATE_PATH_DASH,
        CREATE_CORNER,
        CREATE_SUM,
        CREATE_COMPOSE,
    };
};

class CreateDashPathEffectOpItem : public PathEffectOpItem {
public:
    CreateDashPathEffectOpItem(const std::pair<int32_t, size_t>& intervals, scalar phase);
    ~CreateDashPathEffectOpItem() = default;

    /*
     * @brief            Restores arguments from contiguous memory and plays back the OpItem to create PathEffect.
     * @param cmdList  A contiguous memory.
     */
    std::shared_ptr<PathEffect> Playback(const CmdList& cmdList) const;
private:
    std::pair<int32_t, size_t> intervals_;
    scalar phase_;
};

class CreatePathDashEffectOpItem : public PathEffectOpItem {
public:
    CreatePathDashEffectOpItem(const CmdListHandle& path, scalar advance, scalar phase, PathDashStyle style);
    ~CreatePathDashEffectOpItem() = default;

    /*
     * @brief            Restores arguments from contiguous memory and plays back the OpItem to create PathEffect.
     * @param cmdList  A contiguous memory.
     */
    std::shared_ptr<PathEffect> Playback(const CmdList& cmdList) const;
private:
    CmdListHandle path_;
    scalar advance_;
    scalar phase_;
    PathDashStyle style_;
};

class CreateCornerPathEffectOpItem : public PathEffectOpItem {
public:
    explicit CreateCornerPathEffectOpItem(scalar radius);
    ~CreateCornerPathEffectOpItem() = default;

    /*
     * @brief   Plays back the OpItem to create PathEffect.
     */
    std::shared_ptr<PathEffect> Playback() const;
private:
    scalar radius_;
};

class CreateSumPathEffectOpItem : public PathEffectOpItem {
public:
    CreateSumPathEffectOpItem(const CmdListHandle& effect1, const CmdListHandle& effect2);
    ~CreateSumPathEffectOpItem() = default;

    /*
     * @brief            Restores arguments from contiguous memory and plays back the OpItem to create PathEffect.
     * @param cmdList  A contiguous memory.
     */
    std::shared_ptr<PathEffect> Playback(const CmdList& cmdList) const;
private:
    CmdListHandle effect1_;
    CmdListHandle effect2_;
};

class CreateComposePathEffectOpItem : public PathEffectOpItem {
public:
    CreateComposePathEffectOpItem(const CmdListHandle& effect1, const CmdListHandle& effect2);
    ~CreateComposePathEffectOpItem() = default;

    /*
     * @brief            Restores arguments from contiguous memory and plays back the OpItem to create PathEffect.
     * @param cmdList  A contiguous memory.
     */
    std::shared_ptr<PathEffect> Playback(const CmdList& cmdList) const;
private:
    CmdListHandle effect1_;
    CmdListHandle effect2_;
};
} // namespace Drawing
} // namespace Rosen
} // namespace OHOS
#endif
