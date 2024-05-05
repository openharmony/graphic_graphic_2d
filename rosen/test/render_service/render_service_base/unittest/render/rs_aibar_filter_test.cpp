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

#include "render/rs_aibar_filter.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Rosen {
class RSAIBarFilterTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void RSAIBarFilterTest::SetUpTestCase() {}
void RSAIBarFilterTest::TearDownTestCase() {}
void RSAIBarFilterTest::SetUp() {}
void RSAIBarFilterTest::TearDown() {}

/**
 * @tc.name: DrawImageRectTest
 * @tc.desc: Verify function DrawImageRect
 * @tc.type:FUNC
 * @tc.require: issueI9I98H
 */
HWTEST_F(RSAIBarFilterTest, DrawImageRectTest, TestSize.Level1)
{
    auto filter = std::make_shared<RSAIBarFilter>();
    Drawing::Canvas canvas;
    auto image = std::make_shared<Drawing::Image>();
    Drawing::Rect src;
    Drawing::Rect dst;
    filter->DrawImageRect(canvas, image, src, dst);
    EXPECT_EQ(image->GetWidth(), 0);
}

/**
 * @tc.name: GetDescriptionTest
 * @tc.desc: Verify function GetDescription
 * @tc.type:FUNC
 * @tc.require: issueI9I98H
 */
HWTEST_F(RSAIBarFilterTest, GetDescriptionTest, TestSize.Level1)
{
    auto filter = std::make_shared<RSAIBarFilter>();
    EXPECT_EQ(filter->GetDescription(), "RSAIBarFilter");
}

/**
 * @tc.name: GetAiInvertCoefTest
 * @tc.desc: Verify function GetAiInvertCoef
 * @tc.type:FUNC
 * @tc.require: issueI9I98H
 */
HWTEST_F(RSAIBarFilterTest, GetAiInvertCoefTest, TestSize.Level1)
{
    auto filter = std::make_shared<RSAIBarFilter>();
    // for test
    std::vector<float> aiInvertCoef = { 0.0f, 1.0f, 0.55f, 0.4f, 1.6f, 45.0f };
    EXPECT_EQ(filter->GetAiInvertCoef(), aiInvertCoef);
}

/**
 * @tc.name: IsAiInvertCoefValidTest
 * @tc.desc: Verify function IsAiInvertCoefValid
 * @tc.type:FUNC
 * @tc.require: issueI9I98H
 */
HWTEST_F(RSAIBarFilterTest, IsAiInvertCoefValidTest, TestSize.Level1)
{
    auto filter = std::make_shared<RSAIBarFilter>();
    // for test
    std::vector<float> aiInvertCoef = { 0.0f, 1.0f, 0.55f, 0.4f, 1.6f, 45.0f };
    EXPECT_TRUE(filter->IsAiInvertCoefValid(aiInvertCoef));
}

/**
 * @tc.name: MakeBinarizationShaderTest
 * @tc.desc: Verify function MakeBinarizationShader
 * @tc.type:FUNC
 * @tc.require: issueI9I98H
 */
HWTEST_F(RSAIBarFilterTest, MakeBinarizationShaderTest, TestSize.Level1)
{
    auto filter = std::make_shared<RSAIBarFilter>();
    float imageWidth = 1.0f;
    float imageHeight = 1.0f;
    auto imageShader = std::make_shared<Drawing::ShaderEffect>();
    auto runtimeShaderBuilder = filter->MakeBinarizationShader(imageWidth, imageHeight, imageShader);
    EXPECT_NE(runtimeShaderBuilder, nullptr);
}

/**
 * @tc.name: testGetDetailedDescription
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSAIBarFilterTest, testGetDetailedDescription, TestSize.Level1)
{
    auto filter = std::make_shared<RSAIBarFilter>();
    std::string expectRet = "RSAIBarFilterBlur";
    EXPECT_EQ(filter->GetDetailedDescription(), expectRet);
}
} // namespace OHOS::Rosen