/*
 * Copyright (c) 2023-2025 Huawei Device Co., Ltd.
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
#include "surface_buffer_impl.h"

#include "drawable/rs_display_render_node_drawable.h"
#include "feature/gpuComposition/rs_egl_image_manager.h"
#include "pipeline/rs_display_render_node.h"
#include "pipeline/main_thread/rs_main_thread.h"
#include "render_context/render_context.h"
#include "foundation/graphic/graphic_2d/rosen/test/render_service/render_service/unittest/pipeline/rs_test_util.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Rosen {
class RSEglImageManagerTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;

    static std::shared_ptr<RenderContext> renderContext_;
    static std::shared_ptr<RSEglImageManager> eglImageManager_;
};
std::shared_ptr<RenderContext> RSEglImageManagerTest::renderContext_ = std::make_shared<RenderContext>();
std::shared_ptr<RSEglImageManager> RSEglImageManagerTest::eglImageManager_ = nullptr;

void RSEglImageManagerTest::SetUpTestCase()
{
#ifdef RS_ENABLE_VK
    RsVulkanContext::SetRecyclable(false);
#endif
    renderContext_->InitializeEglContext();
    renderContext_->SetUpGpuContext();
    eglImageManager_ = std::make_shared<RSEglImageManager>(renderContext_->GetEGLDisplay());
    RSTestUtil::InitRenderNodeGC();
}

void RSEglImageManagerTest::TearDownTestCase()
{
    renderContext_ = nullptr;
    eglImageManager_ = nullptr;
}

void RSEglImageManagerTest::SetUp() {}
void RSEglImageManagerTest::TearDown() {}

/**
 * @tc.name: CreateInvalidImageCache001
 * @tc.desc: Create invalid cache with invalid egl params.
 * @tc.type: FUNC
 * @tc.require: issueI6QHNP
 */
HWTEST_F(RSEglImageManagerTest, CreateInvalidImageCache001, TestSize.Level1)
{
    std::shared_ptr<EglImageResource> invalidCache =
        std::make_shared<EglImageResource>(renderContext_->GetEGLDisplay(), EGL_NO_IMAGE_KHR, nullptr);
    auto ret = invalidCache->GetTextureId();
    ASSERT_EQ(ret, 0);
    invalidCache.reset();
}

/**
 * @tc.name: CreateAndShrinkImageCacheFromBuffer001
 * @tc.desc: Create valid ImageCache from buffer and shrink it.
 * @tc.type: FUNC
 * @tc.require: issueI6QHNP
 */
HWTEST_F(RSEglImageManagerTest, CreateAndShrinkImageCacheFromBuffer001, TestSize.Level1)
{
    if (RSSystemProperties::GetGpuApiType() == GpuApiType::VULKAN ||
        RSSystemProperties::GetGpuApiType() == GpuApiType::DDGR || !RSUniRenderJudgement::IsUniRender()) {
        return;
    }
    NodeId id = 0;
    RSDisplayNodeConfig config;
    auto node = std::make_shared<RSDisplayRenderNode>(id, config);
    node->InitRenderParams();
    sptr<IConsumerSurface> consumer = IConsumerSurface::Create("test");
    auto displayDrawable =
        std::static_pointer_cast<DrawableV2::RSDisplayRenderNodeDrawable>(node->GetRenderDrawable());
    auto surfaceHandler = displayDrawable->GetRSSurfaceHandlerOnDraw();
    surfaceHandler->SetConsumer(consumer);
    sptr<SyncFence> acquireFence = SyncFence::INVALID_FENCE;
    int64_t timestamp = 0;
    Rect damage;
    sptr<OHOS::SurfaceBuffer> buffer = new SurfaceBufferImpl(0);
    surfaceHandler->SetBuffer(buffer, acquireFence, damage, timestamp);
    ASSERT_NE(node, nullptr);
    if (auto displayNode = node->ReinterpretCastTo<RSDisplayRenderNode>()) {
        sptr<OHOS::SurfaceBuffer> buffer = surfaceHandler->GetBuffer();
        // create cache from buffer directly
        auto ret = eglImageManager_->CreateEglImageCacheFromBuffer(buffer, 0);
        ASSERT_NE(ret, 0);
        eglImageManager_->ShrinkCachesIfNeeded(false);
    }
}

/**
 * @tc.name: MapImageFromSurfaceBuffer001
 * @tc.desc: Map egl image from buffer.
 * @tc.type: FUNC
 * @tc.require: issueI6QHNP
 */
HWTEST_F(RSEglImageManagerTest, MapImageFromSurfaceBuffer001, TestSize.Level1)
{
    if (RSSystemProperties::GetGpuApiType() == GpuApiType::VULKAN ||
        RSSystemProperties::GetGpuApiType() == GpuApiType::DDGR || !RSUniRenderJudgement::IsUniRender()) {
        return;
    }
    NodeId id = 0;
    RSDisplayNodeConfig config;
    auto node = std::make_shared<RSDisplayRenderNode>(id, config);
    node->InitRenderParams();
    sptr<IConsumerSurface> consumer = IConsumerSurface::Create("test");
    auto displayDrawable =
        std::static_pointer_cast<DrawableV2::RSDisplayRenderNodeDrawable>(node->GetRenderDrawable());
    auto surfaceHandler = displayDrawable->GetRSSurfaceHandlerOnDraw();
    surfaceHandler->SetConsumer(consumer);
    sptr<SyncFence> acquireFence = SyncFence::INVALID_FENCE;
    int64_t timestamp = 0;
    Rect damage;
    sptr<OHOS::SurfaceBuffer> buffer = new SurfaceBufferImpl(0);
    surfaceHandler->SetBuffer(buffer, acquireFence, damage, timestamp);
    ASSERT_NE(node, nullptr);
    if (auto displayNode = node->ReinterpretCastTo<RSDisplayRenderNode>()) {
        sptr<OHOS::SurfaceBuffer> buffer = surfaceHandler->GetBuffer();
        sptr<SyncFence> acquireFence;
        auto ret = eglImageManager_->MapEglImageFromSurfaceBuffer(buffer, acquireFence, 0);
        ASSERT_NE(ret, 0);
    }
}

/**
 * @tc.name: WaitAcquireFence001
 * @tc.desc: Wait nullptr acquirefence
 * @tc.type: FUNC
 * @tc.require: issueI7A39J
 */
HWTEST_F(RSEglImageManagerTest, WaitAcquireFence001, TestSize.Level1)
{
    ASSERT_NE(eglImageManager_, nullptr);
    eglImageManager_->WaitAcquireFence(nullptr);
}

/**
 * @tc.name: ShrinkCachesIfNeeded001
 * @tc.desc: Shrink Caches
 * @tc.type: FUNC
 * @tc.require: issueI7A39J
 */
HWTEST_F(RSEglImageManagerTest, ShrinkCachesIfNeeded001, TestSize.Level1)
{
    ASSERT_NE(eglImageManager_, nullptr);
    for (size_t i = 0; i <= eglImageManager_->MAX_CACHE_SIZE; i++) {
        eglImageManager_->cacheQueue_.push(i);
        eglImageManager_->imageCacheSeqs_[i] = nullptr;
    }
    eglImageManager_->ShrinkCachesIfNeeded(true);
    ASSERT_EQ(eglImageManager_->cacheQueue_.size(), eglImageManager_->MAX_CACHE_SIZE);

    eglImageManager_->cacheQueue_.push(eglImageManager_->MAX_CACHE_SIZE);
    eglImageManager_->imageCacheSeqs_[eglImageManager_->MAX_CACHE_SIZE] = nullptr;
    eglImageManager_->ShrinkCachesIfNeeded(false);
    ASSERT_EQ(eglImageManager_->cacheQueue_.size(), eglImageManager_->MAX_CACHE_SIZE);
}

/**
 * @tc.name: ShrinkCachesIfNeeded001
 * @tc.desc: UnMap eglImage with invalid seqNum
 * @tc.type: FUNC
 * @tc.require: issueI7A39J
 */
HWTEST_F(RSEglImageManagerTest, UnMapEglImage001, TestSize.Level1)
{
    ASSERT_NE(eglImageManager_, nullptr);
    const int invalidSeqNum = -1;
    eglImageManager_->UnMapImageFromSurfaceBuffer(invalidSeqNum);
    eglImageManager_->UnMapEglImageFromSurfaceBufferForUniRedraw(invalidSeqNum);
}

/**
 * @tc.name: ImageCacheSeqCreate001
 * @tc.desc: Create EglImageResource
 * @tc.type: FUNC
 * @tc.require: issueI7A39J
 */
HWTEST_F(RSEglImageManagerTest, ImageCacheSeqCreate001, TestSize.Level1)
{
    auto node = RSTestUtil::CreateSurfaceNodeWithBuffer();
    auto imageCache = EglImageResource::Create(
        EGL_NO_DISPLAY, EGL_NO_IMAGE_KHR, node->GetRSSurfaceHandler()->GetBuffer());
    ASSERT_EQ(imageCache, nullptr);
    imageCache = EglImageResource::Create(
        renderContext_->GetEGLDisplay(), EGL_NO_CONTEXT, node->GetRSSurfaceHandler()->GetBuffer());
    ASSERT_NE(imageCache, nullptr);
}

/**
 * @tc.name: ImageCacheSeqBindToTexture001
 * @tc.desc: Bind to texture
 * @tc.type: FUNC
 * @tc.require: issueI7A39J
 */
HWTEST_F(RSEglImageManagerTest, ImageCacheSeqBindToTexture001, TestSize.Level1)
{
    auto node = RSTestUtil::CreateSurfaceNodeWithBuffer();
    auto imageCache = EglImageResource::Create(
        renderContext_->GetEGLDisplay(), EGL_NO_CONTEXT, node->GetRSSurfaceHandler()->GetBuffer());
    ASSERT_NE(imageCache, nullptr);
    ASSERT_EQ(imageCache->BindToTexture(), true);
    imageCache->eglImage_ = EGL_NO_IMAGE_KHR;
    ASSERT_EQ(imageCache->BindToTexture(), false);
}

/**
 * @tc.name: CreateTest
 * @tc.desc: CreateTest
 * @tc.type: FUNC
 * @tc.require: issueI6QHNP
 */
HWTEST_F(RSEglImageManagerTest, CreateTest, TestSize.Level1)
{
    std::shared_ptr<RSImageManager> imageManager;
    std::shared_ptr<RenderContext> renderContext = std::make_shared<RenderContext>();
#ifdef RS_ENABLE_VK
    imageManager = RSImageManager::Create(renderContext);
    ASSERT_NE(imageManager, nullptr);
#endif // RS_ENABLE_VK
#ifdef RS_ENABLE_GL
    imageManager = RSImageManager::Create(renderContext);
    ASSERT_NE(imageManager, nullptr);
#endif // RS_ENABLE_GL
}

/**
 * @tc.name: CreateImageFromBufferTest
 * @tc.desc: CreateImageFromBuffer
 * @tc.type: FUNC
 * @tc.require: issueI6QHNP
 */
HWTEST_F(RSEglImageManagerTest, CreateImageFromBufferTest, TestSize.Level1)
{
    int canvasHeight = 10;
    int canvasWidth = 10;
    std::unique_ptr<Drawing::Canvas> drawingCanvas = std::make_unique<Drawing::Canvas>(canvasHeight, canvasWidth);
    std::shared_ptr<RSPaintFilterCanvas> canvas = std::make_shared<RSPaintFilterCanvas>(drawingCanvas.get());
    sptr<SurfaceBuffer> buffer = nullptr;
    sptr<SyncFence> acquireFence = nullptr;
    uint32_t threadIndex = 0;
    std::shared_ptr<Drawing::ColorSpace> drawingColorSpace = nullptr;
    std::shared_ptr<RenderContext> renderContext = std::make_shared<RenderContext>();
    renderContext->InitializeEglContext();
    renderContext->SetUpGpuContext();
    std::shared_ptr<RSImageManager> imageManager = std::make_shared<RSEglImageManager>(renderContext->GetEGLDisplay());
    auto res = imageManager->CreateImageFromBuffer(*canvas, buffer, acquireFence, threadIndex, drawingColorSpace);
    EXPECT_EQ(res, nullptr);

    buffer = SurfaceBuffer::Create();
    res = imageManager->CreateImageFromBuffer(*canvas, buffer, acquireFence, threadIndex, drawingColorSpace);
    EXPECT_NE(res, nullptr);
}

/**
 * @tc.name: GetIntersectImageTest
 * @tc.desc: GetIntersectImage
 * @tc.type: FUNC
 * @tc.require: issueI6QHNP
 */
HWTEST_F(RSEglImageManagerTest, GetIntersectImageTest, TestSize.Level1)
{
    std::shared_ptr<RenderContext> renderContext = std::make_shared<RenderContext>();
    renderContext->InitializeEglContext();
    renderContext->SetUpGpuContext();
    std::shared_ptr<RSImageManager> imageManager = std::make_shared<RSEglImageManager>(renderContext->GetEGLDisplay());
    Drawing::RectI imgCutRect = Drawing::RectI{0, 0, 10, 10};
    std::shared_ptr<Drawing::GPUContext> context = std::make_shared<Drawing::GPUContext>();
    sptr<OHOS::SurfaceBuffer> buffer = nullptr;
    sptr<SyncFence> acquireFence = nullptr;
    pid_t threadIndex = 0;
    buffer = SurfaceBuffer::Create();
    auto res = imageManager->GetIntersectImage(imgCutRect, context, buffer, acquireFence, threadIndex);
    EXPECT_EQ(res, nullptr);
}
} // namespace OHOS::Rosen
