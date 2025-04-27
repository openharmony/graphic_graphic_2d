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
#include "recording/draw_cmd.h"
#include "skia_adapter/skia_typeface.h"

#include "text/text_blob.h"


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
    auto typeface1 = SkiaTypeface::MakeDefault();
    ASSERT_TRUE(typeface1->GetFamilyName() != std::string());
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
    ASSERT_TRUE(typeface1->GetFontStyle().GetWidth() >= 0);
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
 * @tc.name: MakeFromName002
 * @tc.desc: Test MakeFromName
 * @tc.type: FUNC
 * @tc.require:I91EDT
 */
HWTEST_F(SkiaTypefaceTest, MakeFromName002, TestSize.Level1)
{
    FontStyle fontStyle;
    auto typeface = SkiaTypeface::MakeDefault();
    ASSERT_TRUE(typeface != nullptr);
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

/**
 * @tc.name: MakeFromFile002
 * @tc.desc: Test MakeFromFile
 * @tc.type: FUNC
 * @tc.require:I91EDT
 */
HWTEST_F(SkiaTypefaceTest, MakeFromFile002, TestSize.Level1)
{
    auto defaultTypeface = SkiaTypeface::MakeDefault();
    ASSERT_NE(defaultTypeface, nullptr) ;
    const char validPath[] = "resources/fonts/Em.ttf";
    auto typefaceInvalidPath = SkiaTypeface::MakeFromFile("invalid/path/to/font.ttf", 0);
    ASSERT_EQ(typefaceInvalidPath, nullptr);
    auto typefaceInvalidIndex = SkiaTypeface::MakeFromFile(validPath, 99); // 索引99是一个无效的索引
    ASSERT_EQ(typefaceInvalidIndex, nullptr);
}

/**
 * @tc.name: IsCustomTypeface001
 * @tc.desc: Test IsCustomTypeface
 * @tc.type: FUNC
 * @tc.require:I91EDT
 */
HWTEST_F(SkiaTypefaceTest, IsCustomTypeface001, TestSize.Level1)
{
    auto typeface1 = SkiaTypeface::MakeDefault();
    ASSERT_TRUE(!typeface1->IsCustomTypeface());
    ASSERT_TRUE(!typeface1->IsThemeTypeface());
    auto typeface2 = SkiaTypeface(nullptr);
    ASSERT_TRUE(!typeface2.IsCustomTypeface());
    ASSERT_TRUE(!typeface2.IsThemeTypeface());
}

/**
 * @tc.name: GetFamilyName002
 * @tc.desc: Test GetFamilyName
 * @tc.type: FUNC
 * @tc.require:I91EDT
 */
HWTEST_F(SkiaTypefaceTest, GetFamilyName002, TestSize.Level1)
{
    auto typeface = SkiaTypeface::MakeDefault();
    ASSERT_TRUE(typeface != nullptr);
    std::string familyNameFromMethod = typeface->GetFamilyName();
    ASSERT_FALSE(familyNameFromMethod.empty());
}

/**
 * @tc.name: GetFontStyle002
 * @tc.desc: Test GetFontStyle
 * @tc.type: FUNC
 * @tc.require:I91EDT
 */
HWTEST_F(SkiaTypefaceTest, GetFontStyle002, TestSize.Level1)
{
    FontStyle expectedStyle;
    auto typeface = SkiaTypeface::MakeDefault();
    ASSERT_NE(typeface, nullptr);
    FontStyle fontStyle = typeface->GetFontStyle();
    EXPECT_EQ(expectedStyle, fontStyle);
}

/**
 * @tc.name: GetTableSize001
 * @tc.desc: Test GetTableSize
 * @tc.type: FUNC
 * @tc.require:I91EDT
 */
HWTEST_F(SkiaTypefaceTest, GetTableSize001, TestSize.Level1)
{
    auto typeface = std::make_shared<Typeface>(nullptr);
    auto tag = HB_TAG('n', 'a', 'm', 'e');
    auto size = typeface->GetTableSize(tag);
    EXPECT_EQ(size, 0u);
}

/**
 * @tc.name: GetTableData002
 * @tc.desc: Test GetTableData
 * @tc.type: FUNC
 * @tc.require:I91EDT
 */
HWTEST_F(SkiaTypefaceTest, GetTableData002, TestSize.Level1)
{
    auto typeface = std::make_shared<Typeface>(nullptr);
    auto tag = HB_TAG('n', 'a', 'm', 'e');
    auto size = typeface->GetTableSize(tag);
    std::unique_ptr<char[]> tableData = nullptr;
    tableData = std::make_unique<char[]>(size);
    auto retTableData = typeface->GetTableData(tag, 0, size, tableData.get());
    ASSERT_TRUE(retTableData == size);
}

/**
 * @tc.name: GetItalic002
 * @tc.desc: Test GetItalic
 * @tc.type: FUNC
 * @tc.require:I91EDT
 */
HWTEST_F(SkiaTypefaceTest, GetItalic002, TestSize.Level1)
{
    auto typeface = SkiaTypeface::MakeDefault();
    ASSERT_TRUE(typeface != nullptr);
    ASSERT_TRUE(!typeface->GetItalic());
}

/**
 * @tc.name: GetUniqueID002
 * @tc.desc: Test GetUniqueID
 * @tc.type: FUNC
 * @tc.require:I91EDT
 */
HWTEST_F(SkiaTypefaceTest, GetUniqueID002, TestSize.Level1)
{
    auto typeface = std::make_shared<SkiaTypeface>(nullptr);
    ASSERT_TRUE(typeface->GetUniqueID() == 0);
}

/**
 * @tc.name: GetUnitsPerEm002
 * @tc.desc: Test GetUnitsPerEm
 * @tc.type: FUNC
 * @tc.require:I91EDT
 */
HWTEST_F(SkiaTypefaceTest, GetUnitsPerEm002, TestSize.Level1)
{
    auto typeface = std::make_shared<SkiaTypeface>(nullptr);
    ASSERT_TRUE(typeface->GetUnitsPerEm() == 0);
}

/**
 * @tc.name: MakeClone001
 * @tc.desc: Test MakeClone
 * @tc.type: FUNC
 * @tc.require:I91EDT
 */
HWTEST_F(SkiaTypefaceTest, MakeClone001, TestSize.Level1)
{
    auto typeface = SkiaTypeface::MakeDefault();
    ASSERT_TRUE(typeface != nullptr);
    FontArguments skArgs;
    ASSERT_TRUE(typeface->MakeClone(skArgs) != nullptr);
}

/**
 * @tc.name: GetSkTypeface002
 * @tc.desc: Test GetSkTypeface
 * @tc.type: FUNC
 * @tc.require:I91EDT
 */
HWTEST_F(SkiaTypefaceTest, GetSkTypeface002, TestSize.Level1)
{
    auto typeface = SkTypeface::MakeDefault();
    auto skiatypeface = SkiaTypeface(typeface);
    ASSERT_TRUE(typeface != nullptr);
    ASSERT_TRUE(skiatypeface.GetSkTypeface() != nullptr);
}

/**
 * @tc.name: MakeDefault001
 * @tc.desc: Test MakeDefault
 * @tc.type: FUNC
 * @tc.require:I91EDT
 */
HWTEST_F(SkiaTypefaceTest, MakeDefault001, TestSize.Level1)
{
    auto typeface = SkiaTypeface::MakeDefault();
    ASSERT_TRUE(typeface != nullptr);
    ASSERT_TRUE(SkiaTypeface::MakeDefault() != nullptr);
}

/**
 * @tc.name: MakeFromStream001
 * @tc.desc: Test MakeFromStream
 * @tc.type: FUNC
 * @tc.require: I91EDT
 */
HWTEST_F(SkiaTypefaceTest, MakeFromStream001, TestSize.Level1)
{
    std::unique_ptr<MemoryStream> memoryStream = std::make_unique<MemoryStream>();
    uint8_t fontData[] = "Mock font data";
    size_t fontDataSize = sizeof(fontData);
    memoryStream = std::make_unique<MemoryStream>(reinterpret_cast<const void*>(fontData), fontDataSize, true);
    std::shared_ptr<Typeface> typefaceWithData = SkiaTypeface::MakeFromStream(std::move(memoryStream), 0);
    ASSERT_EQ(typefaceWithData, nullptr);
    std::shared_ptr<Typeface> typefaceEmptyStream = SkiaTypeface::MakeFromStream(std::make_unique<MemoryStream>(), 0);
    ASSERT_EQ(typefaceEmptyStream, nullptr);
}

/**
 * @tc.name: MakeFromStream002
 * @tc.desc: Test MakeFromStream
 * @tc.type: FUNC
 * @tc.require: I91EDT
 */
HWTEST_F(SkiaTypefaceTest, MakeFromStream002, TestSize.Level1)
{
    std::unique_ptr<MemoryStream> memoryStream = std::make_unique<MemoryStream>();
    uint8_t fontData[] = "Mock font data";
    size_t fontDataSize = sizeof(fontData);
    memoryStream = std::make_unique<MemoryStream>(reinterpret_cast<const void*>(fontData), fontDataSize, true);
    FontArguments args;
    args.SetCollectionIndex(10);
    std::shared_ptr<Typeface> typefaceWithData = SkiaTypeface::MakeFromStream(std::move(memoryStream), args);
    ASSERT_EQ(typefaceWithData, nullptr);
    std::shared_ptr<Typeface> typefaceEmptyStream = SkiaTypeface::MakeFromStream(nullptr, args);
    ASSERT_EQ(typefaceEmptyStream, nullptr);
}

/**
 * @tc.name: DeserializeTypeface001
 * @tc.desc: Test DeserializeTypeface
 * @tc.type: FUNC
 * @tc.require: I91EDT
 */
HWTEST_F(SkiaTypefaceTest, DeserializeTypeface001, TestSize.Level1)
{
    constexpr size_t SOME_SIZE = 5; // 5 SOME_SIZE
    const uint8_t validData[SOME_SIZE] = { 0x01, 0x02, 0x03, 0x04, 0x05 };
    const uint8_t* dataPtr = validData;
    auto typeface = SkiaTypeface::MakeDefault();
    ASSERT_NE(typeface, nullptr);
    ASSERT_TRUE(dataPtr != nullptr);
    ASSERT_TRUE(
        SkiaTypeface::DeserializeTypeface(reinterpret_cast<const void*>(validData), SOME_SIZE, nullptr) == nullptr);
}

/**
 * @tc.name: SerializeTypeface002
 * @tc.desc: Test SerializeTypeface use case normal situation
 * @tc.type: FUNC
 * @tc.require:I91EDT
 */
HWTEST_F(SkiaTypefaceTest, SerializeTypeface002, TestSize.Level1)
{
    auto skTypeface = SkTypeface::MakeDefault();
    bool isCustomTypeface = true;
    std::shared_ptr<Drawing::Typeface> typeface = nullptr;
    TextBlob::Context customContext { typeface, isCustomTypeface };
    auto serializedData = SkiaTypeface::SerializeTypeface(skTypeface.get(), &customContext);
    ASSERT_TRUE(serializedData != nullptr);
    ASSERT_TRUE(customContext.GetTypeface() == nullptr);
}

/**
 * @tc.name: SerializeTypeface003
 * @tc.desc: Test SerializeTypeface Case Exception
 * @tc.type: FUNC
 * @tc.require:I91EDT
 */
HWTEST_F(SkiaTypefaceTest, SerializeTypeface003, TestSize.Level1)
{
    auto skTypeface = SkTypeface::MakeDefault();
    bool isCustomTypeface = true;
    TextBlob::Context customContext { nullptr, isCustomTypeface };
    auto serializedDataNull = SkiaTypeface::SerializeTypeface(nullptr, &customContext);
    ASSERT_TRUE(serializedDataNull == nullptr);
    ASSERT_TRUE(customContext.GetTypeface() == nullptr);
}

/**
 * @tc.name: Deserialize001
 * @tc.desc: Test Deserialize
 * @tc.type: FUNC
 * @tc.require:I91EDT
 */
HWTEST_F(SkiaTypefaceTest, Deserialize001, TestSize.Level1)
{
    const void* data = reinterpret_cast<const void*>("test_data");
    size_t dataSize = sizeof(data);
    auto typeface = SkiaTypeface::MakeDefault();
    ASSERT_NE(typeface, nullptr);
    ASSERT_TRUE(SkiaTypeface::Deserialize(data, dataSize) == nullptr);
}

/**
 * @tc.name: Serialize001
 * @tc.desc: Test Serialize
 * @tc.type: FUNC
 * @tc.require:I91EDT
 */
HWTEST_F(SkiaTypefaceTest, Serialize001, TestSize.Level1)
{
    auto typeface = SkiaTypeface::MakeDefault();
    ASSERT_NE(typeface, nullptr);
    ASSERT_TRUE(typeface->Serialize() != nullptr);
}
} // namespace Drawing
} // namespace Rosen
} // namespace OHOS