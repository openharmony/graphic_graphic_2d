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

#include "gtest/gtest.h"

#include "c/drawing_bitmap.h"
#include "c/drawing_brush.h"
#include "c/drawing_canvas.h"
#include "c/drawing_color.h"
#include "c/drawing_path.h"
#include "c/drawing_pen.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
namespace Drawing {
class NativeDrawingCanvasTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void NativeDrawingCanvasTest::SetUpTestCase() {}
void NativeDrawingCanvasTest::TearDownTestCase() {}
void NativeDrawingCanvasTest::SetUp() {}
void NativeDrawingCanvasTest::TearDown() {}

/*
 * @tc.name: NativeDrawingCanvasTest_canvas001
 * @tc.desc: test for create canvas and destroy canvas.
 * @tc.type: FUNC
 * @tc.require: AR000GTO5R
 */
HWTEST_F(NativeDrawingCanvasTest, NativeDrawingCanvasTest_canvas001, TestSize.Level1)
{
    OH_Drawing_Canvas* canvas = OH_Drawing_CanvasCreate();
    EXPECT_EQ(canvas == nullptr, false);
    OH_Drawing_CanvasDestroy(canvas);
}

/*
 * @tc.name: NativeDrawingCanvasTest_canvas002
 * @tc.desc: test for OH_Drawing_CanvasBind.
 * @tc.type: FUNC
 * @tc.require: AR000GTO5R
 */
HWTEST_F(NativeDrawingCanvasTest, NativeDrawingCanvasTest_canvas002, TestSize.Level1)
{
    OH_Drawing_Canvas* canvas1 = OH_Drawing_CanvasCreate();
    OH_Drawing_Bitmap* bitmap = OH_Drawing_BitmapCreate();
    OH_Drawing_CanvasBind(canvas1, bitmap);
}

/*
 * @tc.name: NativeDrawingCanvasTest_canvas003
 * @tc.desc: test for OH_Drawing_CanvasAttachPen & OH_Drawing_CanvasDetachPen.
 * @tc.type: FUNC
 * @tc.require: AR000GTO5R
 */
HWTEST_F(NativeDrawingCanvasTest, NativeDrawingCanvasTest_canvas003, TestSize.Level1)
{
    OH_Drawing_Canvas* canvas2 = OH_Drawing_CanvasCreate();
    OH_Drawing_Pen* pen = OH_Drawing_PenCreate();
    OH_Drawing_CanvasAttachPen(canvas2, pen);
    OH_Drawing_CanvasDetachPen(canvas2);
}

/*
 * @tc.name: NativeDrawingCanvasTest_canvas004
 * @tc.desc: test for OH_Drawing_CanvasAttachBrush & OH_Drawing_CanvasDetachBrush.
 * @tc.type: FUNC
 * @tc.require: AR000GTO5R
 */
HWTEST_F(NativeDrawingCanvasTest, NativeDrawingCanvasTest_canvas004, TestSize.Level1)
{
    OH_Drawing_Canvas* canvas3 = OH_Drawing_CanvasCreate();
    OH_Drawing_Brush* brush = OH_Drawing_BrushCreate();
    OH_Drawing_CanvasAttachBrush(canvas3, brush);
    OH_Drawing_CanvasDetachBrush(canvas3);
}

/*
 * @tc.name: NativeDrawingCanvasTest_canvas005
 * @tc.desc: test for OH_Drawing_CanvasDrawLine.
 * @tc.type: FUNC
 * @tc.require: AR000GTO5R
 */
HWTEST_F(NativeDrawingCanvasTest, NativeDrawingCanvasTest_canvas005, TestSize.Level1)
{
    OH_Drawing_Canvas* canvas4 = OH_Drawing_CanvasCreate();
    OH_Drawing_CanvasDrawLine(canvas4, 0, 0, 20, 20);
    OH_Drawing_CanvasDrawLine(canvas4, -15.2f, -1, 0, 20);
}

/*
 * @tc.name: NativeDrawingCanvasTest_canvas006
 * @tc.desc: test for OH_Drawing_CanvasDrawPath.
 * @tc.type: FUNC
 * @tc.require: AR000GTO5R
 */
HWTEST_F(NativeDrawingCanvasTest, NativeDrawingCanvasTest_canvas006, TestSize.Level1)
{
    OH_Drawing_Canvas* canvas5 = OH_Drawing_CanvasCreate();
    OH_Drawing_Path* path = OH_Drawing_PathCreate();
    OH_Drawing_CanvasDrawPath(canvas5, path);
}

/*
 * @tc.name: NativeDrawingCanvasTest_canvas007
 * @tc.desc: test for OH_Drawing_CanvasSave & OH_Drawing_CanvasRestore.
 * @tc.type: FUNC
 * @tc.require: AR000GTO5R
 */
HWTEST_F(NativeDrawingCanvasTest, NativeDrawingCanvasTest_canvas007, TestSize.Level1)
{
    OH_Drawing_Canvas* canvas6 = OH_Drawing_CanvasCreate();
    OH_Drawing_Path* path = OH_Drawing_PathCreate();
    OH_Drawing_CanvasDrawPath(canvas6, path);
    OH_Drawing_CanvasSave(canvas6);
    OH_Drawing_CanvasRestore(canvas6);
}

/*
 * @tc.name: NativeDrawingCanvasTest_canvas008
 * @tc.desc: test for OH_Drawing_CanvasClear.
 * @tc.type: FUNC
 * @tc.require: AR000GTO5R
 */
HWTEST_F(NativeDrawingCanvasTest, NativeDrawingCanvasTest_canvas008, TestSize.Level1)
{
    OH_Drawing_Canvas* canvas7 = OH_Drawing_CanvasCreate();
    OH_Drawing_CanvasClear(canvas7, OH_Drawing_ColorSetArgb(0xFF, 0xFF, 0x00, 0x00));
}
} // namespace Drawing
} // namespace Rosen
} // namespace OHOS