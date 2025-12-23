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

#include "rs_render_pipeline.h"
#include "transaction/zidl/rs_render_to_service_connection_proxy.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Rosen {
class RSRenderToServiceConnectionProxyTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
    static inline std::shared_ptr<RSRenderToServiceConnectionProxy> proxy;
};

void RSRenderToServiceConnectionProxyTest::SetUpTestCase()
{
    auto samgr = SystemAbilityManagerClient::GetInstance().GetSystemAbilityManager();
    ASSERT_NE(samgr, nullptr);
    auto remoteObject = samgr->GetSystemAbility(RENDER_SERVICE);
    proxy = std::make_shared<RSRenderToServiceConnectionProxy>(remoteObject);
}
void RSRenderToServiceConnectionProxyTest::TearDownTestCase() {}
void RSRenderToServiceConnectionProxyTest::SetUp() {}
void RSRenderToServiceConnectionProxyTest::TearDown() {}

/**
 * @tc.name: NotifyRpHgmFrameRateTest
 * @tc.desc: Test
 * @tc.type: FUNC
 * @tc.require: issueIBRN69
 */
HWTEST_F(RSRenderToServiceConnectionProxyTest, NotifyRpHgmFrameRateTest, TestSize.Level1)
{
    uint64_t timestamp = 1;
    uint64_t vsyncId = 1;
    int32_t sizes = 10;
    std::unordered_set<ScreenId> screenIds;
    for (uint64_t i = 0; i < sizes; ++i)
    {
        screenIds.insert(i);
    }
    sptr<HgmProcessToServiceInfo> processToServiceInfo = sptr<HgmProcessToServiceInfo>::MakeSptr();
    ASSERT_TRUE(proxy->NotifyRpHgmFrameRate(timestamp, vsyncId, screenIds, processToServiceInfo));
}
} // namespace OHOS::Rosen
