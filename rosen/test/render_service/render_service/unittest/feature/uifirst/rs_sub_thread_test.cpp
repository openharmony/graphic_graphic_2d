/*
 * Copyright (c) 2023-2026 Huawei Device Co., Ltd.
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

#include "drawable/rs_screen_render_node_drawable.h"
#include "feature/uifirst/rs_sub_thread.h"
#include "feature/uifirst/rs_uifirst_manager.h"
#include "engine/rs_base_render_engine.h"
#include "engine/rs_uni_render_engine.h"
#include "params/rs_screen_render_params.h"
#include "pipeline/render_thread/rs_uni_render_thread.h"
#include "pipeline/rs_screen_render_node.h"
#include "pipeline/rs_test_util.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Rosen {
class RsSubThreadTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void RsSubThreadTest::SetUpTestCase()
{
#ifdef RS_ENABLE_VK
    RsVulkanContext::SetRecyclable(false);
#endif
    RSTestUtil::InitRenderNodeGC();
}
void RsSubThreadTest::TearDownTestCase()
{
    sleep(25); // wait 25s ensure asnyc task is excuted.
}
void RsSubThreadTest::SetUp() {}
void RsSubThreadTest::TearDown() {}

/**
 * @tc.name: PostTaskTest
 * @tc.desc: Test RsSubThreadTest.PostTaskTest
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RsSubThreadTest, PostTaskTest, TestSize.Level1)
{
    auto renderContext = RenderContext::Create();
    ASSERT_TRUE(renderContext != nullptr);
    auto curThread = std::make_shared<RSSubThread>(renderContext, 0);
    curThread->PostTask([] {});
}

/**
 * @tc.name: ResetGrContext
 * @tc.desc: Test RsSubThreadTest.ResetGrContext
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RsSubThreadTest, ResetGrContext, TestSize.Level1)
{
    auto curThread = std::make_shared<RSSubThread>(nullptr, 0);
    ASSERT_TRUE(curThread != nullptr);
    curThread->grContext_ = nullptr;
    curThread->ResetGrContext();
    EXPECT_EQ(curThread->grContext_, nullptr);

    curThread->grContext_ = std::make_shared<Drawing::GPUContext>();
    curThread->ResetGrContext();
    EXPECT_NE(curThread->grContext_, nullptr);
}

/**
 * @tc.name: AddToReleaseQueue
 * @tc.desc: Test RsSubThreadTest.AddToReleaseQueue
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RsSubThreadTest, AddToReleaseQueue, TestSize.Level1)
{
    const Drawing::ImageInfo info = Drawing::ImageInfo { 200, 200, Drawing::COLORTYPE_N32, Drawing::ALPHATYPE_OPAQUE };
    auto surface(Drawing::Surface::MakeRaster(info));
    auto curThread = std::make_shared<RSSubThread>(nullptr, 0);
    ASSERT_TRUE(curThread != nullptr);
    curThread->AddToReleaseQueue(std::move(surface));
}

/**
 * @tc.name: PostSyncTaskTest001
 * @tc.desc: Verify function PostSyncTask
 * @tc.type:FUNC
 */
HWTEST_F(RsSubThreadTest, PostSyncTaskTest001, TestSize.Level1)
{
    std::shared_ptr<RenderContext> renderContext = RenderContext::Create();
    auto curThread = std::make_shared<RSSubThread>(renderContext, 0);
    curThread->PostSyncTask([] {});
    curThread->Start();
    curThread->PostSyncTask([] {});
    EXPECT_TRUE(curThread->handler_);
}

/**
 * @tc.name: RemoveTaskTest001
 * @tc.desc: Verify function RemoveTask
 * @tc.type:FUNC
 */
HWTEST_F(RsSubThreadTest, RemoveTaskTest001, TestSize.Level1)
{
    std::shared_ptr<RenderContext> renderContext = RenderContext::Create();
    auto curThread = std::make_shared<RSSubThread>(renderContext, 0);
    curThread->RemoveTask("1");
    curThread->Start();
    curThread->RemoveTask("2");
    EXPECT_TRUE(curThread->handler_);
}

/**
 * @tc.name: DumpMemTest001
 * @tc.desc: Verify function DumpMem
 * @tc.type:FUNC
 */
HWTEST_F(RsSubThreadTest, DumpMemTest001, TestSize.Level1)
{
    std::shared_ptr<RenderContext> renderContext = RenderContext::Create();
    auto curThread = std::make_shared<RSSubThread>(renderContext, 0);
    DfxString log;
    curThread->grContext_ = std::make_shared<Drawing::GPUContext>();
    auto renderNode = std::make_shared<RSSurfaceRenderNode>(0);
    RSMainThread::Instance()->GetContext().GetMutableNodeMap().RegisterRenderNode(renderNode);
    curThread->DumpMem(log, false);
    EXPECT_TRUE(curThread->grContext_);
}

/**
 * @tc.name: DumpMemTest002
 * @tc.desc: Verify function DumpMem
 * @tc.type:FUNC
 */
HWTEST_F(RsSubThreadTest, DumpMemTest002, TestSize.Level1)
{
    std::shared_ptr<RenderContext> renderContext = RenderContext::Create();
    auto curThread = std::make_shared<RSSubThread>(renderContext, 0);
    DfxString log;
    curThread->grContext_ = std::make_shared<Drawing::GPUContext>();
    auto renderNode = std::make_shared<RSSurfaceRenderNode>(0);
    RSMainThread::Instance()->GetContext().GetMutableNodeMap().RegisterRenderNode(renderNode);
    curThread->DumpMem(log, true);
    EXPECT_TRUE(curThread->grContext_);
}

/**
 * @tc.name: DumpGpuMemTest001
 * @tc.desc: Verify function DumpGpuMem
 * @tc.type:FUNC
 */
HWTEST_F(RsSubThreadTest, DumpGpuMemTest001, TestSize.Level1)
{
    std::shared_ptr<RenderContext> renderContext = RenderContext::Create();
    auto curThread = std::make_shared<RSSubThread>(renderContext, 0);
    DfxString log;
    std::vector<std::pair<NodeId, std::string>> nodeTags;
    curThread->grContext_ = std::make_shared<Drawing::GPUContext>();
    curThread->DumpGpuMem(log, nodeTags);
    EXPECT_TRUE(curThread->grContext_);
}

/**
 * @tc.name: GetAppGpuMemoryInMBTest001
 * @tc.desc: Verify function GetAppGpuMemoryInMB
 * @tc.type:FUNC
 */
HWTEST_F(RsSubThreadTest, GetAppGpuMemoryInMBTest001, TestSize.Level1)
{
    std::shared_ptr<RenderContext> renderContext = RenderContext::Create();
    auto curThread = std::make_shared<RSSubThread>(renderContext, 0);
    curThread->grContext_ = std::make_shared<Drawing::GPUContext>();
    curThread->GetAppGpuMemoryInMB();
    EXPECT_TRUE(curThread->grContext_);
}

/**
 * @tc.name: ThreadSafetyReleaseTextureTest001
 * @tc.desc: Verify function ThreadSafetyReleaseTexture
 * @tc.type:FUNC
 */
HWTEST_F(RsSubThreadTest, ThreadSafetyReleaseTextureTest001, TestSize.Level1)
{
    std::shared_ptr<RenderContext> renderContext = RenderContext::Create();
    auto curThread = std::make_shared<RSSubThread>(renderContext, 0);
    curThread->grContext_ = nullptr;
    curThread->ThreadSafetyReleaseTexture();
    EXPECT_EQ(curThread->grContext_, nullptr);

    curThread->grContext_ = std::make_shared<Drawing::GPUContext>();
    curThread->ThreadSafetyReleaseTexture();
    EXPECT_NE(curThread->grContext_, nullptr);
}

/**
 * @tc.name: ReleaseSurfaceTest001
 * @tc.desc: Verify function ReleaseSurface
 * @tc.type:FUNC
 */
HWTEST_F(RsSubThreadTest, ReleaseSurfaceTest001, TestSize.Level1)
{
    std::shared_ptr<RenderContext> renderContext = RenderContext::Create();
    auto curThread = std::make_shared<RSSubThread>(renderContext, 0);
    auto graphicsSurface = std::make_shared<Drawing::Surface>();
    curThread->AddToReleaseQueue(std::move(graphicsSurface));
    curThread->ReleaseSurface();
    EXPECT_TRUE(curThread->tmpSurfaces_.empty());
}

/**
 * @tc.name: DrawableCache001
 * @tc.desc: Verify function DrawableCache
 * @tc.type: FUNC
 * @tc.require: issueIAE59W
 */
HWTEST_F(RsSubThreadTest, DrawableCache001, TestSize.Level1)
{
    std::shared_ptr<RenderContext> renderContext = RenderContext::Create();
    renderContext->Init();
    auto curThread = std::make_shared<RSSubThread>(renderContext, 0);
    auto node = std::make_shared<const RSSurfaceRenderNode>(0);
    std::shared_ptr<DrawableV2::RSSurfaceRenderNodeDrawable> nodeDrawable = nullptr;
    EXPECT_FALSE(curThread->grContext_);
    curThread->DrawableCache(nodeDrawable);

    nodeDrawable = std::make_shared<DrawableV2::RSSurfaceRenderNodeDrawable>(std::move(node));
    curThread->DrawableCache(nodeDrawable);
    EXPECT_TRUE(curThread->grContext_);

    nodeDrawable->renderParams_ = std::make_unique<RSSurfaceRenderParams>(0);
    curThread->DrawableCache(nodeDrawable);
    EXPECT_TRUE(nodeDrawable->GetRenderParams());

    nodeDrawable->GetRsSubThreadCache().SetTaskFrameCount(1);
    curThread->DrawableCache(nodeDrawable);
    EXPECT_TRUE(nodeDrawable->GetRsSubThreadCache().GetTaskFrameCount());
}

/**
 * @tc.name: DrawableCache002
 * @tc.desc: Test subthread skip draw when frame mismatch
 * @tc.type: FUNC
 * @tc.require: issueIBVHE7
 */
HWTEST_F(RsSubThreadTest, DrawableCache002, TestSize.Level1)
{
    std::shared_ptr<RenderContext> renderContext = RenderContext::Create();
    auto curThread = std::make_shared<RSSubThread>(renderContext, 0);
    curThread->grContext_ = std::make_shared<Drawing::GPUContext>();

    auto node = std::make_shared<const RSSurfaceRenderNode>(0);
    auto nodeDrawable = std::make_shared<DrawableV2::RSSurfaceRenderNodeDrawable>(std::move(node));
    nodeDrawable->uifirstRenderParams_ = std::make_unique<RSSurfaceRenderParams>(0);
    nodeDrawable->GetRsSubThreadCache().isCacheCompletedValid_ = true;

    // test task done
    nodeDrawable->GetRsSubThreadCache().SetTaskFrameCount(1);
    RSUniRenderThread::Instance().IncreaseFrameCount();
    curThread->DrawableCache(nodeDrawable);
    EXPECT_FALSE(nodeDrawable->GetRsSubThreadCache().IsSubThreadSkip());
    EXPECT_EQ(nodeDrawable->GetRsSubThreadCache().GetCacheSurfaceProcessedStatus(), CacheProcessStatus::DONE);

    // test task skipped
    RSUniRenderThread::Instance().IncreaseFrameCount();
    curThread->DrawableCache(nodeDrawable);
    EXPECT_TRUE(nodeDrawable->GetRsSubThreadCache().IsSubThreadSkip());
    EXPECT_EQ(nodeDrawable->GetRsSubThreadCache().GetCacheSurfaceProcessedStatus(), CacheProcessStatus::SKIPPED);
}

/**
 * @tc.name: DrawableCache003
 * @tc.desc: Test subthread get subappwindow node id
 * @tc.type: FUNC
 * @tc.require: issueICRMZK
 */
HWTEST_F(RsSubThreadTest, DrawableCache003, TestSize.Level1)
{
    auto renderContext = RenderContext::Create();
    renderContext->Init();
    auto curThread = std::make_shared<RSSubThread>(renderContext, 0);
    auto surfaceNode = RSTestUtil::CreateSurfaceNode();
    std::shared_ptr<DrawableV2::RSSurfaceRenderNodeDrawable> nodeDrawable = nullptr;
    EXPECT_FALSE(curThread->grContext_);
    curThread->DrawableCache(nodeDrawable);

    nodeDrawable = std::static_pointer_cast<DrawableV2::RSSurfaceRenderNodeDrawable>(
        DrawableV2::RSRenderNodeDrawableAdapter::OnGenerate(surfaceNode));
    nodeDrawable->uifirstRenderParams_ = std::make_unique<RSSurfaceRenderParams>(nodeDrawable->GetId());
    curThread->DrawableCache(nodeDrawable);
    EXPECT_TRUE(curThread->grContext_);
    curThread->DrawableCache(nodeDrawable);
    EXPECT_TRUE(nodeDrawable->GetRenderParams());

    auto appWindow = RSTestUtil::CreateSurfaceNode();
    auto appDrawable = std::static_pointer_cast<DrawableV2::RSSurfaceRenderNodeDrawable>(
        DrawableV2::RSRenderNodeDrawableAdapter::OnGenerate(appWindow));
    auto appParams = static_cast<RSSurfaceRenderParams*>(appDrawable->GetRenderParams().get());
    appParams->SetWindowInfo(false, false, true);
    auto nodeParams = static_cast<RSSurfaceRenderParams*>(nodeDrawable->GetUifirstRenderParams().get());
    nodeParams->allSubSurfaceNodeIds_.insert(appWindow->GetId());
    curThread->DrawableCache(nodeDrawable);
    EXPECT_TRUE(nodeDrawable->GetRenderParams());

    auto subLeashWindow = RSTestUtil::CreateSurfaceNode();
    auto subLeashDrawable = std::static_pointer_cast<DrawableV2::RSSurfaceRenderNodeDrawable>(
        DrawableV2::RSRenderNodeDrawableAdapter::OnGenerate(subLeashWindow));
    auto subLeashParams = static_cast<RSSurfaceRenderParams*>(subLeashDrawable->GetRenderParams().get());
    subLeashParams->SetWindowInfo(false, true, false);
    nodeParams->allSubSurfaceNodeIds_.insert(subLeashWindow->GetId());
    curThread->DrawableCache(nodeDrawable);
    EXPECT_TRUE(nodeDrawable->GetRenderParams());

    appDrawable->renderParams_ = nullptr;
    curThread->DrawableCache(nodeDrawable);
    EXPECT_TRUE(nodeDrawable->GetRenderParams());
}

/**
 * @tc.name: CreateShareGrContext001
 * @tc.desc: Verify function CreateShareGrContext
 * @tc.type: FUNC
 * @tc.require: issueIAE59W
 */
HWTEST_F(RsSubThreadTest, CreateShareGrContext001, TestSize.Level1)
{
    std::shared_ptr<RenderContext> renderContext = RenderContext::Create();
    renderContext->Init();
    auto curThread = std::make_shared<RSSubThread>(renderContext, 0);
    EXPECT_TRUE(curThread->CreateShareGrContext());
}

/**
 * @tc.name: ReleaseCacheSurfaceOnly001
 * @tc.desc: Verify function ReleaseCacheSurfaceOnly
 * @tc.type: FUNC
 * @tc.require: issueIAE59W
 */
HWTEST_F(RsSubThreadTest, ReleaseCacheSurfaceOnly001, TestSize.Level1)
{
    std::shared_ptr<RenderContext> renderContext = RenderContext::Create();
    auto curThread = std::make_shared<RSSubThread>(renderContext, 0);
    auto node = std::make_shared<const RSSurfaceRenderNode>(0);
    std::shared_ptr<DrawableV2::RSSurfaceRenderNodeDrawable> nodeDrawable = nullptr;
    curThread->ReleaseCacheSurfaceOnly(nodeDrawable);

    nodeDrawable = std::make_shared<DrawableV2::RSSurfaceRenderNodeDrawable>(std::move(node));
    curThread->ReleaseCacheSurfaceOnly(nodeDrawable);
    EXPECT_FALSE(nodeDrawable->GetRenderParams());

    nodeDrawable->renderParams_ = std::make_unique<RSSurfaceRenderParams>(0);
    curThread->ReleaseCacheSurfaceOnly(nodeDrawable);
    EXPECT_TRUE(nodeDrawable->GetRenderParams());
}

/**
 * @tc.name: SetHighContrastIfEnabledTest
 * @tc.desc: Test high contrast setter
 * @tc.type: FUNC
 * @tc.require: issueIBVHE7
 */
HWTEST_F(RsSubThreadTest, SetHighContrastIfEnabledTest, TestSize.Level1)
{
    std::shared_ptr<RenderContext> renderContext = RenderContext::Create();
    auto curThread = std::make_shared<RSSubThread>(renderContext, 0);

    Drawing::Canvas canvas;
    RSPaintFilterCanvas filterCanvas(&canvas);

    auto renderEngine = std::make_shared<RSUniRenderEngine>();
    RSUniRenderThread::Instance().uniRenderEngine_ = renderEngine;
    ASSERT_NE(RSUniRenderThread::Instance().GetRenderEngine(), nullptr);

    RSUniRenderThread::Instance().GetRenderEngine()->SetHighContrast(true);
    curThread->SetHighContrastIfEnabled(filterCanvas);
    EXPECT_TRUE(filterCanvas.isHighContrastEnabled());

    RSUniRenderThread::Instance().GetRenderEngine()->SetHighContrast(false);
    curThread->SetHighContrastIfEnabled(filterCanvas);
    EXPECT_FALSE(filterCanvas.isHighContrastEnabled());
}

/**
 * @tc.name: DrawableCacheWithSkImageColorSpaceChangeTest001
 * @tc.desc: Test DrawableCacheWithSkImage detects color space change and rebuilds cache
 * @tc.type: FUNC
 * @tc.require: issueColorSpace
 */
HWTEST_F(RsSubThreadTest, DrawableCacheWithSkImageColorSpaceChangeTest001, TestSize.Level1)
{
    auto renderContext = RenderContext::Create();
    renderContext->Init();
    auto curThread = std::make_shared<RSSubThread>(renderContext, 0);
    ASSERT_NE(curThread, nullptr);

    auto surfaceNode = RSTestUtil::CreateSurfaceNode();
    ASSERT_NE(surfaceNode, nullptr);
    auto nodeDrawable = std::static_pointer_cast<DrawableV2::RSSurfaceRenderNodeDrawable>(
        DrawableV2::RSRenderNodeDrawableAdapter::OnGenerate(surfaceNode));
    ASSERT_NE(nodeDrawable, nullptr);

    auto& subCache = nodeDrawable->GetRsSubThreadCache();
    subCache.SetTargetColorGamut(GraphicColorGamut::GRAPHIC_COLOR_GAMUT_SRGB);
    subCache.cacheSurfaceInfo_.colorSpace = GraphicColorGamut::GRAPHIC_COLOR_GAMUT_SRGB;

    bool isColorSpaceChanged = (subCache.GetTargetColorGamut() != subCache.GetCacheSurfaceColorSpace());
    EXPECT_FALSE(isColorSpaceChanged);

    subCache.SetTargetColorGamut(GraphicColorGamut::GRAPHIC_COLOR_GAMUT_DISPLAY_P3);
    isColorSpaceChanged = (subCache.GetTargetColorGamut() != subCache.GetCacheSurfaceColorSpace());
    EXPECT_TRUE(isColorSpaceChanged);
}

/**
 * @tc.name: DrawableCacheWithSkImageColorSpaceChangeTest002
 * @tc.desc: Test DrawableCacheWithSkImage with same color space - no rebuild needed
 * @tc.type: FUNC
 * @tc.require: issueColorSpace
 */
HWTEST_F(RsSubThreadTest, DrawableCacheWithSkImageColorSpaceChangeTest002, TestSize.Level1)
{
    auto renderContext = RenderContext::Create();
    renderContext->Init();
    auto curThread = std::make_shared<RSSubThread>(renderContext, 0);
    ASSERT_NE(curThread, nullptr);

    auto surfaceNode = RSTestUtil::CreateSurfaceNode();
    ASSERT_NE(surfaceNode, nullptr);
    auto nodeDrawable = std::static_pointer_cast<DrawableV2::RSSurfaceRenderNodeDrawable>(
        DrawableV2::RSRenderNodeDrawableAdapter::OnGenerate(surfaceNode));
    ASSERT_NE(nodeDrawable, nullptr);

    auto& subCache = nodeDrawable->GetRsSubThreadCache();
    
    subCache.SetTargetColorGamut(GraphicColorGamut::GRAPHIC_COLOR_GAMUT_SRGB);
    subCache.cacheSurfaceInfo_.colorSpace = GraphicColorGamut::GRAPHIC_COLOR_GAMUT_SRGB;
    EXPECT_EQ(subCache.GetTargetColorGamut(), subCache.GetCacheSurfaceColorSpace());

    subCache.SetTargetColorGamut(GraphicColorGamut::GRAPHIC_COLOR_GAMUT_BT2020);
    subCache.cacheSurfaceInfo_.colorSpace = GraphicColorGamut::GRAPHIC_COLOR_GAMUT_BT2020;
    EXPECT_EQ(subCache.GetTargetColorGamut(), subCache.GetCacheSurfaceColorSpace());

    subCache.SetTargetColorGamut(GraphicColorGamut::GRAPHIC_COLOR_GAMUT_NATIVE);
    subCache.cacheSurfaceInfo_.colorSpace = GraphicColorGamut::GRAPHIC_COLOR_GAMUT_NATIVE;
    EXPECT_EQ(subCache.GetTargetColorGamut(), subCache.GetCacheSurfaceColorSpace());
}

/**
 * @tc.name: DrawableCacheWithSkImageColorSpaceChangeTest003
 * @tc.desc: Test color space change detection logic - BT2020 to SRGB
 * @tc.type: FUNC
 * @tc.require: issueColorSpace
 */
HWTEST_F(RsSubThreadTest, DrawableCacheWithSkImageColorSpaceChangeTest003, TestSize.Level1)
{
    auto renderContext = RenderContext::Create();
    renderContext->Init();
    auto curThread = std::make_shared<RSSubThread>(renderContext, 0);
    ASSERT_NE(curThread, nullptr);

    auto surfaceNode = RSTestUtil::CreateSurfaceNode();
    ASSERT_NE(surfaceNode, nullptr);
    auto nodeDrawable = std::static_pointer_cast<DrawableV2::RSSurfaceRenderNodeDrawable>(
        DrawableV2::RSRenderNodeDrawableAdapter::OnGenerate(surfaceNode));
    ASSERT_NE(nodeDrawable, nullptr);

    auto& subCache = nodeDrawable->GetRsSubThreadCache();
    
    subCache.SetTargetColorGamut(GraphicColorGamut::GRAPHIC_COLOR_GAMUT_BT2020);
    subCache.cacheSurfaceInfo_.colorSpace = GraphicColorGamut::GRAPHIC_COLOR_GAMUT_SRGB;
    
    bool isColorSpaceChanged = (subCache.GetTargetColorGamut() != subCache.GetCacheSurfaceColorSpace());
    EXPECT_TRUE(isColorSpaceChanged);
    EXPECT_EQ(subCache.GetTargetColorGamut(), GraphicColorGamut::GRAPHIC_COLOR_GAMUT_BT2020);
    EXPECT_EQ(subCache.GetCacheSurfaceColorSpace(), GraphicColorGamut::GRAPHIC_COLOR_GAMUT_SRGB);
}

/**
 * @tc.name: DrawableCacheWithSkImageColorSpaceChangeTest004
 * @tc.desc: Test color space change after UpdateCacheSurfaceInfo - should match
 * @tc.type: FUNC
 * @tc.require: issueColorSpace
 */
HWTEST_F(RsSubThreadTest, DrawableCacheWithSkImageColorSpaceChangeTest004, TestSize.Level1)
{
    auto renderContext = RenderContext::Create();
    renderContext->Init();
    auto curThread = std::make_shared<RSSubThread>(renderContext, 0);
    ASSERT_NE(curThread, nullptr);

    auto surfaceNode = RSTestUtil::CreateSurfaceNode();
    ASSERT_NE(surfaceNode, nullptr);
    auto nodeDrawable = std::static_pointer_cast<DrawableV2::RSSurfaceRenderNodeDrawable>(
        DrawableV2::RSRenderNodeDrawableAdapter::OnGenerate(surfaceNode));
    ASSERT_NE(nodeDrawable, nullptr);
    nodeDrawable->uifirstRenderParams_ = std::make_unique<RSSurfaceRenderParams>(nodeDrawable->GetId());

    auto& subCache = nodeDrawable->GetRsSubThreadCache();
    auto surfaceParams = static_cast<RSSurfaceRenderParams*>(nodeDrawable->GetUifirstRenderParams().get());
    
    subCache.SetTargetColorGamut(GraphicColorGamut::GRAPHIC_COLOR_GAMUT_DISPLAY_P3);
    subCache.UpdateCacheSurfaceInfo(nodeDrawable.get(), surfaceParams);
    
    bool isColorSpaceChanged = (subCache.GetTargetColorGamut() != subCache.GetCacheSurfaceColorSpace());
    EXPECT_FALSE(isColorSpaceChanged);
    EXPECT_EQ(subCache.GetCacheSurfaceColorSpace(), GraphicColorGamut::GRAPHIC_COLOR_GAMUT_DISPLAY_P3);
}

/**
 * @tc.name: DrawableCacheWithSkImageBufferFormatAndColorSpaceTest
 * @tc.desc: Test both buffer format and color space change detection
 * @tc.type: FUNC
 * @tc.require: issueColorSpace
 */
HWTEST_F(RsSubThreadTest, DrawableCacheWithSkImageBufferFormatAndColorSpaceTest, TestSize.Level1)
{
    auto renderContext = RenderContext::Create();
    renderContext->Init();
    auto curThread = std::make_shared<RSSubThread>(renderContext, 0);
    ASSERT_NE(curThread, nullptr);

    auto surfaceNode = RSTestUtil::CreateSurfaceNode();
    ASSERT_NE(surfaceNode, nullptr);
    auto nodeDrawable = std::static_pointer_cast<DrawableV2::RSSurfaceRenderNodeDrawable>(
        DrawableV2::RSRenderNodeDrawableAdapter::OnGenerate(surfaceNode));
    ASSERT_NE(nodeDrawable, nullptr);

    auto& subCache = nodeDrawable->GetRsSubThreadCache();
    
    subCache.SetTargetColorGamut(GraphicColorGamut::GRAPHIC_COLOR_GAMUT_SRGB);
    subCache.cacheSurfaceInfo_.colorSpace = GraphicColorGamut::GRAPHIC_COLOR_GAMUT_SRGB;
    
    bool bufferColorSpaceChange = (subCache.GetTargetColorGamut() != subCache.GetCacheSurfaceColorSpace());
    EXPECT_FALSE(bufferColorSpaceChange);
    
    subCache.cacheSurfaceInfo_.colorSpace = GraphicColorGamut::GRAPHIC_COLOR_GAMUT_DISPLAY_P3;
    bufferColorSpaceChange = (subCache.GetTargetColorGamut() != subCache.GetCacheSurfaceColorSpace());
    EXPECT_TRUE(bufferColorSpaceChange);
}

/**
 * @tc.name: DrawableCacheWithSkImageNullNodeDrawableTest
 * @tc.desc: Test DrawableCacheWithSkImage handles null nodeDrawable
 * @tc.type: FUNC
 * @tc.require: issueColorSpace
 */
HWTEST_F(RsSubThreadTest, DrawableCacheWithSkImageNullNodeDrawableTest, TestSize.Level1)
{
    auto renderContext = RenderContext::Create();
    renderContext->Init();
    auto curThread = std::make_shared<RSSubThread>(renderContext, 0);
    ASSERT_NE(curThread, nullptr);

    curThread->DrawableCacheWithSkImage(nullptr);
}

/**
 * @tc.name: DrawableCacheWithSkImageHDRColorSpaceChangeTest
 * @tc.desc: Test color space change with HDR scenario
 * @tc.type: FUNC
 * @tc.require: issueColorSpace
 */
HWTEST_F(RsSubThreadTest, DrawableCacheWithSkImageHDRColorSpaceChangeTest, TestSize.Level1)
{
    auto renderContext = RenderContext::Create();
    renderContext->Init();
    auto curThread = std::make_shared<RSSubThread>(renderContext, 0);
    ASSERT_NE(curThread, nullptr);

    auto surfaceNode = RSTestUtil::CreateSurfaceNode();
    ASSERT_NE(surfaceNode, nullptr);
    auto nodeDrawable = std::static_pointer_cast<DrawableV2::RSSurfaceRenderNodeDrawable>(
        DrawableV2::RSRenderNodeDrawableAdapter::OnGenerate(surfaceNode));
    ASSERT_NE(nodeDrawable, nullptr);

    auto& subCache = nodeDrawable->GetRsSubThreadCache();
    subCache.SetHDRPresent(true);
    EXPECT_TRUE(subCache.GetHDRPresent());
    
    subCache.SetTargetColorGamut(GraphicColorGamut::GRAPHIC_COLOR_GAMUT_BT2020);
    subCache.cacheSurfaceInfo_.colorSpace = GraphicColorGamut::GRAPHIC_COLOR_GAMUT_SRGB;
    
    bool isColorSpaceChanged = (subCache.GetTargetColorGamut() != subCache.GetCacheSurfaceColorSpace());
    EXPECT_TRUE(isColorSpaceChanged);
}

/**
 * @tc.name: DrawableCacheWithSkImageMultipleColorSpaceChangesTest
 * @tc.desc: Test multiple color space changes - simulating frame updates
 * @tc.type: FUNC
 * @tc.require: issueColorSpace
 */
HWTEST_F(RsSubThreadTest, DrawableCacheWithSkImageMultipleColorSpaceChangesTest, TestSize.Level1)
{
    auto renderContext = RenderContext::Create();
    renderContext->Init();
    auto curThread = std::make_shared<RSSubThread>(renderContext, 0);
    ASSERT_NE(curThread, nullptr);

    auto surfaceNode = RSTestUtil::CreateSurfaceNode();
    ASSERT_NE(surfaceNode, nullptr);
    auto nodeDrawable = std::static_pointer_cast<DrawableV2::RSSurfaceRenderNodeDrawable>(
        DrawableV2::RSRenderNodeDrawableAdapter::OnGenerate(surfaceNode));
    ASSERT_NE(nodeDrawable, nullptr);
    nodeDrawable->uifirstRenderParams_ = std::make_unique<RSSurfaceRenderParams>(nodeDrawable->GetId());

    auto& subCache = nodeDrawable->GetRsSubThreadCache();
    auto surfaceParams = static_cast<RSSurfaceRenderParams*>(nodeDrawable->GetUifirstRenderParams().get());
    
    subCache.isOcclusionEnabled_ = false;
    
    subCache.SetTargetColorGamut(GraphicColorGamut::GRAPHIC_COLOR_GAMUT_SRGB);
    subCache.UpdateCacheSurfaceInfo(nodeDrawable.get(), surfaceParams);
    EXPECT_EQ(subCache.GetCacheSurfaceColorSpace(), GraphicColorGamut::GRAPHIC_COLOR_GAMUT_SRGB);
    
    subCache.SetTargetColorGamut(GraphicColorGamut::GRAPHIC_COLOR_GAMUT_DISPLAY_P3);
    subCache.UpdateCacheSurfaceInfo(nodeDrawable.get(), surfaceParams);
    EXPECT_EQ(subCache.GetCacheSurfaceColorSpace(), GraphicColorGamut::GRAPHIC_COLOR_GAMUT_DISPLAY_P3);
    
    subCache.SetTargetColorGamut(GraphicColorGamut::GRAPHIC_COLOR_GAMUT_BT2020);
    subCache.UpdateCacheSurfaceInfo(nodeDrawable.get(), surfaceParams);
    EXPECT_EQ(subCache.GetCacheSurfaceColorSpace(), GraphicColorGamut::GRAPHIC_COLOR_GAMUT_BT2020);
    
    subCache.SetTargetColorGamut(GraphicColorGamut::GRAPHIC_COLOR_GAMUT_SRGB);
    subCache.UpdateCacheSurfaceInfo(nodeDrawable.get(), surfaceParams);
    EXPECT_EQ(subCache.GetCacheSurfaceColorSpace(), GraphicColorGamut::GRAPHIC_COLOR_GAMUT_SRGB);
}

/**
 * @tc.name: GetHdrParamsEarlyReturnBranches
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RsSubThreadTest, GetHdrParamsEarlyReturnBranches, TestSize.Level1)
{
    auto renderContext = RenderContext::Create();
    auto curThread = std::make_shared<RSSubThread>(renderContext, 0);
    ASSERT_NE(curThread, nullptr);

    {
        auto surfaceParams = std::make_shared<RSSurfaceRenderParams>(0);
        auto result = curThread->GetHdrParams(surfaceParams.get(), true, 0);
        EXPECT_TRUE(result.first);
        EXPECT_FLOAT_EQ(result.second, 1.0f);
    }

    {
        auto ancestorSurfaceNode = RSTestUtil::CreateSurfaceNode();
        auto ancestorDrawable = std::static_pointer_cast<DrawableV2::RSSurfaceRenderNodeDrawable>(
            DrawableV2::RSRenderNodeDrawableAdapter::OnGenerate(ancestorSurfaceNode));
        ASSERT_NE(ancestorDrawable, nullptr);

        auto surfaceParams = std::make_shared<RSSurfaceRenderParams>(0);
        surfaceParams->ancestorScreenDrawable_ = ancestorDrawable;

        auto result = curThread->GetHdrParams(surfaceParams.get(), true, 0);
        EXPECT_TRUE(result.first);
        EXPECT_FLOAT_EQ(result.second, 1.0f);
    }

    {
        auto screenNode = std::make_shared<RSScreenRenderNode>(1, 1);
        auto screenDrawable = std::make_shared<DrawableV2::RSScreenRenderNodeDrawable>(std::move(screenNode));
        ASSERT_NE(screenDrawable, nullptr);
        screenDrawable->renderParams_ = nullptr;

        auto surfaceParams = std::make_shared<RSSurfaceRenderParams>(0);
        surfaceParams->ancestorScreenDrawable_ = screenDrawable;

        auto result = curThread->GetHdrParams(surfaceParams.get(), true, 0);
        EXPECT_TRUE(result.first);
        EXPECT_FLOAT_EQ(result.second, 1.0f);
    }
}

/**
 * @tc.name: GetHdrParamsSuccessWithBrightnessRatio
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RsSubThreadTest, GetHdrParamsSuccessWithBrightnessRatio, TestSize.Level1)
{
    auto renderContext = RenderContext::Create();
    auto curThread = std::make_shared<RSSubThread>(renderContext, 0);
    ASSERT_NE(curThread, nullptr);

    auto screenNode = std::make_shared<RSScreenRenderNode>(1, 1);
    auto screenDrawable = std::make_shared<DrawableV2::RSScreenRenderNodeDrawable>(std::move(screenNode));
    ASSERT_NE(screenDrawable, nullptr);
    screenDrawable->renderParams_ = std::make_unique<RSScreenRenderParams>(1);
    ASSERT_NE(screenDrawable->GetRenderParams(), nullptr);

    auto screenParams = static_cast<RSScreenRenderParams*>(screenDrawable->renderParams_.get());
    ASSERT_NE(screenParams, nullptr);
    screenParams->SetHasForceHwcHdrSurface(true);
    constexpr float customRatio = 2.5f;
    screenParams->SetHdrBrightnessRatio(customRatio);
    EXPECT_FLOAT_EQ(screenParams->GetHdrBrightnessRatio(), customRatio);

    auto surfaceParams = std::make_shared<RSSurfaceRenderParams>(2);
    surfaceParams->ancestorScreenDrawable_ = screenDrawable;

    auto surfaceNode = std::make_shared<RSSurfaceRenderNode>(2);
    RSMainThread::Instance()->GetContext().GetMutableNodeMap().RegisterRenderNode(surfaceNode);

    auto result = curThread->GetHdrParams(surfaceParams.get(), true, 2);
    if (result.first) {
        EXPECT_FLOAT_EQ(result.second, 1.0f);
    } else {
        EXPECT_FLOAT_EQ(result.second, customRatio);
    }
}

/**
 * @tc.name: DrawableCacheWithSkImageHDRParams
 * @tc.desc: 
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RsSubThreadTest, DrawableCacheWithSkImageHDRParams, TestSize.Level1)
{
    auto renderContext = RenderContext::Create();
    renderContext->Init();
    auto curThread = std::make_shared<RSSubThread>(renderContext, 0);
    ASSERT_NE(curThread, nullptr);

    auto surfaceNode = RSTestUtil::CreateSurfaceNode();
    auto nodeDrawable = std::static_pointer_cast<DrawableV2::RSSurfaceRenderNodeDrawable>(
        DrawableV2::RSRenderNodeDrawableAdapter::OnGenerate(surfaceNode));
    ASSERT_NE(nodeDrawable, nullptr);

    auto& subCache = nodeDrawable->GetRsSubThreadCache();
    subCache.SetHDRPresent(true);
    subCache.SetTargetColorGamut(GraphicColorGamut::GRAPHIC_COLOR_GAMUT_BT2020);

    nodeDrawable->uifirstRenderParams_ = std::make_unique<RSSurfaceRenderParams>(nodeDrawable->GetId());
    auto surfaceParams = static_cast<RSSurfaceRenderParams*>(nodeDrawable->GetUifirstRenderParams().get());
    ASSERT_NE(surfaceParams, nullptr);
    surfaceParams->SetHDRPresent(true);

    // Set up ancestor screen drawable for GetHdrParams full path
    auto screenNode = std::make_shared<RSScreenRenderNode>(1, 1);
    auto screenDrawable = std::make_shared<DrawableV2::RSScreenRenderNodeDrawable>(std::move(screenNode));
    screenDrawable->renderParams_ = std::make_unique<RSScreenRenderParams>(1);
    auto screenParams = static_cast<RSScreenRenderParams*>(screenDrawable->renderParams_.get());
    ASSERT_NE(screenParams, nullptr);
    screenParams->SetHdrBrightnessRatio(1.5f);
    surfaceParams->ancestorScreenDrawable_ = screenDrawable;

    RSMainThread::Instance()->GetContext().GetMutableNodeMap().RegisterRenderNode(surfaceNode);

    curThread->DrawableCacheWithSkImage(nodeDrawable);
    EXPECT_EQ(curThread->grContext_, nullptr);
    EXPECT_TRUE(subCache.GetHDRPresent());
}
} // namespace OHOS::Rosen