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
    static inline Drawing::Canvas drawingCanvas_;
};

void RSPaintFilterCanvasTest::SetUpTestCase()
{
    canvas_ = new RSPaintFilterCanvas(&drawingCanvas_);
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

/**
 * @tc.name: RestoreAlphaToCountTest
 * @tc.desc:
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSPaintFilterCanvasTest, RestoreAlphaToCountTest, TestSize.Level1)
{
    int count = 0;
    canvas_->RestoreAlphaToCount(count);
    ASSERT_EQ(canvas_->GetAlphaSaveCount(), 1);
}

/**
 * @tc.name: RestoreAlphaToCount
 * @tc.desc:
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSPaintFilterCanvasTest, RestoreEnvTest, TestSize.Level1)
{
    canvas_->RestoreEnv();
}

/**
 * @tc.name: RestoreEnvToCountTest
 * @tc.desc:
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSPaintFilterCanvasTest, RestoreEnvToCountTest, TestSize.Level1)
{
    int count = 0;
    canvas_->RestoreEnvToCount(count);
    ASSERT_EQ(canvas_->GetAlphaSaveCount(), 1);
}

/**
 * @tc.name: SetEnvForegroundColorTest
 * @tc.desc:
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSPaintFilterCanvasTest, SetEnvForegroundColorTest, TestSize.Level1)
{
    Color color;
    canvas_->SetEnvForegroundColor(color);
}

/**
 * @tc.name: GetEnvForegroundColorTest
 * @tc.desc:
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSPaintFilterCanvasTest, GetEnvForegroundColorTest, TestSize.Level1)
{
    Color color {0xFF000000};
    Color setColor {};
    canvas_->SetEnvForegroundColor(setColor);
}

/**
 * @tc.name: onFilterTest
 * @tc.desc:
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSPaintFilterCanvasTest, onFilterTest, TestSize.Level1)
{
    Drawing::Color color {0x00000001};
    Drawing::Brush brush;
    brush.SetColor(color);
    canvas_->OnFilterWithBrush(brush);
}
} // namespace OHOS::Rosen