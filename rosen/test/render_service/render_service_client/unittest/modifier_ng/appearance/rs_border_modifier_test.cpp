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
#include "modifier_ng/appearance/rs_border_modifier.h"

using namespace testing;
using namespace testing::ext;
using namespace OHOS::Rosen::ModifierNG;

namespace OHOS::Rosen {
class RSBorderModifierNGTypeTest : public testing::Test {
public:
    static void SetUpTestCase() {}
    static void TearDownTestCase() {}
    void SetUp() override {}
    void TearDown() override {}
};

/**
 * @tc.name: RSBorderModifierTest
 * @tc.desc: Test Set/Get functions of RSBorderModifier
 * @tc.type: FUNC
 */
HWTEST_F(RSBorderModifierNGTypeTest, RSBorderModifierTest, TestSize.Level1)
{
    std::shared_ptr<ModifierNG::RSBorderModifier> modifier = std::make_shared<ModifierNG::RSBorderModifier>();

    EXPECT_EQ(modifier->GetType(), ModifierNG::RSModifierType::BORDER);

    Vector4<Color> color;
    modifier->SetBorderColor(color);
    EXPECT_EQ(modifier->GetBorderColor(), color);

    Vector4f width = {1.0f, 2.0f, 3.0f, 4.0f};
    modifier->SetBorderWidth(width);
    EXPECT_EQ(modifier->GetBorderWidth(), width);

    Vector4<uint32_t> style = { 1, 2, 3, 4 };
    modifier->SetBorderStyle(style);
    EXPECT_EQ(modifier->GetBorderStyle(), style);


    Vector4f dashWidth = { 1.0f, 2.0f, 3.0f, 4.0f };
    modifier->SetBorderDashWidth(dashWidth);
    EXPECT_EQ(modifier->GetBorderDashWidth(), dashWidth);


    Vector4f dashGap = { 1.0f, 2.0f, 3.0f, 4.0f };
    modifier->SetBorderDashGap(dashGap);
    EXPECT_EQ(modifier->GetBorderDashGap(), dashGap);
}
} // namespace OHOS::Rosen