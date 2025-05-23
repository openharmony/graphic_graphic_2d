/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
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
#include "drawing_text_declaration.h"
#include "gtest/gtest.h"
#include "txt/platform.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
class NativeDrawingRegisterFontTest : public testing::Test {
protected:
    const char* fontFamily_ = "Roboto";
    const char* existFontPath_ = "/system/fonts/Roboto-Regular.ttf";
    const char* notExistFontPath_ = "/system/fonts/Roboto-Regular1.ttf";
};

/*
 * @tc.name: NativeDrawingRegisterFontTest001
 * @tc.desc: test for register font
 * @tc.type: FUNC
 */
HWTEST_F(NativeDrawingRegisterFontTest, NativeDrawingRegisterFontTest001, TestSize.Level1)
{
    OH_Drawing_FontCollection* fontCollection = OH_Drawing_CreateFontCollection();
    uint32_t errorCode = OH_Drawing_RegisterFont(fontCollection, fontFamily_, notExistFontPath_);
    EXPECT_EQ(errorCode, 1);
    errorCode = OH_Drawing_RegisterFont(fontCollection, fontFamily_, existFontPath_);
    EXPECT_EQ(errorCode, 0);
    OH_Drawing_DestroyFontCollection(fontCollection);
}

/*
 * @tc.name: NativeDrawingRegisterFontTest002
 * @tc.desc: test for register font buffer
 * @tc.type: FUNC
 */
HWTEST_F(NativeDrawingRegisterFontTest, NativeDrawingRegisterFontTest002, TestSize.Level1)
{
    OH_Drawing_FontCollection* fontCollection = OH_Drawing_CreateFontCollection();
    std::ifstream fileStream(existFontPath_);
    ASSERT_TRUE(fileStream.is_open());
    fileStream.seekg(0, std::ios::end);
    uint32_t bufferSize = fileStream.tellg();
    fileStream.seekg(0, std::ios::beg);
    std::unique_ptr buffer = std::make_unique<uint8_t[]>(bufferSize);
    fileStream.read(reinterpret_cast<char*>(buffer.get()), bufferSize);
    fileStream.close();
    // 测试有效的数据
    uint32_t result = OH_Drawing_RegisterFontBuffer(fontCollection, fontFamily_, buffer.get(), bufferSize);
    EXPECT_EQ(result, 0);
    uint8_t invalidBuffer[] = { 0, 0, 0, 0, 0 };
    // 测试无效的数据
    result = OH_Drawing_RegisterFontBuffer(fontCollection, fontFamily_, invalidBuffer, sizeof(invalidBuffer));
    OH_Drawing_DestroyFontCollection(fontCollection);
}

/*
 * @tc.name: NativeDrawingRegisterFontTest003
 * @tc.desc: test for nullptr
 * @tc.type: FUNC
 */
HWTEST_F(NativeDrawingRegisterFontTest, NativeDrawingRegisterFontTest003, TestSize.Level1)
{
    // ERROR_NULL_FONT_COLLECTION is 8
    const uint32_t nullFontCollection = 8;
    // ERROR_NULL_FONT_BUFFER is 6
    const uint32_t nullFontBuffer = 6;
    OH_Drawing_FontCollection* fontCollection = OH_Drawing_CreateFontCollection();
    uint32_t result = OH_Drawing_RegisterFontBuffer(fontCollection, nullptr, nullptr, 0);
    EXPECT_EQ(result, nullFontBuffer);
    result = OH_Drawing_RegisterFontBuffer(nullptr, nullptr, nullptr, 0);
    EXPECT_EQ(result, nullFontCollection);
    result = OH_Drawing_RegisterFont(nullptr, nullptr, nullptr);
    EXPECT_EQ(result, nullFontCollection);
    result = OH_Drawing_RegisterFont(fontCollection, nullptr, nullptr);
    EXPECT_EQ(result, nullFontCollection);
    OH_Drawing_DestroyFontCollection(fontCollection);
}

/*
 * @tc.name: NativeDrawingRegisterFontTest004
 * @tc.desc: test for ohosthemefont
 * @tc.type: FUNC
 */
HWTEST_F(NativeDrawingRegisterFontTest, NativeDrawingRegisterFontTest004, TestSize.Level1)
{
    const uint32_t fileCorruption = 9;

    OH_Drawing_FontCollection* fontCollection = OH_Drawing_CreateFontCollection();
    uint32_t result = OH_Drawing_RegisterFont(fontCollection, "ohosthemefont", existFontPath_);
    EXPECT_EQ(result, fileCorruption);

    OH_Drawing_DestroyFontCollection(fontCollection);
}

/*
 * @tc.name: NativeDrawingRegisterFontTest005
 * @tc.desc: test for same ttf with different family name
 * @tc.type: FUNC
 */
HWTEST_F(NativeDrawingRegisterFontTest, NativeDrawingRegisterFontTest005, TestSize.Level1)
{
    OH_Drawing_FontCollection* fontCollection = OH_Drawing_CreateFontCollection();
    uint32_t result = OH_Drawing_RegisterFont(fontCollection, "test1", existFontPath_);
    EXPECT_EQ(result, 0);
    result = OH_Drawing_RegisterFont(fontCollection, "test2", existFontPath_);
    EXPECT_EQ(result, 0);

    OH_Drawing_DestroyFontCollection(fontCollection);
}

/*
 * @tc.name: NativeDrawingRegisterFontTest006
 * @tc.desc: test for unregister font
 * @tc.type: FUNC
 */
HWTEST_F(NativeDrawingRegisterFontTest, NativeDrawingRegisterFontTest006, TestSize.Level1)
{
    OH_Drawing_FontCollection* fontCollection = OH_Drawing_CreateFontCollection();
    uint32_t result = OH_Drawing_RegisterFont(fontCollection, "test1", existFontPath_);
    EXPECT_EQ(result, 0);
    result = OH_Drawing_RegisterFont(fontCollection, "test2", existFontPath_);
    EXPECT_EQ(result, 0);
    EXPECT_EQ(OH_Drawing_UnregisterFont(fontCollection, "test1"), 0);
    EXPECT_EQ(OH_Drawing_UnregisterFont(fontCollection, "test2"), 0);
    EXPECT_EQ(OH_Drawing_UnregisterFont(fontCollection, "test3"), 0);

    OH_Drawing_DestroyFontCollection(fontCollection);
}

/*
 * @tc.name: NativeDrawingRegisterFontTest007
 * @tc.desc: test for unregister font
 * @tc.type: FUNC
 */
HWTEST_F(NativeDrawingRegisterFontTest, NativeDrawingRegisterFontTest007, TestSize.Level1)
{
    // ERROR_NULL_FONT_COLLECTION is 8
    const uint32_t nullFontCollection = 8;
    OH_Drawing_FontCollection* fontCollection = OH_Drawing_CreateFontCollection();
    EXPECT_EQ(OH_Drawing_UnregisterFont(nullptr, ""), nullFontCollection);
    EXPECT_EQ(OH_Drawing_UnregisterFont(nullptr, nullptr), nullFontCollection);
    EXPECT_EQ(OH_Drawing_UnregisterFont(fontCollection, nullptr), nullFontCollection);
    EXPECT_EQ(OH_Drawing_UnregisterFont(fontCollection, ""), nullFontCollection);
    EXPECT_EQ(OH_Drawing_UnregisterFont(fontCollection, Rosen::SPText::OHOS_THEME_FONT), nullFontCollection);
    OH_Drawing_DestroyFontCollection(fontCollection);
}
} // namespace OHOS