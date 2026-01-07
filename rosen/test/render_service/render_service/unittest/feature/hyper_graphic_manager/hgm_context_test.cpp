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

#include "feature/hyper_graphic_manager/hgm_context.h"
#include "hgm_core.h"
#include "render_server/rs_render_service.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Rosen {
namespace {
RSRenderService renderService;
}

class HgmContextTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void HgmContextTest::SetUpTestCase() {}
void HgmContextTest::TearDownTestCase() {}
void HgmContextTest::SetUp() {}
void HgmContextTest::TearDown() {}

/**
 * @tc.name: HgmContextCreateTest
 * @tc.desc: test HgmContext.HandleHgmProcessInfo
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(HgmContextTest, HandleHgmProcessInfoTest, TestSize.Level1)
{
    renderService.HgmInit();
    ASSERT_NE(renderService.hgmContext_, nullptr);
    const auto& hgmContext = renderService.GetHgmContext();
    hgmContext->HandleHgmProcessInfo(nullptr);
    EXPECT_EQ(frameRateManager.isGameNodeOnTree, false)
    sptr<HgmProcessToServiceInfo> processToServiceInfo = sptr<HgmProcessToServiceInfo>::MakeSptr();
    processToServiceInfo->frameRateLinkerDestroyIds = { 1 };
    FrameRateRange range((0, 120, 60));
    processToServiceInfo->frameRateLinkerUpdateInfoMap[2] = { range, 120 };
    processToServiceInfo->rsCurrRange = range;
    processToServiceInfo->surfaceData = { std::tuple<std::string, pid_t>({surfaceName, pid}) };
    processToServiceInfo->isGameNodeOnTree = true;
    hgmContext->HandleHgmProcessInfo(processToServiceInfo);
    auto frameRateManager = hgmContext->frameRateManager_;
    EXPECT_EQ(hgmContext.rsCurrRange_.preferred_, 60)
    EXPECT_EQ(frameRateManager.isGameNodeOnTree, true)
}
} // namespace OHOS::Rosen

