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
#include "skia_adapter/skia_typeface.h"
#include "text/typeface.h"

using namespace testing;
using namespace testing::ext;

#define HB_TAG(c1,c2,c3,c4) ((uint32_t)((((uint32_t)(c1)&0xFF)<<24)|(((uint32_t)(c2)&0xFF)<<16)|(((uint32_t)(c3)&0xFF)<<8)|((uint32_t)(c4)&0xFF)))

namespace OHOS {
namespace Rosen {
namespace Drawing {
class SkiaTypefaceTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void SkiaTypefaceTest::SetUpTestCase() {}
void SkiaTypefaceTest::TearDownTestCase() {}
void SkiaTypefaceTest::SetUp() {}
void SkiaTypefaceTest::TearDown() {}

/**
 * @tc.name: GetFamilyName001
 * @tc.desc: Test GetFamilyName
 * @tc.type: FUNC
 * @tc.require:I91EDT
 */
HWTEST_F(SkiaTypefaceTest, GetFamilyName001, TestSize.Level1)
{
    std::string s = "asd";
    auto typeface1 = SkiaTypeface::MakeDefault();
    typeface1->GetFamilyName();
}

/**
 * @tc.name: GetFontStyle001
 * @tc.desc: Test GetFontStyle
 * @tc.type: FUNC
 * @tc.require:I91EDT
 */
HWTEST_F(SkiaTypefaceTest, GetFontStyle001, TestSize.Level1)
{
    auto typeface1 = SkiaTypeface::MakeDefault();
    typeface1->GetFontStyle();
}

/**
 * @tc.name: GetTableData001
 * @tc.desc: Test GetTableData
 * @tc.type: FUNC
 * @tc.require:I91EDT
 */
HWTEST_F(SkiaTypefaceTest, GetTableData001, TestSize.Level1)
{
    auto typeface1 = SkiaTypeface::MakeDefault();
    auto tag = HB_TAG('n', 'a', 'm', 'e');
    auto size = typeface1->GetTableSize(tag);
    std::unique_ptr<char[]> tableData = nullptr;
    tableData = std::make_unique<char[]>(size);
    auto retTableData = typeface1->GetTableData(tag, 0, size, tableData.get());
    ASSERT_TRUE(retTableData == size);
}

/**
 * @tc.name: GetItalic001
 * @tc.desc: Test GetItalic
 * @tc.type: FUNC
 * @tc.require:I91EDT
 */
HWTEST_F(SkiaTypefaceTest, GetItalic001, TestSize.Level1)
{
    auto typeface1 = SkiaTypeface::MakeDefault();
    ASSERT_TRUE(!typeface1->GetItalic());
}

/**
 * @tc.name: GetUniqueID001
 * @tc.desc: Test GetUniqueID
 * @tc.type: FUNC
 * @tc.require:I91EDT
 */
HWTEST_F(SkiaTypefaceTest, GetUniqueID001, TestSize.Level1)
{
    auto typeface = SkiaTypeface::MakeDefault();
    ASSERT_TRUE(typeface->GetUniqueID() >= 0);
}

/**
 * @tc.name: GetUnitsPerEm001
 * @tc.desc: Test GetUnitsPerEm
 * @tc.type: FUNC
 * @tc.require:I91EDT
 */
HWTEST_F(SkiaTypefaceTest, GetUnitsPerEm001, TestSize.Level1)
{
    auto typeface = SkiaTypeface::MakeDefault();
    ASSERT_TRUE(typeface->GetUnitsPerEm() >= 0);
}

/**
 * @tc.name: SerializeTypeface001
 * @tc.desc: Test SerializeTypeface
 * @tc.type: FUNC
 * @tc.require:I91EDT
 */
HWTEST_F(SkiaTypefaceTest, SerializeTypeface001, TestSize.Level1)
{
    auto skTypeface = SkTypeface::MakeDefault();
    ASSERT_TRUE(SkiaTypeface::SerializeTypeface(skTypeface.get(), nullptr) != nullptr);
    ASSERT_TRUE(SkiaTypeface::SerializeTypeface(nullptr, nullptr) == nullptr);
}

/**
 * @tc.name: MakeFromName001
 * @tc.desc: Test MakeFromName
 * @tc.type: FUNC
 * @tc.require:I91EDT
 */
HWTEST_F(SkiaTypefaceTest, MakeFromName001, TestSize.Level1)
{
    FontStyle fontStyle;
    ASSERT_TRUE(SkiaTypeface::MakeFromName("familyName", fontStyle) != nullptr);
}

/**
 * @tc.name: MakeFromFile001
 * @tc.desc: Test MakeFromFile
 * @tc.type: FUNC
 * @tc.require:I91EDT
 */
HWTEST_F(SkiaTypefaceTest, MakeFromFile001, TestSize.Level1)
{
    ASSERT_TRUE(SkiaTypeface::MakeFromFile("path", 0) == nullptr);
}
} // namespace Drawing
} // namespace Rosen
} // namespace OHOS