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
#include "feature/capture/rs_surface_capture_task_parallel.h"
#include "params/rs_surface_render_params.h"
#include "pipeline/render_thread/rs_uni_render_util.h"
#include "pipeline/main_thread/rs_main_thread.h"
#include "pixel_map.h"
#include "property/rs_properties_def.h"
#include "render/rs_material_filter.h"
#include "render/rs_shadow.h"
#include "mock/mock_meta_data_helper.h"

using namespace testing;
using namespace testing::ext;
using namespace OHOS::Rosen::DrawableV2;

namespace OHOS::Rosen {
constexpr SkImageLeft = 50;
constexpr SkImageTop = 50;
constexpr SkImageWidth = 100;
constexpr SkImageHeight = 100;
class RSRenderUtilTest : public testing::Test {
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
    canvas->DrawRect(Drawing::Rect(SkImageLeft, SkImageTop, SkImageWidth, SkImageHeight));
    canvas->DetachBrush();
    return surface->GetImageSnapshot();
}

void RSRenderUtilTest::SetUpTestCase()
{
    RSTestUtil::InitRenderNodeGC();
}
void RSRenderUtilTest::TearDownTestCase() {}
void RSRenderUtilTest::SetUp() {}
void RSRenderUtilTest::TearDown() {}

/*
 * @tc.name: ScalingModeSrcRectCheck_001
 * @tc.desc: default value
 * @tc.type: FUNC
 * @tc.require:
*/
HWTEST_F(RSRenderUtilTest, ScalingModeSrcRectCheck_001, Function | SmallTest | Level2)
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
 * @tc.name: ScalingModeSrcRectCheck_002
 * @tc.desc: default value
 * @tc.type: FUNC
 * @tc.require:
*/
HWTEST_F(RSRenderUtilTest, ScalingModeSrcRectCheck_002, Function | SmallTest | Level2)
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
 * @tc.name: ScalingModeSrcRectCheck_003
 * @tc.desc: Test SrcRectScaleDown when srcRect is multiple
 * @tc.type: FUNC
 * @tc.require:
*/
HWTEST_F(RSRenderUtilTest, ScalingModeSrcRectCheck_003, Function | SmallTest | Level2)
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
 * @tc.name: ScalingModeSrcRectCheck_004
 * @tc.desc: default value
 * @tc.type: FUNC
 * @tc.require:
*/
HWTEST_F(RSRenderUtilTest, ScalingModeSrcRectCheck_004, Function | SmallTest | Level2)
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
 * @tc.name: SrcRectCheck_001
 * @tc.desc: default value
 * @tc.type: FUNC
 * @tc.require:
*/
HWTEST_F(RSRenderUtilTest, SrcRectCheck_001, Function | SmallTest | Level2)
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
 * @tc.name: SrcRectCheck_002
 * @tc.desc: Test SrcRectScaleFit when srcRect is multiple
 * @tc.type: FUNC
 * @tc.require:
*/
HWTEST_F(RSRenderUtilTest, SrcRectCheck_002, Function | SmallTest | Level2)
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
 * @tc.name: SrcRectCheck_003
 * @tc.desc: test GetMatrixOfBufferToRelRect with surfaceNode without buffer
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSRenderUtilTest, SrcRectCheck_003, Function | SmallTest | Level2)
{
    auto rsSurfaceRenderNode = RSTestUtil::CreateSurfaceNode();
    ASSERT_NE(rsSurfaceRenderNode, nullptr);
    RSSurfaceRenderNode& node = static_cast<RSSurfaceRenderNode&>(*(rsSurfaceRenderNode.get()));
    RSUniRenderUtil::GetMatrixOfBufferToRelRect(node);
}

/*
 * @tc.name: BufferCheck_001
 * @tc.desc: test GetMatrixOfBufferToRelRect with surfaceNode with buffer
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSRenderUtilTest, BufferCheck_001, Function | SmallTest | Level2)
{
    auto rsSurfaceRenderNode = RSTestUtil::CreateSurfaceNodeWithBuffer();
    ASSERT_NE(rsSurfaceRenderNode, nullptr);
    RSSurfaceRenderNode& node = static_cast<RSSurfaceRenderNode&>(*(rsSurfaceRenderNode.get()));
    RSUniRenderUtil::GetMatrixOfBufferToRelRect(node);
}

/*
 * @tc.name: BufferCheck_002
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSRenderUtilTest, BufferCheck_002, Function | SmallTest | Level2)
{
    bool forceCPU = false;
    LayerInfoPtr layer = HdiLayerInfo::CreateHdiLayerInfo();
    RSUniRenderUtil::CreateLayerBufferDrawParam(layer, forceCPU);
}

/*
 * @tc.name: BufferCheck_003
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSRenderUtilTest, BufferCheck_003, Function | SmallTest | Level2)
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
 * @tc.name: BufferCheck_004
 * @tc.desc: Test CreateLayerBufferDrawParam when buffer is nullptr
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSRenderUtilTest, BufferCheck_004, Function | SmallTest | Level2)
{
    bool forceCPU = false;
    auto surfaceNode = RSTestUtil::CreateSurfaceNode();
    ASSERT_NE(surfaceNode, nullptr);
    LayerInfoPtr layer = HdiLayerInfo::CreateHdiLayerInfo();
    layer->SetBuffer(nullptr, surfaceNode->GetRSSurfaceHandler()->GetAcquireFence());
    RSUniRenderUtil::CreateLayerBufferDrawParam(layer, forceCPU);
}

/*
 * @tc.name: BufferCheck_005
 * @tc.desc: Test CreateLayerBufferDrawParam when surface is not nullptr
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSRenderUtilTest, BufferCheck_005, Function | SmallTest | Level2)
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
 * @tc.name: MatrixCheck_001
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSRenderUtilTest, MatrixCheck_001, Function | SmallTest | Level2)
{
    int angle;
    Drawing::Matrix matrix = Drawing::Matrix();
    matrix.SetMatrix(1, 0, 0, 0, 1, 0, 0, 0, 1);
    angle = RSUniRenderUtil::GetRotationFromMatrix(matrix);
    ASSERT_EQ(angle, 0);
}

/*
 * @tc.name: MatrixCheck_002
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSRenderUtilTest, MatrixCheck_002, Function | SmallTest | Level2)
{
    int angle;
    Drawing::Matrix matrix = Drawing::Matrix();
    matrix.SetMatrix(1, 0, 0, 0, 1, 0, 0, 0, 1);
    angle = RSUniRenderUtil::GetRotationDegreeFromMatrix(matrix);
    ASSERT_EQ(angle, 0);
}

/*
 * @tc.name: MatrixCheck_003
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSRenderUtilTest, MatrixCheck_003, Function | SmallTest | Level2)
{
    bool is3DRotation;
    Drawing::Matrix matrix = Drawing::Matrix();
    matrix.SetMatrix(1, 0, 0, 0, 1, 0, 0, 0, 1);
    is3DRotation = RSUniRenderUtil::Is3DRotation(matrix);
    ASSERT_FALSE(is3DRotation);
}

/*
 * @tc.name: MatrixCheck_004
 * @tc.desc: test Is3DRotation with ScaleX and ScaleY have same sign
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSRenderUtilTest, MatrixCheck_004, Function | SmallTest | Level2)
{
    bool is3DRotation;
    Drawing::Matrix matrix = Drawing::Matrix();
    matrix.SetMatrix(-1, 0, 0, 0, -1, 0, 0, 0, 1);
    is3DRotation = RSUniRenderUtil::Is3DRotation(matrix);
    ASSERT_FALSE(is3DRotation);
}

/*
 * @tc.name: MatrixCheck_005
 * @tc.desc: Test Is3DRotation when ScaleX and ScaleY have different sign
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSRenderUtilTest, MatrixCheck_005, Function | SmallTest | Level2)
{
    bool is3DRotation;
    Drawing::Matrix matrix = Drawing::Matrix();
    matrix.SetMatrix(-1, 0, 0, 0, 1, 0, 0, 0, 1);
    is3DRotation = RSUniRenderUtil::Is3DRotation(matrix);
    ASSERT_TRUE(is3DRotation);
}

/*
 * @tc.name: NodeSuccessfulCheck_001
 * @tc.desc: Test AssignWindowNodes when displaynode is nullptr, or has no child.
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSRenderUtilTest, NodeSuccessfulCheck_001, Function | SmallTest | Level2)
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
 * @tc.name: NodeSuccessfulCheck_002
 * @tc.desc: NodeSuccessfulCheck_002
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSRenderUtilTest, NodeSuccessfulCheck_002, Function | SmallTest | Level2)
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
 * @tc.name: NodeSuccessfulCheck_003
 * @tc.desc: Test various displayNode
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSRenderUtilTest, NodeSuccessfulCheck_003, Function | SmallTest | Level2)
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
 * @tc.name: NodeSuccessfulCheck_004
 * @tc.desc: NodeSuccessfulCheck_004
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSRenderUtilTest, NodeSuccessfulCheck_004, Function | SmallTest | Level2)
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
 * @tc.name: NodeSuccessfulCheck_005
 * @tc.desc: NodeSuccessfulCheck_005
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSRenderUtilTest, NodeSuccessfulCheck_005, Function | SmallTest | Level2)
{
    NodeId id = 1;
    std::shared_ptr<RSRenderNode> node = std::make_shared<RSRenderNode>(id);
    ASSERT_NE(node, nullptr);
    uint32_t threadIndex = 0;
    RSUniRenderUtil::ClearCacheSurface(*node, threadIndex);
}

/*
 * @tc.name: NodeSuccessfulCheck_006
 * @tc.desc: NodeSuccessfulCheck_006
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSRenderUtilTest, NodeSuccessfulCheck_006, Function | SmallTest | Level2)
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
 * @tc.name: NodeSuccessfulCheck_007
 * @tc.desc: NodeSuccessfulCheck_007
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSRenderUtilTest, NodeSuccessfulCheck_007, Function | SmallTest | Level2)
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
 * @tc.name: NodeSuccessfulCheck_008
 * @tc.desc: NodeSuccessfulCheck_008
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSRenderUtilTest, NodeSuccessfulCheck_008, Function | SmallTest | Level2)
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
 * @tc.name: NodeSuccessfulCheck_009
 * @tc.desc: NodeSuccessfulCheck_009
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSRenderUtilTest, NodeSuccessfulCheck_009, Function | SmallTest | Level2)
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
 * @tc.name: NodeSuccessfulCheck_010
 * @tc.desc: NodeSuccessfulCheck_010
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSRenderUtilTest, NodeSuccessfulCheck_010, Function | SmallTest | Level2)
{
    Drawing::Canvas drawingCanvas;
    RSPaintFilterCanvas canvas(&drawingCanvas);
    auto surfaceNode = RSTestUtil::CreateSurfaceNode();
    ASSERT_NE(surfaceNode, nullptr);
    RSUniRenderUtil::HandleCaptureNode(*surfaceNode, canvas);
}

/*
 * @tc.name: HandleNode_001
 * @tc.desc: HandleNode_001
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSRenderUtilTest, HandleNode_001, Function | SmallTest | Level2)
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
 * @tc.name: HandleNode_002
 * @tc.desc: HandleNode_002
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSRenderUtilTest, HandleNode_002, Function | SmallTest | Level2)
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
 * @tc.name: HandleNode_003
 * @tc.desc: HandleNode_003
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSRenderUtilTest, HandleNode_003, Function | SmallTest | Level2)
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
 * @tc.name: HandleNode_004
 * @tc.desc: HandleNode_004
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSRenderUtilTest, HandleNode_004, Function | SmallTest | Level2)
{
    std::list<std::shared_ptr<RSSurfaceRenderNode>> mainThreadNodes;
    RSUniRenderUtil::AssignMainThreadNode(mainThreadNodes, nullptr);
    ASSERT_EQ(0, mainThreadNodes.size());
    auto node = RSTestUtil::CreateSurfaceNode();
    RSUniRenderUtil::AssignMainThreadNode(mainThreadNodes, node);
    ASSERT_EQ(1, mainThreadNodes.size());
}

/*
 * @tc.name: HandleNode_005
 * @tc.desc: HandleNode_005
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSRenderUtilTest, HandleNode_005, Function | SmallTest | Level2)
{
    std::list<std::shared_ptr<RSSurfaceRenderNode>> mainThreadNodes;
    ASSERT_EQ(0, mainThreadNodes.size());
    auto node = RSTestUtil::CreateSurfaceNode();
    node->isMainThreadNode_ = false;
    RSUniRenderUtil::AssignMainThreadNode(mainThreadNodes, node);
    ASSERT_EQ(1, mainThreadNodes.size());
}

/*
 * @tc.name: SubNodeCheck_001
 * @tc.desc: SubNodeCheck_001
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSRenderUtilTest, SubNodeCheck_001, Function | SmallTest | Level2)
{
    std::list<std::shared_ptr<RSSurfaceRenderNode>> subThreadNodes;
    RSUniRenderUtil::AssignSubThreadNode(subThreadNodes, nullptr);
    ASSERT_EQ(0, subThreadNodes.size());
    auto node = RSTestUtil::CreateSurfaceNode();
    RSUniRenderUtil::AssignSubThreadNode(subThreadNodes, node);
    ASSERT_EQ(1, subThreadNodes.size());
}

/*
 * @tc.name: SubNodeCheck_002
 * @tc.desc: SubNodeCheck_002
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSRenderUtilTest, SubNodeCheck_002, Function | SmallTest | Level2)
{
    std::list<std::shared_ptr<RSSurfaceRenderNode>> subThreadNodes;
    auto node = RSTestUtil::CreateSurfaceNode();
    node->cacheProcessStatus_ = CacheProcessStatus::DONE;
    RSUniRenderUtil::AssignSubThreadNode(subThreadNodes, node);
    ASSERT_EQ(1, subThreadNodes.size());
}

/*
 * @tc.name: SubNodeCheck_003
 * @tc.desc: SubNodeCheck_003
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSRenderUtilTest, SubNodeCheck_003, Function | SmallTest | Level2)
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
 * @tc.name: SubNodeCheck_004
 * @tc.desc: SubNodeCheck_004
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSRenderUtilTest, SubNodeCheck_004, Function | SmallTest | Level2)
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
 * @tc.name: SubNodeCheck_005
 * @tc.desc: SubNodeCheck_005
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSRenderUtilTest, SubNodeCheck_005, Function | SmallTest | Level2)
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
 * @tc.name: SubNodeCheck_006
 * @tc.desc: SubNodeCheck_006
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSRenderUtilTest, SubNodeCheck_006, Function | SmallTest | Level2)
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
 * @tc.name: SubNodeCheck_007
 * @tc.desc: SubNodeCheck_007
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSRenderUtilTest, SubNodeCheck_007, Function | SmallTest | Level2)
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
 * @tc.name: SubNodeCheck_008
 * @tc.desc: SubNodeCheck_008
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSRenderUtilTest, SubNodeCheck_008, Function | SmallTest | Level2)
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
 * @tc.name: SubNodeCheck_009
 * @tc.desc: SubNodeCheck_009
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSRenderUtilTest, SubNodeCheck_009, Function | SmallTest | Level2)
{
    auto rsSurfaceRenderNode = RSTestUtil::CreateSurfaceNode();
    ASSERT_NE(rsSurfaceRenderNode, nullptr);
    RSSurfaceRenderNode& node = static_cast<RSSurfaceRenderNode&>(*(rsSurfaceRenderNode.get()));
    ComposeInfo info;
    EXPECT_FALSE(RSUniRenderUtil::IsNeedClient(node, info));
}
} // namespace OHOS::Rosen
