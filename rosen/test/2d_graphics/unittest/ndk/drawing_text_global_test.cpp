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

#include "gtest/gtest.h"
#include "drawing_text_global.h"
#include "global_config/text_global_config.h"
#include "text/text_blob.h"

using namespace testing;
using namespace testing::ext;
using namespace OHOS::Rosen::Drawing;
namespace OHOS::Rosen::SrvText {
class NativeDrawingTextGlobalTest : public testing::Test {};

static uint32_t GetTextHighContrast()
{
    auto& instance = ProcessTextConstrast::Instance();
    return static_cast<uint32_t>(instance.GetTextContrast());
}

/*
* @tc.name: OH_Drawing_HighContrastTest01
* @tc.desc: test for text high contrast mode
* @tc.type: FUNC
*/
HWTEST_F(NativeDrawingTextGlobalTest, OH_Drawing_HighContrastTest01, TestSize.Level1)
{
    OH_Drawing_SetTextHighContrast(OH_Drawing_TextHighContrast::TEXT_FOLLOW_SYSTEM_HIGH_CONTRAST);
    EXPECT_EQ(GetTextHighContrast(), OH_Drawing_TextHighContrast::TEXT_FOLLOW_SYSTEM_HIGH_CONTRAST);

    OH_Drawing_SetTextHighContrast(OH_Drawing_TextHighContrast::TEXT_APP_DISABLE_HIGH_CONTRAST);
    EXPECT_EQ(GetTextHighContrast(), OH_Drawing_TextHighContrast::TEXT_APP_DISABLE_HIGH_CONTRAST);

    OH_Drawing_SetTextHighContrast(OH_Drawing_TextHighContrast::TEXT_APP_ENABLE_HIGH_CONTRAST);
    EXPECT_EQ(GetTextHighContrast(), OH_Drawing_TextHighContrast::TEXT_APP_ENABLE_HIGH_CONTRAST);
}

/*
* @tc.name: OH_Drawing_HighContrastTest02
* @tc.desc: test for text high contrast mode（Invalid）
* @tc.type: FUNC
*/
HWTEST_F(NativeDrawingTextGlobalTest, OH_Drawing_HighContrastTest02, TestSize.Level1)
{
    uint32_t preValue = GetTextHighContrast();
    OH_Drawing_SetTextHighContrast(static_cast<OH_Drawing_TextHighContrast>(TEXT_HIGH_CONTRAST_BUTT));
    EXPECT_EQ(GetTextHighContrast(), preValue);
}

} // namespace OHOS::Rosen::SrvText