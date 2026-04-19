/*
 * Copyright (c) 2025 Huawei Device Co., Ltd.
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

#ifndef RS_RENDER_GROUP_CACHE_H
#define RS_RENDER_GROUP_CACHE_H

#include "common/rs_common_def.h"
#include "common/rs_macros.h"

namespace OHOS {
namespace Rosen {

class RSB_EXPORT RSRenderGroupCache {
public:
    RSRenderGroupCache() = default;
    ~RSRenderGroupCache() = default;

    RSRenderGroupCache(const RSRenderGroupCache& other) = default;
    RSRenderGroupCache(RSRenderGroupCache&& other) = default;
    RSRenderGroupCache& operator=(const RSRenderGroupCache& other) = default;
    RSRenderGroupCache& operator=(RSRenderGroupCache&& other) = default;

    bool ExcludedFromNodeGroup(bool isExcluded);
    bool IsExcludedFromNodeGroup() const
    {
        return excludedFromNodeGroup_;
    }

    bool SetHasChildExcludedFromNodeGroup(bool isExcluded);
    bool HasChildExcludedFromNodeGroup() const
    {
        return hasChildExcludedFromNodeGroup_;
    }

    bool SetRenderGroupExcludedStateChanged(bool isChanged);
    bool IsRenderGroupExcludedStateChanged() const
    {
        return isExcludedStateChanged_;
    }

    bool SetCachedSubTreeDirty(bool isDirty);
    bool IsCachedSubTreeDirty() const
    {
        return isCachedSubTreeDirty_;
    }

    bool SetChildHasTranslateOnSqueeze(bool val);
    bool ChildHasTranslateOnSqueeze() const
    {
        return childHasTranslateOnSqueeze_;
    }

    bool SetNeedClipHoleForFilter(bool val);
    bool NeedClipHoleForFilter() const
    {
        return needClipHoleForFilter_;
    }

    enum class RSFreezeFlag : uint8_t {
        NONE = 0,
        FREEZED_BY_UI = 1 << NONE,
        FREEZED_BY_USER = 1 << FREEZED_BY_UI,
    };

    bool SetRSFreezeFlag(bool freezeFlag, bool isMarkedByUI);
    RSFreezeFlag GetRSFreezeFlag() const
    {
        return freezeFlag_;
    }
    // operator overloading for the RSFreezeFlag enumeration
    friend constexpr RSFreezeFlag operator|(RSFreezeFlag lhs, RSFreezeFlag rhs) noexcept;
    friend constexpr RSFreezeFlag operator&(RSFreezeFlag lhs, RSFreezeFlag rhs) noexcept;
    friend constexpr RSFreezeFlag operator~(RSFreezeFlag val) noexcept;
    friend inline RSFreezeFlag& operator|=(RSFreezeFlag& lhs, RSFreezeFlag rhs) noexcept;
    friend inline RSFreezeFlag& operator&=(RSFreezeFlag& lhs, RSFreezeFlag rhs) noexcept;

private:
    bool excludedFromNodeGroup_ = false;
    bool hasChildExcludedFromNodeGroup_ = false;
    bool isExcludedStateChanged_ = false;
    bool isCachedSubTreeDirty_ = false;
    bool childHasTranslateOnSqueeze_ = false;
    bool needClipHoleForFilter_ = false;
    RSFreezeFlag freezeFlag_ = RSFreezeFlag::NONE;
};

constexpr RSRenderGroupCache::RSFreezeFlag operator|(
    RSRenderGroupCache::RSFreezeFlag lhs, RSRenderGroupCache::RSFreezeFlag rhs) noexcept
{
    return static_cast<RSRenderGroupCache::RSFreezeFlag>(
        static_cast<std::underlying_type_t<RSRenderGroupCache::RSFreezeFlag>>(lhs) |
        static_cast<std::underlying_type_t<RSRenderGroupCache::RSFreezeFlag>>(rhs));
}
constexpr RSRenderGroupCache::RSFreezeFlag operator&(
    RSRenderGroupCache::RSFreezeFlag lhs, RSRenderGroupCache::RSFreezeFlag rhs) noexcept
{
    return static_cast<RSRenderGroupCache::RSFreezeFlag>(
        static_cast<std::underlying_type_t<RSRenderGroupCache::RSFreezeFlag>>(lhs) &
        static_cast<std::underlying_type_t<RSRenderGroupCache::RSFreezeFlag>>(rhs));
}
constexpr RSRenderGroupCache::RSFreezeFlag operator~(RSRenderGroupCache::RSFreezeFlag val) noexcept
{
    return static_cast<RSRenderGroupCache::RSFreezeFlag>(
        ~static_cast<std::underlying_type_t<RSRenderGroupCache::RSFreezeFlag>>(val));
}
RSRenderGroupCache::RSFreezeFlag& operator|=(
    RSRenderGroupCache::RSFreezeFlag& lhs, RSRenderGroupCache::RSFreezeFlag rhs) noexcept
{
    lhs = lhs | rhs;
    return lhs;
}
RSRenderGroupCache::RSFreezeFlag& operator&=(
    RSRenderGroupCache::RSFreezeFlag& lhs, RSRenderGroupCache::RSFreezeFlag rhs) noexcept
{
    lhs = lhs & rhs;
    return lhs;
}

class RSB_EXPORT AutoRenderGroupExcludedSubTreeGuard {
public:
    AutoRenderGroupExcludedSubTreeGuard(NodeId& curExcludedRootNodeId, bool isCurNodeExcluded, NodeId curNodeId);
    ~AutoRenderGroupExcludedSubTreeGuard();

    AutoRenderGroupExcludedSubTreeGuard() = delete;
    AutoRenderGroupExcludedSubTreeGuard(const AutoRenderGroupExcludedSubTreeGuard& other) = delete;
    AutoRenderGroupExcludedSubTreeGuard(AutoRenderGroupExcludedSubTreeGuard&& other) = delete;
    AutoRenderGroupExcludedSubTreeGuard& operator=(const AutoRenderGroupExcludedSubTreeGuard& other) = delete;
    AutoRenderGroupExcludedSubTreeGuard& operator=(AutoRenderGroupExcludedSubTreeGuard&& other) = delete;

private:
    NodeId& curExcludedRootNodeId_;
    bool isExcluded_ = false;
};
} // namespace Rosen
} // namespace OHOS
#endif // RS_RENDER_GROUP_CACHE_H