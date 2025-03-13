/*
 * Copyright (c) 2022-2024 Huawei Device Co., Ltd.
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

#include "transaction/rs_render_service_client.h"
#include "platform/ohos/rs_render_service_connect_hub.h"
#include "ui/rs_surface_node.h"
#include "surface_utils.h"
#include <iostream>

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
static constexpr uint32_t SET_REFRESHRATE_SLEEP_US = 50000;  // wait for refreshrate change
class RSClientUnitTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;

    static inline std::shared_ptr<RSRenderServiceClient> rsClient = nullptr;
};

void RSClientUnitTest::SetUpTestCase()
{
    rsClient = std::make_shared<RSRenderServiceClient>();
}
void RSClientUnitTest::TearDownTestCase() {}
void RSClientUnitTest::SetUp() {}
void RSClientUnitTest::TearDown()
{
    usleep(SET_REFRESHRATE_SLEEP_US);
}

/**
 * @tc.name: CreateNodeTest
 * @tc.desc: test CreateNode api
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSClientUnitTest, CreateNodeTest, TestSize.Level1)
{
    ASSERT_NE(rsClient, nullptr);
    RSSurfaceRenderNodeConfig config;
    bool ret = rsClient->CreateNode(config);
    ASSERT_EQ(ret, true);
}

/**
 * @tc.name: CreateVirtualScreenTest
 * @tc.desc: test CreateVirtualScreen api
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSClientUnitTest, CreateVirtualScreenTest, TestSize.Level2)
{
    ASSERT_NE(rsClient, nullptr);
    std::vector<NodeId> whiteList = {1};
    ScreenId virtualScreenId = rsClient->CreateVirtualScreen(
        "virtual0", 320, 180, nullptr, INVALID_SCREEN_ID, -1, whiteList);
    EXPECT_NE(virtualScreenId, INVALID_SCREEN_ID);
}

/**
 * @tc.name: GetMemoryGraphicTest
 * @tc.desc: test GetMemoryGraphic api
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSClientUnitTest, GetMemoryGraphic001, TestSize.Level1)
{
    ASSERT_NE(rsClient, nullptr);
    int pid = 100; // for test
    rsClient->GetMemoryGraphic(pid);
    auto memGraphics = rsClient->GetMemoryGraphics();
    ASSERT_EQ(memGraphics.size(), 0);
}

/**
 * @tc.name: GetTotalAppMemSizeTest
 * @tc.desc: test GetTotalAppMemSize api
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSClientUnitTest, GetTotalAppMemSize001, TestSize.Level1)
{
    ASSERT_NE(rsClient, nullptr);
    float cpuMemSize = 0;
    float gpuMemSize = 0;
    bool ret = rsClient->GetTotalAppMemSize(cpuMemSize, gpuMemSize);
    ASSERT_EQ(ret, true);
}

/**
 * @tc.name: CreateNodeAndSurfaceTest
 * @tc.desc: test CreateNodeAndSurface api
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSClientUnitTest, CreateNodeAndSurface001, TestSize.Level1)
{
    ASSERT_NE(rsClient, nullptr);
    RSSurfaceRenderNodeConfig config = {.id=0, .name="testSurface"};
    ASSERT_NE(rsClient->CreateNodeAndSurface(config), nullptr);
}

/**
 * @tc.name: GetAllScreenIdsTest
 * @tc.desc: test GetAllScreenIds api
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSClientUnitTest, GetAllScreenIds001, TestSize.Level1)
{
    ASSERT_NE(rsClient, nullptr);
    auto csurface = IConsumerSurface::Create();
    EXPECT_NE(csurface, nullptr);
    auto producer = csurface->GetProducer();
    auto psurface = Surface::CreateSurfaceAsProducer(producer);
    EXPECT_NE(psurface, nullptr);
    uint32_t defaultWidth = 480;
    uint32_t defaultHeight = 320;
    ScreenId virtualScreenId = rsClient->CreateVirtualScreen(
        "virtualScreenTest", defaultWidth, defaultHeight, psurface, INVALID_SCREEN_ID, -1);
    EXPECT_NE(virtualScreenId, INVALID_SCREEN_ID);
    auto screenIds = rsClient->GetAllScreenIds();
    ASSERT_NE(screenIds.size(), 0);
}

/**
 * @tc.name: SetFocusAppInfoTest
 * @tc.desc: test SetFocusAppInfo api
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSClientUnitTest, SetFocusAppInfo001, TestSize.Level1)
{
    ASSERT_NE(rsClient, nullptr);
    auto ret = rsClient->SetFocusAppInfo(1, 1, "bundleNameTest", "abilityNameTest", 1);
    ASSERT_EQ(ret, SUCCESS);
}

/**
 * @tc.name: SetRefreshRateModeTest
 * @tc.desc: test SetRefreshRateMode api
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSClientUnitTest, SetRefreshRateModeTest, TestSize.Level1)
{
    uint32_t rateMode = 100;
    rsClient->SetRefreshRateMode(rateMode);
    usleep(SET_REFRESHRATE_SLEEP_US);
    uint32_t currentRateMode = rsClient->GetCurrentRefreshRateMode();
    EXPECT_EQ(currentRateMode, rateMode);
}


/**
 * @tc.name: SetScreenActiveModeTest
 * @tc.desc: test SetScreenActiveMode api
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSClientUnitTest, SetScreenActiveModeTest, TestSize.Level1)
{
    auto screenId = rsClient->GetDefaultScreenId();
    EXPECT_NE(screenId, INVALID_SCREEN_ID);
    rsClient->SetScreenActiveMode(screenId, 0);
    auto modeInfo = rsClient->GetScreenActiveMode(screenId);
    EXPECT_EQ(modeInfo.GetScreenModeId(), 0);
}

/**
 * @tc.name: SetScreenActiveRectTest
 * @tc.desc: test SetScreenActiveRect api
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSInterfacesTest, SetScreenActiveRect001, Function | SmallTest | Level2)
{
    auto screenId = rsClient->GetDefaultScreenId();
    EXPECT_NE(screenId, INVALID_SCREEN_ID);

    Rect activeRect {
        .x = 0,
        .y = 0,
        .w = 0,
        .h = 0,
    };
    EXPECT_EQ(rsClient->SetScreenActiveMode(screenId, activeRect), StatusCode::HDI_ERROR);
}

/**
 * @tc.name: SetScreenChangeCallbackTest
 * @tc.desc: test SetScreenChangeCallback api
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSClientUnitTest, SetScreenChangeCallbackTest, TestSize.Level1)
{
    ScreenId screenId = INVALID_SCREEN_ID;
    ScreenEvent screenEvent = ScreenEvent::UNKNOWN;
    bool callbacked = false;
    auto callback = [&screenId, &screenEvent, &callbacked](ScreenId id, ScreenEvent event) {
        screenId = id;
        screenEvent = event;
        callbacked = true;
    };
    int32_t status = rsClient->SetScreenChangeCallback(callback);
    EXPECT_EQ(status, StatusCode::SUCCESS);
}

/**
 * @tc.name: SetScreenRefreshRateTest
 * @tc.desc: test SetScreenRefreshRate api
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSClientUnitTest, SetScreenRefreshRate001, TestSize.Level1)
{
    auto screenId = rsClient->GetDefaultScreenId();
    EXPECT_NE(screenId, INVALID_SCREEN_ID);
    FrameRateLinkerId id = 0;
    FrameRateRange range;
    rsClient->SyncFrameRateRange(id, range, 0);
    uint32_t rateToSet = 990; // 990 for test
    rsClient->SetScreenRefreshRate(screenId, 0, rateToSet);
    usleep(SET_REFRESHRATE_SLEEP_US);
    uint32_t currentRate = rsClient->GetScreenCurrentRefreshRate(screenId);
    EXPECT_NE(currentRate, rateToSet);
}
} // namespace Rosen
} // namespace OHOS