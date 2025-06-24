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

#ifndef RS_OPINC_CACHE_H
#define RS_OPINC_CACHE_H

#include "common/rs_common_def.h"
#include "common/rs_macros.h"

namespace OHOS {
namespace Rosen {
class RSB_EXPORT RSOpincCache {
public:
    RSOpincCache() = default;
    RSOpincCache(const RSOpincCache&) = delete;
    RSOpincCache(const RSOpincCache&&) = delete;
    RSOpincCache& operator=(const RSOpincCache&) = delete;
    RSOpincCache& operator=(const RSOpincCache&&) = delete;

    ~RSOpincCache() = default;

    // mark stable node
    void OpincSetInAppStateStart(bool& unchangeMarkInApp);
    void OpincSetInAppStateEnd(bool& unchangeMarkInApp);
    void OpincQuickMarkStableNode(bool& unchangeMarkInApp, bool& unchangeMarkEnable, bool isSelfDirty);
    bool IsOpincUnchangeState();

    bool IsMarkedRenderGroup(bool groupTypeNotNone);
    bool OpincForcePrepareSubTree(bool autoCacheEnable, bool isDirty, bool supportFlag);

    // sync to drawable
    void OpincUpdateRootFlag(bool& unchangeMarkEnable, bool isOpincNodeSupportFlag);
    bool OpincGetRootFlag() const;

    // arkui mark
    void MarkSuggestOpincNode(bool isOpincNode, bool isNeedCalculate);
    bool IsSuggestOpincNode() const;

    bool GetCacheChangeFlag() const;
    NodeCacheState GetNodeCacheState() const
    {
        return nodeCacheState_;
    }

    int GetUnchangeCount() const
    {
        return unchangeCount_;
    }

    bool OpincGetSupportFlag() const
    {
        return isOpincSupportFlag_;
    }

    void OpincSetSupportFlag(bool supportFlag)
    {
        isOpincSupportFlag_ = supportFlag;
    }

private:
    // opinc state
    NodeCacheState nodeCacheState_ = NodeCacheState::STATE_INIT;
    bool isSuggestOpincNode_ = false;
    bool isOpincSupportFlag_ = true;
    bool isOpincRootFlag_ = false;
    bool isUnchangeMarkEnable_ = false;
    bool isNeedCalculate_ = false;
    bool isUnchangeMarkInApp_ = false;

    int tryCacheTimes_ = 0;
    int unchangeCount_ = 0;
    int unchangeCountUpper_ = 3; // 3 time is the default to cache
    bool cacheChangeFlag_ = false;
    int waitCount_ = 0;
    // opinc state func
    void NodeCacheStateChange(NodeChangeType type);
    void SetCacheStateByRetrytime();
    void NodeCacheStateReset(NodeCacheState nodeCacheState);
};
} // namespace Rosen
} // namespace OHOS

#endif // RS_OPINC_CACHE_H