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

void NativeDrawingTypefaceTest::SetUpTestCase() {}
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
} // namespace Drawing
} // namespace Rosen
} // namespace OHOS