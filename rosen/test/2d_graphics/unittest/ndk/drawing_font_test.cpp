/*
 * Copyright (c) 2022-2025 Huawei Device Co., Ltd.
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

#include "gtest/gtest.h"

#include "drawing_font.h"
#include "drawing_typeface.h"
#include "drawing_error_code.h"

#ifdef RS_ENABLE_VK
#include "platform/ohos/backend/rs_vulkan_context.h"
#endif

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
namespace Drawing {
class NativeFontTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void NativeFontTest::SetUpTestCase()
{
#ifdef RS_ENABLE_VK
    RsVulkanContext::SetRecyclable(false);
#endif
}
void NativeFontTest::TearDownTestCase() {}
void NativeFontTest::SetUp() {}
void NativeFontTest::TearDown() {}

/*
 * @tc.name: NativeFontTest_FontFeatures001
 * @tc.desc: test for manipulate fontfeatures.
 * @tc.type: FUNC
 * @tc.require: ICG6L3
 */
HWTEST_F(NativeFontTest, NativeFontTest_OH_Drawing_FontFeatures001, TestSize.Level1)
{
    const char* name = "ccmp";
    const float value = 0;
    OH_Drawing_FontFeatures* features = OH_Drawing_FontFeaturesCreate();
    EXPECT_NE(features, nullptr);
    OH_Drawing_ErrorCode ret = OH_Drawing_FontFeaturesAddFeature(features, name, value);
    EXPECT_EQ(ret, OH_DRAWING_SUCCESS);
    ret = OH_Drawing_FontFeaturesAddFeature(nullptr, name, value);
    EXPECT_EQ(ret, OH_DRAWING_ERROR_INVALID_PARAMETER);
    ret = OH_Drawing_FontFeaturesAddFeature(features, nullptr, value);
    EXPECT_EQ(ret, OH_DRAWING_ERROR_INVALID_PARAMETER);
    
    ret = OH_Drawing_FontFeaturesDestroy(features);
    EXPECT_EQ(ret, OH_DRAWING_SUCCESS);
    ret = OH_Drawing_FontFeaturesDestroy(nullptr);
    EXPECT_EQ(ret, OH_DRAWING_ERROR_INVALID_PARAMETER);
}

/*
 * @tc.name: NativeFontTest_MeasureSingleCharacterWithFeatures001
 * @tc.desc: test for measure single character with features.
 * @tc.type: FUNC
 * @tc.require: ICG6L3
 */
HWTEST_F(NativeFontTest, NativeFontTest_OH_Drawing_MeasureSingleCharacterWithFeatures001, TestSize.Level1)
{
    const char* strOne = "a";
    const char* strTwo = "你好";
    const char* strThree = "";
    float textWidth = 0.f;
    OH_Drawing_FontFeatures* featuresEmpty = OH_Drawing_FontFeaturesCreate();
    EXPECT_NE(featuresEmpty, nullptr);
    OH_Drawing_FontFeatures* featuresOne = OH_Drawing_FontFeaturesCreate();
    EXPECT_NE(featuresOne, nullptr);
    OH_Drawing_ErrorCode ret = OH_Drawing_FontFeaturesAddFeature(featuresOne, "ccmp", 0);
    EXPECT_EQ(ret, OH_DRAWING_SUCCESS);
    OH_Drawing_Font *font = OH_Drawing_FontCreate();
    EXPECT_NE(font, nullptr);
    OH_Drawing_Font *fontWithTypeface = OH_Drawing_FontCreate();
    EXPECT_NE(fontWithTypeface, nullptr);
    OH_Drawing_Typeface* typeface = OH_Drawing_TypefaceCreateFromFile("/system/fonts/HarmonyOS_Sans_SC.ttf", 0);
    EXPECT_NE(typeface, nullptr);
    OH_Drawing_FontSetTypeface(fontWithTypeface, typeface);

    EXPECT_EQ(OH_Drawing_FontMeasureSingleCharacterWithFeatures(font, strOne, featuresEmpty, &textWidth),
        OH_DRAWING_SUCCESS);
    EXPECT_EQ(OH_Drawing_FontMeasureSingleCharacterWithFeatures(font, strTwo, featuresEmpty, &textWidth),
        OH_DRAWING_SUCCESS);
    EXPECT_EQ(OH_Drawing_FontMeasureSingleCharacterWithFeatures(font, strOne, featuresOne, &textWidth),
        OH_DRAWING_SUCCESS);
    EXPECT_EQ(OH_Drawing_FontMeasureSingleCharacterWithFeatures(fontWithTypeface, strOne, featuresOne, &textWidth),
        OH_DRAWING_SUCCESS);
    EXPECT_EQ(OH_Drawing_FontMeasureSingleCharacterWithFeatures(nullptr, strOne, featuresEmpty, &textWidth),
        OH_DRAWING_ERROR_INVALID_PARAMETER);
    EXPECT_EQ(OH_Drawing_FontMeasureSingleCharacterWithFeatures(font, nullptr, featuresEmpty, &textWidth),
        OH_DRAWING_ERROR_INVALID_PARAMETER);
    EXPECT_EQ(OH_Drawing_FontMeasureSingleCharacterWithFeatures(font, strOne, nullptr, &textWidth),
        OH_DRAWING_ERROR_INVALID_PARAMETER);
    EXPECT_EQ(OH_Drawing_FontMeasureSingleCharacterWithFeatures(font, strOne, featuresEmpty, nullptr),
        OH_DRAWING_ERROR_INVALID_PARAMETER);
    EXPECT_EQ(OH_Drawing_FontMeasureSingleCharacterWithFeatures(font, strThree, featuresEmpty, &textWidth),
        OH_DRAWING_ERROR_INVALID_PARAMETER);

    OH_Drawing_FontDestroy(font);
    OH_Drawing_FontDestroy(fontWithTypeface);
    OH_Drawing_FontFeaturesDestroy(featuresOne);
    OH_Drawing_FontFeaturesDestroy(featuresEmpty);
}

} // namespace Drawing
} // namespace Rosen
} // namespace OHOS