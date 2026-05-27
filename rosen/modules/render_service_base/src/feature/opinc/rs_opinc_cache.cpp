/*
 * Copyright (c) 2024-2026 Huawei Device Co., Ltd.
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

#include "feature/opinc/rs_opinc_cache.h"

namespace OHOS {
namespace Rosen {
RSOpincCache::RSOpincCache() = default;

RSOpincCache::~RSOpincCache() = default;

bool RSOpincCache::GetSubTreeSupportFlag() const
{
    return subTreeSupportFlag_;
}

void RSOpincCache::SetSubTreeSupportFlag(bool supportFlag)
{
    subTreeSupportFlag_ = supportFlag;
}

void RSOpincCache::UpdateSubTreeSupportFlag(bool childSupportFlag, bool childRootFlag, bool groupTypeIsNone)
{
    subTreeSupportFlag_ = subTreeSupportFlag_ && childSupportFlag && !childRootFlag && groupTypeIsNone;
}

bool RSOpincCache::GetCurNodeTreeSupportFlag() const
{
    return curNodeTreeSupportFlag_;
}

void RSOpincCache::SetCurNodeTreeSupportFlag(bool curNodeTreeSupportFlag)
{
    curNodeTreeSupportFlag_ = curNodeTreeSupportFlag;
}

bool RSOpincCache::HasUnstableOpincNode() const
{
    return hasUnstableOpincNode_;
}

void RSOpincCache::SetHasUnstableOpincNode(bool hasUnstableOpincNode)
{
    hasUnstableOpincNode_ = hasUnstableOpincNode;
}

void RSOpincCache::UpdateSubTreeHasUnstableOpincNode(RSOpincCache& childOpincCache,
    bool childRootHasUnstableOpincNode)
{
    childOpincCache.SetHasUnstableOpincNode(
        childOpincCache.HasUnstableOpincNode() || childRootHasUnstableOpincNode);
    SetHasUnstableOpincNode(HasUnstableOpincNode() || childOpincCache.HasUnstableOpincNode());
}

void RSOpincCache::OpincSetInAppStateStart(bool& unchangeMarkInApp)
{
    if (unchangeMarkInApp) {
        return;
    }
    isUnchangeMarkInApp_ = true;
    unchangeMarkInApp = true;
}

void RSOpincCache::OpincSetInAppStateEnd(bool& unchangeMarkInApp)
{
    if (isUnchangeMarkInApp_) {
        isUnchangeMarkInApp_ = false;
        unchangeMarkInApp = false;
    }
}

void RSOpincCache::MarkMaterialNode(bool isMaterialNode)
{
    isMaterialNode_ = isMaterialNode;
}

bool RSOpincCache::IsMaterialNode() const
{
    return isMaterialNode_;
}

void RSOpincCache::SetLayerPartRenderDirtyFlag(bool dirtyFlag)
{
    layerPartRenderDirtyFlag_ = dirtyFlag;
}

bool RSOpincCache::GetLayerPartRenderDirtyFlag() const
{
    return layerPartRenderDirtyFlag_;
}

void RSOpincCache::SetLayerPartRenderOldAbsDrawRect(const RectI& oldAbsDrawRect)
{
    oldAbsDrawRect_ = oldAbsDrawRect;
}

const RectI& RSOpincCache::GetLayerPartRenderOldAbsDrawRect() const
{
    return oldAbsDrawRect_;
}
} // namespace Rosen
} // namespace OHOS
