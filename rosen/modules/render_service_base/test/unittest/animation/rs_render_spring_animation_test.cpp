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

#include "animation/rs_render_spring_animation.h"
#include "modifier/rs_render_property.h"
#include "pipeline/rs_canvas_render_node.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
class RSRenderSpringAnimationTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;

    static constexpr uint64_t ANIMATION_ID = 12345;
    static constexpr uint64_t PROPERTY_ID = 54321;
    static constexpr uint64_t PROPERTY_ID_2 = 54322;
};

void RSRenderSpringAnimationTest::SetUpTestCase() {}
void RSRenderSpringAnimationTest::TearDownTestCase() {}
void RSRenderSpringAnimationTest::SetUp() {}
void RSRenderSpringAnimationTest::TearDown() {}

/**
 * @tc.name: Marshalling001
 * @tc.desc: Verify the Marshalling
 * @tc.type:FUNC
 */
HWTEST_F(RSRenderSpringAnimationTest, Marshalling001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSRenderSpringAnimationTest Marshalling001 start";
    auto property = std::make_shared<RSRenderAnimatableProperty<float>>(0.0f);
    auto property1 = std::make_shared<RSRenderAnimatableProperty<float>>(0.0f);
    auto property2 = std::make_shared<RSRenderAnimatableProperty<float>>(1.0f);

    auto renderSpringAnimation = std::make_shared<RSRenderSpringAnimation>(
        ANIMATION_ID, PROPERTY_ID, property, property1, property2);
    auto renderNode = std::make_shared<RSCanvasRenderNode>(ANIMATION_ID);

    EXPECT_TRUE(renderSpringAnimation != nullptr);
    Parcel parcel;
    renderSpringAnimation->Marshalling(parcel);
    renderSpringAnimation->Attach(renderNode.get());
    renderSpringAnimation->Start();
    EXPECT_TRUE(renderSpringAnimation->IsRunning());
    GTEST_LOG_(INFO) << "RSRenderSpringAnimationTest Marshalling001 end";
}

/**
 * @tc.name: Unmarshalling001
 * @tc.desc: Verify the Unmarshalling
 * @tc.type:FUNC
 */
HWTEST_F(RSRenderSpringAnimationTest, Unmarshalling001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSRenderSpringAnimationTest Unmarshalling001 start";
    auto property = std::make_shared<RSRenderAnimatableProperty<float>>(0.0f);
    auto property1 = std::make_shared<RSRenderAnimatableProperty<float>>(0.0f);
    auto property2 = std::make_shared<RSRenderAnimatableProperty<float>>(1.0f);

    auto renderSpringAnimation = std::make_shared<RSRenderSpringAnimation>(
        ANIMATION_ID, PROPERTY_ID, property, property1, property2);
    auto renderNode = std::make_shared<RSCanvasRenderNode>(ANIMATION_ID);

    Parcel parcel;
    renderSpringAnimation->Marshalling(parcel);
    RSRenderSpringAnimation::Unmarshalling(parcel);
    EXPECT_TRUE(renderSpringAnimation != nullptr);
    GTEST_LOG_(INFO) << "RSRenderSpringAnimationTest Unmarshalling001 end";
}

/**
 * @tc.name: SetFraction001
 * @tc.desc: Verify the SetFraction
 * @tc.type:FUNC
 */
HWTEST_F(RSRenderSpringAnimationTest, SetFraction001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSRenderSpringAnimationTest SetFraction001 start";
    auto property = std::make_shared<RSRenderAnimatableProperty<float>>(0.0f);
    auto property1 = std::make_shared<RSRenderAnimatableProperty<float>>(0.0f);
    auto property2 = std::make_shared<RSRenderAnimatableProperty<float>>(1.0f);

    auto renderSpringAnimation = std::make_shared<RSRenderSpringAnimation>(
        ANIMATION_ID, PROPERTY_ID, property, property1, property2);
    auto renderNode = std::make_shared<RSCanvasRenderNode>(ANIMATION_ID);

    EXPECT_TRUE(renderSpringAnimation != nullptr);
    renderSpringAnimation->Attach(renderNode.get());
    renderSpringAnimation->Start();
    EXPECT_TRUE(renderSpringAnimation->IsRunning());
    renderSpringAnimation->SetFraction(0.1f);
    renderSpringAnimation->Pause();
    EXPECT_TRUE(renderSpringAnimation->IsPaused());
    renderSpringAnimation->SetFraction(0.1f);
    GTEST_LOG_(INFO) << "RSRenderSpringAnimationTest SetFraction001 end";
}

/**
 * @tc.name: Attach001
 * @tc.desc: Verify the Attach
 * @tc.type:FUNC
 */
HWTEST_F(RSRenderSpringAnimationTest, Attach001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSRenderSpringAnimationTest Attach001 start";
    auto property = std::make_shared<RSRenderAnimatableProperty<float>>(0.0f);
    auto property1 = std::make_shared<RSRenderAnimatableProperty<float>>(0.0f);
    auto property2 = std::make_shared<RSRenderAnimatableProperty<float>>(1.0f);

    auto renderSpringAnimation = std::make_shared<RSRenderSpringAnimation>(
        ANIMATION_ID, PROPERTY_ID, property, property1, property2);

    EXPECT_TRUE(renderSpringAnimation != nullptr);
    renderSpringAnimation->Attach(nullptr);
    renderSpringAnimation->Start();
    EXPECT_TRUE(renderSpringAnimation->IsRunning());
    GTEST_LOG_(INFO) << "RSRenderSpringAnimationTest Attach001 end";
}

/**
 * @tc.name: Attach002
 * @tc.desc: Verify the Attach
 * @tc.type:FUNC
 */
HWTEST_F(RSRenderSpringAnimationTest, Attach002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSRenderSpringAnimationTest Attach002 start";
    auto property1 = std::make_shared<RSRenderAnimatableProperty<float>>(0.0f);
    auto property2 = std::make_shared<RSRenderAnimatableProperty<float>>(0.0f);
    auto startProperty = std::make_shared<RSRenderAnimatableProperty<float>>(0.0f);
    auto endProperty = std::make_shared<RSRenderAnimatableProperty<float>>(1.0f);

    auto renderSpringAnimation1 = std::make_shared<RSRenderSpringAnimation>(
        ANIMATION_ID, PROPERTY_ID, property1, startProperty, endProperty);
    auto renderSpringAnimation2 = std::make_shared<RSRenderSpringAnimation>(
        ANIMATION_ID, PROPERTY_ID_2, property2, endProperty, startProperty);
    auto renderNode = std::make_shared<RSCanvasRenderNode>(ANIMATION_ID);

    EXPECT_TRUE(renderSpringAnimation1 != nullptr);
    renderSpringAnimation1->Attach(renderNode.get());
    renderSpringAnimation1->Start();
    EXPECT_TRUE(renderSpringAnimation1->IsRunning());
    EXPECT_TRUE(renderSpringAnimation2 != nullptr);
    renderSpringAnimation2->Attach(renderNode.get());
    renderSpringAnimation2->Start();
    EXPECT_TRUE(renderSpringAnimation2->IsRunning());
    GTEST_LOG_(INFO) << "RSRenderSpringAnimationTest Attach002 end";
}
} // namespace Rosen
} // namespace OHOS