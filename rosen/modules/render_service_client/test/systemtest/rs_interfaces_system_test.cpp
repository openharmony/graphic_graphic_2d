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
class RSInterfacesSystemTest : public testing::Test {
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

sptr<Display> RSInterfacesSystemTest::defaultDisplay_ = nullptr;
DisplayId RSInterfacesSystemTest::defaultDisplayId_ = DISPLAY_ID_INVALID;
ScreenId RSInterfacesSystemTest::defaultScreenId_ = INVALID_SCREEN_ID;
std::string RSInterfacesSystemTest::defaultName_ = "virtualScreen01";
uint32_t RSInterfacesSystemTest::defaultWidth_ = 480;
uint32_t RSInterfacesSystemTest::defaultHeight_ = 320;
float RSInterfacesSystemTest::defaultDensity_ = 2.0;
int32_t RSInterfacesSystemTest::defaultFlags_ = 0;
VirtualScreenOption RSInterfacesSystemTest::defaultOption_ = {
    defaultName_, defaultWidth_, defaultHeight_, defaultDensity_, nullptr, defaultFlags_
};
const std::string defaultCmd_ = "/system/bin/snapshot_display -i ";

void RSInterfacesSystemTest::SetUpTestCase()
{
    defaultDisplay_ = DisplayManager::GetInstance().GetDefaultDisplay();
    defaultDisplayId_ = defaultDisplay_->GetId();
    defaultScreenId_ = defaultDisplay_->GetScreenId();
    defaultWidth_ = defaultDisplay_->GetWidth();
    defaultHeight_ = defaultDisplay_->GetHeight();
    defaultOption_.width_ = defaultWidth_;
    defaultOption_.height_ = defaultHeight_;
}

void RSInterfacesSystemTest::TearDownTestCase()
{
}

void RSInterfacesSystemTest::SetUp()
{
}

void RSInterfacesSystemTest::TearDown()
{
}

namespace {
/**
 * @tc.name: ScreenManager03
 * @tc.desc: Mirror virtualScreen
 * @tc.type: FUNC
 */
HWTEST_F(RSInterfacesSystemTest, ScreenManager03, Function | MediumTest | Level2)
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
    ScreenId rsId = ids.front();

    int32_t ret = RSInterfaces::GetInstance().SetScreenSkipFrameInterval(rsId, virtualScreenSkipFrameInterval);
    ASSERT_EQ(ret, StatusCode::SUCCESS);
    std::cout << "VirtualScreen is created now" << std::endl;
    sleep(30 * TEST_SLEEP_S);

    auto res = ScreenManager::GetInstance().DestroyVirtualScreen(virtualScreenId);
    ASSERT_EQ(DMError::DM_OK, res);
}

/**
 * @tc.name: SetFocusAppInfo
 * @tc.desc: Set focus AppInfo
 * @tc.type: FUNC
 */
HWTEST_F(RSInterfacesSystemTest, SetFocusAppInfo, Function | MediumTest | Level2)
{
    FocusAppInfo info;

    int32_t ret = RSInterfaces::GetInstance().SetFocusAppInfo(info);
    ASSERT_EQ(ret, StatusCode::SUCCESS);
}

/**
 * @tc.name: ShowWatermark
 * @tc.desc: Test ShowWatermark interface.
 * @tc.type: FUNC
 * @tc.require: issueI6HP8P
 */
HWTEST_F(RSInterfacesSystemTest, ShowWatermark, Function | MediumTest | Level2)
{
    const uint32_t color[8] = { 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80 };
    uint32_t colorLength = sizeof(color) / sizeof(color[0]);
    const int32_t offset = 0;
    Media::InitializationOptions opts;
    opts.size.width = 3;
    opts.size.height = 2;
    opts.pixelFormat = Media::PixelFormat::RGBA_8888;
    opts.alphaType = Media::AlphaType::IMAGE_ALPHA_TYPE_OPAQUE;
    int32_t stride = opts.size.width;
    std::unique_ptr<Media::PixelMap> pixelMap1 = Media::PixelMap::Create(color, colorLength, offset, stride, opts);
    RSInterfaces::GetInstance().ShowWatermark(std::move(pixelMap1), true);
    sleep(TEST_SLEEP_S);
    (void)system("snapshot_display -i 0");
    sleep(3 * TEST_SLEEP_S);
    RSInterfaces::GetInstance().ShowWatermark(nullptr, false);
    sleep(TEST_SLEEP_S);
    (void)system("snapshot_display -i 0");
    sleep(3 * TEST_SLEEP_S);
}

/**
 * @tc.name: SetCurtainScreenUsingStatus001
 * @tc.desc: Test CurtainScreen on.
 * @tc.type: FUNC
 * @tc.require: issueI993OY
 */
HWTEST_F(RSInterfacesSystemTest, SetCurtainScreenUsingStatus001, Function | MediumTest | Level2)
{
    RSInterfaces::GetInstance().SetCurtainScreenUsingStatus(true);
}

/**
 * @tc.name: SetCurtainScreenUsingStatus002
 * @tc.desc: Test CurtainScreen off.
 * @tc.type: FUNC
 * @tc.require: issueI993OY
 */
HWTEST_F(RSInterfacesSystemTest, SetCurtainScreenUsingStatus002, Function | MediumTest | Level2)
{
    RSInterfaces::GetInstance().SetCurtainScreenUsingStatus(false);
}

/**
 * @tc.name: SetVirtualScreenUsingStatus001
 * @tc.desc: Virtual screen use status.
 * @tc.type: FUNC
 * @tc.require: issueI9ABGS
 */
HWTEST_F(RSInterfacesSystemTest, SetVirtualScreenUsingStatus001, Function | MediumTest | Level2)
{
    RSInterfaces::GetInstance().SetVirtualScreenUsingStatus(true);
}

/**
 * @tc.name: SetVirtualScreenUsingStatus002
 * @tc.desc: Virtual screen don't use status.
 * @tc.type: FUNC
 * @tc.require: issueI9ABGS
 */
HWTEST_F(RSInterfacesSystemTest, SetVirtualScreenUsingStatus002, Function | MediumTest | Level2)
{
    RSInterfaces::GetInstance().SetVirtualScreenUsingStatus(false);
}
}
} // namespace Rosen
} // namespace OHOS
