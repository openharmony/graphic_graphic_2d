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

#ifndef SHADER_EFFECT_CMD_LILST_H
#define SHADER_EFFECT_CMD_LILST_H

#include "effect/shader_effect.h"
#include "recording/cmd_list.h"

namespace OHOS {
namespace Rosen {
namespace Drawing {
class ShaderEffectCmdList : public CmdList {
public:
    ShaderEffectCmdList() = default;
    ~ShaderEffectCmdList() override = default;

    /*
     * @brief       Creates a ShaderEffectCmdList with contiguous buffers.
     * @param data  A contiguous buffers.
     */
    static std::shared_ptr<ShaderEffectCmdList> CreateFromData(const CmdListData& data);

    /*
     * @brief                   Creates a ShaderEffectCmdList with contiguous buffers and large memory object.
     * @param data              A contiguous buffers.
     * @param LargeObjectData   A large memory object buffers.
     */
    static std::shared_ptr<ShaderEffectCmdList> CreateFromData(
        const CmdListData& data, const LargeObjectData& LargeObjectData);

    /*
     * using for recording, should to remove after using shared memory
     * @brief       Add large object data to the buffers of ShaderEffectCmdList.
     * @param data  A large object data.
     * @return      Returns the offset of the contiguous buffers and ShaderEffectCmdList head point.
     */
    int AddLargeObject(const LargeObjectData& data);

    /*
     * @brief   Gets the large Object buffers of the ShaderEffectCmdList.
     */
    LargeObjectData GetLargeObjectData() const;

    /*
     * @brief   Create a ShaderEffect by the ShaderEffectCmdList playback operation.
     */
    std::shared_ptr<ShaderEffect> Playback() const;

private:
    MemAllocator largeObjectAllocator_;
};

/* OpItem */
class ShaderEffectOpItem : public OpItem {
public:
    ShaderEffectOpItem(uint32_t type) : OpItem(type) {}
    ~ShaderEffectOpItem() = default;

    enum Type : uint32_t {
        OPITEM_HEAD = 0,    // OPITEM_HEAD must be 0
        CREATE_COLOR_SHADER,
        CREATE_BLEND_SHADER,
        CREATE_IMAGE_SHADER,
        CREATE_PICTURE_SHADER,
        CREATE_LINEAR_GRADIENT,
        CREATE_RADIAL_GRADIENT,
        CREATE_TWO_POINT_CONICAL,
        CREATE_SWEEP_GRADIENT
    };
};

class CreateColorShaderOpItem : public ShaderEffectOpItem {
public:
    CreateColorShaderOpItem(ColorQuad color);
    ~CreateColorShaderOpItem() = default;

    std::shared_ptr<ShaderEffect> Playback() const;
private:
    ColorQuad color_;
};
} // namespace Drawing
} // namespace Rosen
} // namespace OHOS
#endif
