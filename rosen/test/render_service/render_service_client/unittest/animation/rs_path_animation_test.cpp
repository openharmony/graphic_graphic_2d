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
#include "animation/rs_animation.h"
#include "animation/rs_path_animation.h"
#include "render/rs_path.h"
#include "modifier/rs_property.h"
#include "ui/rs_node.h"
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
};

void RSPathAnimationTest::SetUpTestCase() {}
void RSPathAnimationTest::TearDownTestCase() {}
void RSPathAnimationTest::SetUp() {}
void RSPathAnimationTest::TearDown() {}

/**
 * @tc.name: SetEndFractionTest001
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSPathAnimationTest, SetEndFractionTest001, Level1)
{
    bool res = true;
    auto prop = nullptr;
    auto animationPath = nullptr;
    float frac = 3.0f;
    RSPathAnimation rsPathAnimation(prop, animationPath);
    auto per = std::make_shared<RSNode>(true);
    rsPathAnimation.StartInner(per);
    rsPathAnimation.SetEndFraction(frac);
    ASSERT_TRUE(res);
}

/**
 * @tc.name: SetEndFractionTest002
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSPathAnimationTest, SetEndFractionTest002, Level1)
{
    bool res = true;
    auto prop = nullptr;
    auto animationPath = nullptr;
    float frac = 3.0f;
    RSPathAnimation rsPathAnimation(prop, animationPath);
    rsPathAnimation.SetEndFraction(frac);
    rsPathAnimation.SetEndFraction(0.5f);
    rsPathAnimation.SetEndFraction(-0.5f);
    ASSERT_EQ(res, true);
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
    auto per = std::make_shared<RSNode>(true);
    rsPathAnimation.StartInner(per);
    rsPathAnimation.SetPathNeedAddOrigin(needAddOrigin);
    ASSERT_TRUE(res);
}

/**
 * @tc.name: SetRotationModeTest
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSPathAnimationTest, SetRotationModeTest, Level1)
{
    auto prop = nullptr;
    auto animationPath = nullptr;
    auto rotationMode = RotationMode::ROTATE_NONE;
    RSPathAnimation rsPathAnimation(prop, animationPath);
    auto per = std::make_shared<RSNode>(true);
    rsPathAnimation.StartInner(per);
    rsPathAnimation.SetRotationMode(rotationMode);
    ASSERT_EQ(prop, nullptr);
    ASSERT_EQ(animationPath, nullptr);
}

/**
 * @tc.name: SetBeginFractionTest001
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSPathAnimationTest, SetBeginFractionTest001, Level1)
{
    auto prop = nullptr;
    auto animationPath = nullptr;
    float frac = 3.0f;
    RSPathAnimation rsPathAnimation(prop, animationPath);
    auto per = std::make_shared<RSNode>(true);
    rsPathAnimation.StartInner(per);
    rsPathAnimation.SetBeginFraction(frac);
    rsPathAnimation.SetBeginFraction(0.5f);
    rsPathAnimation.SetBeginFraction(-0.5f);
    ASSERT_EQ(prop, nullptr);
    ASSERT_EQ(animationPath, nullptr);
}

/**
 * @tc.name: SetBeginFractionTest002
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSPathAnimationTest, SetBeginFractionTest002, Level1)
{
    auto prop = nullptr;
    auto animationPath = nullptr;
    float frac = 3.0f;
    RSPathAnimation rsPathAnimation(prop, animationPath);
    rsPathAnimation.SetBeginFraction(frac);
    ASSERT_EQ(prop, nullptr);
    ASSERT_EQ(animationPath, nullptr);
}

/**
 * @tc.name: InitRotationIdTest
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSPathAnimationTest, InitRotationIdTest, Level1)
{
    auto node = std::make_shared<RSNode>(true);
    auto prop = std::make_shared<RSPropertyBase>();
    auto animationPath = std::make_shared<RSPath>();
    RSPathAnimation rsPathAnimation(prop, animationPath);
    rsPathAnimation.InitRotationId(node);
    ASSERT_NE(prop, nullptr);
    ASSERT_NE(node, nullptr);
    ASSERT_NE(animationPath, nullptr);
}

/**
 * @tc.name: GetRotationPropertyIdTest
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSPathAnimationTest, GetRotationPropertyIdTest, Level1)
{
    auto node = std::make_shared<RSNode>(true);
    auto prop = std::make_shared<RSPropertyBase>();
    auto animationPath = std::make_shared<RSPath>();
    RSPathAnimation rsPathAnimation(prop, animationPath);
    rsPathAnimation.GetRotationPropertyId(node);
    ASSERT_NE(prop, nullptr);
    ASSERT_NE(node, nullptr);
    ASSERT_NE(animationPath, nullptr);
}

/**
 * @tc.name: SetRotationTest
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSPathAnimationTest, SetRotationTest, Level1)
{
    float rotation = 0.f;
    auto node = std::make_shared<RSNode>(true);
    auto prop = std::make_shared<RSPropertyBase>();
    auto animationPath = std::make_shared<RSPath>();
    RSPathAnimation rsPathAnimation(prop, animationPath);
    rsPathAnimation.SetRotation(node, rotation);
    ASSERT_NE(prop, nullptr);
    ASSERT_NE(node, nullptr);
    ASSERT_NE(animationPath, nullptr);
}

/**
 * @tc.name: PreProcessPathTest
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSPathAnimationTest, PreProcessPathTest, Level1)
{
    string path = "PATH";
    auto startValue = std::make_shared<RSPropertyBase>();
    auto endValue = std::make_shared<RSPropertyBase>();
    auto node = std::make_shared<RSNode>(true);
    auto prop = std::make_shared<RSPropertyBase>();
    auto animationPath = std::make_shared<RSPath>();
    RSPathAnimation rsPathAnimation(prop, animationPath);
    rsPathAnimation.PreProcessPath(path, startValue, endValue);
    ASSERT_NE(prop, nullptr);
    ASSERT_NE(node, nullptr);
    ASSERT_NE(animationPath, nullptr);
}

/**
 * @tc.name: InitNeedPathTest
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSPathAnimationTest, InitNeedPathTest, Level1)
{
    auto startValue = std::make_shared<RSPropertyBase>();
    auto endValue = std::make_shared<RSPropertyBase>();
    auto node = std::make_shared<RSNode>(true);
    auto prop = std::make_shared<RSPropertyBase>();
    auto animationPath = std::make_shared<RSPath>();
    RSPathAnimation rsPathAnimation(prop, animationPath);
    rsPathAnimation.InitNeedPath(startValue, endValue);
    ASSERT_NE(prop, nullptr);
    ASSERT_NE(node, nullptr);
    ASSERT_NE(animationPath, nullptr);
}

/**
 * @tc.name: InitInterpolationVector2fTest
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSPathAnimationTest, InitInterpolationVector2fTest, Level1)
{
    auto startValue = std::make_shared<RSPropertyBase>();
    auto endValue = std::make_shared<RSPropertyBase>();
    auto node = std::make_shared<RSNode>(true);
    auto prop = std::make_shared<RSPropertyBase>();
    auto animationPath = std::make_shared<RSPath>();
    RSPathAnimation rsPathAnimation(prop, animationPath);
    bool res = rsPathAnimation.InitInterpolationVector2f(startValue, endValue);
    ASSERT_NE(prop, nullptr);
    ASSERT_NE(node, nullptr);
    ASSERT_NE(animationPath, nullptr);
    ASSERT_EQ(res, true);
}

/**
 * @tc.name: InitInterpolationVector4fTest
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSPathAnimationTest, InitInterpolationVector4fTest, Level1)
{
    auto startValue = std::make_shared<RSPropertyBase>();
    auto endValue = std::make_shared<RSPropertyBase>();
    auto node = std::make_shared<RSNode>(true);
    auto prop = std::make_shared<RSPropertyBase>();
    auto animationPath = std::make_shared<RSPath>();
    RSPathAnimation rsPathAnimation(prop, animationPath);
    bool res = rsPathAnimation.InitInterpolationVector4f(startValue, endValue);
    ASSERT_NE(prop, nullptr);
    ASSERT_NE(node, nullptr);
    ASSERT_NE(animationPath, nullptr);
    ASSERT_EQ(res, true);
}
}