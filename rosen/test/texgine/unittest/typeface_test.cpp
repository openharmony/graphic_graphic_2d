/*
 * Copyright (c) 2023 Huawei Device Co., Ltd.
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

struct hb_blob_t {};

namespace OHOS {
namespace Rosen {
namespace TextEngine {
struct Mockvars {
    const char *name = "";
    int sizeIndex = 0;
    int lengthIndex = 0;
    int idIndex = 0;
    std::vector<int> tableSize = {1};
    std::vector<int> dataLength = {1};
    int parseRetval = 0;
    std::vector<int> glyphId = {0};
    std::shared_ptr<TextEngine::TexgineTypeface> texgineTypeface = std::make_shared<TextEngine::TexgineTypeface>();
    std::unique_ptr<hb_blob_t> blob = std::make_unique<hb_blob_t>();
    std::unique_ptr<TextEngine::Typeface> typeface = nullptr;
} g_typefaceMockvars;

void InitMyMockVars(Mockvars vars)
{
    g_typefaceMockvars = std::move(vars);
    g_typefaceMockvars.typeface = std::make_unique<TextEngine::Typeface>(g_typefaceMockvars.texgineTypeface);
}





extern "C" {
hb_blob_t* hb_blob_create(const char* data, unsigned int length, hb_memory_mode_t mode,
    void* user_data, hb_destroy_func_t destroy)
{
    return g_typefaceMockvars.blob.get();
}

void hb_blob_destroy(hb_blob_t*)
{
}
}

int CmapParser::Parse(const char* data, int32_t size)
{
    return g_typefaceMockvars.parseRetval;
}

int32_t CmapParser::GetGlyphId(int32_t codepoint) const
{
    return g_typefaceMockvars.glyphId[g_typefaceMockvars.idIndex++];
}

class TypefaceTest : public testing::Test {
public:
};

/**
 * @tc.name: MakeFromFile1
 * @tc.desc: Verify the MakeFromFile
 * @tc.type:FUNC
 */
HWTEST_F(TypefaceTest, MakeFromFile1, TestSize.Level1)
{
    InitMyMockVars({ .name = "cxt" });
    auto typeface = Typeface::MakeFromFile("aaa");
    EXPECT_NE(typeface.get(), nullptr);
    EXPECT_EQ(typeface->GetName(), "cxt");
}

/**
 * @tc.name: MakeFromFile2
 * @tc.desc: Verify the MakeFromFile
 * @tc.type:FUNC
 */
HWTEST_F(TypefaceTest, MakeFromFile2, TestSize.Level1)
{
    InitMyMockVars({ .name = "cxxt", .texgineTypeface = nullptr });
    ASSERT_EXCEPTION(ExceptionType::API_FAILED, Typeface::MakeFromFile("aaa"));
    EXPECT_NE(g_typefaceMockvars.typeface->GetName(), "cxxt");
}

/**
 * @tc.name: Has1
 * @tc.desc: Verify the Has
 * @tc.type:FUNC
 */
HWTEST_F(TypefaceTest, Has1, TestSize.Level1)
{
    InitMyMockVars({ .name = "one", .texgineTypeface = nullptr });
    // 0x0006 is codepoint
    EXPECT_EQ(g_typefaceMockvars.typeface->Has(0x0006), false);
}

/**
 * @tc.name: Has2
 * @tc.desc: Verify the Has
 * @tc.type:FUNC
 */
HWTEST_F(TypefaceTest, Has2, TestSize.Level1)
{
    InitMyMockVars({ .name = "two", .tableSize = {0} });
    ASSERT_EXCEPTION(ExceptionType::API_FAILED, g_typefaceMockvars.typeface->Has(0x0006));
}

/**
 * @tc.name: Has3
 * @tc.desc: Verify the Has
 * @tc.type:FUNC
 */
HWTEST_F(TypefaceTest, Has3, TestSize.Level1)
{
    InitMyMockVars({ .name = "three", .tableSize = {3}, .dataLength = {2} });
    ASSERT_EXCEPTION(ExceptionType::API_FAILED, g_typefaceMockvars.typeface->Has(0x0006));
}

/**
 * @tc.name: Has4
 * @tc.desc: Verify the Has
 * @tc.type:FUNC
 */
HWTEST_F(TypefaceTest, Has4, TestSize.Level1)
{
    InitMyMockVars({ .name = "four", .blob = nullptr });
    ASSERT_EXCEPTION(ExceptionType::API_FAILED, g_typefaceMockvars.typeface->Has(0x0006));
}

/**
 * @tc.name: Has5
 * @tc.desc: Verify the Has
 * @tc.type:FUNC
 */
HWTEST_F(TypefaceTest, Has5, TestSize.Level1)
{
    InitMyMockVars({ .name = "five", .parseRetval = -1 });
    EXPECT_EQ(g_typefaceMockvars.typeface->Has(0x0006), false);
}

/**
 * @tc.name: Has6
 * @tc.desc: Verify the Has
 * @tc.type:FUNC
 */
HWTEST_F(TypefaceTest, Has6, TestSize.Level1)
{
    InitMyMockVars({ .name = "six" });
    EXPECT_EQ(g_typefaceMockvars.typeface->Has(0x0006), true);
}

/**
 * @tc.name: Has7
 * @tc.desc: Verify the Has
 * @tc.type:FUNC
 */
HWTEST_F(TypefaceTest, Has7, TestSize.Level1)
{
    InitMyMockVars({ .name = "seven", .tableSize = {1, 1}, .dataLength = {1, 1}, .glyphId = {0, -1} });
    auto bool1 = g_typefaceMockvars.typeface->Has(0x0006);
    auto bool2 = g_typefaceMockvars.typeface->Has(0x0006);
    EXPECT_EQ(bool1, true);
    EXPECT_EQ(bool2, false);
    EXPECT_EQ(g_typefaceMockvars.sizeIndex, 1);
}

/**
 * @tc.name: Has8
 * @tc.desc: Verify the Has
 * @tc.type:FUNC
 */
HWTEST_F(TypefaceTest, Has8, TestSize.Level1)
{
    InitMyMockVars({ .name = "eight", .tableSize = {2, 1}, .dataLength = {1, 1} });
    ASSERT_EXCEPTION(ExceptionType::API_FAILED, g_typefaceMockvars.typeface->Has(0x0006));
    auto bool1 = g_typefaceMockvars.typeface->Has(0x0006);
    EXPECT_EQ(bool1, true);
    EXPECT_EQ(g_typefaceMockvars.sizeIndex, 2);
}

/**
 * @tc.name: Has9
 * @tc.desc: Verify the Has
 * @tc.type:FUNC
 */
HWTEST_F(TypefaceTest, Has9, TestSize.Level1)
{
    InitMyMockVars({ .name = "nine", .tableSize = {0, 0} });
    ASSERT_EXCEPTION(ExceptionType::API_FAILED, g_typefaceMockvars.typeface->Has(0x0006));
    ASSERT_EXCEPTION(ExceptionType::API_FAILED, g_typefaceMockvars.typeface->Has(0x0006));
    EXPECT_EQ(g_typefaceMockvars.sizeIndex, 2);
}
} // namespace TextEngine
} // namespace Rosen
} // namespace OHOS
