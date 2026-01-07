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
    EXPECT_EQ(refreshRate, 0);

    sptr<HgmServiceToProcessInfo> serviceToProcessInfo = sptr<HgmServiceToProcessInfo>::MakeSptr();
    serviceToProcessInfo->pendingScreenRefreshRate = 60;
    serviceToProcessInfo->pendingConstraintRelativeTime = 1;
    serviceToProcessInfo->ltpoEnabled = true;
    serviceToProcessInfo->isDelayMode = true;
    serviceToProcessInfo->pipelineOffsetPulseNum = 1;
    serviceToProcessInfo->isAdaptive = true;
    serviceToProcessInfo->gameNodeName = "gameNodeName";
    serviceToProcessInfo->isPowerIdle = true;
    hgmRPContext.SetServiceToProcessInfo(serviceToProcessInfo, refreshRate, relativeTime);
    EXPECT_EQ(refreshRate, 60);
    EXPECT_EQ(hgmRPContext.isAdaptive_, false);
    EXPECT_EQ(hgmRPContext.ltpoEnabled_, false);
    EXPECT_EQ(hgmRPContext.hgmRPEnergy_->isTouchIdle_, true);

    serviceToProcessInfo->hgmDataChangeTypes.set(HgmDataChangeType::ADAPTIVE_VSYNC);
    hgmRPContext.SetServiceToProcessInfo(serviceToProcessInfo, refreshRate, relativeTime);
    EXPECT_EQ(hgmRPContext.isAdaptive_, true);

    serviceToProcessInfo->hgmDataChangeTypes.set(HgmDataChangeType::HGM_CONFIG_DATA);
    hgmRPContext.SetServiceToProcessInfo(serviceToProcessInfo, refreshRate, relativeTime);
    EXPECT_EQ(hgmRPContext.ltpoEnabled_, true);
}
} // namespace OHOS::Rosen