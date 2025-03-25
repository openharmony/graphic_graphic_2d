/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
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

#include "accesstoken_kit.h"
#include "nativetoken_kit.h"
#include "token_setproc.h"

using namespace testing::ext;

namespace OHOS {
namespace Rosen {
class RSVirtualScreenRefreshRateTest : public testing::Test {
public:
    static constexpr HiviewDFX::HiLogLabel LOG_LABEL = { LOG_CORE, 0, "RSVirtualScreenRefreshRateTest" };

    static void SetUpTestCase()
    {
        rsInterfaces = &(RSInterfaces::GetInstance());
        uint64_t tokenId;
        const char* perms[1];
        perms[0] = "ohos.permission.CAPTURE_SCREEN";
        NativeTokenInfoParams infoInstance = {
            .dcapsNum = 0,
            .permsNum = 1,
            .aclsNum = 0,
            .dcaps = NULL,
            .perms = perms,
            .acls = NULL,
            .processName = "foundation",
            .aplStr = "system_basic",
        };
        tokenId = GetAccessTokenId(&infoInstance);
        SetSelfTokenID(tokenId);
        OHOS::Security::AccessToken::AccessTokenKit::ReloadNativeTokenInfo();
    }

    static void TearDownTestCase()
    {
        rsInterfaces = nullptr;
    }

    static inline RSInterfaces* rsInterfaces = nullptr;
};

/*
* Function: SetVirtualScreenRefreshRate
* Type: Function
* Rank: Important(1)
* EnvConditions: N/A
* CaseDescription: 1. SetVirtualScreenRefreshRate UINT32_MAX
*/
HWTEST_F(RSVirtualScreenRefreshRateTest, SetVirtualScreenRefreshRate001, Function | SmallTest | Level1)
{
    auto csurface = IConsumerSurface::Create();
    EXPECT_NE(csurface, nullptr);
    auto producer = csurface->GetProducer();
    auto psurface = Surface::CreateSurfaceAsProducer(producer);
    uint32_t defaultWidth = 720; // 720px
    uint32_t defaultHeight = 1280; // 1280px
    EXPECT_NE(psurface, nullptr);

    ScreenId virtualScreenId = rsInterfaces->CreateVirtualScreen(
        "virtualScreen01", defaultWidth, defaultHeight, psurface, INVALID_SCREEN_ID, -1);
    EXPECT_NE(virtualScreenId, INVALID_SCREEN_ID);

    uint32_t maxRefreshRate = UINT32_MAX;
    uint32_t actualRefreshRate = 0;
    std::cout << "Set virtualScreenId:" << virtualScreenId << ", maxRefreshRate:" << maxRefreshRate << std::endl;
    int32_t ret = rsInterfaces->SetVirtualScreenRefreshRate(virtualScreenId, maxRefreshRate, actualRefreshRate);
    EXPECT_EQ(actualRefreshRate, 120);
    EXPECT_EQ(ret, StatusCode::SUCCESS);

    // recover
    ret = rsInterfaces->SetVirtualScreenRefreshRate(virtualScreenId, 60, actualRefreshRate); // 60hz
    EXPECT_EQ(actualRefreshRate, 60); // 60hz
    EXPECT_EQ(ret, StatusCode::SUCCESS);
}

/*
* Function: SetVirtualScreenRefreshRate
* Type: Function
* Rank: Important(1)
* EnvConditions: N/A
* CaseDescription: 1. SetVirtualScreenRefreshRate 0
*/
HWTEST_F(RSVirtualScreenRefreshRateTest, SetVirtualScreenRefreshRate002, Function | SmallTest | Level1)
{
    auto csurface = IConsumerSurface::Create();
    EXPECT_NE(csurface, nullptr);
    auto producer = csurface->GetProducer();
    auto psurface = Surface::CreateSurfaceAsProducer(producer);
    uint32_t defaultWidth = 720; // 720px
    uint32_t defaultHeight = 1280; // 1280px
    EXPECT_NE(psurface, nullptr);

    ScreenId virtualScreenId = rsInterfaces->CreateVirtualScreen(
        "virtualScreen01", defaultWidth, defaultHeight, psurface, INVALID_SCREEN_ID, -1);
    EXPECT_NE(virtualScreenId, INVALID_SCREEN_ID);

    uint32_t maxRefreshRate = 0;
    uint32_t actualRefreshRate = 0;
    std::cout << "Set virtualScreenId:" << virtualScreenId << ", maxRefreshRate:" << maxRefreshRate << std::endl;
    int32_t ret = rsInterfaces->SetVirtualScreenRefreshRate(virtualScreenId, maxRefreshRate, actualRefreshRate);
    EXPECT_NE(ret, StatusCode::SUCCESS);

    // recover
    ret = rsInterfaces->SetVirtualScreenRefreshRate(virtualScreenId, 60, actualRefreshRate); // 60hz
    EXPECT_EQ(ret, StatusCode::SUCCESS);
}

/*
* Function: SetVirtualScreenRefreshRate
* Type: Function
* Rank: Important(1)
* EnvConditions: N/A
* CaseDescription: 1. SetVirtualScreenRefreshRate 61
*/
HWTEST_F(RSVirtualScreenRefreshRateTest, SetVirtualScreenRefreshRate003, Function | SmallTest | Level1)
{
    auto csurface = IConsumerSurface::Create();
    EXPECT_NE(csurface, nullptr);
    auto producer = csurface->GetProducer();
    auto psurface = Surface::CreateSurfaceAsProducer(producer);
    uint32_t defaultWidth = 720; // 720px
    uint32_t defaultHeight = 1280; // 1280px
    EXPECT_NE(psurface, nullptr);

    ScreenId virtualScreenId = rsInterfaces->CreateVirtualScreen(
        "virtualScreen01", defaultWidth, defaultHeight, psurface, INVALID_SCREEN_ID, -1);
    EXPECT_NE(virtualScreenId, INVALID_SCREEN_ID);

    uint32_t maxRefreshRate = 61; // 61hz
    uint32_t actualRefreshRate = 0;
    std::cout << "Set virtualScreenId:" << virtualScreenId << ", maxRefreshRate:" << maxRefreshRate << std::endl;
    int32_t ret = rsInterfaces->SetVirtualScreenRefreshRate(virtualScreenId, maxRefreshRate, actualRefreshRate);
    EXPECT_EQ(actualRefreshRate, 61);
    EXPECT_EQ(ret, StatusCode::SUCCESS);

    // recover
    ret = rsInterfaces->SetVirtualScreenRefreshRate(virtualScreenId, 60, actualRefreshRate); // 60hz
    EXPECT_EQ(actualRefreshRate, 60); // 60hz
    EXPECT_EQ(ret, StatusCode::SUCCESS);
}

/*
* Function: SetVirtualScreenRefreshRate
* Type: Function
* Rank: Important(1)
* EnvConditions: N/A
* CaseDescription: 1. SetVirtualScreenRefreshRate 1~60
*/
HWTEST_F(RSVirtualScreenRefreshRateTest, SetVirtualScreenRefreshRate004, Function | SmallTest | Level1)
{
    auto csurface = IConsumerSurface::Create();
    EXPECT_NE(csurface, nullptr);
    auto producer = csurface->GetProducer();
    auto psurface = Surface::CreateSurfaceAsProducer(producer);
    uint32_t defaultWidth = 720; // 720px
    uint32_t defaultHeight = 1280; // 1280px
    EXPECT_NE(psurface, nullptr);

    ScreenId virtualScreenId = rsInterfaces->CreateVirtualScreen(
        "virtualScreen01", defaultWidth, defaultHeight, psurface, INVALID_SCREEN_ID, -1);
    EXPECT_NE(virtualScreenId, INVALID_SCREEN_ID);

    for (uint32_t maxRefreshRate = 1; maxRefreshRate <= 60; maxRefreshRate++) { // 60hz
        uint32_t actualRefreshRate = 0;
        std::cout << "Set virtualScreenId:" << virtualScreenId << ", maxRefreshRate:" << maxRefreshRate << std::endl;
        int32_t ret = rsInterfaces->SetVirtualScreenRefreshRate(virtualScreenId, maxRefreshRate, actualRefreshRate);
        EXPECT_EQ(ret, StatusCode::SUCCESS);
        EXPECT_EQ(actualRefreshRate, maxRefreshRate);
    }

    // recover
    uint32_t actualRefreshRate = 0;
    int32_t ret = rsInterfaces->SetVirtualScreenRefreshRate(virtualScreenId, 60, actualRefreshRate); // 60hz
    EXPECT_EQ(actualRefreshRate, 60); // 60hz
    EXPECT_EQ(ret, StatusCode::SUCCESS);
}

/*
* Function: SetVirtualScreenRefreshRate
* Type: Function
* Rank: Important(1)
* EnvConditions: N/A
* CaseDescription: 1. SetVirtualScreenRefreshRate with physical screen id
*/
HWTEST_F(RSVirtualScreenRefreshRateTest, SetVirtualScreenRefreshRate005, Function | SmallTest | Level1)
{
    ScreenId defaultScreenId = rsInterfaces->GetDefaultScreenId();
    EXPECT_NE(defaultScreenId, INVALID_SCREEN_ID);
    uint32_t maxRefreshRate = 30; // 30hz
    uint32_t actualRefreshRate = 0;
    std::cout << "Set defaultScreenId:" << defaultScreenId << ", maxRefreshRate:" << maxRefreshRate << std::endl;
    int32_t ret = rsInterfaces->SetVirtualScreenRefreshRate(defaultScreenId, maxRefreshRate, actualRefreshRate);
    EXPECT_EQ(ret, StatusCode::SCREEN_NOT_FOUND);
}

/*
* Function: SetVirtualScreenRefreshRate
* Type: Function
* Rank: Important(1)
* EnvConditions: N/A
* CaseDescription: 1. SetVirtualScreenRefreshRate UINT32_MAX+1
*/
HWTEST_F(RSVirtualScreenRefreshRateTest, SetVirtualScreenRefreshRate006, Function | SmallTest | Level1)
{
    auto csurface = IConsumerSurface::Create();
    auto producer = csurface->GetProducer();
    auto psurface = Surface::CreateSurfaceAsProducer(producer);
    uint32_t defaultWidth = 720; // 720px
    uint32_t defaultHeight = 1280; // 1280px

    ScreenId virtualScreenId = rsInterfaces->CreateVirtualScreen(
        "virtualScreen01", defaultWidth, defaultHeight, psurface, INVALID_SCREEN_ID, -1);
    EXPECT_NE(virtualScreenId, INVALID_SCREEN_ID);

    uint32_t maxRefreshRate = UINT32_MAX + 1;
    uint32_t actualRefreshRate = 0;
    std::cout << "Set virtualScreenId:" << virtualScreenId << ", maxRefreshRate:" << maxRefreshRate << std::endl;
    int32_t ret = rsInterfaces->SetVirtualScreenRefreshRate(virtualScreenId, maxRefreshRate, actualRefreshRate);
    EXPECT_EQ(actualRefreshRate, 0);
    EXPECT_NE(ret, StatusCode::SUCCESS);

    // recover
    ret = rsInterfaces->SetVirtualScreenRefreshRate(virtualScreenId, 60, actualRefreshRate); // 60hz
    EXPECT_EQ(actualRefreshRate, 60); // 60hz
    EXPECT_EQ(ret, StatusCode::SUCCESS);
}
} // namespace Rosen
} // namespace OHOS
