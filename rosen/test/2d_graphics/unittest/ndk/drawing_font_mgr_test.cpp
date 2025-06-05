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

#include "gtest/gtest.h"
#include "drawing_font_mgr.h"
#include "drawing_text_typography.h"
#include "drawing_typeface.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
namespace Drawing {
class OH_Drawing_FontMgrTest : public testing::Test {
};

/*
 * @tc.name: OH_Drawing_FontMgrTest001
 * @tc.desc: test for creating and destroying font manager.
 * @tc.type: FUNC
 */
HWTEST_F(OH_Drawing_FontMgrTest, OH_Drawing_FontMgrTest001, TestSize.Level1)
{
    OH_Drawing_FontMgr *mgr = OH_Drawing_FontMgrCreate();
    EXPECT_NE(mgr, nullptr);
    OH_Drawing_FontMgrDestroy(mgr);
    OH_Drawing_FontMgrDestroy(nullptr);
}

/*
 * @tc.name: OH_Drawing_FontMgrTest002
 * @tc.desc: test for getting family name.
 * @tc.type: FUNC
 */
HWTEST_F(OH_Drawing_FontMgrTest, OH_Drawing_FontMgrTest002, TestSize.Level1)
{
    OH_Drawing_FontMgr *mgr = OH_Drawing_FontMgrCreate();
    EXPECT_NE(mgr, nullptr);
    int count = OH_Drawing_FontMgrGetFamilyCount(mgr);
    EXPECT_EQ(count, 10);

    char *familyName = OH_Drawing_FontMgrGetFamilyName(mgr, 0);
    std::string result = { 0x48, 0x61, 0x72, 0x6d, 0x6f, 0x6e, 0x79, 0x4f, 0x53, 0x2d, 0x53, 0x61, 0x6e, 0x73 };
    EXPECT_EQ(std::string(familyName), result);
    OH_Drawing_FontMgrDestroyFamilyName(familyName);

    OH_Drawing_FontMgrDestroy(mgr);
}

/*
 * @tc.name: OH_Drawing_FontMgrTest003
 * @tc.desc: test for creating and destroying font style set by font mannager.
 * @tc.type: FUNC
 */
HWTEST_F(OH_Drawing_FontMgrTest, OH_Drawing_FontMgrTest003, TestSize.Level1)
{
    OH_Drawing_FontMgr* mgr = OH_Drawing_FontMgrCreate();
    EXPECT_NE(mgr, nullptr);

    OH_Drawing_FontStyleSet* fontStyleSet = OH_Drawing_FontMgrCreateFontStyleSet(mgr, 0);
    OH_Drawing_FontMgrCreateFontStyleSet(reinterpret_cast<OH_Drawing_FontMgr*>(1), 0);
    EXPECT_NE(fontStyleSet, nullptr);
    OH_Drawing_FontMgrDestroyFontStyleSet(fontStyleSet);
    OH_Drawing_FontMgrDestroyFontStyleSet(nullptr);

    OH_Drawing_FontMgrDestroy(mgr);
}

/*
 * @tc.name: OH_Drawing_FontMgrTest004
 * @tc.desc: test for matching font family by family name.
 * @tc.type: FUNC
 */
HWTEST_F(OH_Drawing_FontMgrTest, OH_Drawing_FontMgrTest004, TestSize.Level1)
{
    OH_Drawing_FontMgr *mgr = OH_Drawing_FontMgrCreate();
    EXPECT_NE(mgr, nullptr);
    const char* matchFamilyName = "HarmonyOS-Sans";
    OH_Drawing_FontStyleSet* fontStyleSet = OH_Drawing_FontMgrMatchFamily(mgr, matchFamilyName);
    EXPECT_NE(fontStyleSet, nullptr);
    EXPECT_EQ(OH_Drawing_FontStyleSetCount(fontStyleSet), 2);
    OH_Drawing_FontMgrDestroyFontStyleSet(fontStyleSet);

    OH_Drawing_FontMgrDestroy(mgr);
}


/*
 * @tc.name: OH_Drawing_FontMgrTest005
 * @tc.desc: test for matching font typeface by family name and font style.
 * @tc.type: FUNC
 */
HWTEST_F(OH_Drawing_FontMgrTest, OH_Drawing_FontMgrTest005, TestSize.Level1)
{
    OH_Drawing_FontMgr *mgr = OH_Drawing_FontMgrCreate();
    EXPECT_NE(mgr, nullptr);
    const char* matchFamilyName = "HarmonyOS-Sans";
    OH_Drawing_FontStyleStruct normalStyle;
    normalStyle.weight = FONT_WEIGHT_400;
    normalStyle.width = FONT_WIDTH_NORMAL;
    normalStyle.slant = FONT_STYLE_NORMAL;
    OH_Drawing_Typeface *typeface = OH_Drawing_FontMgrMatchFamilyStyle(mgr, matchFamilyName, normalStyle);
    EXPECT_NE(typeface, nullptr);
    OH_Drawing_TypefaceDestroy(typeface);

    typeface = OH_Drawing_FontMgrMatchFamilyStyle(mgr, nullptr, normalStyle);
    EXPECT_NE(typeface, nullptr);
    OH_Drawing_FontMgrDestroy(mgr);
}

/*
 * @tc.name: OH_Drawing_FontMgrTest006
 * @tc.desc: test for matching font typeface by family name, font style and specific character.
 * @tc.type: FUNC
 */
HWTEST_F(OH_Drawing_FontMgrTest, OH_Drawing_FontMgrTest006, TestSize.Level1)
{
    OH_Drawing_FontMgr *mgr = OH_Drawing_FontMgrCreate();
    EXPECT_NE(mgr, nullptr);

    const char* matchFamilyName = "HarmonyOS-Sans";
    OH_Drawing_FontStyleStruct normalStyle;
    normalStyle.weight = FONT_WEIGHT_400;
    normalStyle.width = FONT_WIDTH_NORMAL;
    normalStyle.slant = FONT_STYLE_NORMAL;

    const char *bcp47[] = {"zh-Hans", "zh-CN"};
    OH_Drawing_Typeface *typeface = OH_Drawing_FontMgrMatchFamilyStyleCharacter(mgr, matchFamilyName,
                                                                                    normalStyle, bcp47, 1, ' ');
    EXPECT_NE(typeface, nullptr);
    OH_Drawing_TypefaceDestroy(typeface);

    OH_Drawing_FontMgrDestroy(mgr);
}

/*
 * @tc.name: OH_Drawing_FontMgrTest007
 * @tc.desc: test for getting family name.
 * @tc.type: FUNC
 */
HWTEST_F(OH_Drawing_FontMgrTest, OH_Drawing_FontMgrTest007, TestSize.Level1)
{
    int count = OH_Drawing_FontMgrGetFamilyCount(nullptr);
    EXPECT_EQ(count, 0);
    OH_Drawing_FontMgr *mgr = OH_Drawing_FontMgrCreate();
    EXPECT_NE(mgr, nullptr);
    char *familyName = OH_Drawing_FontMgrGetFamilyName(nullptr, 0);
    EXPECT_EQ(familyName, nullptr);
    OH_Drawing_FontMgrDestroyFamilyName(familyName);
    OH_Drawing_FontMgrDestroy(mgr);
}

/*
 * @tc.name: OH_Drawing_FontMgrTest008
 * @tc.desc: test for matching family style.
 * @tc.type: FUNC
 */
HWTEST_F(OH_Drawing_FontMgrTest, OH_Drawing_FontMgrTest008, TestSize.Level1)
{
    OH_Drawing_FontStyleSet* fontStyleSet = OH_Drawing_FontMgrCreateFontStyleSet(nullptr, 0);
    EXPECT_EQ(fontStyleSet, nullptr);
    const char* matchFamilyName = "HarmonyOS-Sans";
    fontStyleSet = OH_Drawing_FontMgrMatchFamily(nullptr, matchFamilyName);
    EXPECT_EQ(fontStyleSet, nullptr);

    OH_Drawing_FontStyleStruct normalStyle;
    normalStyle.weight = FONT_WEIGHT_400;
    normalStyle.width = FONT_WIDTH_NORMAL;
    normalStyle.slant = FONT_STYLE_NORMAL;
    OH_Drawing_Typeface *typeface = OH_Drawing_FontMgrMatchFamilyStyle(nullptr, matchFamilyName, normalStyle);
    EXPECT_EQ(typeface, nullptr);

    OH_Drawing_FontMgr *mgr = OH_Drawing_FontMgrCreate();
    typeface = OH_Drawing_FontMgrMatchFamilyStyleCharacter(mgr, nullptr, normalStyle, nullptr, 0, ' ');
    EXPECT_NE(typeface, nullptr);
    OH_Drawing_TypefaceDestroy(typeface);

    typeface = OH_Drawing_FontMgrMatchFamilyStyleCharacter(mgr, matchFamilyName, normalStyle, nullptr, 0, ' ');
    EXPECT_NE(typeface, nullptr);
    OH_Drawing_TypefaceDestroy(typeface);

    OH_Drawing_FontMgrDestroy(mgr);
}

/*
 * @tc.name: OH_Drawing_FontMgrTest009
 * @tc.desc: test for matching family style character.
 * @tc.type: FUNC
 */
HWTEST_F(OH_Drawing_FontMgrTest, OH_Drawing_FontMgrTest009, TestSize.Level1)
{
    const char* matchFamilyName = "HarmonyOS-Sans";
    OH_Drawing_FontStyleStruct normalStyle;
    normalStyle.weight = FONT_WEIGHT_400;
    normalStyle.width = FONT_WIDTH_NORMAL;
    normalStyle.slant = FONT_STYLE_NORMAL;

    const char *bcp47[] = {"zh-Hans", "zh-CN"};
    OH_Drawing_Typeface *typeface = OH_Drawing_FontMgrMatchFamilyStyleCharacter(nullptr, matchFamilyName,
                                                                                    normalStyle, bcp47, 1, ' ');
    EXPECT_EQ(typeface, nullptr);
    OH_Drawing_TypefaceDestroy(typeface);
}

/*
 * @tc.name: OH_Drawing_FontMgrTest010
 * @tc.desc: test for create a typeface for the given index.
 * @tc.type: FUNC
 */
HWTEST_F(OH_Drawing_FontMgrTest, OH_Drawing_FontMgrTest010, TestSize.Level1)
{
    OH_Drawing_FontMgr *mgr = OH_Drawing_FontMgrCreate();
    OH_Drawing_FontStyleSet* fontStyleSet = OH_Drawing_FontMgrCreateFontStyleSet(mgr, 0);
    OH_Drawing_Typeface *typeface = OH_Drawing_FontStyleSetCreateTypeface(fontStyleSet, 0);
    EXPECT_NE(typeface, nullptr);
    typeface = OH_Drawing_FontStyleSetCreateTypeface(fontStyleSet, -1);
    EXPECT_EQ(typeface, nullptr);
    typeface = OH_Drawing_FontStyleSetCreateTypeface(fontStyleSet, 0xffff);
    EXPECT_EQ(typeface, nullptr);
    typeface = OH_Drawing_FontStyleSetCreateTypeface(nullptr, 0);
    EXPECT_EQ(typeface, nullptr);
    OH_Drawing_FontMgrDestroyFontStyleSet(fontStyleSet);
    OH_Drawing_TypefaceDestroy(typeface);
    OH_Drawing_FontMgrDestroy(mgr);
}

/*
 * @tc.name: OH_Drawing_FontMgrTest011
 * @tc.desc: test for get font style struct.
 * @tc.type: FUNC
 */
HWTEST_F(OH_Drawing_FontMgrTest, OH_Drawing_FontMgrTest011, TestSize.Level1)
{
    OH_Drawing_FontMgr *mgr = OH_Drawing_FontMgrCreate();
    OH_Drawing_FontStyleSet* fontStyleSet = OH_Drawing_FontMgrCreateFontStyleSet(mgr, 0);
    OH_Drawing_FontStyleStruct normalStyle;
    char* styleName = nullptr;
    normalStyle = OH_Drawing_FontStyleSetGetStyle(fontStyleSet, 0, &styleName);
    ASSERT_NE(styleName, nullptr);
    EXPECT_STREQ(styleName, "normal");
    EXPECT_EQ(normalStyle.weight, 400);
    OH_Drawing_FontStyleSetFreeStyleName(&styleName);

    normalStyle = OH_Drawing_FontStyleSetGetStyle(fontStyleSet, 0, nullptr);
    OH_Drawing_FontStyleSetFreeStyleName(nullptr);

    normalStyle = OH_Drawing_FontStyleSetGetStyle(nullptr, 0, &styleName);
    EXPECT_EQ(styleName, nullptr);
    EXPECT_EQ(normalStyle.slant, OH_Drawing_FontStyle::FONT_STYLE_NORMAL);
    EXPECT_EQ(normalStyle.weight, OH_Drawing_FontWeight::FONT_WEIGHT_400);
    EXPECT_EQ(normalStyle.width, OH_Drawing_FontWidth::FONT_WIDTH_NORMAL);

    normalStyle = OH_Drawing_FontStyleSetGetStyle(fontStyleSet, -1, &styleName);
    EXPECT_EQ(styleName, nullptr);

    normalStyle = OH_Drawing_FontStyleSetGetStyle(fontStyleSet, 0xffff, &styleName);
    ASSERT_NE(styleName, nullptr);
    EXPECT_STREQ(styleName, "");

    OH_Drawing_FontMgrDestroyFontStyleSet(fontStyleSet);
    OH_Drawing_FontMgrDestroy(mgr);
}

/*
 * @tc.name: OH_Drawing_FontMgrTest012
 * @tc.desc: test for get typeface by match style.
 * @tc.type: FUNC
 */
HWTEST_F(OH_Drawing_FontMgrTest, OH_Drawing_FontMgrTest012, TestSize.Level1)
{
    OH_Drawing_FontMgr *mgr = OH_Drawing_FontMgrCreate();
    OH_Drawing_FontStyleSet* fontStyleSet = OH_Drawing_FontMgrCreateFontStyleSet(mgr, 0);
    OH_Drawing_FontStyleStruct normalStyle;
    normalStyle.weight = FONT_WEIGHT_400;
    normalStyle.width = FONT_WIDTH_NORMAL;
    normalStyle.slant = FONT_STYLE_NORMAL;
    OH_Drawing_Typeface* typeface = OH_Drawing_FontStyleSetMatchStyle(fontStyleSet, normalStyle);
    EXPECT_NE(typeface, nullptr);

    typeface = OH_Drawing_FontStyleSetMatchStyle(nullptr, normalStyle);
    EXPECT_EQ(typeface, nullptr);
    OH_Drawing_FontMgrDestroyFontStyleSet(fontStyleSet);
    OH_Drawing_TypefaceDestroy(typeface);
    OH_Drawing_FontMgrDestroy(mgr);
}

/*
 * @tc.name: OH_Drawing_FontMgrTest013
 * @tc.desc: test for get font style set.
 * @tc.type: FUNC
 */
HWTEST_F(OH_Drawing_FontMgrTest, OH_Drawing_FontMgrTest013, TestSize.Level1)
{
    OH_Drawing_FontMgr *mgr = OH_Drawing_FontMgrCreate();
    OH_Drawing_FontStyleSet* fontStyleSet = OH_Drawing_FontMgrCreateFontStyleSet(mgr, 0);
    int count = OH_Drawing_FontStyleSetCount(fontStyleSet);
    EXPECT_EQ(count, 2);

    count = OH_Drawing_FontStyleSetCount(nullptr);
    EXPECT_EQ(count, 0);
    OH_Drawing_FontMgrDestroyFontStyleSet(fontStyleSet);
    OH_Drawing_FontMgrDestroy(mgr);
}

/*
 * @tc.name: OH_Drawing_FontMgrTest014
 * @tc.desc: test for get font family name with error data.
 * @tc.type: FUNC
 */
HWTEST_F(OH_Drawing_FontMgrTest, OH_Drawing_FontMgrTest014, TestSize.Level1)
{
    OH_Drawing_FontMgr *mgr = OH_Drawing_FontMgrCreate();
    ASSERT_NE(mgr, nullptr);

    char *familyName = OH_Drawing_FontMgrGetFamilyName(mgr, -1);
    EXPECT_EQ(familyName, nullptr);

    familyName = OH_Drawing_FontMgrGetFamilyName(mgr, 20); // out of range
    EXPECT_EQ(familyName, nullptr);

    OH_Drawing_FontMgrDestroy(mgr);
}
} // namespace Drawing
} // namespace Rosen
} // namespace OHOS