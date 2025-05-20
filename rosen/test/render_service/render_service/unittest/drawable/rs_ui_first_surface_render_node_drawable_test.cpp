/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, Hardware
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "gtest/gtest.h"
#include "drawable/rs_surface_render_node_drawable.h"
#include "pipeline/rs_surface_render_node.h"
#include "params/rs_render_thread_params.h"
#include "pipeline/rs_display_render_node.h"
#include "pipeline/rs_render_node.h"
#include "pipeline/rs_uifirst_manager.h"
#include "pipeline/rs_uni_render_thread.h"

using namespace testing;
using namespace testing::ext;
using namespace OHOS::Rosen::DrawableV2;

namespace OHOS::Rosen {
constexpr int32_t DEFAULT_CANVAS_SIZE = 100;
constexpr NodeId DEFAULT_ID = 0xFFFF;
class RSUIFirstSurfaceRenderNodeDrawableTest : public testing::Test {
public:
    std::shared_ptr<RSSurfaceRenderNode> renderNode_;
    std::shared_ptr<RSDisplayRenderNode> displayRenderNode_;
    std::shared_ptr<RSSurfaceRenderNodeDrawable> surfaceDrawable_;
    std::shared_ptr<RSPaintFilterCanvas> canvas_;
    std::shared_ptr<Drawing::Canvas> drawingCanvas_;

    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void RSUIFirstSurfaceRenderNodeDrawableTest::SetUpTestCase() {}
void RSUIFirstSurfaceRenderNodeDrawableTest::TearDownTestCase() {}
void RSUIFirstSurfaceRenderNodeDrawableTest::SetUp()
{
    renderNode_ = std::make_shared<RSSurfaceRenderNode>(DEFAULT_ID);
    RSDisplayNodeConfig config;
    displayRenderNode_ = std::make_shared<RSDisplayRenderNode>(DEFAULT_ID, config);
    if (!renderNode_) {
        RS_LOGE("RSSurfaceRenderNodeDrawableTest: failed to create surface node.");
        return;
    }
    surfaceDrawable_ = std::static_pointer_cast<RSSurfaceRenderNodeDrawable>(
        DrawableV2::RSRenderNodeDrawableAdapter::OnGenerate(renderNode_));
    if (!surfaceDrawable_ || !surfaceDrawable_->renderParams_) {
        RS_LOGE("RSSurfaceRenderNodeDrawableTest: failed to init render params.");
        return;
    }
    drawingCanvas_ = std::make_unique<Drawing::Canvas>(DEFAULT_CANVAS_SIZE, DEFAULT_CANVAS_SIZE);
    if (drawingCanvas_) {
        canvas_ = std::make_shared<RSPaintFilterCanvas>(drawingCanvas_.get());
    }
}
void RSUIFirstSurfaceRenderNodeDrawableTest::TearDown() {}

/**
 * @tc.name: CreateUIFirstSurfaceRenderNodeDrawableTest
 * @tc.desc: Test If UIFirstSurfaceRenderNodeDrawable Can Be Created
 * @tc.type: FUNC
 * @tc.require: #I9NVOG
 */
HWTEST_F(RSUIFirstSurfaceRenderNodeDrawableTest, CreateUIFirstSurfaceRenderNodeDrawableTest, TestSize.Level1)
{
    ASSERT_NE(surfaceDrawable_, nullptr);
}

/**
 * @tc.name: GetCacheSurface
 * @tc.desc: Test If GetCacheSurface Can Run
 * @tc.type: FUNC
 * @tc.require: #I9NVOG
 */
HWTEST_F(RSUIFirstSurfaceRenderNodeDrawableTest, GetCacheSurfaceTest, TestSize.Level1)
{
    if (surfaceDrawable_ == nullptr) {
        return;
    }
    uint32_t threadIndex = 0;
    bool needCheckThread = false;
    bool releaseAfterGet = true;
    auto result = surfaceDrawable_->GetCacheSurface(threadIndex, needCheckThread, releaseAfterGet);
    EXPECT_EQ(result, nullptr);
    ASSERT_FALSE(surfaceDrawable_->cacheSurface_);

    releaseAfterGet = false;
    surfaceDrawable_->cacheSurface_ = std::make_shared<Drawing::Surface>();
    ASSERT_TRUE(surfaceDrawable_->cacheSurface_);
    result = surfaceDrawable_->GetCacheSurface(threadIndex, needCheckThread, releaseAfterGet);
    EXPECT_NE(result, nullptr);

    needCheckThread = true;
    uint32_t index = surfaceDrawable_->cacheSurfaceThreadIndex_;
    surfaceDrawable_->cacheSurfaceThreadIndex_ = threadIndex;
    result = surfaceDrawable_->GetCacheSurface(threadIndex, needCheckThread, releaseAfterGet);
    EXPECT_NE(result, nullptr);
    surfaceDrawable_->cacheSurfaceThreadIndex_ = index;
    ASSERT_FALSE(surfaceDrawable_->cacheSurfaceThreadIndex_ == threadIndex);

    result = surfaceDrawable_->GetCacheSurface(threadIndex, needCheckThread, releaseAfterGet);
    EXPECT_EQ(result, nullptr);
    surfaceDrawable_->cacheSurface_ = nullptr;
    result = surfaceDrawable_->GetCacheSurface(threadIndex, needCheckThread, releaseAfterGet);
    EXPECT_EQ(result, nullptr);
}

/**
 * @tc.name: GetCompletedImage
 * @tc.desc: Test If GetCompletedImage Can Run
 * @tc.type: FUNC
 * @tc.require: #I9NVOG
 */
HWTEST_F(RSUIFirstSurfaceRenderNodeDrawableTest, GetCompletedImageTest, TestSize.Level1)
{
    if (surfaceDrawable_ == nullptr) {
        return;
    }
    uint32_t threadIndex = 1;
    bool isUIFirst = true;

    Drawing::Canvas* drawingCanvas = new Drawing::Canvas(DEFAULT_CANVAS_SIZE, DEFAULT_CANVAS_SIZE);
    auto rscanvas = static_cast<RSPaintFilterCanvas*>(drawingCanvas);
    auto result = surfaceDrawable_->GetCompletedImage(*rscanvas, threadIndex, isUIFirst);
    EXPECT_EQ(result, nullptr);

    isUIFirst = false;
    result = surfaceDrawable_->GetCompletedImage(*rscanvas, threadIndex, isUIFirst);
    ASSERT_EQ(result, nullptr);

    surfaceDrawable_->cacheCompletedSurface_ = std::make_shared<Drawing::Surface>();
    result = surfaceDrawable_->GetCompletedImage(*rscanvas, threadIndex, isUIFirst);
    ASSERT_EQ(result, nullptr);
}

/**
 * @tc.name: DrawCacheSurface
 * @tc.desc: Test If DrawCacheSurface Can Run
 * @tc.type: FUNC
 * @tc.require: #I9NVOG
 */
HWTEST_F(RSUIFirstSurfaceRenderNodeDrawableTest, DrawCacheSurfaceTest, TestSize.Level1)
{
    if (surfaceDrawable_ == nullptr) {
        return;
    }
    Vector2f boundSize;
    uint32_t threadIndex = 1;
    bool isUIFirst = false;
    Drawing::Canvas* drawingCanvas = new Drawing::Canvas(DEFAULT_CANVAS_SIZE, DEFAULT_CANVAS_SIZE);
    auto rscanvas = static_cast<RSPaintFilterCanvas*>(drawingCanvas);
    auto result = surfaceDrawable_->DrawCacheSurface(*rscanvas, boundSize, threadIndex, isUIFirst);
    EXPECT_FALSE(result);

    surfaceDrawable_->boundsWidth_ = 1.f;
    result = surfaceDrawable_->DrawCacheSurface(*rscanvas, boundSize, threadIndex, isUIFirst);
    EXPECT_FALSE(result);

    surfaceDrawable_->boundsHeight_ = 1.f;
    result = surfaceDrawable_->DrawCacheSurface(*rscanvas, boundSize, threadIndex, isUIFirst);
    EXPECT_FALSE(result);
}

/**
 * @tc.name: NeedInitCacheSurface
 * @tc.desc: Test If NeedInitCacheSurface Can Run
 * @tc.type: FUNC
 * @tc.require: #I9NVOG
 */
HWTEST_F(RSUIFirstSurfaceRenderNodeDrawableTest, NeedInitCacheSurfaceTest, TestSize.Level1)
{
    auto result = surfaceDrawable_->NeedInitCacheSurface();
    EXPECT_TRUE(result);
    ASSERT_FALSE(surfaceDrawable_->cacheSurface_);

    surfaceDrawable_->cacheSurface_ = std::make_shared<Drawing::Surface>();
    result = surfaceDrawable_->NeedInitCacheSurface();
    EXPECT_TRUE(result);
    ASSERT_FALSE(surfaceDrawable_->cacheSurface_->GetCanvas());

    surfaceDrawable_->cacheSurface_->cachedCanvas_ = std::make_shared<Drawing::Canvas>();
    result = surfaceDrawable_->NeedInitCacheSurface();
    ASSERT_FALSE(result);
}

/**
 * @tc.name: UpdateCompletedCacheSurface
 * @tc.desc: Test If UpdateCompletedCacheSurface Can Run
 * @tc.type: FUNC
 * @tc.require: #I9NVOG
 */
HWTEST_F(RSUIFirstSurfaceRenderNodeDrawableTest, UpdateCompletedCacheSurfaceTest, TestSize.Level1)
{
    if (surfaceDrawable_ == nullptr) {
        return;
    }
    uint32_t completedSurfaceThreadIndex = 1;
    uint32_t cacheSurfaceThreadIndex = 0;
    surfaceDrawable_->completedSurfaceThreadIndex_ = completedSurfaceThreadIndex;
    surfaceDrawable_->cacheSurfaceThreadIndex_ = cacheSurfaceThreadIndex;
    surfaceDrawable_->UpdateCompletedCacheSurface();
    EXPECT_EQ(surfaceDrawable_->completedSurfaceThreadIndex_, cacheSurfaceThreadIndex);
}

#ifdef RS_ENABLE_VK
/**
 * @tc.name: ClearCacheSurface
 * @tc.desc: Test If ClearCacheSurface Can Run
 * @tc.type: FUNC
 * @tc.require: #I9NVOG
 */
HWTEST_F(RSUIFirstSurfaceRenderNodeDrawableTest, ClearCacheSurfaceTest, TestSize.Level1)
{
    if (surfaceDrawable_ == nullptr) {
        return;
    }
    surfaceDrawable_->ClearCacheSurface(true);
    ASSERT_EQ(surfaceDrawable_->cacheCompletedCleanupHelper_, nullptr);

    surfaceDrawable_->cacheCompletedSurface_ = std::make_shared<Drawing::Surface>();
    surfaceDrawable_->ClearCacheSurface(false);
    ASSERT_EQ(surfaceDrawable_->cacheCompletedCleanupHelper_, nullptr);
}
#endif
/**
 * @tc.name: IsCurFrameStatic
 * @tc.desc: Test If IsCurFrameStatic Can Run
 * @tc.type: FUNC
 * @tc.require: #I9NVOG
 */
HWTEST_F(RSUIFirstSurfaceRenderNodeDrawableTest, IsCurFrameStaticTest, TestSize.Level1)
{
    if (surfaceDrawable_ == nullptr) {
        return;
    }
    auto result = surfaceDrawable_->IsCurFrameStatic(DeviceType::PC);
    ASSERT_EQ(result, false);
}

/**
 * @tc.name: SubDraw
 * @tc.desc: Test If SubDraw Can Run
 * @tc.type: FUNC
 * @tc.require: #I9NVOG
 */
HWTEST_F(RSUIFirstSurfaceRenderNodeDrawableTest, SubDrawTest, TestSize.Level1)
{
    if (surfaceDrawable_ == nullptr) {
        return;
    }
    Drawing::Canvas* drawingCanvas = new Drawing::Canvas(DEFAULT_CANVAS_SIZE, DEFAULT_CANVAS_SIZE);
    surfaceDrawable_->SubDraw(*drawingCanvas);
    EXPECT_TRUE(surfaceDrawable_->uifirstDrawCmdList_.empty());
}

/**
 * @tc.name: DrawUIFirstCache
 * @tc.desc: Test If DrawUIFirstCache Can Run
 * @tc.type: FUNC
 * @tc.require: #I9NVOG
 */
HWTEST_F(RSUIFirstSurfaceRenderNodeDrawableTest, DrawUIFirstCacheTest, TestSize.Level1)
{
    if (surfaceDrawable_ == nullptr) {
        return;
    }
    Drawing::Canvas* drawingCanvas = new Drawing::Canvas(DEFAULT_CANVAS_SIZE, DEFAULT_CANVAS_SIZE);
    auto rscanvas = static_cast<RSPaintFilterCanvas*>(drawingCanvas);
    auto result = surfaceDrawable_->DrawUIFirstCache(*rscanvas, true);
    EXPECT_EQ(result, false);
    ASSERT_FALSE(surfaceDrawable_->HasCachedTexture());

    result = surfaceDrawable_->DrawUIFirstCache(*rscanvas, false);
    EXPECT_EQ(result, false);

    surfaceDrawable_->isTextureValid_.store(true);
    ASSERT_TRUE(surfaceDrawable_->HasCachedTexture());
    result = surfaceDrawable_->DrawUIFirstCache(*rscanvas, false);
    EXPECT_EQ(result, false);
    surfaceDrawable_->isTextureValid_.store(false);
}

/**
 * @tc.name: DrawUIFirstCacheWithStarting
 * @tc.desc: Test If DrawUIFirstCacheWithStarting Can Run
 * @tc.type: FUNC
 * @tc.require: issueIAH6OI
 */
HWTEST_F(RSUIFirstSurfaceRenderNodeDrawableTest, DrawUIFirstCacheWithStartingTest, TestSize.Level1)
{
    ASSERT_NE(surfaceDrawable_, nullptr);
    Drawing::Canvas* drawingCanvas = new Drawing::Canvas(DEFAULT_CANVAS_SIZE, DEFAULT_CANVAS_SIZE);
    auto rscanvas = static_cast<RSPaintFilterCanvas*>(drawingCanvas);
    NodeId id = 0;
    auto result = surfaceDrawable_->DrawUIFirstCacheWithStarting(*rscanvas, id);
    ASSERT_FALSE(result);

    id = 65535; // for test
    surfaceDrawable_->isTextureValid_.store(true);
    result = surfaceDrawable_->DrawUIFirstCacheWithStarting(*rscanvas, id);
    ASSERT_FALSE(result);
    ASSERT_FALSE(RSRenderNodeDrawableAdapter::RenderNodeDrawableCache_.empty());
}

#if defined(RS_ENABLE_GL) || defined(RS_ENABLE_VK)
/**
 * @tc.name: UpdateBackendTexture
 * @tc.desc: Test If UpdateBackendTexture Can Run
 * @tc.type: FUNC
 * @tc.require: issueIAH6OI
 */
HWTEST_F(RSUIFirstSurfaceRenderNodeDrawableTest, UpdateBackendTextureTest, TestSize.Level1)
{
    ASSERT_NE(surfaceDrawable_, nullptr);
    surfaceDrawable_->UpdateBackendTexture();
    ASSERT_FALSE(surfaceDrawable_->cacheSurface_);

    surfaceDrawable_->cacheSurface_ = std::make_shared<Drawing::Surface>();
    surfaceDrawable_->UpdateBackendTexture();
    ASSERT_TRUE(surfaceDrawable_->cacheSurface_);
}
#endif

/**
 * @tc.name: InitCacheSurface
 * @tc.desc: Test If InitCacheSurface Can Run
 * @tc.type: FUNC
 * @tc.require: issueIAH6OI
 */
HWTEST_F(RSUIFirstSurfaceRenderNodeDrawableTest, InitCacheSurfaceTest, TestSize.Level1)
{
    ASSERT_NE(surfaceDrawable_, nullptr);
    Drawing::GPUContext* gpuContext = new Drawing::GPUContext();
    RSSurfaceRenderNodeDrawable::ClearCacheSurfaceFunc func;
    uint32_t threadIndex = 0;
    surfaceDrawable_->InitCacheSurface(gpuContext, func, threadIndex);
    ASSERT_FALSE(surfaceDrawable_->clearCacheSurfaceFunc_);

    func = [](std::shared_ptr<Drawing::Surface>&& oldSurface, std::shared_ptr<Drawing::Surface>&& newSurface,
               uint32_t width, uint32_t height) {};
    surfaceDrawable_->InitCacheSurface(gpuContext, func, threadIndex);
    ASSERT_TRUE(surfaceDrawable_->clearCacheSurfaceFunc_);

    surfaceDrawable_->cacheSurface_ = std::make_shared<Drawing::Surface>();
    surfaceDrawable_->InitCacheSurface(gpuContext, func, threadIndex);
    delete gpuContext;
    gpuContext = nullptr;
    surfaceDrawable_->InitCacheSurface(gpuContext, func, threadIndex);
    ASSERT_FALSE(surfaceDrawable_->cacheSurface_);
}

/**
 * @tc.name: GetGravityTranslate
 * @tc.desc: Test If GetGravityTranslate Can Run
 * @tc.type: FUNC
 * @tc.require: issueIAH6OI
 */
HWTEST_F(RSUIFirstSurfaceRenderNodeDrawableTest, GetGravityTranslateTest, TestSize.Level1)
{
    ASSERT_NE(surfaceDrawable_, nullptr);
    Vector2f res = surfaceDrawable_->GetGravityTranslate(1.f, 1.f);
    ASSERT_EQ(res, Vector2f(0, 0));
}

/**
 * @tc.name: ClearCacheSurfaceOnly
 * @tc.desc: Test If ClearCacheSurfaceOnly Can Run
 * @tc.type: FUNC
 * @tc.require: issueIAH6OI
 */
HWTEST_F(RSUIFirstSurfaceRenderNodeDrawableTest, ClearCacheSurfaceOnlyTest, TestSize.Level1)
{
    ASSERT_NE(surfaceDrawable_, nullptr);
    RSSurfaceRenderNodeDrawable::ClearCacheSurfaceFunc func;
    surfaceDrawable_->ClearCacheSurfaceOnly();
    ASSERT_FALSE(surfaceDrawable_->cacheSurface_);

    surfaceDrawable_->cacheSurface_ = std::make_shared<Drawing::Surface>();
    ASSERT_TRUE(surfaceDrawable_->cacheSurface_);
    surfaceDrawable_->ClearCacheSurfaceOnly();
    ASSERT_FALSE(surfaceDrawable_->clearCacheSurfaceFunc_);

    func = [](std::shared_ptr<Drawing::Surface>&& oldSurface, std::shared_ptr<Drawing::Surface>&& newSurface,
               uint32_t width, uint32_t height) {};
    surfaceDrawable_->clearCacheSurfaceFunc_ = func;
    surfaceDrawable_->ClearCacheSurfaceOnly();
    ASSERT_TRUE(surfaceDrawable_->clearCacheSurfaceFunc_);
}

/**
 * @tc.name: ClearCacheSurfaceInThread
 * @tc.desc: Test If ClearCacheSurfaceInThread Can Run
 * @tc.type: FUNC
 * @tc.require: issueIAH6OI
 */
HWTEST_F(RSUIFirstSurfaceRenderNodeDrawableTest, ClearCacheSurfaceInThreadTest, TestSize.Level1)
{
    ASSERT_NE(surfaceDrawable_, nullptr);
    RSSurfaceRenderNodeDrawable::ClearCacheSurfaceFunc func;
    surfaceDrawable_->ClearCacheSurfaceInThread();
    ASSERT_FALSE(surfaceDrawable_->clearCacheSurfaceFunc_);

    func = [](std::shared_ptr<Drawing::Surface>&& oldSurface, std::shared_ptr<Drawing::Surface>&& newSurface,
               uint32_t width, uint32_t height) {};
    surfaceDrawable_->clearCacheSurfaceFunc_ = func;
    surfaceDrawable_->ClearCacheSurfaceInThread();
    ASSERT_TRUE(surfaceDrawable_->clearCacheSurfaceFunc_);
}
}
