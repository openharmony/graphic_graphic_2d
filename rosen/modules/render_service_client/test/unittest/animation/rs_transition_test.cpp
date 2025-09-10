/*
 * Copyright (c) 2025 Huawei Device Co., Ltd.
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

#include "animation/rs_transition.h"
#include "animation/rs_transition_effect.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
class RSTransitionTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void RSTransitionTest::SetUpTestCase() {}
void RSTransitionTest::TearDownTestCase() {}
void RSTransitionTest::SetUp() {}
void RSTransitionTest::TearDown() {}

/**
 * @tc.name: SetTransitionEffectTest
 * @tc.desc: Verify the SetTransitionEffect
 * @tc.type:FUNC
 */
HWTEST_F(RSTransitionTest, SetTransitionEffectTest, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSTransitionTest SetTransitionEffectTest start";
    RSTransition transition(nullptr, false);
    ASSERT_FALSE(transition.effect_);
    auto effect = RSTransitionEffect::Create();
    transition.SetTransitionEffect(effect);
    ASSERT_TRUE(transition.effect_);
    GTEST_LOG_(INFO) << "RSTransitionTest SetTransitionEffectTest end";
}

/**
 * @tc.name: SetIsCustomTest
 * @tc.desc: Verify the SetIsCustom
 * @tc.type:FUNC
 */
HWTEST_F(RSTransitionTest, SetIsCustomTest, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSTransitionTest SetIsCustomTest start";
    RSTransition transition(nullptr, false);
    transition.SetIsCustom(false);
    ASSERT_FALSE(transition.isCustom_);
    transition.SetIsCustom(true);
    ASSERT_TRUE(transition.isCustom_);
    GTEST_LOG_(INFO) << "RSTransitionTest SetIsCustomTest end";
}
} // namespace Rosen
} // namespace OHOS
