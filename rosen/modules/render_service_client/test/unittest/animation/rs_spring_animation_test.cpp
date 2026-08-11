/*
 * Copyright (c) 2022-2023 Huawei Device Co., Ltd.
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
#include "rs_animation_test_utils.h"

#include "animation/rs_interpolating_spring_animation.h"
#include "animation/rs_spring_animation.h"
#include "ui/rs_canvas_node.h"
#include "ui/rs_ui_context.h"
#include "animation/rs_spring_interpolator.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
using namespace ANIMATIONTEST;
class RSSpringAnimationTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void RSSpringAnimationTest::SetUpTestCase() {}
void RSSpringAnimationTest::TearDownTestCase() {}
void RSSpringAnimationTest::SetUp() {}
void RSSpringAnimationTest::TearDown() {}

class RSNodeMock : public RSNode {
public:
    explicit RSNodeMock(
        bool isRenderServiceNode, bool isTextureExportNode = false, std::shared_ptr<RSUIContext> rsUIContext = nullptr)
        : RSNode(isRenderServiceNode, isTextureExportNode, rsUIContext)
    {}
    ~RSNodeMock() = default;

    bool NeedForcedSendToRemote() const override
    {
        return true;
    }
};

/**
 * @tc.name: InterpolateImplTest
 * @tc.desc: Verify the InterpolateImpl
 * @tc.type: FUNC
 */
HWTEST_F(RSSpringAnimationTest, InterpolateImplTest, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSSpringAnimationTest InterpolateImplTest start";
    RSSpringInterpolator interpolator(1.f, 1.f, 0.f);

    // case1: fraction <= 0
    float fraction = -1.0f;
    ASSERT_EQ(interpolator.InterpolateImpl(fraction), 0);

    // case2: fraction >= 1
    fraction = 1.5f;
    ASSERT_EQ(interpolator.InterpolateImpl(fraction), 1);

    // case3: 0 < fraction < 1
    fraction = 0.5f;
    ASSERT_NE(interpolator.InterpolateImpl(fraction), 0);
    GTEST_LOG_(INFO) << "RSSpringAnimationTest InterpolateImplTest end";
}

/**
 * @tc.name: RSSpringSetZeroThresholdTest_001
 * @tc.desc: Verify SetZeroThreshold with positive value
 * @tc.type: FUNC
 * @tc.require:
 * @tc.author:
 */
HWTEST_F(RSSpringAnimationTest, RSSpringSetZeroThresholdTest_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSSpringAnimationTest RSSpringSetZeroThresholdTest_001 start";
    auto property = std::make_shared<RSAnimatableProperty<float>>(0.0f);
    auto byValue = std::make_shared<RSAnimatableProperty<float>>(1.0f);
    auto animation = std::make_shared<RSSpringAnimation>(nullptr, property, byValue);
    constexpr float positiveThreshold = 0.001f;
    animation->SetZeroThreshold(positiveThreshold);
    EXPECT_EQ(animation->zeroThreshold_, positiveThreshold);
    GTEST_LOG_(INFO) << "RSSpringAnimationTest RSSpringSetZeroThresholdTest_001 end";
}

/**
 * @tc.name: RSSpringSetZeroThresholdTest_002
 * @tc.desc: Verify SetZeroThreshold rejects negative value
 * @tc.type: FUNC
 * @tc.require:
 * @tc.author:
 */
HWTEST_F(RSSpringAnimationTest, RSSpringSetZeroThresholdTest_002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSSpringAnimationTest RSSpringSetZeroThresholdTest_002 start";
    auto property = std::make_shared<RSAnimatableProperty<float>>(0.0f);
    auto byValue = std::make_shared<RSAnimatableProperty<float>>(1.0f);
    auto animation = std::make_shared<RSSpringAnimation>(nullptr, property, byValue);
    constexpr float negativeThreshold = -1.0f;
    animation->SetZeroThreshold(negativeThreshold);
    EXPECT_EQ(animation->zeroThreshold_, 0.0f);
    GTEST_LOG_(INFO) << "RSSpringAnimationTest RSSpringSetZeroThresholdTest_002 end";
}

/**
 * @tc.name: RSSpringSetZeroThresholdTest_003
 * @tc.desc: Verify SetZeroThreshold accepts zero value
 * @tc.type: FUNC
 * @tc.require:
 * @tc.author:
 */
HWTEST_F(RSSpringAnimationTest, RSSpringSetZeroThresholdTest_003, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSSpringAnimationTest RSSpringSetZeroThresholdTest_003 start";
    auto property = std::make_shared<RSAnimatableProperty<float>>(0.0f);
    auto byValue = std::make_shared<RSAnimatableProperty<float>>(1.0f);
    auto animation = std::make_shared<RSSpringAnimation>(nullptr, property, byValue);
    constexpr float zeroThreshold = 0.0f;
    animation->SetZeroThreshold(zeroThreshold);
    EXPECT_EQ(animation->zeroThreshold_, zeroThreshold);
    GTEST_LOG_(INFO) << "RSSpringAnimationTest RSSpringSetZeroThresholdTest_003 end";
}

/**
 * @tc.name: RSInterpolatingSpringSetZeroThresholdTest_001
 * @tc.desc: Verify RSInterpolatingSpringAnimation SetZeroThreshold with positive value
 * @tc.type: FUNC
 * @tc.require:
 * @tc.author:
 */
HWTEST_F(RSSpringAnimationTest, RSInterpolatingSpringSetZeroThresholdTest_001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSSpringAnimationTest RSInterpolatingSpringSetZeroThresholdTest_001 start";
    auto property = std::make_shared<RSAnimatableProperty<float>>(0.0f);
    auto byValue = std::make_shared<RSAnimatableProperty<float>>(1.0f);
    auto animation = std::make_shared<RSInterpolatingSpringAnimation>(nullptr, property, byValue);
    constexpr float positiveThreshold = 0.001f;
    animation->SetZeroThreshold(positiveThreshold);
    EXPECT_EQ(animation->zeroThreshold_, positiveThreshold);
    GTEST_LOG_(INFO) << "RSSpringAnimationTest RSInterpolatingSpringSetZeroThresholdTest_001 end";
}

/**
 * @tc.name: RSInterpolatingSpringSetZeroThresholdTest_002
 * @tc.desc: Verify RSInterpolatingSpringAnimation SetZeroThreshold rejects negative value
 * @tc.type: FUNC
 * @tc.require:
 * @tc.author:
 */
HWTEST_F(RSSpringAnimationTest, RSInterpolatingSpringSetZeroThresholdTest_002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSSpringAnimationTest RSInterpolatingSpringSetZeroThresholdTest_002 start";
    auto property = std::make_shared<RSAnimatableProperty<float>>(0.0f);
    auto byValue = std::make_shared<RSAnimatableProperty<float>>(1.0f);
    auto animation = std::make_shared<RSInterpolatingSpringAnimation>(nullptr, property, byValue);
    constexpr float negativeThreshold = -1.0f;
    animation->SetZeroThreshold(negativeThreshold);
    EXPECT_EQ(animation->zeroThreshold_, 0.0f);
    GTEST_LOG_(INFO) << "RSSpringAnimationTest RSInterpolatingSpringSetZeroThresholdTest_002 end";
}

/**
 * @tc.name: RSInterpolatingSpringSetZeroThresholdTest_003
 * @tc.desc: Verify RSInterpolatingSpringAnimation SetZeroThreshold accepts zero value
 * @tc.type: FUNC
 * @tc.require:
 * @tc.author:
 */
HWTEST_F(RSSpringAnimationTest, RSInterpolatingSpringSetZeroThresholdTest_003, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSSpringAnimationTest RSInterpolatingSpringSetZeroThresholdTest_003 start";
    auto property = std::make_shared<RSAnimatableProperty<float>>(0.0f);
    auto byValue = std::make_shared<RSAnimatableProperty<float>>(1.0f);
    auto animation = std::make_shared<RSInterpolatingSpringAnimation>(nullptr, property, byValue);
    constexpr float zeroThreshold = 0.0f;
    animation->SetZeroThreshold(zeroThreshold);
    EXPECT_EQ(animation->zeroThreshold_, zeroThreshold);
    GTEST_LOG_(INFO) << "RSSpringAnimationTest RSInterpolatingSpringSetZeroThresholdTest_003 end";
}

/**
 * @tc.name: RebuildInRender001
 * @tc.desc: Verify RebuildInRender with null target
 * @tc.type: FUNC
 */
HWTEST_F(RSSpringAnimationTest, RebuildInRender001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSSpringAnimationTest RebuildInRender001 start";
    auto property = std::make_shared<RSAnimatableProperty<Vector4f>>(ANIMATION_START_BOUNDS);
    auto startProperty = std::make_shared<RSAnimatableProperty<Vector4f>>(ANIMATION_START_BOUNDS);
    auto endProperty = std::make_shared<RSAnimatableProperty<Vector4f>>(ANIMATION_END_BOUNDS);
    OHOS::sptr<OHOS::IRemoteObject> connectToRenderRemote;
    auto rsUIContext = std::make_shared<RSUIContext>(0, connectToRenderRemote);
    rsUIContext->SetUITaskRunner([](const std::function<void()>& task, uint32_t delay) { task(); });
    auto springAnimation = std::make_shared<RSSpringAnimation>(rsUIContext, property, startProperty, endProperty);
    springAnimation->SetDuration(300);
    springAnimation->SetRebuildParam({0.5f, false});
    ASSERT_TRUE(springAnimation->GetTarget().lock() == nullptr);
    springAnimation->RebuildInRender();
    ASSERT_EQ(springAnimation->GetRebuildParam().fraction, 0.5f);
    ASSERT_EQ(springAnimation->GetRebuildParam().isReverseCycle, false);
    GTEST_LOG_(INFO) << "RSSpringAnimationTest RebuildInRender001 end";
}

/**
 * @tc.name: CreateRenderAnimationNullValues001
 * @tc.desc: Verify CreateRenderAnimation returns nullptr when originValue/startValue/endValue are null
 * @tc.type:FUNC
 */
HWTEST_F(RSSpringAnimationTest, CreateRenderAnimationNullValues001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSSpringAnimationTest CreateRenderAnimationNullValues001 start";
    auto property = std::make_shared<RSAnimatableProperty<float>>(0.0f);
    auto endValue = std::make_shared<RSAnimatableProperty<float>>(1.0f);
    auto springAnimation = std::make_shared<RSSpringAnimation>(nullptr, property, endValue);
    springAnimation->originValue_ = nullptr;
    springAnimation->startValue_ = nullptr;
    springAnimation->endValue_ = nullptr;
    auto result = springAnimation->CreateRenderAnimation();
    EXPECT_EQ(result, nullptr);
    GTEST_LOG_(INFO) << "RSSpringAnimationTest CreateRenderAnimationNullValues001 end";
}

/**
 * @tc.name: CreateRenderAnimationStartValueNull001
 * @tc.desc: Verify CreateRenderAnimation returns nullptr when only startValue is null
 * @tc.type:FUNC
 */
HWTEST_F(RSSpringAnimationTest, CreateRenderAnimationStartValueNull001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSSpringAnimationTest CreateRenderAnimationStartValueNull001 start";
    auto property = std::make_shared<RSAnimatableProperty<float>>(0.0f);
    auto startValue = std::make_shared<RSAnimatableProperty<float>>(0.0f);
    auto endValue = std::make_shared<RSAnimatableProperty<float>>(1.0f);
    auto springAnimation = std::make_shared<RSSpringAnimation>(nullptr, property, startValue, endValue);
    springAnimation->originValue_ = property;
    springAnimation->startValue_ = nullptr;
    springAnimation->endValue_ = endValue;
    auto result = springAnimation->CreateRenderAnimation();
    EXPECT_EQ(result, nullptr);
    GTEST_LOG_(INFO) << "RSSpringAnimationTest CreateRenderAnimationStartValueNull001 end";
}

/**
 * @tc.name: CreateRenderAnimationEndValueNull001
 * @tc.desc: Verify CreateRenderAnimation returns nullptr when only endValue is null
 * @tc.type:FUNC
 */
HWTEST_F(RSSpringAnimationTest, CreateRenderAnimationEndValueNull001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSSpringAnimationTest CreateRenderAnimationEndValueNull001 start";
    auto property = std::make_shared<RSAnimatableProperty<float>>(0.0f);
    auto startValue = std::make_shared<RSAnimatableProperty<float>>(0.0f);
    auto endValue = std::make_shared<RSAnimatableProperty<float>>(1.0f);
    auto springAnimation = std::make_shared<RSSpringAnimation>(nullptr, property, startValue, endValue);
    springAnimation->originValue_ = property;
    springAnimation->startValue_ = startValue;
    springAnimation->endValue_ = nullptr;
    auto result = springAnimation->CreateRenderAnimation();
    EXPECT_EQ(result, nullptr);
    GTEST_LOG_(INFO) << "RSSpringAnimationTest CreateRenderAnimationEndValueNull001 end";
}

/**
 * @tc.name: CreateRenderAnimationAllNonNull001
 * @tc.desc: Verify CreateRenderAnimation returns valid animation when all values are non-null
 * @tc.type:FUNC
 */
HWTEST_F(RSSpringAnimationTest, CreateRenderAnimationAllNonNull001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSSpringAnimationTest CreateRenderAnimationAllNonNull001 start";
    auto property = std::make_shared<RSAnimatableProperty<float>>(0.0f);
    auto startValue = std::make_shared<RSAnimatableProperty<float>>(0.0f);
    auto endValue = std::make_shared<RSAnimatableProperty<float>>(1.0f);
    auto springAnimation = std::make_shared<RSSpringAnimation>(nullptr, property, startValue, endValue);
    springAnimation->originValue_ = property;
    springAnimation->startValue_ = startValue;
    springAnimation->endValue_ = endValue;
    auto result = springAnimation->CreateRenderAnimation();
    EXPECT_NE(result, nullptr);
    GTEST_LOG_(INFO) << "RSSpringAnimationTest CreateRenderAnimationAllNonNull001 end";
}

/**
 * @tc.name: OnStartAnimationNull001
 * @tc.desc: Verify OnStart handles CreateRenderAnimation returning nullptr
 * @tc.type:FUNC
 */
HWTEST_F(RSSpringAnimationTest, OnStartAnimationNull001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSSpringAnimationTest OnStartAnimationNull001 start";
    auto property = std::make_shared<RSAnimatableProperty<float>>(0.0f);
    auto endValue = std::make_shared<RSAnimatableProperty<float>>(1.0f);
    auto springAnimation = std::make_shared<RSSpringAnimation>(nullptr, property, endValue);
    springAnimation->property_ = nullptr;
    springAnimation->originValue_ = nullptr;
    springAnimation->startValue_ = nullptr;
    springAnimation->endValue_ = nullptr;
    springAnimation->OnStart();
    EXPECT_FALSE(springAnimation->IsRunning());
    GTEST_LOG_(INFO) << "RSSpringAnimationTest OnStartAnimationNull001 end";
}

/**
 * @tc.name: RebuildInRenderAnimationNull001
 * @tc.desc: Verify RebuildInRender handles CreateRenderAnimation returning nullptr
 * @tc.type:FUNC
 */
HWTEST_F(RSSpringAnimationTest, RebuildInRenderAnimationNull001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSSpringAnimationTest RebuildInRenderAnimationNull001 start";
    auto property = std::make_shared<RSAnimatableProperty<float>>(0.0f);
    auto endValue = std::make_shared<RSAnimatableProperty<float>>(1.0f);
    auto springAnimation = std::make_shared<RSSpringAnimation>(nullptr, property, endValue);
    springAnimation->originValue_ = nullptr;
    springAnimation->startValue_ = nullptr;
    springAnimation->endValue_ = nullptr;
    springAnimation->RebuildInRender();
    EXPECT_FALSE(springAnimation->IsRunning());
    GTEST_LOG_(INFO) << "RSSpringAnimationTest RebuildInRenderAnimationNull001 end";
}

/**
 * @tc.name: RebuildInRenderAnimationNullWithTarget001
 * @tc.desc: Verify RebuildInRender handles animation nullptr with valid target
 * @tc.type:FUNC
 */
HWTEST_F(RSSpringAnimationTest, RebuildInRenderAnimationNullWithTarget001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSSpringAnimationTest RebuildInRenderAnimationNullWithTarget001 start";
    auto property = std::make_shared<RSAnimatableProperty<float>>(0.0f);
    auto endValue = std::make_shared<RSAnimatableProperty<float>>(1.0f);
    OHOS::sptr<OHOS::IRemoteObject> connectToRenderRemote;
    auto rsUIContext = std::make_shared<RSUIContext>(0, connectToRenderRemote);
    rsUIContext->SetUITaskRunner([](const std::function<void()>& task, uint32_t delay) { task(); });
    auto canvasNode = RSCanvasNode::Create(false, false, rsUIContext);
    auto springAnimation = std::make_shared<RSSpringAnimation>(rsUIContext, property, endValue);
    springAnimation->target_ = canvasNode;
    springAnimation->originValue_ = nullptr;
    springAnimation->startValue_ = nullptr;
    springAnimation->endValue_ = nullptr;
    springAnimation->SetRebuildParam({0.5f, false});
    springAnimation->RebuildInRender();
    EXPECT_FLOAT_EQ(springAnimation->GetRebuildParam().fraction, 0.5f);
    EXPECT_FALSE(springAnimation->IsRunning());
    GTEST_LOG_(INFO) << "RSSpringAnimationTest RebuildInRenderAnimationNullWithTarget001 end";
}

/**
 * @tc.name: InterpolatingSpringCreateRenderAnimationNull001
 * @tc.desc: Verify RSInterpolatingSpringAnimation CreateRenderAnimation returns nullptr with null values
 * @tc.type:FUNC
 */
HWTEST_F(RSSpringAnimationTest, InterpolatingSpringCreateRenderAnimationNull001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSSpringAnimationTest InterpolatingSpringCreateRenderAnimationNull001 start";
    auto property = std::make_shared<RSAnimatableProperty<float>>(0.0f);
    auto startValue = std::make_shared<RSAnimatableProperty<float>>(0.0f);
    auto byValue = std::make_shared<RSAnimatableProperty<float>>(1.0f);
    auto animation = std::make_shared<RSInterpolatingSpringAnimation>(nullptr, property, byValue);
    animation->originValue_ = nullptr;
    animation->startValue_ = startValue;
    animation->endValue_ = byValue;
    auto result = animation->CreateRenderAnimation();
    EXPECT_EQ(result, nullptr);
    GTEST_LOG_(INFO) << "RSSpringAnimationTest InterpolatingSpringCreateRenderAnimationNull001 end";
}

/**
 * @tc.name: InterpolatingSpringCreateRenderAnimationStartValueNull001
 * @tc.desc: Verify CreateRenderAnimation returns nullptr when only startValue is null
 * @tc.type:FUNC
 */
HWTEST_F(RSSpringAnimationTest, InterpolatingSpringCreateRenderAnimationStartValueNull001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSSpringAnimationTest InterpolatingSpringCreateRenderAnimationStartValueNull001 start";
    auto property = std::make_shared<RSAnimatableProperty<float>>(0.0f);
    auto startValue = std::make_shared<RSAnimatableProperty<float>>(0.0f);
    auto byValue = std::make_shared<RSAnimatableProperty<float>>(1.0f);
    auto animation = std::make_shared<RSInterpolatingSpringAnimation>(nullptr, property, byValue);
    animation->originValue_ = property;
    animation->startValue_ = nullptr;
    animation->endValue_ = byValue;
    auto result = animation->CreateRenderAnimation();
    EXPECT_EQ(result, nullptr);
    GTEST_LOG_(INFO) << "RSSpringAnimationTest InterpolatingSpringCreateRenderAnimationStartValueNull001 end";
}

/**
 * @tc.name: InterpolatingSpringCreateRenderAnimationEndValueNull001
 * @tc.desc: Verify CreateRenderAnimation returns nullptr when only endValue is null
 * @tc.type:FUNC
 */
HWTEST_F(RSSpringAnimationTest, InterpolatingSpringCreateRenderAnimationEndValueNull001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSSpringAnimationTest InterpolatingSpringCreateRenderAnimationEndValueNull001 start";
    auto property = std::make_shared<RSAnimatableProperty<float>>(0.0f);
    auto startValue = std::make_shared<RSAnimatableProperty<float>>(0.0f);
    auto byValue = std::make_shared<RSAnimatableProperty<float>>(1.0f);
    auto animation = std::make_shared<RSInterpolatingSpringAnimation>(nullptr, property, byValue);
    animation->originValue_ = property;
    animation->startValue_ = startValue;
    animation->endValue_ = nullptr;
    auto result = animation->CreateRenderAnimation();
    EXPECT_EQ(result, nullptr);
    GTEST_LOG_(INFO) << "RSSpringAnimationTest InterpolatingSpringCreateRenderAnimationEndValueNull001 end";
}

/**
 * @tc.name: InterpolatingSpringCreateRenderAnimationAllNonNull001
 * @tc.desc: Verify CreateRenderAnimation returns valid animation when all values are non-null
 * @tc.type:FUNC
 */
HWTEST_F(RSSpringAnimationTest, InterpolatingSpringCreateRenderAnimationAllNonNull001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSSpringAnimationTest InterpolatingSpringCreateRenderAnimationAllNonNull001 start";
    auto property = std::make_shared<RSAnimatableProperty<float>>(0.0f);
    auto startValue = std::make_shared<RSAnimatableProperty<float>>(0.0f);
    auto byValue = std::make_shared<RSAnimatableProperty<float>>(1.0f);
    auto animation = std::make_shared<RSInterpolatingSpringAnimation>(nullptr, property, byValue);
    animation->originValue_ = property;
    animation->startValue_ = startValue;
    animation->endValue_ = byValue;
    auto result = animation->CreateRenderAnimation();
    EXPECT_NE(result, nullptr);
    GTEST_LOG_(INFO) << "RSSpringAnimationTest InterpolatingSpringCreateRenderAnimationAllNonNull001 end";
}

/**
 * @tc.name: InterpolatingSpringOnStartAnimationNull001
 * @tc.desc: Verify RSInterpolatingSpringAnimation OnStart handles CreateRenderAnimation returning nullptr
 * @tc.type:FUNC
 */
HWTEST_F(RSSpringAnimationTest, InterpolatingSpringOnStartAnimationNull001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSSpringAnimationTest InterpolatingSpringOnStartAnimationNull001 start";
    auto property = std::make_shared<RSAnimatableProperty<float>>(0.0f);
    auto byValue = std::make_shared<RSAnimatableProperty<float>>(1.0f);
    auto animation = std::make_shared<RSInterpolatingSpringAnimation>(nullptr, property, byValue);
    animation->property_ = nullptr;
    animation->originValue_ = nullptr;
    animation->startValue_ = nullptr;
    animation->endValue_ = nullptr;
    animation->OnStart();
    EXPECT_FALSE(animation->IsRunning());
    GTEST_LOG_(INFO) << "RSSpringAnimationTest InterpolatingSpringOnStartAnimationNull001 end";
}

/**
 * @tc.name: InterpolatingSpringOnStartPropertyNull001
 * @tc.desc: Verify RSInterpolatingSpringAnimation OnStart handles property_ null in isCustom_ branch
 * @tc.type:FUNC
 */
HWTEST_F(RSSpringAnimationTest, InterpolatingSpringOnStartPropertyNull001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSSpringAnimationTest InterpolatingSpringOnStartPropertyNull001 start";
    auto property = std::make_shared<RSAnimatableProperty<float>>(0.0f);
    auto startValue = std::make_shared<RSAnimatableProperty<float>>(0.0f);
    auto endValue = std::make_shared<RSAnimatableProperty<float>>(1.0f);
    auto animation = std::make_shared<RSInterpolatingSpringAnimation>(
        nullptr, property, startValue, endValue);
    animation->isCustom_ = true;
    animation->property_ = nullptr;
    animation->originValue_ = property;
    animation->startValue_ = startValue;
    animation->endValue_ = endValue;
    animation->OnStart();
    EXPECT_FALSE(animation->IsRunning());
    GTEST_LOG_(INFO) << "RSSpringAnimationTest InterpolatingSpringOnStartPropertyNull001 end";
}

/**
 * @tc.name: InterpolatingSpringRebuildInRenderAnimationNull001
 * @tc.desc: Verify RSInterpolatingSpringAnimation RebuildInRender handles CreateRenderAnimation returning nullptr
 * @tc.type:FUNC
 */
HWTEST_F(RSSpringAnimationTest, InterpolatingSpringRebuildInRenderAnimationNull001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSSpringAnimationTest InterpolatingSpringRebuildInRenderAnimationNull001 start";
    auto property = std::make_shared<RSAnimatableProperty<float>>(0.0f);
    auto byValue = std::make_shared<RSAnimatableProperty<float>>(1.0f);
    auto animation = std::make_shared<RSInterpolatingSpringAnimation>(nullptr, property, byValue);
    animation->originValue_ = nullptr;
    animation->RebuildInRender();
    EXPECT_FALSE(animation->IsRunning());
    GTEST_LOG_(INFO) << "RSSpringAnimationTest InterpolatingSpringRebuildInRenderAnimationNull001 end";
}

/**
 * @tc.name: InterpolatingSpringRebuildInRenderAnimationNullWithTarget001
 * @tc.desc: Verify RebuildInRender handles animation nullptr with valid target
 * @tc.type:FUNC
 */
HWTEST_F(RSSpringAnimationTest, InterpolatingSpringRebuildInRenderAnimationNullWithTarget001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSSpringAnimationTest InterpolatingSpringRebuildInRenderAnimationNullWithTarget001 start";
    auto property = std::make_shared<RSAnimatableProperty<float>>(0.0f);
    auto byValue = std::make_shared<RSAnimatableProperty<float>>(1.0f);
    OHOS::sptr<OHOS::IRemoteObject> connectToRenderRemote;
    auto rsUIContext = std::make_shared<RSUIContext>(0, connectToRenderRemote);
    rsUIContext->SetUITaskRunner([](const std::function<void()>& task, uint32_t delay) { task(); });
    auto canvasNode = RSCanvasNode::Create(false, false, rsUIContext);
    auto animation = std::make_shared<RSInterpolatingSpringAnimation>(rsUIContext, property, byValue);
    animation->target_ = canvasNode;
    animation->originValue_ = nullptr;
    animation->SetRebuildParam({0.5f, false});
    animation->RebuildInRender();
    EXPECT_FLOAT_EQ(animation->GetRebuildParam().fraction, 0.5f);
    EXPECT_FALSE(animation->IsRunning());
    GTEST_LOG_(INFO) << "RSSpringAnimationTest InterpolatingSpringRebuildInRenderAnimationNullWithTarget001 end";
}

/**
 * @tc.name: SpringOnStartPropertyNull001
 * @tc.desc: Verify RSSpringAnimation OnStart handles property_ null in isCustom_ branch
 * @tc.type:FUNC
 */
HWTEST_F(RSSpringAnimationTest, SpringOnStartPropertyNull001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSSpringAnimationTest SpringOnStartPropertyNull001 start";
    auto property = std::make_shared<RSAnimatableProperty<float>>(0.0f);
    auto startValue = std::make_shared<RSAnimatableProperty<float>>(0.0f);
    auto endValue = std::make_shared<RSAnimatableProperty<float>>(1.0f);
    OHOS::sptr<OHOS::IRemoteObject> connectToRenderRemote;
    auto rsUIContext = std::make_shared<RSUIContext>(0, connectToRenderRemote);
    rsUIContext->SetUITaskRunner([](const std::function<void()>& task, uint32_t delay) { task(); });
    auto springAnimation = std::make_shared<RSSpringAnimation>(rsUIContext, property, startValue, endValue);
    springAnimation->isCustom_ = true;
    springAnimation->property_ = nullptr;
    springAnimation->originValue_ = property;
    springAnimation->startValue_ = startValue;
    springAnimation->endValue_ = endValue;
    springAnimation->OnStart();
    EXPECT_FALSE(springAnimation->IsRunning());
    GTEST_LOG_(INFO) << "RSSpringAnimationTest SpringOnStartPropertyNull001 end";
}

} // namespace Rosen
} // namespace OHOS
