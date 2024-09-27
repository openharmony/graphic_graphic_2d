/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
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
#include <iostream>

#include "platform/ohos/rs_render_service_connect_hub.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
class RSRenderServiceConnectHubTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;

    static inline std::shared_ptr<RSRenderServiceClient> rsClient = nullptr;
};

void RSRenderServiceConnectHubTest::SetUpTestCase() {}
void RSRenderServiceConnectHubTest::TearDownTestCase() {}
void RSRenderServiceConnectHubTest::SetUp() {}
void RSRenderServiceConnectHubTest::TearDown() {}

/**
 * @tc.name: GetRenderService
 * @tc.desc:
 * @tc.type:FUNC
 * @tc.require: I6X9V1
 */
HWTEST_F(RSRenderServiceConnectHubTest, GetRenderService, TestSize.Level1)
{
    auto renderService = RSRenderServiceConnectHub::GetRenderService();
    ASSERT_NE(renderService, nullptr);
}

/**
 * @tc.name: GetRenderServiceConnectionTest
 * @tc.desc: Verify function GetRenderServiceConnection
 * @tc.type:FUNC
 * @tc.require: issueI9TOXM
 */
HWTEST_F(RSRenderServiceConnectHubTest, GetRenderServiceConnectionTest, TestSize.Level1)
{
    EXPECT_NE(RSRenderServiceConnectHub::GetInstance()->GetRenderServiceConnection(), nullptr);
    RSRenderServiceConnectHub::GetInstance()->conn_ = nullptr;
    RSRenderServiceConnectHub::GetInstance()->renderService_ = nullptr;
    EXPECT_NE(RSRenderServiceConnectHub::GetInstance()->GetRenderServiceConnection(), nullptr);
}

/**
 * @tc.name: ConnectDiedTest
 * @tc.desc: Verify function ConnectDied
 * @tc.type:FUNC
 * @tc.require: issueI9TOXM
 */
HWTEST_F(RSRenderServiceConnectHubTest, ConnectDiedTest, TestSize.Level1)
{
    RSRenderServiceConnectHub::GetInstance()->ConnectDied();
    RSRenderServiceConnectHub::GetInstance()->conn_ = nullptr;
    RSRenderServiceConnectHub::GetInstance()->ConnectDied();
    EXPECT_NE(RSRenderServiceConnectHub::GetInstance(), nullptr);
}
} // namespace Rosen
} // namespace OHOS