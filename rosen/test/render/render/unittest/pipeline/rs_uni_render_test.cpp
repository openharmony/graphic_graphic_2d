/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
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
#include "surface_buffer_impl.h"
#include "surface_type.h"

#include "drawable/rs_display_render_node_drawable.h"
#include "drawable/rs_surface_render_node_drawable.h"
#include "params/rs_surface_render_params.h"
#include "pipeline/rs_main_thread.h"
#include "pipeline/rs_surface_capture_task_parallel.h"
#include "pipeline/rs_uni_render_util.h"
#include "pixel_map.h"
#include "property/rs_properties_def.h"
#include "render/rs_material_filter.h"
#include "render/rs_shadow.h"

using namespace testing;
using namespace testing::ext;
using namespace OHOS::Rosen::DrawableV2;

namespace OHOS::Rosen {
class RSUniRenderTest : public testing::Test {
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

void RSUniRenderTest::SetUpTestCase()
{
    RSTestUtil::InitRenderNodeGC();
}
void RSUniRenderTest::TearDownTestCase() {}
void RSUniRenderTest::SetUp() {}
void RSUniRenderTest::TearDown() {}

/*
 * @tc.name: SrcRectScaleDown_001
 * @tc.desc: default value
 * @tc.type: FUNC
 * @tc.require:
*/
HWTEST_F(RSUniRenderTest, SrcRectScaleDown_001, Function | SmallTest | Level2)
{
    auto rsSurfaceRenderNode = RSTestUtil::CreateSurfaceNode();
    ASSERT_NE(rsSurfaceRenderNode, nullptr);
    RSSurfaceRenderNode& node = static_cast<RSSurfaceRenderNode&>(*(rsSurfaceRenderNode.get()));
    BufferDrawParam params;
    RectF localBounds;
    RSUniRenderUtil::SrcRectScaleDown(
        params, node.GetRSSurfaceHandler()->GetBuffer(), node.GetRSSurfaceHandler()->GetConsumer(), localBounds);
}

/*
 * @tc.name: SrcRectScaleDown_002
 * @tc.desc: default value
 * @tc.type: FUNC
 * @tc.require:
*/
HWTEST_F(RSUniRenderTest, SrcRectScaleDown_002, Function | SmallTest | Level2)
{
    auto rsSurfaceRenderNode = RSTestUtil::CreateSurfaceNodeWithBuffer();
    ASSERT_NE(rsSurfaceRenderNode, nullptr);
    RSSurfaceRenderNode& node = static_cast<RSSurfaceRenderNode&>(*(rsSurfaceRenderNode.get()));
    BufferDrawParam params;
    RectF localBounds;
    RSUniRenderUtil::SrcRectScaleDown(
        params, node.GetRSSurfaceHandler()->GetBuffer(), node.GetRSSurfaceHandler()->GetConsumer(), localBounds);
}

/*
 * @tc.name: SrcRectScaleDown_003
 * @tc.desc: Test SrcRectScaleDown when srcRect is multiple
 * @tc.type: FUNC
 * @tc.require: issueIAJOWI
*/
HWTEST_F(RSUniRenderTest, SrcRectScaleDown_003, Function | SmallTest | Level2)
{
    auto node = RSTestUtil::CreateSurfaceNodeWithBuffer();
    ASSERT_NE(node, nullptr);
    BufferDrawParam params;
    int32_t left = 0;
    int32_t top = 0;
    int32_t right = 1;
    int32_t bottom = 2;
    params.srcRect = Drawing::Rect(left, top, right, bottom);
    right = 2;
    bottom = 1;
    RectF localBounds = RectF(left, top, right, bottom);
    RSUniRenderUtil::SrcRectScaleDown(
        params, node->GetRSSurfaceHandler()->GetBuffer(), node->GetRSSurfaceHandler()->GetConsumer(), localBounds);
   
    params.srcRect.SetRight(right);
    params.srcRect.SetBottom(bottom);
    right = 1;
    bottom = 2;
    localBounds.SetRight(right);
    localBounds.SetBottom(bottom);
    RSUniRenderUtil::SrcRectScaleDown(
        params, node->GetRSSurfaceHandler()->GetBuffer(), node->GetRSSurfaceHandler()->GetConsumer(), localBounds);
}

/*
 * @tc.name: SrcRectScaleFit_001
 * @tc.desc: default value
 * @tc.type: FUNC
 * @tc.require:
*/
HWTEST_F(RSUniRenderTest, SrcRectScaleFit_001, Function | SmallTest | Level2)
{
    auto rsSurfaceRenderNode = RSTestUtil::CreateSurfaceNode();
    ASSERT_NE(rsSurfaceRenderNode, nullptr);
    RSSurfaceRenderNode& node = static_cast<RSSurfaceRenderNode&>(*(rsSurfaceRenderNode.get()));
    BufferDrawParam params;
    RectF localBounds;
    RSUniRenderUtil::SrcRectScaleFit(
        params, node.GetRSSurfaceHandler()->GetBuffer(), node.GetRSSurfaceHandler()->GetConsumer(), localBounds);
}

/*
 * @tc.name: SrcRectScaleFit_002
 * @tc.desc: default value
 * @tc.type: FUNC
 * @tc.require:
*/
HWTEST_F(RSUniRenderTest, SrcRectScaleFit_002, Function | SmallTest | Level2)
{
    auto rsSurfaceRenderNode = RSTestUtil::CreateSurfaceNodeWithBuffer();
    ASSERT_NE(rsSurfaceRenderNode, nullptr);
    RSSurfaceRenderNode& node = static_cast<RSSurfaceRenderNode&>(*(rsSurfaceRenderNode.get()));
    BufferDrawParam params;
    RectF localBounds;
    RSUniRenderUtil::SrcRectScaleFit(
        params, node.GetRSSurfaceHandler()->GetBuffer(), node.GetRSSurfaceHandler()->GetConsumer(), localBounds);
}

/*
 * @tc.name: SrcRectScaleFit_003
 * @tc.desc: Test SrcRectScaleFit when srcRect is multiple
 * @tc.type: FUNC
 * @tc.require: issueIAJOWI
*/
HWTEST_F(RSUniRenderTest, SrcRectScaleFit_003, Function | SmallTest | Level2)
{
    auto node = RSTestUtil::CreateSurfaceNodeWithBuffer();
    ASSERT_NE(node, nullptr);
    BufferDrawParam params;
    int32_t left = 0;
    int32_t top = 0;
    int32_t right = 1;
    int32_t bottom = 2;
    params.srcRect = Drawing::Rect(left, top, right, bottom);
    right = 2;
    bottom = 1;
    RectF localBounds = RectF(left, top, right, bottom);
    RSUniRenderUtil::SrcRectScaleFit(
        params, node->GetRSSurfaceHandler()->GetBuffer(), node->GetRSSurfaceHandler()->GetConsumer(), localBounds);
   
    params.srcRect.SetRight(right);
    params.srcRect.SetBottom(bottom);
    right = 1;
    bottom = 2;
    localBounds.SetRight(right);
    localBounds.SetBottom(bottom);
    RSUniRenderUtil::SrcRectScaleFit(
        params, node->GetRSSurfaceHandler()->GetBuffer(), node->GetRSSurfaceHandler()->GetConsumer(), localBounds);
}

/*
 * @tc.name: GetMatrixOfBufferToRelRect_001
 * @tc.desc: test GetMatrixOfBufferToRelRect with surfaceNode without buffer
 * @tc.type: FUNC
 * @tc.require: #I9E60C
 */
HWTEST_F(RSUniRenderTest, GetMatrixOfBufferToRelRect_001, Function | SmallTest | Level2)
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
HWTEST_F(RSUniRenderTest, GetMatrixOfBufferToRelRect_002, Function | SmallTest | Level2)
{
    auto rsSurfaceRenderNode = RSTestUtil::CreateSurfaceNodeWithBuffer();
    ASSERT_NE(rsSurfaceRenderNode, nullptr);
    RSSurfaceRenderNode& node = static_cast<RSSurfaceRenderNode&>(*(rsSurfaceRenderNode.get()));
    RSUniRenderUtil::GetMatrixOfBufferToRelRect(node);
}

/*
 * @tc.name: CreateLayerBufferDrawParam_001
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSUniRenderTest, CreateLayerBufferDrawParam_001, Function | SmallTest | Level2)
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
HWTEST_F(RSUniRenderTest, CreateLayerBufferDrawParam_002, Function | SmallTest | Level2)
{
    bool forceCPU = false;
    auto surfaceNode = RSTestUtil::CreateSurfaceNodeWithBuffer();
    ASSERT_NE(surfaceNode, nullptr);
    auto buffer = surfaceNode->GetRSSurfaceHandler()->GetBuffer();
    LayerInfoPtr layer = HdiLayerInfo::CreateHdiLayerInfo();
    layer->SetBuffer(buffer, surfaceNode->GetRSSurfaceHandler()->GetAcquireFence());
    RSUniRenderUtil::CreateLayerBufferDrawParam(layer, forceCPU);
}

/*
 * @tc.name: CreateLayerBufferDrawParam_003
 * @tc.desc: Test CreateLayerBufferDrawParam when buffer is nullptr
 * @tc.type: FUNC
 * @tc.require: issueIAJOWI
 */
HWTEST_F(RSUniRenderTest, CreateLayerBufferDrawParam_003, Function | SmallTest | Level2)
{
    bool forceCPU = false;
    auto surfaceNode = RSTestUtil::CreateSurfaceNode();
    ASSERT_NE(surfaceNode, nullptr);
    LayerInfoPtr layer = HdiLayerInfo::CreateHdiLayerInfo();
    layer->SetBuffer(nullptr, surfaceNode->GetRSSurfaceHandler()->GetAcquireFence());
    RSUniRenderUtil::CreateLayerBufferDrawParam(layer, forceCPU);
}

/*
 * @tc.name: CreateLayerBufferDrawParam_004
 * @tc.desc: Test CreateLayerBufferDrawParam when surface is not nullptr
 * @tc.type: FUNC
 * @tc.require: issueIAJOWI
 */
HWTEST_F(RSUniRenderTest, CreateLayerBufferDrawParam_004, Function | SmallTest | Level2)
{
    bool forceCPU = false;
    auto surfaceNode = RSTestUtil::CreateSurfaceNode();
    ASSERT_NE(surfaceNode, nullptr);
    LayerInfoPtr layer = HdiLayerInfo::CreateHdiLayerInfo();
    layer->SetBuffer(nullptr, surfaceNode->GetRSSurfaceHandler()->GetAcquireFence());
    auto csurface = IConsumerSurface::Create();
    layer->SetSurface(csurface);
    RSUniRenderUtil::CreateLayerBufferDrawParam(layer, forceCPU);
}

/*
 * @tc.name: GetRotationFromMatrix
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSUniRenderTest, GetRotationFromMatrix, Function | SmallTest | Level2)
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
HWTEST_F(RSUniRenderTest, GetRotationDegreeFromMatrix, Function | SmallTest | Level2)
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
HWTEST_F(RSUniRenderTest, Is3DRotation_001, Function | SmallTest | Level2)
{
    bool is3DRotation;
    Drawing::Matrix matrix = Drawing::Matrix();
    matrix.SetMatrix(1, 0, 0, 0, 1, 0, 0, 0, 1);
    is3DRotation = RSUniRenderUtil::Is3DRotation(matrix);
    ASSERT_FALSE(is3DRotation);
}

/*
 * @tc.name: Is3DRotation_002
 * @tc.desc: test Is3DRotation with ScaleX and ScaleY have same sign
 * @tc.type: FUNC
 * @tc.require: #IANUEG
 */
HWTEST_F(RSUniRenderTest, Is3DRotation_002, Function | SmallTest | Level2)
{
    bool is3DRotation;
    Drawing::Matrix matrix = Drawing::Matrix();
    matrix.SetMatrix(-1, 0, 0, 0, -1, 0, 0, 0, 1);
    is3DRotation = RSUniRenderUtil::Is3DRotation(matrix);
    ASSERT_FALSE(is3DRotation);
}

/*
 * @tc.name: Is3DRotation_003
 * @tc.desc: Test Is3DRotation when ScaleX and ScaleY have different sign
 * @tc.type: FUNC
 * @tc.require: issueIAJOWI
 */
HWTEST_F(RSUniRenderTest, Is3DRotation_003, Function | SmallTest | Level2)
{
    bool is3DRotation;
    Drawing::Matrix matrix = Drawing::Matrix();
    matrix.SetMatrix(-1, 0, 0, 0, 1, 0, 0, 0, 1);
    is3DRotation = RSUniRenderUtil::Is3DRotation(matrix);
    ASSERT_TRUE(is3DRotation);
}

/*
 * @tc.name: AssignWindowNodes
 * @tc.desc: Test AssignWindowNodes when displaynode is nullptr, or has no child.
 * @tc.type: FUNC
 * @tc.require: issueIATLPV
 */
HWTEST_F(RSUniRenderTest, AssignWindowNodes, Function | SmallTest | Level2)
{
    std::list<std::shared_ptr<RSSurfaceRenderNode>> mainThreadNodes;
    std::list<std::shared_ptr<RSSurfaceRenderNode>> subThreadNodes;
    RSUniRenderUtil::AssignWindowNodes(nullptr, mainThreadNodes, subThreadNodes);
    ASSERT_TRUE(mainThreadNodes.empty());
    ASSERT_TRUE(subThreadNodes.empty());
    NodeId id = 0;
    RSDisplayNodeConfig config;
    auto node = std::make_shared<RSDisplayRenderNode>(id, config);
    RSUniRenderUtil::AssignWindowNodes(node, mainThreadNodes, subThreadNodes);
    ASSERT_TRUE(mainThreadNodes.empty());
    ASSERT_TRUE(subThreadNodes.empty());
}

/*
 * @tc.name: AssignWindowNodes002
 * @tc.desc: Test AssignWindowNodes when mainThreadNodes and subThreadNodes is not empty
 * @tc.type: FUNC
 * @tc.require: issueIAKA4Y
 */
HWTEST_F(RSUniRenderTest, AssignWindowNodes002, Function | SmallTest | Level2)
{
    std::list<std::shared_ptr<RSSurfaceRenderNode>> mainThreadNodes;
    std::list<std::shared_ptr<RSSurfaceRenderNode>> subThreadNodes;
    NodeId id = 1;
    RSDisplayNodeConfig config;
    auto displayNode = std::make_shared<RSDisplayRenderNode>(id, config);
    ASSERT_NE(displayNode, nullptr);
    std::shared_ptr<RSSurfaceRenderNode> node1 = std::make_shared<RSSurfaceRenderNode>(1);
    ASSERT_NE(node1, nullptr);
    std::shared_ptr<RSSurfaceRenderNode> node2 = std::make_shared<RSSurfaceRenderNode>(2);
    ASSERT_NE(node2, nullptr);
    mainThreadNodes.push_back(node1);
    mainThreadNodes.push_back(node2);
    subThreadNodes.push_back(node1);
    subThreadNodes.push_back(node2);
    RSUniRenderUtil::AssignWindowNodes(displayNode, mainThreadNodes, subThreadNodes);
}

/*
 * @tc.name: ClearSurfaceIfNeed
 * @tc.desc: Test ClearSurfaceIfNeed with various displayNode
 * @tc.type: FUNC
 * @tc.require: issueIAKA4Y
 */
HWTEST_F(RSUniRenderTest, ClearSurfaceIfNeed, Function | SmallTest | Level2)
{
    RSRenderNodeMap map;
    std::set<std::shared_ptr<RSBaseRenderNode>> oldChildren;
    RSUniRenderUtil::ClearSurfaceIfNeed(map, nullptr, oldChildren);
    NodeId id = 1;
    RSDisplayNodeConfig config;
    auto displayNode = std::make_shared<RSDisplayRenderNode>(id, config);
    ASSERT_NE(displayNode, nullptr);
    RSUniRenderUtil::ClearSurfaceIfNeed(map, displayNode, oldChildren);
}

/*
 * @tc.name: ClearSurfaceIfNeed002
 * @tc.desc: Test ClearSurfaceIfNeed when oldChildren.size > 0
 * @tc.type: FUNC
 * @tc.require: issueIAKA4Y
 */
HWTEST_F(RSUniRenderTest, ClearSurfaceIfNeed002, Function | SmallTest | Level2)
{
    RSRenderNodeMap map;
    NodeId id = 1;
    RSDisplayNodeConfig config;
    auto displayNode = std::make_shared<RSDisplayRenderNode>(id, config);
    ASSERT_NE(displayNode, nullptr);
    std::set<std::shared_ptr<RSBaseRenderNode>> oldChildren;
    std::shared_ptr<RSBaseRenderNode> node1 = std::make_shared<RSBaseRenderNode>(1);
    ASSERT_NE(node1, nullptr);
    std::shared_ptr<RSBaseRenderNode> node2 = std::make_shared<RSBaseRenderNode>(2);
    ASSERT_NE(node2, nullptr);
    oldChildren.insert(node1);
    oldChildren.insert(node2);
    DeviceType deviceType = DeviceType::PHONE;
    RSUniRenderUtil::ClearSurfaceIfNeed(map, displayNode, oldChildren, deviceType);
}

/*
 * @tc.name: ClearCacheSurface
 * @tc.desc: Test ClearCacheSurface when threadIndex = 0
 * @tc.type: FUNC
 * @tc.require: issueIAKA4Y
 */
HWTEST_F(RSUniRenderTest, ClearCacheSurface, Function | SmallTest | Level2)
{
    NodeId id = 1;
    std::shared_ptr<RSRenderNode> node = std::make_shared<RSRenderNode>(id);
    ASSERT_NE(node, nullptr);
    uint32_t threadIndex = 0;
    RSUniRenderUtil::ClearCacheSurface(*node, threadIndex);
}

/*
 * @tc.name: ClearCacheSurface002
 * @tc.desc: Test ClearCacheSurface reset cacheSurfaceThreadIndex_ and completedSurfaceThreadIndex_
 * @tc.type: FUNC
 * @tc.require: issueIAKA4Y
 */
HWTEST_F(RSUniRenderTest, ClearCacheSurface002, Function | SmallTest | Level2)
{
    NodeId id = 1;
    std::shared_ptr<RSRenderNode> node = std::make_shared<RSRenderNode>(id);
    ASSERT_NE(node, nullptr);
    uint32_t threadIndex = UNI_MAIN_THREAD_INDEX;
    RSUniRenderUtil::ClearCacheSurface(*node, threadIndex);
    node->cacheSurfaceThreadIndex_ = UNI_RENDER_THREAD_INDEX;
    node->completedSurfaceThreadIndex_ = UNI_RENDER_THREAD_INDEX;
    RSUniRenderUtil::ClearCacheSurface(*node, threadIndex);
}

/*
 * @tc.name: PostReleaseSurfaceTask001
 * @tc.desc: Test PostReleaseSurfaceTask when surface is not nullptr
 * @tc.type: FUNC
 * @tc.require: issueIAJOWI
 */
HWTEST_F(RSUniRenderTest, PostReleaseSurfaceTask001, Function | SmallTest | Level2)
{
    int width = 200;
    int height = 200;
    const Drawing::ImageInfo info =
        Drawing::ImageInfo { width, height, Drawing::COLORTYPE_N32, Drawing::ALPHATYPE_OPAQUE };
    auto surface(Drawing::Surface::MakeRaster(info));
    ASSERT_NE(surface, nullptr);
    uint32_t threadIndex = UNI_MAIN_THREAD_INDEX;
    RSUniRenderUtil::PostReleaseSurfaceTask(std::move(surface), threadIndex);
    threadIndex = UNI_RENDER_THREAD_INDEX;
    RSUniRenderUtil::PostReleaseSurfaceTask(std::move(surface), threadIndex);
}

/*
 * @tc.name: PostReleaseSurfaceTask002
 * @tc.desc: Test PostReleaseSurfaceTask when surface is nullptr and change threadIndex
 * @tc.type: FUNC
 * @tc.require: issueIAKA4Y
 */
HWTEST_F(RSUniRenderTest, PostReleaseSurfaceTask002, Function | SmallTest | Level2)
{
    int width = 200;
    int height = 200;
    const Drawing::ImageInfo info =
        Drawing::ImageInfo { width, height, Drawing::COLORTYPE_N32, Drawing::ALPHATYPE_OPAQUE };
    uint32_t threadIndex = INVALID_NODEID;
    RSUniRenderUtil::PostReleaseSurfaceTask(nullptr, threadIndex);
    auto surface(Drawing::Surface::MakeRaster(info));
    ASSERT_NE(surface, nullptr);
    RSUniRenderUtil::PostReleaseSurfaceTask(std::move(surface), threadIndex);
}

/*
 * @tc.name: ClearNodeCacheSurface
 * @tc.desc: Test ClearNodeCacheSurface
 * @tc.type: FUNC
 * @tc.require: issueIATLPV
 */
HWTEST_F(RSUniRenderTest, ClearNodeCacheSurface, Function | SmallTest | Level2)
{
    uint32_t threadIndex = 1;
    RSUniRenderUtil::ClearNodeCacheSurface(nullptr, nullptr, threadIndex, 0);
    NodeId id = 0;
    RSDisplayNodeConfig config;
    auto node = std::make_shared<RSDisplayRenderNode>(id, config);
    ASSERT_NE(node, nullptr);
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
 * @tc.name: HandleCaptureNode001
 * @tc.desc: Test RSUniRenderUtil::HandleCaptureNode api when sufaceNode is nullptr
 * @tc.type: FUNC
 * @tc.require: issueIAKDJI
 */
HWTEST_F(RSUniRenderTest, HandleCaptureNode001, Function | SmallTest | Level2)
{
    Drawing::Canvas drawingCanvas;
    RSPaintFilterCanvas canvas(&drawingCanvas);
    auto surfaceNode = RSTestUtil::CreateSurfaceNode();
    ASSERT_NE(surfaceNode, nullptr);
    RSUniRenderUtil::HandleCaptureNode(*surfaceNode, canvas);
}

/*
 * @tc.name: HandleCaptureNode002
 * @tc.desc: Test RSUniRenderUtil::HandleCaptureNode api when node should not paint
 * @tc.type: FUNC
 * @tc.require: issueIAKDJI
 */
HWTEST_F(RSUniRenderTest, HandleCaptureNode002, Function | SmallTest | Level2)
{
    Drawing::Canvas drawingCanvas;
    RSPaintFilterCanvas canvas(&drawingCanvas);
    auto surfaceNode = RSTestUtil::CreateSurfaceNode();
    auto node = RSTestUtil::CreateSurfaceNode();
    auto drawable = std::make_shared<DrawableV2::RSSurfaceRenderNodeDrawable>(node);
    auto param = std::make_unique<RSSurfaceRenderParams>(node->id_);
    param->shouldPaint_ = false;
    param->contentEmpty_ = false;
    param->isMainWindowType_ = true;
    param->isLeashWindow_ = false;
    param->isAppWindow_ = false;
    drawable->renderParams_ = std::move(param);
    surfaceNode->renderDrawable_ = drawable;
    ASSERT_NE(surfaceNode, nullptr);
    RSUniRenderUtil::HandleCaptureNode(*surfaceNode, canvas);
}

/*
 * @tc.name: HandleCaptureNode003
 * @tc.desc: Test RSUniRenderUtil::HandleCaptureNode api when node is on the tree
 * @tc.type: FUNC
 * @tc.require: issueIAKDJI
 */
HWTEST_F(RSUniRenderTest, HandleCaptureNode003, Function | SmallTest | Level2)
{
    Drawing::Canvas drawingCanvas;
    RSPaintFilterCanvas canvas(&drawingCanvas);
    auto surfaceNode = RSTestUtil::CreateSurfaceNode();
    surfaceNode->isOnTheTree_ = true;
    auto node = RSTestUtil::CreateSurfaceNode();
    auto drawable = std::make_shared<DrawableV2::RSSurfaceRenderNodeDrawable>(node);
    auto param = std::make_unique<RSSurfaceRenderParams>(node->id_);
    param->shouldPaint_ = true;
    param->contentEmpty_ = true;
    param->isMainWindowType_ = true;
    param->isLeashWindow_ = false;
    param->isAppWindow_ = false;
    drawable->renderParams_ = std::move(param);
    surfaceNode->renderDrawable_ = drawable;
    ASSERT_NE(surfaceNode, nullptr);
    RSUniRenderUtil::HandleCaptureNode(*surfaceNode, canvas);
}

/*
 * @tc.name: HandleCaptureNode004
 * @tc.desc: Test RSUniRenderUtil::HandleCaptureNode api when node is not on the tree
 * @tc.type: FUNC
 * @tc.require: issueIAKDJI
 */
HWTEST_F(RSUniRenderTest, HandleCaptureNode004, Function | SmallTest | Level2)
{
    Drawing::Canvas drawingCanvas;
    RSPaintFilterCanvas canvas(&drawingCanvas);
    auto surfaceNode = RSTestUtil::CreateSurfaceNode();
    surfaceNode->isOnTheTree_ = false;
    auto node = RSTestUtil::CreateSurfaceNode();
    auto drawable = std::make_shared<DrawableV2::RSSurfaceRenderNodeDrawable>(node);
    auto param = std::make_unique<RSSurfaceRenderParams>(node->id_);
    param->shouldPaint_ = true;
    param->contentEmpty_ = true;
    param->isMainWindowType_ = true;
    param->isLeashWindow_ = false;
    param->isAppWindow_ = false;
    drawable->renderParams_ = std::move(param);
    surfaceNode->renderDrawable_ = drawable;
    ASSERT_NE(surfaceNode, nullptr);
    RSUniRenderUtil::HandleCaptureNode(*surfaceNode, canvas);
}

/*
 * @tc.name: AssignMainThreadNode
 * @tc.desc:Test RSUniRenderTest.AssignMainThreadNode
 * @tc.type: FUNC
 * @tc.require:issueI7KK3I
 */
HWTEST_F(RSUniRenderTest, AssignMainThreadNode, Function | SmallTest | Level2)
{
    std::list<std::shared_ptr<RSSurfaceRenderNode>> mainThreadNodes;
    RSUniRenderUtil::AssignMainThreadNode(mainThreadNodes, nullptr);
    ASSERT_EQ(0, mainThreadNodes.size());
    auto node = RSTestUtil::CreateSurfaceNode();
    RSUniRenderUtil::AssignMainThreadNode(mainThreadNodes, node);
    ASSERT_EQ(1, mainThreadNodes.size());
}

/*
 * @tc.name: AssignMainThreadNode002
 * @tc.desc:Test AssignMainThreadNode002 when node.isMainThreadNode_ is false
 * @tc.type: FUNC
 * @tc.require: issueIAJOWI
 */
HWTEST_F(RSUniRenderTest, AssignMainThreadNode002, Function | SmallTest | Level2)
{
    std::list<std::shared_ptr<RSSurfaceRenderNode>> mainThreadNodes;
    ASSERT_EQ(0, mainThreadNodes.size());
    auto node = RSTestUtil::CreateSurfaceNode();
    node->isMainThreadNode_ = false;
    RSUniRenderUtil::AssignMainThreadNode(mainThreadNodes, node);
    ASSERT_EQ(1, mainThreadNodes.size());
}

/*
 * @tc.name: AssignSubThreadNode
 * @tc.desc:Test RSUniRenderTest.AssignSubThreadNode
 * @tc.type: FUNC
 * @tc.require:issueI7KK3I
 */
HWTEST_F(RSUniRenderTest, AssignSubThreadNode, Function | SmallTest | Level2)
{
    std::list<std::shared_ptr<RSSurfaceRenderNode>> subThreadNodes;
    RSUniRenderUtil::AssignSubThreadNode(subThreadNodes, nullptr);
    ASSERT_EQ(0, subThreadNodes.size());
    auto node = RSTestUtil::CreateSurfaceNode();
    RSUniRenderUtil::AssignSubThreadNode(subThreadNodes, node);
    ASSERT_EQ(1, subThreadNodes.size());
}

/*
 * @tc.name: AssignSubThreadNode002
 * @tc.desc:Test RSUniRenderTest.AssignSubThreadNode when reset node.cacheProcessStatus_
 * @tc.type: FUNC
 * @tc.require: issueIAKA4Y
 */
HWTEST_F(RSUniRenderTest, AssignSubThreadNode002, Function | SmallTest | Level2)
{
    std::list<std::shared_ptr<RSSurfaceRenderNode>> subThreadNodes;
    auto node = RSTestUtil::CreateSurfaceNode();
    node->cacheProcessStatus_ = CacheProcessStatus::DONE;
    RSUniRenderUtil::AssignSubThreadNode(subThreadNodes, node);
    ASSERT_EQ(1, subThreadNodes.size());
}

/*
 * @tc.name: IsNodeAssignSubThread001
 * @tc.desc: test IsNodeAssignSubThread for non focus node
 * @tc.type: FUNC
 * @tc.require: issueI904G4
 */
HWTEST_F(RSUniRenderTest, IsNodeAssignSubThread001, Function | SmallTest | Level2)
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
HWTEST_F(RSUniRenderTest, IsNodeAssignSubThread002, Function | SmallTest | Level2)
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
HWTEST_F(RSUniRenderTest, IsNodeAssignSubThread003, Function | SmallTest | Level2)
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
HWTEST_F(RSUniRenderTest, IsNodeAssignSubThread004, Function | SmallTest | Level2)
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
HWTEST_F(RSUniRenderTest, IsNodeAssignSubThread005, Function | SmallTest | Level2)
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

/*
 * @tc.name: IsNodeAssignSubThread006
 * @tc.desc: test IsNodeAssignSubThread when nodeType_ is reset
 * @tc.type: FUNC
 * @tc.require: issueIAJOWI
 */
HWTEST_F(RSUniRenderTest, IsNodeAssignSubThread006, Function | SmallTest | Level2)
{
    NodeId id = 1;
    RSDisplayNodeConfig config;
    auto displayNode = std::make_shared<RSDisplayRenderNode>(id, config);
    auto surfaceNode = RSTestUtil::CreateSurfaceNode();
    surfaceNode->nodeType_ = RSSurfaceNodeType::SELF_DRAWING_NODE;
    ASSERT_NE(displayNode, nullptr);
    ASSERT_NE(surfaceNode, nullptr);
    displayNode->AddChild(surfaceNode);
    surfaceNode->SetCacheSurfaceProcessedStatus(CacheProcessStatus::WAITING);
    
    ASSERT_EQ(RSUniRenderUtil::IsNodeAssignSubThread(surfaceNode, displayNode->IsRotationChanged()), false);
}

/**
 * @tc.name: IsNeedClientsTest
 * @tc.desc: Verify function IsNeedClient
 * @tc.type:FUNC
 * @tc.require:issuesI9KRF1
 */
HWTEST_F(RSUniRenderTest, IsNeedClientsTest, Function | SmallTest | Level2)
{
    auto rsSurfaceRenderNode = RSTestUtil::CreateSurfaceNode();
    ASSERT_NE(rsSurfaceRenderNode, nullptr);
    RSSurfaceRenderNode& node = static_cast<RSSurfaceRenderNode&>(*(rsSurfaceRenderNode.get()));
    ComposeInfo info;
    EXPECT_FALSE(RSUniRenderUtil::IsNeedClient(node, info));
}

/**
 * @tc.name: IsNeedClientTest002
 * @tc.desc: Test IsNeedClient with modify params such as Rotation
 * @tc.type:FUNC
 * @tc.require: issueIAJOWI
 */
HWTEST_F(RSUniRenderTest, IsNeedClientTest002, Function | SmallTest | Level2)
{
    std::shared_ptr<RSSurfaceRenderNode> node = std::make_shared<RSSurfaceRenderNode>(1);
    ASSERT_NE(node, nullptr);
    ComposeInfo info;
    node->renderContent_->renderProperties_.SetRotation(1.0f);
    EXPECT_TRUE(RSUniRenderUtil::IsNeedClient(*node, info));
    node->renderContent_->renderProperties_.SetRotationX(1.0f);
    EXPECT_TRUE(RSUniRenderUtil::IsNeedClient(*node, info));
    node->renderContent_->renderProperties_.SetRotationY(1.0f);
    EXPECT_TRUE(RSUniRenderUtil::IsNeedClient(*node, info));
    Quaternion quaternion(90.0f, 90.0f, 90.0f, 90.0f);
    node->renderContent_->renderProperties_.SetQuaternion(quaternion);
    EXPECT_TRUE(RSUniRenderUtil::IsNeedClient(*node, info));
}

/*
 * @tc.name: DrawRectForDfxTest
 * @tc.desc: Verify function DrawRectForDfx
 * @tc.type: FUNC
 * @tc.require: issuesI9KRF1
 */
HWTEST_F(RSUniRenderTest, DrawRectForDfxTest, Function | SmallTest | Level2)
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
HWTEST_F(RSUniRenderTest, DealWithNodeGravityTest, Function | SmallTest | Level2)
{
    NodeId id = 0;
    RSSurfaceRenderNode node(id);
    ScreenInfo screenInfo;
    node.GetRSSurfaceHandler()->buffer_.buffer = OHOS::SurfaceBuffer::Create();
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
 * @tc.name: DealWithNodeGravityTest002
 * @tc.desc: Test DealWithNodeGravity when buffer is nullptr
 * @tc.type: FUNC
 * @tc.require: issueIAJBBO
 */
HWTEST_F(RSUniRenderTest, DealWithNodeGravityTest002, Function | SmallTest | Level2)
{
    NodeId id = 0;
    RSSurfaceRenderNode node(id);
    ScreenInfo screenInfo;
    node.GetRSSurfaceHandler()->buffer_.buffer = nullptr;
    RSUniRenderUtil::DealWithNodeGravity(node, screenInfo);
    EXPECT_TRUE(screenInfo.width == 0);
}

/*
 * @tc.name: DealWithNodeGravityTest003
 * @tc.desc: Test DealWithNodeGravity when screenInfo.rotation is modify
 * @tc.type: FUNC
 * @tc.require: issueIAKA4Y
 */
HWTEST_F(RSUniRenderTest, DealWithNodeGravityTest003, Function | SmallTest | Level2)
{
    NodeId id = 1;
    RSSurfaceRenderNode node(id);
    node.GetRSSurfaceHandler()->buffer_.buffer = OHOS::SurfaceBuffer::Create();
    ScreenInfo screenInfo;
    screenInfo.rotation = ScreenRotation::ROTATION_90;
    RSUniRenderUtil::DealWithNodeGravity(node, screenInfo);
    screenInfo.rotation = ScreenRotation::ROTATION_270;
    RSUniRenderUtil::DealWithNodeGravity(node, screenInfo);
    screenInfo.rotation = ScreenRotation::ROTATION_180;
    RSUniRenderUtil::DealWithNodeGravity(node, screenInfo);
    EXPECT_TRUE(screenInfo.width == 0);
}

/*
 * @tc.name: LayerRotateTest
 * @tc.desc: Verify function LayerRotate
 * @tc.type: FUNC
 * @tc.require: issuesI9KRF1
 */
HWTEST_F(RSUniRenderTest, LayerRotateTest, Function | SmallTest | Level2)
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
HWTEST_F(RSUniRenderTest, LayerCropTest, Function | SmallTest | Level2)
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
HWTEST_F(RSUniRenderTest, GetLayerTransformTest, Function | SmallTest | Level2)
{
    NodeId id = 0;
    RSSurfaceRenderNode node(id);
    ScreenInfo screenInfo;
    GraphicTransformType type = RSUniRenderUtil::GetLayerTransform(node, screenInfo);

    node.GetRSSurfaceHandler()->consumer_ = IConsumerSurface::Create();
    type = RSUniRenderUtil::GetLayerTransform(node, screenInfo);
    EXPECT_TRUE(type == GraphicTransformType::GRAPHIC_ROTATE_NONE);
}

/*
 * @tc.name: GetLayerTransformTest002
 * @tc.desc: Test GetLayerTransform when consumer and buffer is not nullptr
 * @tc.type: FUNC
 * @tc.require: issueIAJBBO
 */
HWTEST_F(RSUniRenderTest, GetLayerTransformTest002, TestSize.Level2)
{
    NodeId id = 0;
    RSSurfaceRenderNode node(id);
    node.surfaceHandler_ = nullptr;
    ScreenInfo screenInfo;
    GraphicTransformType type = RSUniRenderUtil::GetLayerTransform(node, screenInfo);

    node.surfaceHandler_ = std::make_shared<RSSurfaceHandler>(id);
    ASSERT_NE(node.surfaceHandler_, nullptr);
    node.GetRSSurfaceHandler()->buffer_.buffer = OHOS::SurfaceBuffer::Create();
    node.GetRSSurfaceHandler()->consumer_ = IConsumerSurface::Create();
    type = RSUniRenderUtil::GetLayerTransform(node, screenInfo);
    EXPECT_TRUE(type == GraphicTransformType::GRAPHIC_ROTATE_NONE);
}


/*
 * @tc.name: SrcRectRotateTransformTest
 * @tc.desc: Verify function SrcRectRotateTransform
 * @tc.type: FUNC
 * @tc.require: issuesI9KRF1
 */
HWTEST_F(RSUniRenderTest, SrcRectRotateTransformTest, Function | SmallTest | Level2)
{
    NodeId id = 0;
    RSSurfaceRenderNode node(id);
    RSUniRenderUtil::SrcRectRotateTransform(node, GraphicTransformType::GRAPHIC_ROTATE_NONE);

    node.GetRSSurfaceHandler()->consumer_ = IConsumerSurface::Create();
    RSUniRenderUtil::SrcRectRotateTransform(node, GraphicTransformType::GRAPHIC_ROTATE_NONE);
    EXPECT_FALSE(node.GetRSSurfaceHandler()->GetBuffer());
}

/*
 * @tc.name: SrcRectRotateTransformTest002
 * @tc.desc: Verify function SrcRectRotateTransform
 * @tc.type: FUNC
 * @tc.require: issuesI9KRF1
 */
HWTEST_F(RSUniRenderTest, SrcRectRotateTransformTest002, Function | SmallTest | Level2)
{
    NodeId id = 0;
    RSSurfaceRenderNode node(id);
    node.GetRSSurfaceHandler()->consumer_ = IConsumerSurface::Create();
    node.GetRSSurfaceHandler()->GetConsumer()->SetTransform(GraphicTransformType::GRAPHIC_FLIP_V_ROT270);
    RSUniRenderUtil::SrcRectRotateTransform(node, GraphicTransformType::GRAPHIC_FLIP_V_ROT270);
    node.GetRSSurfaceHandler()->GetConsumer()->SetTransform(GraphicTransformType::GRAPHIC_FLIP_V_ROT180);
    RSUniRenderUtil::SrcRectRotateTransform(node, GraphicTransformType::GRAPHIC_FLIP_V_ROT180);
    node.GetRSSurfaceHandler()->GetConsumer()->SetTransform(GraphicTransformType::GRAPHIC_FLIP_V_ROT90);
    RSUniRenderUtil::SrcRectRotateTransform(node, GraphicTransformType::GRAPHIC_FLIP_V_ROT90);
    EXPECT_FALSE(node.GetRSSurfaceHandler()->GetBuffer());
}

/*
 * @tc.name: HandleHardwareNodeTest
 * @tc.desc: Verify function HandleHardwareNode
 * @tc.type: FUNC
 * @tc.require: issuesI9KRF1
 */
HWTEST_F(RSUniRenderTest, HandleHardwareNodeTest, Function | SmallTest | Level2)
{
    NodeId id = 0;
    auto node = std::make_shared<RSSurfaceRenderNode>(id);
    RSUniRenderUtil::HandleHardwareNode(node);
    node->hasHardwareNode_ = true;
    RSUniRenderUtil::HandleHardwareNode(node);
    node->children_.emplace_back(std::make_shared<RSRenderNode>(id));
    node->nodeType_ = RSSurfaceNodeType::LEASH_WINDOW_NODE;
    RSUniRenderUtil::HandleHardwareNode(node);
    EXPECT_FALSE(node->GetRSSurfaceHandler()->GetBuffer());
}

/*
 * @tc.name: HandleHardwareNodeTest002
 * @tc.desc: Test HandleHardwareNode add child to node
 * @tc.type: FUNC
 * @tc.require: issueIAJOWI
 */
HWTEST_F(RSUniRenderTest, HandleHardwareNodeTest002, Function | SmallTest | Level2)
{
    NodeId id = 1;
    auto node = std::make_shared<RSSurfaceRenderNode>(id);
    ASSERT_NE(node, nullptr);
    node->hasHardwareNode_ = true;
    id = 2;
    auto child = std::make_shared<RSSurfaceRenderNode>(id);
    ASSERT_NE(child, nullptr);
    node->AddChild(child);
    RSUniRenderUtil::HandleHardwareNode(node);
}

/*
 * @tc.name: HandleHardwareNodeTest003
 * @tc.desc: Test HandleHardwareNode add childHardwareEnabledNodes_ to node
 * @tc.type: FUNC
 * @tc.require: issueIAKA4Y
 */
HWTEST_F(RSUniRenderTest, HandleHardwareNodeTest003, Function | SmallTest | Level2)
{
    NodeId id = 1;
    auto node = std::make_shared<RSSurfaceRenderNode>(id);
    ASSERT_NE(node, nullptr);
    node->hasHardwareNode_ = true;
    id = 2;
    auto child = std::make_shared<RSSurfaceRenderNode>(id);
    ASSERT_NE(child, nullptr);
    node->AddChild(child);

    std::shared_ptr<RSSurfaceRenderNode> rsNode = nullptr;
    std::weak_ptr<RSSurfaceRenderNode> rsNodeF = rsNode;
    std::weak_ptr<RSSurfaceRenderNode> rsNodeT = std::make_shared<RSSurfaceRenderNode>(1);
    node->childHardwareEnabledNodes_.emplace_back(rsNodeF);
    node->childHardwareEnabledNodes_.emplace_back(rsNodeT);
    RSUniRenderUtil::HandleHardwareNode(node);
}

/*
 * @tc.name: UpdateRealSrcRectTest
 * @tc.desc: Verify function UpdateRealSrcRect
 * @tc.type: FUNC
 * @tc.require: issuesI9KRF1
 */
HWTEST_F(RSUniRenderTest, UpdateRealSrcRectTest, Function | SmallTest | Level2)
{
    NodeId id = 0;
    RSSurfaceRenderNode node(id);
    RectI absRect;
    node.GetRSSurfaceHandler()->buffer_.buffer = OHOS::SurfaceBuffer::Create();
    node.GetRSSurfaceHandler()->consumer_ = IConsumerSurface::Create();
    RSUniRenderUtil::UpdateRealSrcRect(node, absRect);

    absRect = RectI(1, 1, 1, 1);
    RSUniRenderUtil::UpdateRealSrcRect(node, absRect);
    EXPECT_TRUE(node.GetRSSurfaceHandler()->buffer_.buffer);
}

/*
 * @tc.name: UpdateRealSrcRectTest002
 * @tc.desc: Test UpdateRealSrcRect when consumer is nullptr
 * @tc.type: FUNC
 * @tc.require: issueIAJBBO
 */
HWTEST_F(RSUniRenderTest, UpdateRealSrcRectTest002, Function | SmallTest | Level2)
{
    NodeId id = 0;
    RSSurfaceRenderNode node(id);
    node.stagingRenderParams_ = std::make_unique<RSSurfaceRenderParams>(node.GetId());
    RectI absRect;
    node.GetRSSurfaceHandler()->buffer_.buffer = OHOS::SurfaceBuffer::Create();
    node.GetRSSurfaceHandler()->consumer_ = nullptr;
    RSUniRenderUtil::UpdateRealSrcRect(node, absRect);

    node.GetRSSurfaceHandler()->consumer_ = IConsumerSurface::Create();
    node.GetRSSurfaceHandler()->GetConsumer()->SetTransform(GraphicTransformType::GRAPHIC_FLIP_V_ROT90);
    RSUniRenderUtil::UpdateRealSrcRect(node, absRect);
    node.GetRSSurfaceHandler()->GetConsumer()->SetTransform(GraphicTransformType::GRAPHIC_FLIP_V_ROT270);
    RSUniRenderUtil::UpdateRealSrcRect(node, absRect);

    constexpr uint32_t default_canvas_width = 800;
    constexpr uint32_t default_canvas_height = 600;
    int left = 1;
    int top = 1;
    int right = 1;
    int bottom = 1;
    absRect = RectI(left, top, right, bottom);
    node.GetMutableRenderProperties().SetBoundsWidth(default_canvas_width);
    node.GetMutableRenderProperties().SetBoundsHeight(default_canvas_height);
    node.GetMutableRenderProperties().SetFrameGravity(Gravity::TOP_RIGHT);
    RSUniRenderUtil::UpdateRealSrcRect(node, absRect);
    EXPECT_TRUE(node.GetRSSurfaceHandler()->buffer_.buffer);
}

/*
 * @tc.name: CheckForceHardwareAndUpdateDstRectTest
 * @tc.desc: Verify function CheckForceHardwareAndUpdateDstRect
 * @tc.type: FUNC
 * @tc.require: issuesI9KRF1
 */
HWTEST_F(RSUniRenderTest, CheckForceHardwareAndUpdateDstRectTest, Function | SmallTest | Level2)
{
    NodeId id = 0;
    RSSurfaceRenderNode node(id);
    node.GetRSSurfaceHandler()->buffer_.buffer = OHOS::SurfaceBuffer::Create();
    RSUniRenderUtil::CheckForceHardwareAndUpdateDstRect(node);
    node.isFixRotationByUser_ = true;
    RSUniRenderUtil::CheckForceHardwareAndUpdateDstRect(node);
    EXPECT_TRUE(node.GetOriginalDstRect().IsEmpty());
}

/*
 * @tc.name: CheckForceHardwareAndUpdateDstRectTest002
 * @tc.desc: Test CheckForceHardwareAndUpdateDstRect without nullptr
 * @tc.type: FUNC
 * @tc.require: issueIAJBBO
 */
HWTEST_F(RSUniRenderTest, CheckForceHardwareAndUpdateDstRectTest002, Function | SmallTest | Level2)
{
    NodeId id = 0;
    RSSurfaceRenderNode node(id);
    node.isFixRotationByUser_ = true;
    node.isInFixedRotation_ = true;
    node.GetRSSurfaceHandler()->buffer_.buffer = OHOS::SurfaceBuffer::Create();
    node.GetRSSurfaceHandler()->consumer_ = IConsumerSurface::Create();
    RSUniRenderUtil::CheckForceHardwareAndUpdateDstRect(node);
    EXPECT_TRUE(node.GetOriginalDstRect().IsEmpty());
}
} // namespace OHOS::Rosen
