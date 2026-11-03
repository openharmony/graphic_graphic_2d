/*
 * Copyright (c) 2026 Huawei Device Co., Ltd.
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

#include "rs_window_animation_controller.h"
#include "rs_window_animation_finished_callback.h"
#include "rs_window_animation_target.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
class RSWindowAnimationControllerTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;

    sptr<RSWindowAnimationController> controller_;
};

void RSWindowAnimationControllerTest::SetUpTestCase() {}
void RSWindowAnimationControllerTest::TearDownTestCase() {}

void RSWindowAnimationControllerTest::SetUp()
{
    controller_ = new RSWindowAnimationController(nullptr);
}

void RSWindowAnimationControllerTest::TearDown()
{
    controller_ = nullptr;
}

/**
 * @tc.name: SetJsController_EnvNull
 * @tc.desc: Verify SetJsController returns early when env_ is null
 * @tc.type: FUNC
 */
HWTEST_F(RSWindowAnimationControllerTest, SetJsController_EnvNull, TestSize.Level1)
{
    controller_->env_ = nullptr;
    controller_->SetJsController(nullptr);
    EXPECT_EQ(controller_->env_, nullptr);
}

/**
 * @tc.name: HandleOnStartApp_EnvNull
 * @tc.desc: Verify HandleOnStartApp does not crash when env_ is null
 * @tc.type: FUNC
 */
HWTEST_F(RSWindowAnimationControllerTest, HandleOnStartApp_EnvNull, TestSize.Level1)
{
    controller_->env_ = nullptr;
    sptr<RSWindowAnimationTarget> target = new RSWindowAnimationTarget();
    sptr<RSIWindowAnimationFinishedCallback> callback =
        new RSWindowAnimationFinishedCallback(nullptr);
    controller_->HandleOnStartApp(StartingAppType::FROM_LAUNCHER, target, callback);
    EXPECT_EQ(controller_->env_, nullptr);
}

/**
 * @tc.name: HandleOnAppTransition_EnvNull
 * @tc.desc: Verify HandleOnAppTransition does not crash when env_ is null
 * @tc.type: FUNC
 */
HWTEST_F(RSWindowAnimationControllerTest, HandleOnAppTransition_EnvNull, TestSize.Level1)
{
    controller_->env_ = nullptr;
    sptr<RSWindowAnimationTarget> target = new RSWindowAnimationTarget();
    sptr<RSIWindowAnimationFinishedCallback> callback =
        new RSWindowAnimationFinishedCallback(nullptr);
    controller_->HandleOnAppTransition(target, target, callback);
    EXPECT_EQ(controller_->env_, nullptr);
}

/**
 * @tc.name: HandleOnAppBackTransition_EnvNull
 * @tc.desc: Verify HandleOnAppBackTransition does not crash when env_ is null
 * @tc.type: FUNC
 */
HWTEST_F(RSWindowAnimationControllerTest, HandleOnAppBackTransition_EnvNull, TestSize.Level1)
{
    controller_->env_ = nullptr;
    sptr<RSWindowAnimationTarget> target = new RSWindowAnimationTarget();
    sptr<RSIWindowAnimationFinishedCallback> callback =
        new RSWindowAnimationFinishedCallback(nullptr);
    controller_->HandleOnAppBackTransition(target, target, callback);
    EXPECT_EQ(controller_->env_, nullptr);
}

/**
 * @tc.name: HandleOnMinimizeWindow_EnvNull
 * @tc.desc: Verify HandleOnMinimizeWindow does not crash when env_ is null
 * @tc.type: FUNC
 */
HWTEST_F(RSWindowAnimationControllerTest, HandleOnMinimizeWindow_EnvNull, TestSize.Level1)
{
    controller_->env_ = nullptr;
    sptr<RSWindowAnimationTarget> target = new RSWindowAnimationTarget();
    sptr<RSIWindowAnimationFinishedCallback> callback =
        new RSWindowAnimationFinishedCallback(nullptr);
    controller_->HandleOnMinimizeWindow(target, callback);
    EXPECT_EQ(controller_->env_, nullptr);
}

/**
 * @tc.name: HandleOnCloseWindow_EnvNull
 * @tc.desc: Verify HandleOnCloseWindow does not crash when env_ is null
 * @tc.type: FUNC
 */
HWTEST_F(RSWindowAnimationControllerTest, HandleOnCloseWindow_EnvNull, TestSize.Level1)
{
    controller_->env_ = nullptr;
    sptr<RSWindowAnimationTarget> target = new RSWindowAnimationTarget();
    sptr<RSIWindowAnimationFinishedCallback> callback =
        new RSWindowAnimationFinishedCallback(nullptr);
    controller_->HandleOnCloseWindow(target, callback);
    EXPECT_EQ(controller_->env_, nullptr);
}

/**
 * @tc.name: HandleOnScreenUnlock_EnvNull
 * @tc.desc: Verify HandleOnScreenUnlock does not crash when env_ is null
 * @tc.type: FUNC
 */
HWTEST_F(RSWindowAnimationControllerTest, HandleOnScreenUnlock_EnvNull, TestSize.Level1)
{
    controller_->env_ = nullptr;
    sptr<RSIWindowAnimationFinishedCallback> callback =
        new RSWindowAnimationFinishedCallback(nullptr);
    controller_->HandleOnScreenUnlock(callback);
    EXPECT_EQ(controller_->env_, nullptr);
}

/**
 * @tc.name: HandleOnWindowAnimationTargetsUpdate_EnvNull
 * @tc.desc: Verify HandleOnWindowAnimationTargetsUpdate does not crash when env_ is null
 * @tc.type: FUNC
 */
HWTEST_F(RSWindowAnimationControllerTest, HandleOnWindowAnimationTargetsUpdate_EnvNull,
    TestSize.Level1)
{
    controller_->env_ = nullptr;
    sptr<RSWindowAnimationTarget> target = new RSWindowAnimationTarget();
    std::vector<sptr<RSWindowAnimationTarget>> floatingTargets;
    controller_->HandleOnWindowAnimationTargetsUpdate(target, floatingTargets);
    EXPECT_EQ(controller_->env_, nullptr);
}

/**
 * @tc.name: HandleOnWallpaperUpdate_EnvNull
 * @tc.desc: Verify HandleOnWallpaperUpdate does not crash when env_ is null
 * @tc.type: FUNC
 */
HWTEST_F(RSWindowAnimationControllerTest, HandleOnWallpaperUpdate_EnvNull, TestSize.Level1)
{
    controller_->env_ = nullptr;
    sptr<RSWindowAnimationTarget> target = new RSWindowAnimationTarget();
    controller_->HandleOnWallpaperUpdate(target);
    EXPECT_EQ(controller_->env_, nullptr);
}

/**
 * @tc.name: CallJsFunction_EnvNull
 * @tc.desc: Verify CallJsFunction returns early when env_ is null
 * @tc.type: FUNC
 */
HWTEST_F(RSWindowAnimationControllerTest, CallJsFunction_EnvNull, TestSize.Level1)
{
    controller_->env_ = nullptr;
    controller_->CallJsFunction("onStartApp", nullptr, 0);
    EXPECT_EQ(controller_->env_, nullptr);
}

/**
 * @tc.name: EnvCleanupHook_001
 * @tc.desc: Verify EnvCleanupHook sets env_ to nullptr
 * @tc.type: FUNC
 */
HWTEST_F(RSWindowAnimationControllerTest, EnvCleanupHook_001, TestSize.Level1)
{
    controller_->env_ = reinterpret_cast<napi_env>(0x1);
    RSWindowAnimationController::EnvCleanupHook(controller_.GetRefPtr());
    EXPECT_EQ(controller_->env_, nullptr);
}

} // namespace Rosen
} // namespace OHOS
