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

#include <parameters.h>
#include "boot_animation_operation.h"
#include "boot_animation_strategy.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Rosen {
class BootAnimationOperationTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void BootAnimationOperationTest::SetUpTestCase() {}
void BootAnimationOperationTest::TearDownTestCase() {}
void BootAnimationOperationTest::SetUp() {}
void BootAnimationOperationTest::TearDown() {}

/**
 * @tc.name: SetSoundEnable_Enabled_SetCorrectly
 * @tc.desc: Verify the SetSoundEnable function sets sound enabled correctly.
 * @tc.type: FUNC
 */
HWTEST_F(BootAnimationOperationTest, SetSoundEnable_Enabled_SetCorrectly, TestSize.Level0)
{
    BootAnimationOperation operation;
    operation.SetSoundEnable(true);
    EXPECT_EQ(operation.isSoundEnabled_, true);
}

/**
 * @tc.name: SetSoundEnable_Disabled_SetCorrectly
 * @tc.desc: Verify the SetSoundEnable function sets sound disabled correctly.
 * @tc.type: FUNC
 */
HWTEST_F(BootAnimationOperationTest, SetSoundEnable_Disabled_SetCorrectly, TestSize.Level0)
{
    BootAnimationOperation operation;
    operation.SetSoundEnable(false);
    EXPECT_EQ(operation.isSoundEnabled_, false);
}

/**
 * @tc.name: InitRsDisplayNode_Normal_NodeCreated
 * @tc.desc: Verify the InitRsDisplayNode function creates display node successfully.
 * @tc.type: FUNC
 */
HWTEST_F(BootAnimationOperationTest, InitRsDisplayNode_Normal_NodeCreated, TestSize.Level1)
{
    BootAnimationOperation operation;
    std::shared_ptr<BootAnimationStrategy> strategy = std::make_shared<BootAnimationStrategy>();
    strategy->GetConnectToRenderMap(1);
    operation.connectToRender_ = strategy->connectToRenderMap_.begin()->second;
    bool result = operation.InitRsDisplayNode();
    ASSERT_NE(nullptr, operation.rsDisplayNode_);
    EXPECT_TRUE(result);
}

/**
 * @tc.name: InitRsSurfaceNode_Normal_NodeCreated
 * @tc.desc: Verify the InitRsSurfaceNode function creates surface node successfully.
 * @tc.type: FUNC
 */
HWTEST_F(BootAnimationOperationTest, InitRsSurfaceNode_Normal_NodeCreated, TestSize.Level1)
{
    BootAnimationOperation operation;
    std::shared_ptr<BootAnimationStrategy> strategy = std::make_shared<BootAnimationStrategy>();
    strategy->GetConnectToRenderMap(1);
    operation.connectToRender_ = strategy->connectToRenderMap_.begin()->second;
    int32_t degree = 0;
    operation.InitRsDisplayNode();
    bool result = operation.InitRsSurfaceNode(degree);
    ASSERT_NE(nullptr, operation.rsSurfaceNode_);
    EXPECT_TRUE(result);
}

/**
 * @tc.name: InitRsSurface_Normal_SurfaceCreated
 * @tc.desc: Verify the InitRsSurface function creates surface successfully.
 * @tc.type: FUNC
 */
HWTEST_F(BootAnimationOperationTest, InitRsSurface_Normal_SurfaceCreated, TestSize.Level1)
{
    BootAnimationOperation operation;
    std::shared_ptr<BootAnimationStrategy> strategy = std::make_shared<BootAnimationStrategy>();
    strategy->GetConnectToRenderMap(1);
    operation.connectToRender_ = strategy->connectToRenderMap_.begin()->second;
    int32_t degree = 0;
    operation.InitRsDisplayNode();
    operation.InitRsSurfaceNode(degree);
    bool result = operation.InitRsSurface();
    ASSERT_NE(nullptr, operation.rsSurface_);
    EXPECT_TRUE(result);
}

/**
 * @tc.name: IsBootVideoEnabled_PicZipPathOnly_ReturnFalse
 * @tc.desc: Verify the IsBootVideoEnabled function returns false when only picZipPath exists.
 * @tc.type: FUNC
 */
HWTEST_F(BootAnimationOperationTest, IsBootVideoEnabled_PicZipPathOnly_ReturnFalse, TestSize.Level0)
{
    BootAnimationOperation operation;
    BootAnimationConfig config;
    config.picZipPath = "abc";
    EXPECT_EQ(false, operation.IsBootVideoEnabled(config));
}

/**
 * @tc.name: IsBootVideoEnabled_VideoDefaultPathExists_ReturnTrue
 * @tc.desc: Verify the IsBootVideoEnabled function returns true when videoDefaultPath exists.
 * @tc.type: FUNC
 */
HWTEST_F(BootAnimationOperationTest, IsBootVideoEnabled_VideoDefaultPathExists_ReturnTrue, TestSize.Level0)
{
    BootAnimationOperation operation;
    BootAnimationConfig config;
    config.videoDefaultPath = "abc";
    EXPECT_EQ(true, operation.IsBootVideoEnabled(config));
}

/**
 * @tc.name: StartEventHandler_DifferentConfigs_ExecuteSuccessfully
 * @tc.desc: Verify the StartEventHandler function executes with different configs.
 * @tc.type: FUNC
 */
HWTEST_F(BootAnimationOperationTest, StartEventHandler_DifferentConfigs_ExecuteSuccessfully, TestSize.Level1)
{
    BootAnimationOperation operation;
    BootAnimationConfig config1;
    config1.videoDefaultPath = "abc";
    EXPECT_EQ(true, operation.IsBootVideoEnabled(config1));
    operation.StartEventHandler(config1);
    EXPECT_TRUE(true);

    BootAnimationConfig config2;
    config2.picZipPath = "abc";
    EXPECT_EQ(false, operation.IsBootVideoEnabled(config2));
    operation.StartEventHandler(config2);
    EXPECT_TRUE(true);
}

/**
 * @tc.name: InitRsSurfaceNode_BootAnimationReady_NodeCreated
 * @tc.desc: Verify the InitRsSurfaceNode function when boot animation ready.
 * @tc.type: FUNC
 */
HWTEST_F(BootAnimationOperationTest, InitRsSurfaceNode_BootAnimationReady_NodeCreated, TestSize.Level1)
{
    BootAnimationOperation operation;
    std::shared_ptr<BootAnimationStrategy> strategy = std::make_shared<BootAnimationStrategy>();
    strategy->GetConnectToRenderMap(1);
    operation.connectToRender_ = strategy->connectToRenderMap_.begin()->second;
    int32_t degree = 0;
    system::SetParameter(BOOT_ANIMATION_READY, "true");
    operation.InitRsDisplayNode();
    bool result = operation.InitRsSurfaceNode(degree);
    ASSERT_NE(nullptr, operation.rsSurfaceNode_);
    EXPECT_TRUE(result);
}

/**
 * @tc.name: PlayPicture_DifferentStates_ExecuteSuccessfully
 * @tc.desc: Verify the PlayPicture function executes with different states.
 * @tc.type: FUNC
 */
HWTEST_F(BootAnimationOperationTest, PlayPicture_DifferentStates_ExecuteSuccessfully, TestSize.Level1)
{
    BootAnimationOperation operation;
    std::string path = "abc";
    operation.runner_ = AppExecFwk::EventRunner::Create(false);
    system::SetParameter(BOOT_ANIMATION_STARTED, "false");
    operation.PlayPicture(path);
    EXPECT_TRUE(true);
    
    system::SetParameter(BOOT_ANIMATION_STARTED, "true");
    operation.PlayPicture(path);
    EXPECT_EQ(system::GetParameter(BOOT_ANIMATION_STARTED, "false"), "true");
}

/**
 * @tc.name: IsBootVideoEnabled_EmptyConfig_ReturnTrue
 * @tc.desc: Verify the IsBootVideoEnabled function returns true with empty config.
 * @tc.type: FUNC
 */
HWTEST_F(BootAnimationOperationTest, IsBootVideoEnabled_EmptyConfig_ReturnTrue, TestSize.Level1)
{
    BootAnimationOperation operation;
    BootAnimationConfig config;
    EXPECT_EQ(true, operation.IsBootVideoEnabled(config));
    
    config.videoDefaultPath = "abc";
    config.picZipPath = "abc";
    EXPECT_EQ(true, operation.IsBootVideoEnabled(config));
}

/**
 * @tc.name: StopBootAnimation_DifferentStates_ExecuteSuccessfully
 * @tc.desc: Verify the StopBootAnimation function executes with different states.
 * @tc.type: FUNC
 */
HWTEST_F(BootAnimationOperationTest, StopBootAnimation_DifferentStates_ExecuteSuccessfully, TestSize.Level1)
{
    BootAnimationOperation operation;
    operation.runner_ = AppExecFwk::EventRunner::Create(false);
    operation.mainHandler_ = std::make_shared<AppExecFwk::EventHandler>(operation.runner_);
    system::SetParameter(BOOT_ANIMATION_STARTED, "false");
    operation.mainHandler_->PostTask([&operation] { operation.StopBootAnimation(); });
    EXPECT_EQ(system::GetParameter(BOOT_ANIMATION_STARTED, "true"), "false");
    operation.runner_->Run();
    EXPECT_TRUE(true);

    operation.mainHandler_ = std::make_shared<AppExecFwk::EventHandler>(operation.runner_);
    system::SetParameter(BOOT_ANIMATION_STARTED, "true");
    operation.mainHandler_->PostTask([&operation] { operation.StopBootAnimation(); });
    EXPECT_EQ(system::GetParameter(BOOT_ANIMATION_STARTED, "false"), "true");
    operation.runner_->Run();
    EXPECT_TRUE(true);
}
}
