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

class RSTransitionEffectTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void RSTransitionEffectTest::SetUpTestCase() {}
void RSTransitionEffectTest::TearDownTestCase() {}
void RSTransitionEffectTest::SetUp() {}
void RSTransitionEffectTest::TearDown() {}

/**
 * @tc.name: Translate001
 * @tc.desc: Verify the Translate
 * @tc.type:FUNC
 */
HWTEST_F(RSTransitionEffectTest, Translate001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSTransitionEffectTest Translate001 start";
    Vector3f translate = { 0.0f, 0.0f, 0.0f };
    Vector3f translate1 = { 0.0f, 1.0f, 0.0f };
    Vector3f translate2 = { 0.0f, 0.0f, 1.0f };
    auto effect = RSTransitionEffect::Create();
    EXPECT_TRUE(effect != nullptr);
    effect->Translate(translate);
    EXPECT_TRUE(effect != nullptr);

    auto effect1 = RSTransitionEffect::Create();
    effect1->Translate(translate1);
    EXPECT_TRUE(effect1 != nullptr);

    auto effect2 = RSTransitionEffect::Create();
    effect2->Translate(translate2);
    EXPECT_TRUE(effect2 != nullptr);

    GTEST_LOG_(INFO) << "RSTransitionEffectTest Translate001 end";
}

/**
 * @tc.name: Rotate001
 * @tc.desc: Verify the Rotate
 * @tc.type:FUNC
 */
HWTEST_F(RSTransitionEffectTest, Rotate001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSTransitionEffectTest Rotate001 start";
    Vector4f axisAngle = { 0.0f, 0.0f, 0.0f, 0.0f };
    auto effect = RSTransitionEffect::Create();
    EXPECT_TRUE(effect != nullptr);
    effect->Rotate(axisAngle);
    EXPECT_TRUE(effect != nullptr);
    GTEST_LOG_(INFO) << "RSTransitionEffectTest Rotate001 end";
}

/**
 * @tc.name: Custom001
 * @tc.desc: Verify the Custom
 * @tc.type:FUNC
 */
HWTEST_F(RSTransitionEffectTest, Custom001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSTransitionEffectTest Custom001 start";
    std::shared_ptr<RSTransitionModifier> modifier = nullptr;
    auto effect = RSTransitionEffect::Create();
    EXPECT_TRUE(effect != nullptr);
    effect->Custom(modifier);
    EXPECT_TRUE(effect != nullptr);
    GTEST_LOG_(INFO) << "RSTransitionEffectTest Custom001 end";
}
} // namespace Rosen
} // namespace OHOS
