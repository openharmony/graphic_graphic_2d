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

#include "drawable/rs_display_render_node_drawable.h"
#include "feature/gpuComposition/rs_egl_image_manager.h"
#include "pipeline/rs_display_render_node.h"
#include "pipeline/rs_main_thread.h"
#include "render_context/render_context.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Rosen {
class RsEglImageManagerTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;

    static std::shared_ptr<RenderContext> renderContext_;
    static std::shared_ptr<RSEglImageManager> eglImageManager_;
};
std::shared_ptr<RenderContext> RsEglImageManagerTest::renderContext_ = std::make_shared<RenderContext>();
std::shared_ptr<RSEglImageManager> RsEglImageManagerTest::eglImageManager_ = nullptr;

void RsEglImageManagerTest::SetUpTestCase()
{
    renderContext_->InitializeEglContext();
    renderContext_->SetUpGpuContext();
    eglImageManager_ = std::make_shared<RSEglImageManager>(renderContext_->GetEGLDisplay());
}

void RsEglImageManagerTest::TearDownTestCase()
{
    renderContext_ = nullptr;
    eglImageManager_ = nullptr;
}

void RsEglImageManagerTest::SetUp() {}
void RsEglImageManagerTest::TearDown() {}

/**
 * @tc.name: CreateAndShrinkImageCacheFromBuffer01
 * @tc.desc: Create valid ImageCache from buffer and shrink it.
 * @tc.type: FUNC
 * @tc.require: issueI6QHNP
 */
HWTEST_F(RsEglImageManagerTest, CreateAndShrinkImageCacheFromBuffer01, TestSize.Level1)
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
        // create image with null fence
        auto invalidFenceCache = eglImageManager_->CreateImageCacheFromBuffer(buffer, nullptr);
        ASSERT_NE(invalidFenceCache, nullptr);
        invalidFenceCache.release();
        // create image with valid fence
        sptr<SyncFence> acquireFence;
        auto validCache = eglImageManager_->CreateImageCacheFromBuffer(buffer, acquireFence);
        ASSERT_NE(validCache, nullptr);
        validCache.release();
        eglImageManager_->ShrinkCachesIfNeeded(true);

        // create cache from buffer directly
        auto ret = eglImageManager_->CreateImageCacheFromBuffer(buffer, 0);
        ASSERT_NE(ret, 0);
        eglImageManager_->ShrinkCachesIfNeeded(false);
    }
}

/**
 * @tc.name: CreateInvalidImageCache01
 * @tc.desc: Create invalid cache with invalid egl params.
 * @tc.type: FUNC
 * @tc.require: issueI6QHNP
 */
HWTEST_F(RsEglImageManagerTest, CreateInvalidImageCache01, TestSize.Level1)
{
    std::unique_ptr<ImageCacheSeq> invalidCache =
        std::make_unique<ImageCacheSeq>(renderContext_->GetEGLDisplay(), EGL_NO_IMAGE_KHR, nullptr);
    auto ret = invalidCache->TextureId();
    ASSERT_EQ(ret, 0);
    invalidCache.release();
}

/**
 * @tc.name: MapEglImageFromSurfaceBuffer01
 * @tc.desc: Map egl image from buffer.
 * @tc.type: FUNC
 * @tc.require: issueI6QHNP
 */
HWTEST_F(RsEglImageManagerTest, MapEglImageFromSurfaceBuffer01, TestSize.Level1)
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
 * @tc.name: ShrinkCachesIfNeeded01
 * @tc.desc: Shrink Caches
 * @tc.type: FUNC
 * @tc.require: issueI7A39J
 */
HWTEST_F(RsEglImageManagerTest, ShrinkCachesIfNeeded01, TestSize.Level1)
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
 * @tc.name: WaitAcquireFence01
 * @tc.desc: Wait nullptr acquirefence
 * @tc.type: FUNC
 * @tc.require: issueI7A39J
 */
HWTEST_F(RsEglImageManagerTest, WaitAcquireFence01, TestSize.Level1)
{
    ASSERT_NE(eglImageManager_, nullptr);
    eglImageManager_->WaitAcquireFence(nullptr);
}

/**
 * @tc.name: ShrinkCachesIfNeeded01
 * @tc.desc: UnMap eglImage with invalid seqNum
 * @tc.type: FUNC
 * @tc.require: issueI7A39J
 */
HWTEST_F(RsEglImageManagerTest, UnMapEglImage01, TestSize.Level1)
{
    ASSERT_NE(eglImageManager_, nullptr);
    const int invalidSeqNum = -1;
    eglImageManager_->UnMapEglImageFromSurfaceBuffer(invalidSeqNum);
    eglImageManager_->UnMapEglImageFromSurfaceBufferForUniRedraw(invalidSeqNum);
}

/**
 * @tc.name: ImageCacheSeqBindToTexture01
 * @tc.desc: Bind to texture
 * @tc.type: FUNC
 * @tc.require: issueI7A39J
 */
HWTEST_F(RsEglImageManagerTest, ImageCacheSeqBindToTexture01, TestSize.Level1)
{
    auto node = RSTestUtil::CreateSurfaceNodeWithBuffer();
    auto imageCache = ImageCacheSeq::Create(
        renderContext_->GetEGLDisplay(), EGL_NO_CONTEXT, node->GetRSSurfaceHandler()->GetBuffer());
    ASSERT_NE(imageCache, nullptr);
    ASSERT_EQ(imageCache->BindToTexture(), true);
    imageCache->eglImage_ =  EGL_NO_IMAGE_KHR;
    ASSERT_EQ(imageCache->BindToTexture(), false);
}

/**
 * @tc.name: ImageCacheSeqCreate01
 * @tc.desc: Create ImageCacheSeq
 * @tc.type: FUNC
 * @tc.require: issueI7A39J
 */
HWTEST_F(RsEglImageManagerTest, ImageCacheSeqCreate01, TestSize.Level1)
{
    auto node = RSTestUtil::CreateSurfaceNodeWithBuffer();
    auto imageCache = ImageCacheSeq::Create(EGL_NO_DISPLAY, EGL_NO_IMAGE_KHR, node->GetRSSurfaceHandler()->GetBuffer());
    ASSERT_EQ(imageCache, nullptr);
    imageCache = ImageCacheSeq::Create(
        renderContext_->GetEGLDisplay(), EGL_NO_CONTEXT, node->GetRSSurfaceHandler()->GetBuffer());
    ASSERT_NE(imageCache, nullptr);
}
} // namespace OHOS::Rosen
