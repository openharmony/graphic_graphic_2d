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
#include <string.h>

#include "gtest/gtest.h"
#include "gtest/hwext/gtest-tag.h"
#include "message_parcel.h"

#include "common/rs_vector4.h"
#include "modifier_ng/background/rs_background_color_render_modifier.h"
#include "property/rs_properties.h"

using namespace testing;
using namespace testing::ext;
using namespace OHOS::Rosen::ModifierNG;

namespace OHOS::Rosen {
class RSBackgroundColorRenderModifierNGTypeTest : public testing::Test {
public:
    static void SetUpTestCase() {}
    static void TearDownTestCase() {}
    void SetUp() override {}
    void TearDown() override {}
};

/**
 * @tc.name: RSBackgroundColorRenderModifierTest
 * @tc.desc: Test GetType & ResetProperties
 * @tc.type: FUNC
 */
HWTEST_F(RSBackgroundColorRenderModifierNGTypeTest, RSBackgroundColorRenderModifierTest, TestSize.Level1)
{
    RSBackgroundColorRenderModifier modifier;
    RSProperties properties;
    EXPECT_EQ(modifier.GetType(), ModifierNG::RSModifierType::BACKGROUND_COLOR);
    modifier.ResetProperties(properties);


    EXPECT_EQ(properties.GetBackgroundColor(), Color());
    EXPECT_EQ(properties.GetBgBrightnessRates(), Vector4f());
    EXPECT_EQ(properties.GetBgBrightnessSaturation(), 0.0f);
    EXPECT_EQ(properties.GetBgBrightnessPosCoeff(), Vector4f());
    EXPECT_EQ(properties.GetBgBrightnessNegCoeff(), Vector4f());
    EXPECT_EQ(properties.GetBgBrightnessFract(), 1.f);
}
} // namespace OHOS::Rosen