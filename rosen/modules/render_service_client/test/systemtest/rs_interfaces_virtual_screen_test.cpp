/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
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

// gtest
#include <gtest/gtest.h>
#include "future.h"
#include "rs_interfaces_test_utils.h"
#include "screen.h"
#include "transaction/rs_interfaces.h"
#include "transaction/rs_transaction.h"
#include "window.h"
#include "window_option.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
class RSInterfacesVirtualScreenTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
    static sptr<Display> defaultDisplay_;
    static DisplayId defaultDisplayId_;
    static ScreenId defaultScreenId_;
    static std::string defaultName_;
    static uint32_t defaultWidth_;
    static uint32_t defaultHeight_;
    static float defaultDensity_;
    static int32_t defaultFlags_;
    static VirtualScreenOption defaultOption_;
    static constexpr uint32_t TEST_SLEEP_S = 1; // test sleep time
};

sptr<Display> RSInterfacesVirtualScreenTest::defaultDisplay_ = nullptr;
DisplayId RSInterfacesVirtualScreenTest::defaultDisplayId_ = DISPLAY_ID_INVALID;
ScreenId RSInterfacesVirtualScreenTest::defaultScreenId_ = INVALID_SCREEN_ID;
std::string RSInterfacesVirtualScreenTest::defaultName_ = "virtualScreen01";
uint32_t RSInterfacesVirtualScreenTest::defaultWidth_ = 480;
uint32_t RSInterfacesVirtualScreenTest::defaultHeight_ = 320;
float RSInterfacesVirtualScreenTest::defaultDensity_ = 2.0;
int32_t RSInterfacesVirtualScreenTest::defaultFlags_ = 0;
VirtualScreenOption RSInterfacesVirtualScreenTest::defaultOption_ = {
    defaultName_, defaultWidth_, defaultHeight_, defaultDensity_, nullptr, defaultFlags_
};

void RSInterfacesVirtualScreenTest::SetUpTestCase()
{
    defaultDisplay_ = DisplayManager::GetInstance().GetDefaultDisplay();
    defaultDisplayId_ = defaultDisplay_->GetId();
    defaultScreenId_ = defaultDisplay_->GetScreenId();
    defaultWidth_ = defaultDisplay_->GetWidth();
    defaultHeight_ = defaultDisplay_->GetHeight();
    defaultOption_.width_ = defaultWidth_;
    defaultOption_.height_ = defaultHeight_;
}

void RSInterfacesVirtualScreenTest::TearDownTestCase()
{
}

void RSInterfacesVirtualScreenTest::SetUp()
{
}

void RSInterfacesVirtualScreenTest::TearDown()
{
}

namespace {
/**
 * @tc.name: VirtualScreenTest
 * @tc.desc: Test VirtualScreen SetScreenSkipFrameInterval
 * @tc.type: FUNC
 */
HWTEST_F(RSInterfacesVirtualScreenTest, VirtualScreenTest, Function | MediumTest | Level2)
{
    RSInterfacesTestUtils utils;
    ASSERT_TRUE(utils.CreateSurface());
    defaultOption_.surface_ = utils.pSurface_;
    defaultOption_.isForShot_ = false;

    ScreenId virtualScreenId = ScreenManager::GetInstance().CreateVirtualScreen(defaultOption_);
    sleep(TEST_SLEEP_S);

    std::vector<ScreenId> mirrorIds;
    mirrorIds.push_back(virtualScreenId);
    ScreenId screenGroupId;
    ScreenManager::GetInstance().MakeMirror(defaultScreenId_, mirrorIds, screenGroupId);

    uint32_t virtualScreenSkipFrameInterval = 2;
    auto ids = RSInterfaces::GetInstance().GetAllScreenIds();
    ScreenId defaultScreenId = RSInterfaces::GetInstance().GetDefaultScreenId();
    std::cout << "ids.size = " << ids.size() << ", defaultScreenId:" << defaultScreenId << std::endl;
    for (ScreenId screenId : ids) {
        if (screenId == defaultScreenId) {
            continue;
        }
        int32_t ret = RSInterfaces::GetInstance().SetScreenSkipFrameInterval(screenId, virtualScreenSkipFrameInterval);
        EXPECT_EQ(ret, StatusCode::SUCCESS);
        std::cout << "Set Screen:" << screenId << ", SkipFrameInterval:" << virtualScreenSkipFrameInterval << std::endl;
    }

    std::cout << "sleeping for 30s ..." << std::endl;
    sleep(30 * TEST_SLEEP_S);

    // recover
    for (ScreenId screenId : ids) {
        int32_t ret = RSInterfaces::GetInstance().SetScreenSkipFrameInterval(screenId, 1);
        EXPECT_EQ(ret, StatusCode::SUCCESS);
        std::cout << "recover Screen:" << screenId << ", SkipFrameInterval:" << 1 << std::endl;
    }

    auto res = ScreenManager::GetInstance().DestroyVirtualScreen(virtualScreenId);
    ASSERT_EQ(DMError::DM_OK, res);
}
}
} // namespace Rosen
} // namespace OHOS
