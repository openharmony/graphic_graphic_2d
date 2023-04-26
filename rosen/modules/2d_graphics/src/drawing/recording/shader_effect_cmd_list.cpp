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

#include "recording/shader_effect_cmd_list.h"

#include "image/image.h"
#include "utils/log.h"

namespace OHOS {
namespace Rosen {
namespace Drawing {
std::shared_ptr<ShaderEffectCmdList> ShaderEffectCmdList::CreateFromData(const CmdListData& data)
{
    auto cmdList = std::make_shared<ShaderEffectCmdList>();
    if (cmdList == nullptr) {
        LOGE("ShaderEffectCmdList create from data failed!");
        return nullptr;
    }
    cmdList->opAllocator_.BuildFromData(data.first, data.second);
    return cmdList;
}

std::shared_ptr<ShaderEffectCmdList> ShaderEffectCmdList::CreateFromData(
    const CmdListData& data, const LargeObjectData& largeObjectData)
{
    auto cmdList = std::make_shared<ShaderEffectCmdList>();
    if (cmdList == nullptr) {
        LOGE("ShaderEffectCmdList create from data failed!");
        return nullptr;
    }
    cmdList->opAllocator_.BuildFromData(data.first, data.second);

    if (largeObjectData.second) {
        cmdList->largeObjectAllocator_.BuildFromData(largeObjectData.first, largeObjectData.second);
    }

    return cmdList;
}

int ShaderEffectCmdList::AddLargeObject(const LargeObjectData& data)
{
    std::lock_guard<std::mutex> lock(mutex_);
    void* dst = largeObjectAllocator_.Add(data.first, data.second);
    if (dst == nullptr) {
        LOGE("ShaderEffectCmdList AddLargeObject failed!");
        return 0;
    }
    return largeObjectAllocator_.AddrToOffset(dst);
}

LargeObjectData ShaderEffectCmdList::GetLargeObjectData() const
{
    const void* data = largeObjectAllocator_.GetData();
    size_t size = largeObjectAllocator_.GetSize();
    return std::pair<const void*, size_t>(data, size);
}

std::shared_ptr<ShaderEffect> ShaderEffectCmdList::Playback() const
{
    int offset = 0;
    std::shared_ptr<ShaderEffect> se = nullptr;

    do {
        OpItem* itemPtr = static_cast<OpItem*>(opAllocator_.OffsetToAddr(offset));
        if (itemPtr != nullptr) {
            switch (itemPtr->GetType()) {
                case ShaderEffectOpItem::CREATE_COLOR_SHADER:
                    se = static_cast<CreateColorShaderOpItem*>(itemPtr)->Playback();
                    break;
                case ShaderEffectOpItem::CREATE_BLEND_SHADER:
                    se = static_cast<CreateBlendShaderOpItem*>(itemPtr)->Playback(opAllocator_);
                    break;
                case ShaderEffectOpItem::CREATE_IMAGE_SHADER:
                    se = static_cast<CreateImageShaderOpItem*>(itemPtr)->Playback(largeObjectAllocator_);
                    break;
                case ShaderEffectOpItem::CREATE_PICTURE_SHADER:
                    se = static_cast<CreatePictureShaderOpItem*>(itemPtr)->Playback(largeObjectAllocator_);
                    break;
                case ShaderEffectOpItem::CREATE_LINEAR_GRADIENT:
                    se = static_cast<CreateLinearGradientOpItem*>(itemPtr)->Playback(opAllocator_);
                    break;
                case ShaderEffectOpItem::CREATE_RADIAL_GRADIENT:
                    se = static_cast<CreateRadialGradientOpItem*>(itemPtr)->Playback(opAllocator_);
                    break;
                case ShaderEffectOpItem::CREATE_TWO_POINT_CONICAL:
                    se = static_cast<CreateTwoPointConicalOpItem*>(itemPtr)->Playback(opAllocator_);
                    break;
                case ShaderEffectOpItem::CREATE_SWEEP_GRADIENT:
                    se = static_cast<CreateSweepGradientOpItem*>(itemPtr)->Playback(opAllocator_);
                    break;
                default:
                    LOGE("ShaderEffectCmdList unknown OpItem type!");
                    break;
            }
            offset = itemPtr->GetNextOpItemOffset();
        } else {
            LOGE("ShaderEffectCmdList Playback failed!");
            break;
        }
    } while (offset != 0);

    return se;
}

/* OpItem */
CreateColorShaderOpItem::CreateColorShaderOpItem(ColorQuad color)
    : ShaderEffectOpItem(CREATE_COLOR_SHADER), color_(color) {}

std::shared_ptr<ShaderEffect> CreateColorShaderOpItem::Playback() const
{
    return ShaderEffect::CreateColorShader(color_);
}

} // namespace Drawing
} // namespace Rosen
} // namespace OHOS
