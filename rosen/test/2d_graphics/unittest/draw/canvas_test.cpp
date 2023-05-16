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

#include "draw/canvas.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
namespace Drawing {
class CanvasTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void CanvasTest::SetUpTestCase() {}
void CanvasTest::TearDownTestCase() {}
void CanvasTest::SetUp() {}
void CanvasTest::TearDown() {}

/**
 * @tc.name: CreateAndDestroy001
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require: AR000GGNV3
 * @tc.author:
 */
HWTEST_F(CanvasTest, CreateAndDestroy001, TestSize.Level1)
{
    auto canvas = std::make_unique<Canvas>();
    EXPECT_TRUE(nullptr != canvas);
}

/**
 * @tc.name: CanvasSaveLayerTest001
 * @tc.desc: Test for saving Matrix and clipping area, and allocates Surface for subsequent drawing.
 * @tc.type: FUNC
 * @tc.require: I719U5
 */
HWTEST_F(CanvasTest, CanvasSaveLayerTest001, TestSize.Level1)
{
    auto canvas = std::make_unique<Canvas>();
    ASSERT_TRUE(canvas != nullptr);
    SaveLayerOps saveLayerOps;
    canvas->SaveLayer(saveLayerOps);
}

/**
 * @tc.name: CanvasSaveLayerTest002
 * @tc.desc: Test for saving Matrix and clipping area, and allocates Surface for subsequent drawing.
 * @tc.type: FUNC
 * @tc.require: I719U5
 */
HWTEST_F(CanvasTest, CanvasSaveLayerTest002, TestSize.Level1)
{
    auto canvas = std::make_unique<Canvas>();
    ASSERT_TRUE(canvas != nullptr);
    Rect rect(0.0f, 0.0f, 10.0f, 20.0f);
    Brush brush;
    uint32_t saveLayerFlags = 0;
    SaveLayerOps saveLayerRec(&rect, &brush, saveLayerFlags);
    canvas->SaveLayer(saveLayerRec);
}

/**
 * @tc.name: CanvasRestoreTest001
 * @tc.desc: Test for Restore function.
 * @tc.type: FUNC
 * @tc.require: I719U5
 */
HWTEST_F(CanvasTest, CanvasRestoreTest001, TestSize.Level1)
{
    auto canvas = std::make_unique<Canvas>();
    ASSERT_TRUE(canvas != nullptr);
    Rect rect(0.0f, 0.0f, 10.0f, 20.0f);
    Brush brush;
    uint32_t saveLayerFlags = 0;
    SaveLayerOps saveLayerRec(&rect, &brush, saveLayerFlags);
    canvas->SaveLayer(saveLayerRec);
    canvas->Restore();
}

/**
 * @tc.name: CanvasGetSaveCountTest001
 * @tc.desc: Test for geting the number of saved states.
 * @tc.type: FUNC
 * @tc.require: I719U5
 */
HWTEST_F(CanvasTest, CanvasGetSaveCountTest001, TestSize.Level1)
{
    auto canvas = std::make_unique<Canvas>();
    ASSERT_TRUE(canvas != nullptr);
    canvas->Save();
    EXPECT_TRUE(2 == canvas->GetSaveCount());
}

/**
 * @tc.name: CanvasRestoreToCountTest001
 * @tc.desc: Test for restoring Canvas Matrix and clip value state to count.
 * @tc.type: FUNC
 * @tc.require: I719U5
 */
HWTEST_F(CanvasTest, CanvasRestoreToCountTest001, TestSize.Level1)
{
    auto canvas = std::make_unique<Canvas>();
    ASSERT_TRUE(canvas != nullptr);
    canvas->RestoreToCount(2);
}

/**
 * @tc.name: CanvasAttachAndDetachPenTest001
 * @tc.desc: Test for AttachPen and DetachPen functions.
 * @tc.type: FUNC
 * @tc.require: I719U5
 */
HWTEST_F(CanvasTest, CanvasAttachAndDetachPenTest001, TestSize.Level1)
{
    auto canvas = std::make_unique<Canvas>();
    ASSERT_TRUE(canvas != nullptr);
    Pen pen(Color::COLOR_GREEN);
    canvas->AttachPen(pen);
    canvas->DetachPen();
}

/**
 * @tc.name: CanvasAttachAndDetachBrushTest001
 * @tc.desc: Test for AttachBrush and DetachBrush functions.
 * @tc.type: FUNC
 * @tc.require: I719U5
 */
HWTEST_F(CanvasTest, CanvasAttachAndDetachBrushTest001, TestSize.Level1)
{
    auto canvas = std::make_unique<Canvas>();
    ASSERT_TRUE(canvas != nullptr);
    Brush brush(Color::COLOR_GREEN);
    canvas->AttachBrush(brush);
    canvas->DetachBrush();
}

/**
 * @tc.name: GetBounds001
 * @tc.desc: Test for geting the bounds of layer.
 * @tc.type: FUNC
 * @tc.require: I719U5
 */
HWTEST_F(CanvasTest, GetBounds001, TestSize.Level1)
{
    Rect rect(0.0f, 0.0f, 10.0f, 20.0f);
    Brush brush;
    uint32_t saveLayerFlags = 0;
    ImageFilter imageFilter(ImageFilter::FilterType::BLUR, 10.0f, 10.0f, nullptr);
    SaveLayerOps saveLayerRec(&rect, &brush, &imageFilter, saveLayerFlags);
    auto ret = saveLayerRec.GetBounds();
    EXPECT_EQ(ret->GetLeft(), 0.0f);
    EXPECT_EQ(ret->GetBottom(), 20.0f);
}

/**
 * @tc.name: GetBrush001
 * @tc.desc: Test for geting the brush of layer.
 * @tc.type: FUNC
 * @tc.require: I719U5
 */
HWTEST_F(CanvasTest, GetBrush001, TestSize.Level1)
{
    Rect rect(0.0f, 0.0f, 10.0f, 20.0f);
    Brush brush;
    uint32_t saveLayerFlags = 0;
    ImageFilter imageFilter(ImageFilter::FilterType::BLUR, 10.0f, 10.0f, nullptr);
    SaveLayerOps saveLayerRec(&rect, &brush, &imageFilter, saveLayerFlags);
    auto ret = saveLayerRec.GetBrush();
    EXPECT_TRUE(ret != nullptr);
}

/**
 * @tc.name: GetImageFilter001
 * @tc.desc: Test for geting the image filter of layer.
 * @tc.type: FUNC
 * @tc.require: I719U5
 */
HWTEST_F(CanvasTest, GetImageFilter001, TestSize.Level1)
{
    Rect rect(0.0f, 0.0f, 10.0f, 20.0f);
    Brush brush;
    uint32_t saveLayerFlags = 0;
    ImageFilter imageFilter(ImageFilter::FilterType::BLUR, 10.0f, 10.0f, nullptr);
    SaveLayerOps saveLayerRec(&rect, &brush, &imageFilter, saveLayerFlags);
    auto ret = saveLayerRec.GetImageFilter();
    EXPECT_TRUE(ret != nullptr);
}

/**
 * @tc.name: GetSaveLayerFlags001
 * @tc.desc: Test for geting the options to modify layer.
 * @tc.type: FUNC
 * @tc.require: I719U5
 */
HWTEST_F(CanvasTest, GetSaveLayerFlags001, TestSize.Level1)
{
    Rect rect(0.0f, 0.0f, 10.0f, 20.0f);
    Brush brush;
    uint32_t saveLayerFlags = 0;
    ImageFilter imageFilter(ImageFilter::FilterType::BLUR, 10.0f, 10.0f, nullptr);
    SaveLayerOps saveLayerRec(&rect, &brush, &imageFilter, saveLayerFlags);
    auto ret = saveLayerRec.GetSaveLayerFlags();
    EXPECT_EQ(ret, 0);
}

/**
 * @tc.name: AutoCanvasRestoreTest001
 * @tc.desc: Test for Creating AutoCanvasRestore;
 * @tc.type: FUNC
 * @tc.require: I719U5
 */
HWTEST_F(CanvasTest, AutoCanvasRestoreTest001, TestSize.Level1)
{
    Canvas canvas;
    bool doSave = true;
    auto autoCanvasRestore = std::make_unique<AutoCanvasRestore>(canvas, doSave);
    ASSERT_TRUE(autoCanvasRestore != nullptr);
}
} // namespace Drawing
} // namespace Rosen
} // namespace OHOS
