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
#include "rs_test_util.h"

#include "pipeline/rs_main_thread.h"
#include "pipeline/rs_uni_render_util.h"
#include "params/rs_surface_render_params.h"
#include "surface_buffer_impl.h"
#include "render/rs_shadow.h"
#include "render/rs_material_filter.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Rosen {
class RSUniRenderUtilTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;

private:
    static std::shared_ptr<Drawing::Image> CreateSkImage();
};

std::shared_ptr<Drawing::Image> CreateSkImage()
{
    const Drawing::ImageInfo info =
    Drawing::ImageInfo{200, 200, Drawing::COLORTYPE_N32, Drawing::ALPHATYPE_OPAQUE };
    auto surface = Drawing::Surface::MakeRaster(info);
    auto canvas = surface->GetCanvas();
    canvas->Clear(Drawing::Color::COLOR_YELLOW);
    Drawing::Brush paint;
    paint.SetColor(Drawing::Color::COLOR_RED);
    canvas->AttachBrush(paint);
    canvas->DrawRect(Drawing::Rect(50, 50, 100, 100));
    canvas->DetachBrush();
    return surface->GetImageSnapshot();
}

void RSUniRenderUtilTest::SetUpTestCase() {}
void RSUniRenderUtilTest::TearDownTestCase() {}
void RSUniRenderUtilTest::SetUp() {}
void RSUniRenderUtilTest::TearDown() {}

/*
 * @tc.name: MergeDirtyHistory
 * @tc.desc: default value
 * @tc.type: FUNC
 * @tc.require:
*/
HWTEST_F(RSUniRenderUtilTest, MergeDirtyHistory, Function | SmallTest | Level2)
{
    NodeId id = 0;
    RSDisplayNodeConfig config;
    auto node = std::make_shared<RSDisplayRenderNode>(id, config);
    auto rsRenderNode = std::make_shared<RSRenderNode>(id);
    node->renderDrawable_ = std::make_shared<DrawableV2::RSRenderNodeDrawable>(rsRenderNode);
    node->renderDrawable_->renderParams_ = std::make_unique<RSRenderParams>(id);
    int32_t bufferAge = 0;
    RSUniRenderUtil::MergeDirtyHistory(node, bufferAge);
}

/*
 * @tc.name: SrcRectScaleDown_001
 * @tc.desc: default value
 * @tc.type: FUNC
 * @tc.require:
*/
HWTEST_F(RSUniRenderUtilTest, SrcRectScaleDown_001, Function | SmallTest | Level2)
{
    auto rsSurfaceRenderNode = RSTestUtil::CreateSurfaceNode();
    ASSERT_NE(rsSurfaceRenderNode, nullptr);
    RSSurfaceRenderNode& node = static_cast<RSSurfaceRenderNode&>(*(rsSurfaceRenderNode.get()));
    BufferDrawParam params;
    RectF localBounds;
    RSUniRenderUtil::SrcRectScaleDown(params, node.GetBuffer(), node.GetConsumer(), localBounds);
}

/*
 * @tc.name: SrcRectScaleDown_002
 * @tc.desc: default value
 * @tc.type: FUNC
 * @tc.require:
*/
HWTEST_F(RSUniRenderUtilTest, SrcRectScaleDown_002, Function | SmallTest | Level2)
{
    auto rsSurfaceRenderNode = RSTestUtil::CreateSurfaceNodeWithBuffer();
    ASSERT_NE(rsSurfaceRenderNode, nullptr);
    RSSurfaceRenderNode& node = static_cast<RSSurfaceRenderNode&>(*(rsSurfaceRenderNode.get()));
    BufferDrawParam params;
    RectF localBounds;
    RSUniRenderUtil::SrcRectScaleDown(params, node.GetBuffer(), node.GetConsumer(), localBounds);
}

/*
 * @tc.name: GetMatrixOfBufferToRelRect_001
 * @tc.desc: test GetMatrixOfBufferToRelRect with surfaceNode without buffer
 * @tc.type: FUNC
 * @tc.require: #I9E60C
 */
HWTEST_F(RSUniRenderUtilTest, GetMatrixOfBufferToRelRect_001, Function | SmallTest | Level2)
{
    auto rsSurfaceRenderNode = RSTestUtil::CreateSurfaceNode();
    ASSERT_NE(rsSurfaceRenderNode, nullptr);
    RSSurfaceRenderNode& node = static_cast<RSSurfaceRenderNode&>(*(rsSurfaceRenderNode.get()));
    RSUniRenderUtil::GetMatrixOfBufferToRelRect(node);
}

/*
 * @tc.name: GetMatrixOfBufferToRelRect_002
 * @tc.desc: test GetMatrixOfBufferToRelRect with surfaceNode with buffer
 * @tc.type: FUNC
 * @tc.require: #I9E60C
 */
HWTEST_F(RSUniRenderUtilTest, GetMatrixOfBufferToRelRect_002, Function | SmallTest | Level2)
{
    auto rsSurfaceRenderNode = RSTestUtil::CreateSurfaceNodeWithBuffer();
    ASSERT_NE(rsSurfaceRenderNode, nullptr);
    RSSurfaceRenderNode& node = static_cast<RSSurfaceRenderNode&>(*(rsSurfaceRenderNode.get()));
    RSUniRenderUtil::GetMatrixOfBufferToRelRect(node);
}

/*
 * @tc.name: CreateBufferDrawParam_002
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSUniRenderUtilTest, CreateBufferDrawParam_002, Function | SmallTest | Level2)
{
    NodeId id = 0;
    bool forceCPU = false;
    RSDisplayNodeConfig config;
    RSDisplayRenderNode node(id, config);
    node.buffer_.buffer = OHOS::SurfaceBuffer::Create();
    EXPECT_TRUE(RSUniRenderUtil::CreateBufferDrawParam(node, forceCPU).buffer);
}

/*
 * @tc.name: CreateLayerBufferDrawParam_001
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSUniRenderUtilTest, CreateLayerBufferDrawParam_001, Function | SmallTest | Level2)
{
    bool forceCPU = false;
    LayerInfoPtr layer = HdiLayerInfo::CreateHdiLayerInfo();
    RSUniRenderUtil::CreateLayerBufferDrawParam(layer, forceCPU);
}

/*
 * @tc.name: CreateLayerBufferDrawParam_002
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSUniRenderUtilTest, CreateLayerBufferDrawParam_002, Function | SmallTest | Level2)
{
    bool forceCPU = false;
    auto surfaceNode = RSTestUtil::CreateSurfaceNodeWithBuffer();
    ASSERT_NE(surfaceNode, nullptr);
    auto buffer = surfaceNode->GetBuffer();
    LayerInfoPtr layer = HdiLayerInfo::CreateHdiLayerInfo();
    layer->SetBuffer(buffer, surfaceNode->GetAcquireFence());
    RSUniRenderUtil::CreateLayerBufferDrawParam(layer, forceCPU);
}

/*
 * @tc.name: AlignedDirtyRegion_001
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSUniRenderUtilTest, AlignedDirtyRegion_001, Function | SmallTest | Level2)
{
    Occlusion::Region dirtyRegion;
    int32_t alignedBits = 0;
    Occlusion::Region alignedRegion = RSUniRenderUtil::AlignedDirtyRegion(dirtyRegion, alignedBits);
    ASSERT_EQ(dirtyRegion.GetSize(), alignedRegion.GetSize());
    for (size_t i = 0; i < dirtyRegion.GetSize(); i++) {
        ASSERT_TRUE(dirtyRegion.GetRegionRects()[i] == alignedRegion.GetRegionRects()[i]);
    }
}

/*
 * @tc.name: AlignedDirtyRegion_002
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSUniRenderUtilTest, AlignedDirtyRegion_002, Function | SmallTest | Level2)
{
    Occlusion::Region dirtyRegion;
    int32_t alignedBits = 2;
    Occlusion::Region alignedRegion = RSUniRenderUtil::AlignedDirtyRegion(dirtyRegion, alignedBits);
    ASSERT_EQ(dirtyRegion.GetSize(), alignedRegion.GetSize());
    for (size_t i = 0; i < dirtyRegion.GetSize(); i++) {
        ASSERT_TRUE(dirtyRegion.GetRegionRects()[i] != alignedRegion.GetRegionRects()[i]);
    }
}

/*
 * @tc.name: GetRotationFromMatrix
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSUniRenderUtilTest, GetRotationFromMatrix, Function | SmallTest | Level2)
{
    int angle;
    Drawing::Matrix matrix = Drawing::Matrix();
    matrix.SetMatrix(1, 0, 0, 0, 1, 0, 0, 0, 1);
    angle = RSUniRenderUtil::GetRotationFromMatrix(matrix);
    ASSERT_EQ(angle, 0);
}

/*
 * @tc.name: GetRotationDegreeFromMatrix
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSUniRenderUtilTest, GetRotationDegreeFromMatrix, Function | SmallTest | Level2)
{
    int angle;
    Drawing::Matrix matrix = Drawing::Matrix();
    matrix.SetMatrix(1, 0, 0, 0, 1, 0, 0, 0, 1);
    angle = RSUniRenderUtil::GetRotationDegreeFromMatrix(matrix);
    ASSERT_EQ(angle, 0);
}

/*
 * @tc.name: Is3DRotation_001
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSUniRenderUtilTest, Is3DRotation_001, Function | SmallTest | Level2)
{
    bool is3DRotation;
    Drawing::Matrix matrix = Drawing::Matrix();
    matrix.SetMatrix(1, 0, 0, 0, 1, 0, 0, 0, 1);
    is3DRotation = RSUniRenderUtil::Is3DRotation(matrix);
    ASSERT_FALSE(is3DRotation);
}

/*
 * @tc.name: Is3DRotation_002
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSUniRenderUtilTest, Is3DRotation_002, Function | SmallTest | Level2)
{
    bool is3DRotation;
    Drawing::Matrix matrix = Drawing::Matrix();
    matrix.SetMatrix(-1, 0, 0, 0, -1, 0, 0, 0, 1);
    is3DRotation = RSUniRenderUtil::Is3DRotation(matrix);
    ASSERT_TRUE(is3DRotation);
}

/*
 * @tc.name: AssignWindowNodes
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSUniRenderUtilTest, AssignWindowNodes, Function | SmallTest | Level2)
{
    std::list<std::shared_ptr<RSSurfaceRenderNode>> mainThreadNodes;
    std::list<std::shared_ptr<RSSurfaceRenderNode>> subThreadNodes;
    RSUniRenderUtil::AssignWindowNodes(nullptr, mainThreadNodes, subThreadNodes);
    NodeId id = 0;
    RSDisplayNodeConfig config;
    auto node = std::make_shared<RSDisplayRenderNode>(id, config);
    RSUniRenderUtil::AssignWindowNodes(node, mainThreadNodes, subThreadNodes);
}

/*
 * @tc.name: ClearSurfaceIfNeed
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSUniRenderUtilTest, ClearSurfaceIfNeed, Function | SmallTest | Level2)
{
    RSRenderNodeMap map;
    std::set<std::shared_ptr<RSBaseRenderNode>> oldChildren;
    RSUniRenderUtil::ClearSurfaceIfNeed(map, nullptr, oldChildren);
    NodeId id = 0;
    RSDisplayNodeConfig config;
    auto node = std::make_shared<RSDisplayRenderNode>(id, config);
    RSUniRenderUtil::ClearSurfaceIfNeed(map, node, oldChildren);
}

/*
 * @tc.name: ClearCacheSurface
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSUniRenderUtilTest, ClearCacheSurface, Function | SmallTest | Level2)
{
    auto rsSurfaceRenderNode = RSTestUtil::CreateSurfaceNode();
    RSSurfaceRenderNode& node = static_cast<RSSurfaceRenderNode&>(*(rsSurfaceRenderNode.get()));
    uint32_t threadIndex = 0;
    RSUniRenderUtil::ClearCacheSurface(node, threadIndex);
}

/*
 * @tc.name: ClearNodeCacheSurface
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSUniRenderUtilTest, ClearNodeCacheSurface, Function | SmallTest | Level2)
{
    uint32_t threadIndex = 1;
    RSUniRenderUtil::ClearNodeCacheSurface(nullptr, nullptr, threadIndex, 0);
    NodeId id = 0;
    RSDisplayNodeConfig config;
    auto node = std::make_shared<RSDisplayRenderNode>(id, config);
    threadIndex = UNI_MAIN_THREAD_INDEX;
    auto cacheSurface = node->GetCacheSurface(threadIndex, false);
    auto completedSurface= node->GetCompletedCacheSurface(0, true);
    RSUniRenderUtil::ClearNodeCacheSurface(std::move(cacheSurface), std::move(completedSurface), threadIndex, 0);
    threadIndex = 1;
    auto cacheSurface1 = node->GetCacheSurface(threadIndex, false);
    auto completedSurface1= node->GetCompletedCacheSurface(0, true);
    RSUniRenderUtil::ClearNodeCacheSurface(std::move(cacheSurface1), std::move(completedSurface1), threadIndex, 0);
}

/*
 * @tc.name: HandleCaptureNode
 * @tc.desc: Test RSUniRenderUtil::HandleCaptureNode api
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSUniRenderUtilTest, HandleCaptureNode, Function | SmallTest | Level2)
{
    Drawing::Canvas drawingCanvas;
    RSPaintFilterCanvas canvas(&drawingCanvas);
    auto surfaceNode = RSTestUtil::CreateSurfaceNode();
    ASSERT_NE(surfaceNode, nullptr);
    RSUniRenderUtil::HandleCaptureNode(*surfaceNode, canvas);
}

/*
 * @tc.name: AssignMainThreadNode
 * @tc.desc:Test RSUniRenderUtilTest.AssignMainThreadNode
 * @tc.type: FUNC
 * @tc.require:issueI7KK3I
 */
HWTEST_F(RSUniRenderUtilTest, AssignMainThreadNode, Function | SmallTest | Level2)
{
    std::list<std::shared_ptr<RSSurfaceRenderNode>> mainThreadNodes;
    RSUniRenderUtil::AssignMainThreadNode(mainThreadNodes, nullptr);
    ASSERT_EQ(0, mainThreadNodes.size());
    auto node = RSTestUtil::CreateSurfaceNode();
    RSUniRenderUtil::AssignMainThreadNode(mainThreadNodes, node);
    ASSERT_EQ(1, mainThreadNodes.size());
}

/*
 * @tc.name: AssignSubThreadNode
 * @tc.desc:Test RSUniRenderUtilTest.AssignSubThreadNode
 * @tc.type: FUNC
 * @tc.require:issueI7KK3I
 */
HWTEST_F(RSUniRenderUtilTest, AssignSubThreadNode, Function | SmallTest | Level2)
{
    std::list<std::shared_ptr<RSSurfaceRenderNode>> subThreadNodes;
    RSUniRenderUtil::AssignSubThreadNode(subThreadNodes, nullptr);
    ASSERT_EQ(0, subThreadNodes.size());
    auto node = RSTestUtil::CreateSurfaceNode();
    RSUniRenderUtil::AssignSubThreadNode(subThreadNodes, node);
    ASSERT_EQ(1, subThreadNodes.size());
}

/*
 * @tc.name: FloorTransXYInCanvasMatrix
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSUniRenderUtilTest, FloorTransXYInCanvasMatrix, Function | SmallTest | Level2)
{
    Drawing::Matrix matrix = Drawing::Matrix();
    matrix.SetMatrix(1.0, 0.0, 0.1, 0.0, 1.0, 0.1, 0.0, 0.0, 1.0);
    auto drawingCanvas = std::make_unique<Drawing::Canvas>(10, 10);
    auto canvas = std::make_shared<RSPaintFilterCanvas>(drawingCanvas.get());
    auto cachedEffectDataptr = std::make_shared<RSPaintFilterCanvas::CachedEffectData>();
    RSPaintFilterCanvas::CanvasStatus status{0.0, matrix, cachedEffectDataptr};
    canvas->SetCanvasStatus(status);
    RSUniRenderUtil::FloorTransXYInCanvasMatrix(*canvas);
    ASSERT_FALSE(canvas->GetTotalMatrix().Get(Drawing::Matrix::TRANS_X) < 0.001);
    ASSERT_FALSE(canvas->GetTotalMatrix().Get(Drawing::Matrix::TRANS_Y) < 0.001);
}

/*
 * @tc.name: IsNodeAssignSubThread001
 * @tc.desc: test IsNodeAssignSubThread for non focus node
 * @tc.type: FUNC
 * @tc.require: issueI904G4
 */
HWTEST_F(RSUniRenderUtilTest, IsNodeAssignSubThread001, Function | SmallTest | Level2)
{
    NodeId id = 0;
    RSDisplayNodeConfig config;
    auto displayNode = std::make_shared<RSDisplayRenderNode>(id, config);
    auto surfaceNode = RSTestUtil::CreateSurfaceNode();
    ASSERT_NE(displayNode, nullptr);
    ASSERT_NE(surfaceNode, nullptr);
    displayNode->AddChild(surfaceNode);

    auto mainThread = RSMainThread::Instance();
    if (mainThread->GetDeviceType() == DeviceType::PC) {
        ASSERT_EQ(RSUniRenderUtil::IsNodeAssignSubThread(surfaceNode, displayNode->IsRotationChanged()),
            surfaceNode->QuerySubAssignable(displayNode->IsRotationChanged()));
    }
}

/*
 * @tc.name: IsNodeAssignSubThread002
 * @tc.desc: test IsNodeAssignSubThread for focus node which has shared transition
 * @tc.type: FUNC
 * @tc.require: issueI904G4
 */
HWTEST_F(RSUniRenderUtilTest, IsNodeAssignSubThread002, Function | SmallTest | Level2)
{
    NodeId id = 0;
    RSDisplayNodeConfig config;
    auto displayNode = std::make_shared<RSDisplayRenderNode>(id, config);
    auto surfaceNode = RSTestUtil::CreateSurfaceNode();
    ASSERT_NE(displayNode, nullptr);
    ASSERT_NE(surfaceNode, nullptr);
    displayNode->AddChild(surfaceNode);
    surfaceNode->SetHasSharedTransitionNode(true);
    
    auto mainThread = RSMainThread::Instance();
    std::string str = "";
    mainThread->SetFocusAppInfo(-1, -1, str, str, surfaceNode->GetId());
    if (mainThread->GetDeviceType() == DeviceType::PC) {
        ASSERT_FALSE(RSUniRenderUtil::IsNodeAssignSubThread(surfaceNode, displayNode->IsRotationChanged()));
    }
}

/*
 * @tc.name: IsNodeAssignSubThread003
 * @tc.desc: test IsNodeAssignSubThread for focus node which don't have shared transition
 * @tc.type: FUNC
 * @tc.require: issueI904G4
 */
HWTEST_F(RSUniRenderUtilTest, IsNodeAssignSubThread003, Function | SmallTest | Level2)
{
    NodeId id = 0;
    RSDisplayNodeConfig config;
    auto displayNode = std::make_shared<RSDisplayRenderNode>(id, config);
    auto surfaceNode = RSTestUtil::CreateSurfaceNode();
    ASSERT_NE(displayNode, nullptr);
    ASSERT_NE(surfaceNode, nullptr);
    displayNode->AddChild(surfaceNode);
    surfaceNode->SetHasSharedTransitionNode(false);
    
    auto mainThread = RSMainThread::Instance();
    std::string str = "";
    mainThread->SetFocusAppInfo(-1, -1, str, str, surfaceNode->GetId());
    if (mainThread->GetDeviceType() == DeviceType::PC) {
        ASSERT_EQ(RSUniRenderUtil::IsNodeAssignSubThread(surfaceNode, displayNode->IsRotationChanged()),
            surfaceNode->QuerySubAssignable(displayNode->IsRotationChanged()));
    }
}

/*
 * @tc.name: IsNodeAssignSubThread004
 * @tc.desc: test IsNodeAssignSubThread for self drawing node & self drawing window node
 * @tc.type: FUNC
 * @tc.require: issueI98VTC
 */
HWTEST_F(RSUniRenderUtilTest, IsNodeAssignSubThread004, Function | SmallTest | Level2)
{
    NodeId id = 0;
    RSDisplayNodeConfig config;
    auto displayNode = std::make_shared<RSDisplayRenderNode>(id, config);
    auto selfDrawingNode = RSTestUtil::CreateSurfaceNode();
    auto selfDrawingWindowNode = RSTestUtil::CreateSurfaceNode();
    ASSERT_NE(displayNode, nullptr);
    ASSERT_NE(selfDrawingNode, nullptr);
    ASSERT_NE(selfDrawingWindowNode, nullptr);
    displayNode->AddChild(selfDrawingNode);
    displayNode->AddChild(selfDrawingWindowNode);
    selfDrawingNode->SetSurfaceNodeType(RSSurfaceNodeType::SELF_DRAWING_NODE);
    selfDrawingWindowNode->SetSurfaceNodeType(RSSurfaceNodeType::SELF_DRAWING_WINDOW_NODE);
    
    ASSERT_EQ(RSUniRenderUtil::IsNodeAssignSubThread(selfDrawingNode, displayNode->IsRotationChanged()), false);
    ASSERT_EQ(RSUniRenderUtil::IsNodeAssignSubThread(selfDrawingWindowNode, displayNode->IsRotationChanged()), false);
}

/*
 * @tc.name: IsNodeAssignSubThread005
 * @tc.desc: test IsNodeAssignSubThread while cache is waiting for process
 * @tc.type: FUNC
 * @tc.require: issueI98VTC
 */
HWTEST_F(RSUniRenderUtilTest, IsNodeAssignSubThread005, Function | SmallTest | Level2)
{
    NodeId id = 0;
    RSDisplayNodeConfig config;
    auto displayNode = std::make_shared<RSDisplayRenderNode>(id, config);
    auto surfaceNode = RSTestUtil::CreateSurfaceNode();
    ASSERT_NE(displayNode, nullptr);
    ASSERT_NE(surfaceNode, nullptr);
    displayNode->AddChild(surfaceNode);
    surfaceNode->SetCacheSurfaceProcessedStatus(CacheProcessStatus::WAITING);
    
    ASSERT_EQ(RSUniRenderUtil::IsNodeAssignSubThread(surfaceNode, displayNode->IsRotationChanged()), false);
}

/**
 * @tc.name: MergeVisibleDirtyRegionTest
 * @tc.desc: Verify function MergeVisibleDirtyRegion
 * @tc.type:FUNC
 * @tc.require:issuesI9KRF1
 */
HWTEST_F(RSUniRenderUtilTest, MergeVisibleDirtyRegionTest, Function | SmallTest | Level2)
{
    std::vector<RSBaseRenderNode::SharedPtr> allSurfaceNodes;
    std::vector<NodeId> hasVisibleDirtyRegionSurfaceVec;
    NodeId id = 0;
    std::shared_ptr<RSRenderNode> nodeNull = nullptr;
    auto rsRenderNode = std::make_shared<RSRenderNode>(id);
    auto node = std::make_shared<RSRenderNode>(id);
    rsRenderNode->renderDrawable_ = std::make_shared<DrawableV2::RSRenderNodeDrawable>(node);
    rsRenderNode->renderDrawable_->renderParams_ = std::make_unique<RSRenderParams>(id);
    allSurfaceNodes.emplace_back(nodeNull);
    allSurfaceNodes.emplace_back(rsRenderNode);
    RSUniRenderUtil::MergeVisibleDirtyRegion(allSurfaceNodes, hasVisibleDirtyRegionSurfaceVec, false, false);
    RSUniRenderUtil::MergeVisibleDirtyRegion(allSurfaceNodes, hasVisibleDirtyRegionSurfaceVec, true, true);
    EXPECT_TRUE(rsRenderNode->renderDrawable_->renderParams_);
}

/**
 * @tc.name: SetAllSurfaceGlobalDityRegionTest
 * @tc.desc: Verify function SetAllSurfaceGlobalDityRegion
 * @tc.type:FUNC
 * @tc.require:issuesI9KRF1
 */
HWTEST_F(RSUniRenderUtilTest, SetAllSurfaceGlobalDityRegionTest, Function | SmallTest | Level2)
{
    std::vector<RSBaseRenderNode::SharedPtr> allSurfaces;
    RectI globalDirtyRegion;
    NodeId id = 0;
    std::shared_ptr<RSRenderNode> nodeNull = nullptr;
    auto rsRenderNode = std::make_shared<RSRenderNode>(id);
    allSurfaces.emplace_back(nodeNull);
    allSurfaces.emplace_back(rsRenderNode);
    RSUniRenderUtil::SetAllSurfaceGlobalDityRegion(allSurfaces, globalDirtyRegion);
    EXPECT_FALSE(allSurfaces.empty());
}

/**
 * @tc.name: ScreenIntersectDirtyRectsTest
 * @tc.desc: Verify function ScreenIntersectDirtyRects
 * @tc.type:FUNC
 * @tc.require:issuesI9KRF1
 */
HWTEST_F(RSUniRenderUtilTest, ScreenIntersectDirtyRectsTest, Function | SmallTest | Level2)
{
    Occlusion::Region region;
    ScreenInfo screenInfo;
    EXPECT_TRUE(RSUniRenderUtil::ScreenIntersectDirtyRects(region, screenInfo).empty());
}

/**
 * @tc.name: IsNeedClientsTest
 * @tc.desc: Verify function IsNeedClient
 * @tc.type:FUNC
 * @tc.require:issuesI9KRF1
 */
HWTEST_F(RSUniRenderUtilTest, IsNeedClientsTest, Function | SmallTest | Level2)
{
    auto rsSurfaceRenderNode = RSTestUtil::CreateSurfaceNode();
    ASSERT_NE(rsSurfaceRenderNode, nullptr);
    RSSurfaceRenderNode& node = static_cast<RSSurfaceRenderNode&>(*(rsSurfaceRenderNode.get()));
    ComposeInfo info;
    EXPECT_FALSE(RSUniRenderUtil::IsNeedClient(node, info));
}

/**
 * @tc.name: HandleSubThreadNodeTest
 * @tc.desc: Verify function HandleSubThreadNode
 * @tc.type:FUNC
 * @tc.require:issuesI9KRF1
 */
HWTEST_F(RSUniRenderUtilTest, HandleSubThreadNodeTest, Function | SmallTest | Level2)
{
    NodeId id = 0;
    RSSurfaceRenderNode node(id);
    auto canva = std::make_shared<Drawing::Canvas>();
    RSPaintFilterCanvas canvas(canva.get());
    EXPECT_FALSE(RSUniRenderUtil::HandleSubThreadNode(node, canvas));
    node.SetIsMainThreadNode(false);
    RSMainThread::Instance()->deviceType_ = DeviceType::PC;
    EXPECT_TRUE(RSUniRenderUtil::HandleSubThreadNode(node, canvas));
}

/**
 * @tc.name: ReleaseColorPickerResourceTest
 * @tc.desc: Verify function ReleaseColorPickerResource
 * @tc.type:FUNC
 * @tc.require:issuesI9KRF1
 */
HWTEST_F(RSUniRenderUtilTest, ReleaseColorPickerResourceTest, Function | SmallTest | Level2)
{
    NodeId id = 0;
    std::shared_ptr<RSRenderNode> node = nullptr;
    RSUniRenderUtil::ReleaseColorPickerResource(node);
    node = std::make_shared<RSRenderNode>(id);
    node->children_.emplace_back(std::make_shared<RSRenderNode>(id));
    RSUniRenderUtil::ReleaseColorPickerResource(node);
    EXPECT_FALSE(node->children_.empty());
}

/*
 * @tc.name: FloorTransXYInCanvasMatrixTest
 * @tc.desc: Verify function FloorTransXYInCanvasMatrix
 * @tc.type: FUNC
 * @tc.require: issuesI9KRF1
 */
HWTEST_F(RSUniRenderUtilTest, FloorTransXYInCanvasMatrixTest, Function | SmallTest | Level2)
{
    Drawing::Canvas canvas;
    RSPaintFilterCanvas filterCanvas(&canvas);
    RSUniRenderUtil::FloorTransXYInCanvasMatrix(filterCanvas);
    EXPECT_FALSE(filterCanvas.recordDrawable_);
}

/*
 * @tc.name: DrawRectForDfxTest
 * @tc.desc: Verify function DrawRectForDfx
 * @tc.type: FUNC
 * @tc.require: issuesI9KRF1
 */
HWTEST_F(RSUniRenderUtilTest, DrawRectForDfxTest, Function | SmallTest | Level2)
{
    Drawing::Canvas canvas;
    RSPaintFilterCanvas filterCanvas(&canvas);
    RectI rect(1, 1, -1, -1);
    Drawing::Color color;
    float alpha = 1.f;
    std::string extraInfo = "";
    RSUniRenderUtil::DrawRectForDfx(filterCanvas, rect, color, alpha, extraInfo);
    EXPECT_TRUE(rect.width_ <= 0);

    rect.width_ = 1;
    rect.height_ = 1;
    RSUniRenderUtil::DrawRectForDfx(filterCanvas, rect, color, alpha, extraInfo);
    EXPECT_TRUE(rect.width_ >= 0);
}

/*
 * @tc.name: DealWithNodeGravityTest
 * @tc.desc: Verify function DealWithNodeGravity
 * @tc.type: FUNC
 * @tc.require: issuesI9KRF1
 */
HWTEST_F(RSUniRenderUtilTest, DealWithNodeGravityTest, Function | SmallTest | Level2)
{
    NodeId id = 0;
    RSSurfaceRenderNode node(id);
    ScreenInfo screenInfo;
    node.buffer_.buffer = OHOS::SurfaceBuffer::Create();
    node.renderContent_->renderProperties_.frameGravity_ = Gravity::RESIZE;
    RSUniRenderUtil::DealWithNodeGravity(node, screenInfo);
    node.renderContent_->renderProperties_.frameGravity_ = Gravity::TOP_LEFT;
    RSUniRenderUtil::DealWithNodeGravity(node, screenInfo);
    node.renderContent_->renderProperties_.frameGravity_ = Gravity::DEFAULT;
    node.renderContent_->renderProperties_.boundsGeo_->SetHeight(-1.0f);
    RSUniRenderUtil::DealWithNodeGravity(node, screenInfo);
    node.renderContent_->renderProperties_.boundsGeo_->SetWidth(-1.0f);
    RSUniRenderUtil::DealWithNodeGravity(node, screenInfo);
    screenInfo.rotation = ScreenRotation::ROTATION_90;
    RSUniRenderUtil::DealWithNodeGravity(node, screenInfo);
    screenInfo.rotation = ScreenRotation::ROTATION_270;
    RSUniRenderUtil::DealWithNodeGravity(node, screenInfo);
    EXPECT_TRUE(screenInfo.width == 0);
}

/*
 * @tc.name: LayerRotateTest
 * @tc.desc: Verify function LayerRotate
 * @tc.type: FUNC
 * @tc.require: issuesI9KRF1
 */
HWTEST_F(RSUniRenderUtilTest, LayerRotateTest, Function | SmallTest | Level2)
{
    NodeId id = 0;
    RSSurfaceRenderNode node(id);
    ScreenInfo screenInfo;
    screenInfo.rotation = ScreenRotation::ROTATION_90;
    RSUniRenderUtil::LayerRotate(node, screenInfo);

    screenInfo.rotation = ScreenRotation::ROTATION_180;
    RSUniRenderUtil::LayerRotate(node, screenInfo);

    screenInfo.rotation = ScreenRotation::ROTATION_270;
    RSUniRenderUtil::LayerRotate(node, screenInfo);

    screenInfo.rotation = ScreenRotation::ROTATION_0;
    RSUniRenderUtil::LayerRotate(node, screenInfo);
    EXPECT_EQ(screenInfo.width, 0);
}

/*
 * @tc.name: LayerCropTest
 * @tc.desc: Verify function LayerCrop
 * @tc.type: FUNC
 * @tc.require: issuesI9KRF1
 */
HWTEST_F(RSUniRenderUtilTest, LayerCropTest, Function | SmallTest | Level2)
{
    NodeId id = 0;
    RSSurfaceRenderNode node(id);
    ScreenInfo screenInfo;
    RSUniRenderUtil::LayerCrop(node, screenInfo);

    auto dstRect = node.GetDstRect();
    dstRect.left_ = 1;
    RSUniRenderUtil::LayerCrop(node, screenInfo);
    EXPECT_EQ(screenInfo.width, 0);
}

/*
 * @tc.name: GetLayerTransformTest
 * @tc.desc: Verify function GetLayerTransform
 * @tc.type: FUNC
 * @tc.require: issuesI9KRF1
 */
HWTEST_F(RSUniRenderUtilTest, GetLayerTransformTest, Function | SmallTest | Level2)
{
    NodeId id = 0;
    RSSurfaceRenderNode node(id);
    ScreenInfo screenInfo;
    GraphicTransformType type = RSUniRenderUtil::GetLayerTransform(node, screenInfo);

    node.consumer_ = IConsumerSurface::Create();
    type = RSUniRenderUtil::GetLayerTransform(node, screenInfo);
    EXPECT_TRUE(type == GraphicTransformType::GRAPHIC_ROTATE_NONE);
}

/*
 * @tc.name: SrcRectRotateTransformTest
 * @tc.desc: Verify function SrcRectRotateTransform
 * @tc.type: FUNC
 * @tc.require: issuesI9KRF1
 */
HWTEST_F(RSUniRenderUtilTest, SrcRectRotateTransformTest, Function | SmallTest | Level2)
{
    NodeId id = 0;
    RSSurfaceRenderNode node(id);
    RSUniRenderUtil::SrcRectRotateTransform(node);

    node.consumer_ = IConsumerSurface::Create();
    RSUniRenderUtil::SrcRectRotateTransform(node);
    EXPECT_FALSE(node.GetBuffer());
}

/*
 * @tc.name: HandleHardwareNodeTest
 * @tc.desc: Verify function HandleHardwareNode
 * @tc.type: FUNC
 * @tc.require: issuesI9KRF1
 */
HWTEST_F(RSUniRenderUtilTest, HandleHardwareNodeTest, Function | SmallTest | Level2)
{
    NodeId id = 0;
    auto node = std::make_shared<RSSurfaceRenderNode>(id);
    RSUniRenderUtil::HandleHardwareNode(node);
    node->hasHardwareNode_ = true;
    RSUniRenderUtil::HandleHardwareNode(node);
    node->children_.emplace_back(std::make_shared<RSRenderNode>(id));
    node->nodeType_ = RSSurfaceNodeType::LEASH_WINDOW_NODE;
    RSUniRenderUtil::HandleHardwareNode(node);
    EXPECT_FALSE(node->GetBuffer());
}

/*
 * @tc.name: UpdateRealSrcRectTest
 * @tc.desc: Verify function UpdateRealSrcRect
 * @tc.type: FUNC
 * @tc.require: issuesI9KRF1
 */
HWTEST_F(RSUniRenderUtilTest, UpdateRealSrcRectTest, Function | SmallTest | Level2)
{
    NodeId id = 0;
    RSSurfaceRenderNode node(id);
    RectI absRect;
    node.buffer_.buffer = OHOS::SurfaceBuffer::Create();
    node.consumer_ = IConsumerSurface::Create();
    RSUniRenderUtil::UpdateRealSrcRect(node, absRect);

    absRect = RectI(1, 1, 1, 1);
    RSUniRenderUtil::UpdateRealSrcRect(node, absRect);
    EXPECT_TRUE(node.buffer_.buffer);
}

/*
 * @tc.name: CheckForceHardwareAndUpdateDstRectTest
 * @tc.desc: Verify function CheckForceHardwareAndUpdateDstRect
 * @tc.type: FUNC
 * @tc.require: issuesI9KRF1
 */
HWTEST_F(RSUniRenderUtilTest, CheckForceHardwareAndUpdateDstRectTest, Function | SmallTest | Level2)
{
    NodeId id = 0;
    RSSurfaceRenderNode node(id);
    node.buffer_.buffer = OHOS::SurfaceBuffer::Create();
    RSUniRenderUtil::CheckForceHardwareAndUpdateDstRect(node);
    node.isForceHardwareByUser_ = true;
    RSUniRenderUtil::CheckForceHardwareAndUpdateDstRect(node);
    EXPECT_TRUE(node.GetOriginalDstRect().IsEmpty());
}

/*
 * @tc.name: SortSubThreadNodesTest
 * @tc.desc: Verify function SortSubThreadNodes
 * @tc.type: FUNC
 * @tc.require: issuesI9KRF1
 */
HWTEST_F(RSUniRenderUtilTest, SortSubThreadNodesTest, TestSize.Level1)
{
    RSUniRenderUtil rsUniRenderUtil;
    std::list<std::shared_ptr<RSSurfaceRenderNode>> subThreadNodes;
    std::shared_ptr<RSSurfaceRenderNode> nodeTest1 = std::make_shared<RSSurfaceRenderNode>(0);
    EXPECT_TRUE(nodeTest1);
    std::shared_ptr<RSSurfaceRenderNode> nodeTest2 = std::make_shared<RSSurfaceRenderNode>(0);
    EXPECT_TRUE(nodeTest2);
    std::shared_ptr<RSSurfaceRenderNode> nodeTest3 = std::make_shared<RSSurfaceRenderNode>(0);
    EXPECT_TRUE(nodeTest3);
    std::shared_ptr<RSSurfaceRenderNode> nodeTest4 = nullptr;

    nodeTest1->SetPriority(NodePriorityType::SUB_FOCUSNODE_PRIORITY);
    nodeTest1->renderContent_->renderProperties_.SetPositionZ(1.0f);
    nodeTest2->SetPriority(NodePriorityType::MAIN_PRIORITY);
    // for test
    nodeTest2->renderContent_->renderProperties_.SetPositionZ(2.0f);
    nodeTest3->SetPriority(NodePriorityType::MAIN_PRIORITY);
    nodeTest3->renderContent_->renderProperties_.SetPositionZ(1.0f);

    subThreadNodes.push_back(nodeTest1);
    subThreadNodes.push_back(nodeTest2);
    subThreadNodes.push_back(nodeTest3);
    // node4 is nullptr
    subThreadNodes.push_back(nodeTest4);

    rsUniRenderUtil.SortSubThreadNodes(subThreadNodes);
    auto it = subThreadNodes.begin();
    EXPECT_EQ((*it)->GetPriority(), NodePriorityType::MAIN_PRIORITY);
    ++it;
    EXPECT_EQ((*it)->GetPriority(), NodePriorityType::MAIN_PRIORITY);
    ++it;
    EXPECT_EQ((*it)->GetPriority(), NodePriorityType::SUB_FOCUSNODE_PRIORITY);
    // for test
    EXPECT_EQ(subThreadNodes.size(), 4);
}

/*
 * @tc.name: CacheSubThreadNodesTest
 * @tc.desc: Verify function CacheSubThreadNodes
 * @tc.type: FUNC
 * @tc.require: issuesI9KRF1
 */
HWTEST_F(RSUniRenderUtilTest, CacheSubThreadNodesTest, TestSize.Level1)
{
    RSUniRenderUtil rsUniRenderUtil;

    std::list<std::shared_ptr<RSSurfaceRenderNode>> subThreadNodes;
    std::list<std::shared_ptr<RSSurfaceRenderNode>> oldSubThreadNodes;

    std::shared_ptr<RSSurfaceRenderNode> nodeTest1 = std::make_shared<RSSurfaceRenderNode>(0);
    EXPECT_TRUE(nodeTest1);
    std::shared_ptr<RSSurfaceRenderNode> nodeTest2 = std::make_shared<RSSurfaceRenderNode>(1);
    EXPECT_TRUE(nodeTest2);
    std::shared_ptr<RSSurfaceRenderNode> nodeTest3 = std::make_shared<RSSurfaceRenderNode>(0);
    nodeTest3->cacheProcessStatus_ = CacheProcessStatus::DOING;
    EXPECT_TRUE(nodeTest3);
    std::shared_ptr<RSSurfaceRenderNode> nodeTest4 = std::make_shared<RSSurfaceRenderNode>(1);
    EXPECT_TRUE(nodeTest4);

    subThreadNodes.push_back(nodeTest1);
    subThreadNodes.push_back(nodeTest2);
    oldSubThreadNodes.push_back(nodeTest3);
    oldSubThreadNodes.push_back(nodeTest4);

    rsUniRenderUtil.CacheSubThreadNodes(oldSubThreadNodes, subThreadNodes);
    // fot test
    EXPECT_EQ(oldSubThreadNodes.size(), 3);
}

/*
 * @tc.name: LayerScaleDownTest
 * @tc.desc: Verify function LayerScaleDown
 * @tc.type: FUNC
 * @tc.require: issuesI9KRF1
 */
HWTEST_F(RSUniRenderUtilTest, LayerScaleDownTest, TestSize.Level1)
{
    RSUniRenderUtil rsUniRenderUtil;
    RSSurfaceRenderNode nodesTest1(0);
    nodesTest1.buffer_.buffer = nullptr;
    rsUniRenderUtil.LayerScaleDown(nodesTest1);

    RSSurfaceRenderNode nodesTest2(1);
    nodesTest2.buffer_.buffer = OHOS::SurfaceBuffer::Create();
    nodesTest2.consumer_ = nullptr;
    rsUniRenderUtil.LayerScaleDown(nodesTest2);
}
} // namespace OHOS::Rosen