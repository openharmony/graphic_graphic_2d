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
class RSShaderTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void RSShaderTest::SetUpTestCase() {}
void RSShaderTest::TearDownTestCase() {}
void RSShaderTest::SetUp() {}
void RSShaderTest::TearDown() {}

/**
 * @tc.name: CreateRSShaderTest
 * @tc.desc:
 * @tc.type: FUNC
 */
HWTEST_F(RSShaderTest, CreateRSShaderTest, TestSize.Level1)
{
    auto rsShader = RSShader::CreateRSShader();
    ASSERT_NE(rsShader, nullptr);
}

/**
 * @tc.name: SetSkShaderTest
 * @tc.desc:
 * @tc.type: FUNC
 */
HWTEST_F(RSShaderTest, SetSkShaderTest, TestSize.Level1)
{
    std::shared_ptr<RSShader> shaderPtr = RSShader::CreateRSShader();
    std::shared_ptr<Drawing::ShaderEffect> drawingShader;
    shaderPtr->SetDrawingShader(drawingShader);
}
} // namespace OHOS::Rosen
