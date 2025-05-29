/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, Hardware
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "drawing_error_code.h"
#include "drawing_memory_stream.h"
#include "drawing_path.h"
#include "drawing_typeface.h"
#include "gtest/gtest.h"

#ifdef RS_ENABLE_VK
#include "platform/ohos/backend/rs_vulkan_context.h"
#endif

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
namespace Drawing {
class NativeDrawingTypefaceTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void NativeDrawingTypefaceTest::SetUpTestCase()
{
#ifdef RS_ENABLE_VK
    RsVulkanContext::SetRecyclable(false);
#endif
}

void NativeDrawingTypefaceTest::TearDownTestCase() {}
void NativeDrawingTypefaceTest::SetUp() {}
void NativeDrawingTypefaceTest::TearDown() {}

/*
 * @tc.name: OH_Drawing_TypefaceCreateFromFile
 * @tc.desc: Test OH_Drawing_TypefaceCreateFromFile
 * @tc.type: FUNC
 */
HWTEST_F(NativeDrawingTypefaceTest, OH_Drawing_TypefaceCreateFromFile, TestSize.Level1)
{
    const char* path = "oom_score_adj";
    ASSERT_TRUE(path != nullptr);
    OH_Drawing_Typeface* typeface = OH_Drawing_TypefaceCreateFromFile(path, 0);
    ASSERT_TRUE(typeface == nullptr);
}

/*
 * @tc.name: OH_Drawing_TypefaceCreateFromFile
 * @tc.desc: Test OH_Drawing_TypefaceCreateFromFile
 * @tc.type: FUNC
 */
HWTEST_F(NativeDrawingTypefaceTest, OH_Drawing_TypefaceCreateFromStream, TestSize.Level1)
{
    char testData[] = "Hello";
    size_t length = sizeof(testData) - 1;
    OH_Drawing_MemoryStream* memoryStream = OH_Drawing_MemoryStreamCreate(testData, length, false);
    ASSERT_TRUE(memoryStream != nullptr);
    OH_Drawing_Typeface* typeface = OH_Drawing_TypefaceCreateFromStream(memoryStream, 1);
    OH_Drawing_TypefaceCreateFromStream(nullptr, 1);
    EXPECT_EQ(OH_Drawing_ErrorCodeGet(), OH_DRAWING_ERROR_INVALID_PARAMETER);
    ASSERT_TRUE(typeface == nullptr);
}

/*
 * @tc.name: OH_Drawing_TypefaceDestroy
 * @tc.desc: Test OH_Drawing_TypefaceDestroy
 * @tc.type: FUNC
 */
HWTEST_F(NativeDrawingTypefaceTest, OH_Drawing_TypefaceDestroy, TestSize.Level1)
{
    OH_Drawing_Typeface* typeface = OH_Drawing_TypefaceCreateDefault();
    ASSERT_TRUE(typeface != nullptr);
    OH_Drawing_TypefaceDestroy(typeface);
    ASSERT_TRUE(typeface != nullptr);
}

/*
 * @tc.name: OH_Drawing_FontArgumentsCreate
 * @tc.desc: Test OH_Drawing_FontArgumentsCreate
 * @tc.type: FUNC
 */
HWTEST_F(NativeDrawingTypefaceTest, OH_Drawing_FontArgumentsCreate, TestSize.Level1)
{
    OH_Drawing_FontArguments *fontArgs = OH_Drawing_FontArgumentsCreate();
    ASSERT_TRUE(fontArgs != nullptr);
}

/*
 * @tc.name: OH_Drawing_FontArgumentsAddVariation
 * @tc.desc: Test OH_Drawing_FontArgumentsAddVariation
 * @tc.type: FUNC
 */
HWTEST_F(NativeDrawingTypefaceTest, OH_Drawing_FontArgumentsAddVariation, TestSize.Level1)
{
    OH_Drawing_FontArguments *fontArgs = OH_Drawing_FontArgumentsCreate();
    ASSERT_TRUE(fontArgs != nullptr);
    OH_Drawing_ErrorCode drawingErrorCode = OH_DRAWING_SUCCESS;
    drawingErrorCode = OH_Drawing_FontArgumentsAddVariation(fontArgs, "wght", 100); // 100 means coordinate value
    EXPECT_EQ(drawingErrorCode, OH_DRAWING_SUCCESS);
    drawingErrorCode = OH_Drawing_FontArgumentsAddVariation(fontArgs, "wght", 10000); // 10000 means coordinate value
    EXPECT_EQ(drawingErrorCode, OH_DRAWING_SUCCESS);
    drawingErrorCode = OH_Drawing_FontArgumentsAddVariation(fontArgs, "wght", 0);
    EXPECT_EQ(drawingErrorCode, OH_DRAWING_SUCCESS);
    drawingErrorCode = OH_Drawing_FontArgumentsAddVariation(fontArgs, "wght", -100); // -100 means coordinate value
    EXPECT_EQ(drawingErrorCode, OH_DRAWING_SUCCESS);
    drawingErrorCode = OH_Drawing_FontArgumentsAddVariation(nullptr, "wght", 100); // 100 means coordinate value
    EXPECT_EQ(drawingErrorCode, OH_DRAWING_ERROR_INVALID_PARAMETER);
    drawingErrorCode = OH_Drawing_FontArgumentsAddVariation(fontArgs, "weight", 100); // 100 means coordinate value
    EXPECT_EQ(drawingErrorCode, OH_DRAWING_ERROR_INVALID_PARAMETER);
    drawingErrorCode = OH_Drawing_FontArgumentsAddVariation(fontArgs, nullptr, 100); // 100 means coordinate value
    EXPECT_EQ(drawingErrorCode, OH_DRAWING_ERROR_INVALID_PARAMETER);
    OH_Drawing_FontArgumentsDestroy(fontArgs);
}

/*
 * @tc.name: OH_Drawing_FontArgumentsDestroy
 * @tc.desc: Test OH_Drawing_FontArgumentsDestroy
 * @tc.type: FUNC
 */
HWTEST_F(NativeDrawingTypefaceTest, OH_Drawing_FontArgumentsDestroy, TestSize.Level1)
{
    OH_Drawing_FontArguments *fontArgs = OH_Drawing_FontArgumentsCreate();
    ASSERT_TRUE(fontArgs != nullptr);
    OH_Drawing_ErrorCode drawingErrorCode = OH_DRAWING_SUCCESS;
    drawingErrorCode = OH_Drawing_FontArgumentsDestroy(nullptr);
    EXPECT_EQ(drawingErrorCode, OH_DRAWING_ERROR_INVALID_PARAMETER);
    drawingErrorCode = OH_Drawing_FontArgumentsDestroy(fontArgs);
    EXPECT_EQ(drawingErrorCode, OH_DRAWING_SUCCESS);
}

/*
 * @tc.name: OH_Drawing_TypefaceCreateFromFileWithArguments
 * @tc.desc: Test OH_Drawing_TypefaceCreateFromFileWithArguments
 * @tc.type: FUNC
 */
HWTEST_F(NativeDrawingTypefaceTest, OH_Drawing_TypefaceCreateFromFileWithArguments, TestSize.Level1)
{
    OH_Drawing_FontArguments *fontArgs = OH_Drawing_FontArgumentsCreate();
    ASSERT_TRUE(fontArgs != nullptr);
    OH_Drawing_ErrorCode drawingErrorCode = OH_DRAWING_SUCCESS;
    drawingErrorCode = OH_Drawing_FontArgumentsAddVariation(fontArgs, "wght", 100); // 100 means coordinate value
    EXPECT_EQ(drawingErrorCode, OH_DRAWING_SUCCESS);
    OH_Drawing_Typeface *typeface =
        OH_Drawing_TypefaceCreateFromFileWithArguments(nullptr, fontArgs);
    ASSERT_TRUE(typeface == nullptr);
    typeface = OH_Drawing_TypefaceCreateFromFileWithArguments("/system/fonts/HarmonyOS_Sans.ttf", nullptr);
    ASSERT_TRUE(typeface == nullptr);
    typeface = OH_Drawing_TypefaceCreateFromFileWithArguments("/system/fonts/HarmonyOS_Sans.ttf", fontArgs);
    ASSERT_TRUE(typeface != nullptr);
    OH_Drawing_Typeface *typeface1 =
        OH_Drawing_TypefaceCreateFromFileWithArguments("/system/fonts/HarmonyOS_Sans_Digit.ttf", fontArgs);
    ASSERT_TRUE(typeface1 != nullptr);
    OH_Drawing_Typeface *typeface2 =
        OH_Drawing_TypefaceCreateFromFileWithArguments("/system/fonts/HarmonyOS_Sans_TC.ttf", fontArgs);
    ASSERT_TRUE(typeface2 != nullptr);

    OH_Drawing_FontArgumentsDestroy(fontArgs);
    OH_Drawing_TypefaceDestroy(typeface);
    OH_Drawing_TypefaceDestroy(typeface1);
    OH_Drawing_TypefaceDestroy(typeface2);
}

/*
 * @tc.name: OH_Drawing_TypefaceCreateFromCurrent
 * @tc.desc: Test OH_Drawing_TypefaceCreateFromCurrent001
 * @tc.type: FUNC
 */
HWTEST_F(NativeDrawingTypefaceTest, OH_Drawing_TypefaceCreateFromCurrent001, TestSize.Level1)
{
    OH_Drawing_FontArguments *fontArgs = OH_Drawing_FontArgumentsCreate();
    ASSERT_TRUE(fontArgs != nullptr);
    OH_Drawing_ErrorCode drawingErrorCode = OH_DRAWING_SUCCESS;
    drawingErrorCode = OH_Drawing_FontArgumentsAddVariation(fontArgs, "wght", 100); // 100 means coordinate value
    EXPECT_EQ(drawingErrorCode, OH_DRAWING_SUCCESS);

    OH_Drawing_Typeface *current = OH_Drawing_TypefaceCreateFromFile("/system/fonts/HarmonyOS_Sans.ttf", 0);
    ASSERT_TRUE(current != nullptr);
    OH_Drawing_Typeface *typeface = OH_Drawing_TypefaceCreateFromCurrent(current, nullptr);
    ASSERT_TRUE(typeface == nullptr);
    typeface = OH_Drawing_TypefaceCreateFromCurrent(nullptr, fontArgs);
    ASSERT_TRUE(typeface == nullptr);
    typeface = OH_Drawing_TypefaceCreateFromCurrent(current, fontArgs);
    ASSERT_TRUE(typeface != nullptr);

    OH_Drawing_FontArgumentsDestroy(fontArgs);
    OH_Drawing_TypefaceDestroy(current);
    OH_Drawing_TypefaceDestroy(typeface);
}

/*
 * @tc.name: OH_Drawing_TypefaceCreateFromCurrent
 * @tc.desc: Test OH_Drawing_TypefaceCreateFromCurrent002
 * @tc.type: FUNC
 */
HWTEST_F(NativeDrawingTypefaceTest, OH_Drawing_TypefaceCreateFromCurrent002, TestSize.Level1)
{
    OH_Drawing_FontArguments *fontArgs = OH_Drawing_FontArgumentsCreate();
    ASSERT_TRUE(fontArgs != nullptr);
    OH_Drawing_ErrorCode drawingErrorCode = OH_DRAWING_SUCCESS;
    drawingErrorCode = OH_Drawing_FontArgumentsAddVariation(fontArgs, "wght", 800); // 800 means coordinate value
    EXPECT_EQ(drawingErrorCode, OH_DRAWING_SUCCESS);

    OH_Drawing_Typeface *current = OH_Drawing_TypefaceCreateFromFile("/system/fonts/HarmonyOS_Sans_Digit.ttf", 0);
    ASSERT_TRUE(current != nullptr);
    OH_Drawing_Typeface *typeface = OH_Drawing_TypefaceCreateFromCurrent(current, fontArgs);
    ASSERT_TRUE(typeface == nullptr);

    OH_Drawing_FontArgumentsDestroy(fontArgs);
    OH_Drawing_TypefaceDestroy(current);
}
} // namespace Drawing
} // namespace Rosen
} // namespace OHOS