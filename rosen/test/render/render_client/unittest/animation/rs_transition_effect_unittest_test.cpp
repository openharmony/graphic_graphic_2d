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

#include "animation/rs_transition_effect.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {

class RSTransitionEffectUnitTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void RSTransitionEffectUnitTest::SetUpTestCase() {}
void RSTransitionEffectUnitTest::TearDownTestCase() {}
void RSTransitionEffectUnitTest::SetUp() {}
void RSTransitionEffectUnitTest::TearDown() {}

/**
 * @tc.name: Translate001
 * @tc.desc: Verify the Translate
 * @tc.type:FUNC
 */
HWTEST_F(RSTransitionEffectUnitTest, Translate001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSTransitionEffectUnitTest Translate001 start";
    Vector3f translate = { 0.0f, 0.0f, 0.0f };
    Vector3f translate1 = { 0.0f, 1.0f, 0.0f };
    Vector3f translate2 = { 0.0f, 0.0f, 1.0f };
    auto transitionEffect = RSTransitionEffect::Create();
    EXPECT_TRUE(transitionEffect != nullptr);
    transitionEffect->Translate(translate);
    EXPECT_TRUE(transitionEffect != nullptr);

    auto effect1 = RSTransitionEffect::Create();
    effect1->Translate(translate1);
    EXPECT_TRUE(effect1 != nullptr);

    auto effect2 = RSTransitionEffect::Create();
    effect2->Translate(translate2);
    EXPECT_TRUE(effect2 != nullptr);

    GTEST_LOG_(INFO) << "RSTransitionEffectUnitTest Translate001 end";
}

/**
 * @tc.name: Rotate001
 * @tc.desc: Verify the Rotate
 * @tc.type:FUNC
 */
HWTEST_F(RSTransitionEffectUnitTest, Rotate001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSTransitionEffectUnitTest Rotate001 start";
    Vector4f axisAngle = { 0.0f, 0.0f, 0.0f, 0.0f };
    auto transitionEffect = RSTransitionEffect::Create();
    EXPECT_TRUE(transitionEffect != nullptr);
    transitionEffect->Rotate(axisAngle);
    EXPECT_TRUE(transitionEffect != nullptr);
    GTEST_LOG_(INFO) << "RSTransitionEffectUnitTest Rotate001 end";
}

/**
 * @tc.name: Scale001
 * @tc.desc: Verify the Scale
 * @tc.type:FUNC
 */
HWTEST_F(RSTransitionEffectUnitTest, Scale001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSTransitionEffectUnitTest Scale001 start";
    Vector3f sca = { 1.0f, 1.0f, 1.0f};
    auto transitionEffect = RSTransitionEffect::Create();
    EXPECT_TRUE(transitionEffect != nullptr);
    transitionEffect->Scale(sca);
    EXPECT_TRUE(transitionEffect != nullptr);
    GTEST_LOG_(INFO) << "RSTransitionEffectUnitTest Scale001 end";
}

/**
 * @tc.name: Custom001
 * @tc.desc: Verify the Custom
 * @tc.type:FUNC
 */
HWTEST_F(RSTransitionEffectUnitTest, Custom001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSTransitionEffectUnitTest Custom001 start";
    std::shared_ptr<RSTransitionModifier> modifier = nullptr;
    auto transitionEffect = RSTransitionEffect::Create();
    EXPECT_TRUE(transitionEffect != nullptr);
    transitionEffect->Custom(modifier);
    EXPECT_TRUE(transitionEffect != nullptr);
    GTEST_LOG_(INFO) << "RSTransitionEffectUnitTest Custom001 end";
}
} // namespace Rosen
} // namespace OHOS
