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

#include "rs_animation_test_utils.h"

#include "gtest/gtest.h"
#include "rs_motion_path_option.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
using namespace ANIMATIONTEST;
class RSMotionPathOptionTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void RSMotionPathOptionTest::SetUpTestCase() {}
void RSMotionPathOptionTest::TearDownTestCase() {}
void RSMotionPathOptionTest::SetUp() {}
void RSMotionPathOptionTest::TearDown() {}

/**
 * @tc.name: GetPathTest001
 * @tc.desc: Verify the GetPath of MotionPathOptionTest
 * @tc.type: FUNC
 */
HWTEST_F(RSMotionPathOptionTest, GetPathTest001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSMotionPathOptionTest GetPathTest001 start";
    /**
     * @tc.steps: step1. init GetPath
     */
    auto motionPathOption = std::make_unique<RSMotionPathOption>(ANIMATION_PATH);
    /**
     * @tc.steps: step2. start GetPath test
     */
    EXPECT_EQ(ANIMATION_PATH, motionPathOption->GetPath());
    GTEST_LOG_(INFO) << "RSMotionPathOptionTest GetPathTest001 end";
}

/**
 * @tc.name: GetBeginFractionTest001
 * @tc.desc: Verify the GetBeginFraction of MotionPathOptionTest
 * @tc.type: FUNC
 */
HWTEST_F(RSMotionPathOptionTest, GetBeginFractionTest001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSMotionPathOptionTest GetBeginFractionTest001 start";
    /**
     * @tc.steps: step1. init GetBeginFraction
     */
    auto motionPathOption = std::make_unique<RSMotionPathOption>(ANIMATION_PATH);
    for (const auto& data : BEGIN_FRACTION_DATA) {
        motionPathOption->SetBeginFraction(data);
        EXPECT_EQ(data, motionPathOption->GetBeginFraction());
    }
    /**
     * @tc.steps: step2. start GetBeginFraction test
     */
    EXPECT_EQ(ANIMATION_PATH, motionPathOption->GetPath());
    GTEST_LOG_(INFO) << "RSMotionPathOptionTest GetBeginFractionTest001 end";
}

/**
 * @tc.name: GetEndFractionTest001
 * @tc.desc: Verify the GetEndFraction of MotionPathOptionTest
 * @tc.type: FUNC
 */
HWTEST_F(RSMotionPathOptionTest, GetEndFractionTest001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSMotionPathOptionTest GetEndFractionTest001 start";
    /**
     * @tc.steps: step1. init GetEndFraction
     */
    auto motionPathOption = std::make_unique<RSMotionPathOption>(ANIMATION_PATH);

    for (const auto& data : END_FRACTION_DATA) {
        motionPathOption->SetEndFraction(data);
        EXPECT_EQ(data, motionPathOption->GetEndFraction());
    }
    /**
     * @tc.steps: step2. start GetEndFraction test
     */
    GTEST_LOG_(INFO) << "RSMotionPathOptionTest GetEndFractionTest001 end";
}

/**
 * @tc.name: GetRotationModeTest001
 * @tc.desc: Verify the GetRotationMode of MotionPathOptionTest
 * @tc.type: FUNC
 */
HWTEST_F(RSMotionPathOptionTest, GetRotationModeTest001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSMotionPathOptionTest GetRotationModeTest001 start";
    /**
     * @tc.steps: step1. init GetRotationMode
     */
    auto motionPathOption = std::make_unique<RSMotionPathOption>(ANIMATION_PATH);
    motionPathOption->SetRotationMode(RotationMode::ROTATE_NONE);
    EXPECT_EQ(RotationMode::ROTATE_NONE, motionPathOption->GetRotationMode());
    /**
     * @tc.steps: step2. start GetRotationMode test
     */
    GTEST_LOG_(INFO) << "RSMotionPathOptionTest GetRotationModeTest001 end";
}

/**
 * @tc.name: GetPathNeedAddOrigin001
 * @tc.desc: Verify the GetPathNeedAddOrigin of MotionPathOptionTest
 * @tc.type: FUNC
 */
HWTEST_F(RSMotionPathOptionTest, GetPathNeedAddOrigin001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSMotionPathOptionTest GetPathNeedAddOrigin001 start";
    /**
     * @tc.steps: step1. init GetPathNeedAddOrigin
     */
    auto motionPathOption = std::make_unique<RSMotionPathOption>(ANIMATION_PATH);
    motionPathOption->SetPathNeedAddOrigin(true);
    ASSERT_TRUE(motionPathOption->GetPathNeedAddOrigin());
    /**
     * @tc.steps: step2. start GetPathNeedAddOrigin test
     */
    GTEST_LOG_(INFO) << "RSMotionPathOptionTest GetPathNeedAddOrigin001 end";
}
} // namespace Rosen
} // namespace OHOS