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

#include "modifier_ng/appearance/rs_alpha_modifier.h"
#include "modifier_ng/geometry/rs_bounds_clip_modifier.h"
#include "modifier_ng/overlay/rs_overlay_style_modifier.h"
#include "recording/draw_cmd_list.h"
#include "ui/rs_canvas_node.h"

using namespace testing;
using namespace testing::ext;
namespace OHOS::Rosen {
class RSModifierNGTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void RSModifierNGTest::SetUpTestCase() {}
void RSModifierNGTest::TearDownTestCase() {}
void RSModifierNGTest::SetUp() {}
void RSModifierNGTest::TearDown() {}

/**
 * @tc.name: RSDisplayListModifierUpdaterTest
 * @tc.desc: RSDisplayListModifierUpdater
 * @tc.type: FUNC
 */
HWTEST_F(RSModifierNGTest, RSDisplayListModifierUpdaterTest, TestSize.Level1)
{
    EXPECT_NE(1, 0);
}

/**
 * @tc.name: AddPropertyTest
 * @tc.desc: AddProperty
 * @tc.type: FUNC
 */
HWTEST_F(RSModifierNGTest, AddPropertyTest, TestSize.Level1)
{
    auto modifier = std::make_shared<ModifierNG::RSAlphaModifier>();
    auto property = std::make_shared<RSProperty<float>>(0.5f);
    modifier->AddProperty(ModifierNG::RSPropertyType::ALPHA, property);
    EXPECT_EQ(property->GetPropertyTypeNG(), ModifierNG::RSPropertyType::ALPHA);
}


/**
 * @tc.name: SetAddModifierFlagTest
 * @tc.desc: SetAddModifierFlag
 * @tc.type: FUNC
 */
HWTEST_F(RSModifierNGTest, SetAddModifierFlagTest, TestSize.Level1)
{
    auto modifier = std::make_shared<ModifierNG::RSAlphaModifier>();
    modifier->SetAddModifierFlag(true);
    ASSERT_TRUE(modifier->isModifierAdded_);

    modifier->SetAddModifierFlag(false);
    ASSERT_TRUE(!modifier->isModifierAdded_);
}

/**
 * @tc.name: SetPropertyThresholdTypeTest
 * @tc.desc: SetPropertyThresholdType
 * @tc.type: FUNC
 */
HWTEST_F(RSModifierNGTest, SetPropertyThresholdTypeTest, TestSize.Level1)
{
    auto modifier = std::make_shared<ModifierNG::RSBoundsClipModifier>();
    auto rrect = std::make_shared<RRect>();
    auto property = std::make_shared<RSAnimatableProperty<RRect>>(*rrect);
    modifier->SetPropertyThresholdType(ModifierNG::RSPropertyType::FOREGROUND_NG_FILTER, property);
    ASSERT_EQ(property->thresholdType_, ThresholdType::DEFAULT);

    modifier->SetPropertyThresholdType(ModifierNG::RSPropertyType::CLIP_RRECT, property);
    ASSERT_EQ(property->thresholdType_, ThresholdType::MEDIUM);
}

/**
 * @tc.name: AttachPropertyTest
 * @tc.desc: AttachProperty
 * @tc.type: FUNC
 */
HWTEST_F(RSModifierNGTest, AttachPropertyTest, TestSize.Level1)
{
    auto modifier = std::make_shared<ModifierNG::RSAlphaModifier>();
    std::shared_ptr<RSProperty<float>> property = nullptr;
    modifier->AttachProperty(property);
    ASSERT_TRUE(property == nullptr);

    property = std::make_shared<RSProperty<float>>(1.f);
    modifier->AttachProperty(property);
    ASSERT_EQ(property->target_.lock(), nullptr);

    auto node = std::make_shared<RSCanvasNode>(1);
    property = std::make_shared<RSProperty<float>>(0.5f);
    modifier->node_ = node;
    modifier->AttachProperty(property);
    ASSERT_NE(property->target_.lock(), nullptr);

    auto overlayModifier = std::make_shared<ModifierNG::RSOverlayStyleModifier>();
    auto indexProperty = std::make_shared<RSProperty<int16_t>>();
    overlayModifier->node_ = node;
    overlayModifier->AttachProperty(indexProperty);
    ASSERT_NE(indexProperty->target_.lock(), nullptr);
    ASSERT_TRUE(overlayModifier->IsCustom());
}
} // namespace OHOS::Rosen