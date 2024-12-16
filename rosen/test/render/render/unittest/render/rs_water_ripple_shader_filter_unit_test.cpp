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

#include "render/rs_water_ripple_shader_filter.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
class RSWaterRippleShaderFilterUnitTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void RSWaterRippleShaderFilterUnitTest::SetUpTestCase() {}
void RSWaterRippleShaderFilterUnitTest::TearDownTestCase() {}
void RSWaterRippleShaderFilterUnitTest::SetUp() {}
void RSWaterRippleShaderFilterUnitTest::TearDown() {}

/**
 * @tc.name: TestGenerateGEVisualEffect01
 * @tc.desc: Verify function GenerateGEVisualEffect
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSWaterRippleShaderFilterUnitTest, TestGenerateGEVisualEffect01, TestSize.Level1)
{
    float progress = 0.1f;
    uint32_t waveCount = 2;
    float rippleCenterX = 0.3f;
    float rippleCenterY = 0.5f;
    uint32_t rippleMode = 1;

    auto testFilter = std::make_shared<RSWaterRippleShaderFilter>(progress, waveCount, rippleCenterX,
                                                                                rippleCenterY, rippleMode);
    auto visualContainer = std::make_shared<Drawing::GEVisualEffectContainer>();
    testFilter->GenerateGEVisualEffect(visualContainer);
    EXPECT_FALSE(visualContainer->filterVec_.empty());
}
} // namespace Rosen
} // namespace OHOS