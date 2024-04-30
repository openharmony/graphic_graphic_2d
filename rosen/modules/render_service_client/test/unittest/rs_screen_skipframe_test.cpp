/*
 * Copyright (c) 2021-2023 Huawei Device Co., Ltd.
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

#include <cstdint>
#include <gtest/gtest.h>
#include <hilog/log.h>
#include <memory>
#include <unistd.h>
#include <iostream>

#include "transaction/rs_interfaces.h"

using namespace testing::ext;

namespace OHOS {
namespace Rosen {
class RSScreenSkipFrameTest : public testing::Test {
public:
    static constexpr HiviewDFX::HiLogLabel LOG_LABEL = { LOG_CORE, 0, "RSScreenSkipFrameTest" };

    static void SetUpTestCase()
    {
        rsInterfaces = &(RSInterfaces::GetInstance());
    }

    static void TearDownTestCase()
    {
        rsInterfaces = nullptr;
    }

    static inline RSInterfaces* rsInterfaces = nullptr;

    uint32_t ConvertScreenModeToRefreshRate(int32_t modeId)
    {
        uint32_t refreshRate = 0;
        switch (modeId) {
            case 0: // modeId 0 means 120hz
                refreshRate = 120; // modeId 0 means 120hz
                break;
            case 1: // modeId 1 means 90hz
                refreshRate = 90; // modeId 1 means 90hz
                break;
            case 2: // modeId 2 means 60hz
                refreshRate = 60; // modeId 2 means 60hz
                break;
            case 3: // modeId 3 means 30hz
                refreshRate = 30; // modeId 3 means 30hz
                break;
            default:
                refreshRate = 0;
                break;
        }
        return refreshRate;
    }

private:
    static constexpr uint32_t SET_REFRESHRATE_SLEEP_US = 50000;  // wait for refreshrate change
    uint32_t virtualScreenMirrorId = 1;
};

/*
* Function: SetScreenSkipFrameInterval
* Type: Function
* Rank: Important(2)
* EnvConditions: N/A
* CaseDescription: 1. call SetScreenSkipFrameInterval with screenId 0
*/
HWTEST_F(RSScreenSkipFrameTest, SetScreenSkipFrameInterval001, Function | SmallTest | Level2)
{
    ScreenId screenId = rsInterfaces->GetDefaultScreenId();
    EXPECT_NE(screenId, INVALID_SCREEN_ID);
    uint32_t skipFrameInterval = 1;
    std::cout << "Set screenId:" << 0 << ", skipFrameInterval:" << skipFrameInterval << std::endl;
    int32_t ret = rsInterfaces->SetScreenSkipFrameInterval(0, skipFrameInterval);
    if (screenId == 0) {
        EXPECT_EQ(ret, StatusCode::SUCCESS);
    } else {
        EXPECT_EQ(ret, StatusCode::SCREEN_NOT_FOUND);
    }
}

/*
* Function: SetScreenSkipFrameInterval
* Type: Function
* Rank: Important(2)
* EnvConditions: N/A
* CaseDescription: 1. call SetScreenSkipFrameInterval with invalid screenId
*/
HWTEST_F(RSScreenSkipFrameTest, SetScreenSkipFrameInterval002, Function | SmallTest | Level2)
{
    ScreenId screenId = INVALID_SCREEN_ID;
    uint32_t skipFrameInterval = 1;
    std::cout << "Set screenId:" << screenId << ", skipFrameInterval:" << skipFrameInterval << std::endl;
    int32_t ret = rsInterfaces->SetScreenSkipFrameInterval(screenId, skipFrameInterval);
    EXPECT_EQ(ret, StatusCode::SCREEN_NOT_FOUND);
}

/*
* Function: SetScreenSkipFrameInterval
* Type: Function
* Rank: Important(2)
* EnvConditions: N/A
* CaseDescription: 1. call Set primary screen with skipFrameInterval 0
*/
HWTEST_F(RSScreenSkipFrameTest, SetScreenSkipFrameInterval003, Function | SmallTest | Level2)
{
    ScreenId screenId = rsInterfaces->GetDefaultScreenId();
    EXPECT_NE(screenId, INVALID_SCREEN_ID);
    uint32_t skipFrameInterval = 0;
    std::cout << "Set screenId:" << screenId << ", skipFrameInterval:" << skipFrameInterval << std::endl;
    int32_t ret = rsInterfaces->SetScreenSkipFrameInterval(screenId, skipFrameInterval);
    EXPECT_EQ(ret, StatusCode::INVALID_ARGUMENTS);
}

/*
* Function: SetScreenSkipFrameInterval
* Type: Function
* Rank: Important(2)
* EnvConditions: N/A
* CaseDescription: 1. call Set primary screen with skipFrameInterval 1
*/
HWTEST_F(RSScreenSkipFrameTest, SetScreenSkipFrameInterval004, Function | SmallTest | Level2)
{
    ScreenId screenId = rsInterfaces->GetDefaultScreenId();
    EXPECT_NE(screenId, INVALID_SCREEN_ID);
    uint32_t skipFrameInterval = 1;
    std::cout << "Set screenId:" << screenId << ", skipFrameInterval:" << skipFrameInterval << std::endl;
    int32_t ret = rsInterfaces->SetScreenSkipFrameInterval(screenId, skipFrameInterval);
    EXPECT_EQ(ret, StatusCode::SUCCESS);
}

/*
* Function: SetScreenSkipFrameInterval
* Type: Function
* Rank: Important(1)
* EnvConditions: N/A
* CaseDescription: 1. Set primary screen with skipFrameInterval 2
*/
HWTEST_F(RSScreenSkipFrameTest, SetScreenSkipFrameInterval005, Function | SmallTest | Level1)
{
    ScreenId screenId = rsInterfaces->GetDefaultScreenId();
    EXPECT_NE(screenId, INVALID_SCREEN_ID);
    uint32_t skipFrameInterval = 2;
    std::cout << "Set screenId:" << screenId << ", skipFrameInterval:" << skipFrameInterval << std::endl;
    int32_t ret = rsInterfaces->SetScreenSkipFrameInterval(screenId, skipFrameInterval);
    EXPECT_EQ(ret, StatusCode::SUCCESS);

    // recover
    ret = rsInterfaces->SetScreenSkipFrameInterval(screenId, 1);
    EXPECT_EQ(ret, StatusCode::SUCCESS);
}

/*
* Function: SetScreenSkipFrameInterval
* Type: Function
* Rank: Important(1)
* EnvConditions: N/A
* CaseDescription: 1. Set primary screen with the most screen refreshrate
*/
HWTEST_F(RSScreenSkipFrameTest, SetScreenSkipFrameInterval006, Function | SmallTest | Level1)
{
    ScreenId screenId = rsInterfaces->GetDefaultScreenId();
    EXPECT_NE(screenId, INVALID_SCREEN_ID);
    auto formerModeInfo = rsInterfaces->GetScreenActiveMode(screenId);
    uint32_t skipFrameInterval = formerModeInfo.GetScreenRefreshRate();
    std::cout << "Set screenId:" << screenId << ", skipFrameInterval:" << skipFrameInterval << std::endl;
    int32_t ret = rsInterfaces->SetScreenSkipFrameInterval(screenId, skipFrameInterval);
    EXPECT_EQ(ret, StatusCode::SUCCESS);

    // recover
    ret = rsInterfaces->SetScreenSkipFrameInterval(screenId, 1);
    EXPECT_EQ(ret, StatusCode::SUCCESS);
}

/*
* Function: SetScreenSkipFrameInterval
* Type: Function
* Rank: Important(1)
* EnvConditions: N/A
* CaseDescription: 1. Set primary screen with the most screen refreshrate pluse 1
*/
HWTEST_F(RSScreenSkipFrameTest, SetScreenSkipFrameInterval007, Function | SmallTest | Level1)
{
    ScreenId screenId = rsInterfaces->GetDefaultScreenId();
    EXPECT_NE(screenId, INVALID_SCREEN_ID);
    auto formerModeInfo = rsInterfaces->GetScreenActiveMode(screenId);
    uint32_t skipFrameInterval = formerModeInfo.GetScreenRefreshRate() + 1;
    std::cout << "Set screenId:" << screenId << ", skipFrameInterval:" << skipFrameInterval << std::endl;
    int32_t ret = rsInterfaces->SetScreenSkipFrameInterval(screenId, skipFrameInterval);
    EXPECT_EQ(ret, StatusCode::INVALID_ARGUMENTS);

    // recover
    ret = rsInterfaces->SetScreenSkipFrameInterval(screenId, 1);
    EXPECT_EQ(ret, StatusCode::SUCCESS);
}

/*
* Function: SetScreenSkipFrameInterval
* Type: Function
* Rank: Important(1)
* EnvConditions: N/A
* CaseDescription: 1. Set virtual screen with skipFrameInterval 0
*/
HWTEST_F(RSScreenSkipFrameTest, SetScreenSkipFrameInterval008, Function | SmallTest | Level1)
{
    auto csurface = IConsumerSurface::Create();
    EXPECT_NE(csurface, nullptr);
    auto producer = csurface->GetProducer();
    auto psurface = Surface::CreateSurfaceAsProducer(producer);
    uint32_t defaultWidth = 720;
    uint32_t defaultHeight = 1280;
    EXPECT_NE(psurface, nullptr);

    ScreenId virtualScreenId = rsInterfaces->CreateVirtualScreen(
        "virtualScreen01", defaultWidth, defaultHeight, psurface, INVALID_SCREEN_ID, -1);
    EXPECT_NE(virtualScreenId, INVALID_SCREEN_ID);

    uint32_t skipFrameInterval = 0;
    std::cout << "Set virtualScreenId:" << virtualScreenId << ", skipFrameInterval:" << skipFrameInterval << std::endl;
    int32_t ret = rsInterfaces->SetScreenSkipFrameInterval(virtualScreenId, skipFrameInterval);
    EXPECT_EQ(ret, StatusCode::INVALID_ARGUMENTS);
}

/*
* Function: SetScreenSkipFrameInterval
* Type: Function
* Rank: Important(1)
* EnvConditions: N/A
* CaseDescription: 1. Set virtual screen with skipFrameInterval 1
*/
HWTEST_F(RSScreenSkipFrameTest, SetScreenSkipFrameInterval009, Function | SmallTest | Level1)
{
    auto csurface = IConsumerSurface::Create();
    EXPECT_NE(csurface, nullptr);
    auto producer = csurface->GetProducer();
    auto psurface = Surface::CreateSurfaceAsProducer(producer);
    uint32_t defaultWidth = 720;
    uint32_t defaultHeight = 1280;
    EXPECT_NE(psurface, nullptr);

    ScreenId virtualScreenId = rsInterfaces->CreateVirtualScreen(
        "virtualScreen02", defaultWidth, defaultHeight, psurface, INVALID_SCREEN_ID, -1);
    EXPECT_NE(virtualScreenId, INVALID_SCREEN_ID);

    uint32_t skipFrameInterval = 1;
    std::cout << "Set virtualScreenId:" << virtualScreenId << ", skipFrameInterval:" << skipFrameInterval << std::endl;
    int32_t ret = rsInterfaces->SetScreenSkipFrameInterval(virtualScreenId, skipFrameInterval);
    EXPECT_EQ(ret, StatusCode::SUCCESS);
}

/*
* Function: SetScreenSkipFrameInterval
* Type: Function
* Rank: Important(1)
* EnvConditions: N/A
* CaseDescription: 1. Set virtual screen with skipFrameInterval 2
*/
HWTEST_F(RSScreenSkipFrameTest, SetScreenSkipFrameInterval010, Function | SmallTest | Level1)
{
    auto csurface = IConsumerSurface::Create();
    EXPECT_NE(csurface, nullptr);
    auto producer = csurface->GetProducer();
    auto psurface = Surface::CreateSurfaceAsProducer(producer);
    uint32_t defaultWidth = 720;
    uint32_t defaultHeight = 1280;
    EXPECT_NE(psurface, nullptr);

    ScreenId virtualScreenId = rsInterfaces->CreateVirtualScreen(
        "virtualScreen03", defaultWidth, defaultHeight, psurface, INVALID_SCREEN_ID, -1);
    EXPECT_NE(virtualScreenId, INVALID_SCREEN_ID);

    uint32_t skipFrameInterval = 2;
    std::cout << "Set virtualScreenId:" << virtualScreenId << ", skipFrameInterval:" << skipFrameInterval << std::endl;
    int32_t ret = rsInterfaces->SetScreenSkipFrameInterval(virtualScreenId, skipFrameInterval);
    EXPECT_EQ(ret, StatusCode::SUCCESS);

    // recover
    ret = rsInterfaces->SetScreenSkipFrameInterval(virtualScreenId, 1);
    EXPECT_EQ(ret, StatusCode::SUCCESS);
}

/*
* Function: SetScreenSkipFrameInterval
* Type: Function
* Rank: Important(1)
* EnvConditions: N/A
* CaseDescription: 1. Set virtual screen with the most screen refreshrate
*/
HWTEST_F(RSScreenSkipFrameTest, SetScreenSkipFrameInterval011, Function | SmallTest | Level1)
{
    auto csurface = IConsumerSurface::Create();
    EXPECT_NE(csurface, nullptr);
    auto producer = csurface->GetProducer();
    auto psurface = Surface::CreateSurfaceAsProducer(producer);
    uint32_t defaultWidth = 720;
    uint32_t defaultHeight = 1280;
    EXPECT_NE(psurface, nullptr);

    ScreenId virtualScreenId = rsInterfaces->CreateVirtualScreen(
        "virtualScreen04", defaultWidth, defaultHeight, psurface, INVALID_SCREEN_ID, -1);
    EXPECT_NE(virtualScreenId, INVALID_SCREEN_ID);
    ScreenId screenId = rsInterfaces->GetDefaultScreenId();
    EXPECT_NE(screenId, INVALID_SCREEN_ID);
    auto formerModeInfo = rsInterfaces->GetScreenActiveMode(screenId);

    uint32_t skipFrameInterval = formerModeInfo.GetScreenRefreshRate();
    std::cout << "Set virtualScreenId:" << virtualScreenId << ", skipFrameInterval:" << skipFrameInterval << std::endl;
    int32_t ret = rsInterfaces->SetScreenSkipFrameInterval(virtualScreenId, skipFrameInterval);
    EXPECT_EQ(ret, StatusCode::SUCCESS);

    // recover
    ret = rsInterfaces->SetScreenSkipFrameInterval(virtualScreenId, 1);
    EXPECT_EQ(ret, StatusCode::SUCCESS);
}

/*
* Function: SetScreenSkipFrameInterval
* Type: Function
* Rank: Important(1)
* EnvConditions: N/A
* CaseDescription: 1. Set virtual screen with the most screen refreshrate pluse 1
*/
HWTEST_F(RSScreenSkipFrameTest, SetScreenSkipFrameInterval012, Function | SmallTest | Level1)
{
    auto csurface = IConsumerSurface::Create();
    EXPECT_NE(csurface, nullptr);
    auto producer = csurface->GetProducer();
    auto psurface = Surface::CreateSurfaceAsProducer(producer);
    uint32_t defaultWidth = 720;
    uint32_t defaultHeight = 1280;
    EXPECT_NE(psurface, nullptr);

    ScreenId virtualScreenId = rsInterfaces->CreateVirtualScreen(
        "virtualScreen05", defaultWidth, defaultHeight, psurface, INVALID_SCREEN_ID, -1);
    EXPECT_NE(virtualScreenId, INVALID_SCREEN_ID);
    ScreenId screenId = rsInterfaces->GetDefaultScreenId();
    EXPECT_NE(screenId, INVALID_SCREEN_ID);
    auto formerModeInfo = rsInterfaces->GetScreenActiveMode(screenId);

    uint32_t skipFrameInterval = formerModeInfo.GetScreenRefreshRate() + 1;
    std::cout << "Set virtualScreenId:" << virtualScreenId << ", skipFrameInterval:" << skipFrameInterval << std::endl;
    int32_t ret = rsInterfaces->SetScreenSkipFrameInterval(virtualScreenId, skipFrameInterval);
    EXPECT_EQ(ret, StatusCode::INVALID_ARGUMENTS);

    // recover
    ret = rsInterfaces->SetScreenSkipFrameInterval(virtualScreenId, 1);
    EXPECT_EQ(ret, StatusCode::SUCCESS);
}

/*
* Function: SetScreenSkipFrameInterval
* Type: Function
* Rank: Important(1)
* EnvConditions: N/A
* CaseDescription: 1. Set virtual screen with skipFrameInterval 99999
*/
HWTEST_F(RSScreenSkipFrameTest, SetScreenSkipFrameInterval013, Function | SmallTest | Level1)
{
    auto csurface = IConsumerSurface::Create();
    EXPECT_NE(csurface, nullptr);
    auto producer = csurface->GetProducer();
    auto psurface = Surface::CreateSurfaceAsProducer(producer);
    uint32_t defaultWidth = 720;
    uint32_t defaultHeight = 1280;
    EXPECT_NE(psurface, nullptr);

    ScreenId virtualScreenId = rsInterfaces->CreateVirtualScreen(
        "virtualScreen06", defaultWidth, defaultHeight, psurface, INVALID_SCREEN_ID, -1);
    EXPECT_NE(virtualScreenId, INVALID_SCREEN_ID);

    uint32_t skipFrameInterval = 99999;
    std::cout << "Set virtualScreenId:" << virtualScreenId << ", skipFrameInterval:" << skipFrameInterval << std::endl;
    int32_t ret = rsInterfaces->SetScreenSkipFrameInterval(virtualScreenId, skipFrameInterval);
    EXPECT_EQ(ret, StatusCode::INVALID_ARGUMENTS);

    // recover
    ret = rsInterfaces->SetScreenSkipFrameInterval(virtualScreenId, 1);
    EXPECT_EQ(ret, StatusCode::SUCCESS);
}

/*
* Function: SetScreenSkipFrameInterval
* Type: Function
* Rank: Important(1)
* EnvConditions: N/A
* CaseDescription: 1. Set virtual screen with skipFrameInterval 2.1
*/
HWTEST_F(RSScreenSkipFrameTest, SetScreenSkipFrameInterval014, Function | SmallTest | Level1)
{
    auto csurface = IConsumerSurface::Create();
    EXPECT_NE(csurface, nullptr);
    auto producer = csurface->GetProducer();
    auto psurface = Surface::CreateSurfaceAsProducer(producer);
    uint32_t defaultWidth = 720;
    uint32_t defaultHeight = 1280;
    EXPECT_NE(psurface, nullptr);

    ScreenId virtualScreenId = rsInterfaces->CreateVirtualScreen(
        "virtualScreen07", defaultWidth, defaultHeight, psurface, INVALID_SCREEN_ID, -1);
    EXPECT_NE(virtualScreenId, INVALID_SCREEN_ID);

    float skipFrameInterval = 2.1;
    std::cout << "Set virtualScreenId:" << virtualScreenId << ", skipFrameInterval:" << skipFrameInterval << std::endl;
    int32_t ret = rsInterfaces->SetScreenSkipFrameInterval(virtualScreenId, skipFrameInterval);
    EXPECT_EQ(ret, StatusCode::SUCCESS);

    // recover
    ret = rsInterfaces->SetScreenSkipFrameInterval(virtualScreenId, 1);
    EXPECT_EQ(ret, StatusCode::SUCCESS);
}

/*
* Function: SetScreenSkipFrameInterval
* Type: Function
* Rank: Important(1)
* EnvConditions: N/A
* CaseDescription: 1. Set virtual screen and primary screen with skipFrameInterval 2
*/
HWTEST_F(RSScreenSkipFrameTest, SetScreenSkipFrameInterval015, Function | SmallTest | Level1)
{
    auto csurface = IConsumerSurface::Create();
    EXPECT_NE(csurface, nullptr);
    auto producer = csurface->GetProducer();
    auto psurface = Surface::CreateSurfaceAsProducer(producer);
    uint32_t defaultWidth = 720;
    uint32_t defaultHeight = 1280;
    EXPECT_NE(psurface, nullptr);

    ScreenId virtualScreenId = rsInterfaces->CreateVirtualScreen(
        "virtualScreen08", defaultWidth, defaultHeight, psurface, INVALID_SCREEN_ID, -1);
    EXPECT_NE(virtualScreenId, INVALID_SCREEN_ID);
    ScreenId screenId = rsInterfaces->GetDefaultScreenId();
    EXPECT_NE(screenId, INVALID_SCREEN_ID);

    uint32_t skipFrameInterval = 2;
    std::cout << "Set virtualScreenId:" << virtualScreenId << ", skipFrameInterval:" << skipFrameInterval << std::endl;
    int32_t ret = rsInterfaces->SetScreenSkipFrameInterval(virtualScreenId, skipFrameInterval);
    EXPECT_EQ(ret, StatusCode::SUCCESS);
    std::cout << "Set screenId:" << screenId << ", skipFrameInterval:" << skipFrameInterval << std::endl;
    ret = rsInterfaces->SetScreenSkipFrameInterval(screenId, skipFrameInterval);
    EXPECT_EQ(ret, StatusCode::SUCCESS);

    // recover
    ret = rsInterfaces->SetScreenSkipFrameInterval(virtualScreenId, 1);
    EXPECT_EQ(ret, StatusCode::SUCCESS);
    ret = rsInterfaces->SetScreenSkipFrameInterval(screenId, 1);
    EXPECT_EQ(ret, StatusCode::SUCCESS);
}
} // namespace Rosen
} // namespace OHOS
