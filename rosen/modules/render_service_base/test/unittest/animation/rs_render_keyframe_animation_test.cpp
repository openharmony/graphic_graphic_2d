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

#include "animation/rs_render_keyframe_animation.h"
#include "animation/rs_steps_interpolator.h"
#include "modifier/rs_render_property.h"
#include "pipeline/rs_canvas_render_node.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
class RSRenderKeyframeAnimationTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;

    static constexpr uint64_t ANIMATION_ID = 12345;
    static constexpr uint64_t PROPERTY_ID = 54321;
};

void RSRenderKeyframeAnimationTest::SetUpTestCase() {}
void RSRenderKeyframeAnimationTest::TearDownTestCase() {}
void RSRenderKeyframeAnimationTest::SetUp() {}
void RSRenderKeyframeAnimationTest::TearDown() {}

class RSRenderKeyframeAnimationMock : public RSRenderKeyframeAnimation {
public:
    RSRenderKeyframeAnimationMock(
        AnimationId id, const PropertyId& propertyId, const std::shared_ptr<RSRenderPropertyBase>& originValue)
        : RSRenderKeyframeAnimation(id, propertyId, originValue)
    {}
    ~RSRenderKeyframeAnimationMock() {}
    void OnAnimate(float fraction)
    {
        RSRenderKeyframeAnimation::OnAnimate(fraction);
    }
};

/**
 * @tc.name: AddKeyframe001
 * @tc.desc: Verify the AddKeyframe
 * @tc.type:FUNC
 */
HWTEST_F(RSRenderKeyframeAnimationTest, AddKeyframe001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSRenderSpringAnimationTest AddKeyframe001 start";
    auto property = std::make_shared<RSRenderAnimatableProperty<float>>(0.0f);
    auto property1 = std::make_shared<RSRenderAnimatableProperty<float>>(0.1f);

    auto renderKeyframeAnimation = std::make_shared<RSRenderKeyframeAnimation>(
        ANIMATION_ID, PROPERTY_ID, property);
    auto interpolator = std::make_shared<RSStepsInterpolator>(0);
    auto renderNode = std::make_shared<RSCanvasRenderNode>(ANIMATION_ID);
    EXPECT_TRUE(renderKeyframeAnimation != nullptr);
    renderKeyframeAnimation->Attach(renderNode.get());
    renderKeyframeAnimation->AddKeyframe(-1.0f, property1, interpolator);
    renderKeyframeAnimation->Start();
    EXPECT_TRUE(renderKeyframeAnimation->IsRunning());
    renderKeyframeAnimation->AddKeyframe(2.0f, property1, interpolator);
    renderKeyframeAnimation->Start();
    EXPECT_TRUE(renderKeyframeAnimation->IsRunning());
    GTEST_LOG_(INFO) << "RSRenderSpringAnimationTest AddKeyframe001 end";
}

/**
 * @tc.name: AddKeyframe002
 * @tc.desc: Verify the AddKeyframe
 * @tc.type:FUNC
 */
HWTEST_F(RSRenderKeyframeAnimationTest, AddKeyframe002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSRenderSpringAnimationTest AddKeyframe002 start";
    auto property = std::make_shared<RSRenderAnimatableProperty<float>>(0.0f);
    auto property1 = std::make_shared<RSRenderAnimatableProperty<float>>(0.1f);

    auto renderKeyframeAnimation = std::make_shared<RSRenderKeyframeAnimation>(
        ANIMATION_ID, PROPERTY_ID, property);
    auto interpolator = std::make_shared<RSStepsInterpolator>(0);
    auto renderNode = std::make_shared<RSCanvasRenderNode>(ANIMATION_ID);
    renderKeyframeAnimation->AddKeyframe(0.1f, property1, interpolator);
    EXPECT_TRUE(renderKeyframeAnimation != nullptr);
    renderKeyframeAnimation->Attach(renderNode.get());
    renderKeyframeAnimation->Start();
    EXPECT_TRUE(renderKeyframeAnimation->IsRunning());
    renderKeyframeAnimation->AddKeyframe(0.2f, property1, interpolator);
    GTEST_LOG_(INFO) << "RSRenderSpringAnimationTest AddKeyframe002 end";
}

/**
 * @tc.name: AddKeyframes001
 * @tc.desc: Verify the AddKeyframes
 * @tc.type:FUNC
 */
HWTEST_F(RSRenderKeyframeAnimationTest, AddKeyframes001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSRenderSpringAnimationTest AddKeyframes001 start";
    auto property = std::make_shared<RSRenderAnimatableProperty<float>>(0.0f);
    auto property1 = std::make_shared<RSRenderAnimatableProperty<float>>(0.1f);
    auto property2 = std::make_shared<RSRenderAnimatableProperty<float>>(1.0f);
    std::vector<std::tuple<float, std::shared_ptr<RSRenderPropertyBase>, std::shared_ptr<RSInterpolator>>> keyframes;
    auto interpolator = std::make_shared<RSStepsInterpolator>(0);
    keyframes.push_back(std::make_tuple(0.1f, property1, interpolator));
    keyframes.push_back(std::make_tuple(1.0f, property2, interpolator));

    auto renderKeyframeAnimation = std::make_shared<RSRenderKeyframeAnimation>(
        ANIMATION_ID, PROPERTY_ID, property);
    auto renderNode = std::make_shared<RSCanvasRenderNode>(ANIMATION_ID);
    renderKeyframeAnimation->AddKeyframe(0.1f, property1, interpolator);
    EXPECT_TRUE(renderKeyframeAnimation != nullptr);
    renderKeyframeAnimation->Attach(renderNode.get());
    renderKeyframeAnimation->Start();
    EXPECT_TRUE(renderKeyframeAnimation->IsRunning());
    renderKeyframeAnimation->AddKeyframes(keyframes);
    GTEST_LOG_(INFO) << "RSRenderSpringAnimationTest AddKeyframes001 end";
}

/**
 * @tc.name: Marshalling001
 * @tc.desc: Verify the Marshalling
 * @tc.type:FUNC
 */
HWTEST_F(RSRenderKeyframeAnimationTest, Marshalling001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSRenderKeyframeAnimationTest Marshalling001 start";

    auto property = std::make_shared<RSRenderAnimatableProperty<float>>(0.0f);
    auto property1 = std::make_shared<RSRenderAnimatableProperty<float>>(0.1f);

    auto renderKeyframeAnimation = std::make_shared<RSRenderKeyframeAnimation>(
        ANIMATION_ID, PROPERTY_ID, property);
    auto interpolator = std::make_shared<RSStepsInterpolator>(0);
    auto renderNode = std::make_shared<RSCanvasRenderNode>(ANIMATION_ID);
    renderKeyframeAnimation->AddKeyframe(0.1f, property1, interpolator);

    Parcel parcel;
    renderKeyframeAnimation->Marshalling(parcel);
    EXPECT_TRUE(renderKeyframeAnimation != nullptr);
    renderKeyframeAnimation->Attach(renderNode.get());
    renderKeyframeAnimation->Start();
    EXPECT_TRUE(renderKeyframeAnimation->IsRunning());
    GTEST_LOG_(INFO) << "RSRenderKeyframeAnimationTest Marshalling001 end";
}

/**
 * @tc.name: Unmarshalling001
 * @tc.desc: Verify the Unmarshalling
 * @tc.type:FUNC
 */
HWTEST_F(RSRenderKeyframeAnimationTest, Unmarshalling001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSRenderKeyframeAnimationTest Unmarshalling001 start";

    auto property = std::make_shared<RSRenderAnimatableProperty<float>>(0.0f);
    auto property1 = std::make_shared<RSRenderAnimatableProperty<float>>(0.1f);

    auto renderKeyframeAnimation = std::make_shared<RSRenderKeyframeAnimation>(
        ANIMATION_ID, PROPERTY_ID, property);
    auto interpolator = std::make_shared<RSStepsInterpolator>(0);
    auto renderNode = std::make_shared<RSCanvasRenderNode>(ANIMATION_ID);
    renderKeyframeAnimation->AddKeyframe(0.1f, property1, interpolator);

    Parcel parcel;
    renderKeyframeAnimation->Marshalling(parcel);
    std::shared_ptr<RSRenderAnimation>(RSRenderKeyframeAnimation::Unmarshalling(parcel));
    EXPECT_TRUE(renderKeyframeAnimation != nullptr);
    renderKeyframeAnimation->Attach(renderNode.get());
    renderKeyframeAnimation->Start();
    EXPECT_TRUE(renderKeyframeAnimation->IsRunning());
    GTEST_LOG_(INFO) << "RSRenderKeyframeAnimationTest Unmarshalling001 end";
}

/**
 * @tc.name: OnAnimate001
 * @tc.desc: Verify the OnAnimate
 * @tc.type:FUNC
 */
HWTEST_F(RSRenderKeyframeAnimationTest, OnAnimate001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSRenderSpringAnimationTest OnAnimate001 start";
    auto property = std::make_shared<RSRenderAnimatableProperty<float>>(0.0f);
    auto property1 = std::make_shared<RSRenderAnimatableProperty<float>>(0.1f);

    auto renderKeyframeAnimationMock =
        std::make_shared<RSRenderKeyframeAnimationMock>(ANIMATION_ID, PROPERTY_ID, property);
    float fraction = 0.1f;
    renderKeyframeAnimationMock->OnAnimate(fraction);
    EXPECT_TRUE(renderKeyframeAnimationMock != nullptr);
    auto interpolator = std::make_shared<RSStepsInterpolator>(0);
    renderKeyframeAnimationMock->AddKeyframe(2.0f, property1, interpolator);
    renderKeyframeAnimationMock->OnAnimate(fraction);
    EXPECT_TRUE(renderKeyframeAnimationMock != nullptr);
    GTEST_LOG_(INFO) << "RSRenderSpringAnimationTest OnAnimate001 end";
}
} // namespace Rosen
} // namespace OHOS