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

#include "gtest/gtest.h"
#include "include/core/SkSurface.h"

#include "pipeline/rs_paint_filter_canvas.h"
#include "render/rs_material_filter.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Rosen {
class RSMaterialFilterTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void RSMaterialFilterTest::SetUpTestCase() {}
void RSMaterialFilterTest::TearDownTestCase() {}
void RSMaterialFilterTest::SetUp() {}
void RSMaterialFilterTest::TearDown() {}

/**
 * @tc.name: CreateMaterialStyle001
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSMaterialFilterTest, CreateMaterialStyle001, TestSize.Level1)
{
    float dipScale = 1.0;
    BLUR_COLOR_MODE mode = BLUR_COLOR_MODE::DEFAULT;
    MATERIAL_BLUR_STYLE style = static_cast<MATERIAL_BLUR_STYLE>(0);
    RSMaterialFilter rsMaterialFilter(style, dipScale, mode);
    EXPECT_EQ(rsMaterialFilter.GetImageFilter(), nullptr);

    style = MATERIAL_BLUR_STYLE::STYLE_CARD_DARK;
    rsMaterialFilter = RSMaterialFilter(style, dipScale, mode);
    EXPECT_NE(rsMaterialFilter.GetImageFilter(), nullptr);
}
} // namespace OHOS::Rosen
