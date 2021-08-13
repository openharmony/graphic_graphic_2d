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

#ifndef FRAMEWORKS_SURFACE_TEST_SYSTEMTEST_SURFACE_REVERT_IPC_TEST_H
#define FRAMEWORKS_SURFACE_TEST_SYSTEMTEST_SURFACE_REVERT_IPC_TEST_H

#include <gtest/gtest.h>
#include <iservice_registry.h>
#include <surface.h>

namespace OHOS {
class SurfaceRevertIPCTest : public testing::Test, public IBufferConsumerListenerClazz {
public:
    static void SetUpTestCase();
    virtual void OnBufferAvailable() override;

    pid_t ChildProcessMain();

    static inline int32_t pipeFd[2] = {};
    static inline constexpr int32_t ipcSystemAbilityID = 34156;
    static inline BufferRequestConfig requestConfig = {};
    static inline BufferFlushConfig flushConfig = {};
};
} // namespace OHOS

#endif // FRAMEWORKS_SURFACE_TEST_SYSTEMTEST_SURFACE_REVERT_IPC_TEST_H
