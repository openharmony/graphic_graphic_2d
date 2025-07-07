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

#include "common/rs_vector4.h"
#include "modifier_ng/geometry/rs_bounds_clip_modifier.h"

using namespace testing;
using namespace testing::ext;
using namespace OHOS::Rosen::ModifierNG;

namespace OHOS::Rosen {
class RSBoundsClipModifierNGTypeTest : public testing::Test {
public:
    static void SetUpTestCase() {}
    static void TearDownTestCase() {}
    void SetUp() override {}
    void TearDown() override {}
};

/**
 * @tc.name: SetAndGetCornerRadiusTest
 * @tc.desc: Test the set and get functions for the corner radius property
 * @tc.type: FUNC
 */
HWTEST_F(RSBoundsClipModifierNGTypeTest, SetAndGetCornerRadiusTest, TestSize.Level1)
{
    std::shared_ptr<ModifierNG::RSBoundsClipModifier> modifier = std::make_shared<ModifierNG::RSBoundsClipModifier>();

    EXPECT_EQ(modifier->GetType(), ModifierNG::RSModifierType::CLIP_TO_BOUNDS);

    Vector4f cornerRadius(1.0f, 2.0f, 3.0f, 4.0f);
    modifier->SetCornerRadius(cornerRadius);
    EXPECT_EQ(modifier->GetCornerRadius(), cornerRadius);
}

/**
 * @tc.name: SetClipRectWithRadiusTest
 * @tc.desc: Test the function SetClipRectWithRadius
 * @tc.type: FUNC
 */
HWTEST_F(RSBoundsClipModifierNGTypeTest, SetClipRectWithRadiusTest, TestSize.Level1)
{
    auto modifier = std::make_shared<ModifierNG::RSBoundsClipModifier>();
    Vector4f clipRect(100.0f);
    Vector4f clipRadius(50.0f);
    modifier->SetClipRectWithRadius(clipRect, clipRadius);
    EXPECT_TRUE(modifier->HasProperty(ModifierNG::RSPropertyType::CLIP_RRECT));
}

/**
 * @tc.name: SetClipRRectTest
 * @tc.desc: Test the function SetClipRRect
 * @tc.type: FUNC
 */
HWTEST_F(RSBoundsClipModifierNGTypeTest, SetClipRRectTest, TestSize.Level1)
{
    auto modifier = std::make_shared<ModifierNG::RSBoundsClipModifier>();
    auto rrect = std::make_shared<RRect>();
    modifier->SetClipRRect(rrect);
    EXPECT_TRUE(modifier->HasProperty(ModifierNG::RSPropertyType::CLIP_RRECT));
}

/**
 * @tc.name: SetClipBoundsTest
 * @tc.desc: Test the function SetClipBounds
 * @tc.type: FUNC
 */
HWTEST_F(RSBoundsClipModifierNGTypeTest, SetClipBoundsTest, TestSize.Level1)
{
    auto modifier = std::make_shared<ModifierNG::RSBoundsClipModifier>();
    auto clipToBounds = std::make_shared<RSPath>();
    modifier->SetClipBounds(clipToBounds);
    EXPECT_TRUE(modifier->HasProperty(ModifierNG::RSPropertyType::CLIP_BOUNDS));
}

/**
 * @tc.name: SetClipToBoundsTest
 * @tc.desc: Test the function SetClipToBounds
 * @tc.type: FUNC
 */
HWTEST_F(RSBoundsClipModifierNGTypeTest, SetClipToBoundsTest, TestSize.Level1)
{
    auto modifier = std::make_shared<ModifierNG::RSBoundsClipModifier>();
    bool clipToBounds = false;
    modifier->SetClipToBounds(clipToBounds);
    EXPECT_TRUE(modifier->HasProperty(ModifierNG::RSPropertyType::CLIP_TO_BOUNDS));
}
} // namespace OHOS::Rosen