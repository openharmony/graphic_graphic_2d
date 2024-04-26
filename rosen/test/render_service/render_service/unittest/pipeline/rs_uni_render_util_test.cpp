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
    node->InitRenderParams();
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
 * @tc.name: CreateBufferDrawParam_001
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSUniRenderUtilTest, CreateBufferDrawParam_001, Function | SmallTest | Level2)
{
    auto rsSurfaceRenderNode = RSTestUtil::CreateSurfaceNode();
    ASSERT_NE(rsSurfaceRenderNode, nullptr);
    RSSurfaceRenderNode& node = static_cast<RSSurfaceRenderNode&>(*(rsSurfaceRenderNode.get()));
    node.InitRenderParams();
    bool forceCPU = false;
    RSUniRenderUtil::CreateBufferDrawParam(node, forceCPU);
}

/*
 * @tc.name: CreateBufferDrawParam_002
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSUniRenderUtilTest, CreateBufferDrawParam_002, Function | SmallTest | Level2)
{
    auto rsSurfaceRenderNode = RSTestUtil::CreateSurfaceNodeWithBuffer();
    ASSERT_NE(rsSurfaceRenderNode, nullptr);
    RSSurfaceRenderNode& node = static_cast<RSSurfaceRenderNode&>(*(rsSurfaceRenderNode.get()));
    bool forceCPU = false;
    RSUniRenderUtil::CreateBufferDrawParam(node, forceCPU);
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
 * @tc.name: CeilTransXYInCanvasMatrix
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSUniRenderUtilTest, CeilTransXYInCanvasMatrix, Function | SmallTest | Level2)
{
    Drawing::Matrix matrix = Drawing::Matrix();
    matrix.SetMatrix(1.0, 0.0, 0.1, 0.0, 1.0, 0.1, 0.0, 0.0, 1.0);
    auto drawingCanvas = std::make_unique<Drawing::Canvas>(10, 10);
    auto canvas = std::make_shared<RSPaintFilterCanvas>(drawingCanvas.get());
    auto cachedEffectDataptr = std::make_shared<RSPaintFilterCanvas::CachedEffectData>();
    RSPaintFilterCanvas::CanvasStatus status{0.0, matrix, cachedEffectDataptr};
    canvas->SetCanvasStatus(status);
    RSUniRenderUtil::CeilTransXYInCanvasMatrix(*canvas);
    ASSERT_TRUE(canvas->GetTotalMatrix().Get(Drawing::Matrix::TRANS_X) < 0.001);
    ASSERT_TRUE(canvas->GetTotalMatrix().Get(Drawing::Matrix::TRANS_Y) < 0.001);
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
} // namespace OHOS::Rosen