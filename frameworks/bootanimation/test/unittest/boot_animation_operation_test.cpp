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
 * @tc.name: BootAnimationOperationTest_001
 * @tc.desc: Verify the SetSoundEnable
 * @tc.type:FUNC
 */
HWTEST_F(BootAnimationOperationTest, BootAnimationOperationTest_001, TestSize.Level0)
{
    BootAnimationOperation operation;
    operation.SetSoundEnable(true);
    EXPECT_EQ(operation.isSoundEnabled_, true);
}

/**
 * @tc.name: BootAnimationOperationTest_002
 * @tc.desc: Verify the SetSoundEnable
 * @tc.type:FUNC
 */
HWTEST_F(BootAnimationOperationTest, BootAnimationOperationTest_002, TestSize.Level0)
{
    BootAnimationOperation operation;
    operation.SetSoundEnable(false);
    EXPECT_EQ(operation.isSoundEnabled_, false);
}

/**
 * @tc.name: BootAnimationOperationTest_003
 * @tc.desc: Verify the InitRsDisplayNode
 * @tc.type:FUNC
 */
HWTEST_F(BootAnimationOperationTest, BootAnimationOperationTest_003, TestSize.Level1)
{
    BootAnimationOperation operation;
    operation.InitRsDisplayNode();
    ASSERT_NE(nullptr, operation.rsDisplayNode_);
}

/**
 * @tc.name: BootAnimationOperationTest_004
 * @tc.desc: Verify the InitRsSurfaceNode
 * @tc.type:FUNC
 */
HWTEST_F(BootAnimationOperationTest, BootAnimationOperationTest_004, TestSize.Level1)
{
    BootAnimationOperation operation;
    int32_t degree = 0;
    operation.InitRsDisplayNode();
    operation.InitRsSurfaceNode(degree);
    ASSERT_NE(nullptr, operation.rsSurfaceNode_);
}

/**
 * @tc.name: BootAnimationOperationTest_005
 * @tc.desc: Verify the InitRsSurface
 * @tc.type:FUNC
 */
HWTEST_F(BootAnimationOperationTest, BootAnimationOperationTest_005, TestSize.Level1)
{
    BootAnimationOperation operation;
    int32_t degree = 0;
    operation.InitRsDisplayNode();
    operation.InitRsSurfaceNode(degree);
    operation.InitRsSurface();
    ASSERT_NE(nullptr, operation.rsSurface_);
}

/**
 * @tc.name: BootAnimationOperationTest_006
 * @tc.desc: Verify the IsBootVideoEnabled
 * @tc.type:FUNC
 */
HWTEST_F(BootAnimationOperationTest, BootAnimationOperationTest_006, TestSize.Level0)
{
    BootAnimationOperation operation;
    BootAnimationConfig config;
    config.picZipPath = "abc";
    EXPECT_EQ(false, operation.IsBootVideoEnabled(config));
}

/**
 * @tc.name: BootAnimationOperationTest_007
 * @tc.desc: Verify the IsBootVideoEnabled
 * @tc.type:FUNC
 */
HWTEST_F(BootAnimationOperationTest, BootAnimationOperationTest_007, TestSize.Level0)
{
    BootAnimationOperation operation;
    BootAnimationConfig config;
    config.videoDefaultPath = "abc";
    EXPECT_EQ(true, operation.IsBootVideoEnabled(config));
}

/**
 * @tc.name: BootAnimationOperationTest_008
 * @tc.desc: Verify the StartEventHandler
 * @tc.type:FUNC
 */
HWTEST_F(BootAnimationOperationTest, BootAnimationOperationTest_008, TestSize.Level1)
{
    BootAnimationOperation operation;
    BootAnimationConfig config1;
    config1.videoDefaultPath = "abc";
    EXPECT_EQ(true, operation.IsBootVideoEnabled(config1));
    operation.StartEventHandler(config1);

    BootAnimationConfig config2;
    config2.picZipPath = "abc";
    EXPECT_EQ(false, operation.IsBootVideoEnabled(config2));
    operation.StartEventHandler(config2);
}

/**
 * @tc.name: BootAnimationOperationTest_009
 * @tc.desc: Verify the InitRsSurfaceNode
 * @tc.type:FUNC
 */
HWTEST_F(BootAnimationOperationTest, BootAnimationOperationTest_009, TestSize.Level1)
{
    BootAnimationOperation operation;
    int32_t degree = 0;
    system::SetParameter(BOOT_ANIMATION_READY, "true");
    operation.InitRsDisplayNode();
    operation.InitRsSurfaceNode(degree);
    ASSERT_NE(nullptr, operation.rsSurfaceNode_);
}

/**
 * @tc.name: BootAnimationOperationTest_010
 * @tc.desc: Verify the PlayPicture
 * @tc.type:FUNC
 */
HWTEST_F(BootAnimationOperationTest, BootAnimationOperationTest_010, TestSize.Level1)
{
    BootAnimationOperation operation;
    std::string path = "abc";
    operation.runner_ = AppExecFwk::EventRunner::Create(false);
    system::SetParameter(BOOT_ANIMATION_STARTED, "false");
    operation.PlayPicture(path);
    
    system::SetParameter(BOOT_ANIMATION_STARTED, "true");
    operation.PlayPicture(path);
    EXPECT_EQ(system::GetParameter(BOOT_ANIMATION_STARTED, "false"), "true");
}

/**
 * @tc.name: BootAnimationOperationTest_011
 * @tc.desc: Verify the IsBootVideoEnabled
 * @tc.type:FUNC
 */
HWTEST_F(BootAnimationOperationTest, BootAnimationOperationTest_011, TestSize.Level1)
{
    BootAnimationOperation operation;
    BootAnimationConfig config;
    EXPECT_EQ(true, operation.IsBootVideoEnabled(config));
    
    config.videoDefaultPath = "abc";
    config.picZipPath = "abc";
    EXPECT_EQ(true, operation.IsBootVideoEnabled(config));
}

/**
 * @tc.name: BootAnimationOperationTest_012
 * @tc.desc: Verify the StopBootAnimation
 * @tc.type:FUNC
 */
HWTEST_F(BootAnimationOperationTest, BootAnimationOperationTest_012, TestSize.Level1)
{
    BootAnimationOperation operation;
    operation.runner_ = AppExecFwk::EventRunner::Create(false);
    operation.mainHandler_ = std::make_shared<AppExecFwk::EventHandler>(operation.runner_);
    system::SetParameter(BOOT_ANIMATION_STARTED, "false");
    operation.mainHandler_->PostTask([&operation] { operation.StopBootAnimation(); });
    EXPECT_EQ(system::GetParameter(BOOT_ANIMATION_STARTED, "true"), "false");
    operation.runner_->Run();

    operation.mainHandler_ = std::make_shared<AppExecFwk::EventHandler>(operation.runner_);
    system::SetParameter(BOOT_ANIMATION_STARTED, "true");
    operation.mainHandler_->PostTask([&operation] { operation.StopBootAnimation(); });
    EXPECT_EQ(system::GetParameter(BOOT_ANIMATION_STARTED, "false"), "true");
    operation.runner_->Run();
}
}
