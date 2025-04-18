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

#include "gtest/gtest.h"
#include "gfx/first_frame_notifier/rs_first_frame_notifier.h"
#include "ipc_callbacks/rs_ifirst_frame_commit_callback.h"
#include "transaction/rs_render_service_client.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Rosen {
class RSFirstFrameNotifierTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void RSFirstFrameNotifierTest::SetUpTestCase() {}
void RSFirstFrameNotifierTest::TearDownTestCase() {}
void RSFirstFrameNotifierTest::SetUp() {}
void RSFirstFrameNotifierTest::TearDown() {}

class CustomFirstFrameCommitCallback : public RSIFirstFrameCommitCallback {
public:
    explicit CustomFirstFrameCommitCallback(const FirstFrameCommitCallback& callback) : cb_(callback) {}
    ~CustomFirstFrameCommitCallback() override {};

    void OnFirstFrameCommit(uint64_t screenId, int64_t timestamp) override
    {
        if (cb_ != nullptr) {
            cb_(screenId, timestamp);
        }
    }

    sptr<IRemoteObject> AsObject() override
    {
        return nullptr;
    }

private:
    FirstFrameCommitCallback cb_;
};

/**
 * @tc.name: RSFirstFrameNotifier001
 * @tc.desc: test results of RSFirstFrameNotifier
 * @tc.type:FUNC
 * @tc.require: IBE7GI
 */
HWTEST_F(RSFirstFrameNotifierTest, RSFirstFrameNotifier001, TestSize.Level1)
{
    ScreenId screenId = 0;
    pid_t pid0 = 0;
    pid_t pid1 = 1;
    auto firstFrameCommitCallback = [](uint64_t, int64_t) {};
    sptr<CustomFirstFrameCommitCallback> cb = new CustomFirstFrameCommitCallback(firstFrameCommitCallback);
    RSFirstFrameNotifier& firstFrameNotifier = RSFirstFrameNotifier::GetInstance();
    firstFrameNotifier.AddFirstFrameCommitScreen(screenId);
    EXPECT_EQ(firstFrameNotifier.firstFrameCommitScreens_.size(), 1);
    firstFrameNotifier.RegisterFirstFrameCommitCallback(pid0, cb);
    EXPECT_EQ(firstFrameNotifier.firstFrameCommitCallbacks_.size(), 1);
    firstFrameNotifier.RegisterFirstFrameCommitCallback(pid1, nullptr);
    EXPECT_EQ(firstFrameNotifier.firstFrameCommitCallbacks_.size(), 1);
    firstFrameNotifier.ExecIfFirstFrameCommit(screenId);
    EXPECT_EQ(firstFrameNotifier.firstFrameCommitScreens_.size(), 0);
    firstFrameNotifier.ExecIfFirstFrameCommit(screenId);
    firstFrameNotifier.OnFirstFrameCommitCallback(screenId);
    firstFrameNotifier.RegisterFirstFrameCommitCallback(pid0, nullptr);
    EXPECT_EQ(firstFrameNotifier.firstFrameCommitCallbacks_.size(), 0);
}
}