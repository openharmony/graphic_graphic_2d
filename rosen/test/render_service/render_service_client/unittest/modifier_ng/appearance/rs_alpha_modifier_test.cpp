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

#include "modifier_ng/appearance/rs_alpha_modifier.h"

using namespace testing::ext;
using namespace OHOS::Rosen::ModifierNG;

namespace OHOS::Rosen::ModifierNG {
class RSAlphaModifierNGTypeTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void RSAlphaModifierNGTypeTest::SetUpTestCase() {}
void RSAlphaModifierNGTypeTest::TearDownTestCase() {}
void RSAlphaModifierNGTypeTest::SetUp() {}
void RSAlphaModifierNGTypeTest::TearDown() {}

/**
 * @tc.name: RSAlphaRenderModifierTest
 * @tc.desc:ResetProperties & GetLegacyPropertyApplierMap
 * @tc.type:FUNC
 */
HWTEST_F(RSAlphaModifierNGTypeTest, RSAlphaModifierTest, TestSize.Level1)
{
    std::shared_ptr<ModifierNG::RSAlphaModifier> modifier = std::make_shared<ModifierNG::RSAlphaModifier>();

    EXPECT_EQ(modifier->GetType(), ModifierNG::RSModifierType::ALPHA);
    modifier->SetAlpha(0.75f);
    EXPECT_EQ(modifier->GetAlpha(), 0.75f);

    modifier->SetAlphaOffscreen(true);
    EXPECT_EQ(modifier->GetAlphaOffscreen(), true);
}
} // namespace OHOS::Rosen