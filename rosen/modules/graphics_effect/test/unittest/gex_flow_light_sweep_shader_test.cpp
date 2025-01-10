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

#include "ext/gex_flow_light_sweep_shader.h"

#include "draw/color.h"
#include "image/bitmap.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace GraphicsEffectEngine {

using namespace Rosen;

class GEXFlowLightSweepShaderTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void GEXFlowLightSweepShaderTest::SetUpTestCase() {}
void GEXFlowLightSweepShaderTest::TearDownTestCase() {}
void GEXFlowLightSweepShaderTest::SetUp() {}
void GEXFlowLightSweepShaderTest::TearDown() {}

/**
 * @tc.name: CreateDynamicImpl001
 * @tc.desc: Verify function CreateDynamicImpl
 * @tc.type:FUNC
 */
HWTEST_F(GEXFlowLightSweepShaderTest, CreateDynamicImpl001, TestSize.Level1)
{
    std::vector<std::pair<Drawing::Color, float>> para;
    para.push_back(std::pair(Drawing::Color::COLOR_RED, 0.0f));
    para.push_back(std::pair(Drawing::Color::COLOR_GREEN, 0.5f));
    auto shader = GEXFlowLightSweepShader::CreateDynamicImpl(para);

    ASSERT_NE(shader, nullptr);
}

/**
 * @tc.name: GetDescription001
 * @tc.desc: Verify function GetDescription
 * @tc.type:FUNC
 */
HWTEST_F(GEXFlowLightSweepShaderTest, GetDescription001, TestSize.Level1)
{
    auto shader = std::make_shared<GEXFlowLightSweepShader>();
    ASSERT_TRUE(shader != nullptr);

    std::string expectStr = "GEXFlowLightSweepShader";
    EXPECT_EQ(shader->GetDescription(), expectStr);
}

} // namespace GraphicsEffectEngine
} // namespace OHOS
