/*
 * Copyright (c) 2026 Huawei Device Co., Ltd.
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

#include <gtest/gtest.h>
#include "rs_client_to_service_connect_hub.h"
#include "platform/common/rs_log.h"
#include "transaction/rs_render_service_client.h"
#include "animation/rs_frame_rate_range.h"
#include "variable_frame_rate/rs_variable_frame_rate.h"
#include "transaction/zidl/rs_client_to_service_connection_proxy.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
class RSRenderServiceClientProxyTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void RSRenderServiceClientProxyTest::SetUpTestCase() {}
void RSRenderServiceClientProxyTest::TearDownTestCase() {}
void RSRenderServiceClientProxyTest::SetUp() {}
void RSRenderServiceClientProxyTest::TearDown() {}

/**
 * @tc.name: NotifyTouchEventTest
 * @tc.desc: Notify touch event to HGM for refresh rate decisions
 * @tc.type: FUNC
 */
HWTEST_F(RSRenderServiceClientProxyTest, NotifyTouchEventTest, TestSize.Level1)
{
    auto rsClient = RSRenderServiceClient::CreateRenderServiceClient();
    rsClient->NotifyTouchEvent(0, 1, 0);
    EXPECT_NE(rsClient, nullptr);
}

/**
 * @tc.name: SetThermalFrameRateLimit001
 * @tc.desc: Thermal limit API must be callable in proxy mode; null connection exits safely
 * @tc.type: FUNC
 */
HWTEST_F(RSRenderServiceClientProxyTest, SetThermalFrameRateLimit001, TestSize.Level1)
{
    auto rsClient = RSRenderServiceClient::CreateRenderServiceClient();
    ASSERT_NE(rsClient, nullptr);
    // No live connection in test env: early-return path must not crash.
    rsClient->SetThermalFrameRateLimit(30);
}
 
/**
 * @tc.name: SetThermalFrameRateLimit002
 * @tc.desc: Thermal limit API must be callable in proxy mode; null connection exits safely
 * @tc.type: FUNC
 */
HWTEST_F(RSRenderServiceClientProxyTest, SetThermalFrameRateLimit002, TestSize.Level1)
{
    auto rsClient = RSRenderServiceClient::CreateRenderServiceClient();
    ASSERT_NE(rsClient, nullptr);
    rsClient->SetThermalFrameRateLimit(0); // clear path
}
 
/**
 * @tc.name: SetThermalFrameRateLimit003
 * @tc.desc: Thermal limit API must be callable in proxy mode; null connection exits safely
 * @tc.type: FUNC
 */
HWTEST_F(RSRenderServiceClientProxyTest, SetThermalFrameRateLimit003, TestSize.Level1)
{
    auto rsClient = RSRenderServiceClient::CreateRenderServiceClient();
    ASSERT_NE(rsClient, nullptr);
    rsClient->SetThermalFrameRateLimit(30);
    rsClient->SetThermalFrameRateLimit(0); // clear path
}
} // namespace Rosen
} // namespace OHOS
