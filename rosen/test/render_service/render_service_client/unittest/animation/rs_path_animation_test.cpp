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
#include "animation/rs_path_animation.h"
#include <unistd.h>
#ifdef ROSEN_OHOS
#include "hisysevent.h"
#include "sandbox_utils.h"
#endif
using namespace testing;
using namespace testing::ext;

namespace OHOS::Rosen {
class RSPathAnimationTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;

    static constexpr uint64_t ANIMATION_ID = 12345;
    static constexpr uint64_t PROPERTY_ID = 54321;
    const Vector2f PATH_ANIMATION_DEFAULT_VALUE = Vector2f(0.f, 0.f);
    const Vector2f PATH_ANIMATION_START_VALUE = Vector2f(0.f, 0.f);
    const Vector2f PATH_ANIMATION_END_VALUE = Vector2f(500.f, 500.f);
    const Vector3f PATH_ANIMATION_DEFAULT_3F_VALUE = Vector3f(0.f, 0.f, 0.f);
    const Vector3f PATH_ANIMATION_START_3F_VALUE = Vector3f(0.f, 0.f, 0.f);
    const Vector3f PATH_ANIMATION_END_3F_VALUE = Vector3f(500.f, 500.f, 500.f);
    const Vector4f PATH_ANIMATION_DEFAULT_4F_VALUE = Vector4f(0.f, 0.f, 0.f, 0.f);
    const Vector4f PATH_ANIMATION_START_4F_VALUE = Vector4f(0.f, 0.f, 0.f, 0.f);
    const Vector4f PATH_ANIMATION_END_4F_VALUE = Vector4f(500.f, 500.f, 500.f, 500.f);
    const std::string ANIMATION_PATH = "L350 0 L150 100";
};

void RSPathAnimationTest::SetUpTestCase() {}
void RSPathAnimationTest::TearDownTestCase() {}
void RSPathAnimationTest::SetUp() {}
void RSPathAnimationTest::TearDown() {}

/**
 * @tc.name: SetEndFractionTest
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSPathAnimationTest, SetEndFractionTest, Level1)
{
    bool res = true;
    auto prop = nullptr;
    auto animationPath = nullptr;
    float frac = 1.0f;
    RSPathAnimation rsPathAnimation(prop, animationPath);
    rsPathAnimation.SetEndFraction(frac);
    ASSERT_TRUE(res);
}

/**
 * @tc.name: SetPathNeedAddOriginTest
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSPathAnimationTest, SetPathNeedAddOriginTest, Level1)
{
    bool res = true;
    bool needAddOrigin = true;
    auto prop = nullptr;
    auto animationPath = nullptr;
    RSPathAnimation rsPathAnimation(prop, animationPath);
    rsPathAnimation.SetPathNeedAddOrigin(needAddOrigin);
    ASSERT_TRUE(res);
}

/**
 * @tc.name: InitInterpolationValueVector2fTest01
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSPathAnimationTest, InitInterpolationValueVector2fTest01, Level1)
{
    auto property = std::make_shared<RSAnimatableProperty<Vector2f>>(PATH_ANIMATION_DEFAULT_VALUE);
    auto property1 = std::make_shared<RSAnimatableProperty<Vector2f>>(PATH_ANIMATION_START_VALUE);
    auto path = RSPath::CreateRSPath(ANIMATION_PATH);

    auto rsPathAnimation = std::make_shared<RSPathAnimation>(property, path);
    rsPathAnimation->isNeedPath_ = true;
    rsPathAnimation->startValue_ = property1;
    rsPathAnimation->InitInterpolationValue();
    EXPECT_EQ(rsPathAnimation->byValue_, nullptr);
}

/**
 * @tc.name: InitInterpolationValueVector2fTest02
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSPathAnimationTest, InitInterpolationValueVector2fTest02, Level1)
{
    auto property = std::make_shared<RSAnimatableProperty<Vector2f>>(PATH_ANIMATION_DEFAULT_VALUE);
    auto property1 = std::make_shared<RSAnimatableProperty<Vector2f>>(PATH_ANIMATION_START_VALUE);
    auto property2 = std::make_shared<RSAnimatableProperty<Vector2f>>(PATH_ANIMATION_END_VALUE);
    auto path = RSPath::CreateRSPath(ANIMATION_PATH);

    auto rsPathAnimation = std::make_shared<RSPathAnimation>(property, path);
    rsPathAnimation->isNeedPath_ = true;
    rsPathAnimation->startValue_ = property1;
    rsPathAnimation->endValue_ = property2;
    rsPathAnimation->InitInterpolationValue();
    EXPECT_NE(rsPathAnimation->byValue_, nullptr);
}

/**
 * @tc.name: InitInterpolationValueVector3fTest01
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSPathAnimationTest, InitInterpolationValueVector3fTest01, Level1)
{
    auto property = std::make_shared<RSAnimatableProperty<Vector3f>>(PATH_ANIMATION_DEFAULT_3F_VALUE);
    auto property1 = std::make_shared<RSAnimatableProperty<Vector3f>>(PATH_ANIMATION_START_3F_VALUE);
    auto property2 = std::make_shared<RSAnimatableProperty<Vector3f>>(PATH_ANIMATION_END_3F_VALUE);
    auto path = RSPath::CreateRSPath(ANIMATION_PATH);

    auto rsPathAnimation = std::make_shared<RSPathAnimation>(property, path);
    rsPathAnimation->isNeedPath_ = true;
    rsPathAnimation->startValue_ = property1;
    rsPathAnimation->endValue_ = property2;
    rsPathAnimation->InitInterpolationValue();
    EXPECT_NE(rsPathAnimation->byValue_, nullptr);
}

/**
 * @tc.name: InitInterpolationValueVector4fTest01
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSPathAnimationTest, InitInterpolationValueVector4fTest01, Level1)
{
    auto property = std::make_shared<RSAnimatableProperty<Vector4f>>(PATH_ANIMATION_DEFAULT_4F_VALUE);
    auto property1 = std::make_shared<RSAnimatableProperty<Vector4f>>(PATH_ANIMATION_START_4F_VALUE);
    auto path = RSPath::CreateRSPath(ANIMATION_PATH);

    auto rsPathAnimation = std::make_shared<RSPathAnimation>(property, path);
    rsPathAnimation->isNeedPath_ = true;
    rsPathAnimation->startValue_ = property1;
    rsPathAnimation->InitInterpolationValue();
    EXPECT_EQ(rsPathAnimation->byValue_, nullptr);
}

/**
 * @tc.name: InitInterpolationValueVector4fTest02
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSPathAnimationTest, InitInterpolationValueVector4fTest02, Level1)
{
    auto property = std::make_shared<RSAnimatableProperty<Vector4f>>(PATH_ANIMATION_DEFAULT_4F_VALUE);
    auto property1 = std::make_shared<RSAnimatableProperty<Vector4f>>(PATH_ANIMATION_START_4F_VALUE);
    auto property2 = std::make_shared<RSAnimatableProperty<Vector4f>>(PATH_ANIMATION_END_4F_VALUE);
    auto path = RSPath::CreateRSPath(ANIMATION_PATH);

    auto rsPathAnimation = std::make_shared<RSPathAnimation>(property, path);
    rsPathAnimation->isNeedPath_ = true;
    rsPathAnimation->startValue_ = property1;
    rsPathAnimation->endValue_ = property2;
    rsPathAnimation->InitInterpolationValue();
    EXPECT_NE(rsPathAnimation->byValue_, nullptr);
}

/**
 * @tc.name: PreProcessPathVector2fTest
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSPathAnimationTest, PreProcessPathVector2fTest, Level1)
{
    auto property = std::make_shared<RSAnimatableProperty<Vector2f>>(PATH_ANIMATION_DEFAULT_VALUE);
    auto property1 = std::make_shared<RSAnimatableProperty<Vector2f>>(PATH_ANIMATION_START_VALUE);
    auto property2 = std::make_shared<RSAnimatableProperty<Vector2f>>(PATH_ANIMATION_END_VALUE);
    auto path = RSPath::CreateRSPath(ANIMATION_PATH);

    auto rsPathAnimation = std::make_shared<RSPathAnimation>(property, path);
    auto ret = rsPathAnimation->PreProcessPath(ANIMATION_PATH, property1, nullptr);
    EXPECT_EQ(ret, nullptr);
    ret = rsPathAnimation->PreProcessPath(ANIMATION_PATH, property1, property2);
    EXPECT_NE(ret, nullptr);
}

/**
 * @tc.name: PreProcessPathVector3fTest
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSPathAnimationTest, PreProcessPathVector3fTest, Level1)
{
    auto property = std::make_shared<RSAnimatableProperty<Vector3f>>(PATH_ANIMATION_DEFAULT_3F_VALUE);
    auto property1 = std::make_shared<RSAnimatableProperty<Vector3f>>(PATH_ANIMATION_START_3F_VALUE);
    auto property2 = std::make_shared<RSAnimatableProperty<Vector3f>>(PATH_ANIMATION_END_3F_VALUE);
    auto path = RSPath::CreateRSPath(ANIMATION_PATH);

    auto rsPathAnimation = std::make_shared<RSPathAnimation>(property, path);
    auto ret = rsPathAnimation->PreProcessPath(ANIMATION_PATH, property1, nullptr);
    EXPECT_EQ(ret, nullptr);
    ret = rsPathAnimation->PreProcessPath(ANIMATION_PATH, property1, property2);
    EXPECT_EQ(ret, nullptr);
}

/**
 * @tc.name: PreProcessPathVector4fTest
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSPathAnimationTest, PreProcessPathVector4fTest, Level1)
{
    auto property = std::make_shared<RSAnimatableProperty<Vector4f>>(PATH_ANIMATION_DEFAULT_4F_VALUE);
    auto property1 = std::make_shared<RSAnimatableProperty<Vector4f>>(PATH_ANIMATION_START_4F_VALUE);
    auto property2 = std::make_shared<RSAnimatableProperty<Vector4f>>(PATH_ANIMATION_END_4F_VALUE);
    auto path = RSPath::CreateRSPath(ANIMATION_PATH);

    auto rsPathAnimation = std::make_shared<RSPathAnimation>(property, path);
    auto ret = rsPathAnimation->PreProcessPath(ANIMATION_PATH, property1, nullptr);
    EXPECT_EQ(ret, nullptr);
    ret = rsPathAnimation->PreProcessPath(ANIMATION_PATH, property1, property2);
    EXPECT_NE(ret, nullptr);
}

/**
 * @tc.name: InitNeedPathVector4fTest
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSPathAnimationTest, InitNeedPathVector4fTest, Level1)
{
    auto property = std::make_shared<RSAnimatableProperty<Vector4f>>(PATH_ANIMATION_DEFAULT_4F_VALUE);
    auto property1 = std::make_shared<RSAnimatableProperty<Vector4f>>(PATH_ANIMATION_START_4F_VALUE);
    auto property2 = std::make_shared<RSAnimatableProperty<Vector4f>>(PATH_ANIMATION_END_4F_VALUE);
    auto property3f = std::make_shared<RSAnimatableProperty<Vector3f>>(PATH_ANIMATION_DEFAULT_3F_VALUE);
    auto path = RSPath::CreateRSPath(ANIMATION_PATH);

    auto rsPathAnimation = std::make_shared<RSPathAnimation>(property, path);
    rsPathAnimation->isNeedPath_ = false;
    rsPathAnimation->InitNeedPath(property3f, nullptr);
    EXPECT_EQ(rsPathAnimation->isNeedPath_, false);
    rsPathAnimation->InitNeedPath(property1, nullptr);
    EXPECT_EQ(rsPathAnimation->isNeedPath_, false);
    rsPathAnimation->InitNeedPath(property1, property2);
    EXPECT_EQ(rsPathAnimation->isNeedPath_, property1 != property2);
}
}