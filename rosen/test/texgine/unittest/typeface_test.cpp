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
    const char *name_ = "";
    int sizeIndex_ = 0;
    int lengthIndex_ = 0;
    int idIndex_ = 0;
    std::vector<int> tableSize_ = {1};
    std::vector<int> dataLength_ = {1};
    int parseRetval_ = 0;
    std::vector<int> glyphId_ = {0};
    std::shared_ptr<Texgine::TexgineTypeface> texgineTypeface_ = std::make_shared<Texgine::TexgineTypeface>();
    std::unique_ptr<hb_blob_t> blob_ = std::make_unique<hb_blob_t>();
    std::unique_ptr<Texgine::Typeface> typeface_ = nullptr;
} typefaceMockvars;

void InitMyMockVars(Mockvars vars)
{
    typefaceMockvars = std::move(vars);
    typefaceMockvars.typeface_ = std::make_unique<Texgine::Typeface>(typefaceMockvars.texgineTypeface_);
}

std::shared_ptr<TexgineTypeface> TexgineTypeface::MakeFromFile(const char path[], int index)
{
    return typefaceMockvars.texgineTypeface_;
}

size_t TexgineTypeface::GetTableSize(uint32_t tag)
{
    assert(typefaceMockvars.sizeIndex_ < typefaceMockvars.tableSize_.size());
    return typefaceMockvars.tableSize_[typefaceMockvars.sizeIndex_++];
}

size_t TexgineTypeface::GetTableData(uint32_t tag, size_t offset, size_t length,void* data)
{
    return typefaceMockvars.dataLength_[typefaceMockvars.lengthIndex_++];
}

void TexgineTypeface::GetFamilyName(TexgineString* name)
{
    name->SetString(typefaceMockvars.name_);
}

extern "C" {
hb_blob_t * hb_blob_create (const char *data, unsigned int length, hb_memory_mode_t mode,
    void *user_data, hb_destroy_func_t destroy)
{
    return typefaceMockvars.blob_.get();
}

void hb_blob_destroy(hb_blob_t *)
{
}
}

int CmapParser::Parse(const char *data, int32_t size)
{
    return typefaceMockvars.parseRetval_;
}

int32_t CmapParser::GetGlyphId(int32_t codepoint) const
{
    return typefaceMockvars.glyphId_[typefaceMockvars.idIndex_++];
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
    InitMyMockVars({ .name_ = "cxt" });
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
    InitMyMockVars({ .name_ = "cxxt", .texgineTypeface_ = nullptr });
    ASSERT_EXCEPTION(ExceptionType::APIFailed, Typeface::MakeFromFile("aaa"));
    EXPECT_NE(typefaceMockvars.typeface_->GetName(), "cxxt");
}

/**
 * @tc.name: Has1
 * @tc.desc: Verify the Has
 * @tc.type:FUNC
 */
HWTEST_F(TypefaceTest, Has1, TestSize.Level1)
{
    InitMyMockVars({ .name_ = "one", .texgineTypeface_ = nullptr });
    // 0x0006 is codepoint
    EXPECT_EQ(typefaceMockvars.typeface_->Has(0x0006), false);
}

/**
 * @tc.name: Has2
 * @tc.desc: Verify the Has
 * @tc.type:FUNC
 */
HWTEST_F(TypefaceTest, Has2, TestSize.Level1)
{
    InitMyMockVars({ .name_ = "two", .tableSize_ = {0} });
    ASSERT_EXCEPTION(ExceptionType::APIFailed, typefaceMockvars.typeface_->Has(0x0006));
}

/**
 * @tc.name: Has3
 * @tc.desc: Verify the Has
 * @tc.type:FUNC
 */
HWTEST_F(TypefaceTest, Has3, TestSize.Level1)
{
    InitMyMockVars({ .name_ = "three", .tableSize_ = {3}, .dataLength_ = {2} });
    ASSERT_EXCEPTION(ExceptionType::APIFailed, typefaceMockvars.typeface_->Has(0x0006));
}

/**
 * @tc.name: Has4
 * @tc.desc: Verify the Has
 * @tc.type:FUNC
 */
HWTEST_F(TypefaceTest, Has4, TestSize.Level1)
{
    InitMyMockVars({ .name_ = "four", .blob_ = nullptr });
    ASSERT_EXCEPTION(ExceptionType::APIFailed, typefaceMockvars.typeface_->Has(0x0006));
}

/**
 * @tc.name: Has5
 * @tc.desc: Verify the Has
 * @tc.type:FUNC
 */
HWTEST_F(TypefaceTest, Has5, TestSize.Level1)
{
    InitMyMockVars({ .name_ = "five", .parseRetval_ = -1 });
    EXPECT_EQ(typefaceMockvars.typeface_->Has(0x0006), false);
}

/**
 * @tc.name: Has6
 * @tc.desc: Verify the Has
 * @tc.type:FUNC
 */
HWTEST_F(TypefaceTest, Has6, TestSize.Level1)
{
    InitMyMockVars({ .name_ = "six" });
    EXPECT_EQ(typefaceMockvars.typeface_->Has(0x0006), true);
}

/**
 * @tc.name: Has7
 * @tc.desc: Verify the Has
 * @tc.type:FUNC
 */
HWTEST_F(TypefaceTest, Has7, TestSize.Level1)
{
    InitMyMockVars({ .name_ = "seven", .tableSize_ = {1, 1}, .dataLength_ = {1, 1}, .glyphId_ = {0, -1} });
    auto bool1 = typefaceMockvars.typeface_->Has(0x0006);
    auto bool2 = typefaceMockvars.typeface_->Has(0x0006);
    EXPECT_EQ(bool1, true);
    EXPECT_EQ(bool2, false);
    EXPECT_EQ(typefaceMockvars.sizeIndex_, 1);
}

/**
 * @tc.name: Has8
 * @tc.desc: Verify the Has
 * @tc.type:FUNC
 */
HWTEST_F(TypefaceTest, Has8, TestSize.Level1)
{
    InitMyMockVars({ .name_ = "eight", .tableSize_ = {2, 1}, .dataLength_ = {1, 1} });
    ASSERT_EXCEPTION(ExceptionType::APIFailed, typefaceMockvars.typeface_->Has(0x0006));
    auto bool1 = typefaceMockvars.typeface_->Has(0x0006);
    EXPECT_EQ(bool1, true);
    EXPECT_EQ(typefaceMockvars.sizeIndex_, 2);
}

/**
 * @tc.name: Has9
 * @tc.desc: Verify the Has
 * @tc.type:FUNC
 */
HWTEST_F(TypefaceTest, Has9, TestSize.Level1)
{
    InitMyMockVars({ .name_ = "nine", .tableSize_ = {0, 0} });
    ASSERT_EXCEPTION(ExceptionType::APIFailed, typefaceMockvars.typeface_->Has(0x0006));
    ASSERT_EXCEPTION(ExceptionType::APIFailed, typefaceMockvars.typeface_->Has(0x0006));
    EXPECT_EQ(typefaceMockvars.sizeIndex_, 2);
}
} // namespace TextEngine
} // namespace Rosen
} // namespace OHOS
