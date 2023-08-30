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
#include "animation/rs_render_path_animation.h"

#include "animation/rs_value_estimator.h"
#include "pipeline/rs_canvas_render_node.h"
#include "render/rs_path.h"
using namespace testing;
using namespace testing::ext;

namespace OHOS::Rosen {
class RSRenderPathAnimationTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;

    static constexpr uint64_t ANIMATION_ID = 1001;
    static constexpr uint64_t PROPERTY_ID = 1000;
    const Vector2f PATH_ANIMATION_DEFAULT_VALUE = Vector2f(0.f, 0.f);
    const Vector2f PATH_ANIMATION_START_VALUE = Vector2f(0.f, 0.f);
    const Vector2f PATH_ANIMATION_END_VALUE = Vector2f(500.f, 500.f);
    const std::string ANIMATION_PATH = "L350 0 L150 100";
};

void RSRenderPathAnimationTest::SetUpTestCase() {}
void RSRenderPathAnimationTest::TearDownTestCase() {}
void RSRenderPathAnimationTest::SetUp() {}
void RSRenderPathAnimationTest::TearDown() {}


/**
 * @tc.name: SetBeginFractionTest
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSRenderPathAnimationTest, SetBeginFractionTest, TestSize.Level1)
{
    auto originPosition = std::make_shared<RSRenderAnimatableProperty<Vector2f>>(PATH_ANIMATION_DEFAULT_VALUE);
    auto startPosition = std::make_shared<RSRenderAnimatableProperty<Vector2f>>(PATH_ANIMATION_START_VALUE);
    auto endPosition = std::make_shared<RSRenderAnimatableProperty<Vector2f>>(PATH_ANIMATION_END_VALUE);
    float originRotation = 1.0f;
    auto animationPath = RSPath::CreateRSPath(ANIMATION_PATH);
    auto renderPathAnimation = std::make_shared<RSRenderPathAnimation>(
        ANIMATION_ID, PROPERTY_ID, originPosition, startPosition, endPosition, originRotation, animationPath);

    renderPathAnimation->SetBeginFraction(0.0f);
    renderPathAnimation->SetEndFraction(10.0f);
    ASSERT_FALSE(renderPathAnimation->GetBeginFraction() < 0.0f);
    renderPathAnimation->SetBeginFraction(-1.0f);
    renderPathAnimation->SetEndFraction(-1.0f);
}

/**
 * @tc.name: UnmarshallingTest
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSRenderPathAnimationTest, UnmarshallingTest, TestSize.Level1)
{
    auto originPosition = std::make_shared<RSRenderAnimatableProperty<Vector2f>>(PATH_ANIMATION_DEFAULT_VALUE);
    auto startPosition = std::make_shared<RSRenderAnimatableProperty<Vector2f>>(PATH_ANIMATION_START_VALUE);
    auto endPosition = std::make_shared<RSRenderAnimatableProperty<Vector2f>>(PATH_ANIMATION_END_VALUE);
    float originRotation = 1.0f;
    auto animationPath = RSPath::CreateRSPath(ANIMATION_PATH);
    auto renderPathAnimation = std::make_shared<RSRenderPathAnimation>(
        ANIMATION_ID, PROPERTY_ID, originPosition, startPosition, endPosition, originRotation, animationPath);
    Parcel parcel;
    auto res = renderPathAnimation->Marshalling(parcel);
    ASSERT_FALSE(res);
}
} // namespace OHOS::Rosen