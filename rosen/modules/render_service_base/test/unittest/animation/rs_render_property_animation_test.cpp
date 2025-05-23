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

#include "animation/rs_render_property_animation.h"
#include "modifier/rs_render_property.h"
#include "pipeline/rs_canvas_render_node.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
class RSRenderPropertyAnimationTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;

    static constexpr uint64_t ANIMATION_ID = 12345;
    static constexpr uint64_t PROPERTY_ID = 54321;
    static constexpr uint64_t PROPERTY_ID_2 = 54322;
    static constexpr uint64_t PROPERTY_ID_3 = 0;
};

void RSRenderPropertyAnimationTest::SetUpTestCase() {}
void RSRenderPropertyAnimationTest::TearDownTestCase() {}
void RSRenderPropertyAnimationTest::SetUp() {}
void RSRenderPropertyAnimationTest::TearDown() {}

/**
 * @tc.name: Marshalling001
 * @tc.desc: Verify the Marshalling
 * @tc.type:FUNC
 */
HWTEST_F(RSRenderPropertyAnimationTest, Marshalling001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSRenderPropertyAnimationTest Marshalling001 start";
    auto property = std::make_shared<RSRenderAnimatableProperty<float>>(0.0f);

    auto renderPropertyAnimation = std::make_shared<RSRenderPropertyAnimation>(
        ANIMATION_ID, PROPERTY_ID, property);
    auto renderNode = std::make_shared<RSCanvasRenderNode>(ANIMATION_ID);

    EXPECT_TRUE(renderPropertyAnimation != nullptr);
    Parcel parcel;
    renderPropertyAnimation->Marshalling(parcel);
    renderPropertyAnimation->Attach(renderNode.get());
    renderPropertyAnimation->Start();
    EXPECT_TRUE(renderPropertyAnimation->IsRunning());
    GTEST_LOG_(INFO) << "RSRenderPropertyAnimationTest Marshalling001 end";
}

/**
 * @tc.name: SetPropertyValue001
 * @tc.desc: Verify the SetPropertyValue
 * @tc.type:FUNC
 */
HWTEST_F(RSRenderPropertyAnimationTest, SetPropertyValue001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSRenderPropertyAnimationTest SetPropertyValue001 start";
    auto property = std::make_shared<RSRenderAnimatableProperty<float>>(0.0f);

    auto renderPropertyAnimation = std::make_shared<RSRenderPropertyAnimation>(
        ANIMATION_ID, PROPERTY_ID, property);
    auto renderNode = std::make_shared<RSCanvasRenderNode>(ANIMATION_ID);
    auto animationValue = std::make_shared<RSRenderAnimatableProperty<float>>(1.0f);

    EXPECT_TRUE(renderPropertyAnimation != nullptr);
    renderPropertyAnimation->SetPropertyValue(animationValue);
    renderPropertyAnimation->Attach(renderNode.get());
    renderPropertyAnimation->Start();
    EXPECT_TRUE(renderPropertyAnimation->IsRunning());
    GTEST_LOG_(INFO) << "RSRenderPropertyAnimationTest SetPropertyValue001 end";
}

/**
 * @tc.name: GetPropertyValue001
 * @tc.desc: Verify the GetPropertyValue
 * @tc.type:FUNC
 */
HWTEST_F(RSRenderPropertyAnimationTest, GetPropertyValue001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSRenderPropertyAnimationTest GetPropertyValue001 start";
    auto property = std::make_shared<RSRenderAnimatableProperty<float>>(0.0f);

    auto renderPropertyAnimation = std::make_shared<RSRenderPropertyAnimation>(
        ANIMATION_ID, PROPERTY_ID, property);
    auto animationValue = std::make_shared<RSRenderAnimatableProperty<float>>(1.0f);

    EXPECT_TRUE(renderPropertyAnimation != nullptr);
    renderPropertyAnimation->SetPropertyValue(animationValue);
    auto propertyValue = renderPropertyAnimation->GetPropertyValue();
    auto lastValue = renderPropertyAnimation->GetLastValue();
    EXPECT_NE(propertyValue, nullptr);
    EXPECT_NE(lastValue, nullptr);
    renderPropertyAnimation->property_ = nullptr;
    propertyValue = renderPropertyAnimation->GetPropertyValue();
    EXPECT_NE(propertyValue, nullptr);
    renderPropertyAnimation->lastValue_ = nullptr;
    propertyValue = renderPropertyAnimation->GetPropertyValue();
    EXPECT_EQ(propertyValue, nullptr);
    propertyValue = propertyValue + property;
    EXPECT_NE(property, nullptr);
    EXPECT_EQ(propertyValue, nullptr);
    propertyValue += property;
    EXPECT_EQ(propertyValue, nullptr);
    GTEST_LOG_(INFO) << "RSRenderPropertyAnimationTest GetPropertyValue001 end";
}

/**
 * @tc.name: SetAnimationValue001
 * @tc.desc: Verify the SetAnimationValue
 * @tc.type:FUNC
 */
HWTEST_F(RSRenderPropertyAnimationTest, SetAnimationValue001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSRenderPropertyAnimationTest SetAnimationValue001 start";
    auto property = std::make_shared<RSRenderAnimatableProperty<float>>(0.0f);

    auto renderPropertyAnimation = std::make_shared<RSRenderPropertyAnimation>(
        ANIMATION_ID, PROPERTY_ID, property);
    auto animationValue = std::make_shared<RSRenderAnimatableProperty<float>>(1.0f);

    EXPECT_TRUE(renderPropertyAnimation != nullptr);
    renderPropertyAnimation->SetAnimationValue(animationValue);
    animationValue = nullptr;
    renderPropertyAnimation->SetAnimationValue(animationValue);
    renderPropertyAnimation->Start();
    EXPECT_TRUE(renderPropertyAnimation->IsRunning());
    GTEST_LOG_(INFO) << "RSRenderPropertyAnimationTest SetAnimationValue001 end";
}

/**
 * @tc.name: GetAnimationValue001
 * @tc.desc: Verify the GetAnimationValue
 * @tc.type:FUNC
 */
HWTEST_F(RSRenderPropertyAnimationTest, GetAnimationValue001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSRenderPropertyAnimationTest GetAnimationValue001 start";
    auto property = std::make_shared<RSRenderAnimatableProperty<float>>(0.0f);

    auto renderPropertyAnimation = std::make_shared<RSRenderPropertyAnimation>(
        ANIMATION_ID, PROPERTY_ID, property);
    auto animationValue = std::make_shared<RSRenderAnimatableProperty<float>>(1.0f);

    EXPECT_TRUE(renderPropertyAnimation != nullptr);
    auto newAnimationValue = renderPropertyAnimation->GetAnimationValue(animationValue);
    EXPECT_NE(newAnimationValue, nullptr);
    animationValue = nullptr;
    newAnimationValue = renderPropertyAnimation->GetAnimationValue(animationValue);
    EXPECT_EQ(newAnimationValue, nullptr);
    GTEST_LOG_(INFO) << "RSRenderPropertyAnimationTest GetAnimationValue001 end";
}

/**
 * @tc.name: ProcessAnimateVelocityUnderAngleRotation001
 * @tc.desc: Verify the ProcessAnimateVelocityUnderAngleRotation
 * @tc.type:FUNC
 */
HWTEST_F(RSRenderPropertyAnimationTest, ProcessAnimateVelocityUnderAngleRotation001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSRenderPropertyAnimationTest ProcessAnimateVelocityUnderAngleRotation001 start";
    auto property = std::make_shared<RSRenderAnimatableProperty<float>>(0.0f);

    auto renderPropertyAnimation = std::make_shared<RSRenderPropertyAnimation>(
        ANIMATION_ID, PROPERTY_ID, property);
    auto renderPropertyAnimation2 = std::make_shared<RSRenderPropertyAnimation>(
        ANIMATION_ID, PROPERTY_ID, property);
    auto animationValue = std::make_shared<RSRenderAnimatableProperty<float>>(1.0f);
    float frameInterval = 17.0f;
    float frameInterval2 = 0.0f;

    EXPECT_TRUE(renderPropertyAnimation != nullptr);
    renderPropertyAnimation->AttachRenderProperty(animationValue);
    renderPropertyAnimation->RecordLastAnimateValue();
    renderPropertyAnimation->ProcessAnimateVelocityUnderAngleRotation(frameInterval);
    renderPropertyAnimation->Start();
    EXPECT_TRUE(renderPropertyAnimation->IsRunning());

    EXPECT_TRUE(renderPropertyAnimation2 != nullptr);
    renderPropertyAnimation2->AttachRenderProperty(animationValue);
    renderPropertyAnimation2->RecordLastAnimateValue();
    renderPropertyAnimation2->ProcessAnimateVelocityUnderAngleRotation(frameInterval2);
    renderPropertyAnimation2->Start();
    EXPECT_TRUE(renderPropertyAnimation2->IsRunning());
    GTEST_LOG_(INFO) << "RSRenderPropertyAnimationTest ProcessAnimateVelocityUnderAngleRotation001 end";
}

/**
 * @tc.name: RSRenderPropertyAnimation_Constructor001
 * @tc.desc: Verify the RSRenderPropertyAnimation_Constructor originValue is not null
 * @tc.type:FUNC
 */
HWTEST_F(RSRenderPropertyAnimationTest, RSRenderPropertyAnimation_Constructor001, TestSize.Level1)
{
    AnimationId id = 1;
    PropertyId propertyId = 2;
    auto originValue = std::make_shared<RSRenderAnimatableProperty<float>>(0);
    RSRenderPropertyAnimation animation(id, propertyId, originValue);
    EXPECT_NE(animation.GetOriginValue(), nullptr);
    EXPECT_NE(animation.GetLastValue(), nullptr);
}

/**
 * @tc.name: RSRenderPropertyAnimation_Constructor002
 * @tc.desc: Verify the RSRenderPropertyAnimation_Constructor originValue is null
 * @tc.type:FUNC
 */
HWTEST_F(RSRenderPropertyAnimationTest, RSRenderPropertyAnimation_Constructor002, TestSize.Level1)
{
    AnimationId id = 1;
    PropertyId propertyId = 2;
    RSRenderPropertyAnimation animation(id, propertyId, nullptr);
    EXPECT_NE(animation.GetOriginValue(), nullptr);
    EXPECT_NE(animation.GetLastValue(), nullptr);
}
} // namespace Rosen
} // namespace OHOS