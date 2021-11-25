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

#ifndef FRAMEWORKS_FENCE_INCLUDE_FENCE_H
#define FRAMEWORKS_FENCE_INCLUDE_FENCE_H

#ifdef __cplusplus
extern "C" {
#endif

enum FenceStatus {
    ERROR, ACTIVE, SIGNALED
};

bool IsSupportSwSync(void);
int CreateTimeline(void);
int CreateFenceFromTimeline(int timeline, const char* name, unsigned int totalSteps);
int FenceHold(int fd, int timeout);
int TimelineActivate(int timeline, unsigned int step);
enum FenceStatus FenceGetStatus(int fd);
int FenceMerge(const char* name, int fd1, int fd2);

#ifdef __cplusplus
}

#endif
#endif // FRAMEWORKS_FENCE_INCLUDE_FENCE_H
