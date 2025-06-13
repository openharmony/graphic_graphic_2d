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

#include "gtest/gtest.h"
#include "text_global_config.h"
#include "text/text_blob.h"

using namespace testing;
using namespace testing::ext;
using namespace OHOS::Rosen::Drawing;
namespace OHOS::Rosen::SrvText {
class TextGlobalConfigTest : public testing::Test {};

static uint32_t GetTextHighContrast()
{
    auto& instance = ProcessTextConstrast::Instance();
    return static_cast<uint32_t>(instance.GetTextContrast());
}

/*
* @tc.name: TextHighContrastTest01
* @tc.desc: test for text high contrast mode
* @tc.type: FUNC
*/
HWTEST_F(TextGlobalConfigTest, TextHighContrastTest01, TestSize.Level0)
{
    auto result = TextGlobalConfig::SetTextHighContrast(TEXT_FOLLOW_SYSTEM_HIGH_CONTRAST);
    EXPECT_EQ(result, TEXT_SUCCESS);
    EXPECT_EQ(GetTextHighContrast(), TEXT_FOLLOW_SYSTEM_HIGH_CONTRAST);

    result = TextGlobalConfig::SetTextHighContrast(TEXT_APP_DISABLE_HIGH_CONTRAST);
    EXPECT_EQ(result, TEXT_SUCCESS);
    EXPECT_EQ(GetTextHighContrast(), TEXT_APP_DISABLE_HIGH_CONTRAST);

    result = TextGlobalConfig::SetTextHighContrast(TEXT_APP_ENABLE_HIGH_CONTRAST);
    EXPECT_EQ(result, TEXT_SUCCESS);
    EXPECT_EQ(GetTextHighContrast(), TEXT_APP_ENABLE_HIGH_CONTRAST);
}

/*
* @tc.name: TextHighContrastTest02
* @tc.desc: test for text high contrast mode（Invalid）
* @tc.type: FUNC
*/
HWTEST_F(TextGlobalConfigTest, TextHighContrastTest02, TestSize.Level0)
{
    uint32_t preValue = GetTextHighContrast();
    auto result = TextGlobalConfig::SetTextHighContrast(TEXT_HIGH_CONTRAST_BUTT);
    EXPECT_EQ(result, TEXT_ERR_PARA_INVALID);
    EXPECT_EQ(GetTextHighContrast(), preValue);
}


/*
* @tc.name: TextUndefinedGlyphDisplayTest01
* @tc.desc: test for text high contrast mode（Invalid）
* @tc.type: FUNC
*/
HWTEST_F(TextGlobalConfigTest, TextUndefinedGlyphDisplayTest01, TestSize.Level0)
{
    uint32_t result = TextGlobalConfig::SetTextUndefinedGlyphDisplay(UNDEFINED_GLYPH_USE_DEFAULT);
    EXPECT_EQ(result, 0);
    result = TextGlobalConfig::SetTextUndefinedGlyphDisplay(UNDEFINED_GLYPH_USE_TOFU);
    EXPECT_EQ(result, 0);
    result = TextGlobalConfig::SetTextUndefinedGlyphDisplay(3);
    EXPECT_EQ(result, TEXT_ERR_PARA_INVALID);
}

} // namespace OHOS::Rosen::SrvText