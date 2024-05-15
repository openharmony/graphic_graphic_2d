/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
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

#include <cstddef>
#include "gtest/gtest.h"
#include "skia_adapter/skia_hm_symbol_config_ohos.h"
#include "src/ports/skia_ohos/HmSymbolConfig_ohos.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
namespace Drawing {
class SkiaHmSymbolConfigOhosTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void SkiaHmSymbolConfigOhosTest::SetUpTestCase() {}
void SkiaHmSymbolConfigOhosTest::TearDownTestCase() {}
void SkiaHmSymbolConfigOhosTest::SetUp() {}
void SkiaHmSymbolConfigOhosTest::TearDown() {}

/**
 * @tc.name: GetSymbolLayersGroups001
 * @tc.desc: Test GetSymbolLayersGroups
 * @tc.type: FUNC
 * @tc.require: I91EQ7
 */
HWTEST_F(SkiaHmSymbolConfigOhosTest, GetSymbolLayersGroups001, TestSize.Level1)
{
    HmSymbolConfig_OHOS::GetInstance()->ParseConfigOfHmSymbol("hm_symbol_config_next.json",
        SkString("/system/fonts/"));
    auto groups = SkiaHmSymbolConfigOhos::GetSymbolLayersGroups(3); // 3 is an existing GlyphID
    ASSERT_TRUE(groups.symbolGlyphId != 0);
}

/**
 * @tc.name: GetGroupParameters001
 * @tc.desc: Test GetGroupParameters  by scale effect with wholeSymbol or bylayer
 * @tc.type: FUNC
 * @tc.require: I91EQ7
 */
HWTEST_F(SkiaHmSymbolConfigOhosTest, GetGroupParameters001, TestSize.Level1)
{
    auto parameters = SkiaHmSymbolConfigOhos::GetGroupParameters(DrawingAnimationType::SCALE_TYPE, 1, 1);
    if (!parameters.empty()) {
        EXPECT_EQ(parameters.size(), 1);
    } else {
        EXPECT_EQ(parameters.empty(), true);
    }

    uint16_t groups = 3; // the 3 is layers of effect
    auto parameters1 = SkiaHmSymbolConfigOhos::GetGroupParameters(DrawingAnimationType::SCALE_TYPE, groups, 0);
    if (!parameters1.empty()) {
        EXPECT_EQ(parameters1.size(), groups);
    }

    groups = 2; // the 2 is layers of effect
    auto parameters2 = SkiaHmSymbolConfigOhos::GetGroupParameters(DrawingAnimationType::SCALE_TYPE, groups, 0);
    if (!parameters2.empty()) {
        EXPECT_EQ(parameters2.size(), groups);
    }
}

/**
 * @tc.name: GetGroupParameters002
 * @tc.desc: Test GetGroupParameters by scale effect with UP or DOWN
 * @tc.type: FUNC
 * @tc.require: I91EQ7
 */
HWTEST_F(SkiaHmSymbolConfigOhosTest, GetGroupParameters002, TestSize.Level1)
{
    uint16_t groups = 1; // the 1 is layers of effect
    auto parameters1 = SkiaHmSymbolConfigOhos::GetGroupParameters(DrawingAnimationType::SCALE_TYPE, groups, 1,
        DrawingCommonSubType::UP);
    if (!parameters1.empty()) {
        EXPECT_EQ(parameters1.size(), groups);
    }

    auto parameters2 = SkiaHmSymbolConfigOhos::GetGroupParameters(DrawingAnimationType::SCALE_TYPE, groups, 1,
    DrawingCommonSubType::DOWN);
    if (!parameters2.empty()) {
        EXPECT_EQ(parameters2.size(), groups);
    }
}

/**
 * @tc.name: GetGroupParameters003
 * @tc.desc: Test GetGroupParameters  by scale effect with number of error layers
 * @tc.type: FUNC
 * @tc.require: I91EQ7
 */
HWTEST_F(SkiaHmSymbolConfigOhosTest, GetGroupParameters003, TestSize.Level1)
{
    uint16_t groups = 300; // the 300 is Out of layers Scope
    auto parameters = SkiaHmSymbolConfigOhos::GetGroupParameters(DrawingAnimationType::SCALE_TYPE, groups, 0);
    EXPECT_EQ(parameters.empty(), true);
}

/**
 * @tc.name: GetGroupParameters004
 * @tc.desc: Test GetGroupParameters by variable_color effext by cumulative or iteratuve
 * @tc.type: FUNC
 * @tc.require: I91EQ7
 */
HWTEST_F(SkiaHmSymbolConfigOhosTest, GetGroupParameters004, TestSize.Level1)
{
    uint16_t groups = 3; // the 1 is layers of effect
    auto parameters1 = SkiaHmSymbolConfigOhos::GetGroupParameters(DrawingAnimationType::VARIABLE_COLOR_TYPE, groups, 1,
        DrawingCommonSubType::UP);
    if (!parameters1.empty()) {
        EXPECT_EQ(parameters1.size(), groups);
    }

    auto parameters2 = SkiaHmSymbolConfigOhos::GetGroupParameters(DrawingAnimationType::VARIABLE_COLOR_TYPE, groups, 0,
    DrawingCommonSubType::DOWN);
    if (!parameters2.empty()) {
        EXPECT_EQ(parameters2.size(), groups);
    }
}
} // namespace Drawing
} // namespace Rosen
} // namespace OHOS