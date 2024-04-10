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

#include <iostream>
#include <vector>

#include <gtest/gtest.h>
#include <hb.h>

#include "opentype_parser/cmap_parser.h"
#include "param_test_macros.h"
#include "texgine_exception.h"
#include "typeface.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
namespace TextEngine {
struct Mockvars {
    std::shared_ptr<TextEngine::TexgineTypeface> texgineTypeface = std::make_shared<TextEngine::TexgineTypeface>();
} g_fontLoadMockvars;

void InitSystemFontMockVars(Mockvars vars)
{
    g_fontLoadMockvars = std::move(vars);
}

#ifndef USE_ROSEN_DRAWING
std::shared_ptr<TexgineTypeface> TexgineTypeface::MakeFromFile(const std::string &path)
{
    return g_fontLoadMockvars.texgineTypeface;
}
#endif

class SystemFontLoadTest : public testing::Test {
public:
};

/**
 * @tc.name: SystemFontLoad
 * @tc.desc: Verify the SystemFontLoad
 * @tc.type:FUNC
 */
HWTEST_F(SystemFontLoadTest, SystemFontLoad1, TestSize.Level1)
{
    InitSystemFontMockVars({});
    auto typeface = Typeface::MakeFromFile("/system/fonts/NotoSans[wdth,wght].ttf");
    EXPECT_TRUE(typeface != nullptr);
}

/**
 * @tc.name: SystemFontLoad
 * @tc.desc: Verify the SystemFontLoad
 * @tc.type:FUNC
 */
HWTEST_F(SystemFontLoadTest, SystemFontLoad2, TestSize.Level1)
{
    InitSystemFontMockVars({});
    auto typeface = Typeface::MakeFromFile("/system/fonts/NotoSansSymbols2-Regular.ttf");
    EXPECT_TRUE(typeface != nullptr);
}

/**
 * @tc.name: SystemFontLoad
 * @tc.desc: Verify the SystemFontLoad
 * @tc.type:FUNC
 */
HWTEST_F(SystemFontLoadTest, SystemFontLoad3, TestSize.Level1)
{
    InitSystemFontMockVars({});
    auto typeface = Typeface::MakeFromFile("/system/fonts/NotoSansMyanmar[wdth,wght].ttf");
    EXPECT_TRUE(typeface != nullptr);
}

/**
 * @tc.name: SystemFontLoad
 * @tc.desc: Verify the SystemFontLoad
 * @tc.type:FUNC
 */
HWTEST_F(SystemFontLoadTest, SystemFontLoad4, TestSize.Level1)
{
    InitSystemFontMockVars({});
    auto typeface = Typeface::MakeFromFile("/system/fonts/NotoSansThai[wdth,wght].ttf");
    EXPECT_TRUE(typeface != nullptr);
}

/**
 * @tc.name: SystemFontLoad
 * @tc.desc: Verify the SystemFontLoad
 * @tc.type:FUNC
 */
HWTEST_F(SystemFontLoadTest, SystemFontLoad5, TestSize.Level1)
{
    InitSystemFontMockVars({});
    auto typeface = Typeface::MakeFromFile("/system/fonts/NotoSansLao[wdth,wght].ttf");
    EXPECT_TRUE(typeface != nullptr);
}

/**
 * @tc.name: SystemFontLoad
 * @tc.desc: Verify the SystemFontLoad
 * @tc.type:FUNC
 */
HWTEST_F(SystemFontLoadTest, SystemFontLoad6, TestSize.Level1)
{
    InitSystemFontMockVars({});
    auto typeface = Typeface::MakeFromFile("/system/fonts/NotoSansKhmer[wdth,wght].ttf");
    EXPECT_TRUE(typeface != nullptr);
}

/**
 * @tc.name: SystemFontLoad
 * @tc.desc: Verify the SystemFontLoad
 * @tc.type:FUNC
 */
HWTEST_F(SystemFontLoadTest, SystemFontLoad7, TestSize.Level1)
{
    InitSystemFontMockVars({});
    auto typeface = Typeface::MakeFromFile("/system/fonts/NotoSansHebrew[wdth,wght].ttf");
    EXPECT_TRUE(typeface != nullptr);
}

/**
 * @tc.name: SystemFontLoad
 * @tc.desc: Verify the SystemFontLoad
 * @tc.type:FUNC
 */
HWTEST_F(SystemFontLoadTest, SystemFontLoad8, TestSize.Level1)
{
    InitSystemFontMockVars({});
    auto typeface = Typeface::MakeFromFile("/system/fonts/NotoSansGeorgian[wdth,wght].ttf");
    EXPECT_TRUE(typeface != nullptr);
}

/**
 * @tc.name: SystemFontLoad
 * @tc.desc: Verify the SystemFontLoad
 * @tc.type:FUNC
 */
HWTEST_F(SystemFontLoadTest, SystemFontLoad9, TestSize.Level1)
{
    InitSystemFontMockVars({});
    auto typeface = Typeface::MakeFromFile("/system/fonts/NotoSansSinhala[wdth,wght].ttf");
    EXPECT_TRUE(typeface != nullptr);
}

/**
 * @tc.name: SystemFontLoad
 * @tc.desc: Verify the SystemFontLoad
 * @tc.type:FUNC
 */
HWTEST_F(SystemFontLoadTest, SystemFontLoad10, TestSize.Level1)
{
    InitSystemFontMockVars({});
    auto typeface = Typeface::MakeFromFile("/system/fonts/NotoSansDevanagari[wdth,wght].ttf");
    EXPECT_TRUE(typeface != nullptr);
}

/**
 * @tc.name: SystemFontLoad
 * @tc.desc: Verify the SystemFontLoad
 * @tc.type:FUNC
 */
HWTEST_F(SystemFontLoadTest, SystemFontLoad11, TestSize.Level1)
{
    InitSystemFontMockVars({});
    auto typeface = Typeface::MakeFromFile("/system/fonts/NotoSansEthiopic[wdth,wght].ttf");
    EXPECT_TRUE(typeface != nullptr);
}

/**
 * @tc.name: SystemFontLoad
 * @tc.desc: Verify the SystemFontLoad
 * @tc.type:FUNC
 */
HWTEST_F(SystemFontLoadTest, SystemFontLoad12, TestSize.Level1)
{
    InitSystemFontMockVars({});
    auto typeface = Typeface::MakeFromFile("/system/fonts/NotoSansTelugu[wdth,wght].ttf");
    EXPECT_TRUE(typeface != nullptr);
}

/**
 * @tc.name: SystemFontLoad
 * @tc.desc: Verify the SystemFontLoad
 * @tc.type:FUNC
 */
HWTEST_F(SystemFontLoadTest, SystemFontLoad13, TestSize.Level1)
{
    InitSystemFontMockVars({});
    auto typeface = Typeface::MakeFromFile("/system/fonts/NotoSansTamil[wdth,wght].ttf");
    EXPECT_TRUE(typeface != nullptr);
}

/**
 * @tc.name: SystemFontLoad
 * @tc.desc: Verify the SystemFontLoad
 * @tc.type:FUNC
 */
HWTEST_F(SystemFontLoadTest, SystemFontLoad14, TestSize.Level1)
{
    InitSystemFontMockVars({});
    auto typeface = Typeface::MakeFromFile("/system/fonts/NotoSansGujarati[wdth,wght].ttf");
    EXPECT_TRUE(typeface != nullptr);
}

/**
 * @tc.name: SystemFontLoad
 * @tc.desc: Verify the SystemFontLoad
 * @tc.type:FUNC
 */
HWTEST_F(SystemFontLoadTest, SystemFontLoad15, TestSize.Level1)
{
    InitSystemFontMockVars({});
    auto typeface = Typeface::MakeFromFile("/system/fonts/NotoSansKannada[wdth,wght].ttf");
    EXPECT_TRUE(typeface != nullptr);
}

/**
 * @tc.name: SystemFontLoad
 * @tc.desc: Verify the SystemFontLoad
 * @tc.type:FUNC
 */
HWTEST_F(SystemFontLoadTest, SystemFontLoad16, TestSize.Level1)
{
    InitSystemFontMockVars({});
    auto typeface = Typeface::MakeFromFile("/system/fonts/NotoSansMalayalam[wdth,wght].ttf");
    EXPECT_TRUE(typeface != nullptr);
}

/**
 * @tc.name: SystemFontLoad
 * @tc.desc: Verify the SystemFontLoad
 * @tc.type:FUNC
 */
HWTEST_F(SystemFontLoadTest, SystemFontLoad17, TestSize.Level1)
{
    InitSystemFontMockVars({});
    auto typeface = Typeface::MakeFromFile("/system/fonts/NotoSansBengali[wdth,wght].ttf");
    EXPECT_TRUE(typeface != nullptr);
}

/**
 * @tc.name: SystemFontLoad
 * @tc.desc: Verify the SystemFontLoad
 * @tc.type:FUNC
 */
HWTEST_F(SystemFontLoadTest, SystemFontLoad18, TestSize.Level1)
{
    InitSystemFontMockVars({});
    auto typeface = Typeface::MakeFromFile("/system/fonts/NotoSansCJK-Regular.ttc");
    EXPECT_TRUE(typeface != nullptr);
}

} // namespace TextEngine
} // namespace Rosen
} // namespace OHOS
