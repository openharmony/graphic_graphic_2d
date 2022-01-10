/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
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

#include "screen_manager_test.h"

namespace OHOS {
namespace Rosen {
using namespace HiviewDFX;
void RSScreenManagerTest::SetUpTestCase()
{
    screenManager = CreateOrGetScreenManager();
    if (screenManager->Init() == false) {
        HiLog::Error(LOG_LABEL, "%s: Failed to Init screenManager instance.", __func__);
    }
    cSurface = Surface::CreateSurfaceAsConsumer();
    producer = cSurface->GetProducer();
    pSurface = Surface::CreateSurfaceAsProducer(producer);
}

void RSScreenManagerTest::TearDownTestCase()
{
    cSurface = nullptr;
    producer = nullptr;
    pSurface = nullptr;
}

namespace {
HWTEST_F(RSScreenManagerTest, GetDefaultScreenId, testing::ext::TestSize.Level0)
{
    screenId = screenManager->GetDefaultScreenId();
    ASSERT_NE(screenId,  OHOS::Rosen::INVALID_SCREEN_ID);
}

HWTEST_F(RSScreenManagerTest, CreateVirtualScreen, testing::ext::TestSize.Level1)
{
    ScreenId virtualScreenId = screenManager->CreateVirtualScreen("testVirtualScreen", 100, 100, pSurface, 0, 0);
    ASSERT_NE(virtualScreenId,  OHOS::Rosen::INVALID_SCREEN_ID);
    screenManager->RemoveVirtualScreen(virtualScreenId);
}

HWTEST_F(RSScreenManagerTest, GetScreenActiveMode, testing::ext::TestSize.Level1)
{
    RSScreenModeInfo screenModeInfo0;
    screenManager->GetScreenActiveMode(screenId, screenModeInfo0);
    ASSERT_NE(screenModeInfo0.GetScreenWidth(), -1);
    ASSERT_NE(screenModeInfo0.GetScreenHeight(), -1);
    ASSERT_NE(screenModeInfo0.GetScreenFreshRate(), 0);
    ASSERT_NE(screenModeInfo0.GetScreenModeId(), -1);
    RSScreenModeInfo screenModeInfo1;
    screenManager->GetScreenActiveMode(OHOS::Rosen::INVALID_SCREEN_ID, screenModeInfo1);
    ASSERT_EQ(screenModeInfo1.GetScreenWidth(), -1);
    ASSERT_EQ(screenModeInfo1.GetScreenHeight(), -1);
    ASSERT_EQ(screenModeInfo1.GetScreenFreshRate(), 0);
    ASSERT_EQ(screenModeInfo1.GetScreenModeId(), -1);
}

HWTEST_F(RSScreenManagerTest, GetScreenSupportedModes, testing::ext::TestSize.Level2)
{
    std::vector<RSScreenModeInfo> screenSupportedModes0;
    screenSupportedModes0 = screenManager->GetScreenSupportedModes(screenId);
    ASSERT_NE(screenSupportedModes0.size(), 0);
    ASSERT_NE(screenSupportedModes0[0].GetScreenWidth(), -1);
    ASSERT_NE(screenSupportedModes0[0].GetScreenHeight(), -1);
    ASSERT_NE(screenSupportedModes0[0].GetScreenFreshRate(), 0);
    ASSERT_NE(screenSupportedModes0[0].GetScreenModeId(), -1);
    std::vector<RSScreenModeInfo> screenSupportedModes1;
    screenSupportedModes1 = screenManager->GetScreenSupportedModes(OHOS::Rosen::INVALID_SCREEN_ID);
    ASSERT_EQ(screenSupportedModes1.size(), 0);
}

HWTEST_F(RSScreenManagerTest, GetScreenCapability, testing::ext::TestSize.Level2)
{
    RSScreenCapability screenCapability0;
    screenCapability0 = screenManager->GetScreenCapability(screenId);
    ASSERT_NE(screenCapability0.GetType(),  OHOS::Rosen::DISP_INVALID);
    RSScreenCapability screenCapability1;
    screenCapability1 = screenManager->GetScreenCapability(OHOS::Rosen::INVALID_SCREEN_ID);
    ASSERT_EQ(screenCapability1.GetType(),  OHOS::Rosen::DISP_INVALID);
}

HWTEST_F(RSScreenManagerTest, GetScreenData, testing::ext::TestSize.Level2)
{
    RSScreenData screenData0;
    screenData0 = screenManager->GetScreenData(screenId);
    ASSERT_NE(screenData0.GetSupportModeInfo().size(), 0);
    RSScreenData screenData1;
    screenData1 = screenManager->GetScreenData(OHOS::Rosen::INVALID_SCREEN_ID);
    ASSERT_EQ(screenData1.GetSupportModeInfo().size(), 0);
}

HWTEST_F(RSScreenManagerTest, SetScreenActiveMode, testing::ext::TestSize.Level2)
{
    screenManager->SetScreenActiveMode(screenId, 1);
    RSScreenModeInfo screenModeInfo0;
    screenManager->GetScreenActiveMode(screenId, screenModeInfo0);
    // TODO: ASSERT_NE
    RSScreenModeInfo screenModeInfo1;
    std::vector<RSScreenModeInfo> screenSupportedModes;
    screenSupportedModes = screenManager->GetScreenSupportedModes(screenId);
    screenManager->SetScreenActiveMode(screenId, screenSupportedModes.size()+3);
    screenManager->GetScreenActiveMode(screenId, screenModeInfo1);
    // TODO: ASSERT_EQ
}

HWTEST_F(RSScreenManagerTest, GetScreenDump, testing::ext::TestSize.Level2)
{
    std::string dumpString;
    screenManager->DisplayDump(dumpString);
    screenManager->SurfaceDump(dumpString);
    std::cout<<dumpString;
}
}
} // namespace Rosen
} // namespace OHOS
