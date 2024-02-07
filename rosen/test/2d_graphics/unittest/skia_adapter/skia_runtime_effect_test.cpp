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
#include "skia_adapter/skia_runtime_effect.h"
#include "effect/runtime_effect.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
namespace Drawing {
class SkiaRuntimeEffectTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void SkiaRuntimeEffectTest::SetUpTestCase() {}
void SkiaRuntimeEffectTest::TearDownTestCase() {}
void SkiaRuntimeEffectTest::SetUp() {}
void SkiaRuntimeEffectTest::TearDown() {}

/**
 * @tc.name: InitForShader001
 * @tc.desc: Test InitForShader
 * @tc.type: FUNC
 * @tc.require: I91EQ7
 */
HWTEST_F(SkiaRuntimeEffectTest, InitForShader001, TestSize.Level1)
{
    RuntimeEffectOptions op;
    std::string s = "safd";
    SkiaRuntimeEffect skiaRuntimeEffect;
    skiaRuntimeEffect.InitForShader(s, op);
}

/**
 * @tc.name: InitForShader002
 * @tc.desc: Test InitForShader
 * @tc.type: FUNC
 * @tc.require: I91EQ7
 */
HWTEST_F(SkiaRuntimeEffectTest, InitForShader002, TestSize.Level1)
{
    std::string s = "safd";
    SkiaRuntimeEffect skiaRuntimeEffect;
    skiaRuntimeEffect.InitForShader(s);
}

/**
 * @tc.name: InitForBlender001
 * @tc.desc: Test InitForBlender
 * @tc.type: FUNC
 * @tc.require: I91EQ7
 */
HWTEST_F(SkiaRuntimeEffectTest, InitForBlender001, TestSize.Level1)
{
    std::string s = "safd";
    SkiaRuntimeEffect skiaRuntimeEffect;
    skiaRuntimeEffect.InitForBlender(s);
}

/**
 * @tc.name: GetRuntimeEffect001
 * @tc.desc: Test GetRuntimeEffect
 * @tc.type: FUNC
 * @tc.require: I91EQ7
 */
HWTEST_F(SkiaRuntimeEffectTest, GetRuntimeEffect001, TestSize.Level1)
{
    SkiaRuntimeEffect skiaRuntimeEffect;
    EXPECT_TRUE(skiaRuntimeEffect.GetRuntimeEffect() == nullptr);
}

/**
 * @tc.name: MakeShader001
 * @tc.desc: Test MakeShader
 * @tc.type: FUNC
 * @tc.require: I91EQ7
 */
HWTEST_F(SkiaRuntimeEffectTest, MakeShader001, TestSize.Level1)
{
    SkiaRuntimeEffect skiaRuntimeEffect;
    std::shared_ptr<Data> uniforms = std::make_shared<Data>();
    std::shared_ptr<ShaderEffect> children[] = {ShaderEffect::CreateColorShader(0xFF000000)};
    Matrix matrix;
    skiaRuntimeEffect.MakeShader(uniforms, children, 1, &matrix, true);
    skiaRuntimeEffect.MakeShader(nullptr, children, 1, nullptr, true);
}
} // namespace Drawing
} // namespace Rosen
} // namespace OHOS