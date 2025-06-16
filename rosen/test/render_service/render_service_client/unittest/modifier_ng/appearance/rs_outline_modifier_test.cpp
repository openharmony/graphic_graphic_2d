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
#include "modifier_ng/appearance/rs_outline_modifier.h"

using namespace testing;
using namespace testing::ext;
using namespace OHOS::Rosen::ModifierNG;

namespace OHOS::Rosen {
class RSOutlineModifierNGTypeTest : public testing::Test {
public:
    static void SetUpTestCase() {};
    static void TearDownTestCase() {};
    void SetUp() override {};
    void TearDown() override {};
};

/**
 * @tc.name: RSBorderModifierTest
 * @tc.desc: Test Set/Get functions of RSBorderModifier
 * @tc.type: FUNC
 */
HWTEST_F(RSOutlineModifierNGTypeTest, RSOutlineModifierTest, TestSize.Level1)
{

    std::shared_ptr<ModifierNG::RSOutlineModifier> modifier = std::make_shared<ModifierNG::RSOutlineModifier>();

    EXPECT_EQ(modifier->GetType(), ModifierNG::RSModifierType::OUTLINE);

    Vector4<Color> color = {};
    modifier->SetOutlineColor(color);
    EXPECT_EQ(modifier->GetOutlineColor(), color);

    Vector4f width = { 1.0f, 2.0f, 3.0f, 4.0f };
    modifier->SetOutlineWidth(width);
    EXPECT_EQ(modifier->GetOutlineWidth(), width);

    Vector4<uint32_t> style = { 1, 2, 3, 4 };
    modifier->SetOutlineStyle(style);
    EXPECT_EQ(modifier->GetOutlineStyle(), style);

    Vector4f dashWidth = { 1.2f, 1.3f, 1.4f, 1.5f };
    modifier->SetOutlineDashWidth(dashWidth);
    EXPECT_EQ(modifier->GetOutlineDashWidth(), dashWidth);

    Vector4f dashGap = { 2.2f, 2.3f, 2.1f, 2.0f };
    modifier->SetOutlineDashGap(dashGap);
    EXPECT_EQ(modifier->GetOutlineDashGap(), dashGap);

    Vector4f radius = { 3.2f, 3.3f, 3.1f, 3.0f };
    modifier->SetOutlineRadius(radius);
    EXPECT_EQ(modifier->GetOutlineRadius(), radius);
}
} // namespace OHOS::Rosen