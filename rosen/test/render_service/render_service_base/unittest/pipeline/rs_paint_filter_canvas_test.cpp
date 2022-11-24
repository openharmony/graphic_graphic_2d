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

#include "gtest/gtest.h"

#include "pipeline/rs_paint_filter_canvas.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Rosen {
class RSPaintFilterCanvasTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
    static inline RSPaintFilterCanvas* canvas_;
    static inline SkCanvas skCanvas_;
};

void RSPaintFilterCanvasTest::SetUpTestCase()
{
    canvas_ = new RSPaintFilterCanvas(&skCanvas_);
}
void RSPaintFilterCanvasTest::TearDownTestCase()
{
    delete canvas_;
}
void RSPaintFilterCanvasTest::SetUp() {}
void RSPaintFilterCanvasTest::TearDown() {}

/**
 * @tc.name: SetHighContrast001
 * @tc.desc:
 * @tc.type:FUNC
 * @tc.require:issueI5NMHT
 */
HWTEST_F(RSPaintFilterCanvasTest, SetHighContrast001, TestSize.Level1)
{
    bool mode = true;
    canvas_->SetHighContrast(mode);
    ASSERT_EQ(canvas_->isHighContrastEnabled(), mode);

    mode = false;
    canvas_->SetHighContrast(mode);
    ASSERT_EQ(canvas_->isHighContrastEnabled(), mode);
}

/**
 * @tc.name: RestoreAlpha001
 * @tc.desc:
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSPaintFilterCanvasTest, RestoreAlpha001, TestSize.Level1)
{
    canvas_->RestoreAlpha();
}
} // namespace OHOS::Rosen