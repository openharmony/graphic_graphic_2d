/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
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

#ifndef UTILS_INCLUDE_SW_SYNC_H
#define UTILS_INCLUDE_SW_SYNC_H

#include <cerrno>
#include <fcntl.h>
#include <poll.h>
#include <sys/ioctl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#include <securec.h>

#ifdef __cplusplus
extern "C" {
#endif

struct sw_sync_create_fence_data {
    unsigned int value;
    char name[32];
    unsigned int fence;
};

#define SYNC_IOC_CREATE_SW_SYNC_FENCE _IOWR('W', 0, struct sw_sync_create_fence_data)
#define SYNC_IOC_INCREASE_TIMELINE    _IOW('W', 1, unsigned int)

static inline bool IsSupportSoftwareSync(void)
{
    struct stat syncStatus = {};
    if (!stat("/sys/kernel/debug/sync/sw_sync", &syncStatus)) {
        return true;
    }
    return false;
}

static inline int CreateSyncTimeline(void)
{
    int timeline = open("/sys/kernel/debug/sync/sw_sync", O_RDWR);
    if (timeline < 0) {
        return -1;
    }
    if (fcntl(timeline, F_GETFD, 0) < 0) {
        close(timeline);
        return -1;
    }
    return timeline;
}

static inline int IncreaseSyncPointOnTimeline(int timelineFd, unsigned int count)
{
    unsigned int count_ = count;

    int ret = ioctl(timelineFd, SYNC_IOC_INCREASE_TIMELINE, &count_);
    return ret;
}

static inline int CreateSyncFence(int timelineFd, const char* name, unsigned int totalSteps)
{
    struct sw_sync_create_fence_data data = { .value = totalSteps };
    if (strcpy_s(data.name, sizeof(data.name), name)) {
        return -1;
    }
    int ret = ioctl(timelineFd, SYNC_IOC_CREATE_SW_SYNC_FENCE, &data);
    return ret != 0 ? ret : data.fence;
}

#ifdef __cplusplus
}
#endif

#endif