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

#include "render/rs_flow_light_sweep_shader.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
class RSFlowLightSweepShaderTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void RSFlowLightSweepShaderTest::SetUpTestCase() {}
void RSFlowLightSweepShaderTest::TearDownTestCase() {}
void RSFlowLightSweepShaderTest::SetUp() {}
void RSFlowLightSweepShaderTest::TearDown() {}

/**
 * @tc.name: MakeDrawingShaderTest
 * @tc.desc: Verify function MakeDrawingShader
 * @tc.type:FUNC
 * @tc.require: issuesI9UWCD
 */
HWTEST_F(RSFlowLightSweepShaderTest, MakeDrawingShaderTest, TestSize.Level1)
{
    std::vector<std::pair<Drawing::Color, float>> para;
    para.push_back(std::pair(Drawing::Color::COLOR_RED, 0.0f));
    para.push_back(std::pair(Drawing::Color::COLOR_GREEN, 0.5f));
    auto rsFlowLightSweepShader = std::make_shared<RSFlowLightSweepShader>(para);

    RectF destRect(10.0f, 10.0f, 50.0f, 50.0f);
    float progress = 0.5f;
    rsFlowLightSweepShader->MakeDrawingShader(destRect, progress);

    EXPECT_TRUE(rsFlowLightSweepShader->GetShaderType() == RSShader::ShaderType::FLOW_LIGHT_SWEEP);
}

/**
 * @tc.name: GetDrawingShaderTest01
 * @tc.desc: Verify function GetDrawingShader
 * @tc.type:FUNC
 * @tc.require: issuesI9UWCD
 */
HWTEST_F(RSFlowLightSweepShaderTest, GetDrawingShaderTest01, TestSize.Level1)
{
    std::shared_ptr<RSFlowLightSweepShader> rsFlowLightSweepShader = nullptr;

    EXPECT_TRUE(rsFlowLightSweepShader->GetDrawingShader() == nullptr);
}

/**
 * @tc.name: MarshallingTest
 * @tc.desc: Verify function Marshalling
 * @tc.type:FUNC
 * @tc.require: issuesI9UWCD
 */
HWTEST_F(RSFlowLightSweepShaderTest, MarshallingTest, TestSize.Level1)
{
    std::vector<std::pair<Drawing::Color, float>> para;
    para.push_back(std::pair(Drawing::Color::COLOR_RED, 0.0f));
    para.push_back(std::pair(Drawing::Color::COLOR_GREEN, 0.5f));
    auto rsFlowLightSweepShader = std::make_shared<RSFlowLightSweepShader>(para);

    Parcel parcel;
    EXPECT_TRUE(rsFlowLightSweepShader->Marshalling(parcel) == true);
}

/**
 * @tc.name: UnmarshallingTest
 * @tc.desc: Verify function Unmarshalling
 * @tc.type:FUNC
 * @tc.require: issuesI9UWCD
 */
HWTEST_F(RSFlowLightSweepShaderTest, UnmarshallingTest, TestSize.Level1)
{
    std::vector<std::pair<Drawing::Color, float>> para;
    para.push_back(std::pair(Drawing::Color::COLOR_RED, 0.0f));
    para.push_back(std::pair(Drawing::Color::COLOR_GREEN, 0.5f));
    auto rsFlowLightSweepShader = std::make_shared<RSFlowLightSweepShader>(para);

    Parcel parcel;
    rsFlowLightSweepShader->Marshalling(parcel);

    auto rsFlowLightSweepShader2 = std::make_shared<RSFlowLightSweepShader>();
    bool needReset = true;
    ASSERT_EQ(rsFlowLightSweepShader2->Unmarshalling(parcel, needReset), true);
    EXPECT_FALSE(needReset);
}
} // namespace Rosen
} // namespace OHOS
