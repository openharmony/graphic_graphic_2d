/*
 * Copyright (c) 2026 Huawei Device Co., Ltd.
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

#include "custom_symbol_config.h"
#include "gtest/gtest.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
namespace Symbol {
class CustomSymbolConfigTest : public testing::Test {};

/**
 * @tc.name: CustomSymbolConfigTest001
 * @tc.desc: Test ParseConfig with nullptr data
 * @tc.type: FUNC
 */
HWTEST_F(CustomSymbolConfigTest, CustomSymbolConfigTest001, TestSize.Level0)
{
    std::string familyName = "test_family";
    auto result = CustomSymbolConfig::GetInstance()->ParseConfig(familyName, nullptr, 0);
    EXPECT_EQ(result, LoadSymbolErrorCode::LOAD_FAILED);
}

/**
 * @tc.name: CustomSymbolConfigTest002
 * @tc.desc: Test ParseConfig with duplicate family name
 * @tc.type: FUNC
 */
HWTEST_F(CustomSymbolConfigTest, CustomSymbolConfigTest002, TestSize.Level0)
{
    std::string familyName = "test_family_duplicate";
    const char* validJson = R"({
        "symbol_layers_grouping": [
            {
                "native_glyph_id": 1001,
                "symbol_glyph_id": 2001,
                "layers": []
            }
        ]
    })";

    auto result1 = CustomSymbolConfig::GetInstance()->ParseConfig(
        familyName, reinterpret_cast<const uint8_t*>(validJson), strlen(validJson));
    EXPECT_EQ(result1, LoadSymbolErrorCode::SUCCESS);

    auto result2 = CustomSymbolConfig::GetInstance()->ParseConfig(
        familyName, reinterpret_cast<const uint8_t*>(validJson), strlen(validJson));
    EXPECT_EQ(result2, LoadSymbolErrorCode::SUCCESS);
}

/**
 * @tc.name: CustomSymbolConfigTest003
 * @tc.desc: Test ParseConfig with invalid JSON
 * @tc.type: FUNC
 */
HWTEST_F(CustomSymbolConfigTest, CustomSymbolConfigTest003, TestSize.Level0)
{
    std::string familyName = "test_family_invalid_json";
    const char* invalidJson = R"({ invalid json })";

    auto result = CustomSymbolConfig::GetInstance()->ParseConfig(
        familyName, reinterpret_cast<const uint8_t*>(invalidJson), strlen(invalidJson));
    EXPECT_EQ(result, LoadSymbolErrorCode::JSON_ERROR);
}

/**
 * @tc.name: CustomSymbolConfigTest004
 * @tc.desc: Test ParseConfig with empty JSON object
 * @tc.type: FUNC
 */
HWTEST_F(CustomSymbolConfigTest, CustomSymbolConfigTest004, TestSize.Level0)
{
    std::string familyName = "test_family_empty_json";
    const char* emptyJson = R"({})";

    auto result = CustomSymbolConfig::GetInstance()->ParseConfig(
        familyName, reinterpret_cast<const uint8_t*>(emptyJson), strlen(emptyJson));
    EXPECT_EQ(result, LoadSymbolErrorCode::SUCCESS);
}

/**
 * @tc.name: CustomSymbolConfigTest005
 * @tc.desc: Test GetSymbolLayersGroups with non-existent family name
 * @tc.type: FUNC
 */
HWTEST_F(CustomSymbolConfigTest, CustomSymbolConfigTest005, TestSize.Level0)
{
    std::string familyName = "non_existent_family";
    uint16_t glyphId = 1001;

    auto result = CustomSymbolConfig::GetInstance()->GetSymbolLayersGroups(familyName, glyphId);
    EXPECT_EQ(result, std::nullopt);
}

/**
 * @tc.name: CustomSymbolConfigTest006
 * @tc.desc: Test GetSymbolLayersGroups with non-existent glyph ID
 * @tc.type: FUNC
 */
HWTEST_F(CustomSymbolConfigTest, CustomSymbolConfigTest006, TestSize.Level0)
{
    std::string familyName = "test_family_glyph";
    const char* validJson = R"({
        "symbol_layers_grouping": [
            {
                "native_glyph_id": 1001,
                "symbol_glyph_id": 2001,
                "layers": []
            }
        ]
    })";

    auto parseResult = CustomSymbolConfig::GetInstance()->ParseConfig(
        familyName, reinterpret_cast<const uint8_t*>(validJson), strlen(validJson));
    EXPECT_EQ(parseResult, LoadSymbolErrorCode::SUCCESS);

    uint16_t nonExistentGlyphId = 9999;
    auto result = CustomSymbolConfig::GetInstance()->GetSymbolLayersGroups(familyName, nonExistentGlyphId);
    EXPECT_EQ(result, std::nullopt);
}

/**
 * @tc.name: CustomSymbolConfigTest007
 * @tc.desc: Test ParseConfig with valid JSON
 * @tc.type: FUNC
 */
HWTEST_F(CustomSymbolConfigTest, CustomSymbolConfigTest007, TestSize.Level0)
{
    std::string familyName = "test_family_valid";
    const char* validJson = R"({
        "symbol_layers_grouping": [
            {
                "native_glyph_id": 1001,
                "symbol_glyph_id": 2001,
                "layers": [
                    { "components": [1, 2, 3] }
                ]
            }
        ]
    })";

    auto result = CustomSymbolConfig::GetInstance()->ParseConfig(
        familyName, reinterpret_cast<const uint8_t*>(validJson), strlen(validJson));
    EXPECT_EQ(result, LoadSymbolErrorCode::SUCCESS);

    auto layersGroups = CustomSymbolConfig::GetInstance()->GetSymbolLayersGroups(familyName, 1001);
    EXPECT_TRUE(layersGroups.has_value());
}

} // namespace Symbol
} // namespace Rosen
} // namespace OHOS
