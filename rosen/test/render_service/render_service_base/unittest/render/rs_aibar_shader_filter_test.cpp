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

#include "render/rs_aibar_shader_filter.h"

using namespace testing;
using namespace testing::ext;
namespace OHOS {
namespace Rosen {

class RSAIBarShaderFilterTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void RSAIBarShaderFilterTest::SetUpTestCase() {}
void RSAIBarShaderFilterTest::TearDownTestCase() {}
void RSAIBarShaderFilterTest::SetUp() {}
void RSAIBarShaderFilterTest::TearDown() {}

/**
 * @tc.name: DrawImageRectTest
 * @tc.desc: Verify function DrawImageRect
 * @tc.type:FUNC
 * @tc.require: issuesI9PH4G
 */
HWTEST_F(RSAIBarShaderFilterTest, DrawImageRectTest, TestSize.Level1)
{
    auto rsAIBarShaderFilter = std::make_shared<RSAIBarShaderFilter>();
    // for test
    std::vector<float> aiInvertCoef = { 0.0, 1.0, 0.55, 0.4, 1.6, 45.0 };
    EXPECT_EQ(rsAIBarShaderFilter->GetAiInvertCoef(), aiInvertCoef);
}

/**
 * @tc.name: GenerateGEVisualEffectTest
 * @tc.desc: Verify function GenerateGEVisualEffect
 * @tc.type:FUNC
 * @tc.require: issuesI9PH4G
 */
HWTEST_F(RSAIBarShaderFilterTest, GenerateGEVisualEffectTest, TestSize.Level1)
{
    auto rsAIBarShaderFilter = std::make_shared<RSAIBarShaderFilter>();
    auto visualEffectContainer = std::make_shared<Drawing::GEVisualEffectContainer>();
    rsAIBarShaderFilter->GenerateGEVisualEffect(visualEffectContainer);
    EXPECT_FALSE(visualEffectContainer->filterVec_.empty());
}
} // namespace Rosen
} // namespace OHOS