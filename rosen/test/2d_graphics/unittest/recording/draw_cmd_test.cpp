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

#include "pixel_map.h"
#include "recording/cmd_list.h"
#include "recording/cmd_list_helper.h"
#include "recording/draw_cmd.h"
#include "recording/draw_cmd_list.h"
#include "recording/mask_cmd_list.h"
#include "recording/recording_canvas.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
namespace Drawing {
class DrawCmdTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void DrawCmdTest::SetUpTestCase() {}
void DrawCmdTest::TearDownTestCase() {}
void DrawCmdTest::SetUp() {}
void DrawCmdTest::TearDown() {}

/**
 * @tc.name: DrawCmdList001
 * @tc.desc: Test the creation of CmdList.
 * @tc.type: FUNC
 * @tc.require: I7SO7X
 */
HWTEST_F(DrawCmdTest, DrawCmdList001, TestSize.Level1)
{
    auto drawCmdList = DrawCmdList::CreateFromData({ nullptr, 0 }, false);
    drawCmdList->SetWidth(10);
    drawCmdList->SetHeight(20);
    drawCmdList->AddOp<ClearOpItem::ConstructorHandle>(Color::COLOR_BLACK);
    auto cmdData = drawCmdList->GetData();

    auto newDrawCmdList = DrawCmdList::CreateFromData(cmdData, true);
    EXPECT_TRUE(newDrawCmdList != nullptr);
    newDrawCmdList->SetWidth(drawCmdList->GetWidth());
    newDrawCmdList->SetHeight(drawCmdList->GetHeight());
    EXPECT_EQ(newDrawCmdList->GetWidth(), drawCmdList->GetWidth());
    EXPECT_EQ(newDrawCmdList->GetHeight(), drawCmdList->GetHeight());

    CmdListData cmdListData = { nullptr, 0 };
    newDrawCmdList = DrawCmdList::CreateFromData(cmdListData, false);
    EXPECT_TRUE(newDrawCmdList != nullptr);
    EXPECT_EQ(newDrawCmdList->GetWidth(), 0.f);
    EXPECT_EQ(newDrawCmdList->GetHeight(), 0.f);

    auto imageData = drawCmdList->GetAllImageData();
    auto cmdList = DrawCmdList::CreateFromData(cmdData, false);
    cmdList->SetUpImageData(imageData.first, imageData.second);
}

/**
 * @tc.name: Marshalling000
 * @tc.desc: Test Marshalling
 * @tc.type: FUNC
 * @tc.require: I9120P
 */
HWTEST_F(DrawCmdTest, Marshalling000, TestSize.Level1)
{
    auto drawCmdList = DrawCmdList::CreateFromData({ nullptr, 0 }, false);
    PaintHandle paintHandle;
    DrawPointsOpItem::ConstructorHandle handle{PointMode::POINTS_POINTMODE, {0, 0}, paintHandle};
    DrawPointsOpItem opItem{*drawCmdList, &handle};
    opItem.Marshalling(*drawCmdList);
    auto recordingCanvas = std::make_shared<RecordingCanvas>(10, 10); // 10: width, height
    opItem.Playback(recordingCanvas.get(), nullptr);
}

/**
 * @tc.name: BrushHandleToBrush001
 * @tc.desc: Test BrushHandleToBrush
 * @tc.type: FUNC
 * @tc.require: I9120P
 */
HWTEST_F(DrawCmdTest, BrushHandleToBrush001, TestSize.Level1)
{
    BrushHandle brushHandle;
    brushHandle.colorFilterHandle.size = 1;
    brushHandle.colorSpaceHandle.size = 1;
    brushHandle.shaderEffectHandle.size = 1;
    brushHandle.imageFilterHandle.size = 1;
    brushHandle.maskFilterHandle.size = 1;
    auto drawCmdList = DrawCmdList::CreateFromData({ nullptr, 0 }, false);
    Brush brush;
    DrawOpItem::BrushHandleToBrush(brushHandle, *drawCmdList, brush);
}

/**
 * @tc.name: GeneratePaintFromHandle001
 * @tc.desc: Test GeneratePaintFromHandle
 * @tc.type: FUNC
 * @tc.require: I9120P
 */
HWTEST_F(DrawCmdTest, GeneratePaintFromHandle001, TestSize.Level1)
{
    PaintHandle paintHandle;
    paintHandle.colorSpaceHandle.size = 1;
    paintHandle.imageFilterHandle.size = 1;
    paintHandle.pathEffectHandle.size = 1;
    auto drawCmdList = DrawCmdList::CreateFromData({ nullptr, 0 }, false);
    Paint paint;
    paint.SetStyle(Paint::PaintStyle::PAINT_FILL_STROKE);
    DrawOpItem::GeneratePaintFromHandle(paintHandle, *drawCmdList, paint);
}

/**
 * @tc.name: GenerateHandleFromPaint001
 * @tc.desc: Test GenerateHandleFromPaint
 * @tc.type: FUNC
 * @tc.require: I9120P
 */
HWTEST_F(DrawCmdTest, GenerateHandleFromPaint001, TestSize.Level1)
{
    PaintHandle paintHandle;
    auto drawCmdList = DrawCmdList::CreateFromData({ nullptr, 0 }, false);
    Paint paint;
    Filter filter;
    paint.SetFilter(filter);
    auto space = std::make_shared<ColorSpace>();
    Color4f color;
    paint.SetColor(color, space);
    auto pathEffect = PathEffect::CreateCornerPathEffect(10);
    paint.SetPathEffect(pathEffect);
    paint.SetShaderEffect(ShaderEffect::CreateColorShader(0xFF000000));
    paint.SetStyle(Paint::PaintStyle::PAINT_FILL_STROKE);
    DrawOpItem::GenerateHandleFromPaint(*drawCmdList, paint, paintHandle);
}

/**
 * @tc.name: GenerateCachedOpItem001
 * @tc.desc: Test GenerateCachedOpItem
 * @tc.type: FUNC
 * @tc.require: I9120P
 */
HWTEST_F(DrawCmdTest, GenerateCachedOpItem001, TestSize.Level1)
{
    auto drawCmdList = DrawCmdList::CreateFromData({ nullptr, 0 }, false);
    GenerateCachedOpItemPlayer player{*drawCmdList, nullptr, nullptr};
    player.GenerateCachedOpItem(DrawOpItem::TEXT_BLOB_OPITEM, nullptr);
    OpDataHandle opDataHandle;
    OpDataHandle typefaceHandle;
    PaintHandle paintHandle;
    DrawTextBlobOpItem::ConstructorHandle handle{opDataHandle,
        typefaceHandle, 0, 0, paintHandle};
    player.GenerateCachedOpItem(DrawOpItem::TEXT_BLOB_OPITEM, &handle);
    player.GenerateCachedOpItem(DrawOpItem::PICTURE_OPITEM, &handle);
}

/**
 * @tc.name: DrawShadowOpItem001
 * @tc.desc: Test DrawShadowOpItem
 * @tc.type: FUNC
 * @tc.require: I9120P
 */
HWTEST_F(DrawCmdTest, DrawShadowOpItem001, TestSize.Level1)
{
    Path path;
    Point3 planeParams;
    Point3 devLightPos;
    Color ambientColor = 0xFF000000;
    Color spotColor = 0xFF000000;
    DrawShadowOpItem opItem{path, planeParams, devLightPos,
        10, ambientColor, spotColor, ShadowFlags::NONE}; // 10: lightRadius
    auto drawCmdList = DrawCmdList::CreateFromData({ nullptr, 0 }, false);
    opItem.Marshalling(*drawCmdList);
}

/**
 * @tc.name: ClipPathOpItem001
 * @tc.desc: Test ClipPathOpItem
 * @tc.type: FUNC
 * @tc.require: I9120P
 */
HWTEST_F(DrawCmdTest, ClipPathOpItem001, TestSize.Level1)
{
    Path path;
    ClipPathOpItem opItem{path, ClipOp::DIFFERENCE, true};
    auto drawCmdList = DrawCmdList::CreateFromData({ nullptr, 0 }, false);
    opItem.Marshalling(*drawCmdList);
}

/**
 * @tc.name: SaveLayerOpItem001
 * @tc.desc: Test SaveLayerOpItem
 * @tc.type: FUNC
 * @tc.require: I9120P
 */
HWTEST_F(DrawCmdTest, SaveLayerOpItem001, TestSize.Level1)
{
    SaveLayerOps ops;
    SaveLayerOpItem opItem{ops};
    auto drawCmdList = DrawCmdList::CreateFromData({ nullptr, 0 }, false);
    opItem.Marshalling(*drawCmdList);
    auto recordingCanvas = std::make_shared<RecordingCanvas>(10, 10); // 10: width, height
    Rect rect1;
    opItem.Playback(recordingCanvas.get(), &rect1);
    Rect rect2{0, 0, 100, 100}; // 100: right, bottom
    opItem.Playback(recordingCanvas.get(), &rect2);

    Rect bounds;
    Brush brush;
    SaveLayerOps ops2{&bounds, &brush, 0};
    SaveLayerOpItem opItem2{ops2};
    opItem2.Marshalling(*drawCmdList);
    opItem2.Playback(recordingCanvas.get(), &rect1);
}

/**
 * @tc.name: DrawSymbolOpItem001
 * @tc.desc: Test DrawSymbolOpItem
 * @tc.type: FUNC
 * @tc.require: I9120P
 */
HWTEST_F(DrawCmdTest, DrawSymbolOpItem001, TestSize.Level1)
{
    auto drawCmdList = DrawCmdList::CreateFromData({ nullptr, 0 }, false);
    DrawingHMSymbolData drawingHMSymbolData;
    DrawingSymbolLayers symbolInfo;
    DrawingRenderGroup groups;
    DrawingGroupInfo info;
    info.layerIndexes = {1, 4};
    info.maskIndexes = {1, 4};
    groups.groupInfos = {info};
    symbolInfo.renderGroups = {groups};
    symbolInfo.layers = {{1}, {1}, {1}};
    drawingHMSymbolData.symbolInfo_ = symbolInfo;
    Point point;
    Paint paint;
    DrawSymbolOpItem opItem{drawingHMSymbolData, point, paint};
    opItem.SetSymbol();
    opItem.InitialScale();
    opItem.InitialVariableColor();
    opItem.SetSymbol();
    opItem.SetScale(0);
    opItem.SetVariableColor(0);
    Path path;
    opItem.UpdateScale(100, path); // 100: cur
    opItem.UpdataVariableColor(100, 0); // 100: cur
    opItem.Marshalling(*drawCmdList);
    SymbolOpHandle symbolOpHandle;
    PaintHandle paintHandle;
    Point locate;
    DrawSymbolOpItem::ConstructorHandle handle{symbolOpHandle, locate, paintHandle};
    opItem.Unmarshalling(*drawCmdList, &handle);
    auto recordingCanvas = std::make_shared<RecordingCanvas>(10, 10); // 10: width, height
    opItem.Playback(recordingCanvas.get(), nullptr);
    opItem.Playback(nullptr, nullptr);
    
    DrawingHMSymbolData drawingHMSymbolData2;
    drawingHMSymbolData2.symbolInfo_.effect = DrawingEffectStrategy::HIERARCHICAL;
    DrawSymbolOpItem opItem2{drawingHMSymbolData2, point, paint};
    opItem2.Playback(recordingCanvas.get(), nullptr);
    opItem2.SetScale(10); // 10: scale
    opItem2.SetVariableColor(5); // 5: index
    opItem2.SetScale(0);
    opItem2.SetScale(10); // 10: scale
    opItem2.InitialScale();
    opItem2.SetScale(0);
    opItem2.InitialScale();
    opItem2.SetScale(0);
}

/**
 * @tc.name: Marshalling001
 * @tc.desc: Test Marshalling for DrawPointOpItem
 * @tc.type: FUNC
 * @tc.require: I9120P
 */
HWTEST_F(DrawCmdTest, Marshalling001, TestSize.Level1)
{
    auto drawCmdList = DrawCmdList::CreateFromData({ nullptr, 0 }, false);
    Point point;
    PaintHandle paintHandle;
    DrawPointOpItem::ConstructorHandle handle{point, paintHandle};
    DrawPointOpItem opItem{*drawCmdList, &handle};
    opItem.Marshalling(*drawCmdList);
}

/**
 * @tc.name: Marshalling002
 * @tc.desc: Test Marshalling for DrawPieOpItem
 * @tc.type: FUNC
 * @tc.require: I9120P
 */
HWTEST_F(DrawCmdTest, Marshalling002, TestSize.Level1)
{
    auto drawCmdList = DrawCmdList::CreateFromData({ nullptr, 0 }, false);
    Rect rect;
    PaintHandle paintHandle;
    DrawPieOpItem::ConstructorHandle handle{rect, 0, 0, paintHandle};
    DrawPieOpItem opItem{*drawCmdList, &handle};
    opItem.Marshalling(*drawCmdList);
}

/**
 * @tc.name: Marshalling003
 * @tc.desc: Test Marshalling for DrawOvalOpItem
 * @tc.type: FUNC
 * @tc.require: I9120P
 */
HWTEST_F(DrawCmdTest, Marshalling003, TestSize.Level1)
{
    auto drawCmdList = DrawCmdList::CreateFromData({ nullptr, 0 }, false);
    Rect rect;
    PaintHandle paintHandle;
    DrawOvalOpItem::ConstructorHandle handle{rect, paintHandle};
    DrawOvalOpItem opItem{*drawCmdList, &handle};
    opItem.Marshalling(*drawCmdList);
}

/**
 * @tc.name: Marshalling004
 * @tc.desc: Test Marshalling for DrawCircleOpItem
 * @tc.type: FUNC
 * @tc.require: I9120P
 */
HWTEST_F(DrawCmdTest, Marshalling004, TestSize.Level1)
{
    auto drawCmdList = DrawCmdList::CreateFromData({ nullptr, 0 }, false);
    Point point;
    PaintHandle paintHandle;
    DrawCircleOpItem::ConstructorHandle handle{point, 100, paintHandle}; // 100: radius
    DrawCircleOpItem opItem{*drawCmdList, &handle};
    opItem.Marshalling(*drawCmdList);
}

/**
 * @tc.name: Marshalling005
 * @tc.desc: Test Marshalling for DrawPathOpItem
 * @tc.type: FUNC
 * @tc.require: I9120P
 */
HWTEST_F(DrawCmdTest, Marshalling005, TestSize.Level1)
{
    auto drawCmdList = DrawCmdList::CreateFromData({ nullptr, 0 }, false);
    Path path;
    Paint paint;
    DrawPathOpItem opItem{path, paint};
    opItem.Marshalling(*drawCmdList);
}

/**
 * @tc.name: Marshalling006
 * @tc.desc: Test Marshalling for DrawBackgroundOpItem
 * @tc.type: FUNC
 * @tc.require: I9120P
 */
HWTEST_F(DrawCmdTest, Marshalling006, TestSize.Level1)
{
    auto drawCmdList = DrawCmdList::CreateFromData({ nullptr, 0 }, false);
    BrushHandle brushHandle;
    DrawBackgroundOpItem::ConstructorHandle handle{brushHandle};
    DrawBackgroundOpItem opItem{*drawCmdList, &handle};
    opItem.Marshalling(*drawCmdList);
}

/**
 * @tc.name: Marshalling007
 * @tc.desc: Test Marshalling for DrawRegionOpItem
 * @tc.type: FUNC
 * @tc.require: I9120P
 */
HWTEST_F(DrawCmdTest, Marshalling007, TestSize.Level1)
{
    auto drawCmdList = DrawCmdList::CreateFromData({ nullptr, 0 }, false);
    Region region;
    Paint paint;
    DrawRegionOpItem opItem{region, paint};
    opItem.Marshalling(*drawCmdList);
}

/**
 * @tc.name: Marshalling008
 * @tc.desc: Test Marshalling for DrawVerticesOpItem
 * @tc.type: FUNC
 * @tc.require: I9120P
 */
HWTEST_F(DrawCmdTest, Marshalling008, TestSize.Level1)
{
    auto drawCmdList = DrawCmdList::CreateFromData({ nullptr, 0 }, false);
    Vertices vertices;
    Paint paint;
    DrawVerticesOpItem opItem{vertices, BlendMode::SRC_OVER, paint};
    opItem.Marshalling(*drawCmdList);
    auto recordingCanvas = std::make_shared<RecordingCanvas>(10, 10); // 10: width, height
    opItem.Playback(recordingCanvas.get(), nullptr);
}

/**
 * @tc.name: Marshalling009
 * @tc.desc: Test Marshalling for DrawColorOpItem
 * @tc.type: FUNC
 * @tc.require: I9120P
 */
HWTEST_F(DrawCmdTest, Marshalling009, TestSize.Level1)
{
    auto drawCmdList = DrawCmdList::CreateFromData({ nullptr, 0 }, false);
    ColorQuad colorQuad{0};
    DrawColorOpItem::ConstructorHandle handle{colorQuad, BlendMode::SRC_OVER};
    DrawColorOpItem opItem{&handle};
    opItem.Marshalling(*drawCmdList);
    auto recordingCanvas = std::make_shared<RecordingCanvas>(10, 10); // 10: width, height
    opItem.Playback(recordingCanvas.get(), nullptr);
}

/**
 * @tc.name: Marshalling010
 * @tc.desc: Test Marshalling for DrawImageNineOpItem
 * @tc.type: FUNC
 * @tc.require: I9120P
 */
HWTEST_F(DrawCmdTest, Marshalling010, TestSize.Level1)
{
    auto drawCmdList = DrawCmdList::CreateFromData({ nullptr, 0 }, false);
    OpDataHandle opDataHandle;
    RectI recti;
    Rect rect;
    BrushHandle brushHandle;
    DrawImageNineOpItem::ConstructorHandle handle{opDataHandle, recti, rect, FilterMode::NEAREST, brushHandle, true};
    Image image;
    Brush brush;
    DrawImageNineOpItem opItem{&image, recti, rect, FilterMode::NEAREST, &brush};
    opItem.Marshalling(*drawCmdList);
    opItem.Unmarshalling(*drawCmdList, &handle);
    auto recordingCanvas = std::make_shared<RecordingCanvas>(10, 10); // 10: width, height
    opItem.Playback(recordingCanvas.get(), nullptr);
    DrawImageNineOpItem opItem2{&image, recti, rect, FilterMode::NEAREST, nullptr};
    opItem2.Marshalling(*drawCmdList);
}

/**
 * @tc.name: Marshalling011
 * @tc.desc: Test Marshalling for DrawImageLatticeOpItem
 * @tc.type: FUNC
 * @tc.require: I9120P
 */
HWTEST_F(DrawCmdTest, Marshalling011, TestSize.Level1)
{
    auto drawCmdList = DrawCmdList::CreateFromData({ nullptr, 0 }, false);
    Image image;
    Lattice lattice;
    Rect dst;
    Brush brush;
    DrawImageLatticeOpItem opItem{&image, lattice, dst, FilterMode::NEAREST, &brush};
    opItem.Marshalling(*drawCmdList);
    auto recordingCanvas = std::make_shared<RecordingCanvas>(10, 10); // 10: width, height
    opItem.Playback(recordingCanvas.get(), nullptr);
    DrawImageLatticeOpItem opItem2{&image, lattice, dst, FilterMode::NEAREST, nullptr};
    opItem2.Marshalling(*drawCmdList);
    opItem2.Playback(recordingCanvas.get(), nullptr);
}

/**
 * @tc.name: Marshalling012
 * @tc.desc: Test Marshalling for DrawBitmapOpItem
 * @tc.type: FUNC
 * @tc.require: I9120P
 */
HWTEST_F(DrawCmdTest, Marshalling012, TestSize.Level1)
{
    auto drawCmdList = DrawCmdList::CreateFromData({ nullptr, 0 }, false);
    Bitmap bitmap;
    Paint paint;
    DrawBitmapOpItem opItem{bitmap, 0, 0, paint};
    opItem.Marshalling(*drawCmdList);
}

/**
 * @tc.name: Marshalling013
 * @tc.desc: Test Marshalling for DrawImageOpItem
 * @tc.type: FUNC
 * @tc.require: I9120P
 */
HWTEST_F(DrawCmdTest, Marshalling013, TestSize.Level1)
{
    auto drawCmdList = DrawCmdList::CreateFromData({ nullptr, 0 }, false);
    Image image;
    SamplingOptions options;
    Paint paint;
    DrawImageOpItem opItem{image, 0, 0, options, paint};
    opItem.Marshalling(*drawCmdList);
}

/**
 * @tc.name: Marshalling014
 * @tc.desc: Test Marshalling for DrawImageRectOpItem
 * @tc.type: FUNC
 * @tc.require: I9120P
 */
HWTEST_F(DrawCmdTest, Marshalling014, TestSize.Level1)
{
    auto drawCmdList = DrawCmdList::CreateFromData({ nullptr, 0 }, false);
    Image image;
    Rect src;
    Rect dst;
    SamplingOptions samplingOptions;
    Paint paint;
    DrawImageRectOpItem opItem{image, src, dst, samplingOptions,
        SrcRectConstraint::STRICT_SRC_RECT_CONSTRAINT, paint, false};
    opItem.Marshalling(*drawCmdList);
    DrawImageRectOpItem opItem2{image, src, dst, samplingOptions,
        SrcRectConstraint::STRICT_SRC_RECT_CONSTRAINT, paint, true};
    opItem2.Marshalling(*drawCmdList);
    auto recordingCanvas = std::make_shared<RecordingCanvas>(10, 10); // 10: width, height
    Rect rect;
    opItem2.Playback(recordingCanvas.get(), &rect);
}

/**
 * @tc.name: Marshalling015
 * @tc.desc: Test Marshalling for DrawPictureOpItem
 * @tc.type: FUNC
 * @tc.require: I9120P
 */
HWTEST_F(DrawCmdTest, Marshalling015, TestSize.Level1)
{
    auto drawCmdList = DrawCmdList::CreateFromData({ nullptr, 0 }, false);
    Picture picture;
    DrawPictureOpItem opItem{picture};
    opItem.Marshalling(*drawCmdList);
}

/**
 * @tc.name: DrawTextBlobOpItem001
 * @tc.desc: Test functions for DrawTextBlobOpItem
 * @tc.type: FUNC
 * @tc.require: I9120P
 */
HWTEST_F(DrawCmdTest, DrawTextBlobOpItem001, TestSize.Level1)
{
    auto drawCmdList = DrawCmdList::CreateFromData({ nullptr, 0 }, false);
    Font font;
    auto textBlob = TextBlob::MakeFromString("11", font, TextEncoding::UTF8);
    Paint paint;
    auto space = std::make_shared<ColorSpace>();
    Color4f color;
    color.alphaF_ = 0;
    color.blueF_ = 1;
    paint.SetColor(color, space);
    DrawTextBlobOpItem opItem{textBlob.get(), 0, 0, paint};
    opItem.Marshalling(*drawCmdList);
    auto recordingCanvas = std::make_shared<RecordingCanvas>(1, 10); // 1: width, 10: height
    Rect rect;
    opItem.Playback(recordingCanvas.get(), &rect);
    Canvas canvas;
    opItem.GenerateCachedOpItem(&canvas);
    auto recordingCanvas2 = std::make_shared<RecordingCanvas>(10, 10); // 10: width, height
    opItem.Playback(recordingCanvas2.get(), &rect);

    DrawTextBlobOpItem::ConstructorHandle::GenerateCachedOpItem(*drawCmdList, nullptr, 0, 0, paint);
    DrawTextBlobOpItem::ConstructorHandle::GenerateCachedOpItem(*drawCmdList, textBlob.get(), 0, 0, paint);
    TextBlob textBlob2{nullptr};
    DrawTextBlobOpItem::ConstructorHandle::GenerateCachedOpItem(*drawCmdList, &textBlob2, 0, 0, paint);

    auto opDataHandle = CmdListHelper::AddTextBlobToCmdList(*drawCmdList, textBlob.get());
    opDataHandle.offset = 2;
    opDataHandle.size = 10;
    OpDataHandle typefaceHandle;
    PaintHandle paintHandle;
    DrawTextBlobOpItem::ConstructorHandle handler{opDataHandle, typefaceHandle, 10, 10, paintHandle}; // 10: x, y
    handler.GenerateCachedOpItem(*drawCmdList, &canvas);
}

/**
 * @tc.name: DrawCmdList002
 * @tc.desc: Test function for DrawCmdList
 * @tc.type: FUNC
 * @tc.require: I9120P
 */
HWTEST_F(DrawCmdTest, DrawCmdList002, TestSize.Level1)
{
    auto drawCmdList = std::make_shared<DrawCmdList>(DrawCmdList::UnmarshalMode::DEFERRED);
    ColorQuad color = 0xFF000000;
    drawCmdList->AddOp<DrawColorOpItem::ConstructorHandle>(color, BlendMode::SRC_OVER);
    EXPECT_TRUE(drawCmdList->IsEmpty());
    drawCmdList->SetIsCache(true);
    EXPECT_TRUE(drawCmdList->GetIsCache());
    drawCmdList->SetCachedHighContrast(true);
    EXPECT_TRUE(drawCmdList->GetCachedHighContrast());
    EXPECT_TRUE(drawCmdList->GetOpItemSize() >= 0);
    std::string s = "";
    s = drawCmdList->GetOpsWithDesc();
    drawCmdList->ClearOp();

    auto drawCmdList2 = std::make_shared<DrawCmdList>(
        10, 10, DrawCmdList::UnmarshalMode::IMMEDIATE); // 10: width, height
    EXPECT_TRUE(drawCmdList2->IsEmpty());
    EXPECT_TRUE(drawCmdList->GetOpItemSize() >= 0);
    drawCmdList->AddDrawOp(nullptr);
    EXPECT_TRUE(drawCmdList2->IsEmpty());
}

/**
 * @tc.name: GetOpsWithDesc001
 * @tc.desc: Test GetOpsWithDesc
 * @tc.type: FUNC
 * @tc.require: I9120P
 */
HWTEST_F(DrawCmdTest, GetOpsWithDesc001, TestSize.Level1)
{
    auto drawCmdList = std::make_shared<DrawCmdList>(DrawCmdList::UnmarshalMode::DEFERRED);
    Brush brush;
    drawCmdList->AddDrawOp(std::make_shared<DrawBackgroundOpItem>(brush));
    drawCmdList->AddDrawOp(nullptr);

    std::string s = "";
    s = drawCmdList->GetOpsWithDesc();
    drawCmdList->ClearOp();
}

/**
 * @tc.name: MarshallingDrawOps001
 * @tc.desc: Test MarshallingDrawOps
 * @tc.type: FUNC
 * @tc.require: I9120P
 */
HWTEST_F(DrawCmdTest, MarshallingDrawOps001, TestSize.Level1)
{
    auto drawCmdList1 = std::make_shared<DrawCmdList>(DrawCmdList::UnmarshalMode::IMMEDIATE);
    drawCmdList1->MarshallingDrawOps();
    drawCmdList1->UnmarshallingDrawOps();
    drawCmdList1->AddDrawOp(nullptr);
    auto drawCmdList2 = std::make_shared<DrawCmdList>(DrawCmdList::UnmarshalMode::DEFERRED);
    drawCmdList2->MarshallingDrawOps();
    drawCmdList2->AddDrawOp(nullptr);
    Brush brush;
    drawCmdList2->AddDrawOp(std::make_shared<DrawBackgroundOpItem>(brush));
    drawCmdList2->UnmarshallingDrawOps();
}

/**
 * @tc.name: Playback001
 * @tc.desc: Test Playback for DrawCmdList
 * @tc.type: FUNC
 * @tc.require: I9120P
 */
HWTEST_F(DrawCmdTest, Playback001, TestSize.Level1)
{
    auto drawCmdList = std::make_shared<DrawCmdList>(DrawCmdList::UnmarshalMode::IMMEDIATE);
    drawCmdList->SetWidth(0);
    Canvas canvas;
    Rect rect;
    drawCmdList->Playback(canvas, &rect);
    drawCmdList->SetWidth(10);
    drawCmdList->SetHeight(10);
    drawCmdList->SetCachedHighContrast(false);
    drawCmdList->Playback(canvas, &rect);
    auto recordingCanvas = std::make_shared<RecordingCanvas>(10, 10); // 10: width, height
    drawCmdList->Playback(*recordingCanvas, &rect);
    drawCmdList->SetIsCache(true);
    drawCmdList->SetCachedHighContrast(false);
    drawCmdList->Playback(canvas, &rect);
    drawCmdList->SetCachedHighContrast(true);
    drawCmdList->Playback(canvas, &rect);

    auto drawCmdList2 = std::make_shared<DrawCmdList>(10, 10, DrawCmdList::UnmarshalMode::DEFERRED);
    drawCmdList2->Playback(*recordingCanvas, &rect);

    auto drawCmdList3 = DrawCmdList::CreateFromData({ nullptr, 0 }, false);
    drawCmdList3->Playback(*recordingCanvas, &rect);
}

/**
 * @tc.name: GenerateCache001
 * @tc.desc: Test GenerateCache for DrawCmdList
 * @tc.type: FUNC
 * @tc.require: I9120P
 */
HWTEST_F(DrawCmdTest, GenerateCache001, TestSize.Level1)
{
    auto drawCmdList = std::make_shared<DrawCmdList>(DrawCmdList::UnmarshalMode::IMMEDIATE);
    drawCmdList->SetIsCache(true);
    auto recordingCanvas = std::make_shared<RecordingCanvas>(10, 10); // 10: width, height
    Rect rect;
    drawCmdList->Playback(*recordingCanvas, &rect);
    drawCmdList->GenerateCache(recordingCanvas.get(), &rect);
    drawCmdList->SetIsCache(false);
    drawCmdList->GenerateCache(recordingCanvas.get(), &rect);
    Brush brush;
    drawCmdList->AddDrawOp(std::make_shared<DrawBackgroundOpItem>(brush));
    Font font;
    auto textBlob = TextBlob::MakeFromString("11", font, TextEncoding::UTF8);
    Paint paint;
    auto space = std::make_shared<ColorSpace>();
    Color4f color;
    color.alphaF_ = 0;
    color.blueF_ = 1;
    paint.SetColor(color, space);
    auto opItem = std::make_shared<DrawTextBlobOpItem>(textBlob.get(), 0, 0, paint);
    drawCmdList->AddDrawOp(opItem);
    drawCmdList->GenerateCache(recordingCanvas.get(), &rect);
    drawCmdList->Playback(*recordingCanvas, &rect);
    drawCmdList->MarshallingDrawOps();
    drawCmdList->AddDrawOp(nullptr);
    drawCmdList->UnmarshallingDrawOps();
    
    auto drawCmdList2 = std::make_shared<DrawCmdList>(DrawCmdList::UnmarshalMode::DEFERRED);
    drawCmdList2->SetIsCache(false);
    drawCmdList2->GenerateCache(recordingCanvas.get(), &rect);
    drawCmdList2->AddDrawOp(nullptr);
    drawCmdList2->AddDrawOp(std::make_shared<DrawBackgroundOpItem>(brush));
    drawCmdList2->AddDrawOp(opItem);
    drawCmdList2->GenerateCache(recordingCanvas.get(), &rect);
}

/**
 * @tc.name: UpdateNodeIdToPicture001
 * @tc.desc: Test UpdateNodeIdToPicture
 * @tc.type: FUNC
 * @tc.require: I9120P
 */
HWTEST_F(DrawCmdTest, UpdateNodeIdToPicture001, TestSize.Level1)
{
    auto drawCmdList = DrawCmdList::CreateFromData({ nullptr, 0 }, false);
    NodeId nodeId = 100;
    drawCmdList->AddDrawOp(nullptr);
    Brush brush;
    drawCmdList->AddDrawOp(std::make_shared<DrawBackgroundOpItem>(brush));
    drawCmdList->UpdateNodeIdToPicture(nodeId);
}

/**
 * @tc.name: MaskCmdList001
 * @tc.desc: Test functions for MaskCmdList
 * @tc.type: FUNC
 * @tc.require: I9120P
 */
HWTEST_F(DrawCmdTest, MaskCmdList001, TestSize.Level1)
{
    Drawing::CmdListData listData;
    auto maskCmdList = MaskCmdList::CreateFromData(listData, true);
    auto path = std::make_shared<Path>();
    Brush brush;
    maskCmdList->Playback(path, brush);
    Pen pen;
    maskCmdList->Playback(path, pen, brush);

    auto maskCmdList2 = MaskCmdList::CreateFromData(listData, false);
    maskCmdList2->Playback(path, brush);
    maskCmdList2->Playback(path, pen, brush);
}

#ifdef ROSEN_OHOS
/**
 * @tc.name: SurfaceBuffer001
 * @tc.desc: Test SurfaceBuffer
 * @tc.type: FUNC
 * @tc.require: I9120P
 */
HWTEST_F(DrawCmdTest, SurfaceBuffer001, TestSize.Level1)
{
    auto drawCmdList = DrawCmdList::CreateFromData({ nullptr, 0 }, false);
    sptr<SurfaceBuffer> surfaceBuffer;
    drawCmdList->AddSurfaceBuffer(surfaceBuffer);
    EXPECT_TRUE(drawCmdList->GetSurfaceBuffer(0) == nullptr);
    EXPECT_TRUE(drawCmdList->GetSurfaceBuffer(10) == nullptr);
    std::vector<sptr<SurfaceBuffer>> surfaceBufferVec;
    uint32_t surfaceBufferSize = drawCmdList->GetAllSurfaceBuffer(surfaceBufferVec);
    EXPECT_TRUE(surfaceBufferSize >= 0);
}
#endif
} // namespace Drawing
} // namespace Rosen
} // namespace OHOS