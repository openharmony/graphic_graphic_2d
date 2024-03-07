/*
 * Copyright (c) 2023 Huawei Device Co., Ltd.
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

#include <memory>

#include "gtest/gtest.h"
#include "limit_number.h"

#include "pipeline/driven_render/rs_driven_render_manager.h"
#include "pipeline/driven_render/rs_driven_render_listener.h"
#include "pipeline/rs_context.h"
#include "pipeline/rs_processor_factory.h"
#include "pipeline/rs_render_node.h"
#include "pipeline/rs_uni_render_visitor.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Rosen {
class RSDrivenRenderManagerTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void RSDrivenRenderManagerTest::SetUpTestCase() {}
void RSDrivenRenderManagerTest::TearDownTestCase() {}
void RSDrivenRenderManagerTest::SetUp() {}
void RSDrivenRenderManagerTest::TearDown() {}

/**
 * @tc.name: InitInstance
 * @tc.desc: Test RSDrivenRenderManagerTest.InitInstance
 * @tc.type: FUNC
 * @tc.require: issueI6J4IL
 */
HWTEST_F(RSDrivenRenderManagerTest, InitInstance, TestSize.Level1)
{
    (void)system::SetParameter("persist.rosen.drivenrender.enabled", "false");
    RSDrivenRenderManager::InitInstance();

    (void)system::SetParameter("persist.rosen.drivenrender.enabled", "true");
    RSDrivenRenderManager::InitInstance();
}

/**
 * @tc.name: GetDrivenRenderEnabled
 * @tc.desc: Test RSDrivenRenderManagerTest.GetDrivenRenderEnabled
 * @tc.type: FUNC
 * @tc.require: issueI6J4IL
 */
HWTEST_F(RSDrivenRenderManagerTest, GetDrivenRenderEnabled, TestSize.Level1)
{
    (void)system::SetParameter("persist.rosen.drivenrender.enabled", "true");
    RSDrivenRenderManager::InitInstance();
    bool isDrivenRenderEnable = RSDrivenRenderManager::GetInstance().GetDrivenRenderEnabled();
    ASSERT_EQ(true, isDrivenRenderEnable);
}

/**
 * @tc.name: GetUniDrivenRenderMode
 * @tc.desc: Test RSDrivenRenderManagerTest.GetUniDrivenRenderMode
 * @tc.type: FUNC
 * @tc.require: issueI6J4IL
 */
HWTEST_F(RSDrivenRenderManagerTest, GetUniDrivenRenderMode, TestSize.Level1)
{
    (void)system::SetParameter("persist.rosen.drivenrender.enabled", "false");
    RSDrivenRenderManager::InitInstance();
    DrivenUniRenderMode uniRenderMode = RSDrivenRenderManager::GetInstance().GetUniDrivenRenderMode();
    ASSERT_EQ(DrivenUniRenderMode::RENDER_WITH_NORMAL, uniRenderMode);
}

/**
 * @tc.name: GetUniRenderGlobalZOrder
 * @tc.desc: Test RSDrivenRenderManagerTest.GetUniRenderGlobalZOrder
 * @tc.type: FUNC
 * @tc.require: issueI6J4IL
 */
HWTEST_F(RSDrivenRenderManagerTest, GetUniRenderGlobalZOrder, TestSize.Level1)
{
    (void)system::SetParameter("persist.rosen.drivenrender.enabled", "false");
    RSDrivenRenderManager::InitInstance();
    float uniGlobalZOrder = RSDrivenRenderManager::GetInstance().GetUniRenderGlobalZOrder();
    ASSERT_EQ(0.0f, uniGlobalZOrder);
}

/**
 * @tc.name: GetUniRenderSurfaceClipHoleRect
 * @tc.desc: Test RSDrivenRenderManagerTest.GetUniRenderSurfaceClipHoleRect
 * @tc.type: FUNC
 * @tc.require: issueI6J4IL
 */
HWTEST_F(RSDrivenRenderManagerTest, GetUniRenderSurfaceClipHoleRect, TestSize.Level1)
{
    (void)system::SetParameter("persist.rosen.drivenrender.enabled", "false");
    RSDrivenRenderManager::InitInstance();
    RSDrivenRenderManager::GetInstance().GetUniRenderSurfaceClipHoleRect();
}

/**
 * @tc.name: ClipHoleForDrivenNode
 * @tc.desc: Test RSDrivenRenderManagerTest.ClipHoleForDrivenNode
 * @tc.type: FUNC
 * @tc.require: issueI6J4IL
 */
HWTEST_F(RSDrivenRenderManagerTest, ClipHoleForDrivenNode, TestSize.Level1)
{
    (void)system::SetParameter("persist.rosen.drivenrender.enabled", "false");
    RSDrivenRenderManager::InitInstance();
    auto rsContext = std::make_shared<RSContext>();
    Drawing::Canvas tmpCanvas;
    RSPaintFilterCanvas canvas(&tmpCanvas);
    // canvas node id = 1
    auto rsCanvasRenderNode = std::make_shared<RSCanvasRenderNode>(1, rsContext->weak_from_this());

    RSDrivenRenderManager::GetInstance().GetContentSurfaceNode()->SetDrivenCanvasNode(nullptr);
    bool isClipHole = RSDrivenRenderManager::GetInstance().ClipHoleForDrivenNode(canvas, *rsCanvasRenderNode);
    ASSERT_EQ(false, isClipHole);

    auto rsDrivenCanvasRenderNode = std::make_shared<RSCanvasRenderNode>(2, rsContext->weak_from_this());
    RSDrivenRenderManager::GetInstance().GetContentSurfaceNode()->SetDrivenCanvasNode(rsDrivenCanvasRenderNode);
    isClipHole = RSDrivenRenderManager::GetInstance().ClipHoleForDrivenNode(canvas, *rsCanvasRenderNode);
    ASSERT_EQ(false, isClipHole);

    RSDrivenRenderManager::GetInstance().GetContentSurfaceNode()->SetDrivenCanvasNode(rsCanvasRenderNode);
    isClipHole = RSDrivenRenderManager::GetInstance().ClipHoleForDrivenNode(canvas, *rsCanvasRenderNode);
    ASSERT_EQ(true, isClipHole);

    // bounds(0, 0, 100, 100)
    rsCanvasRenderNode->GetMutableRenderProperties().SetBounds({0, 0, 100, 100});
    RSDrivenRenderManager::GetInstance().GetContentSurfaceNode()->SetDrivenCanvasNode(rsCanvasRenderNode);
    isClipHole = RSDrivenRenderManager::GetInstance().ClipHoleForDrivenNode(canvas, *rsCanvasRenderNode);
    ASSERT_EQ(true, isClipHole);
}

/**
 * @tc.name: DoPrepareRenderTask
 * @tc.desc: Test RSDrivenRenderManagerTest.DoPrepareRenderTask
 * @tc.type: FUNC
 * @tc.require: issueI6J4IL
 */
HWTEST_F(RSDrivenRenderManagerTest, DoPrepareRenderTask, TestSize.Level1)
{
    (void)system::SetParameter("persist.rosen.drivenrender.enabled", "false");
    RSDrivenRenderManager::InitInstance();
    auto rsContext = std::make_shared<RSContext>();
    // content node id = 1
    auto contentNode = std::make_shared<RSCanvasRenderNode>(1, rsContext->weak_from_this());
    // background node id = 2
    auto backgroundNode = std::make_shared<RSCanvasRenderNode>(2, rsContext->weak_from_this());
    contentNode->SetIsMarkDrivenRender(true);
    contentNode->SetIsMarkDriven(true);

    DrivenDirtyInfo dirtyInfo = { false, false, false, DrivenDirtyType::INVALID };
    DrivenPrepareInfo prepareInfo = { dirtyInfo, backgroundNode, contentNode, { 0, 0, 0, 0 }, false, false };
    RSDrivenRenderManager::GetInstance().DoPrepareRenderTask(prepareInfo);

    dirtyInfo = { false, false, false, DrivenDirtyType::INVALID };
    prepareInfo = { dirtyInfo, backgroundNode, contentNode, { 0, 0, 0, 0 }, true, false };
    RSDrivenRenderManager::GetInstance().DoPrepareRenderTask(prepareInfo);

    dirtyInfo = { false, false, false, DrivenDirtyType::INVALID };
    prepareInfo = { dirtyInfo, backgroundNode, contentNode, { 0, 0, 0, 0 }, false, true };
    RSDrivenRenderManager::GetInstance().DoPrepareRenderTask(prepareInfo);

    dirtyInfo = { false, false, false, DrivenDirtyType::MARK_DRIVEN_RENDER };
    prepareInfo = { dirtyInfo, backgroundNode, contentNode, { 0, 0, 0, 0 }, false, true };
    RSDrivenRenderManager::GetInstance().DoPrepareRenderTask(prepareInfo);

    dirtyInfo = { false, false, false, DrivenDirtyType::MARK_DRIVEN };
    prepareInfo = { dirtyInfo, backgroundNode, contentNode, { 0, 0, 0, 0 }, false, true };
    RSDrivenRenderManager::GetInstance().DoPrepareRenderTask(prepareInfo);
}

/**
 * @tc.name: DoProcessRenderTask
 * @tc.desc: Test RSDrivenRenderManagerTest.DoProcessRenderTask
 * @tc.type: FUNC
 * @tc.require: issueI6J4IL
 */
HWTEST_F(RSDrivenRenderManagerTest, DoProcessRenderTask, TestSize.Level1)
{
    (void)system::SetParameter("persist.rosen.drivenrender.enabled", "true");
    RSDrivenRenderManager::InitInstance();

    auto rsContext = std::make_shared<RSContext>();
    RSSurfaceRenderNodeConfig config;
    config.id = 0;
    config.name = "wx0";
    auto defaultSurfaceNode = std::make_shared<RSSurfaceRenderNode>(config, rsContext->weak_from_this());
    defaultSurfaceNode->SetSurfaceNodeType(RSSurfaceNodeType::APP_WINDOW_NODE);

    // content node id = 1
    auto drivenContentNode = std::make_shared<RSCanvasRenderNode>(1, rsContext->weak_from_this());
    // background node id = 2
    auto drivenBackgroundNode = std::make_shared<RSCanvasRenderNode>(2, rsContext->weak_from_this());

    // firstItemNode id = 3
    auto firstItemNode = std::make_shared<RSCanvasRenderNode>(3, rsContext->weak_from_this());
    firstItemNode->SetItemIndex(0);
    drivenContentNode->AddChild(firstItemNode, -1);
    // secondItemNode id = 4
    auto secondItemNode = std::make_shared<RSCanvasRenderNode>(4, rsContext->weak_from_this());
    secondItemNode->SetItemIndex(1);
    drivenContentNode->AddChild(secondItemNode, -1);
    drivenContentNode->SetIsMarkDrivenRender(true);
    drivenContentNode->SetIsMarkDriven(true);
    drivenBackgroundNode->AddChild(drivenContentNode, -1);
    defaultSurfaceNode->AddChild(drivenBackgroundNode, -1);
    // content bounds(0, 0, 100, 100)
    drivenContentNode->GetMutableRenderProperties().SetBounds({0, 0, 100, 100});
    // background bounds(0, 0, 100, 100)
    drivenBackgroundNode->GetMutableRenderProperties().SetBounds({0, 0, 100, 100});

    DrivenDirtyInfo dirtyInfo;
    DrivenPrepareInfo prepareInfo;
    DrivenProcessInfo processInfo;
    std::shared_ptr<RSProcessor> processor = nullptr;
    // screen rect(0, 0, 100, 100)
    RectI screenRect = {0, 0, 100, 100};

    auto contentSurfaceNodePtr = RSDrivenRenderManager::GetInstance().GetContentSurfaceNode();
    sptr<IBufferConsumerListener> contentListener = new RSDrivenRenderListener(contentSurfaceNodePtr);
    contentSurfaceNodePtr->CreateSurface(contentListener);
    auto backgroundSurfaceNodePtr = RSDrivenRenderManager::GetInstance().GetBackgroundSurfaceNode();
    sptr<IBufferConsumerListener> backgroundListener = new RSDrivenRenderListener(backgroundSurfaceNodePtr);
    backgroundSurfaceNodePtr->CreateSurface(backgroundListener);

    // frist frame: itemNode1 frame rect(0, 0, 100, 80), itemNode2 frame rect(0, 80, 100, 80)
    firstItemNode->GetMutableRenderProperties().SetFrame({0, 0, 100, 80});
    secondItemNode->GetMutableRenderProperties().SetFrame({0, 80, 100, 80});
    dirtyInfo = { false, false, false, DrivenDirtyType::MARK_DRIVEN_RENDER };
    prepareInfo = { dirtyInfo, drivenBackgroundNode, drivenContentNode, screenRect, false, true };
    RSDrivenRenderManager::GetInstance().DoPrepareRenderTask(prepareInfo);
    auto uniGlobalZOrder = RSDrivenRenderManager::GetInstance().GetUniRenderGlobalZOrder();
    processInfo = { processor, GraphicColorGamut::GRAPHIC_COLOR_GAMUT_SRGB, uniGlobalZOrder };
    RSDrivenRenderManager::GetInstance().DoProcessRenderTask(processInfo);

    // second frame: itemNode1 frame rect(0, -5, 100, 80), itemNode2 frame rect(0, 75, 100, 80)
    firstItemNode->GetMutableRenderProperties().SetFrame({0, -5, 100, 80});
    secondItemNode->GetMutableRenderProperties().SetFrame({0, 75, 100, 80});
    RSDrivenRenderManager::GetInstance().DoPrepareRenderTask(prepareInfo);
    uniGlobalZOrder = RSDrivenRenderManager::GetInstance().GetUniRenderGlobalZOrder();
    processInfo = { processor, GraphicColorGamut::GRAPHIC_COLOR_GAMUT_SRGB, uniGlobalZOrder };
    RSDrivenRenderManager::GetInstance().DoProcessRenderTask(processInfo);

    // third frame: itemNode1 frame rect(0, -10, 100, 80), itemNode2 frame rect(0, 70, 100, 80)
    firstItemNode->GetMutableRenderProperties().SetFrame({0, -10, 100, 80});
    secondItemNode->GetMutableRenderProperties().SetFrame({0, 70, 100, 80});
    RSDrivenRenderManager::GetInstance().DoPrepareRenderTask(prepareInfo);
    uniGlobalZOrder = RSDrivenRenderManager::GetInstance().GetUniRenderGlobalZOrder();
    auto currProcessor = RSProcessorFactory::CreateProcessor(RSDisplayRenderNode::CompositeType::UNI_RENDER_COMPOSITE);
    processInfo = { currProcessor, GraphicColorGamut::GRAPHIC_COLOR_GAMUT_SRGB, uniGlobalZOrder };
    RSDrivenRenderManager::GetInstance().DoProcessRenderTask(processInfo);

    // fourth frame: itemNode1 frame rect(0, -15, 100, 80), itemNode2 frame rect(0, 65, 100, 80)
    firstItemNode->GetMutableRenderProperties().SetFrame({0, -15, 100, 80});
    secondItemNode->GetMutableRenderProperties().SetFrame({0, 65, 100, 80});
    dirtyInfo = { false, false, false, DrivenDirtyType::MARK_DRIVEN };
    prepareInfo = { dirtyInfo, drivenBackgroundNode, drivenContentNode, screenRect, false, true };
    RSDrivenRenderManager::GetInstance().DoPrepareRenderTask(prepareInfo);
    uniGlobalZOrder = RSDrivenRenderManager::GetInstance().GetUniRenderGlobalZOrder();
    processInfo = { processor, GraphicColorGamut::GRAPHIC_COLOR_GAMUT_SRGB, uniGlobalZOrder };
    RSDrivenRenderManager::GetInstance().DoProcessRenderTask(processInfo);

    // fifth frame: itemNode1 frame rect(0, -15, 100, 80), itemNode2 frame rect(0, 65, 100, 80)
    firstItemNode->GetMutableRenderProperties().SetFrame({0, -15, 100, 80});
    secondItemNode->GetMutableRenderProperties().SetFrame({0, 65, 100, 80});
    dirtyInfo = { true, false, false, DrivenDirtyType::MARK_DRIVEN_RENDER };
    prepareInfo = { dirtyInfo, drivenBackgroundNode, drivenContentNode, screenRect, false, true };
    RSDrivenRenderManager::GetInstance().DoPrepareRenderTask(prepareInfo);
    uniGlobalZOrder = RSDrivenRenderManager::GetInstance().GetUniRenderGlobalZOrder();
    processInfo = { processor, GraphicColorGamut::GRAPHIC_COLOR_GAMUT_SRGB, uniGlobalZOrder };
    RSDrivenRenderManager::GetInstance().DoProcessRenderTask(processInfo);

    // sixth frame: itemNode1 frame rect(0, -20, 100, 80), itemNode2 frame rect(0, 60, 100, 80)
    firstItemNode->GetMutableRenderProperties().SetFrame({0, -20, 100, 80});
    secondItemNode->GetMutableRenderProperties().SetFrame({0, 60, 100, 80});
    dirtyInfo = { false, false, true, DrivenDirtyType::MARK_DRIVEN_RENDER };
    prepareInfo = { dirtyInfo, drivenBackgroundNode, drivenContentNode, screenRect, false, true };
    RSDrivenRenderManager::GetInstance().DoPrepareRenderTask(prepareInfo);
    uniGlobalZOrder = RSDrivenRenderManager::GetInstance().GetUniRenderGlobalZOrder();
    processInfo = { processor, GraphicColorGamut::GRAPHIC_COLOR_GAMUT_SRGB, uniGlobalZOrder };
    RSDrivenRenderManager::GetInstance().DoProcessRenderTask(processInfo);
}

/**
 * @tc.name: RSDrivenRenderListener
 * @tc.desc: Test RSDrivenRenderManagerTest.RSDrivenRenderListener
 * @tc.type: FUNC
 * @tc.require: issueI6J4IL
 */
HWTEST_F(RSDrivenRenderManagerTest, RSDrivenRenderListener, TestSize.Level1)
{
    auto contentSurfaceNodePtr = RSDrivenRenderManager::GetInstance().GetContentSurfaceNode();
    sptr<IBufferConsumerListener> contentListener = new RSDrivenRenderListener(contentSurfaceNodePtr);
    contentSurfaceNodePtr->CreateSurface(contentListener);
    contentListener->OnBufferAvailable();
    auto backgroundSurfaceNodePtr = RSDrivenRenderManager::GetInstance().GetBackgroundSurfaceNode();
    sptr<IBufferConsumerListener> backgroundListener = new RSDrivenRenderListener(backgroundSurfaceNodePtr);
    backgroundSurfaceNodePtr->CreateSurface(backgroundListener);
    backgroundListener->OnGoBackground();
}

} // namespace OHOS::Rosen