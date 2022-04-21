/*
 * Copyright (c) 2021-2022 Huawei Device Co., Ltd.
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

#include "sync_fence.h"

#include <cstddef>
#include <cstdint>
#include <cstdlib>
#include <unistd.h>
#include <errno.h>
#include <securec.h>

#include <linux/sync_file.h>
#include <libsync.h>

#include "sw_sync.h"
#include "hilog/log.h"

namespace OHOS {
using namespace OHOS::HiviewDFX;

namespace {
constexpr HiLogLabel LABEL = { LOG_CORE, 0xD001400, "SyncFence" };
constexpr int32_t INVALID_FD = -1;
constexpr int64_t INVALID_TIMESTAMP = -1;
}  // namespace

const sptr<SyncFence> SyncFence::INVALID_FENCE = sptr<SyncFence>(new SyncFence(INVALID_FD));

SyncTimeline::SyncTimeline() noexcept
{
    if (!IsSupportSoftwareSync()) {
        HiLog::Error(LABEL, "don't support SyncTimeline");
        return;
    }
    int32_t fd = CreateSyncTimeline();
    if (fd > 0) {
        timeLineFd_ = fd;
    }
}

SyncTimeline::~SyncTimeline() noexcept
{
    if (timeLineFd_ > 0) {
        close(timeLineFd_);
        timeLineFd_ = -1;
        isValid_ = false;
    }
}

int32_t SyncTimeline::IncreaseSyncPoint(uint32_t step)
{
    if (timeLineFd_ < 0) {
        return -1;
    }
    return IncreaseSyncPointOnTimeline(timeLineFd_, step);
}

bool SyncTimeline::IsValid()
{
    if (timeLineFd_ > 0) {
        if (fcntl(timeLineFd_, F_GETFD, 0) < 0) {
            return false;
        } else {
            return true;
        }
    } else {
        return false;
    }
}

int32_t SyncTimeline::GenerateFence(std::string name, uint32_t point)
{
    if (timeLineFd_ < 0) {
        return -1;
    }
    int32_t fd = CreateSyncFence(timeLineFd_, name.c_str(), point);
    if (fd < 0) {
        HiLog::Error(LABEL, "Fail to CreateSyncFence");
        return -1;
    }
    return fd;
}

SyncFence::SyncFence(int32_t fenceFd) : fenceFd_(fenceFd)
{
}

SyncFence::~SyncFence()
{
}

int32_t SyncFence::Wait(uint32_t timeout)
{
    if (fenceFd_ < 0) {
        return 0;
    }

    int32_t err = sync_wait(fenceFd_, timeout);
    return err < 0 ? -errno : 0;
}

sptr<SyncFence> SyncFence::MergeFence(const std::string &name,
                const sptr<SyncFence>& fence1, const sptr<SyncFence>& fence2)
{
    int32_t newFenceFd = INVALID_FD;
    int32_t fenceFd1 = fence1->fenceFd_;
    int32_t fenceFd2 = fence2->fenceFd_;

    if (fenceFd1 >= 0 && fenceFd2 >= 0) {
        newFenceFd = sync_merge(name.c_str(), fenceFd1, fenceFd2);
    } else if (fenceFd1 >= 0) {
        newFenceFd = sync_merge(name.c_str(), fenceFd1, fenceFd1);
    } else if (fenceFd2 >= 0) {
        newFenceFd = sync_merge(name.c_str(), fenceFd2, fenceFd2);
    } else {
        return INVALID_FENCE;
    }

    if (newFenceFd == INVALID_FD) {
        HiLog::Error(LABEL, "sync_merge(%{public}s) failed, error: %{public}s (%{public}d)",
                     name.c_str(), strerror(errno), errno);
        return INVALID_FENCE;
    }

    return sptr<SyncFence>(new SyncFence(newFenceFd));
}

int64_t SyncFence::SyncFileReadTimestamp()
{
    std::vector<SyncPointInfo> ptInfos = GetFenceInfo();
    if (ptInfos.empty()) {
        return INVALID_TIMESTAMP;
    }
    size_t signalFenceCount = 0;
    for (auto &info : ptInfos) {
        if (info.status == SIGNALED) {
            signalFenceCount++;
        }
    }
    if (signalFenceCount == ptInfos.size()) {
        // fence signaled
        uint64_t timestamp = 0;
        for (auto &ptInfo : ptInfos) {
            if (ptInfo.timestampNs > timestamp) {
                timestamp = ptInfo.timestampNs;
            }
        }
        return static_cast<int64_t>(timestamp);
    } else {
        // fence still active
        return INVALID_TIMESTAMP;
    }
}

std::vector<SyncPointInfo> SyncFence::GetFenceInfo()
{
    struct sync_file_info arg;
    struct sync_file_info *infoPtr = nullptr;

    (void)memset_s(&arg, sizeof(struct sync_file_info), 0, sizeof(struct sync_file_info));
    int32_t ret = ioctl(fenceFd_, SYNC_IOC_FILE_INFO, &arg);
    if (ret < 0) {
        HiLog::Error(LABEL, "GetFenceInfo SYNC_IOC_FILE_INFO ioctl failed, ret: %{public}d", ret);
        return {};
    }

    if (arg.num_fences <= 0) {
        HiLog::Error(LABEL, "GetFenceInfo arg.num_fences failed, num_fences: %{public}d", arg.num_fences);
        return {};
    }
    // to malloc sync_file_info and the number of 'sync_fence_info' memory
    size_t syncFileInfoMemSize = sizeof(struct sync_file_info) + sizeof(struct sync_fence_info) * arg.num_fences;
    infoPtr = (struct sync_file_info *)malloc(syncFileInfoMemSize);
    if (infoPtr == nullptr) {
        HiLog::Error(LABEL, "GetFenceInfo malloc failed oom");
        return {};
    }
    (void)memset_s(infoPtr, syncFileInfoMemSize, 0, syncFileInfoMemSize);
    infoPtr->num_fences = arg.num_fences;
    infoPtr->sync_fence_info = static_cast<uint64_t>(uintptr_t(infoPtr + 1));

    ret = ioctl(fenceFd_, SYNC_IOC_FILE_INFO, infoPtr);
    if (ret < 0) {
        HiLog::Error(LABEL, "GetTotalFenceInfo SYNC_IOC_FILE_INFO ioctl failed, ret: %{public}d", ret);
        free(infoPtr);
        return {};
    }
    std::vector<SyncPointInfo> infos;
    const auto fenceInfos = (struct sync_fence_info *)(uintptr_t)(infoPtr->sync_fence_info);
    for (uint32_t i = 0; i < infoPtr->num_fences; i++) {
        infos.push_back(SyncPointInfo { fenceInfos[i].timestamp_ns,
            static_cast<FenceStatus>(fenceInfos[i].status) } );
    }

    free(infoPtr);
    return infos;
}

int32_t SyncFence::Dup() const
{
    return ::dup(fenceFd_);
}

FenceStatus SyncFence::GetStatus()
{
    if (fenceFd_ < 0) {
        return ERROR;
    }
    std::vector<SyncPointInfo> ptInfos = GetFenceInfo();
    if (ptInfos.empty()) {
        return ERROR;
    }
    size_t signalFenceCount = 0;
    for (auto &info : ptInfos) {
        if (info.status == SIGNALED) {
            signalFenceCount++;
        }
    }
    if (signalFenceCount == ptInfos.size()) {
        return SIGNALED;
    } else {
        return ACTIVE;
    }
}

int32_t SyncFence::Get() const
{
    return fenceFd_;
}

} // namespace OHOS
