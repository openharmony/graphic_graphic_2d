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

#include "gtest/gtest.h"

#include "common/rs_vector4.h"
#include "rs_window_animation_stub.h"
#include "rs_window_animation_finished_callback.h"
#include "rs_window_animation_target.h"
#include "ui/rs_surface_node.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
class RSWindowAnimationStubMock : public RSWindowAnimationStub {
public:
    RSWindowAnimationStubMock() = default;
    virtual ~RSWindowAnimationStubMock() = default;

    void OnStartApp(StartingAppType type, const sptr<RSWindowAnimationTarget>& startingWindowTarget,
        const sptr<RSIWindowAnimationFinishedCallback>& finishedCallback) override {};

    void OnAppTransition(const sptr<RSWindowAnimationTarget>& fromWindowTarget,
        const sptr<RSWindowAnimationTarget>& toWindowTarget,
        const sptr<RSIWindowAnimationFinishedCallback>& finishedCallback) override {};

    void OnAppBackTransition(const sptr<RSWindowAnimationTarget>& fromWindowTarget,
        const sptr<RSWindowAnimationTarget>& toWindowTarget,
        const sptr<RSIWindowAnimationFinishedCallback>& finishedCallback) override {};

    void OnMinimizeWindow(const sptr<RSWindowAnimationTarget>& minimizingWindowTarget,
        const sptr<RSIWindowAnimationFinishedCallback>& finishedCallback) override {};

    void OnMinimizeAllWindow(std::vector<sptr<RSWindowAnimationTarget>> minimizingWindowsTarget,
        const sptr<RSIWindowAnimationFinishedCallback>& finishedCallback) override {};

    void OnCloseWindow(const sptr<RSWindowAnimationTarget>& closingWindowTarget,
        const sptr<RSIWindowAnimationFinishedCallback>& finishedCallback) override {};

    void OnScreenUnlock(const sptr<RSIWindowAnimationFinishedCallback>& finishedCallback) override {};

    void OnWindowAnimationTargetsUpdate(const sptr<RSWindowAnimationTarget>& fullScreenWindowTarget,
        const std::vector<sptr<RSWindowAnimationTarget>>& floatingWindowTargets) override {};

    void OnWallpaperUpdate(const sptr<RSWindowAnimationTarget>& wallpaperTarget) override {};
};

class RSWindowAnimationStubTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;

    static sptr<RSWindowAnimationFinishedCallback> finishCallBack_;
    static std::shared_ptr<RSSurfaceNode> animationSurfaceNode;
    static sptr<RSWindowAnimationStubMock> windowAnimationStub_;
    static std::shared_ptr<RSWindowAnimationTarget> windowAnimationTarget_;
};
sptr<RSWindowAnimationFinishedCallback> RSWindowAnimationStubTest::finishCallBack_ = nullptr;
std::shared_ptr<RSSurfaceNode> RSWindowAnimationStubTest::animationSurfaceNode = nullptr;
sptr<RSWindowAnimationStubMock> RSWindowAnimationStubTest::windowAnimationStub_ = nullptr;
std::shared_ptr<RSWindowAnimationTarget> RSWindowAnimationStubTest::windowAnimationTarget_ =
    nullptr;

void RSWindowAnimationStubTest::SetUpTestCase()
{
    finishCallBack_ = new RSWindowAnimationFinishedCallback(nullptr);
    RSSurfaceNodeConfig config;
    animationSurfaceNode = RSSurfaceNode::Create(config, true);
    windowAnimationStub_ = new RSWindowAnimationStubMock();
    windowAnimationTarget_ = std::make_shared<RSWindowAnimationTarget>();
    windowAnimationTarget_->bundleName_ = "";
    windowAnimationTarget_->abilityName_ = "";
    windowAnimationTarget_->windowBounds_ = RRect();
    windowAnimationTarget_->surfaceNode_ = animationSurfaceNode;
    windowAnimationTarget_->windowId_ = 0;
    windowAnimationTarget_->displayId_ = 0;
    windowAnimationTarget_->missionId_ = 0;
}
void RSWindowAnimationStubTest::TearDownTestCase()
{
    windowAnimationStub_ = nullptr;
    finishCallBack_ = nullptr;
}
void RSWindowAnimationStubTest::SetUp() {}
void RSWindowAnimationStubTest::TearDown() {}

/**
 * @tc.name: OnRemoteRequest001
 * @tc.desc: Verify the OnRemoteRequest
 * @tc.type:FUNC
 */
HWTEST_F(RSWindowAnimationStubTest, OnRemoteRequest001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSWindowAnimationStubTest OnRemoteRequest001 start";
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;

    int res = windowAnimationStub_->OnRemoteRequest(
        RSIWindowAnimationController::ON_START_APP, data, reply, option);
    ASSERT_EQ(res, ERR_INVALID_STATE);
    GTEST_LOG_(INFO) << "RSWindowAnimationStubTest OnRemoteRequest001 end";
}

/**
 * @tc.name: OnRemoteRequest002
 * @tc.desc: Verify the OnRemoteRequest
 * @tc.type:FUNC
 */
HWTEST_F(RSWindowAnimationStubTest, OnRemoteRequest002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSWindowAnimationStubTest OnRemoteRequest002 start";
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    data.WriteInterfaceToken(RSIWindowAnimationController::GetDescriptor());
    int res = windowAnimationStub_->OnRemoteRequest(-1, data, reply, option);
    ASSERT_EQ(res, ERR_UNKNOWN_TRANSACTION);
    GTEST_LOG_(INFO) << "RSWindowAnimationStubTest OnRemoteRequest002 end";
}

/**
 * @tc.name: StartApp001
 * @tc.desc: Verify the StartApp
 * @tc.type:FUNC
 */
HWTEST_F(RSWindowAnimationStubTest, StartApp001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSWindowAnimationStubTest StartApp001 start";
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    data.WriteInterfaceToken(RSIWindowAnimationController::GetDescriptor());
    data.WriteInt32(StartingAppType::FROM_LAUNCHER);
    int res = windowAnimationStub_->OnRemoteRequest(
        RSIWindowAnimationController::ON_START_APP, data, reply, option);
    ASSERT_EQ(res, ERR_INVALID_DATA);
    GTEST_LOG_(INFO) << "RSWindowAnimationStubTest StartApp001 end";
}

/**
 * @tc.name: StartApp002
 * @tc.desc: Verify the StartApp
 * @tc.type:FUNC
 */
HWTEST_F(RSWindowAnimationStubTest, StartApp002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSWindowAnimationStubTest StartApp002 start";
    MessageParcel data1;
    MessageParcel reply;
    MessageOption option;
    data1.WriteInterfaceToken(RSIWindowAnimationController::GetDescriptor());
    data1.WriteInt32(StartingAppType::FROM_LAUNCHER);
    data1.WriteParcelable(windowAnimationTarget_.get());
    int res = windowAnimationStub_->OnRemoteRequest(
        RSIWindowAnimationController::ON_START_APP, data1, reply, option);
    ASSERT_EQ(res, ERR_INVALID_DATA);
    MessageParcel data2;
    data2.WriteInterfaceToken(RSIWindowAnimationController::GetDescriptor());
    data2.WriteInt32(StartingAppType::FROM_LAUNCHER);
    data2.WriteParcelable(windowAnimationTarget_.get());
    data2.WriteRemoteObject(finishCallBack_->AsObject());
    res = windowAnimationStub_->OnRemoteRequest(
        RSIWindowAnimationController::ON_START_APP, data2, reply, option);
    ASSERT_EQ(res, ERR_NONE);
    GTEST_LOG_(INFO) << "RSWindowAnimationStubTest StartApp002 end";
}

/**
 * @tc.name: AppTransition001
 * @tc.desc: Verify the AppTransition
 * @tc.type:FUNC
 */
HWTEST_F(RSWindowAnimationStubTest, AppTransition001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSWindowAnimationStubTest AppTransition001 start";
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    data.WriteInterfaceToken(RSIWindowAnimationController::GetDescriptor());
    int res = windowAnimationStub_->OnRemoteRequest(
        RSIWindowAnimationController::ON_APP_TRANSITION, data, reply, option);
    ASSERT_EQ(res, ERR_INVALID_DATA);
    GTEST_LOG_(INFO) << "RSWindowAnimationStubTest AppTransition001 end";
}

/**
 * @tc.name: AppTransition002
 * @tc.desc: Verify the AppTransition
 * @tc.type:FUNC
 */
HWTEST_F(RSWindowAnimationStubTest, AppTransition002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSWindowAnimationStubTest AppTransition001 start";
    MessageParcel data1;
    MessageParcel reply;
    MessageOption option;
    data1.WriteInterfaceToken(RSIWindowAnimationController::GetDescriptor());
    data1.WriteParcelable(windowAnimationTarget_.get());
    int res = windowAnimationStub_->OnRemoteRequest(
        RSIWindowAnimationController::ON_APP_TRANSITION, data1, reply, option);
    ASSERT_EQ(res, ERR_INVALID_DATA);
    MessageParcel data2;
    data2.WriteInterfaceToken(RSIWindowAnimationController::GetDescriptor());
    data2.WriteParcelable(windowAnimationTarget_.get());
    data2.WriteParcelable(windowAnimationTarget_.get());
    res = windowAnimationStub_->OnRemoteRequest(
        RSIWindowAnimationController::ON_APP_TRANSITION, data2, reply, option);
    ASSERT_EQ(res, ERR_INVALID_DATA);
    MessageParcel data3;
    data3.WriteInterfaceToken(RSIWindowAnimationController::GetDescriptor());
    data3.WriteParcelable(windowAnimationTarget_.get());
    data3.WriteParcelable(windowAnimationTarget_.get());
    data3.WriteRemoteObject(finishCallBack_->AsObject());
    res = windowAnimationStub_->OnRemoteRequest(
        RSIWindowAnimationController::ON_APP_TRANSITION, data3, reply, option);
    ASSERT_EQ(res, ERR_NONE);
    GTEST_LOG_(INFO) << "RSWindowAnimationStubTest AppTransition002 end";
}

/**
 * @tc.name: AppBackTransition001
 * @tc.desc: Verify the AppBackTransition
 * @tc.type:FUNC
 */
HWTEST_F(RSWindowAnimationStubTest, AppBackTransition001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSWindowAnimationStubTest AppBackTransition001 start";
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    data.WriteInterfaceToken(RSIWindowAnimationController::GetDescriptor());
    int res = windowAnimationStub_->OnRemoteRequest(
        RSIWindowAnimationController::ON_APP_BACK_TRANSITION, data, reply, option);
    ASSERT_EQ(res, ERR_INVALID_DATA);
    GTEST_LOG_(INFO) << "RSWindowAnimationStubTest AppBackTransition001 end";
}

/**
 * @tc.name: AppBackTransition002
 * @tc.desc: Verify the AppBackTransition
 * @tc.type:FUNC
 */
HWTEST_F(RSWindowAnimationStubTest, AppBackTransition002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSWindowAnimationStubTest AppBackTransition002 start";
    MessageParcel data1;
    MessageParcel reply;
    MessageOption option;
    data1.WriteInterfaceToken(RSIWindowAnimationController::GetDescriptor());
    data1.WriteParcelable(windowAnimationTarget_.get());
    int res = windowAnimationStub_->OnRemoteRequest(
        RSIWindowAnimationController::ON_APP_BACK_TRANSITION, data1, reply, option);
    ASSERT_EQ(res, ERR_INVALID_DATA);
    MessageParcel data2;
    data2.WriteInterfaceToken(RSIWindowAnimationController::GetDescriptor());
    data2.WriteParcelable(windowAnimationTarget_.get());
    data2.WriteParcelable(windowAnimationTarget_.get());
    res = windowAnimationStub_->OnRemoteRequest(
        RSIWindowAnimationController::ON_APP_BACK_TRANSITION, data2, reply, option);
    ASSERT_EQ(res, ERR_INVALID_DATA);
    MessageParcel data3;
    data3.WriteInterfaceToken(RSIWindowAnimationController::GetDescriptor());
    data3.WriteParcelable(windowAnimationTarget_.get());
    data3.WriteParcelable(windowAnimationTarget_.get());
    data3.WriteRemoteObject(finishCallBack_->AsObject());
    res = windowAnimationStub_->OnRemoteRequest(
        RSIWindowAnimationController::ON_APP_BACK_TRANSITION, data3, reply, option);
    ASSERT_EQ(res, ERR_NONE);
    GTEST_LOG_(INFO) << "RSWindowAnimationStubTest AppBackTransition002 end";
}

/**
 * @tc.name: MinimizeWindow001
 * @tc.desc: Verify the MinimizeWindow
 * @tc.type:FUNC
 */
HWTEST_F(RSWindowAnimationStubTest, MinimizeWindow001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSWindowAnimationStubTest MinimizeWindow001 start";
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    data.WriteInterfaceToken(RSIWindowAnimationController::GetDescriptor());
    data.WriteInt32(StartingAppType::FROM_LAUNCHER);
    int res = windowAnimationStub_->OnRemoteRequest(
        RSIWindowAnimationController::ON_MINIMIZE_WINDOW, data, reply, option);
    ASSERT_EQ(res, ERR_INVALID_DATA);
    GTEST_LOG_(INFO) << "RSWindowAnimationStubTest MinimizeWindow001 end";
}

/**
 * @tc.name: MinimizeWindow002
 * @tc.desc: Verify the MinimizeWindow
 * @tc.type:FUNC
 */
HWTEST_F(RSWindowAnimationStubTest, MinimizeWindow002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSWindowAnimationStubTest MinimizeWindow002 start";
    MessageParcel data1;
    MessageParcel reply;
    MessageOption option;
    data1.WriteInterfaceToken(RSIWindowAnimationController::GetDescriptor());
    data1.WriteParcelable(windowAnimationTarget_.get());
    int res = windowAnimationStub_->OnRemoteRequest(
        RSIWindowAnimationController::ON_MINIMIZE_WINDOW, data1, reply, option);
    ASSERT_EQ(res, ERR_INVALID_DATA);
    MessageParcel data2;
    data2.WriteInterfaceToken(RSIWindowAnimationController::GetDescriptor());
    data2.WriteParcelable(windowAnimationTarget_.get());
    data2.WriteRemoteObject(finishCallBack_->AsObject());
    res = windowAnimationStub_->OnRemoteRequest(
        RSIWindowAnimationController::ON_MINIMIZE_WINDOW, data2, reply, option);
    ASSERT_EQ(res, ERR_NONE);
    GTEST_LOG_(INFO) << "RSWindowAnimationStubTest MinimizeWindow002 end";
}

/**
 * @tc.name: MinimizeAllWindow001
 * @tc.desc: Verify the MinimizeAllWindow
 * @tc.type:FUNC
 */
HWTEST_F(RSWindowAnimationStubTest, MinimizeAllWindow001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSWindowAnimationStubTest MinimizeAllWindow001 start";
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    data.WriteInterfaceToken(RSIWindowAnimationController::GetDescriptor());
    data.WriteInt32(StartingAppType::FROM_LAUNCHER);
    data.WriteUint32(1);
    int res = windowAnimationStub_->OnRemoteRequest(
        RSIWindowAnimationController::ON_MINIMIZE_ALLWINDOW, data, reply, option);
    ASSERT_EQ(res, ERR_INVALID_DATA);
    GTEST_LOG_(INFO) << "RSWindowAnimationStubTest MinimizeAllWindow001 end";
}

/**
 * @tc.name: MinimizeAllWindow002
 * @tc.desc: Verify the MinimizeAllWindow
 * @tc.type:FUNC
 */
HWTEST_F(RSWindowAnimationStubTest, MinimizeAllWindow002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSWindowAnimationStubTest MinimizeAllWindow002 start";
    MessageParcel data1;
    MessageParcel reply;
    MessageOption option;
    data1.WriteInterfaceToken(RSIWindowAnimationController::GetDescriptor());
    data1.WriteUint32(1);
    data1.WriteParcelable(windowAnimationTarget_.get());
    int res = windowAnimationStub_->OnRemoteRequest(
        RSIWindowAnimationController::ON_MINIMIZE_ALLWINDOW, data1, reply, option);
    ASSERT_EQ(res, ERR_INVALID_DATA);
    MessageParcel data2;
    data2.WriteInterfaceToken(RSIWindowAnimationController::GetDescriptor());
    data2.WriteUint32(1);
    data2.WriteParcelable(windowAnimationTarget_.get());
    data2.WriteRemoteObject(finishCallBack_->AsObject());
    res = windowAnimationStub_->OnRemoteRequest(
        RSIWindowAnimationController::ON_MINIMIZE_ALLWINDOW, data2, reply, option);
    ASSERT_EQ(res, ERR_NONE);
    GTEST_LOG_(INFO) << "RSWindowAnimationStubTest MinimizeAllWindow002 end";
}

/**
 * @tc.name: CloseWindow001
 * @tc.desc: Verify the CloseWindow
 * @tc.type:FUNC
 */
HWTEST_F(RSWindowAnimationStubTest, CloseWindow001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSWindowAnimationStubTest CloseWindow001 start";
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    data.WriteInterfaceToken(RSIWindowAnimationController::GetDescriptor());
    int res = windowAnimationStub_->OnRemoteRequest(
        RSIWindowAnimationController::ON_CLOSE_WINDOW, data, reply, option);
    ASSERT_EQ(res, ERR_INVALID_DATA);
    GTEST_LOG_(INFO) << "RSWindowAnimationStubTest CloseWindow001 end";
}

/**
 * @tc.name: CloseWindow002
 * @tc.desc: Verify the CloseWindow
 * @tc.type:FUNC
 */
HWTEST_F(RSWindowAnimationStubTest, CloseWindow002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSWindowAnimationStubTest CloseWindow002 start";
    MessageParcel data1;
    MessageParcel reply;
    MessageOption option;
    data1.WriteInterfaceToken(RSIWindowAnimationController::GetDescriptor());
    data1.WriteParcelable(windowAnimationTarget_.get());
    int res = windowAnimationStub_->OnRemoteRequest(
        RSIWindowAnimationController::ON_CLOSE_WINDOW, data1, reply, option);
    ASSERT_EQ(res, ERR_INVALID_DATA);
    MessageParcel data2;
    data2.WriteInterfaceToken(RSIWindowAnimationController::GetDescriptor());
    data2.WriteParcelable(windowAnimationTarget_.get());
    data2.WriteRemoteObject(finishCallBack_->AsObject());
    res = windowAnimationStub_->OnRemoteRequest(
        RSIWindowAnimationController::ON_CLOSE_WINDOW, data2, reply, option);
    ASSERT_EQ(res, ERR_NONE);
    GTEST_LOG_(INFO) << "RSWindowAnimationStubTest CloseWindow002 end";
}

/**
 * @tc.name: ScreenUnlock001
 * @tc.desc: Verify the ScreenUnlock
 * @tc.type:FUNC
 */
HWTEST_F(RSWindowAnimationStubTest, ScreenUnlock001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSWindowAnimationStubTest ScreenUnlock001 start";
    MessageParcel data1;
    MessageParcel reply;
    MessageOption option;
    data1.WriteInterfaceToken(RSIWindowAnimationController::GetDescriptor());
    int res = windowAnimationStub_->OnRemoteRequest(
        RSIWindowAnimationController::ON_SCREEN_UNLOCK, data1, reply, option);
    ASSERT_EQ(res, ERR_INVALID_DATA);
    MessageParcel data2;
    data2.WriteInterfaceToken(RSIWindowAnimationController::GetDescriptor());
    data2.WriteRemoteObject(finishCallBack_->AsObject());
    res = windowAnimationStub_->OnRemoteRequest(
        RSIWindowAnimationController::ON_SCREEN_UNLOCK, data2, reply, option);
    ASSERT_EQ(res, ERR_NONE);
    GTEST_LOG_(INFO) << "RSWindowAnimationStubTest ScreenUnlock001 end";
}

/**
 * @tc.name: WindowAnimationTargetsUpdate001
 * @tc.desc: Verify the WindowAnimationTargetsUpdate
 * @tc.type:FUNC
 */
HWTEST_F(RSWindowAnimationStubTest, WindowAnimationTargetsUpdate001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSWindowAnimationStubTest WindowAnimationTargetsUpdate001 start";
    MessageParcel reply;
    MessageOption option;
    MessageParcel data1;
    data1.WriteInterfaceToken(RSIWindowAnimationController::GetDescriptor());
    data1.WriteBool(false);
    data1.WriteUint32(1000);
    int res = windowAnimationStub_->OnRemoteRequest(
        RSIWindowAnimationController::ON_WINDOW_ANIMATION_TARGETS_UPDATE, data1, reply, option);
    ASSERT_EQ(res, ERR_INVALID_DATA);
    MessageParcel data2;
    data2.WriteInterfaceToken(RSIWindowAnimationController::GetDescriptor());
    data2.WriteBool(false);
    data2.WriteUint32(1);
    res = windowAnimationStub_->OnRemoteRequest(
        RSIWindowAnimationController::ON_WINDOW_ANIMATION_TARGETS_UPDATE, data2, reply, option);
    ASSERT_EQ(res, ERR_INVALID_DATA);
    MessageParcel data3;
    data3.WriteInterfaceToken(RSIWindowAnimationController::GetDescriptor());
    data3.WriteBool(false);
    data3.WriteUint32(1);
    data3.WriteParcelable(windowAnimationTarget_.get());
    res = windowAnimationStub_->OnRemoteRequest(
        RSIWindowAnimationController::ON_WINDOW_ANIMATION_TARGETS_UPDATE, data3, reply, option);
    ASSERT_EQ(res, ERR_NONE);
    GTEST_LOG_(INFO) << "RSWindowAnimationStubTest CloseWinWindowAnimationTargetsUpdate001dow001 end";
}

/**
 * @tc.name: WallpaperUpdate001
 * @tc.desc: Verify the WallpaperUpdate
 * @tc.type:FUNC
 */
HWTEST_F(RSWindowAnimationStubTest, WallpaperUpdate001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSWindowAnimationStubTest WallpaperUpdate001 start";
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    data.WriteInterfaceToken(RSIWindowAnimationController::GetDescriptor());
    data.WriteInt32(StartingAppType::FROM_LAUNCHER);
    int res = windowAnimationStub_->OnRemoteRequest(
        RSIWindowAnimationController::ON_WALLPAPER_UPDATE, data, reply, option);
    ASSERT_EQ(res, ERR_NONE);
    GTEST_LOG_(INFO) << "RSWindowAnimationStubTest WallpaperUpdate001 end";
}
} // namespace Rosen
} // namespace OHOS