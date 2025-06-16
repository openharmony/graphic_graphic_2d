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
#include <string.h>
#include <securec.h>
#include "gtest/gtest.h"
#include "gtest/hwext/gtest-tag.h"
#include "common/rs_vector4.h"
#include "modifier_ng/rs_modifier_ng_type.h"

#include "message_parcel.h"
#include "property/rs_properties.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Rosen {
class RSModifierNGTypeTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void RSModifierNGTypeTest::SetUpTestCase() {}
void RSModifierNGTypeTest::TearDownTestCase() {}
void RSModifierNGTypeTest::SetUp() {}
void RSModifierNGTypeTest::TearDown() {}

/**
 * @tc.name: GetPropertyTypeString
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSModifierNGTypeTest, GetPropertyTypeString, TestSize.Level1)
{
    EXPECT_EQ(ModifierNG::RSModifierTypeString::GetPropertyTypeString(ModifierNG::RSPropertyType::BOUNDS), "Bounds");
    EXPECT_EQ(ModifierNG::RSModifierTypeString::GetPropertyTypeString(ModifierNG::RSPropertyType::FRAME), "Frame");

    EXPECT_EQ(ModifierNG::RSModifierTypeString::GetPropertyTypeString(ModifierNG::RSPropertyType::INVALID), "Invalid");
    EXPECT_EQ(ModifierNG::RSModifierTypeString::GetPropertyTypeString(static_cast<ModifierNG::RSPropertyType>(-1)), "Unknown");
    // RSPropertyType max value is 153, so test with 154 to ensure it returns "Unknown" for out-of-range values.
    EXPECT_EQ(ModifierNG::RSModifierTypeString::GetPropertyTypeString(static_cast<ModifierNG::RSPropertyType>(154)), "Unknown");
}
}