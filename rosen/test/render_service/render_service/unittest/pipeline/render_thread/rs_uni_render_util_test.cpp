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
#include "foundation/graphic/graphic_2d/rosen/test/render_service/render_service/unittest/pipeline/rs_test_util.h"
#include "surface_buffer_impl.h"
#include "surface_type.h"

#include "drawable/dfx/rs_dirty_rects_dfx.h"
#include "drawable/rs_display_render_node_drawable.h"
#include "drawable/rs_surface_render_node_drawable.h"
#include "feature/capture/rs_surface_capture_task_parallel.h"
#include "foundation/graphic/graphic_2d/rosen/test/render_service/render_service/unittest/pipeline/mock/mock_meta_data_helper.h"
#include "params/rs_surface_render_params.h"
#include "pipeline/render_thread/rs_uni_render_util.h"
#include "pipeline/main_thread/rs_main_thread.h"
#include "pixel_map.h"
#include "property/rs_properties_def.h"
#include "render/rs_material_filter.h"
#include "render/rs_shadow.h"

using namespace testing;
using namespace testing::ext;
using namespace OHOS::Rosen::DrawableV2;

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

RSDisplayRenderNodeDrawable* GenerateDisplayDrawableById(NodeId id, RSDisplayNodeConfig config)
{
    std::shared_ptr<RSDisplayRenderNode> renderNode = std::make_shared<RSDisplayRenderNode>(id, config);
    if (!renderNode) {
        return nullptr;
    }
    RSRenderNodeDrawableAdapter* displayAdapter = RSDisplayRenderNodeDrawable::OnGenerate(renderNode);
    if (!displayAdapter) {
        return nullptr;
    }
    return static_cast<RSDisplayRenderNodeDrawable*>(displayAdapter);
}

void RSUniRenderUtilTest::SetUpTestCase()
{
    RSTestUtil::InitRenderNodeGC();
}
void RSUniRenderUtilTest::TearDownTestCase() {}
void RSUniRenderUtilTest::SetUp() {}
void RSUniRenderUtilTest::TearDown() {}

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
    RSUniRenderUtil::SrcRectScaleDown(
        params, node.GetRSSurfaceHandler()->GetBuffer(), node.GetRSSurfaceHandler()->GetConsumer(), localBounds);
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
    RSUniRenderUtil::SrcRectScaleDown(
        params, node.GetRSSurfaceHandler()->GetBuffer(), node.GetRSSurfaceHandler()->GetConsumer(), localBounds);
}

/*
 * @tc.name: SrcRectScaleDown_003
 * @tc.desc: Test SrcRectScaleDown when srcRect is multiple
 * @tc.type: FUNC
 * @tc.require: issueIAJOWI
*/
HWTEST_F(RSUniRenderUtilTest, SrcRectScaleDown_003, Function | SmallTest | Level2)
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
HWTEST_F(RSUniRenderUtilTest, SrcRectScaleFit_001, Function | SmallTest | Level2)
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
HWTEST_F(RSUniRenderUtilTest, SrcRectScaleFit_002, Function | SmallTest | Level2)
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
HWTEST_F(RSUniRenderUtilTest, SrcRectScaleFit_003, Function | SmallTest | Level2)
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
HWTEST_F(RSUniRenderUtilTest, CreateLayerBufferDrawParam_003, Function | SmallTest | Level2)
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
HWTEST_F(RSUniRenderUtilTest, CreateLayerBufferDrawParam_004, Function | SmallTest | Level2)
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
 * @tc.name: PostReleaseSurfaceTask001
 * @tc.desc: Test PostReleaseSurfaceTask when surface is not nullptr
 * @tc.type: FUNC
 * @tc.require: issueIAJOWI
 */
HWTEST_F(RSUniRenderUtilTest, PostReleaseSurfaceTask001, Function | SmallTest | Level2)
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
HWTEST_F(RSUniRenderUtilTest, PostReleaseSurfaceTask002, Function | SmallTest | Level2)
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
HWTEST_F(RSUniRenderUtilTest, ClearNodeCacheSurface, Function | SmallTest | Level2)
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
 * @tc.name: ProcessCacheImage
 * @tc.desc: ProcessCacheImage test with modify params
 * @tc.type: FUNC
 * @tc.require: issueIAJBBO
 */
HWTEST_F(RSUniRenderUtilTest, ProcessCacheImage, TestSize.Level2)
{
    RSUniRenderUtil rsUniRenderUtil;
    Drawing::Canvas drawingCanvas;
    RSPaintFilterCanvas canvas(&drawingCanvas);
    std::shared_ptr<Drawing::Image> image = std::make_shared<Drawing::Image>();
    ASSERT_NE(image, nullptr);

    rsUniRenderUtil.ProcessCacheImage(canvas, *image);
}

/*
 * @tc.name: ProcessCacheImageRect001
 * @tc.desc: ProcessCacheImageRect test with abnoraml rect params
 * @tc.type: FUNC
 * @tc.require: issueIB2KBH
 */
HWTEST_F(RSUniRenderUtilTest, ProcessCacheImageRect001, TestSize.Level2)
{
    RSUniRenderUtil rsUniRenderUtil;
    Drawing::Canvas drawingCanvas;
    RSPaintFilterCanvas canvas(&drawingCanvas);
    std::shared_ptr<Drawing::Image> image = std::make_shared<Drawing::Image>();
    ASSERT_NE(image, nullptr);

    auto src = Drawing::Rect(-10, -10, -100, -100);  // test value
    auto dst = src;
    rsUniRenderUtil.ProcessCacheImageRect(canvas, *image, src, dst);
}

/*
 * @tc.name: ProcessCacheImageRect002
 * @tc.desc: ProcessCacheImageRect test with noraml rect params
 * @tc.type: FUNC
 * @tc.require: issueIB2KBH
 */
HWTEST_F(RSUniRenderUtilTest, ProcessCacheImageRect002, TestSize.Level2)
{
    RSUniRenderUtil rsUniRenderUtil;
    Drawing::Canvas drawingCanvas;
    RSPaintFilterCanvas canvas(&drawingCanvas);
    std::shared_ptr<Drawing::Image> image = std::make_shared<Drawing::Image>();
    ASSERT_NE(image, nullptr);

    auto src = Drawing::Rect(50, 50, 100, 100);  // test value
    auto dst = src;
    rsUniRenderUtil.ProcessCacheImageRect(canvas, *image, src, dst);
}

/*
 * @tc.name: ProcessCacheImageForMultiScreenView
 * @tc.desc: ProcessCacheImageForMultiScreenView test with image width and height
 * @tc.type: FUNC
 * @tc.require: issueIB2KBH
 */
HWTEST_F(RSUniRenderUtilTest, ProcessCacheImageForMultiScreenView, TestSize.Level2)
{
    RSUniRenderUtil rsUniRenderUtil;
    Drawing::Canvas drawingCanvas;
    RSPaintFilterCanvas canvas(&drawingCanvas);
    std::shared_ptr<Drawing::Image> image = std::make_shared<Drawing::Image>();
    ASSERT_NE(image, nullptr);
    auto rect = RectF(1, 1, 10, 10);
    rsUniRenderUtil.ProcessCacheImageForMultiScreenView(canvas, *image, rect);
}

/*
 * @tc.name: TraverseAndCollectUIExtensionInfo001
 * @tc.desc: TraverseAndCollectUIExtensionInfo test when node is nullptr
 * @tc.type: FUNC
 * @tc.require: issueIAJBBO
 */
HWTEST_F(RSUniRenderUtilTest, TraverseAndCollectUIExtensionInfo001, TestSize.Level2)
{
    RSUniRenderUtil rsUniRenderUtil;
    std::shared_ptr<RSRenderNode> node = nullptr;
    ASSERT_EQ(node, nullptr);
    Drawing::Matrix parentMatrix = Drawing::Matrix();
    parentMatrix.SetMatrix(1, 0, 0, 0, 1, 0, 0, 0, 1);
    NodeId hostId = 0;
    UIExtensionCallbackData callbackData;

    rsUniRenderUtil.TraverseAndCollectUIExtensionInfo(node, parentMatrix, hostId, callbackData);
    ASSERT_EQ(callbackData.empty(), true);
}

/*
 * @tc.name: TraverseAndCollectUIExtensionInfo002
 * @tc.desc: TraverseAndCollectUIExtensionInfo test when node is not nullptr
 * @tc.type: FUNC
 * @tc.require: issueIAJBBO
 */
HWTEST_F(RSUniRenderUtilTest, TraverseAndCollectUIExtensionInfo002, TestSize.Level2)
{
    RSUniRenderUtil rsUniRenderUtil;
    std::shared_ptr<RSRenderNode> node = std::make_shared<RSRenderNode>(0);
    ASSERT_NE(node, nullptr);
    node->childrenHasUIExtension_ = true;
    Drawing::Matrix parentMatrix = Drawing::Matrix();
    parentMatrix.SetMatrix(1, 0, 0, 0, 1, 0, 0, 0, 1);
    NodeId hostId = 0;
    UIExtensionCallbackData callbackData;

    rsUniRenderUtil.TraverseAndCollectUIExtensionInfo(node, parentMatrix, hostId, callbackData);
    ASSERT_EQ(callbackData.empty(), true);
}

/*
 * @tc.name: TraverseAndCollectUIExtensionInfo003
 * @tc.desc: TraverseAndCollectUIExtensionInfo test when nodeType_ != nullptr
 * @tc.type: FUNC
 * @tc.require: issueIAKA4Y
 */
HWTEST_F(RSUniRenderUtilTest, TraverseAndCollectUIExtensionInfo003, TestSize.Level2)
{
    RSUniRenderUtil rsUniRenderUtil;
    std::shared_ptr<RSSurfaceRenderNode> node = std::make_shared<RSSurfaceRenderNode>(1);
    ASSERT_NE(node, nullptr);
    node->SetSurfaceNodeType(RSSurfaceNodeType::STARTING_WINDOW_NODE);
    node->childrenHasUIExtension_ = true;
    Drawing::Matrix parentMatrix = Drawing::Matrix();
    parentMatrix.SetMatrix(1, 0, 0, 0, 1, 0, 0, 0, 1);
    NodeId hostId = 1;
    UIExtensionCallbackData callbackData;
    rsUniRenderUtil.TraverseAndCollectUIExtensionInfo(node, parentMatrix, hostId, callbackData);
    ASSERT_EQ(callbackData.empty(), true);
}

HWTEST_F(RSUniRenderUtilTest, TraverseAndCollectUIExtensionInfo004, TestSize.Level2)
{
    RSUniRenderUtil rsUniRenderUtil;
    std::shared_ptr<RSSurfaceRenderNode> node = std::make_shared<RSSurfaceRenderNode>(1);
    ASSERT_NE(node, nullptr);

    node->childrenHasUIExtension_ = true;
    Drawing::Matrix parentMatrix = Drawing::Matrix();
    parentMatrix.SetMatrix(1, 0, 0, 0, 1, 0, 0, 0, 1);
    NodeId hostId = 1;
    UIExtensionCallbackData callbackData;
    rsUniRenderUtil.TraverseAndCollectUIExtensionInfo(node, parentMatrix, hostId, callbackData);
    ASSERT_EQ(callbackData.empty(), true);

    RSSurfaceRenderNodeConfig config = {.id = 2, .nodeType = RSSurfaceNodeType::UI_EXTENSION_COMMON_NODE};
    std::shared_ptr<RSSurfaceRenderNode> childNode = std::make_shared<RSSurfaceRenderNode>(config);
    ASSERT_NE(node, nullptr);
    childNode->SetUIExtensionUnobscured(true);
    node->AddChild(childNode);
    node->GenerateFullChildrenList();
    UIExtensionCallbackData callbackData1;
    rsUniRenderUtil.TraverseAndCollectUIExtensionInfo(node, parentMatrix, hostId, callbackData1, true);
    ASSERT_EQ(callbackData1.size() == 1, true);
}

/*
 * @tc.name: OptimizedFlushAndSubmit001
 * @tc.desc: OptimizedFlushAndSubmit test when gpuContext is nullptr
 * @tc.type: FUNC
 * @tc.require: issueIAJBBO
 */
HWTEST_F(RSUniRenderUtilTest, OptimizedFlushAndSubmit001, TestSize.Level2)
{
    RSUniRenderUtil rsUniRenderUtil;
    auto surface = std::make_shared<Drawing::Surface>();
    ASSERT_NE(surface, nullptr);
    Drawing::GPUContext* gpuContext = nullptr;
    bool optFenceWait = false;

    rsUniRenderUtil.OptimizedFlushAndSubmit(surface, gpuContext, optFenceWait);
}

/*
 * @tc.name: OptimizedFlushAndSubmit002
 * @tc.desc: OptimizedFlushAndSubmit test when surface and gpuContext is multiple
 * @tc.type: FUNC
 * @tc.require: issueIALXTP
 */
HWTEST_F(RSUniRenderUtilTest, OptimizedFlushAndSubmit002, TestSize.Level2)
{
    RSUniRenderUtil rsUniRenderUtil;
    bool optFenceWait = false;
    std::shared_ptr<Drawing::Surface> surface = nullptr;
    rsUniRenderUtil.OptimizedFlushAndSubmit(surface, nullptr, optFenceWait);
    std::shared_ptr<Drawing::GPUContext> gpuContext = std::make_shared<Drawing::GPUContext>();
    ASSERT_NE(gpuContext, nullptr);
    rsUniRenderUtil.OptimizedFlushAndSubmit(surface, gpuContext.get(), optFenceWait);
    surface = std::make_shared<Drawing::Surface>();
    ASSERT_NE(surface, nullptr);
    rsUniRenderUtil.OptimizedFlushAndSubmit(surface, gpuContext.get(), optFenceWait);
}

/*
 * @tc.name: CreateBufferDrawParam001
 * @tc.desc: test CreateBufferDrawParam when surfaceHandler.buffer is nullptr
 * @tc.type: FUNC
 * @tc.require: issueIAJOWI
 */
HWTEST_F(RSUniRenderUtilTest, CreateBufferDrawParam001, TestSize.Level2)
{
    NodeId id = 1;
    RSSurfaceHandler surfaceHandler(id);
    bool forceCPU = true;
    BufferDrawParam params = RSUniRenderUtil::CreateBufferDrawParam(surfaceHandler, forceCPU);
    ASSERT_EQ(params.buffer, nullptr);
}

/*
 * @tc.name: CreateBufferDrawParam011
 * @tc.desc: test CreateBufferDrawParam with surfaceRenderNode
 * @tc.type: FUNC
 * @tc.require: issueIAJOWI
 */
HWTEST_F(RSUniRenderUtilTest, CreateBufferDrawParam011, TestSize.Level2)
{
    auto surfaceNode = RSTestUtil::CreateSurfaceNode();
    bool forceCPU = true;
    BufferDrawParam params = RSUniRenderUtil::CreateBufferDrawParam(*surfaceNode, forceCPU);
    ASSERT_EQ(params.buffer, nullptr);

    surfaceNode = RSTestUtil::CreateSurfaceNodeWithBuffer();
    params = RSUniRenderUtil::CreateBufferDrawParam(*surfaceNode, forceCPU);
    ASSERT_NE(params.buffer, nullptr);
}

/*
 * @tc.name: DealWithRotationAndGravityForRotationFixed
 * @tc.desc: test DealWithRotationAndGravityForRotationFixed
 * @tc.type: FUNC
 * @tc.require: issueIAJOWI
 */
HWTEST_F(RSUniRenderUtilTest, DealWithRotationAndGravityForRotationFixedTest, TestSize.Level2)
{
    GraphicTransformType transform = GraphicTransformType::GRAPHIC_ROTATE_NONE;
    Gravity gravity = Gravity::CENTER;
    RectF localBounds;
    BufferDrawParam params;
    RSUniRenderUtil::DealWithRotationAndGravityForRotationFixed(transform, gravity, localBounds, params);
    ASSERT_EQ(params.dstRect.GetWidth(), 0);
}

/*
 * @tc.name: GetMatrixOfBufferToRelRect_003
 * @tc.desc: test GetMatrixOfBufferToRelRect with surfaceNode without consumer
 * @tc.type: FUNC
 * @tc.require: issueIAJBBO
 */
HWTEST_F(RSUniRenderUtilTest, GetMatrixOfBufferToRelRect_003, TestSize.Level2)
{
    auto rsSurfaceRenderNode = RSTestUtil::CreateSurfaceNodeWithBuffer();
    ASSERT_NE(rsSurfaceRenderNode, nullptr);
    RSSurfaceRenderNode& node = static_cast<RSSurfaceRenderNode&>(*(rsSurfaceRenderNode.get()));
    node.GetRSSurfaceHandler()->consumer_ = nullptr;
    auto matrix = RSUniRenderUtil::GetMatrixOfBufferToRelRect(node);
    ASSERT_EQ(matrix, Drawing::Matrix());
}

/*
 * @tc.name: CreateBufferDrawParam002
 * @tc.desc: CreateBufferDrawParam test with RSDisplayRenderNode when buffer is not nullptr
 * @tc.type: FUNC
 * @tc.require: issueIAKDJI
 */
HWTEST_F(RSUniRenderUtilTest, CreateBufferDrawParam002, TestSize.Level2)
{
    RSUniRenderUtil rsUniRenderUtil;
    NodeId id = 1;
    RSDisplayNodeConfig config;
    auto rsDisplayRenderNode = std::make_shared<RSDisplayRenderNode>(id++, config);
    ASSERT_NE(rsDisplayRenderNode, nullptr);
    auto node = std::make_shared<RSRenderNode>(id++);
    auto rsDisplayRenderNodeDrawable = std::make_shared<DrawableV2::RSDisplayRenderNodeDrawable>(node);
    rsDisplayRenderNodeDrawable->surfaceHandler_ = std::make_shared<RSSurfaceHandler>(node->id_);
    rsDisplayRenderNode->renderDrawable_ = rsDisplayRenderNodeDrawable;
    auto surfaceNode = RSTestUtil::CreateSurfaceNodeWithBuffer();
    auto buffer = surfaceNode->surfaceHandler_->GetBuffer();
    ASSERT_NE(buffer, nullptr);
    buffer->SetSurfaceBufferWidth(400);
    buffer->SetSurfaceBufferHeight(400);
    rsDisplayRenderNodeDrawable->surfaceHandler_->buffer_.buffer = buffer;
    auto cpuParam = rsUniRenderUtil.CreateBufferDrawParam(*rsDisplayRenderNode, true);
    auto nocpuParam = rsUniRenderUtil.CreateBufferDrawParam(*rsDisplayRenderNode, false);
    ASSERT_NE(cpuParam.useCPU, nocpuParam.useCPU);
    ASSERT_EQ(cpuParam.buffer, nocpuParam.buffer);
    ASSERT_EQ(cpuParam.acquireFence, nocpuParam.acquireFence);
    ASSERT_EQ(cpuParam.srcRect, nocpuParam.srcRect);
    ASSERT_EQ(cpuParam.dstRect, nocpuParam.dstRect);
}

/*
 * @tc.name: CreateBufferDrawParam003
 * @tc.desc: CreateBufferDrawParam test with RSDisplayRenderNode when drawable is nullptr
 * @tc.type: FUNC
 * @tc.require: issueIAKDJI
 */
HWTEST_F(RSUniRenderUtilTest, CreateBufferDrawParam003, TestSize.Level2)
{
    RSUniRenderUtil rsUniRenderUtil;
    NodeId id = 1;
    RSDisplayNodeConfig config;
    auto rsDisplayRenderNode = std::make_shared<RSDisplayRenderNode>(id++, config);
    auto cpuParam = rsUniRenderUtil.CreateBufferDrawParam(*rsDisplayRenderNode, true);
    auto nocpuParam = rsUniRenderUtil.CreateBufferDrawParam(*rsDisplayRenderNode, false);
    ASSERT_NE(cpuParam.useCPU, nocpuParam.useCPU);
    ASSERT_EQ(cpuParam.buffer, nullptr);
    ASSERT_EQ(nocpuParam.buffer, nullptr);
}

/*
 * @tc.name: CreateBufferDrawParam004
 * @tc.desc: CreateBufferDrawParam test with RSDisplayRenderNode when buffer is nullptr
 * @tc.type: FUNC
 * @tc.require: issueIAKDJI
 */
HWTEST_F(RSUniRenderUtilTest, CreateBufferDrawParam004, TestSize.Level2)
{
    RSUniRenderUtil rsUniRenderUtil;
    NodeId id = 1;
    RSDisplayNodeConfig config;
    auto rsDisplayRenderNode = std::make_shared<RSDisplayRenderNode>(id++, config);
    auto node = std::make_shared<RSRenderNode>(id++);
    auto rsDisplayRenderNodeDrawable = std::make_shared<DrawableV2::RSDisplayRenderNodeDrawable>(node);
    rsDisplayRenderNodeDrawable->surfaceHandler_ = std::make_shared<RSSurfaceHandler>(node->id_);
    rsDisplayRenderNode->renderDrawable_ = rsDisplayRenderNodeDrawable;
    auto cpuParam = rsUniRenderUtil.CreateBufferDrawParam(*rsDisplayRenderNode, true);
    auto nocpuParam = rsUniRenderUtil.CreateBufferDrawParam(*rsDisplayRenderNode, false);
    ASSERT_NE(cpuParam.useCPU, nocpuParam.useCPU);
    ASSERT_EQ(cpuParam.buffer, nullptr);
    ASSERT_EQ(nocpuParam.buffer, nullptr);
}

/*
 * @tc.name: CreateBufferDrawParam005
 * @tc.desc: CreateBufferDrawParam test with RSSurfaceRenderNodeDrawable when param is nullptr
 * @tc.type: FUNC
 * @tc.require: issueIAKDJI
 */
HWTEST_F(RSUniRenderUtilTest, CreateBufferDrawParam005, TestSize.Level2)
{
    RSUniRenderUtil rsUniRenderUtil;
    auto node = RSTestUtil::CreateSurfaceNode();
    auto drawable = std::make_shared<DrawableV2::RSSurfaceRenderNodeDrawable>(node);
    auto cpuParam = rsUniRenderUtil.CreateBufferDrawParam(*drawable, true, 1);
    auto nocpuParam = rsUniRenderUtil.CreateBufferDrawParam(*drawable, false, 1);
    ASSERT_EQ(cpuParam.buffer, nullptr);
    ASSERT_EQ(nocpuParam.buffer, nullptr);
}

/*
 * @tc.name: CreateBufferDrawParam006
 * @tc.desc: CreateBufferDrawParam test with RSSurfaceRenderNodeDrawable when buffer is nullptr
 * @tc.type: FUNC
 * @tc.require: issueIAKDJI
 */
HWTEST_F(RSUniRenderUtilTest, CreateBufferDrawParam006, TestSize.Level2)
{
    RSUniRenderUtil rsUniRenderUtil;
    auto node = RSTestUtil::CreateSurfaceNode();
    auto drawable = std::make_shared<DrawableV2::RSSurfaceRenderNodeDrawable>(node);
    drawable->renderParams_ = std::make_unique<RSSurfaceRenderParams>(node->id_);
    auto cpuParam = rsUniRenderUtil.CreateBufferDrawParam(*drawable, true, 1);
    auto nocpuParam = rsUniRenderUtil.CreateBufferDrawParam(*drawable, false, 1);
    ASSERT_NE(cpuParam.useCPU, nocpuParam.useCPU);
    ASSERT_EQ(cpuParam.useBilinearInterpolation, nocpuParam.useBilinearInterpolation);
    ASSERT_EQ(cpuParam.threadIndex, nocpuParam.threadIndex);
    ASSERT_EQ(cpuParam.buffer, nullptr);
    ASSERT_EQ(nocpuParam.buffer, nullptr);
}

/*
 * @tc.name: CreateBufferDrawParam007
 * @tc.desc: CreateBufferDrawParam test with RSSurfaceRenderNodeDrawable when consumer is nullptr
 * @tc.type: FUNC
 * @tc.require: issueIAKDJI
 */
HWTEST_F(RSUniRenderUtilTest, CreateBufferDrawParam007, TestSize.Level2)
{
    RSUniRenderUtil rsUniRenderUtil;
    auto surfaceNode = RSTestUtil::CreateSurfaceNodeWithBuffer();
    auto node = RSTestUtil::CreateSurfaceNode();
    auto drawable = std::make_shared<DrawableV2::RSSurfaceRenderNodeDrawable>(node);
    auto param = std::make_unique<RSSurfaceRenderParams>(node->id_);
    param->buffer_ = surfaceNode->surfaceHandler_->GetBuffer();
    drawable->renderParams_ = std::move(param);
    auto cpuParam = rsUniRenderUtil.CreateBufferDrawParam(*drawable, true, 1);
    auto nocpuParam = rsUniRenderUtil.CreateBufferDrawParam(*drawable, false, 1);
    ASSERT_NE(cpuParam.useCPU, nocpuParam.useCPU);
    ASSERT_EQ(cpuParam.useBilinearInterpolation, nocpuParam.useBilinearInterpolation);
    ASSERT_EQ(cpuParam.threadIndex, nocpuParam.threadIndex);
    ASSERT_EQ(cpuParam.buffer, nocpuParam.buffer);
    ASSERT_EQ(cpuParam.acquireFence, nocpuParam.acquireFence);
    ASSERT_EQ(cpuParam.srcRect, nocpuParam.srcRect);
}

/*
 * @tc.name: CreateBufferDrawParam008
 * @tc.desc: CreateBufferDrawParam test with RSSurfaceRenderNodeDrawable when consumer is nullptr
 * @tc.type: FUNC
 * @tc.require: issueIAKDJI
 */
HWTEST_F(RSUniRenderUtilTest, CreateBufferDrawParam008, TestSize.Level2)
{
    RSUniRenderUtil rsUniRenderUtil;
    auto surfaceNode = RSTestUtil::CreateSurfaceNodeWithBuffer();
    auto node = RSTestUtil::CreateSurfaceNode();
    auto drawable = std::make_shared<DrawableV2::RSSurfaceRenderNodeDrawable>(node);
    auto param = std::make_unique<RSSurfaceRenderParams>(node->id_);
    param->buffer_ = surfaceNode->surfaceHandler_->GetBuffer();
    drawable->consumerOnDraw_ = surfaceNode->surfaceHandler_->GetConsumer();
    drawable->renderParams_ = std::move(param);
    auto cpuParam = rsUniRenderUtil.CreateBufferDrawParam(*drawable, true, 1);
    auto nocpuParam = rsUniRenderUtil.CreateBufferDrawParam(*drawable, false, 1);
    ASSERT_NE(cpuParam.useCPU, nocpuParam.useCPU);
    ASSERT_EQ(cpuParam.useBilinearInterpolation, nocpuParam.useBilinearInterpolation);
    ASSERT_EQ(cpuParam.threadIndex, nocpuParam.threadIndex);
    ASSERT_EQ(cpuParam.buffer, nocpuParam.buffer);
    ASSERT_EQ(cpuParam.acquireFence, nocpuParam.acquireFence);
    ASSERT_EQ(cpuParam.srcRect, nocpuParam.srcRect);
}

/*
 * @tc.name: CreateBufferDrawParam009
 * @tc.desc: CreateBufferDrawParam test with RSSurfaceRenderNodeDrawable when consumer is nullptr
 * @tc.type: FUNC
 * @tc.require: issueIAKDJI
 */
HWTEST_F(RSUniRenderUtilTest, CreateBufferDrawParam009, TestSize.Level2)
{
    RSUniRenderUtil rsUniRenderUtil;
    auto surfaceNode = RSTestUtil::CreateSurfaceNodeWithBuffer();
    auto node = RSTestUtil::CreateSurfaceNode();
    auto drawable = std::make_shared<DrawableV2::RSSurfaceRenderNodeDrawable>(node);
    auto param = std::make_unique<RSSurfaceRenderParams>(node->id_);
    param->buffer_ = surfaceNode->surfaceHandler_->GetBuffer();
    drawable->consumerOnDraw_ = surfaceNode->surfaceHandler_->GetConsumer();
    drawable->renderParams_ = std::move(param);
    auto cpuParam = rsUniRenderUtil.CreateBufferDrawParam(*drawable, true, 1);
    auto nocpuParam = rsUniRenderUtil.CreateBufferDrawParam(*drawable, false, 1);
    ASSERT_NE(cpuParam.useCPU, nocpuParam.useCPU);
    ASSERT_EQ(cpuParam.useBilinearInterpolation, nocpuParam.useBilinearInterpolation);
    ASSERT_EQ(cpuParam.threadIndex, nocpuParam.threadIndex);
    ASSERT_EQ(cpuParam.buffer, nocpuParam.buffer);
    ASSERT_EQ(cpuParam.acquireFence, nocpuParam.acquireFence);
    ASSERT_EQ(cpuParam.srcRect, nocpuParam.srcRect);
}

/*
 * @tc.name: CreateBufferDrawParam010
 * @tc.desc: Test CreateBufferDrawParam with crop metadata handling
 * @tc.type: FUNC
 * @tc.require: issueIAKDJI
 */
HWTEST_F(RSUniRenderUtilTest, CreateBufferDrawParam010, TestSize.Level2)
{
    // Basic setup
    RSUniRenderUtil rsUniRenderUtil;
    NodeId id = 1;
    RSDisplayNodeConfig config;
    auto rsDisplayRenderNode = std::make_shared<RSDisplayRenderNode>(id++, config);
    ASSERT_NE(rsDisplayRenderNode, nullptr);
    auto node = std::make_shared<RSRenderNode>(id++);
    auto rsDisplayRenderNodeDrawable = std::make_shared<DrawableV2::RSDisplayRenderNodeDrawable>(node);
    rsDisplayRenderNodeDrawable->surfaceHandler_ = std::make_shared<RSSurfaceHandler>(node->id_);
    rsDisplayRenderNode->renderDrawable_ = rsDisplayRenderNodeDrawable;

    // Create surface node with buffer
    auto surfaceNode = RSTestUtil::CreateSurfaceNodeWithBuffer();
    auto buffer = surfaceNode->surfaceHandler_->GetBuffer();
    ASSERT_NE(buffer, nullptr);
    buffer->SetSurfaceBufferWidth(1920);
    buffer->SetSurfaceBufferHeight(1080);
    rsDisplayRenderNodeDrawable->surfaceHandler_->buffer_.buffer = buffer;

    // Test case: Verify crop metadata handling and final dstRect
    {
        auto mockHelper = Mock::MockMetadataHelper::GetInstance();
        ASSERT_NE(mockHelper, nullptr);

        HDI::Display::Graphic::Common::V1_0::BufferHandleMetaRegion cropRegion = {
            .left = 100,
            .top = 200,
            .width = 800,
            .height = 600
        };

        EXPECT_CALL(*mockHelper, GetCropRectMetadata(_, _))
            .WillRepeatedly(DoAll(
                SetArgReferee<1>(cropRegion),
                Return(GSERROR_OK)
            ));
        HDI::Display::Graphic::Common::V1_0::BufferHandleMetaRegion testRegion;
        auto param = rsUniRenderUtil.CreateBufferDrawParam(*rsDisplayRenderNode, false);
        param.hasCropMetadata = mockHelper->GetCropRectMetadata(buffer, testRegion) == GSERROR_OK;
        ASSERT_EQ(testRegion.left, cropRegion.left);
        ASSERT_TRUE(param.hasCropMetadata);
        if (param.hasCropMetadata) {
            param.srcRect = Drawing::Rect(testRegion.left, testRegion.top,
                testRegion.left + testRegion.width, testRegion.top + testRegion.height);
            param.dstRect = Drawing::Rect(0, 0, buffer->GetSurfaceBufferWidth(), buffer->GetSurfaceBufferHeight());
        }

        // Verify crop metadata and srcRect
        ASSERT_EQ(param.srcRect.GetLeft(), cropRegion.left);
        ASSERT_EQ(param.srcRect.GetTop(), cropRegion.top);
        ASSERT_EQ(param.srcRect.GetWidth(), cropRegion.width);
        ASSERT_EQ(param.srcRect.GetHeight(), cropRegion.height);

        // Verify final dstRect after all internal processing
        ASSERT_EQ(param.dstRect.GetLeft(), 0);
        ASSERT_EQ(param.dstRect.GetTop(), 0);
        ASSERT_EQ(param.dstRect.GetWidth(), buffer->GetSurfaceBufferWidth());
        ASSERT_EQ(param.dstRect.GetHeight(), buffer->GetSurfaceBufferHeight());
    }
}

/*
 * @tc.name: FlushDmaSurfaceBuffer001
 * @tc.desc: test FlushDmaSurfaceBuffer when pixelMap is nullptr
 * @tc.type: FUNC
 * @tc.require: issueIAL5XA
 */
HWTEST_F(RSUniRenderUtilTest, FlushDmaSurfaceBuffer001, TestSize.Level2)
{
    Media::PixelMap* pixelMap = nullptr;
    ASSERT_EQ(pixelMap, nullptr);
    RSUniRenderUtil::FlushDmaSurfaceBuffer(pixelMap);
}

/*
 * @tc.name: FlushDmaSurfaceBuffer002
 * @tc.desc: test FlushDmaSurfaceBuffer when pixelMap is not nullptr
 * @tc.type: FUNC
 * @tc.require: issueIALXTP
 */
HWTEST_F(RSUniRenderUtilTest, FlushDmaSurfaceBuffer002, TestSize.Level2)
{
    Media::InitializationOptions opts;
    const double width = 100;
    const double height = 100;
    opts.size.width = width;
    opts.size.height = height;
    auto pixelMap = Media::PixelMap::Create(opts);
    ASSERT_NE(pixelMap, nullptr);
    RSUniRenderUtil::FlushDmaSurfaceBuffer(pixelMap.get());
#if defined(ROSEN_OHOS) && defined(RS_ENABLE_VK)
    DmaMem dmaMem;
    Drawing::ImageInfo info = Drawing::ImageInfo{ pixelMap->GetWidth(), pixelMap->GetHeight(),
        Drawing::COLORTYPE_RGBA_8888, Drawing::ALPHATYPE_PREMUL };
    sptr<SurfaceBuffer> surFaceBuffer = dmaMem.DmaMemAlloc(info, pixelMap);
    RSUniRenderUtil::FlushDmaSurfaceBuffer(pixelMap.get());
#endif
}

/**
 * @tc.name: CheckRenderSkipIfScreenOff001
 * @tc.desc: Test CheckRenderSkipIfScreenOff, no need for extra frame
 * @tc.type: FUNC
 * @tc.require: #I9UNQP
 */
HWTEST_F(RSUniRenderUtilTest, CheckRenderSkipIfScreenOff001, TestSize.Level1)
{
    if (RSSystemProperties::GetSkipDisplayIfScreenOffEnabled()) {
        ScreenId screenId = 1;
        auto screenManager = CreateOrGetScreenManager();
        OHOS::Rosen::impl::RSScreenManager& screenManagerImpl =
            static_cast<OHOS::Rosen::impl::RSScreenManager&>(*screenManager);
        screenManagerImpl.powerOffNeedProcessOneFrame_ = false;

        screenManagerImpl.screenPowerStatus_[screenId] = ScreenPowerStatus::POWER_STATUS_ON;
        EXPECT_FALSE(RSUniRenderUtil::CheckRenderSkipIfScreenOff(false, screenId));
        screenManagerImpl.screenPowerStatus_[screenId] = ScreenPowerStatus::POWER_STATUS_ON_ADVANCED;
        EXPECT_FALSE(RSUniRenderUtil::CheckRenderSkipIfScreenOff(false, screenId));
        screenManagerImpl.screenPowerStatus_[screenId] = ScreenPowerStatus::POWER_STATUS_SUSPEND;
        EXPECT_TRUE(RSUniRenderUtil::CheckRenderSkipIfScreenOff(false, screenId));
        screenManagerImpl.screenPowerStatus_[screenId] = ScreenPowerStatus::POWER_STATUS_OFF;
        EXPECT_TRUE(RSUniRenderUtil::CheckRenderSkipIfScreenOff(false, screenId));
    }
}

/**
 * @tc.name: CheckRenderSkipIfScreenOff002
 * @tc.desc: Test CheckRenderSkipIfScreenOff, need extra frame
 * @tc.type: FUNC
 * @tc.require: #I9UNQP
 */
HWTEST_F(RSUniRenderUtilTest, CheckRenderSkipIfScreenOff002, TestSize.Level1)
{
    if (RSSystemProperties::GetSkipDisplayIfScreenOffEnabled()) {
        ScreenId screenId = 1;
        auto screenManager = CreateOrGetScreenManager();
        OHOS::Rosen::impl::RSScreenManager& screenManagerImpl =
            static_cast<OHOS::Rosen::impl::RSScreenManager&>(*screenManager);

        screenManagerImpl.powerOffNeedProcessOneFrame_ = true;
        screenManagerImpl.screenPowerStatus_[screenId] = ScreenPowerStatus::POWER_STATUS_ON;
        EXPECT_FALSE(RSUniRenderUtil::CheckRenderSkipIfScreenOff(false, screenId));
        screenManagerImpl.powerOffNeedProcessOneFrame_ = true;
        screenManagerImpl.screenPowerStatus_[screenId] = ScreenPowerStatus::POWER_STATUS_ON_ADVANCED;
        EXPECT_FALSE(RSUniRenderUtil::CheckRenderSkipIfScreenOff(false, screenId));
        screenManagerImpl.powerOffNeedProcessOneFrame_ = true;
        screenManagerImpl.screenPowerStatus_[screenId] = ScreenPowerStatus::POWER_STATUS_SUSPEND;
        EXPECT_FALSE(RSUniRenderUtil::CheckRenderSkipIfScreenOff(false, screenId));
        screenManagerImpl.powerOffNeedProcessOneFrame_ = true;
        screenManagerImpl.screenPowerStatus_[screenId] = ScreenPowerStatus::POWER_STATUS_OFF;
        EXPECT_FALSE(RSUniRenderUtil::CheckRenderSkipIfScreenOff(false, screenId));
    }
}

/**
 * @tc.name: RequestPerf
 * @tc.desc: Test RequestPerf
 * @tc.type: FUNC
 * @tc.require: #IB2XN0
 */
HWTEST_F(RSUniRenderUtilTest, RequestPerf, TestSize.Level1)
{
    uint32_t layerLevel[] = { 0, 1, 2, 3 };
    bool onOffTag = true;
    int count = 0;
    int total = 4;
    for (uint32_t level : layerLevel) {
        RSUniRenderUtil::RequestPerf(level, onOffTag);
        count++;
    }
    EXPECT_TRUE(count == total);
}

/**
 * @tc.name: MultiLayersPerf
 * @tc.desc: test results of MultiLayersPerf
 * @tc.type: FUNC
 * @tc.require: #IB2XN0
 */
HWTEST_F(RSUniRenderUtilTest, MultiLayersPerf, TestSize.Level1)
{
    uint32_t layerNum[] = {3, 20, 1, 0};
    int total = 4;
    int num = 0;
    for (auto i : layerNum) {
        RSUniRenderUtil::MultiLayersPerf(i);
        num++;
    }
    EXPECT_TRUE(num == total);
}

/**
 * @tc.name: GetSampledDamageAndDrawnRegion001
 * @tc.desc: test GetSampledDamageAndDrawnRegion without rounding operation
 * @tc.type: FUNC
 * @tc.require: #IBRXJN
 */
HWTEST_F(RSUniRenderUtilTest, GetSampledDamageAndDrawnRegion001, TestSize.Level1)
{
    constexpr int srcDamageRegionWidth{100};
    constexpr int srcDamageRegionHeight{200};
    constexpr int sampledDamageRegionWidth{50};
    constexpr int sampledDamageRegionHeight{100};
    constexpr int sampledDrawnRegionWidth{100};
    constexpr int sampledDrawnRegionHeight{200};

    ScreenInfo screenInfo{
        .isSamplingOn = true,
        .samplingDistance = 0,
        .samplingTranslateX = 0.f,
        .samplingTranslateY = 0.f,
        .samplingScale = 0.5f
    };
    Occlusion::Region srcDamageRegion{Occlusion::Rect(0, 0, srcDamageRegionWidth, srcDamageRegionHeight)};
    Occlusion::Region sampledDamageRegion;
    Occlusion::Region sampledDrawnRegion;
    RSUniRenderUtil::GetSampledDamageAndDrawnRegion(screenInfo, srcDamageRegion, false,
        sampledDamageRegion, sampledDrawnRegion);
    EXPECT_EQ(sampledDamageRegion.GetBound().GetWidth(), sampledDamageRegionWidth);
    EXPECT_EQ(sampledDamageRegion.GetBound().GetHeight(), sampledDamageRegionHeight);
    EXPECT_EQ(sampledDrawnRegion.GetBound().GetWidth(), sampledDrawnRegionWidth);
    EXPECT_EQ(sampledDrawnRegion.GetBound().GetHeight(), sampledDrawnRegionHeight);
}

/**
 * @tc.name: GetSampledDamageAndDrawnRegion002
 * @tc.desc: test GetSampledDamageAndDrawnRegion with rounding operation for decimal numbers equal to 0.5
 * @tc.type: FUNC
 * @tc.require: #IBRXJN
 */
HWTEST_F(RSUniRenderUtilTest, GetSampledDamageAndDrawnRegion002, TestSize.Level1)
{
    constexpr int srcDamageRegionWidth{101};
    constexpr int srcDamageRegionHeight{203};
    constexpr int sampledDamageRegionWidth{57};
    constexpr int sampledDamageRegionHeight{108};
    constexpr int sampledDrawnRegionWidth{114};
    constexpr int sampledDrawnRegionHeight{216};

    ScreenInfo screenInfo{
        .isSamplingOn = true,
        .samplingDistance = 3,
        .samplingTranslateX = 0.f,
        .samplingTranslateY = 0.f,
        .samplingScale = 0.5f
    };
    Occlusion::Region srcDamageRegion{Occlusion::Rect(0, 0, srcDamageRegionWidth, srcDamageRegionHeight)};
    Occlusion::Region sampledDamageRegion;
    Occlusion::Region sampledDrawnRegion;
    RSUniRenderUtil::GetSampledDamageAndDrawnRegion(screenInfo, srcDamageRegion, false,
        sampledDamageRegion, sampledDrawnRegion);
    EXPECT_EQ(sampledDamageRegion.GetBound().GetWidth(), sampledDamageRegionWidth);
    EXPECT_EQ(sampledDamageRegion.GetBound().GetHeight(), sampledDamageRegionHeight);
    EXPECT_EQ(sampledDrawnRegion.GetBound().GetWidth(), sampledDrawnRegionWidth);
    EXPECT_EQ(sampledDrawnRegion.GetBound().GetHeight(), sampledDrawnRegionHeight);
}

/**
 * @tc.name: GetSampledDamageAndDrawnRegion003
 * @tc.desc: test GetSampledDamageAndDrawnRegion with rounding operation for decimal numbers less than 0.5
 * @tc.type: FUNC
 * @tc.require: #IBRXJN
 */
HWTEST_F(RSUniRenderUtilTest, GetSampledDamageAndDrawnRegion003, TestSize.Level1)
{
    constexpr int srcDamageRegionWidth{101};
    constexpr int srcDamageRegionHeight{203};
    constexpr int sampledDamageRegionWidth{13};
    constexpr int sampledDamageRegionHeight{26};
    constexpr int sampledDrawnRegionWidth{104};
    constexpr int sampledDrawnRegionHeight{208};

    ScreenInfo screenInfo{
        .isSamplingOn = true,
        .samplingDistance = 0,
        .samplingTranslateX = 10.f,
        .samplingTranslateY = 11.f,
        .samplingScale = 0.125f
    };
    Occlusion::Region srcDamageRegion{Occlusion::Rect(0, 0, srcDamageRegionWidth, srcDamageRegionHeight)};
    Occlusion::Region sampledDamageRegion;
    Occlusion::Region sampledDrawnRegion;
    RSUniRenderUtil::GetSampledDamageAndDrawnRegion(screenInfo, srcDamageRegion, false,
        sampledDamageRegion, sampledDrawnRegion);
    EXPECT_EQ(sampledDamageRegion.GetBound().GetWidth(), sampledDamageRegionWidth);
    EXPECT_EQ(sampledDamageRegion.GetBound().GetHeight(), sampledDamageRegionHeight);
    EXPECT_EQ(sampledDrawnRegion.GetBound().GetWidth(), sampledDrawnRegionWidth);
    EXPECT_EQ(sampledDrawnRegion.GetBound().GetHeight(), sampledDrawnRegionHeight);
}

/**
 * @tc.name: GetSampledDamageAndDrawnRegion004
 * @tc.desc: test GetSampledDamageAndDrawnRegion with alignment operations
 * @tc.type: FUNC
 * @tc.require: #IBRXJN
 */
HWTEST_F(RSUniRenderUtilTest, GetSampledDamageAndDrawnRegion004, TestSize.Level1)
{
    constexpr int srcDamageRegionWidth{1001};
    constexpr int srcDamageRegionHeight{2003};
    constexpr int sampledDamageRegionWidth{132};
    constexpr int sampledDamageRegionHeight{257};
    constexpr int sampledDrawnRegionWidth{2048};
    constexpr int sampledDrawnRegionHeight{3072};

    ScreenInfo screenInfo{
        .isSamplingOn = true,
        .samplingDistance = 3,
        .samplingTranslateX = 10.f,
        .samplingTranslateY = 11.f,
        .samplingScale = 0.125f
    };
    Occlusion::Region srcDamageRegion{Occlusion::Rect(0, 0, srcDamageRegionWidth, srcDamageRegionHeight)};
    Occlusion::Region sampledDamageRegion;
    Occlusion::Region sampledDrawnRegion;
    RSUniRenderUtil::GetSampledDamageAndDrawnRegion(screenInfo, srcDamageRegion, true,
        sampledDamageRegion, sampledDrawnRegion);
    EXPECT_EQ(sampledDamageRegion.GetBound().GetWidth(), sampledDamageRegionWidth);
    EXPECT_EQ(sampledDamageRegion.GetBound().GetHeight(), sampledDamageRegionHeight);
    EXPECT_EQ(sampledDrawnRegion.GetBound().GetWidth(), sampledDrawnRegionWidth);
    EXPECT_EQ(sampledDrawnRegion.GetBound().GetHeight(), sampledDrawnRegionHeight);
}

/**
 * @tc.name: MergeDirtyHistoryForDrawable001
 * @tc.desc: test results of MergeDirtyHistoryForDrawable if surface drawable invalid
 * @tc.type: FUNC
 * @tc.require: #IBDJVI
 */
HWTEST_F(RSUniRenderUtilTest, MergeDirtyHistoryForDrawable001, TestSize.Level1)
{
    NodeId defaultDisplayId = 5;
    RSDisplayNodeConfig config;
    RSDisplayRenderNodeDrawable* displayDrawable = GenerateDisplayDrawableById(defaultDisplayId, config);
    ASSERT_NE(displayDrawable, nullptr);
    int32_t bufferAge = 0;
    std::unique_ptr<RSDisplayRenderParams> params = std::make_unique<RSDisplayRenderParams>(defaultDisplayId);
    params->isFirstVisitCrossNodeDisplay_ = false;
    bool aligned = false;
    std::vector<std::shared_ptr<RSRenderNodeDrawableAdapter>> surfaceAdapters{nullptr};

    NodeId defaultSurfaceId = 10;
    std::shared_ptr<RSSurfaceRenderNode> renderNode = std::make_shared<RSSurfaceRenderNode>(defaultSurfaceId);
    auto surfaceAdapter = RSSurfaceRenderNodeDrawable::OnGenerate(renderNode);
    // default surface
    surfaceAdapters.emplace_back(surfaceAdapter);
    
    params->SetAllMainAndLeashSurfaceDrawables(surfaceAdapters);
    RSUniRenderUtil::MergeDirtyHistoryForDrawable(*displayDrawable, bufferAge, *params, aligned);
    displayDrawable = nullptr;
}

/**
 * @tc.name: MergeDirtyHistoryForDrawable002
 * @tc.desc: test results of MergeDirtyHistoryForDrawable if app drawable is first-cross node or not
 * @tc.type: FUNC
 * @tc.require: #IBDJVI
 */
HWTEST_F(RSUniRenderUtilTest, MergeDirtyHistoryForDrawable002, TestSize.Level1)
{
    NodeId defaultDisplayId = 5;
    RSDisplayNodeConfig config;
    RSDisplayRenderNodeDrawable* displayDrawable = GenerateDisplayDrawableById(defaultDisplayId, config);
    ASSERT_NE(displayDrawable, nullptr);
    int32_t bufferAge = 0;
    std::unique_ptr<RSDisplayRenderParams> params = std::make_unique<RSDisplayRenderParams>(defaultDisplayId);
    params->isFirstVisitCrossNodeDisplay_ = false;
    bool aligned = false;
    std::vector<std::shared_ptr<RSRenderNodeDrawableAdapter>> surfaceAdapters;

    NodeId defaultSurfaceId = 10;
    // normal appwindow
    std::shared_ptr<RSSurfaceRenderNode> renderNode = std::make_shared<RSSurfaceRenderNode>(defaultSurfaceId++);
    renderNode->nodeType_ = RSSurfaceNodeType::APP_WINDOW_NODE;
    auto surfaceAdapter = RSSurfaceRenderNodeDrawable::OnGenerate(renderNode);
    surfaceAdapters.emplace_back(surfaceAdapter);

    // cross appwindow but not first visited
    renderNode = std::make_shared<RSSurfaceRenderNode>(defaultSurfaceId);
    renderNode->nodeType_ = RSSurfaceNodeType::APP_WINDOW_NODE;
    surfaceAdapter = RSSurfaceRenderNodeDrawable::OnGenerate(renderNode);
    ASSERT_NE(surfaceAdapter, nullptr);
    surfaceAdapter->renderParams_ = std::make_unique<RSSurfaceRenderParams>(defaultSurfaceId);
    surfaceAdapter->renderParams_->isFirstLevelCrossNode_ = true;
    surfaceAdapters.emplace_back(surfaceAdapter);

    params->SetAllMainAndLeashSurfaceDrawables(surfaceAdapters);
    RSUniRenderUtil::MergeDirtyHistoryForDrawable(*displayDrawable, bufferAge, *params, aligned);
    displayDrawable = nullptr;
}

/**
 * @tc.name: MergeDirtyHistoryForDrawable003
 * @tc.desc: test results of MergeDirtyHistoryForDrawable if app drawable first visited within first cross display
 * @tc.type: FUNC
 * @tc.require: #IBDJVI
 */
HWTEST_F(RSUniRenderUtilTest, MergeDirtyHistoryForDrawable003, TestSize.Level1)
{
    NodeId defaultDisplayId = 5;
    RSDisplayNodeConfig config;
    RSDisplayRenderNodeDrawable* displayDrawable = GenerateDisplayDrawableById(defaultDisplayId, config);
    ASSERT_NE(displayDrawable, nullptr);
    int32_t bufferAge = 0;
    std::unique_ptr<RSDisplayRenderParams> params = std::make_unique<RSDisplayRenderParams>(defaultDisplayId);
    params->isFirstVisitCrossNodeDisplay_ = true;
    bool aligned = false;
    std::vector<std::shared_ptr<RSRenderNodeDrawableAdapter>> surfaceAdapters;

    NodeId defaultSurfaceId = 10;
    // cross appwindow
    auto renderNode = std::make_shared<RSSurfaceRenderNode>(defaultSurfaceId);
    renderNode->nodeType_ = RSSurfaceNodeType::APP_WINDOW_NODE;
    auto surfaceAdapter = RSSurfaceRenderNodeDrawable::OnGenerate(renderNode);
    ASSERT_NE(surfaceAdapter, nullptr);
    surfaceAdapter->renderParams_ = std::make_unique<RSSurfaceRenderParams>(defaultSurfaceId);
    surfaceAdapter->renderParams_->isFirstLevelCrossNode_ = true;
    surfaceAdapters.emplace_back(surfaceAdapter);
    
    params->SetAllMainAndLeashSurfaceDrawables(surfaceAdapters);
    RSUniRenderUtil::MergeDirtyHistoryForDrawable(*displayDrawable, bufferAge, *params, aligned);
    displayDrawable = nullptr;
}

/**
 * @tc.name: MergeDirtyHistory001
 * @tc.desc: test MergeDirtyHistory
 * @tc.type: FUNC
 * @tc.require: #IBIOQ4
 */
HWTEST_F(RSUniRenderUtilTest, MergeDirtyHistory001, TestSize.Level1)
{
    NodeId defaultDisplayId = 5;
    RSDisplayNodeConfig config;
    RSDisplayRenderNodeDrawable* displayDrawable = GenerateDisplayDrawableById(defaultDisplayId, config);
    ASSERT_NE(displayDrawable, nullptr);
    int32_t bufferAge = 0;
    std::unique_ptr<RSDisplayRenderParams> params = std::make_unique<RSDisplayRenderParams>(defaultDisplayId);
    params->isFirstVisitCrossNodeDisplay_ = false;
    std::vector<std::shared_ptr<RSRenderNodeDrawableAdapter>> surfaceAdapters{nullptr};

    NodeId defaultSurfaceId = 10;
    std::shared_ptr<RSSurfaceRenderNode> renderNode = std::make_shared<RSSurfaceRenderNode>(defaultSurfaceId);
    auto surfaceAdapter = RSSurfaceRenderNodeDrawable::OnGenerate(renderNode);
    // default surface
    surfaceAdapters.emplace_back(surfaceAdapter);
    
    params->SetAllMainAndLeashSurfaceDrawables(surfaceAdapters);
    ScreenInfo screenInfo;
    RSDirtyRectsDfx rsDirtyRectsDfx(*displayDrawable);
    auto renderParams = std::make_unique<RSRenderThreadParams>();
    RSUniRenderThread::Instance().Sync(std::move(renderParams));
    auto rects = RSUniRenderUtil::MergeDirtyHistory(*displayDrawable, bufferAge, screenInfo, rsDirtyRectsDfx, *params);
    EXPECT_EQ(rects.empty(), false);
    displayDrawable = nullptr;
}

/**
 * @tc.name: MergeDirtyHistoryInVirtual001
 * @tc.desc: test MergeDirtyHistoryInVirtual
 * @tc.type: FUNC
 * @tc.require: #IBIOQ4
 */
HWTEST_F(RSUniRenderUtilTest, MergeDirtyHistoryInVirtual001, TestSize.Level1)
{
    NodeId defaultDisplayId = 5;
    RSDisplayNodeConfig config;
    RSDisplayRenderNodeDrawable* displayDrawable = GenerateDisplayDrawableById(defaultDisplayId, config);
    ASSERT_NE(displayDrawable, nullptr);
    int32_t bufferAge = 0;
    std::unique_ptr<RSDisplayRenderParams> params = std::make_unique<RSDisplayRenderParams>(defaultDisplayId);
    params->isFirstVisitCrossNodeDisplay_ = false;
    std::vector<std::shared_ptr<RSRenderNodeDrawableAdapter>> surfaceAdapters{nullptr};

    NodeId defaultSurfaceId = 10;
    std::shared_ptr<RSSurfaceRenderNode> renderNode = std::make_shared<RSSurfaceRenderNode>(defaultSurfaceId);
    auto surfaceAdapter = RSSurfaceRenderNodeDrawable::OnGenerate(renderNode);
    // default surface
    surfaceAdapters.emplace_back(surfaceAdapter);
    
    params->SetAllMainAndLeashSurfaceDrawables(surfaceAdapters);
    ScreenInfo screenInfo;
    auto rects = RSUniRenderUtil::MergeDirtyHistoryInVirtual(*displayDrawable, bufferAge, screenInfo);
    EXPECT_EQ(rects.empty(), true);
    displayDrawable = nullptr;
}

/**
 * @tc.name: FrameAwareTraceBoostTest
 * @tc.desc: test FrameAwareTraceBoost
 * @tc.type: FUNC
 * @tc.require: #IBIA3V
 */
HWTEST_F(RSUniRenderUtilTest, FrameAwareTraceBoostTest, TestSize.Level1)
{
    size_t layerNum = 10;
    auto res = RSUniRenderUtil::FrameAwareTraceBoost(layerNum);
    ASSERT_EQ(res, false);

    layerNum = 11;
    res = RSUniRenderUtil::FrameAwareTraceBoost(layerNum);
    ASSERT_EQ(res, false);
}

/**
 * @tc.name: GetImageRegionsTest
 * @tc.desc: test GetImageRegions
 * @tc.type: FUNC
 * @tc.require: #IBIA3V
 */
HWTEST_F(RSUniRenderUtilTest, GetImageRegionsTest, TestSize.Level1)
{
    float screenWidth = 100.0f;
    float screenHeight = 100.0f;
    float realImageWidth = 0.0f;
    float realImageHeight = 0.0f;
    auto regions = RSUniRenderUtil::GetImageRegions(screenHeight, screenWidth, realImageHeight, realImageWidth);
    ASSERT_EQ(regions.GetHeight(), 100);

    realImageWidth = 10.0f;
    realImageHeight = 10.0f;
    regions = RSUniRenderUtil::GetImageRegions(screenHeight, screenWidth, realImageHeight, realImageWidth);
    ASSERT_EQ(regions.GetHeight(), 100);
}

/**
 * @tc.name: SetSrcRectForAnco
 * @tc.desc: SetSrcRectForAnco test for anco node
 * @tc.type: FUNC
 * @tc.require: issueICA0I8
 */
HWTEST_F(RSUniRenderUtilTest, SetSrcRectForAnco, TestSize.Level2)
{
    BufferDrawParam params;
    auto surfaceNode = RSTestUtil::CreateSurfaceNodeWithBuffer();
    ASSERT_NE(surfaceNode, nullptr);
    ASSERT_NE(surfaceNode->surfaceHandler_, nullptr);
    auto buffer = surfaceNode->surfaceHandler_->GetBuffer();
    ASSERT_NE(buffer, nullptr);
    buffer->SetSurfaceBufferWidth(100);
    buffer->SetSurfaceBufferHeight(100);
    LayerInfoPtr layer = HdiLayerInfo::CreateHdiLayerInfo();
    LayerInfoPtr nullLayer = nullptr;
    layer->SetBuffer(buffer, {});
    auto csurface = IConsumerSurface::Create();
    ASSERT_NE(csurface, nullptr);
    layer->SetSurface(csurface);
    params = RSUniRenderUtil::CreateLayerBufferDrawParam(layer, false);
    RSUniRenderUtil::SetSrcRectForAnco(layer, params);
    RSUniRenderUtil::SetSrcRectForAnco(nullLayer, params);
    layer->SetAncoFlags(static_cast<uint32_t>(AncoFlags::ANCO_SFV_NODE));
    RSUniRenderUtil::SetSrcRectForAnco(layer, params);

    layer->SetCropRect({0, 0, 100, 0});
    RSUniRenderUtil::SetSrcRectForAnco(layer, params);
    ASSERT_TRUE(params.srcRect == Drawing::Rect(0, 0, 100, 100));
    layer->SetCropRect({0, 0, 0, 100});
    RSUniRenderUtil::SetSrcRectForAnco(layer, params);
    ASSERT_TRUE(params.srcRect == Drawing::Rect(0, 0, 100, 100));
    layer->SetCropRect({0, 0, 50, 50});
    RSUniRenderUtil::SetSrcRectForAnco(layer, params);
    ASSERT_TRUE(params.srcRect == Drawing::Rect(0, 0, 50, 50));

    auto drawable = std::make_shared<DrawableV2::RSSurfaceRenderNodeDrawable>(surfaceNode);
    ASSERT_NE(drawable, nullptr);
    ASSERT_EQ(drawable->renderParams_, nullptr);
    drawable->renderParams_ = std::make_unique<RSSurfaceRenderParams>(surfaceNode->GetId() + 1);
    RSSurfaceRenderParams* surfaceParams = nullptr;
    surfaceParams = static_cast<RSSurfaceRenderParams*>(drawable->renderParams_.get());
    ASSERT_NE(surfaceParams, nullptr);
    surfaceParams->buffer_ = buffer;
    drawable->consumerOnDraw_ = surfaceNode->surfaceHandler_->GetConsumer();
    ASSERT_NE(drawable->consumerOnDraw_, nullptr);
    params = RSUniRenderUtil::CreateBufferDrawParam(*drawable, false, 1);
    RSUniRenderUtil::SetSrcRectForAnco(*surfaceParams, params);

    surfaceParams->SetAncoFlags(static_cast<uint32_t>(AncoFlags::ANCO_SFV_NODE));
    RSUniRenderUtil::SetSrcRectForAnco(*surfaceParams, params);
    ASSERT_TRUE(params.srcRect == Drawing::Rect(0, 0, 100, 100));
    surfaceParams->SetAncoSrcCrop({0, 0, 100, 0});
    RSUniRenderUtil::SetSrcRectForAnco(*surfaceParams, params);
    ASSERT_TRUE(params.srcRect == Drawing::Rect(0, 0, 100, 100));
    surfaceParams->SetAncoSrcCrop({0, 0, 0, 100});
    RSUniRenderUtil::SetSrcRectForAnco(*surfaceParams, params);
    ASSERT_TRUE(params.srcRect == Drawing::Rect(0, 0, 100, 100));
    surfaceParams->SetAncoSrcCrop({0, 0, 50, 50});
    RSUniRenderUtil::SetSrcRectForAnco(*surfaceParams, params);
    ASSERT_TRUE(params.srcRect == Drawing::Rect(0, 0, 50, 50));
}
} // namespace OHOS::Rosen
