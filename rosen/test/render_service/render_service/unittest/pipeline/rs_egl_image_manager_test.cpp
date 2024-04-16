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
#include "pipeline/rs_egl_image_manager.h"

#include "pipeline/rs_display_render_node.h"
#include "pipeline/rs_main_thread.h"
#include "render_context/render_context.h"
#include "rs_test_util.h"

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
    renderContext_->InitializeEglContext();
    renderContext_->SetUpGpuContext();
    eglImageManager_ = std::make_shared<RSEglImageManager>(renderContext_->GetEGLDisplay());
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
    std::unique_ptr<ImageCacheSeq> invalidCache =
        std::make_unique<ImageCacheSeq>(renderContext_->GetEGLDisplay(), EGL_NO_IMAGE_KHR, nullptr);
    auto ret = invalidCache->TextureId();
    ASSERT_EQ(ret, 0);
    invalidCache.release();
}

/**
 * @tc.name: CreateAndShrinkImageCacheFromBuffer001
 * @tc.desc: Create valid ImageCache from buffer and shrink it.
 * @tc.type: FUNC
 * @tc.require: issueI6QHNP
 */
HWTEST_F(RSEglImageManagerTest, CreateAndShrinkImageCacheFromBuffer001, TestSize.Level1)
{
    NodeId displayId = 0;
    auto node = RSMainThread::Instance()->GetContext().GetNodeMap().GetRenderNode(displayId);
    ASSERT_NE(node, nullptr);
    if (auto displayNode = node->ReinterpretCastTo<RSDisplayRenderNode>()) {
        sptr<OHOS::SurfaceBuffer> buffer = displayNode->GetBuffer();
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
 * @tc.name: MapEglImageFromSurfaceBuffer001
 * @tc.desc: Map egl image from buffer.
 * @tc.type: FUNC
 * @tc.require: issueI6QHNP
 */
HWTEST_F(RSEglImageManagerTest, MapEglImageFromSurfaceBuffer001, TestSize.Level1)
{
    NodeId displayId = 0;
    auto node = RSMainThread::Instance()->GetContext().GetNodeMap().GetRenderNode(displayId);
    ASSERT_NE(node, nullptr);
    if (auto displayNode = node->ReinterpretCastTo<RSDisplayRenderNode>()) {
        sptr<OHOS::SurfaceBuffer> buffer = displayNode->GetBuffer();
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
 * @tc.desc: UnMap eglImage with invaild seqNum
 * @tc.type: FUNC
 * @tc.require: issueI7A39J
 */
HWTEST_F(RSEglImageManagerTest, UnMapEglImage001, TestSize.Level1)
{
    ASSERT_NE(eglImageManager_, nullptr);
    const int invaildSeqNum = -1;
    eglImageManager_->UnMapEglImageFromSurfaceBuffer(invaildSeqNum);
    eglImageManager_->UnMapEglImageFromSurfaceBufferForUniRedraw(invaildSeqNum);
}

/**
 * @tc.name: ImageCacheSeqCreate001
 * @tc.desc: Create ImageCacheSeq
 * @tc.type: FUNC
 * @tc.require: issueI7A39J
 */
HWTEST_F(RSEglImageManagerTest, ImageCacheSeqCreate001, TestSize.Level1)
{
    auto node = RSTestUtil::CreateSurfaceNodeWithBuffer();
    auto imageCache = ImageCacheSeq::Create(EGL_NO_DISPLAY, EGL_NO_IMAGE_KHR, node->GetBuffer());
    ASSERT_EQ(imageCache, nullptr);
    imageCache = ImageCacheSeq::Create(renderContext_->GetEGLDisplay(), EGL_NO_CONTEXT, node->GetBuffer());
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
    auto imageCache = ImageCacheSeq::Create(renderContext_->GetEGLDisplay(), EGL_NO_CONTEXT, node->GetBuffer());
    ASSERT_NE(imageCache, nullptr);
    ASSERT_EQ(imageCache->BindToTexture(), true);
    imageCache->eglImage_ =  EGL_NO_IMAGE_KHR;
    ASSERT_EQ(imageCache->BindToTexture(), false);
}
} // namespace OHOS::Rosen
