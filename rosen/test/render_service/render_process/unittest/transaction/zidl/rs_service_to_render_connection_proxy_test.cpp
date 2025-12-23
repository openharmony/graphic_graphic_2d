/*
 * Copyright (c) 2025 Huawei Device Co., Ltd.
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

#include "gtest/gtest.h"

#include <if_system_ability_manager.h>
#include <iremote_stub.h>
#include <iservice_registry.h>
#include <mutex>
#include <refbase.h>
#include <system_ability_definition.h>
#include <unistd.h>

#include "transaction/zidl/rs_service_to_render_connection_proxy.h"

using namespace testing;
using namespace testing::ext;


namespace OHOS::Rosen {
class RSServiceToRenderConnectionProxyTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
    static inline std::shared_ptr<RSServiceToRenderConnectionProxy> proxy;
};

void RSServiceToRenderConnectionProxyTest::SetUpTestCase()
{
    auto samgr = SystemAbilityManagerClient::GetInstance().GetSystemAbilityManager();
    ASSERT_NE(samgr, nullptr);
    auto remoteObject = samgr->GetSystemAbility(RENDER_SERVICE);
    proxy = std::make_shared<RSServiceToRenderConnectionProxy>(remoteObject);
}
void RSServiceToRenderConnectionProxyTest::TearDownTestCase() {}
void RSServiceToRenderConnectionProxyTest::SetUp() {}
void RSServiceToRenderConnectionProxyTest::TearDown() {}

/**
 * @tc.name: GetShowRefreshRateEnabledTest
 * @tc.desc: Test
 * @tc.type: FUNC
 * @tc.require: issueIBRN69
 */
HWTEST_F(RSServiceToRenderConnectionProxyTest, GetShowRefreshRateEnabledTest, TestSize.Level1)
{
    bool enabled = false;
    bool enabled1 = true;
    proxy->GetShowRefreshRateEnabled(enabled);
    proxy->GetShowRefreshRateEnabled(enabled1);
    ASSERT_TRUE(proxy);
}

/**
 * @tc.name: SetShowRefreshRateEnabledTest
 * @tc.desc: Test
 * @tc.type: FUNC
 * @tc.require: issueIBRN69
 */
HWTEST_F(RSServiceToRenderConnectionProxyTest, SetShowRefreshRateEnabledTest, TestSize.Level1)
{
    bool enabled = false;
    bool enabled1 = true;

    proxy->SetShowRefreshRateEnabled(enabled, 0);
    proxy->SetShowRefreshRateEnabled(enabled1, 0);
    ASSERT_TRUE(proxy);
}

/**
 * @tc.name: GetRealtimeRefreshRateTest
 * @tc.desc: Test
 * @tc.type: FUNC
 * @tc.require: issueIBRN69
 */
HWTEST_F(RSServiceToRenderConnectionProxyTest, GetRealtimeRefreshRateTest, TestSize.Level1)
{
    EXPECT_EQ(proxy->GetRealtimeRefreshRate(INVALID_SCREEN_ID), 0);
}
} // namespace OHOS::Rosen
