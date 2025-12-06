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

#include <fstream>

#include "drawing_font_collection.h"
#include "drawing_register_font.h"
#include "drawing_text_font_descriptor.h"
#include "drawing_text_declaration.h"
#include "gtest/gtest.h"
#include "txt/platform.h"

using namespace testing;
using namespace testing::ext;

#define SUCCESSED 0
#define ERROR_FILE_NOT_EXISTS 1
#define ERROR_READ_FILE_FAILED 3
#define ERROR_NULL_FONT_BUFFER 6
#define ERROR_NULL_FONT_COLLECTION 8
#define ERROR_FILE_CORRUPTION 9

namespace OHOS {
class NdkRegisterFontIndexTest : public testing::Test {
protected:
    const char* fontFamily_ = "NotoSansCJKjp-Regular-Alphabetic";
    const char* existFontPath_ = "/system/fonts/NotoSansCJK-Regular.ttc";
    const char* notExistFontPath_ = "/system/fonts/Roboto-Regular1.ttf";
    const char* themeFontFamily_ = "ohosthemefont";

    OH_Drawing_FontCollection* fontCollection_ = nullptr;
    std::unique_ptr <uint8_t[]> existFontBuffer_ = nullptr;
    int invalidFontCount = 100;
    uint32_t bufferSize_ = 0;

public:
    void TearDown() override;
    void SetUp() override;
};

void LoadFontBuffers()
{
    std::ifstream fileStream(existFontPath_);
    ASSERT_TRUE(fileStream.is_open());
    fileStream.seekg(0, std::ios::end);
    bufferSize_ = fileStream.tellg();
    fileStream.seekg(0, std::ios::beg);
    existFontBuffer_ = std::make_unique<uint8_t[]>(bufferSize_);
    fileStream.read(reinterpret_cast<char*>(existFontBuffer_.get()), bufferSize_);
    fileStream.close();
}

void NdkRegisterFontIndexTest::TearDown()
{
    if (fontCollection_)
    {
        OH_Drawing_DestroyFontCollection(fontCollection);
    }
}

void NdkRegisterFontIndexTest::SetUp()
{
    fontCollection_ = OH_Drawing_CreateFontCollection();
    ASSERT_NE(fontCollection_, nullptr);
    LoadFontBuffers();
}

/*
 * @tc.name: NdkRegisterFontIndexTest001
 * @tc.desc: test for nullptr
 * @tc.type: FUNC
 */
HWTEST_F(NdkRegisterFontIndexTest, NdkRegisterFontIndexTest001, TestSize.Level0)
{
    uint32_t errorCode = OH_Drawing_RegisterFontBufferByIndex(fontCollection_, nullptr, nullptr, 0, 0);
    EXPECT_EQ(errorCode, ERROR_NULL_FONT_BUFFER);
    errorCode = OH_Drawing_RegisterFontBufferByIndex(nullptr, nullptr, nullptr, 0, 0);
    EXPECT_EQ(errorCode, ERROR_NULL_FONT_COLLECTION);

    errorCode = OH_Drawing_RegisterFontByIndex(nullptr, nullptr, nullptr, 0);
    EXPECT_EQ(errorCode, ERROR_NULL_FONT_COLLECTION);
    errorCode = OH_Drawing_RegisterFontBufferByIndex(fontCollection_, nullptr, nullptr, 0, 0);
    EXPECT_EQ(errorCode, ERROR_NULL_FONT_COLLECTION);
}

/*
 * @tc.name: NdkRegisterFontIndexTest002
 * @tc.desc: test for register font by index
 * @tc.type: FUNC
 */
HWTEST_F(NdkRegisterFontIndexTest, NdkRegisterFontIndexTest002, TestSize.Level0)
{
    uint32_t errorCode = OH_Drawing_RegisterFontByIndex(fontCollection_, fontFamily_, notExistFontPath_, 0);
    EXPECT_EQ(errorCode, ERROR_FILE_NOT_EXISTS);
    errorCode = OH_Drawing_RegisterFontByIndex(fontCollection_, fontFamily_, existFontPath_, 0);
    EXPECT_EQ(errorCode, SUCCESSED);
}

/*
 * @tc.name: NdkRegisterFontIndexTest003
 * @tc.desc: test for the bound of registering font by index
 * @tc.type: FUNC
 */
HWTEST_F(NdkRegisterFontIndexTest, NdkRegisterFontIndexTest003, TestSize.Level0)
{
    uint32_t fontCount = OH_Drawing_GetFontCountFromFile(existFontPath_);
    EXPECT_EQ(fontCount, 10);

    uint32_t errorCode = OH_Drawing_RegisterFontByIndex(fontCollection_, fontFamily_, existFontPath_, fontCount - 1);
    EXPECT_EQ(errorCode, SUCCESSED);

    errorCode = OH_Drawing_RegisterFontByIndex(
        fontCollection_, fontFamily_, existFontPath_, fontCount);
    EXPECT_EQ(errorCode, ERROR_READ_FILE_FAILED);
    errorCode = OH_Drawing_RegisterFontByIndex(
        fontCollection_, fontFamily_, existFontPath_, fontCount + invalidFontCount);
    EXPECT_EQ(errorCode, ERROR_READ_FILE_FAILED);
}

/*
 * @tc.name: NdkRegisterFontIndexTest004
 * @tc.desc: test for ohosthemefont
 * @tc.type: FUNC
 */
HWTEST_F(NdkRegisterFontIndexTest, NdkRegisterFontIndexTest004, TestSize.Level0)
{
    uint32_t result = OH_Drawing_RegisterFontByIndex(fontCollection_, themeFontFamily_, existFontPath_, 0);
    EXPECT_EQ(result, ERROR_FILE_CORRUPTION);
}

/*
 * @tc.name: NdkRegisterFontIndexTest005
 * @tc.desc: test for register font buffer by index
 * @tc.type: FUNC
 */
HWTEST_F(NdkRegisterFontIndexTest, NdkRegisterFontIndexTest005, TestSize.Level0)
{
    std::ifstream fileStream(existFontPath_);
    uint32_t result = OH_Drawing_RegisterFontBufferByIndex(
        fontCollection_, fontFamily_, existFontBuffer_.get(), bufferSize_, 0);
    EXPECT_EQ(result, SUCCESSED);
 
    uint8_t invalidBuffer[] = { 0, 0, 0, 0, 0 };
    result = OH_Drawing_RegisterFontBufferByIndex(
        fontCollection_, fontFamily_, invalidBuffer, sizeof(invalidBuffer), 0);
    EXPECT_EQ(result, ERROR_FILE_CORRUPTION);
}

/*
 * @tc.name: NdkRegisterFontIndexTest006
 * @tc.desc: test for the bound of registering font buffer by index
 * @tc.type: FUNC
 */
HWTEST_F(NdkRegisterFontIndexTest, NdkRegisterFontIndexTest006, TestSize.Level0)
{
    uint32_t fontCount = OH_Drawing_GetFontCountFromBuffer(existFontBuffer_.get(), bufferSize_);
    EXPECT_EQ(fontCount, 10);

    uint32_t errorCode = OH_Drawing_RegisterFontBufferByIndex(
        fontCollection_, fontFamily_, existFontBuffer_.get(), bufferSize_, fontCount - 1);
    EXPECT_EQ(errorCode, SUCCESSED);

    errorCode = OH_Drawing_RegisterFontBufferByIndex(
        fontCollection_, fontFamily_, existFontBuffer_.get(), bufferSize_, fontCount);
    EXPECT_EQ(errorCode, ERROR_READ_FILE_FAILED);
    errorCode = OH_Drawing_RegisterFontBufferByIndex(
        fontCollection_, fontFamily_, existFontBuffer_.get(), bufferSize_, fontCount + invalidFontCount);
    EXPECT_EQ(errorCode, ERROR_READ_FILE_FAILED);
}

/*
 * @tc.name: NdkRegisterFontIndexTest007
 * @tc.desc: test for unregister font
 * @tc.type: FUNC
 */
HWTEST_F(NdkRegisterFontIndexTest, NdkRegisterFontIndexTest007, TestSize.Level0)
{
    uint32_t fontCount = OH_Drawing_GetFontCountFromBuffer(existFontBuffer_.get(), bufferSize_);
    EXPECT_EQ(fontCount, 10);

    uint32_t errorCode = OH_Drawing_RegisterFontBufferByIndex(
        fontCollection_, "test1", existFontBuffer_.get(), bufferSize_, fontCount - 1);
    EXPECT_EQ(errorCode, SUCCESSED);
    uint32_t errorCode = OH_Drawing_RegisterFontByIndex(fontCollection_, "test2", existFontPath_, 0);
    EXPECT_EQ(errorCode, SUCCESSED);
    EXPECT_EQ(OH_Drawing_UnregisterFont(fontCollection_, "test1"), 0);
    EXPECT_EQ(OH_Drawing_UnregisterFont(fontCollection_, "test2"), 0);
}
} // namespace OHOS