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
HWTEST_F(BootAnimationOperationTest, BootAnimationOperationTest_001, TestSize.Level1)
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
HWTEST_F(BootAnimationOperationTest, BootAnimationOperationTest_002, TestSize.Level1)
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
    operation.InitRsSurfaceNode(degree);
    operation.InitRsSurface();
    ASSERT_NE(nullptr, operation.rsSurface_);
}

/**
 * @tc.name: BootAnimationOperationTest_006
 * @tc.desc: Verify the IsBootVideoEnabled
 * @tc.type:FUNC
 */
HWTEST_F(BootAnimationOperationTest, BootAnimationOperationTest_006, TestSize.Level1)
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
HWTEST_F(BootAnimationOperationTest, BootAnimationOperationTest_007, TestSize.Level1)
{
    BootAnimationOperation operation;
    BootAnimationConfig config;
    config.videoDefaultPath = "abc";
    EXPECT_EQ(true, operation.IsBootVideoEnabled(config));
}
}
