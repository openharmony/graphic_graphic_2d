/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
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

#ifndef USE_ROSEN_DRAWING
#include <gtest/gtest.h>

#include "pipeline/rs_draw_cmd.h"
#include "pipeline/rs_draw_cmd_list.h"
#include "pipeline/rs_paint_filter_canvas.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
class DrawCmdListTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void DrawCmdListTest::SetUpTestCase() {}
void DrawCmdListTest::TearDownTestCase() {}
void DrawCmdListTest::SetUp() {}
void DrawCmdListTest::TearDown() {}

/**
 * @tc.name: ClearDrawCmdList001
 * @tc.desc: test
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(DrawCmdListTest, ClearDrawCmdList001, TestSize.Level1)
{
    int w = 150;
    int h = 300;
    DrawCmdList list(w, h);
    list.GetSize();
    ASSERT_EQ(list.GetWidth(), w);
    ASSERT_EQ(list.GetHeight(), h);
    list.ClearOp();
}

/**
 * @tc.name: GetOpUnmarshallingFunc001
 * @tc.desc: test
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(DrawCmdListTest, GetOpUnmarshallingFunc001, TestSize.Level1)
{
    int w = 150;
    int h = 300;
    DrawCmdList list(w, h);
    RSOpType type = static_cast<RSOpType>(-1);
    SkCanvas skCanvas;
    RSPaintFilterCanvas canvas(&skCanvas);
    SkRect rect = SkRect::MakeLTRB(0, 0, 512, 512);
    list.GenerateCache(&canvas, &rect);
    canvas.SetHighContrast(true);
    list.Playback(canvas, &rect);
    list.GenerateCache(&canvas, &rect);
    canvas.SetHighContrast(false);
    list.Playback(canvas, &rect);
    ASSERT_EQ(nullptr, list.GetOpUnmarshallingFunc(type));
}

/**
 * @tc.name: PlayBackForRecord001
 * @tc.desc: test
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(DrawCmdListTest, PlayBackForRecord001, TestSize.Level1)
{
    int w = 0;
    int h = 0;
    DrawCmdList list(w, h);
    SkCanvas skCanvas;
    RSPaintFilterCanvas canvas(&skCanvas);
    SkRect rect = SkRect::MakeLTRB(0, 0, 512, 512);
    int startOpId = 0;
    int descStartOpId = 0;
    std::string str = list.PlayBackForRecord(canvas, startOpId, list.GetSize(), descStartOpId, &rect);
    ASSERT_TRUE(str.empty());
}

/**
 * @tc.name: PlayBackForRecord002
 * @tc.desc: test
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(DrawCmdListTest, PlayBackForRecord002, TestSize.Level1)
{
    int w = 300;
    int h = 300;
    DrawCmdList list = DrawCmdList(w, h);
    SkPaint paint;
    SkRRect rrect;
    SkCanvas skCanvas;
    RSPaintFilterCanvas canvas(&skCanvas);
    SkRect rect = SkRect::MakeWH(50, 100);
    bool antiAlias = false;
    list.AddOp(std::make_unique<RoundRectOpItem>(rrect, paint));
    list.AddOp(std::make_unique<ClipRectOpItem>(rect, SkClipOp::kDifference, antiAlias));
    ASSERT_FALSE(list.GetOpsWithDesc().empty());
    int startOpId = 0;
    int descStartOpId = 0;
    std::string str = list.PlayBackForRecord(canvas, startOpId, list.GetSize(), descStartOpId, &rect);
    ASSERT_FALSE(str.empty());
}
} // namespace Rosen
} // namespace OHOS
#endif
