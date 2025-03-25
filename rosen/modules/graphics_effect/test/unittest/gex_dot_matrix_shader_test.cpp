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
 
#include <gtest/gtest.h>
 
#include "ext/gex_dot_matrix_shader.h"
 
using namespace testing;
using namespace testing::ext;
 
namespace OHOS {
namespace GraphicsEffectEngine {
 
using namespace Rosen;
 
class GEXDotMatrixShaderTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
 
    DotMatrixNormalParams params_;
};
 
void GEXDotMatrixShaderTest::SetUpTestCase(void) {}
void GEXDotMatrixShaderTest::TearDownTestCase(void) {}
 
void GEXDotMatrixShaderTest::SetUp() {}
 
void GEXDotMatrixShaderTest::TearDown() {}
 
/**
 * @tc.name: CreateDynamicImpl001
 * @tc.desc: Verify function CreateDynamicImpl
 * @tc.type:FUNC
 */
HWTEST_F(GEXDotMatrixShaderTest, CreateDynamicImpl001, TestSize.Level1)
{
    auto impl = GEXDotMatrixShader::CreateDynamicImpl(params_);
    EXPECT_EQ(impl, nullptr);
}
 
/**
 * @tc.name: GetDescription001
 * @tc.desc: Verify function GetDescription
 * @tc.type:FUNC
 */
HWTEST_F(GEXDotMatrixShaderTest, GetDescription001, TestSize.Level1)
{
    auto dotShader = std::make_shared<GEXDotMatrixShader>();
    auto description = dotShader->GetDescription();
    EXPECT_FALSE(description.empty());
}
} // namespace GraphicsEffectEngine
} // namespace OHOS