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

#include "render/rs_magnifier_shader_filter.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
class RSMagnifierShaderFilterUnitTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void RSMagnifierShaderFilterUnitTest::SetUpTestCase() {}
void RSMagnifierShaderFilterUnitTest::TearDownTestCase() {}
void RSMagnifierShaderFilterUnitTest::SetUp() {}
void RSMagnifierShaderFilterUnitTest::TearDown() {}

/**
 * @tc.name: TestGenerateGEVisualEffect01
 * @tc.desc: Verify function GenerateGEVisualEffect
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSMagnifierShaderFilterUnitTest, TestGenerateGEVisualEffect01, TestSize.Level1)
{
    auto param = std::make_shared<RSMagnifierParams>();
    auto testFilter = std::make_shared<RSMagnifierShaderFilter>(param);
    auto effectContainer = std::make_shared<Drawing::GEVisualEffectContainer>();
    testFilter->GenerateGEVisualEffect(effectContainer);
    EXPECT_FALSE(effectContainer->filterVec_.empty());
}

/**
 * @tc.name: TestSetMagnifierOffset01
 * @tc.desc: Verify function SetMagnifierOffsetTest
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSMagnifierShaderFilterUnitTest, TestSetMagnifierOffset01, TestSize.Level1)
{
    auto param = std::make_shared<RSMagnifierParams>();
    auto testFilter = std::make_shared<RSMagnifierShaderFilter>(param);
    auto testFilterNull = std::make_shared<RSMagnifierShaderFilter>(nullptr);

    Drawing::Matrix matrix1;
    matrix1.PostRotate(90); // 90 degree
    testFilter->SetMagnifierOffset(matrix1);
    ASSERT_EQ(testFilter->rotateDegree_, 270);

    Drawing::Matrix matrix2;
    matrix2.PostRotate(180); // 180 degree
    testFilter->SetMagnifierOffset(matrix2);
    ASSERT_EQ(testFilter->rotateDegree_, 180);

    Drawing::Matrix matrix3;
    matrix3.PostRotate(270); // 270 degree
    testFilter->SetMagnifierOffset(matrix3);
    ASSERT_EQ(testFilter->rotateDegree_, 90);

    Drawing::Matrix matrix4; // 0 degree
    testFilter->SetMagnifierOffset(matrix4);
    ASSERT_EQ(testFilter->rotateDegree_, 0);
}

/**
 * @tc.name: TestGetMagnifierOffset01
 * @tc.desc: Verify function GetMagnifierOffset
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSMagnifierShaderFilterUnitTest, TestGetMagnifierOffset01, TestSize.Level1)
{
    auto param = std::make_shared<RSMagnifierParams>();
    auto testFilter = std::make_shared<RSMagnifierShaderFilter>(param);
    EXPECT_EQ(testFilter->GetMagnifierOffsetX(), 0.f);
    EXPECT_EQ(testFilter->GetMagnifierOffsetY(), 0.f);
}
} // namespace Rosen
} // namespace OHOS