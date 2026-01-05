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

#include "feature/hyper_graphic_manager/hgm_rp_context.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Rosen {
namespace {
}
class HgmRPContextTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void HgmRPContextTest::SetUpTestCase() {}
void HgmRPContextTest::TearDownTestCase() {}
void HgmRPContextTest::SetUp() {}
void HgmRPContextTest::TearDown() {}

/**
 * @tc.name: TestSetServiceToProcessInfo
 * @tc.desc: test HgmRPContext.SetServiceToProcessInfo
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(HgmRPContextTest, TestSetServiceToProcessInfo, TestSize.Level1)
{
    sptr<RSIRenderToServiceConnection> renderToServiceConnection = nullptr;
    HgmRPContext hgmRPContext(renderToServiceConnection);
    uint32_t refreshRate = 0;
    uint64_t relativeTime = 0;
    hgmRPContext.SetServiceToProcessInfo(nullptr, refreshRate, relativeTime);
    EXPECT_EQ(hgmRPContext.isAdaptive_, false);

    sptr<HgmServiceToProcessInfo> serviceToProcessInfo = sptr<HgmServiceToProcessInfo>::MakeSptr();
    // serviceToProcessInfo->SetHgmRefreshRateInfo(60, 1);
    // serviceToProcessInfo->SetAdaptiveVsyncInfo(false, true, "gameNodeName");
    // serviceToProcessInfo->SetRPHgmConfigData(false, true, true, 1, nullptr);
    // serviceToProcessInfo->SetPowerIdle(true);
    hgmRPContext.SetServiceToProcessInfo(serviceToProcessInfo, refreshRate, relativeTime);
    EXPECT_EQ(refreshRate, 60);
    EXPECT_EQ(hgmRPContext.isAdaptive_, false);

    // serviceToProcessInfo->SetAdaptiveVsyncInfo(true, true, "gameNodeName");
    hgmRPContext.SetServiceToProcessInfo(serviceToProcessInfo, refreshRate, relativeTime);

    // serviceToProcessInfo->SetRPHgmConfigData(true, true, true, 1, std::make_shared<RPHgmConfigData>());
    hgmRPContext.SetServiceToProcessInfo(serviceToProcessInfo, refreshRate, relativeTime);
    EXPECT_EQ(hgmRPContext.ltpoEnabled_, true);

    // serviceToProcessInfo->SetRPHgmConfigData(true, false, true, 1, nullptr);
    hgmRPContext.SetServiceToProcessInfo(serviceToProcessInfo, refreshRate, relativeTime);
    EXPECT_EQ(hgmRPContext.ltpoEnabled_, false);
    
}
} // namespace OHOS::Rosen