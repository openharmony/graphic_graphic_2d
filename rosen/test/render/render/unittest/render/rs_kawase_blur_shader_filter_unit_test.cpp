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

#include "render/rs_kawase_blur_shader_filter.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
class RSKawaseBlurShaderFilterUnitTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void RSKawaseBlurShaderFilterUnitTest::SetUpTestCase() {}
void RSKawaseBlurShaderFilterUnitTest::TearDownTestCase() {}
void RSKawaseBlurShaderFilterUnitTest::SetUp() {}
void RSKawaseBlurShaderFilterUnitTest::TearDown() {}

/**
 * @tc.name: TestGetRadius
 * @tc.desc: Verify function GetRadius
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSKawaseBlurShaderFilterUnitTest, TestGetRadius, TestSize.Level1)
{
    int radius = 1;
    auto testFilter = std::make_shared<RSKawaseBlurShaderFilter>(radius);
    EXPECT_EQ(testFilter->GetRadius(), 1);
}

/**
 * @tc.name: TestGenerateGEVisualEffect
 * @tc.desc: Verify function GenerateGEVisualEffect
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSKawaseBlurShaderFilterUnitTest, TestGenerateGEVisualEffect, TestSize.Level1)
{
    int radius = 0;
    auto shaderFilter = std::make_shared<RSKawaseBlurShaderFilter>(radius);
    auto testEffectContainer = std::make_shared<Drawing::GEVisualEffectContainer>();
    shaderFilter->GenerateGEVisualEffect(testEffectContainer);
    EXPECT_FALSE(testEffectContainer->filterVec_.empty());
}
} // namespace Rosen
} // namespace OHOS