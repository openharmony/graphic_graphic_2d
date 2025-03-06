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

#include <gtest/gtest.h>

#include "event_handler.h"
#include "vsync_receiver.h"
#include "util.h"
#include "boot_animation_operation.h"
#include "boot_picture_player.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Rosen {
class BootPicturePlayerTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void BootPicturePlayerTest::SetUpTestCase() {}
void BootPicturePlayerTest::TearDownTestCase() {}
void BootPicturePlayerTest::SetUp() {}
void BootPicturePlayerTest::TearDown() {}

/**
 * @tc.name: BootPicturePlayerTest_001
 * @tc.desc: Verify the ReadPicZipFile
 * @tc.type:FUNC
 */
HWTEST_F(BootPicturePlayerTest, BootPicturePlayerTest_001, TestSize.Level1)
{
    PlayerParams params;
    std::shared_ptr<BootPicturePlayer> player = std::make_shared<BootPicturePlayer>(params);
    ImageStructVec imgVec;
    int32_t freq = 30;
    EXPECT_EQ(player->ReadPicZipFile(imgVec, freq), true);
}

/**
 * @tc.name: BootPicturePlayerTest_002
 * @tc.desc: Verify the CheckFrameRateValid
 * @tc.type:FUNC
 */
HWTEST_F(BootPicturePlayerTest, BootPicturePlayerTest_002, TestSize.Level1)
{
    PlayerParams params;
    std::shared_ptr<BootPicturePlayer> player = std::make_shared<BootPicturePlayer>(params);
    int32_t frameRate = 0;
    EXPECT_EQ(player->CheckFrameRateValid(frameRate), false);
}

/**
 * @tc.name: BootPicturePlayerTest_003
 * @tc.desc: Verify the CheckFrameRateValid
 * @tc.type:FUNC
 */
HWTEST_F(BootPicturePlayerTest, BootPicturePlayerTest_003, TestSize.Level1)
{
    PlayerParams params;
    std::shared_ptr<BootPicturePlayer> player = std::make_shared<BootPicturePlayer>(params);
    int32_t frameRate = 30;
    EXPECT_EQ(player->CheckFrameRateValid(frameRate), true);
}

/**
 * @tc.name: BootPicturePlayerTest_004
 * @tc.desc: Verify the Play
 * @tc.type:FUNC
 */
HWTEST_F(BootPicturePlayerTest, BootPicturePlayerTest_004, TestSize.Level1)
{
    PlayerParams params;
    std::shared_ptr<BootPicturePlayer> player = std::make_shared<BootPicturePlayer>(params);
    std::shared_ptr<OHOS::AppExecFwk::EventRunner> runner = AppExecFwk::EventRunner::Create(false);
    std::shared_ptr<OHOS::AppExecFwk::EventHandler> handler = std::make_shared<AppExecFwk::EventHandler>(runner);
    int flag = 0;
    handler->PostTask([&] {
        player->Play();
        flag = 1;
        runner->Stop();
    });
    runner->Run();
    EXPECT_EQ(flag, 1);
    
    player->receiver_ = nullptr;
    auto& rsClient = OHOS::Rosen::RSInterfaces::GetInstance();
    player->receiver_ = rsClient.CreateVSyncReceiver("BootAnimation", handler);
    handler->PostTask([&] {
        player->Play();
        flag = 0;
        runner->Stop();
    });
    runner->Run();
    EXPECT_EQ(flag, 0);

    ImageStructVec imgVec;
    player->imageVector_ = imgVec;
    handler->PostTask([&] {
        player->Play();
        flag = 1;
        runner->Stop();
    });
    runner->Run();
    EXPECT_EQ(flag, 1);
}

/**
 * @tc.name: BootPicturePlayerTest_005
 * @tc.desc: Verify the GetPicZipPath
 * @tc.type:FUNC
 */
HWTEST_F(BootPicturePlayerTest, BootPicturePlayerTest_005, TestSize.Level1)
{
    PlayerParams params;
    std::shared_ptr<BootPicturePlayer> player = std::make_shared<BootPicturePlayer>(params);
    params.resPath = "/system/etc/graphic/bootpic.zip";
    EXPECT_EQ(player->GetPicZipPath(), "/system/etc/graphic/bootpic.zip");
}

/**
 * @tc.name: BootPicturePlayerTest_006
 * @tc.desc: Verify the Draw
 * @tc.type:FUNC
 */
HWTEST_F(BootPicturePlayerTest, BootPicturePlayerTest_006, TestSize.Level1)
{
    BootAnimationOperation operation;
    std::shared_ptr<OHOS::AppExecFwk::EventRunner> runner = AppExecFwk::EventRunner::Create(false);
    std::shared_ptr<OHOS::AppExecFwk::EventHandler> handler = std::make_shared<AppExecFwk::EventHandler>(runner);
    int32_t degree = 0;
    operation.InitRsDisplayNode();
    operation.InitRsSurfaceNode(degree);
    operation.InitRsSurface();
    ASSERT_NE(nullptr, operation.rsSurface_);
    PlayerParams params;
    std::shared_ptr<BootPicturePlayer> player = std::make_shared<BootPicturePlayer>(params);
    player->rsSurface_ = operation.rsSurface_;
    player->picCurNo_ = 2;
    player->imgVecSize_ = 0;
    handler->PostTask([&] {
        player->Draw();
        runner->Stop();
    });
    runner->Run();

    ImageStructVec imgVec;
    player->imageVector_ = imgVec;
    player->picCurNo_ = -2;
    player->imgVecSize_ = 0;
    handler->PostTask([&] {
        player->Draw();
        runner->Stop();
    });
    runner->Run();
}

/**
 * @tc.name: BootPicturePlayerTest_007
 * @tc.desc: Verify the OnDraw
 * @tc.type:FUNC
 */
HWTEST_F(BootPicturePlayerTest, BootPicturePlayerTest_007, TestSize.Level1)
{
    std::shared_ptr<OHOS::AppExecFwk::EventRunner> runner = AppExecFwk::EventRunner::Create(false);
    std::shared_ptr<OHOS::AppExecFwk::EventHandler> handler = std::make_shared<AppExecFwk::EventHandler>(runner);
    PlayerParams params;
    bool flag = false;
    std::shared_ptr<BootPicturePlayer> player = std::make_shared<BootPicturePlayer>(params);
    handler->PostTask([&] {
        flag = player->OnDraw(nullptr, 0);
        runner->Stop();
    });
    runner->Run();
    EXPECT_EQ(flag, false);

    Rosen::Drawing::CoreCanvas canvas;
    handler->PostTask([&] {
        flag = player->OnDraw(&canvas, 0);
        runner->Stop();
    });
    runner->Run();
    EXPECT_EQ(flag, false);

    player->imgVecSize_ = 1;
    handler->PostTask([&] {
        flag = player->OnDraw(&canvas, -1);
        runner->Stop();
    });
    runner->Run();
    EXPECT_EQ(flag, false);

    player->imgVecSize_ = -1;
    handler->PostTask([&] {
        flag = player->OnDraw(&canvas, -1);
        runner->Stop();
    });
    runner->Run();
    EXPECT_EQ(flag, false);

    ImageStructVec imgVec;
    player->imageVector_ = imgVec;
    player->picCurNo_ = 0;
    player->imgVecSize_ = 2;
    player->OnVsync();
    flag = player->Stop();
    EXPECT_EQ(flag, false);
}
}
