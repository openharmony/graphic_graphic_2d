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

#include "default_symbol_config.h"
#include "gtest/gtest.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
namespace Symbol {
class DefaultSymbolConfigTest : public testing::Test {
public:
    void SetUp() override;
    void TearDown() override;
};

void DefaultSymbolConfigTest::SetUp()
{
    DefaultSymbolConfig::GetInstance()->ParseConfigOfHmSymbol("/system/fonts/hm_symbol_config_next.json");
    auto groups = DefaultSymbolConfig::GetInstance()->GetSymbolLayersGroups(3); // 3 is an existing GlyphID
    ASSERT_TRUE(groups.symbolGlyphId != 0);
}
void DefaultSymbolConfigTest::TearDown()
{
    DefaultSymbolConfig::GetInstance()->Clear();
}

/**
 * @tc.name: GetSymbolLayersGroups001
 * @tc.desc: Test GetSymbolLayersGroups
 * @tc.type: FUNC
 */
HWTEST_F(DefaultSymbolConfigTest, GetSymbolLayersGroups001, TestSize.Level0)
{
    DefaultSymbolConfig::GetInstance()->Clear();
    int result =
        DefaultSymbolConfig::GetInstance()->ParseConfigOfHmSymbol("/system/fonts/hm_symbol_config_next_undefined.json");
    EXPECT_EQ(result, 1);                                                       // 1 means ERROR_CONFIG_NOT_FOUND
    auto groups = DefaultSymbolConfig::GetInstance()->GetSymbolLayersGroups(3); // 3 is an existing GlyphID
    ASSERT_TRUE(groups.symbolGlyphId == 0);
}

/**
 * @tc.name: GetSymbolLayersGroups002
 * @tc.desc: Test GetSymbolLayersGroups
 * @tc.type: FUNC
 */
HWTEST_F(DefaultSymbolConfigTest, GetSymbolLayersGroups002, TestSize.Level0)
{
    DefaultSymbolConfig::GetInstance()->Clear();
    int result = DefaultSymbolConfig::GetInstance()->ParseConfigOfHmSymbol("/system/fonts/hm_symbol_config_next.json");
    EXPECT_EQ(result, 0);                                                       // 0 meas NO_ERROR
    auto groups = DefaultSymbolConfig::GetInstance()->GetSymbolLayersGroups(3); // 3 is an existing GlyphID
    ASSERT_TRUE(groups.symbolGlyphId != 0);
}

/**
 * @tc.name: GetGroupParameters001
 * @tc.desc: Test GetGroupParameters  by scale effect with wholeSymbol or bylayer
 * @tc.type: FUNC
 */
HWTEST_F(DefaultSymbolConfigTest, GetGroupParameters001, TestSize.Level0)
{
    auto parameters = DefaultSymbolConfig::GetInstance()->GetGroupParameters(
        OHOS::Rosen::Drawing::DrawingAnimationType::SCALE_TYPE, 1, 1);
    if (!parameters.empty()) {
        EXPECT_EQ(parameters.size(), 1);
    } else {
        EXPECT_EQ(parameters.empty(), true);
    }

    uint16_t groups = 3; // the 3 is layers of effect
    auto parameters1 = DefaultSymbolConfig::GetInstance()->GetGroupParameters(
        OHOS::Rosen::Drawing::DrawingAnimationType::SCALE_TYPE, groups, 0);
    if (!parameters1.empty()) {
        EXPECT_EQ(parameters1.size(), groups);
    }

    groups = 2; // the 2 is layers of effect
    auto parameters2 = DefaultSymbolConfig::GetInstance()->GetGroupParameters(
        OHOS::Rosen::Drawing::DrawingAnimationType::SCALE_TYPE, groups, 0);
    if (!parameters2.empty()) {
        EXPECT_EQ(parameters2.size(), groups);
    }
}

/**
 * @tc.name: GetGroupParameters002
 * @tc.desc: Test GetGroupParameters by scale effect with UP or DOWN
 * @tc.type: FUNC
 */
HWTEST_F(DefaultSymbolConfigTest, GetGroupParameters002, TestSize.Level0)
{
    uint16_t groups = 1; // the 1 is layers of effect
    auto parameters1 =
        DefaultSymbolConfig::GetInstance()->GetGroupParameters(OHOS::Rosen::Drawing::DrawingAnimationType::SCALE_TYPE,
            groups, 1, OHOS::Rosen::Drawing::DrawingCommonSubType::UP);
    if (!parameters1.empty()) {
        EXPECT_EQ(parameters1.size(), groups);
    }

    auto parameters2 =
        DefaultSymbolConfig::GetInstance()->GetGroupParameters(OHOS::Rosen::Drawing::DrawingAnimationType::SCALE_TYPE,
            groups, 1, OHOS::Rosen::Drawing::DrawingCommonSubType::DOWN);
    if (!parameters2.empty()) {
        EXPECT_EQ(parameters2.size(), groups);
    }
}

/**
 * @tc.name: GetGroupParameters003
 * @tc.desc: Test GetGroupParameters  by scale effect with number of error layers
 * @tc.type: FUNC
 */
HWTEST_F(DefaultSymbolConfigTest, GetGroupParameters003, TestSize.Level0)
{
    uint16_t groups = 300; // the 300 is Out of layers Scope
    auto parameters = DefaultSymbolConfig::GetInstance()->GetGroupParameters(
        OHOS::Rosen::Drawing::DrawingAnimationType::SCALE_TYPE, groups, 0);
    EXPECT_EQ(parameters.empty(), true);
}

/**
 * @tc.name: GetGroupParameters004
 * @tc.desc: Test GetGroupParameters by variable_color effext by cumulative or iteratuve
 * @tc.type: FUNC
 */
HWTEST_F(DefaultSymbolConfigTest, GetGroupParameters004, TestSize.Level0)
{
    uint16_t groups = 3; // the 1 is layers of effect
    auto parameters1 = DefaultSymbolConfig::GetInstance()->GetGroupParameters(
        OHOS::Rosen::Drawing::DrawingAnimationType::VARIABLE_COLOR_TYPE, groups, 1,
        OHOS::Rosen::Drawing::DrawingCommonSubType::UP);
    if (!parameters1.empty()) {
        EXPECT_EQ(parameters1.size(), groups);
    }

    auto parameters2 = DefaultSymbolConfig::GetInstance()->GetGroupParameters(
        OHOS::Rosen::Drawing::DrawingAnimationType::VARIABLE_COLOR_TYPE, groups, 0,
        OHOS::Rosen::Drawing::DrawingCommonSubType::DOWN);
    if (!parameters2.empty()) {
        EXPECT_EQ(parameters2.size(), groups);
    }
}
} // namespace Symbol
} // namespace Rosen
} // namespace OHOS