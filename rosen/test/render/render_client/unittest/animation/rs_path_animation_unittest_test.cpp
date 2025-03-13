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

#include "gtest/gtest.h"
#include "animation/rs_animation.h"
#include "animation/rs_path_animation.h"
#include "render/rs_path.h"
#include "modifier/rs_property.h"
#include "ui/rs_node.h"
#include <unistd.h>
#ifdef ROSEN_OHOS
#include "base/hiviewdfx/hisysevent/interfaces/native/innerkits/hisysevent/include/hisysevent.h"
#include "sandbox_utils.h"
#endif
using namespace testing;
using namespace testing::ext;

namespace OHOS::Rosen {
class RsPathAnimationTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void RsPathAnimationTest::SetUpTestCase() {}
void RsPathAnimationTest::TearDownTestCase() {}
void RsPathAnimationTest::SetUp() {}
void RsPathAnimationTest::TearDown() {}

/**
 * @tc.name: SetEndFractionTest01
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RsPathAnimationTest, SetEndFractionTest01, Level1)
{
    auto animationPath = nullptr;
    float frac = 3.0f;
    bool res = true;
    auto prop = nullptr;
    RSPathAnimation rsPathAnimation(prop, animationPath);
    auto per = std::make_shared<RSNode>(true);
    rsPathAnimation.StartInner(per);
    rsPathAnimation.SetEndFraction(frac);
    ASSERT_EQ(res, true);
}

/**
 * @tc.name: SetEndFractionTest02
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RsPathAnimationTest, SetEndFractionTest02, Level1)
{
    auto animationPath = nullptr;
    bool res = true;
    auto prop = nullptr;
    float frac = 3.0f;
    RSPathAnimation rsPathAnimation(prop, animationPath);
    rsPathAnimation.SetEndFraction(frac);
    rsPathAnimation.SetEndFraction(0.5f);
    rsPathAnimation.SetEndFraction(-0.5f);
    ASSERT_EQ(res, true);
}

/**
 * @tc.name: SetPathNeedAddOriginTest01
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RsPathAnimationTest, SetPathNeedAddOriginTest01, Level1)
{
    auto prop = nullptr;
    auto animationPath = nullptr;
    bool res = true;
    bool needAddOrigin = true;
    RSPathAnimation rsPathAnimation(prop, animationPath);
    auto per = std::make_shared<RSNode>(true);
    rsPathAnimation.StartInner(per);
    rsPathAnimation.SetPathNeedAddOrigin(needAddOrigin);
    ASSERT_EQ(res, true);
}

/**
 * @tc.name: SetRotationModeTest01
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RsPathAnimationTest, SetRotationModeTest01, Level1)
{
    auto prop = nullptr;
    auto animationPath = nullptr;
    auto rotationMode = RotationMode::ROTATE_NONE;
    RSPathAnimation rsPathAnimation(prop, animationPath);
    auto per = std::make_shared<RSNode>(true);
    rsPathAnimation.StartInner(per);
    rsPathAnimation.SetRotationMode(rotationMode);
    ASSERT_EQ(animationPath, nullptr);
    ASSERT_EQ(prop, nullptr);
}

/**
 * @tc.name: SetBeginFractionTest01
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RsPathAnimationTest, SetBeginFractionTest01, Level1)
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
    ASSERT_EQ(animationPath, nullptr);
    ASSERT_EQ(prop, nullptr);
}

/**
 * @tc.name: SetBeginFractionTest02
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RsPathAnimationTest, SetBeginFractionTest02, Level1)
{
    auto prop = nullptr;
    auto animationPath = nullptr;
    float frac = 3.0f;
    RSPathAnimation rsPathAnimation(prop, animationPath);
    rsPathAnimation.SetBeginFraction(frac);
    ASSERT_EQ(animationPath, nullptr);
    ASSERT_EQ(prop, nullptr);
}

/**
 * @tc.name: InitRotationIdTest01
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RsPathAnimationTest, InitRotationIdTest01, Level1)
{
    auto node = std::make_shared<RSNode>(true);
    auto prop = std::make_shared<RSPropertyBase>();
    auto animationPath = std::make_shared<RSPath>();
    RSPathAnimation rsPathAnimation(prop, animationPath);
    rsPathAnimation.InitRotationId(node);
    ASSERT_NE(node, nullptr);
    ASSERT_NE(prop, nullptr);
    ASSERT_NE(animationPath, nullptr);
}

/**
 * @tc.name: GetRotationPropertyIdTest01
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RsPathAnimationTest, GetRotationPropertyIdTest01, Level1)
{
    auto node = std::make_shared<RSNode>(true);
    auto prop = std::make_shared<RSPropertyBase>();
    auto animationPath = std::make_shared<RSPath>();
    RSPathAnimation rsPathAnimation(prop, animationPath);
    rsPathAnimation.GetRotationPropertyId(node);
    ASSERT_NE(node, nullptr);
    ASSERT_NE(prop, nullptr);
    ASSERT_NE(animationPath, nullptr);
}

/**
 * @tc.name: SetRotationTest01
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RsPathAnimationTest, SetRotationTest01, Level1)
{
    float rotation = 0.f;
    auto node = std::make_shared<RSNode>(true);
    auto prop = std::make_shared<RSPropertyBase>();
    auto animationPath = std::make_shared<RSPath>();
    RSPathAnimation rsPathAnimation(prop, animationPath);
    rsPathAnimation.SetRotation(node, rotation);
    ASSERT_NE(node, nullptr);
    ASSERT_NE(prop, nullptr);
    ASSERT_NE(animationPath, nullptr);
}

/**
 * @tc.name: PreProcessPathTest01
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RsPathAnimationTest, PreProcessPathTest01, Level1)
{
    string path = "PATH";
    auto startValue = std::make_shared<RSPropertyBase>();
    auto endValue = std::make_shared<RSPropertyBase>();
    auto node = std::make_shared<RSNode>(true);
    auto prop = std::make_shared<RSPropertyBase>();
    auto animationPath = std::make_shared<RSPath>();
    RSPathAnimation rsPathAnimation(prop, animationPath);
    rsPathAnimation.PreProcessPath(path, startValue, endValue);
    ASSERT_NE(node, nullptr);
    ASSERT_NE(prop, nullptr);
    ASSERT_NE(animationPath, nullptr);
}

/**
 * @tc.name: InitNeedPathTest01
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RsPathAnimationTest, InitNeedPathTest01, Level1)
{
    auto startValue = std::make_shared<RSPropertyBase>();
    auto endValue = std::make_shared<RSPropertyBase>();
    auto node = std::make_shared<RSNode>(true);
    auto prop = std::make_shared<RSPropertyBase>();
    auto animationPath = std::make_shared<RSPath>();
    RSPathAnimation rsPathAnimation(prop, animationPath);
    rsPathAnimation.InitNeedPath(startValue, endValue);
    ASSERT_NE(node, nullptr);
    ASSERT_NE(prop, nullptr);
    ASSERT_NE(animationPath, nullptr);
}

/**
 * @tc.name: InitInterpolationVector2fTest01
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RsPathAnimationTest, InitInterpolationVector2fTest01, Level1)
{
    auto startValue = std::make_shared<RSPropertyBase>();
    auto endValue = std::make_shared<RSPropertyBase>();
    auto node = std::make_shared<RSNode>(true);
    auto prop = std::make_shared<RSPropertyBase>();
    auto animationPath = std::make_shared<RSPath>();
    RSPathAnimation rsPathAnimation(prop, animationPath);
    bool res = rsPathAnimation.InitInterpolationVector2f(startValue, endValue);
    ASSERT_NE(node, nullptr);
    ASSERT_NE(prop, nullptr);
    ASSERT_NE(animationPath, nullptr);
    ASSERT_EQ(res, true);
}

/**
 * @tc.name: InitInterpolationVector4fTest01
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RsPathAnimationTest, InitInterpolationVector4fTest01, Level1)
{
    auto startValue = std::make_shared<RSPropertyBase>();
    auto endValue = std::make_shared<RSPropertyBase>();
    auto node = std::make_shared<RSNode>(true);
    auto prop = std::make_shared<RSPropertyBase>();
    auto animationPath = std::make_shared<RSPath>();
    RSPathAnimation rsPathAnimation(prop, animationPath);
    bool res = rsPathAnimation.InitInterpolationVector4f(startValue, endValue);
    ASSERT_NE(node, nullptr);
    ASSERT_NE(prop, nullptr);
    ASSERT_NE(animationPath, nullptr);
    ASSERT_EQ(res, true);
}
}