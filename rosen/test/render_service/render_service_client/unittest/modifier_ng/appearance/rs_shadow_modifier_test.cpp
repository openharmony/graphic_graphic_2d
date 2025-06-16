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
#include "modifier_ng/appearance/rs_shadow_modifier.h"

using namespace testing;
using namespace testing::ext;
using namespace OHOS::Rosen::ModifierNG;

namespace OHOS::Rosen {
class RSShadowModifierNGTypeTest : public testing::Test {
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
HWTEST_F(RSShadowModifierNGTypeTest, RSShadowModifierTest, TestSize.Level1)
{

    std::shared_ptr<ModifierNG::RSShadowModifier> modifier = std::make_shared<ModifierNG::RSShadowModifier>();

    EXPECT_EQ(modifier->GetType(), ModifierNG::RSModifierType::SHADOW);

    Color shadowColor(15, 11, 15, 16);
    modifier->SetShadowColor(shadowColor);
    EXPECT_EQ(modifier->GetShadowColor(), shadowColor);

    modifier->SetShadowOffsetX(1.0f);
    EXPECT_EQ(modifier->GetShadowOffsetX(), 1.0f);

    modifier->SetShadowOffsetY(1.0f);
    EXPECT_EQ(modifier->GetShadowOffsetY(), 1.0f);

    modifier->SetShadowAlpha(1.0f);
    EXPECT_EQ(modifier->GetShadowAlpha(), 1.0f);

    modifier->SetShadowElevation(1.0f);
    EXPECT_EQ(modifier->GetShadowElevation(), 1.0f);

    modifier->SetShadowRadius(1.0f);
    EXPECT_EQ(modifier->GetShadowRadius(), 1.0f);

    std::shared_ptr<RSPath> shadowPath = std::make_shared<RSPath>();
    modifier->SetShadowPath(shadowPath);
    EXPECT_EQ(modifier->GetShadowPath(), shadowPath);

    modifier->SetShadowMask(true);
    EXPECT_EQ(modifier->GetShadowMask(), true);

    modifier->SetShadowColorStrategy(1);
    EXPECT_EQ(modifier->GetShadowColorStrategy(), 1);

    modifier->SetShadowIsFilled(true);
    EXPECT_EQ(modifier->GetShadowIsFilled(), true);

    modifier->SetUseShadowBatching(true);
    EXPECT_EQ(modifier->GetUseShadowBatching(), true);
}
} // namespace OHOS::Rosen