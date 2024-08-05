/*
 * Copyright (c) 2024-2024 Huawei Device Co., Ltd.
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
#include "pipeline/rs_processor_factory.h"
#include "pipeline/rs_main_thread.h"
#include "pipeline/pointer_render/rs_pointer_render_manager.h"
using namespace testing;
using namespace testing::ext;

namespace OHOS::Rosen {
class RSPointerRenderManagerTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
    static std::shared_ptr<RenderContext> renderContext_;
};
std::shared_ptr<RenderContext> RSPointerRenderManagerTest::renderContext_ = std::make_shared<RenderContext>();

void RSPointerRenderManagerTest::SetUpTestCase()
{
    renderContext_->InitializeEglContext();
    renderContext_->SetUpGpuContext();
#if defined (RS_ENABLE_VK)
    auto vkImageManager = std::make_shared<RSVkImageManager>();
    RSPointerRenderManager::InitInstance(vkImageManager);
#endif

#if defined (RS_ENABLE_GL) && defined (RS_ENABLE_EGLIMAGE)
    auto eglImageManager = std::make_shared<RSEglImageManager>(renderContext_->GetEGLDisplay());
    RSPointerRenderManager::InitInstance(eglImageManager);
#endif
    RSPointerRenderManager::GetInstance().SetPointerColorInversionEnabled(true);
}
void RSPointerRenderManagerTest::TearDownTestCase()
{
    renderContext_ = nullptr;
}
void RSPointerRenderManagerTest::SetUp() {}
void RSPointerRenderManagerTest::TearDown() {}

/**
 * @tc.name: ColorPicker with image
 * @tc.desc: Test RSPointerRenderManager.ColorPicker
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSPointerRenderManagerTest, ColorPickerWithImage, TestSize.Level1)
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

    Drawing::Canvas drawingCanvas;
    RSPaintFilterCanvas paintFilterCanvas(&drawingCanvas);
    auto rsUniRenderProcessor = RSProcessorFactory::CreateProcessor(RSDisplayRenderNode::CompositeType::
        UNI_RENDER_COMPOSITE);

    auto& nodeMap = RSMainThread::Instance()->GetContext().GetMutableNodeMap();
    nodeMap.surfaceNodeMap_[0] = nullptr;
    NodeId id = 1;
    auto node1 = std::make_shared<RSSurfaceRenderNode>(id);
    nodeMap.surfaceNodeMap_[1] = node1;
    id = 2;
    auto node2 = std::make_shared<RSSurfaceRenderNode>(id);
    node2->SetIsOnTheTree(true);
    nodeMap.surfaceNodeMap_[2] = node2;
    id = 3;
    auto node3 = std::make_shared<RSSurfaceRenderNode>(id);
    node3->SetIsOnTheTree(true);
    node3->isLastFrameHardwareEnabled_ = true;
    nodeMap.surfaceNodeMap_[3] = node3;

    RSPointerRenderManager::GetInstance().SetCacheImgForPointer(surface->GetImageSnapshot());
    RSPointerRenderManager::GetInstance().ProcessColorPicker(rsUniRenderProcessor, paintFilterCanvas.GetGPUContext());
    RSPointerRenderManager::GetInstance().SetCacheImgForPointer(nullptr);
}

/**
 * @tc.name: ColorPicker without image
 * @tc.desc: Test RSPointerRenderManager.ColorPicker
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSPointerRenderManagerTest, ColorPickerWithoutImage, TestSize.Level1)
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

    Drawing::Canvas drawingCanvas;
    RSPaintFilterCanvas paintFilterCanvas(&drawingCanvas);
    auto rsUniRenderProcessor = RSProcessorFactory::CreateProcessor(RSDisplayRenderNode::CompositeType::
        UNI_RENDER_COMPOSITE);

    auto& nodeMap = RSMainThread::Instance()->GetContext().GetMutableNodeMap();
    nodeMap.surfaceNodeMap_[0] = nullptr;
    NodeId id = 1;
    auto node1 = std::make_shared<RSSurfaceRenderNode>(id);
    nodeMap.surfaceNodeMap_[1] = node1;
    id = 2;
    auto node2 = std::make_shared<RSSurfaceRenderNode>(id);
    node2->SetIsOnTheTree(true);
    nodeMap.surfaceNodeMap_[2] = node2;
    id = 3;
    auto node3 = std::make_shared<RSSurfaceRenderNode>(id);
    node3->SetIsOnTheTree(true);
    node3->isLastFrameHardwareEnabled_ = true;
    nodeMap.surfaceNodeMap_[3] = node3;

    RSPointerRenderManager::GetInstance().SetCacheImgForPointer(surface->GetImageSnapshot());
    RSPointerRenderManager::GetInstance().ProcessColorPicker(rsUniRenderProcessor, paintFilterCanvas.GetGPUContext());
    RSPointerRenderManager::GetInstance().SetCacheImgForPointer(nullptr);
}
} // namespace OHOS::Rosen