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
class RSCanvasRenderNodeTest : public testing::Test {
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

void RSCanvasRenderNodeTest::SetUpTestCase()
{
    canvas_ = new RSPaintFilterCanvas(&drawingCanvas_);
}
void RSCanvasRenderNodeTest::TearDownTestCase()
{
    if (canvas_) {
        delete canvas_;
        canvas_ = nullptr;
    }
}
void RSCanvasRenderNodeTest::SetUp() {}
void RSCanvasRenderNodeTest::TearDown() {}

/**
 * @tc.name: ProcessRenderContentsTest
 * @tc.desc: test results of ProcessRenderContents
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSCanvasRenderNodeTest, ProcessRenderContentsTest, TestSize.Level1)
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
HWTEST_F(RSCanvasRenderNodeTest, ProcessRenderContentsOtherTest, TestSize.Level1)
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
 * @tc.name: ResetSurfaceTest
 * @tc.desc: test results of ResetSurface
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSCanvasRenderNodeTest, ResetSurfaceTest, TestSize.Level1)
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
 * @tc.name: GetSizeFromDrawCmdModifiersTest001
 * @tc.desc: test results of GetSizeFromDrawCmdModifiers
 * @tc.type: FUNC
 * @tc.require: issueI9W0GK
 */
HWTEST_F(RSCanvasRenderNodeTest, GetSizeFromDrawCmdModifiersTest001, TestSize.Level1)
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
HWTEST_F(RSCanvasRenderNodeTest, GetSizeFromDrawCmdModifiersTest002, TestSize.Level1)
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
HWTEST_F(RSCanvasRenderNodeTest, GetSizeFromDrawCmdModifiersTest003, TestSize.Level1)
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
    EXPECT_EQ(width, 0);
    EXPECT_EQ(height, 0);
}
/**
 * @tc.name: IsNeedResetSurfaceTest
 * @tc.desc: test results of IsNeedResetSurface
 * @tc.type: FUNC
 * @tc.require: issueI9W0GK
 */
HWTEST_F(RSCanvasRenderNodeTest, IsNeedResetSurfaceTest, TestSize.Level1)
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
HWTEST_F(RSCanvasRenderNodeTest, InitRenderParamsTest, TestSize.Level1)
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
HWTEST_F(RSCanvasRenderNodeTest, PlaybackInCorrespondThread, TestSize.Level1)
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
    NodeId canvasNodeId = rsCanvasDrawingRenderNode->GetId();
    pid_t canvasNodePid = ExtractPid(canvasNodeId);
    ctxNodeMap.renderNodeMap_[canvasNodePid][canvasNodeId] = rsCanvasDrawingRenderNode;
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
HWTEST_F(RSCanvasRenderNodeTest, ApplyDrawCmdModifier, TestSize.Level1)
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
HWTEST_F(RSCanvasRenderNodeTest, GetBitmap, TestSize.Level1)
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
HWTEST_F(RSCanvasRenderNodeTest, GetPixelmap, TestSize.Level1)
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
HWTEST_F(RSCanvasRenderNodeTest, GetPixelmapSurfaceImgValidTest, TestSize.Level1)
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
HWTEST_F(RSCanvasRenderNodeTest, AddDirtyType, TestSize.Level1)
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
HWTEST_F(RSCanvasRenderNodeTest, ResetSurface, TestSize.Level1)
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
HWTEST_F(RSCanvasRenderNodeTest, ResetSurfaceWithTextureTest, TestSize.Level1)
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
HWTEST_F(RSCanvasRenderNodeTest, IsNeedProcessTest, TestSize.Level1)
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
HWTEST_F(RSCanvasRenderNodeTest, SetNeedProcessTest, TestSize.Level1)
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
HWTEST_F(RSCanvasRenderNodeTest, GetTidTest, TestSize.Level1)
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
HWTEST_F(RSCanvasRenderNodeTest, GetImageTest, TestSize.Level1)
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
HWTEST_F(RSCanvasRenderNodeTest, ClearResourceTest, TestSize.Level1)
{
    NodeId nodeId = 7;
    auto rsCanvasDrawingRenderNode = std::make_shared<RSCanvasDrawingRenderNode>(nodeId);
    auto drawable = DrawableV2::RSRenderNodeDrawableAdapter::OnGenerate(rsCanvasDrawingRenderNode);
    if (!drawable) {
        return;
    }
    EXPECT_FALSE(drawable == nullptr);
    rsCanvasDrawingRenderNode->ClearResource();
    auto lists = rsCanvasDrawingRenderNode->GetDrawCmdLists();
    EXPECT_TRUE(lists.empty());
}

/**
 * @tc.name: SetandGetTranslateY005
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSCanvasRenderNodeTest, SetandGetTranslateY005, TestSize.Level1)
{
    RSCanvasNode::SharedPtr canvasNode = RSCanvasNode::Create();
    canvasNode->SetTranslateY(floatData[0]);
    EXPECT_TRUE(ROSEN_EQ(canvasNode->GetStagingProperties().GetTranslate().y_, floatData[0]));
}

/**
 * @tc.name: SetandGetTranslateZ001
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSCanvasRenderNodeTest, SetandGetTranslateZ001, TestSize.Level1)
{
    RSCanvasNode::SharedPtr canvasNode = RSCanvasNode::Create();
    canvasNode->SetTranslateZ(floatData[1]);
    EXPECT_TRUE(ROSEN_EQ(canvasNode->GetStagingProperties().GetTranslateZ(), floatData[1]));
}

/**
 * @tc.name: SetandGetTranslateZ002
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSCanvasRenderNodeTest, SetandGetTranslateZ002, TestSize.Level1)
{
    RSCanvasNode::SharedPtr canvasNode = RSCanvasNode::Create();
    canvasNode->SetTranslateZ(floatData[2]);
    EXPECT_TRUE(ROSEN_EQ(canvasNode->GetStagingProperties().GetTranslateZ(), floatData[2]));
}

/**
 * @tc.name: SetandGetTranslateZ003
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSCanvasRenderNodeTest, SetandGetTranslateZ003, TestSize.Level1)
{
    RSCanvasNode::SharedPtr canvasNode = RSCanvasNode::Create();
    canvasNode->SetTranslateZ(floatData[3]);
    EXPECT_TRUE(ROSEN_EQ(canvasNode->GetStagingProperties().GetTranslateZ(), floatData[3]));
}

/**
 * @tc.name: SetandGetTranslateZ004
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSCanvasRenderNodeTest, SetandGetTranslateZ004, TestSize.Level1)
{
    RSCanvasNode::SharedPtr canvasNode = RSCanvasNode::Create();
    canvasNode->SetTranslateZ(floatData[4]);
    EXPECT_TRUE(ROSEN_EQ(canvasNode->GetStagingProperties().GetTranslateZ(), floatData[4]));
}

/**
 * @tc.name: SetandGetTranslateZ005
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSCanvasRenderNodeTest, SetandGetTranslateZ005, TestSize.Level1)
{
    RSCanvasNode::SharedPtr canvasNode = RSCanvasNode::Create();
    canvasNode->SetTranslateZ(floatData[0]);
    EXPECT_TRUE(ROSEN_EQ(canvasNode->GetStagingProperties().GetTranslateZ(), floatData[0]));
}

/**
 * @tc.name: SetandGetClipToBounds001
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSCanvasRenderNodeTest, SetandGetClipToBounds001, TestSize.Level1)
{
    RSCanvasNode::SharedPtr canvasNode = RSCanvasNode::Create();
    canvasNode->SetClipToBounds(true);
    EXPECT_EQ(canvasNode->GetStagingProperties().GetClipToBounds(), true);
}

/**
 * @tc.name: SetandGetClipToBounds002
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSCanvasRenderNodeTest, SetandGetClipToBounds002, TestSize.Level1)
{
    RSCanvasNode::SharedPtr canvasNode = RSCanvasNode::Create();
    canvasNode->SetClipToBounds(false);
    EXPECT_EQ(canvasNode->GetStagingProperties().GetClipToBounds(), false);
}

/**
 * @tc.name: SetandGetClipToFrame001
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSCanvasRenderNodeTest, SetandGetClipToFrame001, TestSize.Level1)
{
    RSCanvasNode::SharedPtr canvasNode = RSCanvasNode::Create();
    canvasNode->SetClipToFrame(true);
    EXPECT_EQ(canvasNode->GetStagingProperties().GetClipToFrame(), true);
}

/**
 * @tc.name: SetandGetClipToFrame002
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSCanvasRenderNodeTest, SetandGetClipToFrame002, TestSize.Level1)
{
    RSCanvasNode::SharedPtr canvasNode = RSCanvasNode::Create();
    canvasNode->SetClipToFrame(false);
    EXPECT_EQ(canvasNode->GetStagingProperties().GetClipToFrame(), false);
}

/**
 * @tc.name: SetandGetVisible001
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSCanvasRenderNodeTest, SetandGetVisible001, TestSize.Level1)
{
    RSCanvasNode::SharedPtr canvasNode = RSCanvasNode::Create();
    canvasNode->SetVisible(true);
    EXPECT_EQ(canvasNode->GetStagingProperties().GetVisible(), true);
}

/**
 * @tc.name: SetandGetVisible002
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSCanvasRenderNodeTest, SetandGetVisible002, TestSize.Level1)
{
    RSCanvasNode::SharedPtr canvasNode = RSCanvasNode::Create();
    canvasNode->SetVisible(false);
    EXPECT_EQ(canvasNode->GetStagingProperties().GetVisible(), false);
}

/**
 * @tc.name: SetandGetBorderStyle001
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSCanvasRenderNodeTest, SetandGetBorderStyle001, TestSize.Level1)
{
    RSCanvasNode::SharedPtr canvasNode = RSCanvasNode::Create();
    auto borderStyle = static_cast<uint32_t>(BorderStyle::SOLID);
    canvasNode->SetBorderStyle(static_cast<uint32_t>(borderStyle));
    EXPECT_TRUE(canvasNode->GetStagingProperties().GetBorderStyle() == borderStyle);
}

/**
 * @tc.name: SetandGetBorderStyle002
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSCanvasRenderNodeTest, SetandGetBorderStyle002, TestSize.Level1)
{
    RSCanvasNode::SharedPtr canvasNode = RSCanvasNode::Create();
    auto borderStyle = static_cast<uint32_t>(BorderStyle::DASHED);
    canvasNode->SetBorderStyle(static_cast<uint32_t>(borderStyle));
    EXPECT_TRUE(canvasNode->GetStagingProperties().GetBorderStyle() == borderStyle);
}

/**
 * @tc.name: SetandGetBorderStyle003
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSCanvasRenderNodeTest, SetandGetBorderStyle003, TestSize.Level1)
{
    RSCanvasNode::SharedPtr canvasNode = RSCanvasNode::Create();
    auto borderStyle = static_cast<uint32_t>(BorderStyle::DOTTED);
    canvasNode->SetBorderStyle(static_cast<uint32_t>(borderStyle));
    EXPECT_TRUE(canvasNode->GetStagingProperties().GetBorderStyle() == borderStyle);
}

/**
 * @tc.name: SetandGetBorderStyle004
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSCanvasRenderNodeTest, SetandGetBorderStyle004, TestSize.Level1)
{
    RSCanvasNode::SharedPtr canvasNode = RSCanvasNode::Create();
    auto borderStyle = static_cast<uint32_t>(BorderStyle::NONE);
    canvasNode->SetBorderStyle(static_cast<uint32_t>(borderStyle));
    EXPECT_TRUE(canvasNode->GetStagingProperties().GetBorderStyle() == borderStyle);
}

/**
 * @tc.name: SetandGetFrameGravity001
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSCanvasRenderNodeTest, SetandGetFrameGravity001, TestSize.Level1)
{
    RSCanvasNode::SharedPtr canvasNode = RSCanvasNode::Create();
    Gravity gravity = Gravity::CENTER;
    canvasNode->SetFrameGravity(gravity);
    EXPECT_TRUE(canvasNode->GetStagingProperties().GetFrameGravity() == gravity);
}

/**
 * @tc.name: SetandGetFrameGravity002
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSCanvasRenderNodeTest, SetandGetFrameGravity002, TestSize.Level1)
{
    RSCanvasNode::SharedPtr canvasNode = RSCanvasNode::Create();
    Gravity gravity = Gravity::TOP;
    canvasNode->SetFrameGravity(gravity);
    EXPECT_TRUE(canvasNode->GetStagingProperties().GetFrameGravity() == gravity);
}

/**
 * @tc.name: SetandGetFrameGravity003
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSCanvasRenderNodeTest, SetandGetFrameGravity003, TestSize.Level1)
{
    RSCanvasNode::SharedPtr canvasNode = RSCanvasNode::Create();
    Gravity gravity = Gravity::BOTTOM;
    canvasNode->SetFrameGravity(gravity);
    EXPECT_TRUE(canvasNode->GetStagingProperties().GetFrameGravity() == gravity);
}

/**
 * @tc.name: SetandGetFrameGravity004
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSCanvasRenderNodeTest, SetandGetFrameGravity004, TestSize.Level1)
{
    RSCanvasNode::SharedPtr canvasNode = RSCanvasNode::Create();
    Gravity gravity = Gravity::LEFT;
    canvasNode->SetFrameGravity(gravity);
    EXPECT_TRUE(canvasNode->GetStagingProperties().GetFrameGravity() == gravity);
}

/**
 * @tc.name: SetandGetFrameGravity005
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSCanvasRenderNodeTest, SetandGetFrameGravity005, TestSize.Level1)
{
    RSCanvasNode::SharedPtr canvasNode = RSCanvasNode::Create();
    Gravity gravity = Gravity::RIGHT;
    canvasNode->SetFrameGravity(gravity);
    EXPECT_TRUE(canvasNode->GetStagingProperties().GetFrameGravity() == gravity);
}

/**
 * @tc.name: SetandGetFrameGravity006
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSCanvasRenderNodeTest, SetandGetFrameGravity006, TestSize.Level1)
{
    RSCanvasNode::SharedPtr canvasNode = RSCanvasNode::Create();
    Gravity gravity = Gravity::TOP_LEFT;
    canvasNode->SetFrameGravity(gravity);
    EXPECT_TRUE(canvasNode->GetStagingProperties().GetFrameGravity() == gravity);
}

/**
 * @tc.name: SetandGetFrameGravity007
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSCanvasRenderNodeTest, SetandGetFrameGravity007, TestSize.Level1)
{
    RSCanvasNode::SharedPtr canvasNode = RSCanvasNode::Create();
    Gravity gravity = Gravity::TOP_RIGHT;
    canvasNode->SetFrameGravity(gravity);
    EXPECT_TRUE(canvasNode->GetStagingProperties().GetFrameGravity() == gravity);
}

/**
 * @tc.name: SetandGetFrameGravity008
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSCanvasRenderNodeTest, SetandGetFrameGravity008, TestSize.Level1)
{
    RSCanvasNode::SharedPtr canvasNode = RSCanvasNode::Create();
    Gravity gravity = Gravity::BOTTOM_LEFT;
    canvasNode->SetFrameGravity(gravity);
    EXPECT_TRUE(canvasNode->GetStagingProperties().GetFrameGravity() == gravity);
}

/**
 * @tc.name: SetandGetFrameGravity009
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSCanvasRenderNodeTest, SetandGetFrameGravity009, TestSize.Level1)
{
    RSCanvasNode::SharedPtr canvasNode = RSCanvasNode::Create();
    Gravity gravity = Gravity::BOTTOM_RIGHT;
    canvasNode->SetFrameGravity(gravity);
    EXPECT_TRUE(canvasNode->GetStagingProperties().GetFrameGravity() == gravity);
}

/**
 * @tc.name: SetandGetFrameGravity010
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSCanvasRenderNodeTest, SetandGetFrameGravity010, TestSize.Level1)
{
    RSCanvasNode::SharedPtr canvasNode = RSCanvasNode::Create();
    Gravity gravity = Gravity::RESIZE;
    canvasNode->SetFrameGravity(gravity);
    EXPECT_TRUE(canvasNode->GetStagingProperties().GetFrameGravity() == gravity);
}

/**
 * @tc.name: SetandGetFrameGravity011
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSCanvasRenderNodeTest, SetandGetFrameGravity011, TestSize.Level1)
{
    RSCanvasNode::SharedPtr canvasNode = RSCanvasNode::Create();
    Gravity gravity = Gravity::RESIZE_ASPECT;
    canvasNode->SetFrameGravity(gravity);
    EXPECT_TRUE(canvasNode->GetStagingProperties().GetFrameGravity() == gravity);
}

/**
 * @tc.name: SetandGetFrameGravity012
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSCanvasRenderNodeTest, SetandGetFrameGravity012, TestSize.Level1)
{
    RSCanvasNode::SharedPtr canvasNode = RSCanvasNode::Create();
    Gravity gravity = Gravity::RESIZE_ASPECT_FILL;
    canvasNode->SetFrameGravity(gravity);
    EXPECT_TRUE(canvasNode->GetStagingProperties().GetFrameGravity() == gravity);
}

/**
 * @tc.name: NotifyTransition001
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSCanvasRenderNodeTest, NotifyTransition001, TestSize.Level1)
{
    RSCanvasNode::SharedPtr canvasNode = RSCanvasNode::Create();
    canvasNode->NotifyTransition(RSTransitionEffect::Create(), true);
}

/**
 * @tc.name: NotifyTransition002
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSCanvasRenderNodeTest, NotifyTransition002, TestSize.Level1)
{
    RSCanvasNode::SharedPtr canvasNode = RSCanvasNode::Create();
    canvasNode->NotifyTransition(RSTransitionEffect::Create(), false);
}

/**
 * @tc.name: SetandGetForegroundColor001
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSCanvasRenderNodeTest, SetandGetForegroundColor001, TestSize.Level1)
{
    RSCanvasNode::SharedPtr canvasNode = RSCanvasNode::Create();
    constexpr uint32_t colorValue = 0x034123;
    canvasNode->SetForegroundColor(colorValue);
    EXPECT_TRUE(canvasNode->GetStagingProperties().GetForegroundColor() == Color::FromArgbInt(colorValue));
}

/**
 * @tc.name: SetandGetForegroundColor002
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSCanvasRenderNodeTest, SetandGetForegroundColor002, TestSize.Level1)
{
    RSCanvasNode::SharedPtr canvasNode = RSCanvasNode::Create();
    constexpr uint32_t colorValue = std::numeric_limits<uint32_t>::max();
    canvasNode->SetForegroundColor(colorValue);
    EXPECT_TRUE(canvasNode->GetStagingProperties().GetForegroundColor() == Color::FromArgbInt(colorValue));
}

/**
 * @tc.name: SetandGetForegroundColor003
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSCanvasRenderNodeTest, SetandGetForegroundColor003, TestSize.Level1)
{
    RSCanvasNode::SharedPtr canvasNode = RSCanvasNode::Create();
    constexpr uint32_t colorValue = std::numeric_limits<uint32_t>::min();
    canvasNode->SetForegroundColor(colorValue);
    EXPECT_TRUE(canvasNode->GetStagingProperties().GetForegroundColor() == Color::FromArgbInt(colorValue));
}

/**
 * @tc.name: SetandGetBackgroundColor001
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSCanvasRenderNodeTest, SetandGetBackgroundColor001, TestSize.Level1)
{
    RSCanvasNode::SharedPtr canvasNode = RSCanvasNode::Create();
    constexpr uint32_t colorValue = 0x034123;
    canvasNode->SetBackgroundColor(colorValue);
    EXPECT_TRUE(canvasNode->GetStagingProperties().GetBackgroundColor() == Color::FromArgbInt(colorValue));
}

/**
 * @tc.name: SetandGetBackgroundColor002
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSCanvasRenderNodeTest, SetandGetBackgroundColor002, TestSize.Level1)
{
    RSCanvasNode::SharedPtr canvasNode = RSCanvasNode::Create();
    constexpr uint32_t colorValue = std::numeric_limits<uint32_t>::max();
    canvasNode->SetBackgroundColor(colorValue);
    EXPECT_TRUE(canvasNode->GetStagingProperties().GetBackgroundColor() == Color::FromArgbInt(colorValue));
}

/**
 * @tc.name: SetandGetBackgroundColor003
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSCanvasRenderNodeTest, SetandGetBackgroundColor003, TestSize.Level1)
{
    RSCanvasNode::SharedPtr canvasNode = RSCanvasNode::Create();
    constexpr uint32_t colorValue = std::numeric_limits<uint32_t>::min();
    canvasNode->SetBackgroundColor(colorValue);
    EXPECT_TRUE(canvasNode->GetStagingProperties().GetBackgroundColor() == Color::FromArgbInt(colorValue));
}

/**
 * @tc.name: SetandGetBorderColor001
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSCanvasRenderNodeTest, SetandGetBorderColor001, TestSize.Level1)
{
    RSCanvasNode::SharedPtr canvasNode = RSCanvasNode::Create();
    constexpr uint32_t colorValue = 0x034123;
    canvasNode->SetBorderColor(colorValue);
    EXPECT_TRUE(canvasNode->GetStagingProperties().GetBorderColor() == Color::FromArgbInt(colorValue));
}

/**
 * @tc.name: SetandGetBorderColor002
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSCanvasRenderNodeTest, SetandGetBorderColor002, TestSize.Level1)
{
    RSCanvasNode::SharedPtr canvasNode = RSCanvasNode::Create();
    constexpr uint32_t colorValue = std::numeric_limits<uint32_t>::max();
    canvasNode->SetBorderColor(colorValue);
    EXPECT_TRUE(canvasNode->GetStagingProperties().GetBorderColor() == Color::FromArgbInt(colorValue));
}

/**
 * @tc.name: SetandGetBorderColor003
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSCanvasRenderNodeTest, SetandGetBorderColor003, TestSize.Level1)
{
    RSCanvasNode::SharedPtr canvasNode = RSCanvasNode::Create();
    constexpr uint32_t colorValue = std::numeric_limits<uint32_t>::min();
    canvasNode->SetBorderColor(colorValue);
    EXPECT_TRUE(canvasNode->GetStagingProperties().GetBorderColor() == Color::FromArgbInt(colorValue));
}

/**
 * @tc.name: SetandGetRotationVector001
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSCanvasRenderNodeTest, SetandGetRotationVector001, TestSize.Level1)
{
    RSCanvasNode::SharedPtr canvasNode = RSCanvasNode::Create();
    Vector4f quaternion(std::numeric_limits<int>::min(), 2.f, 3.f, 4.f);
    canvasNode->SetRotation(quaternion);
}

/**
 * @tc.name: SetandGetTranslateVector001
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSCanvasRenderNodeTest, SetandGetTranslateVector001, TestSize.Level1)
{
    RSCanvasNode::SharedPtr canvasNode = RSCanvasNode::Create();
    Vector2f quaternion(std::numeric_limits<int>::max(), 2.f);
    canvasNode->SetTranslate(quaternion);
}

/**
 * @tc.name: CreateBlurFilter001
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSCanvasRenderNodeTest, CreateBlurFilter001, TestSize.Level1)
{
    RSCanvasNode::SharedPtr canvasNode = RSCanvasNode::Create();
    std::shared_ptr<RSFilter> backgroundFilter = RSFilter::CreateBlurFilter(floatData[0], floatData[1]);
    canvasNode->SetBackgroundFilter(backgroundFilter);
    EXPECT_TRUE(canvasNode->GetStagingProperties().GetBackgroundBlurRadiusX() == floatData[0]);
    EXPECT_TRUE(canvasNode->GetStagingProperties().GetBackgroundBlurRadiusY() == floatData[1]);
}

/**
 * @tc.name: CreateBlurFilter002
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSCanvasRenderNodeTest, CreateBlurFilter002, TestSize.Level2)
{
    RSCanvasNode::SharedPtr canvasNode = RSCanvasNode::Create();
    std::shared_ptr<RSFilter> backgroundFilter = RSFilter::CreateBlurFilter(floatData[1], floatData[2]);
    canvasNode->SetBackgroundFilter(backgroundFilter);
    EXPECT_TRUE(canvasNode->GetStagingProperties().GetBackgroundBlurRadiusX() == floatData[1]);
    EXPECT_TRUE(canvasNode->GetStagingProperties().GetBackgroundBlurRadiusY() == floatData[2]);
}

/**
 * @tc.name: CreateBlurFilter003
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSCanvasRenderNodeTest, CreateBlurFilter003, TestSize.Level3)
{
    RSCanvasNode::SharedPtr canvasNode = RSCanvasNode::Create();
    std::shared_ptr<RSFilter> backgroundFilter = RSFilter::CreateBlurFilter(floatData[2], floatData[3]);
    canvasNode->SetBackgroundFilter(backgroundFilter);
    EXPECT_TRUE(canvasNode->GetStagingProperties().GetBackgroundBlurRadiusX() == floatData[2]);
    EXPECT_TRUE(canvasNode->GetStagingProperties().GetBackgroundBlurRadiusY() == floatData[3]);
}

/**
 * @tc.name: CreateBlurFilter004
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSCanvasRenderNodeTest, CreateBlurFilter004, TestSize.Level1)
{
    RSCanvasNode::SharedPtr canvasNode = RSCanvasNode::Create();
    std::shared_ptr<RSFilter> backgroundFilter = RSFilter::CreateBlurFilter(floatData[3], floatData[4]);
    canvasNode->SetBackgroundFilter(backgroundFilter);
    EXPECT_TRUE(canvasNode->GetStagingProperties().GetBackgroundBlurRadiusX() == floatData[3]);
    EXPECT_TRUE(canvasNode->GetStagingProperties().GetBackgroundBlurRadiusY() == floatData[4]);
}

/**
 * @tc.name: CreateBlurFilter005
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSCanvasRenderNodeTest, CreateBlurFilter005, TestSize.Level1)
{
    RSCanvasNode::SharedPtr canvasNode = RSCanvasNode::Create();
    std::shared_ptr<RSFilter> backgroundFilter = RSFilter::CreateBlurFilter(floatData[4], floatData[0]);
    canvasNode->SetBackgroundFilter(backgroundFilter);
    EXPECT_TRUE(canvasNode->GetStagingProperties().GetBackgroundBlurRadiusX() == floatData[4]);
    EXPECT_TRUE(canvasNode->GetStagingProperties().GetBackgroundBlurRadiusY() == floatData[0]);
}

/**
 * @tc.name: CreateNormalFilter001
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSCanvasRenderNodeTest, CreateNormalFilter001, TestSize.Level1)
{
    RSCanvasNode::SharedPtr canvasNode = RSCanvasNode::Create();
    std::shared_ptr<RSFilter> filter = RSFilter::CreateBlurFilter(floatData[0], floatData[1]);
    canvasNode->SetFilter(filter);
    EXPECT_TRUE(canvasNode->GetStagingProperties().GetForegroundBlurRadiusX() == floatData[0]);
    EXPECT_TRUE(canvasNode->GetStagingProperties().GetForegroundBlurRadiusY() == floatData[1]);
}

/**
 * @tc.name: CreateNormalFilter002
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSCanvasRenderNodeTest, CreateNormalFilter002, TestSize.Level2)
{
    RSCanvasNode::SharedPtr canvasNode = RSCanvasNode::Create();
    std::shared_ptr<RSFilter> filter = RSFilter::CreateBlurFilter(floatData[1], floatData[2]);
    canvasNode->SetFilter(filter);
    EXPECT_TRUE(canvasNode->GetStagingProperties().GetForegroundBlurRadiusX() == floatData[1]);
    EXPECT_TRUE(canvasNode->GetStagingProperties().GetForegroundBlurRadiusY() == floatData[2]);
}

/**
 * @tc.name: CreateNormalFilter003
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSCanvasRenderNodeTest, CreateNormalFilter003, TestSize.Level3)
{
    RSCanvasNode::SharedPtr canvasNode = RSCanvasNode::Create();
    std::shared_ptr<RSFilter> filter = RSFilter::CreateBlurFilter(floatData[2], floatData[3]);
    canvasNode->SetFilter(filter);
    EXPECT_TRUE(canvasNode->GetStagingProperties().GetForegroundBlurRadiusX() == floatData[2]);
    EXPECT_TRUE(canvasNode->GetStagingProperties().GetForegroundBlurRadiusY() == floatData[3]);
}

/**
 * @tc.name: CreateNormalFilter004
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSCanvasRenderNodeTest, CreateNormalFilter004, TestSize.Level1)
{
    RSCanvasNode::SharedPtr canvasNode = RSCanvasNode::Create();
    std::shared_ptr<RSFilter> filter = RSFilter::CreateBlurFilter(floatData[3], floatData[4]);
    canvasNode->SetFilter(filter);
    EXPECT_TRUE(canvasNode->GetStagingProperties().GetForegroundBlurRadiusX() == floatData[3]);
    EXPECT_TRUE(canvasNode->GetStagingProperties().GetForegroundBlurRadiusY() == floatData[4]);
}

/**
 * @tc.name: CreateNormalFilter005
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSCanvasRenderNodeTest, CreateNormalFilter005, TestSize.Level1)
{
    RSCanvasNode::SharedPtr canvasNode = RSCanvasNode::Create();
    std::shared_ptr<RSFilter> filter = RSFilter::CreateBlurFilter(floatData[4], floatData[0]);
    canvasNode->SetFilter(filter);
    EXPECT_TRUE(canvasNode->GetStagingProperties().GetForegroundBlurRadiusX() == floatData[4]);
    EXPECT_TRUE(canvasNode->GetStagingProperties().GetForegroundBlurRadiusY() == floatData[0]);
}

/**
 * @tc.name: SetandGetClipBounds001
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSCanvasRenderNodeTest, SetandGetClipBounds001, TestSize.Level1)
{
    RSCanvasNode::SharedPtr canvasNode = RSCanvasNode::Create();
    std::shared_ptr<RSPath> clipPath = RSPath::CreateRSPath();
    canvasNode->SetClipBounds(clipPath);
    EXPECT_TRUE(canvasNode->GetStagingProperties().GetClipBounds() == clipPath);
}

/**
 * @tc.name: GetId001
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSCanvasRenderNodeTest, GetId001, TestSize.Level1)
{
    RSCanvasNode::SharedPtr canvasNode = RSCanvasNode::Create();
    canvasNode->GetId();
}

/**
 * @tc.name: GetStagingProperties001
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSCanvasRenderNodeTest, GetStagingProperties001, TestSize.Level1)
{
    RSCanvasNode::SharedPtr canvasNode = RSCanvasNode::Create();
    canvasNode->GetStagingProperties();
}

/**
 * @tc.name: GetMotionPathOption001
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSCanvasRenderNodeTest, GetMotionPathOption002, TestSize.Level1)
{
    RSCanvasNode::SharedPtr canvasNode = RSCanvasNode::Create();
    canvasNode->GetMotionPathOption();
}

/**
 * @tc.name: SetBgImage001
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSCanvasRenderNodeTest, SetBgImage001, TestSize.Level1)
{
    RSCanvasNode::SharedPtr canvasNode = RSCanvasNode::Create();
    auto image = std::make_shared<RSImage>();
    canvasNode->SetBgImage(image);
}

/**
 * @tc.name: SetBackgroundShader001
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSCanvasRenderNodeTest, SetBackgroundShader001, TestSize.Level1)
{
    RSCanvasNode::SharedPtr canvasNode = RSCanvasNode::Create();
    auto shader = RSShader::CreateRSShader();
    canvasNode->SetBackgroundShader(shader);
}

/**
 * @tc.name: SetCompositingFilter001
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSCanvasRenderNodeTest, SetCompositingFilter001, TestSize.Level1)
{
    RSCanvasNode::SharedPtr canvasNode = RSCanvasNode::Create();
    auto compositingFilter = RSFilter::CreateBlurFilter(0.0f, 0.0f);
    canvasNode->SetCompositingFilter(compositingFilter);
}

/**
 * @tc.name: SetShadowPath001
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSCanvasRenderNodeTest, SetShadowPath001, TestSize.Level1)
{
    RSCanvasNode::SharedPtr canvasNode = RSCanvasNode::Create();
    auto shadowpath = RSPath::CreateRSPath();
    canvasNode->SetShadowPath(shadowpath);
}

/**
 * @tc.name: GetType001
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSCanvasRenderNodeTest, GetType001, TestSize.Level1)
{
    RSCanvasNode::SharedPtr canvasNode = RSCanvasNode::Create();
    ASSERT_TRUE(canvasNode != nullptr);
    ASSERT_TRUE(canvasNode->GetType() == RSUINodeType::CANVAS_NODE);
}

/**
 * @tc.name: GetType002
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSCanvasRenderNodeTest, GetType002, TestSize.Level1)
{
    RSCanvasNode::SharedPtr canvasNode = RSCanvasNode::Create(true);
    ASSERT_TRUE(canvasNode != nullptr);
    ASSERT_TRUE(canvasNode->GetType() == RSUINodeType::CANVAS_NODE);
}

/**
 * @tc.name: GetType003
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSCanvasRenderNodeTest, GetType003, TestSize.Level1)
{
    RSCanvasNode::SharedPtr canvasNode = RSCanvasNode::Create(false);
    ASSERT_TRUE(canvasNode != nullptr);
    ASSERT_TRUE(canvasNode->GetType() == RSUINodeType::CANVAS_NODE);
}

/**
 * @tc.name: SetBoundsChangedCallbackTest
 * @tc.desc: test results of SetBoundsChangedCallback
 * @tc.type:FUNC
 * @tc.require:issueI9MWJR
 */
HWTEST_F(RSCanvasRenderNodeTest, SetBoundsChangedCallbackTest, TestSize.Level1)
{
    RSCanvasNode::SharedPtr canvasNode = RSCanvasNode::Create(false);
    ASSERT_TRUE(canvasNode != nullptr);
    canvasNode->DrawOnNode(RSModifierType::BOUNDS, [](std::shared_ptr<Drawing::Canvas>) {});
    ASSERT_FALSE(canvasNode->recordingUpdated_);
    canvasNode->SetHDRPresent(true);
    canvasNode->SetBoundsChangedCallback([](const Rosen::Vector4f& vector4f) {});
    ASSERT_NE(canvasNode->boundsChangedCallback_, nullptr);
}

/**
 * @tc.name: CreateRenderNodeForTextureExportSwitch
 * @tc.desc: test results of CreateRenderNodeForTextureExportSwitch
 * @tc.type: FUNC
 * @tc.require: issueI9KDPI
 */
HWTEST_F(RSCanvasRenderNodeTest, CreateRenderNodeForTextureExportSwitch, TestSize.Level1)
{
    RSCanvasNode::SharedPtr canvasNode = RSCanvasNode::Create();
    canvasNode->CreateRenderNodeForTextureExportSwitch();
    ASSERT_TRUE(RSTransactionProxy::instance_ != nullptr);
}

/**
 * @tc.name: DrawOnNode
 * @tc.desc: test results of DrawOnNode
 * @tc.type: FUNC
 * @tc.require: issueI9KDPI
 */
HWTEST_F(RSCanvasRenderNodeTest, DrawOnNode, TestSize.Level1)
{
    RSCanvasNode::SharedPtr canvasNode = RSCanvasNode::Create();
    RSModifierType type = RSModifierType::INVALID;
    DrawFunc func = [&](std::shared_ptr<Drawing::Canvas>) {};
    delete RSTransactionProxy::instance_;
    RSTransactionProxy::instance_ = nullptr;
    canvasNode->DrawOnNode(type, func);
    EXPECT_TRUE(RSTransactionProxy::instance_ == nullptr);

    RSTransactionProxy::instance_ = new RSTransactionProxy();
    canvasNode->DrawOnNode(type, func);
    EXPECT_TRUE(RSTransactionProxy::instance_ != nullptr);
}

/**
 * @tc.name: GetPaintWidth
 * @tc.desc: test results of GetPaintWidth
 * @tc.type: FUNC
 * @tc.require: issueI9KDPI
 */
HWTEST_F(RSCanvasRenderNodeTest, GetPaintWidth, TestSize.Level1)
{
    RSCanvasNode::SharedPtr canvasNode = RSCanvasNode::Create();
    float res = canvasNode->GetPaintWidth();
    EXPECT_FALSE(res == 1.f);
}

/**
 * @tc.name: GetPaintHeight
 * @tc.desc: test results of GetPaintHeight
 * @tc.type: FUNC
 * @tc.require: issueI9KDPI
 */
HWTEST_F(RSCanvasRenderNodeTest, GetPaintHeight, TestSize.Level1)
{
    RSCanvasNode::SharedPtr canvasNode = RSCanvasNode::Create();
    float res = canvasNode->GetPaintHeight();
    EXPECT_FALSE(res == 1.f);
}

/**
 * @tc.name: SetFreeze
 * @tc.desc: test results of SetFreeze
 * @tc.type: FUNC
 * @tc.require: issueI9KDPI
 */
HWTEST_F(RSCanvasRenderNodeTest, SetFreeze, TestSize.Level1)
{
    RSCanvasNode::SharedPtr canvasNode = RSCanvasNode::Create();
    canvasNode->SetFreeze(true);
    EXPECT_TRUE(RSTransactionProxy::instance_ != nullptr);
}

/**
 * @tc.name: OnBoundsSizeChanged
 * @tc.desc: test results of OnBoundsSizeChanged
 * @tc.type: FUNC
 * @tc.require: issueI9KDPI
 */
HWTEST_F(RSCanvasRenderNodeTest, OnBoundsSizeChanged, TestSize.Level1)
{
    RSCanvasNode::SharedPtr canvasNode = RSCanvasNode::Create();
    canvasNode->OnBoundsSizeChanged();
    EXPECT_TRUE(RSTransactionProxy::instance_ != nullptr);
}

/**
 * @tc.name: SetBoundsChangedCallback
 * @tc.desc: test results of SetBoundsChangedCallback
 * @tc.type: FUNC
 * @tc.require: issueI9KDPI
 */
HWTEST_F(RSCanvasRenderNodeTest, SetBoundsChangedCallback, TestSize.Level1)
{
    RSCanvasNode::SharedPtr canvasNode = RSCanvasNode::Create();
    canvasNode->boundsChangedCallback_ = [](const Rosen::Vector4f& bounds) {};
    canvasNode->SetBoundsChangedCallback(canvasNode->boundsChangedCallback_);
    EXPECT_TRUE(RSTransactionProxy::instance_ != nullptr);
}

/**
 * @tc.name: BeginRecording001
 * @tc.desc: test results of BeginRecording
 * @tc.type: FUNC
 * @tc.require: issueI9R0EY
 */
HWTEST_F(RSCanvasRenderNodeTest, BeginRecording001, TestSize.Level1)
{
    RSCanvasNode::SharedPtr canvasNode = RSCanvasNode::Create();
    canvasNode->BeginRecording(200, 300);
    EXPECT_TRUE(RSTransactionProxy::instance_ != nullptr);

    canvasNode->recordingUpdated_ = true;
    canvasNode->BeginRecording(200, 300);
    EXPECT_TRUE(!canvasNode->recordingUpdated_);

    delete RSTransactionProxy::instance_;
    RSTransactionProxy::instance_ = nullptr;
    canvasNode->BeginRecording(200, 300);
    EXPECT_TRUE(RSTransactionProxy::instance_ == nullptr);
    RSTransactionProxy::instance_ = new RSTransactionProxy();
    EXPECT_TRUE(RSTransactionProxy::instance_ != nullptr);
}

/**
 * @tc.name: CreateTextureExportRenderNodeInRT001
 * @tc.desc: test results of CreateRenderNodeForTextureExportSwitch
 * @tc.type: FUNC
 * @tc.require: issueI9R0EY
 */
HWTEST_F(RSCanvasRenderNodeTest, CreateTextureExportRenderNodeInRT001, TestSize.Level1)
{
    RSCanvasNode::SharedPtr canvasNode = RSCanvasNode::Create();
    canvasNode->CreateRenderNodeForTextureExportSwitch();
    EXPECT_TRUE(RSTransactionProxy::instance_ != nullptr);
}

/**
 * @tc.name: FinishRecording001
 * @tc.desc: test results of FinishRecording
 * @tc.type: FUNC
 * @tc.require: issueI9R0EY
 */
HWTEST_F(RSCanvasRenderNodeTest, FinishRecording001, TestSize.Level1)
{
    RSCanvasNode::SharedPtr canvasNode = RSCanvasNode::Create();
    canvasNode->FinishRecording();
    EXPECT_TRUE(RSTransactionProxy::instance_ != nullptr);

    canvasNode->BeginRecording(200, 300);
    canvasNode->recordingCanvas_->cmdList_ =
        std::make_shared<Drawing::DrawCmdList>(Drawing::DrawCmdList::UnmarshalMode::DEFERRED);
    canvasNode->FinishRecording();
    EXPECT_TRUE(RSTransactionProxy::instance_ != nullptr);

    canvasNode->BeginRecording(200, 300);
    canvasNode->recordingCanvas_->cmdList_ =
        std::make_shared<Drawing::DrawCmdList>(Drawing::DrawCmdList::UnmarshalMode::IMMEDIATE);
    Drawing::Paint paint;
    auto item = std::make_shared<Drawing::DrawWithPaintOpItem>(paint, 0);
    canvasNode->recordingCanvas_->cmdList_->drawOpItems_.push_back(item);
    canvasNode->FinishRecording();
    EXPECT_TRUE(canvasNode->recordingUpdated_);

    canvasNode->BeginRecording(200, 300);
    delete RSTransactionProxy::instance_;
    RSTransactionProxy::instance_ = nullptr;
    canvasNode->FinishRecording();
    EXPECT_TRUE(RSTransactionProxy::instance_ == nullptr);
    RSTransactionProxy::instance_ = new RSTransactionProxy();
    EXPECT_TRUE(RSTransactionProxy::instance_ != nullptr);

    canvasNode->BeginRecording(200, 300);
    canvasNode->recordingCanvas_->cmdList_ = nullptr;
    canvasNode->FinishRecording();
    EXPECT_TRUE(RSTransactionProxy::instance_ != nullptr);
}

/**
 * @tc.name: DrawOnNode001
 * @tc.desc: test results of DrawOnNode
 * @tc.type: FUNC
 * @tc.require: issueI9R0EY
 */
HWTEST_F(RSCanvasRenderNodeTest, DrawOnNode001, TestSize.Level1)
{
    RSCanvasNode::SharedPtr canvasNode = RSCanvasNode::Create();
    DrawFunc func = [&](std::shared_ptr<Drawing::Canvas>) {};
    canvasNode->DrawOnNode(RSModifierType::INVALID, func);
    EXPECT_TRUE(RSTransactionProxy::instance_ != nullptr);

    canvasNode->BeginRecording(200, 300);
    delete RSTransactionProxy::instance_;
    RSTransactionProxy::instance_ = nullptr;
    canvasNode->DrawOnNode(RSModifierType::INVALID, func);
    EXPECT_TRUE(RSTransactionProxy::instance_ == nullptr);
    RSTransactionProxy::instance_ = new RSTransactionProxy();
    EXPECT_TRUE(RSTransactionProxy::instance_ != nullptr);
}

/**
 * @tc.name: SetFreeze001
 * @tc.desc: test results of SetFreeze
 * @tc.type: FUNC
 * @tc.require: issueI9R0EY
 */
HWTEST_F(RSCanvasRenderNodeTest, SetFreeze001, TestSize.Level1)
{
    RSCanvasNode::SharedPtr canvasNode = RSCanvasNode::Create();
    canvasNode->SetFreeze(true);
    EXPECT_TRUE(RSTransactionProxy::instance_ != nullptr);

    RSSystemProperties::GetUniRenderEnabled();
    RSSystemProperties::isUniRenderEnabled_ = true;
    canvasNode->SetFreeze(true);
    ASSERT_TRUE(RSSystemProperties::isUniRenderEnabled_);

    delete RSTransactionProxy::instance_;
    RSTransactionProxy::instance_ = nullptr;
    canvasNode->SetFreeze(true);
    ASSERT_TRUE(RSSystemProperties::isUniRenderEnabled_);
    RSTransactionProxy::instance_ = new RSTransactionProxy();
}

/**
 * @tc.name: SetHDRPresent001
 * @tc.desc: test results of SetHDRPresent
 * @tc.type: FUNC
 * @tc.require: issueI9R0EY
 */
HWTEST_F(RSCanvasRenderNodeTest, SetHDRPresent001, TestSize.Level1)
{
    RSCanvasNode::SharedPtr canvasNode = RSCanvasNode::Create();
    canvasNode->SetHDRPresent(true);
    EXPECT_TRUE(RSTransactionProxy::instance_ != nullptr);

    canvasNode->SetHDRPresent(true);
    EXPECT_TRUE(RSTransactionProxy::instance_ != nullptr);

    canvasNode->SetHDRPresent(false);
    EXPECT_TRUE(RSTransactionProxy::instance_ != nullptr);

    canvasNode->SetHDRPresent(false);
    EXPECT_TRUE(RSTransactionProxy::instance_ != nullptr);
}

/**
 * @tc.name: OnBoundsSizeChanged001
 * @tc.desc: test results of OnBoundsSizeChanged
 * @tc.type: FUNC
 * @tc.require: issueI9R0EY
 */
HWTEST_F(RSCanvasRenderNodeTest, OnBoundsSizeChanged001, TestSize.Level1)
{
    RSCanvasNode::SharedPtr canvasNode = RSCanvasNode::Create();
    canvasNode->OnBoundsSizeChanged();
    EXPECT_TRUE(RSTransactionProxy::instance_ != nullptr);

    canvasNode->boundsChangedCallback_ = [](const Rosen::Vector4f& bounds) {};
    canvasNode->OnBoundsSizeChanged();
    EXPECT_TRUE(RSTransactionProxy::instance_ != nullptr);
}
} // namespace OHOS::Rosen
