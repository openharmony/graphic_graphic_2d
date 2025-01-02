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
static constexpr uint64_t TEST_ID = 123;
class RSClientTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;

    static inline std::shared_ptr<RSRenderServiceClient> rsClient = nullptr;
};

void RSClientTest::SetUpTestCase()
{
    rsClient = std::make_shared<RSRenderServiceClient>();
}
void RSClientTest::TearDownTestCase() {}
void RSClientTest::SetUp() {}
void RSClientTest::TearDown()
{
    usleep(SET_REFRESHRATE_SLEEP_US);
}

/**
 * @tc.name: CreateNode Test
 * @tc.desc: CreateNode Test
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSClientTest, CreateNode_Test, TestSize.Level1)
{
    ASSERT_NE(rsClient, nullptr);
    RSSurfaceRenderNodeConfig config;
    bool ret = rsClient->CreateNode(config);
    ASSERT_EQ(ret, true);
}

class TestSurfaceCaptureCallback : public SurfaceCaptureCallback {
public:
    explicit TestSurfaceCaptureCallback() {}
    ~TestSurfaceCaptureCallback() override {}
    void OnSurfaceCapture(std::shared_ptr<Media::PixelMap> pixelmap) override
    {
    }
};

/**
 * @tc.name: ExecuteSynchronousTask Test
 * @tc.desc: Test ExecuteSynchronousTask
 * @tc.type:FUNC
 * @tc.require: issuesI9K7SJ
 */
HWTEST_F(RSClientTest, ExecuteSynchronousTaskTest001, TestSize.Level1)
{
    ASSERT_NE(rsClient, nullptr);
    rsClient->ExecuteSynchronousTask(nullptr);

    auto task1 = std::make_shared<RSNodeGetShowingPropertyAndCancelAnimation>(0, nullptr);
    ASSERT_NE(task1, nullptr);
    rsClient->ExecuteSynchronousTask(task1);
}

/**
 * @tc.name: GetScreenData Test
 * @tc.desc: GetScreenData Test
 * @tc.type:FUNC
 * @tc.require: issuesI9K7SJ
 */
HWTEST_F(RSClientTest, GetScreenDataTest001, TestSize.Level1)
{
    auto screenId1 = rsClient->GetDefaultScreenId();
    EXPECT_NE(screenId1, INVALID_SCREEN_ID);
    auto screenData = rsClient->GetScreenData(screenId1);
    EXPECT_GT(screenData.GetSupportModeInfo().size(), 0);
    EXPECT_NE(screenData.GetCapability().GetType(), DISP_INVALID);
}

/**
 * @tc.name: GetScreenBacklight Test
 * @tc.desc: GetScreenBacklight Test
 * @tc.type:FUNC
 * @tc.require: issuesI9K7SJ
 */
HWTEST_F(RSClientTest, GetScreenBacklightTest001, TestSize.Level1)
{
    auto screenId1 = rsClient->GetDefaultScreenId();
    EXPECT_NE(screenId1, INVALID_SCREEN_ID);
    rsClient->SetScreenBacklight(screenId1, 60); // for test
    usleep(SET_REFRESHRATE_SLEEP_US);
    auto backLight = rsClient->GetScreenBacklight(screenId1);
}

/*
 * @tc.name: SetScreenGamutMap Test
 * @tc.desc: SetScreenGamutMap Test
 * @tc.type:FUNC
 * @tc.require: issuesI9K7SJ
*/
HWTEST_F(RSClientTest, SetScreenGamutMapTest001, TestSize.Level1)
{
    ScreenGamutMap mode1 = ScreenGamutMap::GAMUT_MAP_CONSTANT;
    ScreenId screenId1 = rsClient->GetDefaultScreenId();
    uint32_t ret = rsClient->SetScreenGamutMap(screenId1, mode1);
    ASSERT_EQ(ret, StatusCode::HDI_ERROR);
    ret = rsClient->GetScreenGamutMap(screenId1, mode1);
    ASSERT_EQ(ret, StatusCode::HDI_ERROR);
}

/*
 * @tc.name: GetScreenColorGamut Test
 * @tc.desc: GetScreenColorGamut Test
 * @tc.type:FUNC
 * @tc.require: issuesI9K7SJ
*/
HWTEST_F(RSClientTest, GetScreenColorGamutTest001, TestSize.Level1)
{
    auto csurface = IConsumerSurface::Create();
    EXPECT_NE(csurface, nullptr);

    auto producer = csurface->GetProducer();
    auto psurface = Surface::CreateSurfaceAsProducer(producer);
    EXPECT_NE(psurface, nullptr);

    uint32_t defaultHeight = 1280;
    uint32_t defaultWidth = 720;
    ScreenId virtualScreenId = rsClient->CreateVirtualScreen(
        "virtualScreenTest", defaultWidth, defaultHeight, psurface, INVALID_SCREEN_ID, -1);
    EXPECT_NE(virtualScreenId, INVALID_SCREEN_ID);

    int32_t modeIdx1 = 0;
    ScreenColorGamut mode = COLOR_GAMUT_NATIVE;
    int ret = rsClient->SetScreenColorGamut(virtualScreenId, modeIdx1);
    ret = rsClient->GetScreenColorGamut(virtualScreenId, mode);
}

/*
 * @tc.name: SetScreenCorrection Test
 * @tc.desc: SetScreenCorrection Test
 * @tc.type:FUNC
 * @tc.require: issuesI9K7SJ
*/
HWTEST_F(RSClientTest, SetScreenCorrectionTest001, TestSize.Level1)
{
    ScreenId screenId1 = rsClient->GetDefaultScreenId();
    uint32_t res = rsClient->SetScreenCorrection(screenId1, ScreenRotation::ROTATION_90);
    ASSERT_EQ(res, SUCCESS);
}

/*
 * @tc.name: SetVirtualMirrorScreenCanvasRotation Test
 * @tc.desc: SetVirtualMirrorScreenCanvasRotation Test
 * @tc.type:FUNC
 * @tc.require: issuesI9K7SJ
*/
HWTEST_F(RSClientTest, SetVirtualMirrorScreenCanvasRotationTest001, TestSize.Level1)
{
    auto csurface = IConsumerSurface::Create();
    EXPECT_NE(csurface, nullptr);
    auto producer = csurface->GetProducer();
    auto psurface = Surface::CreateSurfaceAsProducer(producer);
    EXPECT_NE(psurface, nullptr);
    uint32_t defaultHeight = 2772;
    uint32_t defaultWidth = 1344;
    ScreenId virtualScreenId1 = rsClient->CreateVirtualScreen(
        "virtualScreenTest", defaultWidth, defaultHeight, psurface, INVALID_SCREEN_ID, -1);
    EXPECT_NE(virtualScreenId1, INVALID_SCREEN_ID);
    EXPECT_EQ(rsClient->SetVirtualMirrorScreenCanvasRotation(virtualScreenId1, true), true);
    EXPECT_EQ(rsClient->SetVirtualMirrorScreenCanvasRotation(virtualScreenId1, false), true);
}

/**
 * @tc.name: RegisterUIExtensionCallback Test
 * @tc.desc: RegisterUIExtensionCallback, expected success when callback non-empty.
 * @tc.type:FUNC
 * @tc.require: issueIABHAX
 */
HWTEST_F(RSClientTest, RegisterUIExtensionCallbackTest001, TestSize.Level1)
{
    uint64_t userId1 = 0;
    UIExtensionCallback callback1 = [](std::shared_ptr<RSUIExtensionData>, uint64_t) {};
    EXPECT_EQ(rsClient->RegisterUIExtensionCallback(userId1, callback1),
        StatusCode::SUCCESS);
}

/**
 * @tc.name: RegisterUIExtensionCallback Test
 * @tc.desc: RegisterUIExtensionCallback, expected success when callback is empty.
 * @tc.type:FUNC
 * @tc.require: issueIABHAX
 */
HWTEST_F(RSClientTest, RegisterUIExtensionCallbackTest002, TestSize.Level1)
{
    uint64_t userId2 = 0;
    UIExtensionCallback callback2 = nullptr;
    EXPECT_NE(rsClient->RegisterUIExtensionCallback(userId2, callback2),
        StatusCode::INVALID_ARGUMENTS);
}

/*
 * @tc.name: SetVirtualMirrorScreenScaleMode Test
 * @tc.desc: SetVirtualMirrorScreenScaleMode Test
 * @tc.type:FUNC
 * @tc.require: issuesI9K7SJ
*/
HWTEST_F(RSClientTest, SetVirtualMirrorScreenScaleModeTest001, TestSize.Level1)
{
    auto csurface = IConsumerSurface::Create();
    EXPECT_NE(csurface, nullptr);

    auto producer = csurface->GetProducer();
    auto psurface = Surface::CreateSurfaceAsProducer(producer);
    EXPECT_NE(psurface, nullptr);

    uint32_t defaultHeight = 2772;
    uint32_t defaultWidth = 1344;
    ScreenId virtualScreenId1 = rsClient->CreateVirtualScreen(
        "virtualScreenTest", defaultWidth, defaultHeight, psurface, INVALID_SCREEN_ID, -1);
    EXPECT_NE(virtualScreenId1, INVALID_SCREEN_ID);
    EXPECT_EQ(rsClient->SetVirtualMirrorScreenScaleMode(virtualScreenId1, ScreenScaleMode::UNISCALE_MODE), true);
}
} // namespace Rosen
} // namespace OHOS