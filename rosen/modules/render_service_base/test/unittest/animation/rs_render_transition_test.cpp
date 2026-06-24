/*
 * Copyright (c) 2023 Huawei Device Co., Ltd.
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

#include "animation/rs_render_animation.h"
#include "animation/rs_render_transition.h"
#include "pipeline/rs_canvas_render_node.h"
#include "transaction/rs_marshalling_helper.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
class RSRenderTransitionTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;

    static constexpr AnimationId ANIMATION_ID = 12345;
};

void RSRenderTransitionTest::SetUpTestCase() {}
void RSRenderTransitionTest::TearDownTestCase() {}
void RSRenderTransitionTest::SetUp() {}
void RSRenderTransitionTest::TearDown() {}

class RSRenderTransitionMock : public RSRenderTransition {
public:
    explicit RSRenderTransitionMock(
        AnimationId id, const std::vector<std::shared_ptr<RSRenderTransitionEffect>>& effects, bool isTransitionIn)
        : RSRenderTransition(id, effects, isTransitionIn)
    {}
    ~RSRenderTransitionMock() = default;
    bool ParseParam(Parcel& parcel) override
    {
        return RSRenderTransition::ParseParam(parcel);
    }
    void OnAnimate(float fraction) override
    {
        RSRenderTransition::OnAnimate(fraction);
    }
    void OnAttach() override
    {
        RSRenderTransition::OnAttach();
    }
    void OnDetach() override
    {
        RSRenderTransition::OnDetach();
    }

    std::shared_ptr<RSInterpolator> GetInterpolator()
    {
        return RSRenderTransition::interpolator_;
    }
};

class RSRenderAnimationMock : public RSRenderAnimation {
public:
    RSRenderAnimationMock() : RSRenderAnimation() {}
    explicit RSRenderAnimationMock(AnimationId id) : RSRenderAnimation(id) {}
    ~RSRenderAnimationMock() = default;
};
/**
 * @tc.name: OnAttach001
 * @tc.desc: Verify the OnAttach
 * @tc.type:FUNC
 */
HWTEST_F(RSRenderTransitionTest, OnAttach001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSRenderTransitionTest OnAttach001 start";
    auto fadeEffect = std::make_shared<RSTransitionFade>(1.0f);
    std::vector<std::shared_ptr<RSRenderTransitionEffect>> effects;
    effects.emplace_back(fadeEffect);
    bool isTransitionIn = true;
    auto transition = std::make_shared<RSRenderTransitionMock>(ANIMATION_ID, effects, isTransitionIn);
    transition->OnAttach();
    EXPECT_TRUE(transition != nullptr);
    GTEST_LOG_(INFO) << "RSRenderTransitionTest OnAttach001 end";
}

/**
 * @tc.name: OnAttach002
 * @tc.desc: Verify the OnAttach
 * @tc.type:FUNC
 */
HWTEST_F(RSRenderTransitionTest, OnAttach002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSRenderTransitionTest OnAttach002 start";
    auto fadeEffect = std::make_shared<RSTransitionFade>(1.0f);
    std::vector<std::shared_ptr<RSRenderTransitionEffect>> effects;
    effects.emplace_back(fadeEffect);
    bool isTransitionIn = true;
    auto transition = std::make_shared<RSRenderTransitionMock>(ANIMATION_ID, effects, isTransitionIn);
    auto renderNode = std::make_shared<RSCanvasRenderNode>(ANIMATION_ID);
    transition->Attach(renderNode.get());
    transition->OnAttach();
    EXPECT_TRUE(transition != nullptr);

    bool isTransitionIn2 = false;
    auto transition2 = std::make_shared<RSRenderTransitionMock>(ANIMATION_ID, effects, isTransitionIn2);
    auto renderNode2 = std::make_shared<RSCanvasRenderNode>(ANIMATION_ID);
    transition2->Attach(renderNode2.get());
    transition2->OnAttach();
    EXPECT_TRUE(transition2 != nullptr);
    GTEST_LOG_(INFO) << "RSRenderTransitionTest OnAttach002 end";
}

/**
 * @tc.name: OnDetach001
 * @tc.desc: Verify the OnDetach
 * @tc.type:FUNC
 */
HWTEST_F(RSRenderTransitionTest, OnDetach001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSRenderTransitionTest OnDetach001 start";
    auto fadeEffect = std::make_shared<RSTransitionFade>(1.0f);
    std::vector<std::shared_ptr<RSRenderTransitionEffect>> effects;
    effects.emplace_back(fadeEffect);
    bool isTransitionIn = true;
    auto transition = std::make_shared<RSRenderTransitionMock>(ANIMATION_ID, effects, isTransitionIn);
    transition->OnDetach();
    EXPECT_TRUE(transition != nullptr);
    GTEST_LOG_(INFO) << "RSRenderTransitionTest OnDetach001 end";
}

/**
 * @tc.name: OnDetach002
 * @tc.desc: Verify the OnDetach
 * @tc.type:FUNC
 */
HWTEST_F(RSRenderTransitionTest, OnDetach002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSRenderTransitionTest OnDetach002 start";
    auto fadeEffect = std::make_shared<RSTransitionFade>(1.0f);
    std::vector<std::shared_ptr<RSRenderTransitionEffect>> effects;
    effects.emplace_back(fadeEffect);
    bool isTransitionIn = true;
    auto transition = std::make_shared<RSRenderTransitionMock>(ANIMATION_ID, effects, isTransitionIn);
    auto renderNode = std::make_shared<RSCanvasRenderNode>(ANIMATION_ID);
    transition->Attach(renderNode.get());
    transition->OnDetach();
    EXPECT_TRUE(transition != nullptr);

    bool isTransitionIn2 = false;
    auto transition2 = std::make_shared<RSRenderTransitionMock>(ANIMATION_ID, effects, isTransitionIn2);
    auto renderNode2 = std::make_shared<RSCanvasRenderNode>(ANIMATION_ID);
    transition2->Attach(renderNode2.get());
    transition2->OnDetach();
    EXPECT_TRUE(transition2 != nullptr);
    GTEST_LOG_(INFO) << "RSRenderTransitionTest OnDetach002 end";
}

/**
 * @tc.name: ParseParam001
 * @tc.desc: Verify the  ParseParam
 * @tc.type:FUNC
 */
HWTEST_F(RSRenderTransitionTest, ParseParam001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSRenderTransitionTest ParseParam001 start";

    Parcel parcel;
    std::vector<std::shared_ptr<RSRenderTransitionEffect>> effects;
    bool isTransitionIn = true;
    auto transition = std::make_shared<RSRenderTransitionMock>(ANIMATION_ID, effects, isTransitionIn);
    bool result = transition->ParseParam(parcel);
    EXPECT_TRUE(result == false);
    GTEST_LOG_(INFO) << "RSRenderTransitionTest ParseParam001 end";
}

/**
 * @tc.name: ParseParam002
 * @tc.desc: Verify the ParseParam
 * @tc.type:FUNC
 */
HWTEST_F(RSRenderTransitionTest, ParseParam002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSRenderTransitionTest ParseParam002 start";

    auto fadeEffect = std::make_shared<RSTransitionFade>(1.0f);
    std::vector<std::shared_ptr<RSRenderTransitionEffect>> effects;
    effects.emplace_back(fadeEffect);
    bool isTransitionIn = true;
    auto transition = std::make_shared<RSRenderTransitionMock>(ANIMATION_ID, effects, isTransitionIn);

    Parcel parcel;
    auto animtaion = std::make_shared<RSRenderAnimationMock>(ANIMATION_ID);
    animtaion->Marshalling(parcel);
    bool result = transition->ParseParam(parcel);
    EXPECT_TRUE(result == false);

    Parcel parcel1;
    auto animtaion1 = std::make_shared<RSRenderAnimationMock>(ANIMATION_ID);
    animtaion1->Marshalling(parcel1);
    RSMarshallingHelper::Marshalling(parcel1, effects);
    bool result1 = transition->ParseParam(parcel1);
    EXPECT_TRUE(result1 == false);

    Parcel parcel2;
    auto animtaion2 = std::make_shared<RSRenderAnimationMock>(ANIMATION_ID);
    animtaion2->Marshalling(parcel2);
    RSMarshallingHelper::Marshalling(parcel2, effects);
    RSMarshallingHelper::Marshalling(parcel2, isTransitionIn);
    bool result2 = transition->ParseParam(parcel2);
    EXPECT_TRUE(result2 == false);

    Parcel parcel3;
    auto animtaion3 = std::make_shared<RSRenderAnimationMock>(ANIMATION_ID);
    animtaion3->Marshalling(parcel3);
    RSMarshallingHelper::Marshalling(parcel3, effects);
    RSMarshallingHelper::Marshalling(parcel3, isTransitionIn);
    transition->GetInterpolator()->Marshalling(parcel3);
    bool result3 = transition->ParseParam(parcel3);
    EXPECT_TRUE(result3 == true);

    GTEST_LOG_(INFO) << "RSRenderTransitionTest ParseParam002 end";
}
} // namespace Rosen
} // namespace OHOS
