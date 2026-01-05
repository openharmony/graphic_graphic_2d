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

void HgmContextTest::SetUpTestCase()
{
    OHOS::system::SetParameter("bootevent.samgr.ready", "false");
    renderService.Init();
}
void HgmContextTest::TearDownTestCase() {}
void HgmContextTest::SetUp() {}
void HgmContextTest::TearDown() {}

/**
 * @tc.name: HgmContextCreateTest
 * @tc.desc: test HgmContext.HgmContext
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(HgmContextTest, HgmContextInitTest, TestSize.Level1)
{
    renderService.HgmInit();
    ASSERT_NE(renderService.hgmContext_, nullptr);
    const auto& hgmContext = renderService.GetHgmContext();
    ASSERT_NE(hgmContext->rsFrameRateLinker_, nullptr);
    auto frameRateManager = hgmContext->frameRateManager_;
    ASSERT_NE(frameRateManager->forceUpdateCallback_, nullptr);
    ASSERT_NE(frameRateManager->hgmConfigUpdateCallback_, nullptr);
    ASSERT_NE(frameRateManager->adaptiveVsyncUpdateCallback_, nullptr);
    frameRateManager->forceUpdateCallback_(true);
    frameRateManager->adaptiveVsyncUpdateCallback_(true, "test");
    auto& hgmCore = HgmCore::Instance();
    frameRateManager->hgmConfigUpdateCallback_(std::make_shared<RPHgmConfigData>(),
        hgmCore.GetLtpoEnabled(), hgmCore.IsDelayMode(), hgmCore.GetPipelineOffsetPulseNum());
    ASSERT_NE(hgmContext->hgmDataChangeTypes_.test(HgmDataChangeType::ADAPTIVE_VSYNC), true)
    ASSERT_NE(hgmContext->hgmDataChangeTypes_.test(HgmDataChangeType::HGM_CONFIG_DATA), true)
}
} // namespace OHOS::Rosen

