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

#include "ge_visual_effect.h"
#include "ge_visual_effect_container.h"
#include "render/rs_render_magnifier_filter.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
class RSMagnifierShaderFilterTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void RSMagnifierShaderFilterTest::SetUpTestCase() {}
void RSMagnifierShaderFilterTest::TearDownTestCase() {}
void RSMagnifierShaderFilterTest::SetUp() {}
void RSMagnifierShaderFilterTest::TearDown() {}

/**
 * @tc.name: GenerateGEVisualEffectTest
 * @tc.desc: Verify function GenerateGEVisualEffect
 * @tc.type:FUNC
 * @tc.require: issuesI9UWCD
 */
HWTEST_F(RSMagnifierShaderFilterTest, GenerateGEVisualEffectTest, TestSize.Level1)
{
    auto para = std::make_shared<RSMagnifierParams>();
    auto rsMagnifierShaderFilter = std::make_shared<RSMagnifierShaderFilter>(para);
    auto visualEffectContainer = std::make_shared<Drawing::GEVisualEffectContainer>();
    ASSERT_NE(rsMagnifierShaderFilter, nullptr);
    rsMagnifierShaderFilter->GenerateGEVisualEffect(visualEffectContainer);
    ASSERT_NE(visualEffectContainer, nullptr);
    EXPECT_FALSE(visualEffectContainer->filterVec_.empty());
}

/**
 * @tc.name: SetMagnifierOffsetTest
 * @tc.desc: Verify function SetMagnifierOffsetTest
 * @tc.type:FUNC
 * @tc.require: issuesI9UWCD
 */
HWTEST_F(RSMagnifierShaderFilterTest, SetMagnifierOffsetTest, TestSize.Level1)
{
    auto para = std::make_shared<RSMagnifierParams>();
    auto rsMagnifierShaderFilter = std::make_shared<RSMagnifierShaderFilter>(para);
    auto rsMagnifierShaderFilterNull = std::make_shared<RSMagnifierShaderFilter>(nullptr);
    ASSERT_NE(rsMagnifierShaderFilter, nullptr);
    Drawing::Matrix matrix1;
    matrix1.PostRotate(90); // 90 degree
    rsMagnifierShaderFilter->SetMagnifierOffset(matrix1);
    ASSERT_EQ(rsMagnifierShaderFilter->rotateDegree_, 270);

    Drawing::Matrix matrix2;
    matrix2.PostRotate(180); // 180 degree
    rsMagnifierShaderFilter->SetMagnifierOffset(matrix2);
    ASSERT_EQ(rsMagnifierShaderFilter->rotateDegree_, 180);

    Drawing::Matrix matrix3;
    matrix3.PostRotate(270); // 270 degree
    rsMagnifierShaderFilter->SetMagnifierOffset(matrix3);
    ASSERT_EQ(rsMagnifierShaderFilter->rotateDegree_, 90);

    Drawing::Matrix matrix4; // 0 degree
    rsMagnifierShaderFilter->SetMagnifierOffset(matrix4);
    ASSERT_EQ(rsMagnifierShaderFilter->rotateDegree_, 0);
}

/**
 * @tc.name: GetMagnifierOffsetTest
 * @tc.desc: Verify function GetMagnifierOffsetTest
 * @tc.type:FUNC
 * @tc.require: issuesI9UWCD
 */
HWTEST_F(RSMagnifierShaderFilterTest, GetMagnifierOffsetTest, TestSize.Level1)
{
    auto para = std::make_shared<RSMagnifierParams>();
    auto rsMagnifierShaderFilter = std::make_shared<RSMagnifierShaderFilter>(para);
    ASSERT_NE(rsMagnifierShaderFilter, nullptr);
    EXPECT_EQ(rsMagnifierShaderFilter->GetMagnifierOffsetX(), 0.f);
    EXPECT_EQ(rsMagnifierShaderFilter->GetMagnifierOffsetY(), 0.f);
}
} // namespace Rosen
} // namespace OHOS