/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, Hardware
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <gtest/gtest.h>

#include "platform/drawing/rs_vsync_client.h"
#include "platform/ohos/rs_vsync_client_ohos.h"
#include "platform/ohos/rs_render_service_connect_hub.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
class RSVsyncClientTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;

    std::unique_ptr<RSVsyncClient> vsyncClient_ = nullptr;
    std::unique_ptr<RSVsyncClientOhos> vsyncClientOhos_ = nullptr;
    static inline RSVsyncClient::VsyncCallback cb = nullptr;
    int64_t nanoTimestamp_ = 1717552460;
    int64_t frameCount_ = 1;
};

void RSVsyncClientTest::SetUpTestCase()
{
    cb = [](int64_t timestamp, int64_t frameCount) {};
}
void RSVsyncClientTest::TearDownTestCase() {}
void RSVsyncClientTest::SetUp()
{
    vsyncClient_ = RSVsyncClient::Create();
    vsyncClientOhos_ = std::make_unique<RSVsyncClientOhos>();
}
void RSVsyncClientTest::TearDown() {}

/**
 * @tc.name: VsyncCallbackTest001
 * @tc.desc: Test VsyncCallback
 * @tc.type:FUNC
 * @tc.require: issueI9V3Y2
 */
HWTEST_F(RSVsyncClientTest, VsyncCallbackTest001, TestSize.Level1)
{
    RSVsyncClient::VsyncCallback vsyncCallback1 = [](int64_t nanoTimestamp, int64_t frameCount) {
        printf("VsyncCallbackTest vsyncCallback\n");
    };
    vsyncClientOhos_->SetVsyncCallback(vsyncCallback1);
    vsyncClientOhos_->requestFlag_.store(true);
    vsyncClientOhos_->VsyncCallback(nanoTimestamp_, frameCount_);
    ASSERT_FALSE(vsyncClientOhos_->requestFlag_);
}

/**
 * @tc.name: SetVsyncCallback Test
 * @tc.desc: SetVsyncCallback Test
 * @tc.type:FUNC
 * @tc.require: issueI9V3Y2
 */
HWTEST_F(RSVsyncClientTest, SetVsyncCallback_Test001, TestSize.Level1)
{
    ASSERT_NE(vsyncClient_, nullptr);
    vsyncClient_->SetVsyncCallback(cb);
}

/**
 * @tc.name: OnVsyncTest001
 * @tc.desc: Test OnVsync
 * @tc.type:FUNC
 * @tc.require: issueI9V3Y2
 */
HWTEST_F(RSVsyncClientTest, OnVsyncTest001, TestSize.Level1)
{
    vsyncClient_.reset(nullptr);
    ASSERT_TRUE(vsyncClient_ == nullptr);
    vsyncClientOhos_->OnVsync(nanoTimestamp_, frameCount_, vsyncClient_.get());

    RSVsyncClient::VsyncCallback vsyncCallback1 = [](int64_t nanoTimestamp, int64_t frameCount) {
        printf("OnVsyncTest vsyncCallback\n");
    };
    vsyncClient_ = RSVsyncClient::Create();
    vsyncClient_->SetVsyncCallback(vsyncCallback1);
    ASSERT_TRUE(vsyncClient_ != nullptr);
    vsyncClientOhos_->OnVsync(nanoTimestamp_, frameCount_, vsyncClient_.get());
}
} // namespace Rosen
} // namespace OHOS