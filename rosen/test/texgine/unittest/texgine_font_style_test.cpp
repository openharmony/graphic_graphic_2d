/*
 * Copyright (c) 2023 Huawei Device Co., Ltd.
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

#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include "texgine_font_style.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
namespace TextEngine {
class TexgineFontStyleTest : public testing::Test {
};

/**
 * @tc.name:SetAndGet
 * @tc.desc: Verify the SetAndGet
 * @tc.type:FUNC
 */
HWTEST_F(TexgineFontStyleTest, SetAndGet, TestSize.Level1)
{
    std::shared_ptr<RSFontStyle> sfs1 = std::make_shared<RSFontStyle>();
    std::shared_ptr<RSFontStyle> sfs2 = nullptr;
    EXPECT_NO_THROW({
        std::shared_ptr<TexgineFontStyle> tfs1 = std::make_shared<TexgineFontStyle>(sfs1);
        EXPECT_EQ(tfs1->GetFontStyle(), sfs1);
        tfs1->SetFontStyle(sfs2);
        tfs1->SetStyle(*sfs1);
        EXPECT_EQ(tfs1->GetFontStyle(), nullptr);

        std::shared_ptr<TexgineFontStyle> tfs2 = std::make_shared<TexgineFontStyle>();
        tfs2->SetStyle(*sfs1);
        EXPECT_EQ(*tfs2->GetFontStyle(), *sfs1);
    });
}
} // namespace TextEngine
} // namespace Rosen
} // namespace OHOS
