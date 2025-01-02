/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
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

#include "render/rs_shader.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Rosen {
class RSShaderUnitTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void RSShaderUnitTest::SetUpTestCase() {}
void RSShaderUnitTest::TearDownTestCase() {}
void RSShaderUnitTest::SetUp() {}
void RSShaderUnitTest::TearDown() {}

/**
 * @tc.name: TestCreateRSShader01
 * @tc.desc: Verify function CreateRSShader
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSShaderUnitTest, TestCreateRSShader01, TestSize.Level1)
{
    auto testShader = RSShader::CreateRSShader();
    ASSERT_NE(testShader, nullptr);
}

/**
 * @tc.name: TestCreateRSShader02
 * @tc.desc: Verify function CreateRSShader
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSShaderUnitTest, TestCreateRSShader02, TestSize.Level1)
{
    std::shared_ptr<Drawing::ShaderEffect> testShader;
    EXPECT_NE(RSShader::CreateRSShader(testShader), nullptr);
}

/**
 * @tc.name: TestSetSkShader01
 * @tc.desc: Verify function SetSkShader
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSShaderUnitTest, TestSetSkShader01, TestSize.Level1)
{
    std::shared_ptr<RSShader> shaderPtr = RSShader::CreateRSShader();
    std::shared_ptr<Drawing::ShaderEffect> testShader;
    shaderPtr->SetDrawingShader(testShader);
    ASSERT_EQ(shaderPtr->drShader_, testShader);
}

/**
 * @tc.name: TestCreateRSShader03
 * @tc.desc: Verify function CreateRSShader
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSShaderUnitTest, TestCreateRSShader03, TestSize.Level1)
{
    auto testShader = RSShader::CreateRSShader();
    EXPECT_EQ(testShader->GetDrawingShader(), nullptr);
}

} // namespace OHOS::Rosen
