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

#include "gtest/gtest.h"

#include "draw/paint.h"

#ifdef RS_ENABLE_GPU
#include "image/gpu_context.h"
#endif

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
namespace Drawing {
class PaintTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void PaintTest::SetUpTestCase() {}
void PaintTest::TearDownTestCase() {}
void PaintTest::SetUp() {}
void PaintTest::TearDown() {}

/**
 * @tc.name: SetGPUContext001
 * @tc.desc: test Paint
 * @tc.type: FUNC
 * @tc.require: issue#ICHPKE
 * @tc.author:
 */
HWTEST_F(PaintTest, SetGPUContext001, TestSize.Level1)
{
    auto paint = std::make_unique<Paint>();
    ASSERT_TRUE(paint != nullptr);
    auto shaderEffect = ShaderEffect::CreateColorShader(Color::COLOR_TRANSPARENT);
    ASSERT_TRUE(shaderEffect != nullptr);
    paint->SetShaderEffect(shaderEffect);
#ifdef RS_ENABLE_GPU
    auto gpuContext = std::make_shared<GPUContext>();
    paint->SetGPUContext(gpuContext);
#endif
}
} // namespace Drawing
} // namespace Rosen
} // namespace OHOS
