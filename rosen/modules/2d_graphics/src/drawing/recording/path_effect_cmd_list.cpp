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

#include "recording/path_effect_cmd_list.h"

#include "recording/path_cmd_list.h"
#include "utils/log.h"

namespace OHOS {
namespace Rosen {
namespace Drawing {
std::shared_ptr<PathEffectCmdList> PathEffectCmdList::CreateFromData(const CmdListData& data)
{
    auto cmdList = std::make_shared<PathEffectCmdList>();
    if (cmdList == nullptr) {
        LOGE("PathEffectCmdList create from data failed!");
        return nullptr;
    }
    cmdList->opAllocator_.BuildFromData(data.first, data.second);
    return cmdList;
}

std::shared_ptr<PathEffect> PathEffectCmdList::Playback() const
{
    int offset = 0;
    std::shared_ptr<PathEffect> pe = nullptr;

    do {
        OpItem* itemPtr = static_cast<OpItem*>(opAllocator_.OffsetToAddr(offset));
        if (itemPtr == nullptr) {
            LOGE("PathEffectCmdList Playback failed!");
            break;
        }

        switch (itemPtr->GetType()) {
            case PathEffectOpItem::CREATE_DASH:
                pe = static_cast<CreateDashPathEffectOpItem*>(itemPtr)->Playback(opAllocator_);
                break;
            case PathEffectOpItem::CREATE_PATH_DASH:
                pe = static_cast<CreatePathDashEffectOpItem*>(itemPtr)->Playback(opAllocator_);
                break;
            case PathEffectOpItem::CREATE_CORNER:
                pe = static_cast<CreateCornerPathEffectOpItem*>(itemPtr)->Playback();
                break;
            case PathEffectOpItem::CREATE_SUM:
                pe = static_cast<CreateSumPathEffectOpItem*>(itemPtr)->Playback(opAllocator_);
                break;
            case PathEffectOpItem::CREATE_COMPOSE:
                pe = static_cast<CreateComposePathEffectOpItem*>(itemPtr)->Playback(opAllocator_);
                break;
            default:
                LOGE("PathEffectCmdList unknown OpItem type!");
                break;
        }
        offset = itemPtr->GetNextOpItemOffset();
    } while (offset != 0);

    return pe;
}

/* OpItem */
CreateDashPathEffectOpItem::CreateDashPathEffectOpItem(const std::pair<int, size_t>& intervals, scalar phase)
    : PathEffectOpItem(CREATE_DASH), intervals_(intervals), phase_(phase) {}

std::shared_ptr<PathEffect> CreateDashPathEffectOpItem::Playback(const MemAllocator& allocator) const
{
    auto* intervals = static_cast<scalar*>(allocator.OffsetToAddr(intervals_.first));
    int count = intervals_.second / sizeof(scalar);

    return PathEffect::CreateDashPathEffect(intervals, count, phase_);
}

CreatePathDashEffectOpItem::CreatePathDashEffectOpItem(
    const CmdListSiteInfo& path, scalar advance, scalar phase, PathDashStyle style)
    : PathEffectOpItem(CREATE_PATH_DASH), path_(path), advance_(advance), phase_(phase), style_(style) {}

std::shared_ptr<PathEffect> CreatePathDashEffectOpItem::Playback(const MemAllocator& allocator) const
{
    void* ptr = allocator.OffsetToAddr(path_.first);
    if (!ptr) {
        return nullptr;
    }
    auto cmdList = std::make_shared<PathCmdList>(std::make_pair(ptr, path_.second));
    Path path;
    cmdList->Playback(path);

    return PathEffect::CreatePathDashEffect(path, advance_, phase_, style_);
}

CreateCornerPathEffectOpItem::CreateCornerPathEffectOpItem(scalar radius)
    : PathEffectOpItem(CREATE_CORNER), radius_(radius) {}

std::shared_ptr<PathEffect> CreateCornerPathEffectOpItem::Playback() const
{
    return PathEffect::CreateCornerPathEffect(radius_);
}

CreateSumPathEffectOpItem::CreateSumPathEffectOpItem(const CmdListSiteInfo& effect1, const CmdListSiteInfo& effect2)
    : PathEffectOpItem(CREATE_SUM), effect1_(effect1), effect2_(effect2) {}

std::shared_ptr<PathEffect> CreateSumPathEffectOpItem::Playback(const MemAllocator& allocator) const
{
    void* ptr = allocator.OffsetToAddr(effect1_.first);
    auto cmdList = PathEffectCmdList::CreateFromData({ ptr, effect1_.second });
    if (cmdList == nullptr) {
        return nullptr;
    }
    auto effect1 = cmdList->Playback();

    ptr = allocator.OffsetToAddr(effect2_.first);
    cmdList = PathEffectCmdList::CreateFromData({ ptr, effect2_.second });
    if (cmdList == nullptr) {
        return nullptr;
    }
    auto effect2 = cmdList->Playback();
    if (effect1 == nullptr || effect2 == nullptr) {
        return nullptr;
    }
    return PathEffect::CreateSumPathEffect(*effect1, *effect2);
}

CreateComposePathEffectOpItem::CreateComposePathEffectOpItem(
    const CmdListSiteInfo& effect1, const CmdListSiteInfo& effect2)
    : PathEffectOpItem(CREATE_COMPOSE), effect1_(effect1), effect2_(effect2) {}

std::shared_ptr<PathEffect> CreateComposePathEffectOpItem::Playback(const MemAllocator& allocator) const
{
    void* ptr = allocator.OffsetToAddr(effect1_.first);
    auto cmdList = PathEffectCmdList::CreateFromData({ ptr, effect1_.second });
    if (cmdList == nullptr) {
        return nullptr;
    }
    auto effect1 = cmdList->Playback();

    ptr = allocator.OffsetToAddr(effect2_.first);
    cmdList = PathEffectCmdList::CreateFromData({ ptr, effect2_.second });
    if (cmdList == nullptr) {
        return nullptr;
    }
    auto effect2 = cmdList->Playback();
    if (effect1 == nullptr || effect2 == nullptr) {
        return nullptr;
    }
    return PathEffect::CreateComposePathEffect(*effect1, *effect2);
}
} // namespace Drawing
} // namespace Rosen
} // namespace OHOS
