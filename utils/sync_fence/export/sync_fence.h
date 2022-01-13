/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
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

#ifndef UTILS_INCLUDE_SYNC_FENCE_H
#define UTILS_INCLUDE_SYNC_FENCE_H

#include <string>
#include <refbase.h>
#include <unique_fd.h>

namespace OHOS {

class SyncFence : public RefBase {
public:
    explicit SyncFence(int32_t fenceFd);
    /* When the SyncFence is destroyed, the fd will be closed in UniqueFd */
    virtual ~SyncFence();

    SyncFence(const SyncFence& rhs) = delete;
    SyncFence& operator=(const SyncFence& rhs) = delete;
    SyncFence(SyncFence&& rhs) = delete;
    SyncFence& operator=(SyncFence&& rhs) = delete;

    static const sptr<SyncFence> INVALID_FENCE;
    int32_t Wait(uint32_t timeout);
    static sptr<SyncFence> MergeFence(const std::string &name,
            const sptr<SyncFence>& fence1, const sptr<SyncFence>& fence2);
    int64_t SyncFileReadTimestamp();
    int32_t Dup() const;

    /* this is dangerous, when you use it, do not operator the fd */
    int32_t Get() const;

private:
    struct SyncFenceInfo {
        char objName_[32];
        char driverName_[32];
        int32_t status_;
        uint32_t flags_;
        uint64_t timestampNs_;
    };

    struct SyncFileInfo {
        char name_[32];
        int32_t status_;
        uint32_t flags_;
        uint32_t numFences_;
        uint32_t pad_;
        uint64_t syncFenceInfo_;
    };

    UniqueFd fenceFd_;
};

}

#endif // UTILS_INCLUDE_SYNC_FENCE_H