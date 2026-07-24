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
#include "zip.h"
#include "boot_animation_operation.h"
#include "boot_picture_player.h"
#include "boot_animation_strategy.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Rosen {
namespace {
    constexpr const char* TEST_PIC_ZIP_PATH = "/data/local/tmp/ba_pic_player_test.zip";
    constexpr const char* DEFAULT_BOOT_PIC_ZIP = "/system/etc/graphic/bootpic.zip";
}
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
 * @tc.name: ReadPicZipFile_EmptyParams_PropagateReadZipResult
 * @tc.desc: Verify ReadPicZipFile propagates ReadZipFile's return value instead of
 *           always returning true. When /system/etc/graphic/bootpic.zip exists and is
 *           readable, returns true; when missing, returns false.
 * @tc.type: FUNC
 */
HWTEST_F(BootPicturePlayerTest, ReadPicZipFile_EmptyParams_PropagateReadZipResult, TestSize.Level1)
{
    PlayerParams params;
    std::shared_ptr<BootPicturePlayer> player = std::make_shared<BootPicturePlayer>(params);
    ASSERT_NE(player, nullptr);
    ImageStructVec imgVec;
    int32_t freq = 30;
    bool defaultZipExists = OHOS::IsFileExisted(DEFAULT_BOOT_PIC_ZIP);
    EXPECT_EQ(player->ReadPicZipFile(imgVec, freq), defaultZipExists);
}

/**
 * @tc.name: ReadPicZipFile_ValidZip_ReturnTrue
 * @tc.desc: Verify ReadPicZipFile returns true and reads images with a valid temporary
 *           zip file. This end-to-end covers both the ReadImageFile do-while break fix
 *           (Bug 1: readLen==0 at EOF no longer triggers memcpy_s with destMax=0) and
 *           the ReadPicZipFile return-value check fix (Bug 2: failure is propagated).
 * @tc.type: FUNC
 */
HWTEST_F(BootPicturePlayerTest, ReadPicZipFile_ValidZip_ReturnTrue, TestSize.Level1)
{
    zipFile zf = zipOpen(TEST_PIC_ZIP_PATH, APPEND_STATUS_CREATE);
    ASSERT_NE(zf, nullptr);
    zip_fileinfo info = {};
    ASSERT_EQ(zipOpenNewFileInZip(zf, "test_image.png", &info, nullptr, 0, nullptr, 0, nullptr,
        Z_DEFLATED, Z_DEFAULT_COMPRESSION), ZIP_OK);
    std::string content = "fake image content";
    ASSERT_EQ(zipWriteInFileInZip(zf, content.data(), content.size()), ZIP_OK);
    zipCloseFileInZip(zf);
    zipClose(zf, nullptr);

    PlayerParams params;
    params.resPath = TEST_PIC_ZIP_PATH;
    std::shared_ptr<BootPicturePlayer> player = std::make_shared<BootPicturePlayer>(params);
    ASSERT_NE(player, nullptr);
    ImageStructVec imgVec;
    int32_t freq = 30;
    EXPECT_EQ(player->ReadPicZipFile(imgVec, freq), true);
    EXPECT_GT(imgVec.size(), static_cast<size_t>(0));
    remove(TEST_PIC_ZIP_PATH);
}

/**
 * @tc.name: CheckFrameRateValid_ZeroFrameRate_ReturnFalse
 * @tc.desc: Verify the CheckFrameRateValid function returns false with zero frame rate.
 * @tc.type: FUNC
 */
HWTEST_F(BootPicturePlayerTest, CheckFrameRateValid_ZeroFrameRate_ReturnFalse, TestSize.Level1)
{
    PlayerParams params;
    std::shared_ptr<BootPicturePlayer> player = std::make_shared<BootPicturePlayer>(params);
    ASSERT_NE(player, nullptr);
    int32_t frameRate = 0;
    EXPECT_EQ(player->CheckFrameRateValid(frameRate), false);
}

/**
 * @tc.name: CheckFrameRateValid_ValidFrameRate_ReturnTrue
 * @tc.desc: Verify the CheckFrameRateValid function returns true with valid frame rate.
 * @tc.type: FUNC
 */
HWTEST_F(BootPicturePlayerTest, CheckFrameRateValid_ValidFrameRate_ReturnTrue, TestSize.Level1)
{
    PlayerParams params;
    std::shared_ptr<BootPicturePlayer> player = std::make_shared<BootPicturePlayer>(params);
    ASSERT_NE(player, nullptr);
    int32_t frameRate = 30;
    EXPECT_EQ(player->CheckFrameRateValid(frameRate), true);
}

/**
 * @tc.name: Play_DifferentStates_ExecuteSuccessfully
 * @tc.desc: Verify the Play function executes with different states.
 * @tc.type: FUNC
 */
HWTEST_F(BootPicturePlayerTest, Play_DifferentStates_ExecuteSuccessfully, TestSize.Level1)
{
    PlayerParams params;
    std::shared_ptr<BootPicturePlayer> player = std::make_shared<BootPicturePlayer>(params);
    ASSERT_NE(player, nullptr);
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
 * @tc.name: GetPicZipPath_SetPath_ReturnCorrectPath
 * @tc.desc: Verify the GetPicZipPath function returns correct path.
 * @tc.type: FUNC
 */
HWTEST_F(BootPicturePlayerTest, GetPicZipPath_SetPath_ReturnCorrectPath, TestSize.Level1)
{
    PlayerParams params;
    std::shared_ptr<BootPicturePlayer> player = std::make_shared<BootPicturePlayer>(params);
    ASSERT_NE(player, nullptr);
    params.resPath = "/system/etc/graphic/bootpic.zip";
    EXPECT_EQ(player->GetPicZipPath(), "/system/etc/graphic/bootpic.zip");
}

/**
 * @tc.name: OnDraw_DifferentConditions_ExecuteSuccessfully
 * @tc.desc: Verify the OnDraw function executes with different conditions.
 * @tc.type: FUNC
 */
HWTEST_F(BootPicturePlayerTest, OnDraw_DifferentConditions_ExecuteSuccessfully, TestSize.Level1)
{
    std::shared_ptr<OHOS::AppExecFwk::EventRunner> runner = AppExecFwk::EventRunner::Create(false);
    std::shared_ptr<OHOS::AppExecFwk::EventHandler> handler = std::make_shared<AppExecFwk::EventHandler>(runner);
    PlayerParams params;
    bool flag = false;
    std::shared_ptr<BootPicturePlayer> player = std::make_shared<BootPicturePlayer>(params);
    ASSERT_NE(player, nullptr);
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
 * @tc.name: Draw_Normal_ExecuteSuccessfully
 * @tc.desc: Verify the Draw function executes successfully.
 * @tc.type: FUNC
 */
HWTEST_F(BootPicturePlayerTest, Draw_Normal_ExecuteSuccessfully, TestSize.Level1)
{
    BootAnimationOperation operation;
    std::shared_ptr<OHOS::AppExecFwk::EventRunner> runner = AppExecFwk::EventRunner::Create(false);
    std::shared_ptr<OHOS::AppExecFwk::EventHandler> handler = std::make_shared<AppExecFwk::EventHandler>(runner);
    std::shared_ptr<BootAnimationStrategy> strategy = std::make_shared<BootAnimationStrategy>();
    strategy->GetConnectToRenderMap(1);
    operation.connectToRender_ = strategy->connectToRenderMap_.begin()->second;
    int32_t degree = 0;
    operation.InitRsDisplayNode();
    operation.InitRsSurfaceNode(degree);
    operation.InitRsSurface();
    ASSERT_NE(nullptr, operation.rsSurface_);
    PlayerParams params;
    std::shared_ptr<BootPicturePlayer> player = std::make_shared<BootPicturePlayer>(params);
    ASSERT_NE(player, nullptr);
    player->rsSurface_ = operation.rsSurface_;
    player->picCurNo_ = 2;
    player->imgVecSize_ = 0;
    handler->PostTask([&] {
        player->Draw();
        runner->Stop();
    });
    runner->Run();
    EXPECT_TRUE(true);

    ImageStructVec imgVec;
    player->imageVector_ = imgVec;
    player->picCurNo_ = -2;
    player->imgVecSize_ = 0;
    handler->PostTask([&] {
        player->Draw();
        runner->Stop();
    });
    runner->Run();
    EXPECT_TRUE(true);
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
