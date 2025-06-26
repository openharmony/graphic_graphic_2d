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
#ifndef RENDER_SERVICE_BASE_RS_HPAE_SCHEDULER_H
#define RENDER_SERVICE_BASE_RS_HPAE_SCHEDULER_H

#include <condition_variable>
#include <deque>
#include <map>
#include <mutex>
#include <vector>
#include "hpae_base/rs_hpae_base_types.h"

namespace OHOS::Rosen {

class RSB_EXPORT RSHpaeScheduler {
public:
    RSHpaeScheduler(const RSHpaeScheduler &) = delete;
    RSHpaeScheduler &operator=(const RSHpaeScheduler &) = delete;
    static RSHpaeScheduler &GetInstance();

    // call from HpaeSubThread
    void Reset();
    void CacheHpaeItem(const HpaeBackgroundCacheItem& item);
    HpaeBackgroundCacheItem GetCachedHpaeItem();

    // call from graphic pattern
    void SetHpaeFrameId(uint64_t frameId);
    uint64_t GetHpaeFrameId() const;

    void SetToWaitBuildTask();
    void NotifyBuildTaskDone();
    void WaitBuildTask();

private:
    RSHpaeScheduler() = default;
    ~RSHpaeScheduler() = default;

    mutable std::mutex mutex_; // main mutex for public API
    HpaeBackgroundCacheItem cachedItem_; // item to submit for current frame
    uint64_t hpaeFrameId_ = 0; // frameId for background (preFrameId)

    std::mutex buildTaskMutex_;
    bool buildTaskDone_ = true;
    std::condition_variable buildTaskCv_;
};

} // OHOS::Rosen

#endif // RENDER_SERVICE_BASE_RS_HPAE_SCHEDULER_H