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

#ifndef FRAMEWORKS_WM_SRC_TEST_UTIL_H
#define FRAMEWORKS_WM_SRC_TEST_UTIL_H

#include <cstdint>
#include <functional>

#include <vsync_helper.h>

namespace OHOS {
void SetVsyncRate(int32_t rate);
uint32_t RequestSync(const SyncFunc syncFunc, void *data = nullptr);
void PostTask(std::function<void()> func, uint32_t delayTime = 0);
void ExitTest();
int64_t GetNowTime();
} // namespace OHOS

#endif // FRAMEWORKS_WM_SRC_TEST_UTIL_H
