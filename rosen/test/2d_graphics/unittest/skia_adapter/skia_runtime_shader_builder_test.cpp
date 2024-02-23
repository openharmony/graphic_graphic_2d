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

#include <cstddef>
#include "gtest/gtest.h"
#include "draw/surface.h"
#include "effect/runtime_effect.h"
#include "effect/runtime_shader_builder.h"
#include "effect/shader_effect.h"
#include "skia_adapter/skia_runtime_shader_builder.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
namespace Drawing {
class SkiaRuntimeShaderBuilderTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void SkiaRuntimeShaderBuilderTest::SetUpTestCase() {}
void SkiaRuntimeShaderBuilderTest::TearDownTestCase() {}
void SkiaRuntimeShaderBuilderTest::SetUp() {}
void SkiaRuntimeShaderBuilderTest::TearDown() {}

/**
 * @tc.name: MakeShader001
 * @tc.desc: Test MakeShader
 * @tc.type: FUNC
 * @tc.require: I91EQ7
 */
HWTEST_F(SkiaRuntimeShaderBuilderTest, MakeShader001, TestSize.Level1)
{
    SkiaRuntimeShaderBuilder skiaRuntimeShaderBuilder;
    skiaRuntimeShaderBuilder.MakeShader(nullptr, false);
    Matrix matrix;
    skiaRuntimeShaderBuilder.MakeShader(&matrix, false);
}

/**
 * @tc.name: SetUniform001
 * @tc.desc: Test SetUniform
 * @tc.type: FUNC
 * @tc.require: I91EQ7
 */
HWTEST_F(SkiaRuntimeShaderBuilderTest, SetUniform001, TestSize.Level1)
{
    SkiaRuntimeShaderBuilder skiaRuntimeShaderBuilder;
    skiaRuntimeShaderBuilder.SetUniform("lightPos", 1.0f);
    skiaRuntimeShaderBuilder.SetUniform("lightPos", 1.0f, 1.0f);
    skiaRuntimeShaderBuilder.SetUniform("lightPos", 1.0f, 1.0f, 1.0f);
    skiaRuntimeShaderBuilder.SetUniform("lightPos", 1.0f, 1.0f, 1.0f, 1.0f);
    Matrix44 matrix;
    skiaRuntimeShaderBuilder.SetUniform("lightPos", matrix);
}

/**
 * @tc.name: SetUniformVec4001
 * @tc.desc: Test SetUniformVec4
 * @tc.type: FUNC
 * @tc.require: I91EQ7
 */
HWTEST_F(SkiaRuntimeShaderBuilderTest, SetUniformVec4001, TestSize.Level1)
{
    SkiaRuntimeShaderBuilder skiaRuntimeShaderBuilder;
    skiaRuntimeShaderBuilder.SetUniformVec4("lightPos", 1.0f, 1.0f, 1.0f, 1.0f);
}
} // namespace Drawing
} // namespace Rosen
} // namespace OHOS