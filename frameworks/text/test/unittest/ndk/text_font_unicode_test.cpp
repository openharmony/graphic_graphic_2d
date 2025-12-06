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

#define OH_DRAWING_SUCCESS 0
#define OH_DRAWING_ERROR_INCORRECT_PARAMETER 26200004

namespace OHOS {
class NdkFontUnicodeTest : public testing::Test {
protected:
    const char* ttcFontPath_ = "/system/fonts/NotoSansCJK-Regular.ttc";
    const char* ttfFontPath_ = "/system/fonts/Roboto-Regular.ttf";
    const char* notExistFontPath_ = "/system/fonts/Roboto-Regular1.ttf";
    const char* themeFontFamily_ = "ohosthemefont";

    int invalidFontCount = 100;
    int32_t* unicodeArray = nullptr;
    int32_t arrayLength = 0;

    OH_Drawing_FontCollection* fontCollection_ = nullptr;
    std::unique_ptr <uint8_t[]> existFontBuffer_ = nullptr;
    std::vector<uint8_t> invalidBuffer_ = {};

    uint32_t bufferSize_ = 0;

public:
public:
    void TearDown() override;
    void SetUp() override;
};

void LoadFontBuffers()
{
    std::ifstream fileStream(ttcFontPath_);
    ASSERT_TRUE(fileStream.is_open());
    fileStream.seekg(0, std::ios::end);
    bufferSize_ = fileStream.tellg();
    fileStream.seekg(0, std::ios::beg);
    existFontBuffer_ = std::make_unique<uint8_t[]>(bufferSize_);
    fileStream.read(reinterpret_cast<char*>(existFontBuffer_.get()), bufferSize_);
    fileStream.close();
}

void NdkFontUnicodeTest::TearDown() override
{
    if (fontCollection_ != nullptr) {
        OH_Drawing_DestroyFontCollection(fontCollection);
    }

    if (unicodeArray != nullptr) {
        std::free(unicodeArray);
    }
}

void NdkFontUnicodeTest::SetUp() override
{
    fontCollection_ = OH_Drawing_CreateFontCollection();
    ASSERT_NE(fontCollection_, nullptr);
    LoadFontBuffers();
    invalidBuffer_ = std::vector<uint8_t>(invalidFontCount, 0xFF);
}

/*
* @tc.name: NdkFontUnicodeTest001
* @tc.desc: test for getting unicode array from file
* @tc.type: FUNC
*/
HWTEST_F(NdkFontUnicodeTest, NdkFontUnicodeTest001, TestSize.Level0)
{
    OH_Drawing_ErrorCode errorCode = OH_Drawing_GetFontUnicodeArrayFromFile(
        ttcFontPath_, 0, &unicodeArray, &arrayLength);
    EXPECT_EQ(errorCode, OH_DRAWING_SUCCESS);
    EXPECT_NE(unicodeArray, nullptr);
    EXPECT_NE(arrayLength, 0);

    errorCode = OH_Drawing_GetFontUnicodeArrayFromFile(
        ttfFontPath_, 0, &unicodeArray, &arrayLength);
    EXPECT_EQ(errorCode, OH_DRAWING_SUCCESS);
    EXPECT_NE(unicodeArray, nullptr);
    EXPECT_NE(arrayLength, 0);
}

/*
* @tc.name: NdkFontUnicodeTest002
* @tc.desc: test for getting unicode array from buffer
* @tc.type: FUNC
*/
HWTEST_F(NdkFontUnicodeTest, NdkFontUnicodeTest002, TestSize.Level0)
{
    OH_Drawing_ErrorCode errorCode = OH_Drawing_GetFontUnicodeArrayFromBuffer(
        existFontBuffer_.get(), bufferSize_, 0, &unicodeArray, &arrayLength);
    EXPECT_EQ(errorCode, OH_DRAWING_SUCCESS);
    EXPECT_NE(unicodeArray, nullptr);
    EXPECT_NE(arrayLength, 0);
}

/*
* @tc.name: NdkFontUnicodeTest003
* @tc.desc: test for nullptr
* @tc.type: FUNC
*/
HWTEST_F(NdkFontUnicodeTest, NdkFontUnicodeTest003, TestSize.Level0)
{
    OH_Drawing_ErrorCode errorCode = OH_Drawing_GetFontUnicodeArrayFromFile(
        nullptr, 0, &unicodeArray, &arrayLength);
    EXPECT_EQ(errorCode, OH_DRAWING_ERROR_INCORRECT_PARAMETER);
    EXPECT_EQ(unicodeArray, nullptr);
    EXPECT_EQ(arrayLength, 0);

    errorCode = OH_Drawing_GetFontUnicodeArrayFromBuffer(
        nullptr, invalidFontCount, 0, &unicodeArray, &arrayLength);
    EXPECT_EQ(errorCode, OH_DRAWING_ERROR_INCORRECT_PARAMETER);
    EXPECT_EQ(unicodeArray, nullptr);
    EXPECT_EQ(arrayLength, 0);
}

/*
* @tc.name: NdkFontUnicodeTest004
* @tc.desc: test for getting font count from file or buffer
* @tc.type: FUNC
*/
HWTEST_F(NdkFontUnicodeTest, NdkFontUnicodeTest004, TestSize.Level0)
{
    uint32_t countFromFile = OH_Drawing_GetFontCountFromFile(ttcFontPath_);
 
    ASSERT_NE(existFontBuffer_, nullptr);
    EXPECT_NE(bufferSize_, 0);
    uint32_t countFromBuffer = OH_Drawing_GetFontCountFromBuffer(
        existFontBuffer_.get(), bufferSize_);

    EXPECT_EQ(countFromFile, countFromBuffer);
    EXPECT_EQ(countFromFile, 10);
    EXPECT_EQ(countFromBuffer, 10);
}

/*
* @tc.name: NdkFontUnicodeTest005
* @tc.desc: test for nullptr
* @tc.type: FUNC
*/
HWTEST_F(NdkFontUnicodeTest, NdkFontUnicodeTest005, TestSize.Level0)
{
    uint32_t countFromFile = OH_Drawing_GetFontCountFromFile(nullptr);
    EXPECT_EQ(countFromFile, 0);
    uint32_t countFromBuffer = OH_Drawing_GetFontCountFromBuffer(nullptr, invalidFontCount);
    EXPECT_EQ(countFromBuffer, 0);

    countFromFile = OH_Drawing_GetFontCountFromFile(notExistFontPath_);
    EXPECT_EQ(countFromFile, 0);
    countFromBuffer = OH_Drawing_GetFontCountFromBuffer(invalidBuffer_.data(), invalidFontCount);
    EXPECT_EQ(countFromBuffer, 0);
}
} // namespace OHOS