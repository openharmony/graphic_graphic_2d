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

#include "fence.h"

#include <cerrno>
#include <fcntl.h>
#include <poll.h>
#include <sys/ioctl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#include <hilog/log.h>
#include <securec.h>
#include <linux/sync_file.h>

bool IsSupportSwSync()
{
    struct stat syncStatus = {};
    if (!stat("/sys/kernel/debug/sync/sw_sync", &syncStatus)) {
        return true;
    }
    return false;
}

int CreateTimeline()
{
    int timeline = open("/sys/kernel/debug/sync/sw_sync", O_RDWR);
    if (timeline < 0) {
        HiLogPrint(LOG_CORE, LOG_ERROR, 0, "fence", "open file failed");
        return -1;
    }

    if (fcntl(timeline, F_GETFD, 0) < 0) {
        HiLogPrint(LOG_CORE, LOG_ERROR, 0, "fence", "the timeline is valid");
        close(timeline);
        return -1;
    }
    return timeline;
}

int CreateFenceFromTimeline(int timeline, const char* name, unsigned int totalSteps)
{
    struct sw_sync_create_fence_data {
        unsigned int value;
        char name[32];
        unsigned int fence;
    };

    struct sw_sync_create_fence_data data = {
        .value = totalSteps
    };

    if (strcpy_s(data.name, sizeof(data.name), name)) {
        HiLogPrint(LOG_CORE, LOG_ERROR, 0, "fence", "Create Fence From Timeline Failed");
        return -1;
    }
    int ret = ioctl(timeline, _IOWR('W', 0, struct sw_sync_create_fence_data), &data);
    if (ret != 0) {
        HiLogPrint(LOG_CORE, LOG_ERROR, 0, "fence", " data.fence are invalid");
        return -1;
    }
    return data.fence;
}

int FenceHold(int fd, int timeout)
{
    int ret = 0;
    if (fd < 0) {
        HiLogPrint(LOG_CORE, LOG_ERROR, 0, "fence", "the fd is invalid");
        return -1;
    }

    struct pollfd pollfds = {
        .fd = fd,
        .events = POLLIN | POLLERR,
    };

    do {
        ret = poll(&pollfds, 1, timeout);
    } while (ret == -1 && errno == EINTR);
    return ret;
}

int TimelineActivate(int timeline, unsigned int step)
{
    int ret = ioctl(timeline, _IOW('W', 1, unsigned int), &step);
    if (ret != 0) {
        return errno;
    }
    return ret;
}

enum FenceStatus FenceGetStatus(int fd)
{
    int ret = FenceHold(fd, 0);

    enum FenceStatus status = ACTIVE;
    if (ret < 0) {
        status = ERROR;
    } else if (ret > 0) {
        status = SIGNALED;
    }
    return status;
}

int FenceMerge(const char* name, int fd1, int fd2)
{
    int result_code;
    struct sync_merge_data sync_merge_data = {};
    if (strcpy_s(sync_merge_data.name, sizeof(sync_merge_data.name), name)) {
        HiLogPrint(LOG_CORE, LOG_ERROR, 0, "fence", "FenceMerge strcpy name failed");
        return -1;
    }

    if (fd1 >= 0 && fd2 < 0) {
        sync_merge_data.fd2 = fd1;
        result_code = ioctl(fd1, SYNC_IOC_MERGE, &sync_merge_data);
    }

    if (fd1 < 0 && fd2 >= 0) {
        sync_merge_data.fd2 = fd2;
        result_code = ioctl(fd2, SYNC_IOC_MERGE, &sync_merge_data);
    }

    if (fd1 >= 0 && fd2 >= 0) {
        sync_merge_data.fd2 = fd2;
        result_code = ioctl(fd1, SYNC_IOC_MERGE, &sync_merge_data);
    }

    if (fd1 < 0 && fd2 < 0) {
        HiLogPrint(LOG_CORE, LOG_ERROR, 0, "fence", "fd1 and fd2 are invalid");
        return -1;
    }

    if (result_code < 0) {
        HiLogPrint(LOG_CORE, LOG_ERROR, 0, "fence", "merge failed %{public}d", errno);
        return result_code;
    }
    return sync_merge_data.fence;
}

