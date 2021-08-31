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

#ifndef FRAMEWORKS_VSYNC_TEST_UNITTEST_VSYNC_REMOTE_MANAGER_PROXY_TEST_H
#define FRAMEWORKS_VSYNC_TEST_UNITTEST_VSYNC_REMOTE_MANAGER_PROXY_TEST_H

#include <gtest/gtest.h>

#include "vsync_callback_stub.h"
#include "vsync_manager.h"

namespace OHOS {
namespace Vsync {
namespace {
constexpr int32_t IPC_VSYNCMANAGER_SAID = 664321;
}
class VsyncManagerTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    virtual void SetUp() override;
    virtual void TearDown() override;

private:
    static inline pid_t pid_;
    static inline int pipeFd[2];
    static inline sptr<IRemoteObject> robj_;
    static inline sptr<IVsyncManager> vc_;
};

class VsyncCallback : public VsyncCallbackStub {
public:
    virtual VsyncError OnVsync(int64_t timestamp) override;
};
} // namespace Vsync
} // namespace OHOS

#endif // FRAMEWORKS_VSYNC_TEST_UNITTEST_VSYNC_REMOTE_MANAGER_PROXY_TEST_H
