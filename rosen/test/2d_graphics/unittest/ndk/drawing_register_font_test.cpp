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
#include <gtest/gtest.h>

#include "c/drawing_font_collection.h"
#include "c/drawing_register_font.h"
#include "c/drawing_text_declaration.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
static const char* FONT_FAMILY = "HarmonyOS_SansSC_VF";
static const char* FONT_PATH = "/data/HarmonyOS_SansSC_VF.ttf";

class OH_Drawing_RegisterFontTest : public testing::Test {
};

/*
 * @tc.name: OH_Drawing_RegisterFontTest001
 * @tc.desc: test for register font
 * @tc.type: FUNC
 */
HWTEST_F(OH_Drawing_RegisterFontTest, OH_Drawing_RegisterFontTest001, TestSize.Level1)
{
    OH_Drawing_FontCollection* fontCollection = OH_Drawing_CreateFontCollection();
    uint32_t errorCode = OH_Drawing_RegisterFont(fontCollection, FONT_FAMILY, FONT_PATH);
    std::ifstream fileStream(FONT_PATH);
    if (fileStream.is_open()) {
        EXPECT_EQ(errorCode, 0);
        fileStream.close();
    } else {
        EXPECT_EQ(errorCode, 1);
    }
    OH_Drawing_DestroyFontCollection(fontCollection);
}
}