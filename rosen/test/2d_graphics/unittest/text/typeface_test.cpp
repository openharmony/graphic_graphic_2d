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
#include <fstream>

#include "gtest/gtest.h"

#include "text/typeface.h"
#include "utils/memory_stream.h"

using namespace testing;
using namespace testing::ext;

constexpr uint32_t HB_TAG(char c1, char c2, char c3, char c4)
{
    const uint32_t c1_shift = static_cast<uint32_t>(c1) & 0xFF;
    const uint32_t c2_shift = static_cast<uint32_t>(c2) & 0xFF;
    const uint32_t c3_shift = static_cast<uint32_t>(c3) & 0xFF;
    const uint32_t c4_shift = static_cast<uint32_t>(c4) & 0xFF;
    return (c1_shift << 24) | // 24 is the shift for the first byte
           (c2_shift << 16) | // 16 is the shift for the second byte
           (c3_shift << 8) |  // 8 is the shift for the third byte
           c4_shift;
}

namespace OHOS {
namespace Rosen {
namespace Drawing {
class TypefaceTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();

    inline static std::unique_ptr<char[]> ttfData_;
    inline static size_t ttfLen_;
};

void TypefaceTest::SetUpTestCase()
{
    std::ifstream ttfFile("/system/fonts/Roboto-Regular.ttf", std::ios::in | std::ios::binary);
    ASSERT_TRUE(ttfFile.is_open());
    ttfFile.seekg(0, std::ios::end);
    ttfLen_ = ttfFile.tellg();
    if (ttfLen_ >= INT_MAX) {
        ASSERT_TRUE(false);
    }
    ttfFile.seekg(0, std::ios::beg);
    ttfData_.reset(new char[ttfLen_]);
    ASSERT_NE(ttfData_, nullptr);
    ttfFile.read(ttfData_.get(), ttfLen_);
    ttfFile.close();
}

void TypefaceTest::TearDownTestCase()
{
    ttfData_.reset();
}

/**
 * @tc.name: MakeFromStream001
 * @tc.desc: Test MakeFromStream
 * @tc.type: FUNC
 * @tc.require:I91EDT
 */
HWTEST_F(TypefaceTest, MakeFromStream001, TestSize.Level1)
{
    auto stream = std::make_unique<MemoryStream>(ttfData_.get(), ttfLen_);
    ASSERT_NE(stream, nullptr);
    auto typeface = Typeface::MakeFromStream(std::move(stream));
    ASSERT_NE(typeface, nullptr);
    EXPECT_EQ(typeface->GetFamilyName(), "Roboto");
}

/**
 * @tc.name: MakeFromName001
 * @tc.desc: Test MakeFromName
 * @tc.type: FUNC
 * @tc.require:I91EDT
 */
HWTEST_F(TypefaceTest, MakeFromName001, TestSize.Level1)
{
    FontStyle style;
    std::string familyName = "Arial";
    std::string resultFamilyName = { 0x0048, 0x0061, 0x0072, 0x006D, 0x006F, 0x006E, 0x0079, 0x004F, 0x0053, 0x002D,
        0x0053, 0x0061, 0x006E, 0x0073 };
    auto typeface = Typeface::MakeFromName(familyName.c_str(), style);
    ASSERT_NE(typeface, nullptr);
    EXPECT_EQ(typeface->GetFamilyName(), resultFamilyName);
}

/**
 * @tc.name: MakeClone001
 * @tc.desc: Test MakeClone
 * @tc.type: FUNC
 * @tc.require:I91EDT
 */
HWTEST_F(TypefaceTest, MakeClone001, TestSize.Level1)
{
    auto stream = std::make_unique<MemoryStream>(ttfData_.get(), ttfLen_);
    ASSERT_NE(stream, nullptr);

    auto typeface = Typeface::MakeFromStream(std::move(stream));
    ASSERT_NE(typeface, nullptr);
    FontArguments arg;
    auto clonedTypeface = typeface->MakeClone(arg);
    ASSERT_NE(clonedTypeface, nullptr);
    EXPECT_EQ(clonedTypeface->GetHash(), typeface->GetHash());
}

/**
 * @tc.name: GetTableSize001
 * @tc.desc: Test GetTableSize
 * @tc.type: FUNC
 * @tc.require:I91EDT
 */
HWTEST_F(TypefaceTest, GetTableSize001, TestSize.Level1)
{
    auto stream = std::make_unique<MemoryStream>(ttfData_.get(), ttfLen_);
    ASSERT_NE(stream, nullptr);

    auto typeface = Typeface::MakeFromStream(std::move(stream));
    ASSERT_NE(typeface, nullptr);
    // 'cmap' table size is 4678
    EXPECT_EQ(typeface->GetTableSize(HB_TAG('c', 'm', 'a', 'p')), 4678);
}

/**
 * @tc.name: Serialize001
 * @tc.desc: Test Serialize
 * @tc.type: FUNC
 * @tc.require:I91EDT
 */
HWTEST_F(TypefaceTest, Serialize001, TestSize.Level1)
{
    auto stream = std::make_unique<MemoryStream>(ttfData_.get(), ttfLen_);
    ASSERT_NE(stream, nullptr);

    auto typeface = Typeface::MakeFromStream(std::move(stream));
    ASSERT_NE(typeface, nullptr);
    auto serialized = typeface->Serialize();
    ASSERT_NE(serialized, nullptr);
    EXPECT_EQ(serialized->GetSize(), typeface->GetSize());
}

/**
 * @tc.name: Deserialize001
 * @tc.desc: Test Deserialize
 * @tc.type: FUNC
 * @tc.require:I91EDT
 */
HWTEST_F(TypefaceTest, Deserialize001, TestSize.Level1)
{
    auto stream = std::make_unique<MemoryStream>(ttfData_.get(), ttfLen_);
    ASSERT_NE(stream, nullptr);

    auto typeface = Typeface::MakeFromStream(std::move(stream));
    ASSERT_NE(typeface, nullptr);
    auto serialized = typeface->Serialize();
    ASSERT_NE(serialized, nullptr);
    auto deserialized = Typeface::Deserialize(serialized->GetData(), serialized->GetSize());
    ASSERT_NE(deserialized, nullptr);
    EXPECT_EQ(deserialized->GetFamilyName(), "Roboto");
}

/**
 * @tc.name: NullTests001
 * @tc.desc: Test nullptr
 * @tc.type: FUNC
 * @tc.require:I91EDT
 */
HWTEST_F(TypefaceTest, NullTests001, TestSize.Level1)
{
    auto typeface = std::make_shared<Typeface>(nullptr);
    ASSERT_NE(typeface, nullptr);
    EXPECT_EQ(typeface->GetFamilyName(), "");
    EXPECT_EQ(typeface->GetTableSize(0), 0);
    EXPECT_EQ(typeface->GetTableData(0, 0, 0, nullptr), 0);
    EXPECT_EQ(typeface->GetItalic(), false);
    EXPECT_EQ(typeface->GetUniqueID(), 0);
    EXPECT_EQ(typeface->Serialize(), nullptr);
    char data[10] = { 0 };
    EXPECT_EQ(Typeface::Deserialize(data, 10), nullptr);
}
} // namespace Drawing
} // namespace Rosen
} // namespace OHOS