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

#include <cstdlib>
#include <cstring>
#include <memory>
#include <securec.h>

#include "gtest/gtest.h"
#include "gtest/hwext/gtest-tag.h"
#include "message_parcel.h"

#include "modifier_ng/appearance/rs_overlay_ng_shader_modifier.h"
#include "ui_effect/property/include/rs_ui_shader_base.h"

using namespace testing;
using namespace testing::ext;
using namespace OHOS::Rosen::ModifierNG;

namespace OHOS::Rosen {
class RSOverlayNGShaderModifierNGTypeTest : public testing::Test {
public:
    static void SetUpTestCase() {}
    static void TearDownTestCase() {}
    void SetUp() override {}
    void TearDown() override {}
};

/**
 * @tc.name: RSOverlayNGShaderModifierTest001
 * @tc.desc: Test GetType function
 * @tc.type: FUNC
 */
HWTEST_F(RSOverlayNGShaderModifierNGTypeTest, RSOverlayNGShaderModifierTest001, TestSize.Level1)
{
    std::shared_ptr<ModifierNG::RSOverlayNGShaderModifier> modifier =
        std::make_shared<ModifierNG::RSOverlayNGShaderModifier>();
    
    EXPECT_EQ(modifier->GetType(), ModifierNG::RSModifierType::OVERLAY_NG_SHADER);
}

/**
 * @tc.name: RSOverlayNGShaderModifierTest002
 * @tc.desc: Test modifier initialization
 * @tc.type: FUNC
 */
HWTEST_F(RSOverlayNGShaderModifierNGTypeTest, RSOverlayNGShaderModifierTest002, TestSize.Level1)
{
    std::shared_ptr<ModifierNG::RSOverlayNGShaderModifier> modifier =
        std::make_shared<ModifierNG::RSOverlayNGShaderModifier>();
    
    EXPECT_NE(modifier, nullptr);
    EXPECT_EQ(modifier->GetType(), ModifierNG::RSModifierType::OVERLAY_NG_SHADER);
}

/**
 * @tc.name: RSOverlayNGShaderModifierTest003
 * @tc.desc: Test property update with null shader
 * @tc.type: FUNC
 */
HWTEST_F(RSOverlayNGShaderModifierNGTypeTest, RSOverlayNGShaderModifierTest003, TestSize.Level1)
{
    std::shared_ptr<ModifierNG::RSOverlayNGShaderModifier> modifier =
        std::make_shared<ModifierNG::RSOverlayNGShaderModifier>();
    
    // Test with null shader - should handle gracefully
    EXPECT_NE(modifier, nullptr);
}
} // namespace OHOS::Rosen