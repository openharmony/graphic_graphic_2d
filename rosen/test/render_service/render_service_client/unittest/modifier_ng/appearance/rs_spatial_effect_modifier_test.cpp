/*
 * Copyright (c) 2026 Huawei Device Co., Ltd.
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

#include <memory>

#include "gtest/gtest.h"
#include "modifier_ng/appearance/rs_spatial_effect_modifier.h"

using namespace testing;
using namespace testing::ext;
using namespace OHOS::Rosen::ModifierNG;

namespace OHOS::Rosen {
class RSSpatialEffectModifierTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override {}
    void TearDown() override {}
};

void RSSpatialEffectModifierTest::SetUpTestCase() {}
void RSSpatialEffectModifierTest::TearDownTestCase() {}

/**
 * @tc.name: GetType001
 * @tc.desc: Test GetType of RSSpatialEffectModifier
 * @tc.type: FUNC
 */
HWTEST_F(RSSpatialEffectModifierTest, GetType001, TestSize.Level1)
{
    auto modifier = std::make_shared<ModifierNG::RSSpatialEffectModifier>();
    EXPECT_EQ(modifier->GetType(), ModifierNG::RSModifierType::SPATIAL_EFFECT);
}

/**
 * @tc.name: SetSpatialEffectDepth001
 * @tc.desc: Test SetSpatialEffectDepth with positive value
 * @tc.type: FUNC
 */
HWTEST_F(RSSpatialEffectModifierTest, SetSpatialEffectDepth001, TestSize.Level1)
{
    auto modifier = std::make_shared<ModifierNG::RSSpatialEffectModifier>();
    EXPECT_NO_FATAL_FAILURE(modifier->SetSpatialEffectDepth(0.5f));
}

/**
 * @tc.name: SetSpatialEffectDepth002
 * @tc.desc: Test SetSpatialEffectDepth with negative value
 * @tc.type: FUNC
 */
HWTEST_F(RSSpatialEffectModifierTest, SetSpatialEffectDepth002, TestSize.Level1)
{
    auto modifier = std::make_shared<ModifierNG::RSSpatialEffectModifier>();
    EXPECT_NO_FATAL_FAILURE(modifier->SetSpatialEffectDepth(-0.5f));
}

/**
 * @tc.name: SetSpatialEffectDepth003
 * @tc.desc: Test SetSpatialEffectDepth with zero value
 * @tc.type: FUNC
 */
HWTEST_F(RSSpatialEffectModifierTest, SetSpatialEffectDepth003, TestSize.Level1)
{
    auto modifier = std::make_shared<ModifierNG::RSSpatialEffectModifier>();
    EXPECT_NO_FATAL_FAILURE(modifier->SetSpatialEffectDepth(0.0f));
}

/**
 * @tc.name: SetSpatialEffectLeftTop001
 * @tc.desc: Test SetSpatialEffectLeftTop with custom Vector3f
 * @tc.type: FUNC
 */
HWTEST_F(RSSpatialEffectModifierTest, SetSpatialEffectLeftTop001, TestSize.Level1)
{
    auto modifier = std::make_shared<ModifierNG::RSSpatialEffectModifier>();
    EXPECT_NO_FATAL_FAILURE(modifier->SetSpatialEffectLeftTop(Vector3f(1.0f, 2.0f, 3.0f)));
}

/**
 * @tc.name: SetSpatialEffectLeftTop002
 * @tc.desc: Test SetSpatialEffectLeftTop with zero values
 * @tc.type: FUNC
 */
HWTEST_F(RSSpatialEffectModifierTest, SetSpatialEffectLeftTop002, TestSize.Level1)
{
    auto modifier = std::make_shared<ModifierNG::RSSpatialEffectModifier>();
    EXPECT_NO_FATAL_FAILURE(modifier->SetSpatialEffectLeftTop(Vector3f(0.0f, 0.0f, 0.0f)));
}

/**
 * @tc.name: SetSpatialEffectRightTop001
 * @tc.desc: Test SetSpatialEffectRightTop with custom Vector3f
 * @tc.type: FUNC
 */
HWTEST_F(RSSpatialEffectModifierTest, SetSpatialEffectRightTop001, TestSize.Level1)
{
    auto modifier = std::make_shared<ModifierNG::RSSpatialEffectModifier>();
    EXPECT_NO_FATAL_FAILURE(modifier->SetSpatialEffectRightTop(Vector3f(4.0f, 5.0f, 6.0f)));
}

/**
 * @tc.name: SetSpatialEffectRightTop002
 * @tc.desc: Test SetSpatialEffectRightTop with zero values
 * @tc.type: FUNC
 */
HWTEST_F(RSSpatialEffectModifierTest, SetSpatialEffectRightTop002, TestSize.Level1)
{
    auto modifier = std::make_shared<ModifierNG::RSSpatialEffectModifier>();
    EXPECT_NO_FATAL_FAILURE(modifier->SetSpatialEffectRightTop(Vector3f(0.0f, 0.0f, 0.0f)));
}

/**
 * @tc.name: SetSpatialEffectLeftBottom001
 * @tc.desc: Test SetSpatialEffectLeftBottom with custom Vector3f
 * @tc.type: FUNC
 */
HWTEST_F(RSSpatialEffectModifierTest, SetSpatialEffectLeftBottom001, TestSize.Level1)
{
    auto modifier = std::make_shared<ModifierNG::RSSpatialEffectModifier>();
    EXPECT_NO_FATAL_FAILURE(modifier->SetSpatialEffectLeftBottom(Vector3f(7.0f, 8.0f, 9.0f)));
}

/**
 * @tc.name: SetSpatialEffectLeftBottom002
 * @tc.desc: Test SetSpatialEffectLeftBottom with zero values
 * @tc.type: FUNC
 */
HWTEST_F(RSSpatialEffectModifierTest, SetSpatialEffectLeftBottom002, TestSize.Level1)
{
    auto modifier = std::make_shared<ModifierNG::RSSpatialEffectModifier>();
    EXPECT_NO_FATAL_FAILURE(modifier->SetSpatialEffectLeftBottom(Vector3f(0.0f, 0.0f, 0.0f)));
}

/**
 * @tc.name: SetSpatialEffectRightBottom001
 * @tc.desc: Test SetSpatialEffectRightBottom with custom Vector3f
 * @tc.type: FUNC
 */
HWTEST_F(RSSpatialEffectModifierTest, SetSpatialEffectRightBottom001, TestSize.Level1)
{
    auto modifier = std::make_shared<ModifierNG::RSSpatialEffectModifier>();
    EXPECT_NO_FATAL_FAILURE(modifier->SetSpatialEffectRightBottom(Vector3f(10.0f, 11.0f, 12.0f)));
}

/**
 * @tc.name: SetSpatialEffectRightBottom002
 * @tc.desc: Test SetSpatialEffectRightBottom with zero values
 * @tc.type: FUNC
 */
HWTEST_F(RSSpatialEffectModifierTest, SetSpatialEffectRightBottom002, TestSize.Level1)
{
    auto modifier = std::make_shared<ModifierNG::RSSpatialEffectModifier>();
    EXPECT_NO_FATAL_FAILURE(modifier->SetSpatialEffectRightBottom(Vector3f(0.0f, 0.0f, 0.0f)));
}

/**
 * @tc.name: SetSpatialEffectOcclusionWeight001
 * @tc.desc: Test SetSpatialEffectOcclusionWeight with positive value
 * @tc.type: FUNC
 */
HWTEST_F(RSSpatialEffectModifierTest, SetSpatialEffectOcclusionWeight001, TestSize.Level1)
{
    auto modifier = std::make_shared<ModifierNG::RSSpatialEffectModifier>();
    EXPECT_NO_FATAL_FAILURE(modifier->SetSpatialEffectOcclusionWeight(0.5f));
}

/**
 * @tc.name: SetSpatialEffectOcclusionWeight002
 * @tc.desc: Test SetSpatialEffectOcclusionWeight with zero value
 * @tc.type: FUNC
 */
HWTEST_F(RSSpatialEffectModifierTest, SetSpatialEffectOcclusionWeight002, TestSize.Level1)
{
    auto modifier = std::make_shared<ModifierNG::RSSpatialEffectModifier>();
    EXPECT_NO_FATAL_FAILURE(modifier->SetSpatialEffectOcclusionWeight(0.0f));
}

/**
 * @tc.name: SetSpatialEffectOcclusionWeight003
 * @tc.desc: Test SetSpatialEffectOcclusionWeight with max value
 * @tc.type: FUNC
 */
HWTEST_F(RSSpatialEffectModifierTest, SetSpatialEffectOcclusionWeight003, TestSize.Level1)
{
    auto modifier = std::make_shared<ModifierNG::RSSpatialEffectModifier>();
    EXPECT_NO_FATAL_FAILURE(modifier->SetSpatialEffectOcclusionWeight(1.0f));
}

/**
 * @tc.name: MultipleSetterCalls001
 * @tc.desc: Test calling multiple setters in sequence
 * @tc.type: FUNC
 */
HWTEST_F(RSSpatialEffectModifierTest, MultipleSetterCalls001, TestSize.Level1)
{
    auto modifier = std::make_shared<ModifierNG::RSSpatialEffectModifier>();
    EXPECT_NO_FATAL_FAILURE(modifier->SetSpatialEffectDepth(0.8f));
    EXPECT_NO_FATAL_FAILURE(modifier->SetSpatialEffectLeftTop(Vector3f(1.0f, 2.0f, 3.0f)));
    EXPECT_NO_FATAL_FAILURE(modifier->SetSpatialEffectRightTop(Vector3f(4.0f, 5.0f, 6.0f)));
    EXPECT_NO_FATAL_FAILURE(modifier->SetSpatialEffectLeftBottom(Vector3f(7.0f, 8.0f, 9.0f)));
    EXPECT_NO_FATAL_FAILURE(modifier->SetSpatialEffectRightBottom(Vector3f(10.0f, 11.0f, 12.0f)));
    EXPECT_NO_FATAL_FAILURE(modifier->SetSpatialEffectOcclusionWeight(0.5f));
}

/**
 * @tc.name: SetSpatialEffectMode001
 * @tc.desc: Test SetSpatialEffectMode with both mode values
 * @tc.type: FUNC
 */
HWTEST_F(RSSpatialEffectModifierTest, SetSpatialEffectMode001, TestSize.Level1)
{
    auto modifier = std::make_shared<ModifierNG::RSSpatialEffectModifier>();
    EXPECT_NO_FATAL_FAILURE(modifier->SetSpatialEffectMode(0));
    EXPECT_NO_FATAL_FAILURE(modifier->SetSpatialEffectMode(1));
}
} // namespace OHOS::Rosen
