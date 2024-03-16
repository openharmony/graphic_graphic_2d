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
#include "c/drawing_font_mgr.h"
#include "c/drawing_typeface.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
namespace Drawing {
class OH_Drawing_FontMgrTest : public testing::Test {
};

/*
 * @tc.name: OH_Drawing_FontMgrTest001
 * @tc.desc: test for font manager.
 * @tc.type: FUNC
 */
HWTEST_F(OH_Drawing_FontMgrTest, OH_Drawing_FontMgrTest001, TestSize.Level1)
{
    OH_Drawing_FontMgr *mgr = OH_Drawing_FontMgrCreate();
    EXPECT_NE(mgr, nullptr);
    int count = OH_Drawing_FontMgrGetFamiliesCount(mgr);
    EXPECT_TRUE(count > 0);

    int familyNameSize = 0;
    char *familyName = OH_Drawing_FontMgrGetFamilyName(mgr, 0, &familyNameSize);
    OH_Drawing_DestroyFamilyName(familyName);

    OH_Drawing_FontStyleSet* fontStyleSet = OH_Drawing_FontStyleSetCreate(mgr, 0);
    EXPECT_NE(fontStyleSet, nullptr);
    OH_Drawing_DestroyFontStyleSet(fontStyleSet);

    OH_Drawing_FontMgrDestroy(mgr);
}

/*
 * @tc.name: OH_Drawing_FontMgrTest002
 * @tc.desc: test for font manager match family.
 * @tc.type: FUNC
 */
HWTEST_F(OH_Drawing_FontMgrTest, OH_Drawing_FontMgrTest002, TestSize.Level1)
{
    OH_Drawing_FontMgr *mgr = OH_Drawing_FontMgrCreate();
    EXPECT_NE(mgr, nullptr);
    const char* matchFamilyName = "HarmonyOS-Sans";
    OH_Drawing_FontStyleSet* fontStyleSet = OH_Drawing_FontMgrMatchFamily(mgr, matchFamilyName);
    EXPECT_NE(fontStyleSet, nullptr);
    OH_Drawing_DestroyFontStyleSet(fontStyleSet);

    OH_Drawing_FontForm normalForm;
    normalForm.weight = NORMAL_WEIGHT;
    normalForm.width = NORMAL_WIDTH;
    normalForm.slant = UPRIGHT_SLANT;
    OH_Drawing_Typeface *typeface = OH_Drawing_FontMgrMatchFamilyStyle(mgr, matchFamilyName, &normalForm);
    EXPECT_NE(typeface, nullptr);
    OH_Drawing_TypefaceDestroy(typeface);

    const char *bcp47[] = {"zh-Hans"};
    OH_Drawing_Typeface *CharTypeface = OH_Drawing_FontMgrMatchFamilyStyleCharacter(
        mgr, matchFamilyName, &normalForm, bcp47, 1, ' ');
    EXPECT_NE(CharTypeface, nullptr);
    OH_Drawing_TypefaceDestroy(CharTypeface);

    OH_Drawing_FontMgrDestroy(mgr);
}
} // namespace Drawing
} // namespace Rosen
} // namespace OHOS