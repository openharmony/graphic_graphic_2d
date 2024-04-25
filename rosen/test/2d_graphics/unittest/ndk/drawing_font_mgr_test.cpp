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
    EXPECT_TRUE(count > 0);

    char *familyName = OH_Drawing_FontMgrGetFamilyName(mgr, 0);
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
    OH_Drawing_FontMgr *mgr = OH_Drawing_FontMgrCreate();
    EXPECT_NE(mgr, nullptr);

    OH_Drawing_FontStyleSet* fontStyleSet = OH_Drawing_FontMgrCreateFontStyleSet(mgr, 0);
    EXPECT_NE(fontStyleSet, nullptr);
    OH_Drawing_FontMgrDestroyFontStyleSet(fontStyleSet);

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
    OH_Drawing_Typeface *CharTypeface = OH_Drawing_FontMgrMatchFamilyStyleCharacter(mgr, matchFamilyName,
                                                                                    normalStyle, bcp47, 1, ' ');
    EXPECT_NE(CharTypeface, nullptr);
    OH_Drawing_TypefaceDestroy(CharTypeface);

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
    EXPECT_TRUE(count == 0);
    OH_Drawing_FontMgr *mgr = OH_Drawing_FontMgrCreate();
    EXPECT_NE(mgr, nullptr);
    char *familyName = OH_Drawing_FontMgrGetFamilyName(nullptr, 0);
    EXPECT_TRUE(familyName == nullptr);
    OH_Drawing_FontMgrDestroyFamilyName(familyName);
    OH_Drawing_FontMgrDestroy(mgr);
}

/*
 * @tc.name: OH_Drawing_FontMgrTest008
 * @tc.desc: test for getting font style set.
 * @tc.type: FUNC
 */
HWTEST_F(OH_Drawing_FontMgrTest, OH_Drawing_FontMgrTest008, TestSize.Level1)
{
    OH_Drawing_FontStyleSet* fontStyleSet = OH_Drawing_FontMgrCreateFontStyleSet(nullptr, 0);
    EXPECT_TRUE(fontStyleSet == nullptr);
    const char* matchFamilyName = "HarmonyOS-Sans";
    fontStyleSet = OH_Drawing_FontMgrMatchFamily(nullptr, matchFamilyName);
    EXPECT_TRUE(fontStyleSet == nullptr);

    OH_Drawing_FontStyleStruct normalStyle;
    normalStyle.weight = FONT_WEIGHT_400;
    normalStyle.width = FONT_WIDTH_NORMAL;
    normalStyle.slant = FONT_STYLE_NORMAL;
    OH_Drawing_Typeface *typeface = OH_Drawing_FontMgrMatchFamilyStyle(nullptr, matchFamilyName, normalStyle);
    EXPECT_TRUE(typeface == nullptr);
}

/*
 * @tc.name: OH_Drawing_FontMgrTest009
 * @tc.desc: test for getting font style set.
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
    OH_Drawing_Typeface *CharTypeface = OH_Drawing_FontMgrMatchFamilyStyleCharacter(nullptr, matchFamilyName,
                                                                                    normalStyle, bcp47, 1, ' ');
    EXPECT_TRUE(CharTypeface == nullptr);
}

///////////////////////////////////////////

/*
 * @tc.name: OH_Drawing_FontMgrTest010
 * @tc.desc: test for create a typeface for the given index..
 * @tc.type: FUNC
 */
HWTEST_F(OH_Drawing_FontMgrTest, OH_Drawing_FontMgrTest010, TestSize.Level1)
{
    OH_Drawing_FontMgr *mgr = OH_Drawing_FontMgrCreate();
    OH_Drawing_FontStyleSet* fontStyleSet = OH_Drawing_FontMgrCreateFontStyleSet(mgr, 0);
    OH_Drawing_Typeface *typeface = OH_Drawing_FontStyleSetCreateTypeface(fontStyleSet, 0);
    EXPECT_NE(typeface, nullptr);
    typeface = OH_Drawing_FontStyleSetCreateTypeface(nullptr, 0);
    EXPECT_TRUE(typeface == nullptr);
    OH_Drawing_FontMgrDestroyFontStyleSet(fontStyleSet);
    OH_Drawing_TypefaceDestroy(typeface);
    OH_Drawing_FontMgrDestroy(mgr);
}

/*
 * @tc.name: OH_Drawing_FontMgrTest011
 * @tc.desc: test for create a typeface for the given index..
 * @tc.type: FUNC
 */
HWTEST_F(OH_Drawing_FontMgrTest, OH_Drawing_FontMgrTest011, TestSize.Level1)
{
    OH_Drawing_FontMgr *mgr = OH_Drawing_FontMgrCreate();
    OH_Drawing_FontStyleSet* fontStyleSet = OH_Drawing_FontMgrCreateFontStyleSet(mgr, 0);
    OH_Drawing_FontStyleStruct normalStyle;
    char** styleName = nullptr;
    normalStyle = OH_Drawing_FontStyleSetGetStyle(fontStyleSet, 0, styleName);
    EXPECT_TRUE(normalStyle.weight == FONT_WEIGHT_400);
    OH_Drawing_FontStyleSetFreeStyleName(styleName);
    OH_Drawing_FontMgrDestroyFontStyleSet(fontStyleSet);
    OH_Drawing_FontMgrDestroy(mgr);
}

/*
 * @tc.name: OH_Drawing_FontMgrTest012
 * @tc.desc: test for create a typeface for the given index..
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
    EXPECT_TRUE(typeface == nullptr);
    OH_Drawing_FontMgrDestroyFontStyleSet(fontStyleSet);
    OH_Drawing_TypefaceDestroy(typeface);
    OH_Drawing_FontMgrDestroy(mgr);
}

/*
 * @tc.name: OH_Drawing_FontMgrTest013
 * @tc.desc: test for create a typeface for the given index.
 * @tc.type: FUNC
 */
HWTEST_F(OH_Drawing_FontMgrTest, OH_Drawing_FontMgrTest013, TestSize.Level1)
{
    OH_Drawing_FontMgr *mgr = OH_Drawing_FontMgrCreate();
    OH_Drawing_FontStyleSet* fontStyleSet = OH_Drawing_FontMgrCreateFontStyleSet(mgr, 0);
    int count = OH_Drawing_FontStyleSetCount(fontStyleSet);
    EXPECT_TRUE(count > 0);
    
    count = OH_Drawing_FontStyleSetCount(nullptr);
    EXPECT_TRUE(count == 0);
    OH_Drawing_FontMgrDestroyFontStyleSet(fontStyleSet);
    OH_Drawing_FontMgrDestroy(mgr);
}
} // namespace Drawing
} // namespace Rosen
} // namespace OHOS