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
#include "modifier_ng/appearance/rs_visibility_modifier.h"
#include "property/rs_properties.h"

using namespace testing;
using namespace testing::ext;
using namespace OHOS::Rosen::ModifierNG;

namespace OHOS::Rosen {
class RSVisibilityModifierNGTypeTest : public testing::Test {
public:
    static void SetUpTestCase() {}
    static void TearDownTestCase() {}
    void SetUp() override {}
    void TearDown() override {}
};

/**
 * @tc.name: RSVisibilityModifierTest
 * @tc.desc: Test Set/Get functions of RSVisibilityModifier (Visible)
 * @tc.type: FUNC
 */
HWTEST_F(RSVisibilityModifierNGTypeTest, RSVisibilityModifierTest, TestSize.Level1)
{
    std::shared_ptr<ModifierNG::RSVisibilityModifier> modifier = std::make_shared<ModifierNG::RSVisibilityModifier>();

    EXPECT_EQ(modifier->GetType(), ModifierNG::RSModifierType::VISIBILITY);

    modifier->SetVisible(true);
    EXPECT_EQ(modifier->GetVisible(), true);
}

/**
 * @tc.name: RSVisibilityModifierSetDoubleSidedAndGet
 * @tc.desc: Test SetDoubleSidedEnabled and GetDoubleSidedEnabled
 * @tc.type: FUNC
 * @tc.require: issueIXXXXX
 */
HWTEST_F(RSVisibilityModifierNGTypeTest, RSVisibilityModifierSetDoubleSidedAndGet, TestSize.Level1)
{
    std::shared_ptr<ModifierNG::RSVisibilityModifier> modifier = std::make_shared<ModifierNG::RSVisibilityModifier>();

    EXPECT_EQ(modifier->GetDoubleSidedEnabled(), false);

    modifier->SetDoubleSidedEnabled(true);
    EXPECT_EQ(modifier->GetDoubleSidedEnabled(), true);

    modifier->SetDoubleSidedEnabled(false);
    EXPECT_EQ(modifier->GetDoubleSidedEnabled(), false);
}

/**
 * @tc.name: RSVisibilityModifierResetPropertiesDoubleSided
 * @tc.desc: Test ResetProperties applies default value for DoubleSided
 * @tc.type: FUNC
 * @tc.require: issueIXXXXX
 */
HWTEST_F(RSVisibilityModifierNGTypeTest, RSVisibilityModifierResetPropertiesDoubleSided, TestSize.Level1)
{
    std::shared_ptr<ModifierNG::RSVisibilityModifier> modifier = std::make_shared<ModifierNG::RSVisibilityModifier>();

    RSProperties properties;
    modifier->ResetProperties(properties);
    EXPECT_EQ(properties.GetDoubleSidedEnabled(), true);
}

/**
 * @tc.name: RSVisibilityModifierResetPropertiesDoubleSidedOverride
 * @tc.desc: Test ResetProperties with pre-set value for DoubleSided
 * @tc.type: FUNC
 * @tc.require: issueIXXXXX
 */
HWTEST_F(RSVisibilityModifierNGTypeTest, RSVisibilityModifierResetPropertiesDoubleSidedOverride, TestSize.Level1)
{
    std::shared_ptr<ModifierNG::RSVisibilityModifier> modifier = std::make_shared<ModifierNG::RSVisibilityModifier>();

    modifier->SetDoubleSidedEnabled(false);
    EXPECT_EQ(modifier->GetDoubleSidedEnabled(), false);

    RSProperties properties;
    modifier->ResetProperties(properties);
    EXPECT_EQ(properties.GetDoubleSidedEnabled(), true);
}

/**
 * @tc.name: RSVisibilityModifierTypeStringTestDoubleSided
 * @tc.desc: Test modifier type and property type string for DOUBLE_SIDED
 * @tc.type: FUNC
 * @tc.require: issueIXXXXX
 */
HWTEST_F(RSVisibilityModifierNGTypeTest, RSVisibilityModifierTypeStringTestDoubleSided, TestSize.Level1)
{
    EXPECT_EQ(ModifierNG::RSModifierTypeString::GetModifierTypeString(ModifierNG::RSModifierType::VISIBILITY),
        "Visibility");
    EXPECT_EQ(ModifierNG::RSModifierTypeString::GetPropertyTypeString(ModifierNG::RSPropertyType::DOUBLE_SIDED),
        "DoubleSided");
}
} // namespace OHOS::Rosen