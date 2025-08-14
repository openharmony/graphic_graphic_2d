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
#include "pipeline/main_thread/rs_main_thread.h"
#include "pipeline/magic_pointer_render/rs_magic_pointer_render_manager.h"
using namespace testing;
using namespace testing::ext;

namespace OHOS::Rosen {
class RSMagicPointerRenderManagerTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
    static std::shared_ptr<RenderContext> renderContext_;
};
std::shared_ptr<RenderContext> RSMagicPointerRenderManagerTest::renderContext_ = std::make_shared<RenderContext>();

void RSMagicPointerRenderManagerTest::SetUpTestCase()
{
    renderContext_->InitializeEglContext();
    renderContext_->SetUpGpuContext();
#if defined (RS_ENABLE_VK)
    auto vkImageManager = std::make_shared<RSVkImageManager>();
    RSMagicPointerRenderManager::InitInstance(vkImageManager);
#endif

#if defined (RS_ENABLE_GL) && defined (RS_ENABLE_EGLIMAGE)
    auto eglImageManager = std::make_shared<RSEglImageManager>(renderContext_->GetEGLDisplay());
    RSMagicPointerRenderManager::InitInstance(eglImageManager);
#endif
    RSMagicPointerRenderManager::GetInstance().SetPointerColorInversionEnabled(true);
}
void RSMagicPointerRenderManagerTest::TearDownTestCase()
{
    renderContext_ = nullptr;
}
void RSMagicPointerRenderManagerTest::SetUp() {}
void RSMagicPointerRenderManagerTest::TearDown() {}

/**
 * @tc.name: SetPointerColorInversionConfig
 * @tc.desc: Test SetPointerColorInversionConfig
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSMagicPointerRenderManagerTest, SetPointerColorInversionConfig, TestSize.Level1)
{
    float darkBuffer = 0.5f;
    float brightBuffer = 0.5f;
    int64_t interval = 50;
    int32_t rangeSize = 20;
    RSMagicPointerRenderManager::GetInstance().SetPointerColorInversionConfig(darkBuffer, brightBuffer,
        interval, rangeSize);
    ASSERT_EQ(RSMagicPointerRenderManager::GetInstance().GetPointerColorInversionEnabled(), true);
}

/**
 * @tc.name: SetPointerColorInversionEnabled
 * @tc.desc: Test SetPointerColorInversionEnabled
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSMagicPointerRenderManagerTest, SetPointerColorInversionEnabled, TestSize.Level1)
{
    RSMagicPointerRenderManager::GetInstance().SetPointerColorInversionEnabled(false);
    ASSERT_EQ(RSMagicPointerRenderManager::GetInstance().GetPointerColorInversionEnabled(), false);
}

/**
 * @tc.name: RegisterPointerLuminanceChangeCallback
 * @tc.desc: Test RegisterPointerLuminanceChangeCallback
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSMagicPointerRenderManagerTest, RegisterPointerLuminanceChangeCallback, TestSize.Level1)
{
    RSMagicPointerRenderManager::GetInstance().UnRegisterPointerLuminanceChangeCallback(1);
    RSMagicPointerRenderManager::GetInstance().RegisterPointerLuminanceChangeCallback(1, nullptr);
    ASSERT_EQ(RSMagicPointerRenderManager::GetInstance().GetPointerColorInversionEnabled(), false);
}

/**
 * @tc.name: ColorPicker with image
 * @tc.desc: Test RSMagicPointerRenderManager.ColorPicker
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSMagicPointerRenderManagerTest, ColorPickerWithImage, TestSize.Level1)
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
    auto rsUniRenderProcessor = RSProcessorFactory::CreateProcessor(CompositeType::
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

    RSMagicPointerRenderManager::GetInstance().SetCacheImgForPointer(surface->GetImageSnapshot());
    RSMagicPointerRenderManager::GetInstance().ProcessColorPicker(rsUniRenderProcessor,
        paintFilterCanvas.GetGPUContext());
    RSMagicPointerRenderManager::GetInstance().SetCacheImgForPointer(nullptr);
    ASSERT_EQ(RSMagicPointerRenderManager::GetInstance().GetPointerColorInversionEnabled(), false);
}

/**
 * @tc.name: ColorPicker without image
 * @tc.desc: Test RSMagicPointerRenderManager.ColorPicker
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSMagicPointerRenderManagerTest, ColorPickerWithoutImage, TestSize.Level1)
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
    auto rsUniRenderProcessor = RSProcessorFactory::CreateProcessor(CompositeType::
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

    RSMagicPointerRenderManager::GetInstance().SetCacheImgForPointer(surface->GetImageSnapshot());
    RSMagicPointerRenderManager::GetInstance().ProcessColorPicker(rsUniRenderProcessor,
        paintFilterCanvas.GetGPUContext());
    RSMagicPointerRenderManager::GetInstance().SetCacheImgForPointer(nullptr);
    ASSERT_EQ(RSMagicPointerRenderManager::GetInstance().GetPointerColorInversionEnabled(), false);
}
} // namespace OHOS::Rosen