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

#include "common/rs_obj_geometry.h"
#include "draw/canvas.h"
#include "modifier/rs_modifier_type.h"
#include "params/rs_render_params.h"
#include "pipeline/rs_canvas_drawing_render_node.h"
#include "pipeline/rs_context.h"
#include "pipeline/rs_surface_render_node.h"
#include "property/rs_properties_painter.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Rosen {
class RSCanvasDrawingRenderNodeDrawableAdapterTest : public DrawableV2::RSRenderNodeDrawableAdapter {
public:
    explicit RSCanvasDrawingRenderNodeDrawableAdapterTest(std::shared_ptr<const RSRenderNode> node)
        : RSRenderNodeDrawableAdapter(std::move(node))
    {}
    void Draw(Drawing::Canvas& canvas) {}
};
class RSCanvasDrawingRenderNodeTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
    static inline NodeId id;
    static inline std::weak_ptr<RSContext> context = {};
    static inline RSPaintFilterCanvas* canvas_;
    static inline Drawing::Canvas drawingCanvas_;
};

void RSCanvasDrawingRenderNodeTest::SetUpTestCase()
{
    canvas_ = new RSPaintFilterCanvas(&drawingCanvas_);
}
void RSCanvasDrawingRenderNodeTest::TearDownTestCase()
{
    if (canvas_) {
        delete canvas_;
        canvas_ = nullptr;
    }
}
void RSCanvasDrawingRenderNodeTest::SetUp() {}
void RSCanvasDrawingRenderNodeTest::TearDown() {}

/**
 * @tc.name: ProcessRenderContentsTest
 * @tc.desc: test results of ProcessRenderContents
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSCanvasDrawingRenderNodeTest, ProcessRenderContentsTest, TestSize.Level1)
{
    NodeId nodeId = 1;
    std::weak_ptr<RSContext> context;
    RSCanvasDrawingRenderNode rsCanvasDrawingRenderNode(nodeId, context);
    rsCanvasDrawingRenderNode.ProcessRenderContents(*canvas_);

    Drawing::Matrix matrix;
    PropertyId id = 1;
    std::shared_ptr<RSRenderProperty<Drawing::Matrix>> property =
        std::make_shared<RSRenderProperty<Drawing::Matrix>>(matrix, id);
    std::shared_ptr<RSGeometryTransRenderModifier> modifierCast =
        std::make_shared<RSGeometryTransRenderModifier>(property);
    std::shared_ptr<RSRenderModifier> modifier = modifierCast;
    modifierCast->drawStyle_ = RSModifierType::BOUNDS;
    rsCanvasDrawingRenderNode.boundsModifier_ = modifier;
    rsCanvasDrawingRenderNode.AddGeometryModifier(modifier);
    rsCanvasDrawingRenderNode.surface_ = std::make_shared<Drawing::Surface>();
    rsCanvasDrawingRenderNode.image_ = std::make_shared<Drawing::Image>();
    rsCanvasDrawingRenderNode.ProcessRenderContents(*canvas_);
    ASSERT_FALSE(rsCanvasDrawingRenderNode.isNeedProcess_);
}

/**
 * @tc.name: ProcessRenderContentsOtherTest
 * @tc.desc: test results of ProcessRenderContents other case
 * @tc.type:FUNC
 * @tc.require: issueI9W0GK
 */
HWTEST_F(RSCanvasDrawingRenderNodeTest, ProcessRenderContentsOtherTest, TestSize.Level1)
{
    NodeId nodeId = 1;
    RSCanvasDrawingRenderNode rsCanvasDrawingRenderNode(nodeId);
    // case 1: can GetSizeFromDrawCmdModifiers
    std::shared_ptr<Drawing::DrawCmdList> drawCmdList = std::make_shared<Drawing::DrawCmdList>();
    drawCmdList->SetWidth(1024);
    drawCmdList->SetHeight(1090);
    auto property = std::make_shared<RSRenderProperty<Drawing::DrawCmdListPtr>>();
    property->GetRef() = drawCmdList;
    std::list<std::shared_ptr<RSRenderModifier>> listModifier { std::make_shared<RSDrawCmdListRenderModifier>(
        property) };
    rsCanvasDrawingRenderNode.renderContent_->drawCmdModifiers_.emplace(RSModifierType::CONTENT_STYLE, listModifier);
    std::function<void(std::shared_ptr<Drawing::Surface>)> callbackFunc = [](std::shared_ptr<Drawing::Surface>) {
        printf("ProcessRenderContentsTest callbackFunc\n");
    };
    // case 1.1: IsNeedResetSurface
    rsCanvasDrawingRenderNode.preThreadInfo_.second = callbackFunc;
    rsCanvasDrawingRenderNode.ProcessRenderContents(*canvas_);
    // case 1.2: Isn't NeedResetSurface
    rsCanvasDrawingRenderNode.isGpuSurface_ = true;
    rsCanvasDrawingRenderNode.preThreadInfo_.first = 10;
    rsCanvasDrawingRenderNode.isNeedProcess_.store(true);
    rsCanvasDrawingRenderNode.ProcessRenderContents(*canvas_);
    EXPECT_TRUE(rsCanvasDrawingRenderNode.isNeedProcess_);
    // case 2.2.1: can GetGravityMatrix, recordingCanvas_ is false
    rsCanvasDrawingRenderNode.isGpuSurface_ = false;
    rsCanvasDrawingRenderNode.GetMutableRenderProperties().frameGravity_ = Gravity::TOP;
    rsCanvasDrawingRenderNode.GetMutableRenderProperties().frameGeo_.SetHeight(3048.0f);
    rsCanvasDrawingRenderNode.GetMutableRenderProperties().frameGeo_.SetWidth(2048.0f);
    rsCanvasDrawingRenderNode.ProcessRenderContents(*canvas_);
    EXPECT_FALSE(rsCanvasDrawingRenderNode.isNeedProcess_);
    EXPECT_TRUE(rsCanvasDrawingRenderNode.recordingCanvas_ == nullptr);
    EXPECT_TRUE(rsCanvasDrawingRenderNode.image_ != nullptr);
    EXPECT_FALSE(canvas_->GetRecordingState());
    // case 2.2.2: can GetGravityMatrix, recordingCanvas_ is true
    canvas_->SetRecordingState(true);
    rsCanvasDrawingRenderNode.ProcessRenderContents(*canvas_);
    canvas_->SetRecordingState(false);
}

/**
 * @tc.name: ResetSurfaceTest001
 * @tc.desc: test results of ResetSurface
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSCanvasDrawingRenderNodeTest, ResetSurfaceTest001, TestSize.Level1)
{
    int width = 0;
    int height = 0;
    NodeId nodeId = 1;
    std::weak_ptr<RSContext> context;
    RSCanvasDrawingRenderNode rsCanvasDrawingRenderNode(nodeId, context);
    auto res = rsCanvasDrawingRenderNode.ResetSurface(width, height, *canvas_);
    ASSERT_FALSE(res);
}

/**
 * @tc.name: ResetSurfaceTest002
 * @tc.desc: test results of ResetSurface
 * @tc.type:FUNC
 * @tc.require: #ICDBD1
 */
HWTEST_F(RSCanvasDrawingRenderNodeTest, ResetSurfaceTest002, TestSize.Level1)
{
    int width = 0;
    int height = 0;
    NodeId nodeId = 1;
    NodeId rootNodeId = 2;
    auto context = std::make_shared<RSContext>();
    RSCanvasDrawingRenderNode rsCanvasDrawingRenderNode(nodeId, context);
    auto surfaceNode = std::make_shared<RSSurfaceRenderNode>(rootNodeId);
    surfaceNode->SetColorSpace(GraphicColorGamut::GRAPHIC_COLOR_GAMUT_DISPLAY_P3);
    context->GetMutableNodeMap().RegisterRenderNode(surfaceNode);
    rsCanvasDrawingRenderNode.instanceRootNodeId_ = rootNodeId;
    EXPECT_NE(rsCanvasDrawingRenderNode.GetInstanceRootNode(), nullptr);
    auto appSurfaceNode =
        RSBaseRenderNode::ReinterpretCast<RSSurfaceRenderNode>(rsCanvasDrawingRenderNode.GetInstanceRootNode());
    EXPECT_NE(appSurfaceNode, nullptr);
    rsCanvasDrawingRenderNode.ResetSurface(width, height);
    ASSERT_EQ(rsCanvasDrawingRenderNode.stagingRenderParams_->surfaceParams_.colorSpace,
        GraphicColorGamut::GRAPHIC_COLOR_GAMUT_DISPLAY_P3);
}

/**
 * @tc.name: GetSizeFromDrawCmdModifiersTest001
 * @tc.desc: test results of GetSizeFromDrawCmdModifiers
 * @tc.type: FUNC
 * @tc.require: issueI9W0GK
 */
HWTEST_F(RSCanvasDrawingRenderNodeTest, GetSizeFromDrawCmdModifiersTest001, TestSize.Level1)
{
    int32_t width = 0;
    int32_t height = 0;
    NodeId nodeId = 1;
    std::weak_ptr<RSContext> context;
    RSCanvasDrawingRenderNode rsCanvasDrawingRenderNode(nodeId, context);
    EXPECT_FALSE(rsCanvasDrawingRenderNode.GetSizeFromDrawCmdModifiers(width, height));

    std::shared_ptr<Drawing::DrawCmdList> drawCmdList = std::make_shared<Drawing::DrawCmdList>(1024, 1090);
    auto property = std::make_shared<RSRenderProperty<Drawing::DrawCmdListPtr>>();
    property->GetRef() = drawCmdList;
    std::list<std::shared_ptr<RSRenderModifier>> listModifier { std::make_shared<RSDrawCmdListRenderModifier>(
        property) };
    rsCanvasDrawingRenderNode.renderContent_->drawCmdModifiers_.emplace(RSModifierType::CONTENT_STYLE, listModifier);
    EXPECT_TRUE(rsCanvasDrawingRenderNode.GetSizeFromDrawCmdModifiers(width, height));
}

/**
 * @tc.name: GetSizeFromDrawCmdModifiersTest002
 * @tc.desc: test results of GetSizeFromDrawCmdModifiers
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSCanvasDrawingRenderNodeTest, GetSizeFromDrawCmdModifiersTest002, TestSize.Level1)
{
    int width = -1;
    int height = -1;
    NodeId nodeId = 1;
    std::weak_ptr<RSContext> context;
    RSCanvasDrawingRenderNode rsCanvasDrawingRenderNode(nodeId, context);

    Drawing::Matrix matrix;
    PropertyId id = 1;
    std::shared_ptr<RSRenderProperty<Drawing::Matrix>> property =
        std::make_shared<RSRenderProperty<Drawing::Matrix>>(matrix, id);
    std::shared_ptr<RSGeometryTransRenderModifier> modifierCast =
        std::make_shared<RSGeometryTransRenderModifier>(property);
    modifierCast->SetType(RSModifierType::CONTENT_STYLE);
    std::shared_ptr<RSRenderModifier> modifier = modifierCast;
    rsCanvasDrawingRenderNode.modifiers_.emplace(modifier->GetPropertyId(), modifier);
    auto res = rsCanvasDrawingRenderNode.GetSizeFromDrawCmdModifiers(width, height);
    ASSERT_FALSE(res);

    width = 1;
    height = 1;
    ASSERT_FALSE(rsCanvasDrawingRenderNode.GetSizeFromDrawCmdModifiers(width, height));
}

/**
 * @tc.name: GetSizeFromDrawCmdModifiersTest003
 * @tc.desc: test results of GetSizeFromDrawCmdModifiers
 * @tc.type: FUNC
 * @tc.require: issueI9W0GK
 */
HWTEST_F(RSCanvasDrawingRenderNodeTest, GetSizeFromDrawCmdModifiersTest003, TestSize.Level1)
{
    int32_t width = 0;
    int32_t height = 0;
    NodeId nodeId = 1;
    RSCanvasDrawingRenderNode rsCanvasDrawingRenderNode(nodeId);
    EXPECT_FALSE(rsCanvasDrawingRenderNode.GetSizeFromDrawCmdModifiers(width, height));
    std::shared_ptr<Drawing::DrawCmdList> drawCmdList = std::make_shared<Drawing::DrawCmdList>(width, height);
    auto property = std::make_shared<RSRenderProperty<Drawing::DrawCmdListPtr>>();
    property->GetRef() = drawCmdList;
    std::list<std::shared_ptr<RSRenderModifier>> listModifier { std::make_shared<RSDrawCmdListRenderModifier>(
        property) };
    rsCanvasDrawingRenderNode.renderContent_->drawCmdModifiers_.emplace(RSModifierType::CONTENT_STYLE, listModifier);
    EXPECT_FALSE(rsCanvasDrawingRenderNode.GetSizeFromDrawCmdModifiers(width, height));
}
/**
 * @tc.name: IsNeedResetSurfaceTest
 * @tc.desc: test results of IsNeedResetSurface
 * @tc.type: FUNC
 * @tc.require: issueI9W0GK
 */
HWTEST_F(RSCanvasDrawingRenderNodeTest, IsNeedResetSurfaceTest, TestSize.Level1)
{
    NodeId nodeId = 1;
    std::weak_ptr<RSContext> context;
    RSCanvasDrawingRenderNode rsCanvasDrawingRenderNode(nodeId, context);
    rsCanvasDrawingRenderNode.surface_ = std::make_shared<Drawing::Surface>();
    ASSERT_TRUE(rsCanvasDrawingRenderNode.IsNeedResetSurface());

    int32_t width = 1024;
    int32_t height = 2468;
    rsCanvasDrawingRenderNode.surface_->cachedCanvas_ = std::make_shared<Drawing::Canvas>(width, height);
    EXPECT_FALSE(rsCanvasDrawingRenderNode.IsNeedResetSurface());
}


/**
 * @tc.name: InitRenderParamsTest
 * @tc.desc: test results of InitRenderParams
 * @tc.type: FUNC
 * @tc.require: issueI9W0GK
 */
HWTEST_F(RSCanvasDrawingRenderNodeTest, InitRenderParamsTest, TestSize.Level1)
{
    NodeId nodeId = 1;
    auto rsCanvasDrawingRenderNode = std::make_shared<RSCanvasDrawingRenderNode>(nodeId);
    rsCanvasDrawingRenderNode->InitRenderParams();
    EXPECT_TRUE(rsCanvasDrawingRenderNode->renderDrawable_ == nullptr);
}

/**
 * @tc.name: PlaybackInCorrespondThread
 * @tc.desc: test results of PlaybackInCorrespondThread
 * @tc.type: FUNC
 * @tc.require: issueI9W0GK
 */
HWTEST_F(RSCanvasDrawingRenderNodeTest, PlaybackInCorrespondThread, TestSize.Level1)
{
    NodeId nodeId = 1;
    std::shared_ptr<RSContext> sharedContext = std::make_shared<RSContext>();
    std::weak_ptr<RSContext> context = sharedContext;
    auto rsCanvasDrawingRenderNode = std::make_shared<RSCanvasDrawingRenderNode>(nodeId, context);
    auto ctxNode =
        context.lock()->GetNodeMap().GetRenderNode<RSCanvasDrawingRenderNode>(rsCanvasDrawingRenderNode->GetId());
    EXPECT_TRUE(ctxNode == nullptr);
    rsCanvasDrawingRenderNode->PlaybackInCorrespondThread();

    auto& ctxNodeMap = context.lock()->GetMutableNodeMap();
    NodeId rsCanvasDrawingRenderNodeId = rsCanvasDrawingRenderNode->GetId();
    pid_t rsCanvasDrawingRenderNodePid = ExtractPid(rsCanvasDrawingRenderNodeId);
    ctxNodeMap.renderNodeMap_[rsCanvasDrawingRenderNodePid][rsCanvasDrawingRenderNodeId] = rsCanvasDrawingRenderNode;
    ctxNode = context.lock()->GetNodeMap().GetRenderNode<RSCanvasDrawingRenderNode>(rsCanvasDrawingRenderNode->GetId());
    EXPECT_TRUE(ctxNode != nullptr);
    int32_t width = 1;
    int32_t height = 1;
    Drawing::Canvas canvas(width, height);
    rsCanvasDrawingRenderNode->canvas_ = std::make_unique<RSPaintFilterCanvas>(&canvas);
    rsCanvasDrawingRenderNode->PlaybackInCorrespondThread();
    rsCanvasDrawingRenderNode->surface_ = std::make_shared<Drawing::Surface>();
    rsCanvasDrawingRenderNode->isNeedProcess_.store(true);
    rsCanvasDrawingRenderNode->PlaybackInCorrespondThread();
    EXPECT_FALSE(rsCanvasDrawingRenderNode->isNeedProcess_);
}

/**
 * @tc.name: ApplyDrawCmdModifier
 * @tc.desc: test results of ApplyDrawCmdModifier
 * @tc.type: FUNC
 * @tc.require: issueI9W0GK
 */
HWTEST_F(RSCanvasDrawingRenderNodeTest, ApplyDrawCmdModifier, TestSize.Level1)
{
    NodeId nodeId = 1;
    std::weak_ptr<RSContext> context;
    RSCanvasDrawingRenderNode rsCanvasDrawingRenderNode(nodeId, context);

    RSProperties properties;
    RSModifierContext contextArgs(properties);
    rsCanvasDrawingRenderNode.ApplyDrawCmdModifier(contextArgs, RSModifierType::CONTENT_STYLE);

    int width = 1024;
    int hight = 1090;
    auto type = RSModifierType::CONTENT_STYLE;
    std::list<Drawing::DrawCmdListPtr> listDrawCmd { std::make_shared<Drawing::DrawCmdList>(width, hight) };
    rsCanvasDrawingRenderNode.drawCmdLists_.emplace(type, listDrawCmd);
    auto it = rsCanvasDrawingRenderNode.drawCmdLists_.find(type);
    EXPECT_FALSE(it == rsCanvasDrawingRenderNode.drawCmdLists_.end() || it->second.empty());
    Drawing::Canvas canvas(width, hight);
    RSPaintFilterCanvas paintCanvas(&canvas);
    contextArgs.canvas_ = &paintCanvas;
    rsCanvasDrawingRenderNode.ApplyDrawCmdModifier(contextArgs, type);
}

/**
 * @tc.name: GetBitmap
 * @tc.desc: test results of GetBitmap
 * @tc.type: FUNC
 * @tc.require: issueI9W0GK
 */
HWTEST_F(RSCanvasDrawingRenderNodeTest, GetBitmap, TestSize.Level1)
{
    NodeId nodeId = 1;
    std::weak_ptr<RSContext> context;
    RSCanvasDrawingRenderNode rsCanvasDrawingRenderNode(nodeId, context);
    uint64_t tid = 1;
    rsCanvasDrawingRenderNode.GetBitmap(tid);

    rsCanvasDrawingRenderNode.image_ = std::make_shared<Drawing::Image>();
    Drawing::Bitmap bitmap;
    EXPECT_FALSE(rsCanvasDrawingRenderNode.image_->AsLegacyBitmap(bitmap));
    ASSERT_NE(rsCanvasDrawingRenderNode.GetBitmap(tid).bmpImplPtr, nullptr);

    rsCanvasDrawingRenderNode.GetBitmap();
}

/**
 * @tc.name: GetPixelmap
 * @tc.desc: test results of GetPixelmap
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSCanvasDrawingRenderNodeTest, GetPixelmap, TestSize.Level1)
{
    NodeId nodeId = 1;
    std::weak_ptr<RSContext> context;
    RSCanvasDrawingRenderNode rsCanvasDrawingRenderNode(nodeId, context);
    uint64_t tid = 1;
    int32_t width = 1;
    int32_t height = 1;
    {
        std::shared_ptr<Media::PixelMap> pixelmap = std::make_shared<Media::PixelMap>();
        Drawing::Rect rect(1, 1, 1, 1);
        std::shared_ptr<Drawing::DrawCmdList> drawCmdList = std::make_shared<Drawing::DrawCmdList>(width, height);
        ASSERT_FALSE(rsCanvasDrawingRenderNode.GetPixelmap(pixelmap, &rect, tid, drawCmdList));
    }
    {
        std::shared_ptr<Media::PixelMap> pixelmap;
        Drawing::Rect* rect = nullptr;
        std::shared_ptr<Drawing::DrawCmdList> drawCmdList = std::make_shared<Drawing::DrawCmdList>(width, height);
        ASSERT_FALSE(rsCanvasDrawingRenderNode.GetPixelmap(pixelmap, rect, tid, drawCmdList));
    }
    {
        std::shared_ptr<Media::PixelMap> pixelmap = std::make_shared<Media::PixelMap>();
        Drawing::Rect rect(1, 1, 1, 1);
        std::shared_ptr<Drawing::DrawCmdList> drawCmdList = std::make_shared<Drawing::DrawCmdList>(width, height);
        rsCanvasDrawingRenderNode.surface_ = std::make_shared<Drawing::Surface>();
        uint64_t tidTwo = OHOS::Rosen::UNI_MAIN_THREAD_INDEX;
        ASSERT_FALSE(rsCanvasDrawingRenderNode.GetPixelmap(pixelmap, &rect, tid, drawCmdList));
        ASSERT_FALSE(rsCanvasDrawingRenderNode.GetPixelmap(pixelmap, &rect, tidTwo, drawCmdList));
    }
}

/**
 * @tc.name: GetPixelmapSurfaceImgValidTest
 * @tc.desc: test results of GetPixelmapSurfaceImgValid
 * @tc.type: FUNC
 * @tc.require: issueI9W0GK
 */
HWTEST_F(RSCanvasDrawingRenderNodeTest, GetPixelmapSurfaceImgValidTest, TestSize.Level1)
{
    uint64_t tid = 1;
    NodeId nodeId = 2;
    auto rsCanvasDrawingRenderNode = std::make_shared<RSCanvasDrawingRenderNode>(nodeId);
    std::shared_ptr<Media::PixelMap> pixelmap = std::make_shared<Media::PixelMap>();
    Drawing::Rect rect(1, 2, 3, 4);
    std::shared_ptr<Drawing::DrawCmdList> drawCmdList;
    rsCanvasDrawingRenderNode->surface_ = std::make_shared<Drawing::Surface>();
    Drawing::BitmapFormat bitmapFormat { Drawing::COLORTYPE_RGBA_8888, Drawing::ALPHATYPE_OPAQUE };
    Drawing::Bitmap bitmap;
    bitmap.Build(10, 10, bitmapFormat);
    EXPECT_TRUE(rsCanvasDrawingRenderNode->surface_->Bind(bitmap));
    EXPECT_TRUE(rsCanvasDrawingRenderNode->surface_->GetImageSnapshot() != nullptr);
    EXPECT_FALSE(rsCanvasDrawingRenderNode->GetPixelmap(pixelmap, &rect, tid, drawCmdList));

    tid = rsCanvasDrawingRenderNode->GetTid();
    EXPECT_FALSE(rsCanvasDrawingRenderNode->GetPixelmap(pixelmap, &rect, tid, drawCmdList));

#if (defined(RS_ENABLE_GL) || defined(RS_ENABLE_VK))
    int32_t width = 1024;
    int32_t height = 2468;
    drawCmdList = std::make_shared<Drawing::DrawCmdList>(width, height);
    Drawing::Canvas canvas(width, height);
    rsCanvasDrawingRenderNode->canvas_ = std::make_unique<RSPaintFilterCanvas>(&canvas);
    EXPECT_TRUE(rsCanvasDrawingRenderNode->canvas_->GetGPUContext() == nullptr);
    EXPECT_FALSE(rsCanvasDrawingRenderNode->GetPixelmap(pixelmap, &rect, tid, drawCmdList));

    rsCanvasDrawingRenderNode->canvas_->gpuContext_ = std::make_shared<Drawing::GPUContext>();
    EXPECT_FALSE(rsCanvasDrawingRenderNode->GetPixelmap(pixelmap, &rect, tid, drawCmdList));
#endif
}

/**
 * @tc.name: AddDirtyType
 * @tc.desc: test results of AddDirtyType
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSCanvasDrawingRenderNodeTest, AddDirtyType, TestSize.Level1)
{
    NodeId nodeId = 1;
    std::weak_ptr<RSContext> context;
    RSCanvasDrawingRenderNode rsCanvasDrawingRenderNode(nodeId, context);
    Drawing::Matrix matrix;
    PropertyId id = 1;
    std::shared_ptr<RSRenderProperty<Drawing::Matrix>> property =
        std::make_shared<RSRenderProperty<Drawing::Matrix>>(matrix, id);
    std::shared_ptr<RSGeometryTransRenderModifier> modifierCast =
        std::make_shared<RSGeometryTransRenderModifier>(property);
    std::shared_ptr<RSRenderModifier> modifier = modifierCast;
    modifierCast->drawStyle_ = RSModifierType::BOUNDS;
    rsCanvasDrawingRenderNode.boundsModifier_ = modifier;
    rsCanvasDrawingRenderNode.AddGeometryModifier(modifier);
    rsCanvasDrawingRenderNode.AddDirtyType(RSModifierType::BLOOM);
    EXPECT_FALSE(rsCanvasDrawingRenderNode.isNeedProcess_);

    int32_t width = 1024;
    int32_t height = 2468;
    auto type = RSModifierType::CONTENT_STYLE;
    std::shared_ptr<Drawing::DrawCmdList> drawCmdList = std::make_shared<Drawing::DrawCmdList>(width, height);
    auto property1 = std::make_shared<RSRenderProperty<Drawing::DrawCmdListPtr>>();
    property1->GetRef() = drawCmdList;
    std::list<std::shared_ptr<RSRenderModifier>> listModifier { std::make_shared<RSDrawCmdListRenderModifier>(
        property1) };
    std::shared_ptr<RSDrawCmdListRenderModifier> modifier1;
    listModifier.emplace_back(modifier1);
    auto property2 = std::make_shared<RSRenderProperty<Drawing::DrawCmdListPtr>>();
    auto modifier2 = std::make_shared<RSDrawCmdListRenderModifier>(property2);
    listModifier.emplace_back(modifier2);
    rsCanvasDrawingRenderNode.renderContent_->drawCmdModifiers_.emplace(type, listModifier);
    std::list<Drawing::DrawCmdListPtr> listDrawCmd;
    auto listDrawCmdMax = 20;
    for (int i = 0; i < listDrawCmdMax; ++i) {
        listDrawCmd.emplace_back(std::make_shared<Drawing::DrawCmdList>(width + i, height + i));
    }
    rsCanvasDrawingRenderNode.drawCmdLists_.emplace(type, listDrawCmd);
    rsCanvasDrawingRenderNode.AddDirtyType(type);
    EXPECT_TRUE(rsCanvasDrawingRenderNode.isNeedProcess_);
    rsCanvasDrawingRenderNode.ClearOp();
    const auto& curDrawCmdLists = rsCanvasDrawingRenderNode.GetDrawCmdLists();
    EXPECT_TRUE(curDrawCmdLists.empty());
}

/**
 * @tc.name: ResetSurface
 * @tc.desc: test results of ResetSurface
 * @tc.type: FUNC
 * @tc.require: issueI9W0GK
 */
HWTEST_F(RSCanvasDrawingRenderNodeTest, ResetSurface, TestSize.Level1)
{
    NodeId nodeId = 1;
    std::weak_ptr<RSContext> context;
    int width = 1;
    int height = 1;
    RSCanvasDrawingRenderNode rsCanvasDrawingRenderNode(nodeId, context);
    rsCanvasDrawingRenderNode.stagingRenderParams_ = std::make_unique<RSRenderParams>(nodeId);
    rsCanvasDrawingRenderNode.ResetSurface(width, height);
    ASSERT_EQ(rsCanvasDrawingRenderNode.surface_, nullptr);

    rsCanvasDrawingRenderNode.surface_ = std::make_shared<Drawing::Surface>();
    std::function<void(std::shared_ptr<Drawing::Surface>)> callBackFunc = [](std::shared_ptr<Drawing::Surface>) {
        printf("ResetSurface preThreadInfo_ callBackFunc\n");
    };
    rsCanvasDrawingRenderNode.preThreadInfo_.second = callBackFunc;
    ASSERT_TRUE(rsCanvasDrawingRenderNode.preThreadInfo_.second && rsCanvasDrawingRenderNode.surface_);
    rsCanvasDrawingRenderNode.ResetSurface(width, height);
    ASSERT_EQ(rsCanvasDrawingRenderNode.surface_, nullptr);

#if (defined(RS_ENABLE_GL) || defined(RS_ENABLE_VK))
    Drawing::Canvas canvas(1024, 1096);
    RSPaintFilterCanvas paintCanvas(&canvas);
    EXPECT_TRUE(paintCanvas.GetGPUContext() == nullptr);
    auto ret = rsCanvasDrawingRenderNode.ResetSurface(canvas.GetWidth() + 10, canvas.GetHeight() + 10, paintCanvas);
    EXPECT_TRUE(ret);

    paintCanvas.canvas_->gpuContext_ = std::make_shared<Drawing::GPUContext>();
    EXPECT_TRUE(paintCanvas.GetGPUContext() != nullptr);
    ret = rsCanvasDrawingRenderNode.ResetSurface(canvas.GetWidth() + 20, canvas.GetHeight() + 20, paintCanvas);
    EXPECT_TRUE(ret);
#endif
}

/**
 * @tc.name: ResetSurfaceWithTextureTest
 * @tc.desc: Test ResetSurfaceWithTexture
 * @tc.type: FUNC
 * @tc.require: issueI9W0GK
 */
HWTEST_F(RSCanvasDrawingRenderNodeTest, ResetSurfaceWithTextureTest, TestSize.Level1)
{
#if (defined(RS_ENABLE_GL) || defined(RS_ENABLE_VK))
    NodeId nodeId = 2;
    auto rsCanvasDrawingRenderNode = std::make_shared<RSCanvasDrawingRenderNode>(nodeId);
    rsCanvasDrawingRenderNode->surface_ = std::make_shared<Drawing::Surface>();
    int width = 1024;
    int height = 1920;
    Drawing::Canvas canvas(width, height);
    RSPaintFilterCanvas paintCanvas(&canvas);
    auto ret = rsCanvasDrawingRenderNode->ResetSurface(width + 10, height + 10, paintCanvas);
    EXPECT_TRUE(ret);
    auto preSurface = rsCanvasDrawingRenderNode->surface_;
    ret = rsCanvasDrawingRenderNode->ResetSurfaceWithTexture(width, height, paintCanvas);
    EXPECT_FALSE(ret);
#endif
}

/**
 * @tc.name: IsNeedProcessTest
 * @tc.desc: Test IsNeedProcess
 * @tc.type: FUNC
 * @tc.require: issueI9W0GK
 */
HWTEST_F(RSCanvasDrawingRenderNodeTest, IsNeedProcessTest, TestSize.Level1)
{
    NodeId nodeId = 3;
    auto rsCanvasDrawingRenderNode = std::make_shared<RSCanvasDrawingRenderNode>(nodeId);
    std::shared_ptr<const RSRenderNode> otherNode = std::make_shared<const RSRenderNode>(nodeId + 1);
    rsCanvasDrawingRenderNode->renderDrawable_ =
        std::make_shared<RSCanvasDrawingRenderNodeDrawableAdapterTest>(otherNode);
    rsCanvasDrawingRenderNode->renderDrawable_->renderParams_ = std::make_unique<RSRenderParams>(nodeId);
    EXPECT_FALSE(rsCanvasDrawingRenderNode->IsNeedProcess());
}

/**
 * @tc.name: SetNeedProcessTest
 * @tc.desc: Test SetNeedProcess
 * @tc.type: FUNC
 * @tc.require: issueI9W0GK
 */
HWTEST_F(RSCanvasDrawingRenderNodeTest, SetNeedProcessTest, TestSize.Level1)
{
    NodeId nodeId = 4;
    auto rsCanvasDrawingRenderNode = std::make_shared<RSCanvasDrawingRenderNode>(nodeId);
    rsCanvasDrawingRenderNode->stagingRenderParams_ = std::make_unique<RSRenderParams>(nodeId);
    bool needProcess = true;
    rsCanvasDrawingRenderNode->SetNeedProcess(needProcess);
    EXPECT_FALSE(rsCanvasDrawingRenderNode->stagingRenderParams_->NeedSync());
    EXPECT_TRUE(rsCanvasDrawingRenderNode->isNeedProcess_);
}

/**
 * @tc.name: GetTidTest
 * @tc.desc: Test GetTid
 * @tc.type: FUNC
 * @tc.require: issueI9W0GK
 */
HWTEST_F(RSCanvasDrawingRenderNodeTest, GetTidTest, TestSize.Level1)
{
    NodeId nodeId = 5;
    auto rsCanvasDrawingRenderNode = std::make_shared<RSCanvasDrawingRenderNode>(nodeId);
    EXPECT_TRUE(rsCanvasDrawingRenderNode->drawingNodeRenderID != UNI_RENDER_THREAD_INDEX);
    rsCanvasDrawingRenderNode->GetTid();

    rsCanvasDrawingRenderNode->drawingNodeRenderID = UNI_RENDER_THREAD_INDEX;
    rsCanvasDrawingRenderNode->GetTid();
}

/**
 * @tc.name: GetImageTest
 * @tc.desc: Test GetImage
 * @tc.type: FUNC
 * @tc.require: issueI9W0GK
 */
HWTEST_F(RSCanvasDrawingRenderNodeTest, GetImageTest, TestSize.Level1)
{
    NodeId nodeId = 6;
    auto rsCanvasDrawingRenderNode = std::make_shared<RSCanvasDrawingRenderNode>(nodeId);
    uint64_t tid = 1;
    EXPECT_TRUE(rsCanvasDrawingRenderNode->GetTid() != tid);
    EXPECT_TRUE(rsCanvasDrawingRenderNode->GetImage(tid) == nullptr);
    tid = rsCanvasDrawingRenderNode->GetTid();
    EXPECT_TRUE(rsCanvasDrawingRenderNode->image_ == nullptr);
    EXPECT_TRUE(rsCanvasDrawingRenderNode->GetImage(tid) == nullptr);
}

/**
 * @tc.name: ClearResourceTest
 * @tc.desc: Test ClearResource
 * @tc.type: FUNC
 * @tc.require: issueI9W0GK
 */
HWTEST_F(RSCanvasDrawingRenderNodeTest, ClearResourceTest, TestSize.Level1)
{
    NodeId nodeId = 7;
    auto rsCanvasDrawingRenderNode = std::make_shared<RSCanvasDrawingRenderNode>(nodeId);
    rsCanvasDrawingRenderNode->ClearResource();
    auto lists = rsCanvasDrawingRenderNode->GetDrawCmdLists();
    EXPECT_TRUE(lists.empty());
}

/**
 * @tc.name: CheckCanvasDrawingPostPlaybacked
 * @tc.desc: Test CheckCanvasDrawingPostPlaybacked
 * @tc.type: FUNC
 * @tc.require: issueIB8OVD
 */
HWTEST_F(RSCanvasDrawingRenderNodeTest, CheckCanvasDrawingPostPlaybackedTest, TestSize.Level1)
{
    NodeId nodeId = 7;
    auto rsCanvasDrawingRenderNode = std::make_shared<RSCanvasDrawingRenderNode>(nodeId);
    EXPECT_NE(rsCanvasDrawingRenderNode->GetTid(), 1);
    rsCanvasDrawingRenderNode->isPostPlaybacked_ = true;
    rsCanvasDrawingRenderNode->CheckCanvasDrawingPostPlaybacked();
}

/**
 * @tc.name: ContentStyleSlotUpdateTest
 * @tc.desc: Test ContentStyleSlotUpdateTest
 * @tc.type: FUNC
 * @tc.require: issueIB8OVD
 */
HWTEST_F(RSCanvasDrawingRenderNodeTest, ContentStyleSlotUpdateTest, TestSize.Level1)
{
    NodeId nodeId = 7;
    auto node = std::make_shared<RSCanvasDrawingRenderNode>(nodeId);
    EXPECT_NE(node, nullptr);

    node->waitSync_ = true;
    node->ContentStyleSlotUpdate();

    node->waitSync_ = false;
    node->isOnTheTree_ = true;
    node->ContentStyleSlotUpdate();

    node->waitSync_ = false;
    node->isOnTheTree_ = false;
    node->isNeverOnTree_ = true;
    node->ContentStyleSlotUpdate();

    node->waitSync_ = false;
    node->isOnTheTree_ = false;
    node->isNeverOnTree_ = false;
    node->isTextureExportNode_ = true;
    node->ContentStyleSlotUpdate();
}
} // namespace OHOS::Rosen