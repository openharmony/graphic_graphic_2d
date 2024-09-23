/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
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

#include "drawing_text_font_descriptor.h"
#include "gtest/gtest.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {

class OH_Drawing_FontDescriptorTest : public testing::Test {
};

/*
 * @tc.name: OH_Drawing_FontDescriptorTest001
 * @tc.desc: test for the fontDescriptor.
 * @tc.type: FUNC
 */
HWTEST_F(OH_Drawing_FontDescriptorTest, OH_Drawing_FontDescriptorTest001, TestSize.Level1)
{
    OH_Drawing_FontDescriptor* descArr = OH_Drawing_MatchFontDescriptors(nullptr, nullptr);
    EXPECT_EQ(descArr, nullptr);
}

/*
 * @tc.name: OH_Drawing_FontDescriptorTest002
 * @tc.desc: test for the fontDescriptor.
 * @tc.type: FUNC
 */
HWTEST_F(OH_Drawing_FontDescriptorTest, OH_Drawing_FontDescriptorTest002, TestSize.Level1)
{
    OH_Drawing_FontDescriptor* desc = OH_Drawing_CreateFontDescriptor();
    size_t num = 0;
    OH_Drawing_FontDescriptor* descArr = OH_Drawing_MatchFontDescriptors(desc, &num);
    OH_Drawing_DestroyFontDescriptor(desc);
    EXPECT_NE(descArr, nullptr);
    EXPECT_NE(num, 0);
    OH_Drawing_DestroyFontDescriptors(descArr, num);
}

/*
 * @tc.name: OH_Drawing_FontDescriptorTest003
 * @tc.desc: test for the fontDescriptor.
 * @tc.type: FUNC
 */
HWTEST_F(OH_Drawing_FontDescriptorTest, OH_Drawing_FontDescriptorTest003, TestSize.Level1)
{
    OH_Drawing_FontDescriptor* desc = OH_Drawing_CreateFontDescriptor();
    desc->weight = -1;
    size_t num = 0;
    OH_Drawing_FontDescriptor* descArr = OH_Drawing_MatchFontDescriptors(desc, &num);
    OH_Drawing_DestroyFontDescriptor(desc);
    EXPECT_EQ(descArr, nullptr);
    EXPECT_EQ(num, 0);
    OH_Drawing_DestroyFontDescriptors(descArr, num);
}

/*
 * @tc.name: OH_Drawing_FontDescriptorTest004
 * @tc.desc: test for the fontDescriptor.
 * @tc.type: FUNC
 */
HWTEST_F(OH_Drawing_FontDescriptorTest, OH_Drawing_FontDescriptorTest004, TestSize.Level1)
{
    OH_Drawing_FontDescriptor* desc = OH_Drawing_CreateFontDescriptor();
    char* fontFamily = strdup("HarmonyOS Sans");
    desc->fontFamily = fontFamily;
    size_t num = 0;
    OH_Drawing_FontDescriptor* descArr = OH_Drawing_MatchFontDescriptors(desc, &num);
    EXPECT_NE(descArr, nullptr);
    EXPECT_EQ(num, 1);
    EXPECT_STREQ(descArr[0].fontFamily, fontFamily);
    OH_Drawing_DestroyFontDescriptors(descArr, num);
    free(fontFamily);

    fontFamily = strdup("HarmonyOS Sans Condensed");
    desc->fontFamily = fontFamily;
    descArr = OH_Drawing_MatchFontDescriptors(desc, &num);
    OH_Drawing_DestroyFontDescriptor(desc);
    EXPECT_NE(descArr, nullptr);
    EXPECT_EQ(num, 1);
    EXPECT_STREQ(descArr[0].fontFamily, fontFamily);
    OH_Drawing_DestroyFontDescriptors(descArr, num);
    free(fontFamily);
}

/*
 * @tc.name: OH_Drawing_FontDescriptorTest005
 * @tc.desc: test for the fontDescriptor.
 * @tc.type: FUNC
 */
HWTEST_F(OH_Drawing_FontDescriptorTest, OH_Drawing_FontDescriptorTest005, TestSize.Level1)
{
    OH_Drawing_FontDescriptor* desc = OH_Drawing_CreateFontDescriptor();
    char* fontFamily = strdup("HarmonyOS Sans");
    desc->fontFamily = fontFamily;
    desc->weight = 400;
    desc->italic = 0;

    size_t num = 0;
    OH_Drawing_FontDescriptor* descArr = OH_Drawing_MatchFontDescriptors(desc, &num);
    OH_Drawing_DestroyFontDescriptor(desc);
    EXPECT_NE(descArr, nullptr);
    EXPECT_EQ(num, 1);
    EXPECT_STREQ(descArr[0].fontFamily, fontFamily);
    EXPECT_EQ(descArr[0].weight, 400);
    EXPECT_EQ(descArr[0].italic, 0);
    OH_Drawing_DestroyFontDescriptors(descArr, num);
    free(fontFamily);
}

}