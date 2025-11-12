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
#include "modifier_ng/rs_render_modifier_ng.h"
#include "params/rs_render_params.h"
#include "pipeline/rs_canvas_drawing_render_node.h"
#include "pipeline/rs_context.h"
#include "pipeline/rs_surface_render_node.h"
#include "property/rs_properties_painter.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Rosen {
constexpr size_t DRAWCMDLIST_DUMP_LIMIT = 10; // limit of the DrawCmdListDump.
constexpr size_t OP_DUMP_LIMIT_PER_CMD = 15; // limit of the DrawOpItemsDump.

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
    static inline NodeId nodeId_ = 0;
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

#ifndef MODIFIER_NG
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
#endif

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
    auto modifier = std::make_shared<ModifierNG::RSCustomRenderModifier<ModifierNG::RSModifierType::CONTENT_STYLE>>();
    modifier->AttachProperty(ModifierNG::RSPropertyType::CONTENT_STYLE, property);
    RSRenderNode::ModifierNGContainer vecModifier = { modifier };
    rsCanvasDrawingRenderNode.modifiersNG_[ModifierNG::RSModifierType::CONTENT_STYLE] = vecModifier;
    // case 1.1: IsNeedResetSurface
    rsCanvasDrawingRenderNode.ProcessRenderContents(*canvas_);
    // case 1.2: Isn't NeedResetSurface
    rsCanvasDrawingRenderNode.isGpuSurface_ = true;
    rsCanvasDrawingRenderNode.isNeedProcess_.store(true);
    rsCanvasDrawingRenderNode.ProcessRenderContents(*canvas_);
    EXPECT_FALSE(rsCanvasDrawingRenderNode.isNeedProcess_);
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
    auto modifier = std::make_shared<ModifierNG::RSCustomRenderModifier<ModifierNG::RSModifierType::CONTENT_STYLE>>();
    modifier->AttachProperty(ModifierNG::RSPropertyType::CONTENT_STYLE, property);
    RSRenderNode::ModifierNGContainer vecModifier = { modifier };
    rsCanvasDrawingRenderNode.modifiersNG_[ModifierNG::RSModifierType::CONTENT_STYLE] = vecModifier;
    EXPECT_TRUE(rsCanvasDrawingRenderNode.GetSizeFromDrawCmdModifiers(width, height));
}

#ifndef MODIFIER_NG
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
    rsCanvasDrawingRenderNode.drawCmdModifiers_.emplace(RSModifierType::CONTENT_STYLE, listModifier);
    EXPECT_FALSE(rsCanvasDrawingRenderNode.GetSizeFromDrawCmdModifiers(width, height));
}
#endif

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

#ifndef MODIFIER_NG
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
#endif

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

    rsCanvasDrawingRenderNode.drawingNodeRenderID = UINT32_MAX - 1;
    rsCanvasDrawingRenderNode.cachedOpCount_ = 1;
    ASSERT_TRUE(rsCanvasDrawingRenderNode.HasCachedOp());
    ASSERT_NE(rsCanvasDrawingRenderNode.GetBitmap(UINT32_MAX - 1).bmpImplPtr, nullptr);
    rsCanvasDrawingRenderNode.cachedOpCount_ = 0;
    ASSERT_FALSE(rsCanvasDrawingRenderNode.HasCachedOp());
    ASSERT_NE(rsCanvasDrawingRenderNode.GetBitmap(UINT32_MAX - 1).bmpImplPtr, nullptr);
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
    rsCanvasDrawingRenderNode->cachedOpCount_ = 1;
    EXPECT_FALSE(rsCanvasDrawingRenderNode->GetPixelmap(pixelmap, &rect, tid, drawCmdList));
    rsCanvasDrawingRenderNode->cachedOpCount_ = 0;
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

#ifndef MODIFIER_NG
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
    rsCanvasDrawingRenderNode.drawCmdModifiers_.emplace(type, listModifier);
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
#endif

/**
 * @tc.name: AddDirtyType
 * @tc.desc: test results of AddDirtyType
 * @tc.type:FUNC
 */
HWTEST_F(RSCanvasDrawingRenderNodeTest, AddDirtyTypeTest, TestSize.Level1)
{
    auto node = std::make_shared<RSCanvasDrawingRenderNode>(1);
    auto drawCmdList = std::make_shared<Drawing::DrawCmdList>(100, 100);
    node->outOfLimitCmdList_.emplace_back(drawCmdList);
    EXPECT_FALSE(node->outOfLimitCmdList_.empty());
    node->AddDirtyType(ModifierNG::RSModifierType::CONTENT_STYLE);
    EXPECT_TRUE(node->outOfLimitCmdList_.empty());
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

#ifndef MODIFIER_NG
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
#endif

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

#ifndef MODIFIER_NG
/**
 * @tc.name: GetDrawCmdListsTest
 * @tc.desc: Test GetDrawCmdLists
 * @tc.type: FUNC
 * @tc.require: ICETEZ
 */
HWTEST_F(RSCanvasDrawingRenderNodeTest, GetDrawCmdListsTest, TestSize.Level1)
{
    NodeId nodeId = 8;
    auto node = std::make_shared<RSCanvasDrawingRenderNode>(nodeId);
    auto type = RSModifierType::CONTENT_STYLE;
    std::list<Drawing::DrawCmdListPtr> cmdLists;
    auto cmd = std::make_shared<Drawing::DrawCmdList>(Drawing::DrawCmdList::UnmarshalMode::DEFERRED);
    Drawing::Brush brush;
    cmd->AddDrawOp(std::make_shared<Drawing::DrawBackgroundOpItem>(brush));
    cmdLists.push_back(cmd);
    node->drawCmdLists_.emplace(type, cmdLists);

    auto lists = node->GetDrawCmdLists();
    EXPECT_FALSE(lists.empty());
    node->ClearResource();
    auto lists2 = node->GetDrawCmdLists();
    EXPECT_TRUE(lists2.empty());
}
#endif

/**
 * @tc.name: ApplyCachedCmdListTest
 * @tc.desc: Test ApplyCachedCmdList
 * @tc.type: FUNC
 */
HWTEST_F(RSCanvasDrawingRenderNodeTest, ApplyCachedCmdListTest, TestSize.Level1)
{
    auto drawCmdList = std::make_shared<Drawing::DrawCmdList>(10, 10);
    auto node = std::make_shared<RSCanvasDrawingRenderNode>(9);
    node->outOfLimitCmdList_.emplace_back(drawCmdList);
    node->ApplyCachedCmdList();
    EXPECT_TRUE(node->outOfLimitCmdList_.empty());
    node->ApplyCachedCmdList();
    EXPECT_TRUE(node->outOfLimitCmdList_.empty());
}

/**
 * @tc.name: ApplyModifiersTest
 * @tc.desc: Test ApplyModifiers
 * @tc.type: FUNC
 */
HWTEST_F(RSCanvasDrawingRenderNodeTest, ApplyModifiersTest, TestSize.Level1)
{
    auto node = std::make_shared<RSCanvasDrawingRenderNode>(10);
    node->cachedOpCount_ = 0;
    node->dirtyTypesNG_.set(static_cast<int>(ModifierNG::RSModifierType::CONTENT_STYLE), false);
    node->ApplyModifiers();
    EXPECT_FALSE(node->HasCachedOp());
    node->cachedOpCount_ = 1;
    node->dirtyTypesNG_.set(static_cast<int>(ModifierNG::RSModifierType::CONTENT_STYLE), true);
    node->ApplyModifiers();
    EXPECT_TRUE(node->HasCachedOp());
    node->cachedOpCount_ = 0;
    node->dirtyTypesNG_.set(static_cast<int>(ModifierNG::RSModifierType::CONTENT_STYLE), true);
    node->ApplyModifiers();
    EXPECT_FALSE(node->HasCachedOp());
    node->cachedOpCount_ = 1;
    node->dirtyTypesNG_.set(static_cast<int>(ModifierNG::RSModifierType::CONTENT_STYLE), false);
    node->ApplyModifiers();
    EXPECT_TRUE(node->HasCachedOp());
}

/**
 * @tc.name: CheckCachedOpTest
 * @tc.desc: Test CheckCachedOp
 * @tc.type: FUNC
 */
HWTEST_F(RSCanvasDrawingRenderNodeTest, CheckCachedOpTest, TestSize.Level1)
{
    auto node = std::make_shared<RSCanvasDrawingRenderNode>(11);
    node->cachedOpCount_ = 0;
    EXPECT_FALSE(node->CheckCachedOp());
    node->cachedOpCount_ = 1;
    EXPECT_TRUE(node->CheckCachedOp());
    node->cachedOpCount_ = 1;
    node->isOnTheTree_ = true;
    EXPECT_TRUE(node->CheckCachedOp());
}

/**
 * @tc.name: ContentStyleSlotUpdateTest001
 * @tc.desc: Test ContentStyleSlotUpdate
 * @tc.type: FUNC
 */
HWTEST_F(RSCanvasDrawingRenderNodeTest, ContentStyleSlotUpdateTest001, TestSize.Level1)
{
    auto node = std::make_shared<RSCanvasDrawingRenderNode>(12);
    node->dirtyTypesNG_.set(static_cast<int>(ModifierNG::RSModifierType::CONTENT_STYLE), true);
    node->waitSync_ = false;
    node->isOnTheTree_ = false;
    node->isNeverOnTree_ = false;
    node->stagingRenderParams_ = std::make_unique<RSRenderParams>(node->GetId());
    node->stagingRenderParams_->surfaceParams_ = { 100, 100, GraphicColorGamut::GRAPHIC_COLOR_GAMUT_SRGB };
    node->isTextureExportNode_ = false;
    RSUniRenderJudgement::uniRenderEnabledType_ = UniRenderEnabledType::UNI_RENDER_ENABLED_FOR_ALL;
    node->ContentStyleSlotUpdate();
    EXPECT_FALSE(node->dirtyTypesNG_.test(static_cast<size_t>(ModifierNG::RSModifierType::CONTENT_STYLE)));

    node->dirtyTypesNG_.set(static_cast<int>(ModifierNG::RSModifierType::CONTENT_STYLE), true);
    node->waitSync_ = true;
    node->isOnTheTree_ = false;
    node->isNeverOnTree_ = false;
    node->stagingRenderParams_ = nullptr;
    node->isTextureExportNode_ = false;
    RSUniRenderJudgement::uniRenderEnabledType_ = UniRenderEnabledType::UNI_RENDER_DISABLED;
    node->ContentStyleSlotUpdate();
    EXPECT_TRUE(node->dirtyTypesNG_.test(static_cast<size_t>(ModifierNG::RSModifierType::CONTENT_STYLE)));

    node->waitSync_ = false;
    node->isOnTheTree_ = true;
    node->isNeverOnTree_ = false;
    node->stagingRenderParams_ = nullptr;
    node->isTextureExportNode_ = false;
    RSUniRenderJudgement::uniRenderEnabledType_ = UniRenderEnabledType::UNI_RENDER_DISABLED;
    node->ContentStyleSlotUpdate();
    EXPECT_TRUE(node->dirtyTypesNG_.test(static_cast<size_t>(ModifierNG::RSModifierType::CONTENT_STYLE)));

    node->waitSync_ = false;
    node->isOnTheTree_ = false;
    node->isNeverOnTree_ = true;
    node->stagingRenderParams_ = nullptr;
    node->isTextureExportNode_ = false;
    RSUniRenderJudgement::uniRenderEnabledType_ = UniRenderEnabledType::UNI_RENDER_DISABLED;
    node->ContentStyleSlotUpdate();
    EXPECT_TRUE(node->dirtyTypesNG_.test(static_cast<size_t>(ModifierNG::RSModifierType::CONTENT_STYLE)));
}

/**
 * @tc.name: ContentStyleSlotUpdateTest002
 * @tc.desc: Test ContentStyleSlotUpdate
 * @tc.type: FUNC
 */
HWTEST_F(RSCanvasDrawingRenderNodeTest, ContentStyleSlotUpdateTest002, TestSize.Level1)
{
    auto node = std::make_shared<RSCanvasDrawingRenderNode>(12);
    node->dirtyTypesNG_.set(static_cast<int>(ModifierNG::RSModifierType::CONTENT_STYLE), true);
    node->waitSync_ = false;
    node->isOnTheTree_ = false;
    node->isNeverOnTree_ = false;
    node->stagingRenderParams_ = nullptr;
    node->isTextureExportNode_ = true;
    RSUniRenderJudgement::uniRenderEnabledType_ = UniRenderEnabledType::UNI_RENDER_DISABLED;
    node->ContentStyleSlotUpdate();
    EXPECT_TRUE(node->dirtyTypesNG_.test(static_cast<size_t>(ModifierNG::RSModifierType::CONTENT_STYLE)));

    node->waitSync_ = false;
    node->isOnTheTree_ = false;
    node->isNeverOnTree_ = false;
    node->stagingRenderParams_ = std::make_unique<RSRenderParams>(node->GetId());
    node->isTextureExportNode_ = false;
    RSUniRenderJudgement::uniRenderEnabledType_ = UniRenderEnabledType::UNI_RENDER_DISABLED;
    node->ContentStyleSlotUpdate();
    EXPECT_TRUE(node->dirtyTypesNG_.test(static_cast<size_t>(ModifierNG::RSModifierType::CONTENT_STYLE)));

    node->waitSync_ = false;
    node->isOnTheTree_ = false;
    node->isNeverOnTree_ = false;
    node->stagingRenderParams_ = nullptr;
    node->isTextureExportNode_ = false;
    RSUniRenderJudgement::uniRenderEnabledType_ = UniRenderEnabledType::UNI_RENDER_ENABLED_FOR_ALL;
    node->ContentStyleSlotUpdate();
    EXPECT_TRUE(node->dirtyTypesNG_.test(static_cast<size_t>(ModifierNG::RSModifierType::CONTENT_STYLE)));
}

/**
 * @tc.name: SplitDrawCmdListTest
 * @tc.desc: Test SplitDrawCmdList
 * @tc.type: FUNC
 */
HWTEST_F(RSCanvasDrawingRenderNodeTest, SplitDrawCmdListTest, TestSize.Level1)
{
    auto node = std::make_shared<RSCanvasDrawingRenderNode>(13);
    auto drawCmdList = std::make_shared<Drawing::DrawCmdList>();
    Drawing::Brush brush;
    drawCmdList->AddDrawOp(std::make_shared<Drawing::DrawBackgroundOpItem>(brush));
    node->SplitDrawCmdList(1, drawCmdList, true);
    EXPECT_TRUE(drawCmdList->IsEmpty());
    drawCmdList->AddDrawOp(std::make_shared<Drawing::DrawBackgroundOpItem>(brush));
    node->SplitDrawCmdList(1, drawCmdList, false);
    EXPECT_TRUE(drawCmdList->IsEmpty());
    node->cachedOpCount_ = 300000;
    drawCmdList->AddDrawOp(std::make_shared<Drawing::DrawBackgroundOpItem>(brush));
    node->SplitDrawCmdList(1, drawCmdList, true);
    EXPECT_TRUE(drawCmdList->IsEmpty());
    node->cachedOpCount_ = 300000;
    drawCmdList->AddDrawOp(std::make_shared<Drawing::DrawBackgroundOpItem>(brush));
    node->SplitDrawCmdList(1, drawCmdList, false);
    EXPECT_TRUE(drawCmdList->IsEmpty());
}

/**
 * @tc.name: GetDrawOpItemInfoTest001
 * @tc.desc: Test GetDrawOpItemInfo
 * @tc.type: FUNC
 */
HWTEST_F(RSCanvasDrawingRenderNodeTest, GetDrawOpItemInfoTest001, TestSize.Level1)
{
    auto node = std::make_shared<RSCanvasDrawingRenderNode>(nodeId_ + 1);

    node->cachedReversedOpTypes_.clear();
    std::shared_ptr<Drawing::DrawCmdList> drawCmdList0 = nullptr;
    node->GetDrawOpItemInfo(drawCmdList0, 1);
    EXPECT_EQ(node->cachedReversedOpTypes_.size(), 0);
    std::string outTest1;
    node->DumpSubClassNode(outTest1);
    EXPECT_EQ(outTest1, ", lastResetSurfaceTime: 0, opCountAfterReset: 0, drawOpInfo: []");
}

/**
 * @tc.name: GetDrawOpItemInfoTest002
 * @tc.desc: Test GetDrawOpItemInfo
 * @tc.type: FUNC
 */
HWTEST_F(RSCanvasDrawingRenderNodeTest, GetDrawOpItemInfoTest002, TestSize.Level1)
{
    auto node = std::make_shared<RSCanvasDrawingRenderNode>(nodeId_ + 1);
    Drawing::Brush brush;

    // add special drawCmdList 
    auto drawCmdList1 = std::make_shared<Drawing::DrawCmdList>(Drawing::DrawCmdList::UnmarshalMode::DEFERRED);
    drawCmdList1->width_ = 1;
    drawCmdList1->height_ = 1;
    drawCmdList1->drawOpItems_.clear();
    node->GetDrawOpItemInfo(drawCmdList1, drawCmdList1->drawOpItems_.size());

    auto drawCmdList2 = std::make_shared<Drawing::DrawCmdList>(Drawing::DrawCmdList::UnmarshalMode::DEFERRED);
    drawCmdList2->width_ = 2;
    drawCmdList2->height_ = 2;
    drawCmdList2->AddDrawOp(std::make_shared<Drawing::DrawBackgroundOpItem>(brush));
    drawCmdList2->AddDrawOp(nullptr);
    drawCmdList2->AddDrawOp(std::make_shared<Drawing::DrawBackgroundOpItem>(brush));
    node->GetDrawOpItemInfo(drawCmdList2, drawCmdList2->drawOpItems_.size());

    std::string outTest1;
    node->DumpSubClassNode(outTest1);
    EXPECT_EQ(outTest1, ", lastResetSurfaceTime: 0, opCountAfterReset: 0, drawOpInfo: [[1,1,0][],[2,2,3][15,15]]");
    std::cout << outTest1 << std::endl;
}

/**
 * @tc.name: GetDrawOpItemInfoTest003
 * @tc.desc: Test GetDrawOpItemInfo
 * @tc.type: FUNC
 */
HWTEST_F(RSCanvasDrawingRenderNodeTest, GetDrawOpItemInfoTest003, TestSize.Level1)
{
    auto node = std::make_shared<RSCanvasDrawingRenderNode>(nodeId_ + 1);
    Drawing::Brush brush;

    // add special drawCmdList 
    auto drawCmdList = std::make_shared<Drawing::DrawCmdList>(Drawing::DrawCmdList::UnmarshalMode::DEFERRED);
    drawCmdList->width_ = 2;
    drawCmdList->height_ = 2;
    drawCmdList->AddDrawOp(std::make_shared<Drawing::DrawBackgroundOpItem>(brush));
    node->GetDrawOpItemInfo(drawCmdList, drawCmdList->drawOpItems_.size());

    // could not happen
    node->cachedReversedOpTypes_[0].drawOpTypes.clear();

    std::string outTest1;
    node->DumpSubClassNode(outTest1);
    EXPECT_EQ(outTest1, ", lastResetSurfaceTime: 0, opCountAfterReset: 0, drawOpInfo: [[2,2,1][]]");
    std::cout << outTest1 << std::endl;
}

/**
 * @tc.name: GetDrawOpItemInfoTest004
 * @tc.desc: Test GetDrawOpItemInfo
 * @tc.type: FUNC
 */
HWTEST_F(RSCanvasDrawingRenderNodeTest, GetDrawOpItemInfoTest004, TestSize.Level1)
{
    auto node = std::make_shared<RSCanvasDrawingRenderNode>(nodeId_ + 1);
    Drawing::Brush brush;

    // drawCmdList num less than DRAWCMDLIST_DUMP_LIMIT
    // drawOpItems num less than OP_DUMP_LIMIT_PER_CMD
    size_t drawCmdListLimit = 2;
    size_t drawOpItemsLimit = 2;
    
    auto drawCmdList = std::make_shared<Drawing::DrawCmdList>(Drawing::DrawCmdList::UnmarshalMode::DEFERRED);
    drawCmdList->width_ = 1;
    drawCmdList->height_ = 1;
    for (int i = 0; i < drawOpItemsLimit; ++i) {
        drawCmdList->AddDrawOp(std::make_shared<Drawing::DrawBackgroundOpItem>(brush));
    }
    auto opItemsSize = drawCmdList->GetDrawOpItems().size();
    EXPECT_EQ(opItemsSize, drawOpItemsLimit);

    for (int i = 0; i < drawCmdListLimit; ++i) {
        node->GetDrawOpItemInfo(drawCmdList, opItemsSize);
    }

    EXPECT_EQ(node->cachedReversedOpTypes_.front().opItemSize, drawOpItemsLimit);
    EXPECT_EQ(node->cachedReversedOpTypes_.front().drawOpTypes.size(), drawOpItemsLimit);
    EXPECT_EQ(node->cachedReversedOpTypes_.back().opItemSize, drawOpItemsLimit);
    EXPECT_EQ(node->cachedReversedOpTypes_.back().drawOpTypes.size(), drawOpItemsLimit);

    std::string outTest1;
    node->DumpSubClassNode(outTest1);
    EXPECT_EQ(outTest1, 
        ", lastResetSurfaceTime: 0, opCountAfterReset: 0, drawOpInfo: [[1,1,2][15,15],[1,1,2][15,15]]");
    std::cout << outTest1 << std::endl;
}

/**
 * @tc.name: GetDrawOpItemInfoTest005
 * @tc.desc: Test GetDrawOpItemInfo
 * @tc.type: FUNC
 */
HWTEST_F(RSCanvasDrawingRenderNodeTest, GetDrawOpItemInfoTest005, TestSize.Level1)
{
    auto node = std::make_shared<RSCanvasDrawingRenderNode>(nodeId_ + 1);
    Drawing::Brush brush;

    // drawCmdList num more than DRAWCMDLIST_DUMP_LIMIT
    // drawOpItems num less than OP_DUMP_LIMIT_PER_CMD
    size_t drawCmdListLimit = DRAWCMDLIST_DUMP_LIMIT + 1;
    size_t drawOpItemsLimit = 3;
    
    auto drawCmdList = std::make_shared<Drawing::DrawCmdList>(Drawing::DrawCmdList::UnmarshalMode::DEFERRED);
    drawCmdList->width_ = 1;
    drawCmdList->height_ = 1;
    for (int i = 0; i < drawOpItemsLimit; ++i) {
        drawCmdList->AddDrawOp(std::make_shared<Drawing::DrawBackgroundOpItem>(brush));
    }
    auto opItemsSize = drawCmdList->GetDrawOpItems().size();
    EXPECT_EQ(opItemsSize, drawOpItemsLimit);

    for (int i = 0; i < drawCmdListLimit; ++i) {
        node->GetDrawOpItemInfo(drawCmdList, opItemsSize);
    }

    EXPECT_EQ(node->cachedReversedOpTypes_.front().opItemSize, drawOpItemsLimit);
    EXPECT_EQ(node->cachedReversedOpTypes_.front().drawOpTypes.size(), drawOpItemsLimit);
    EXPECT_EQ(node->cachedReversedOpTypes_.back().opItemSize, drawOpItemsLimit);
    EXPECT_EQ(node->cachedReversedOpTypes_.back().drawOpTypes.size(), drawOpItemsLimit);

    EXPECT_EQ(node->cachedReversedOpTypes_.size(), DRAWCMDLIST_DUMP_LIMIT);

    std::string outTest1;
    node->DumpSubClassNode(outTest1);

    std::string opTypesStr = "[";
    for (int i = 0; i < drawOpItemsLimit; ++i) {
        opTypesStr += "15,";
    }
    opTypesStr.pop_back();
    opTypesStr += "]";

    std::string ret = ", lastResetSurfaceTime: 0, opCountAfterReset: 0, drawOpInfo: [";
    for (int i = 0; i < DRAWCMDLIST_DUMP_LIMIT; ++i) {
        ret += "[1,1," + std::to_string(opItemsSize) + "]" + opTypesStr + ",";
    }
    ret.pop_back();
    ret += "]";
    EXPECT_EQ(outTest1, ret);
    std::cout << outTest1 << std::endl;
}

/**
 * @tc.name: GetDrawOpItemInfoTest006
 * @tc.desc: Test GetDrawOpItemInfo
 * @tc.type: FUNC
 */
HWTEST_F(RSCanvasDrawingRenderNodeTest, GetDrawOpItemInfoTest006, TestSize.Level1)
{
    auto node = std::make_shared<RSCanvasDrawingRenderNode>(nodeId_ + 1);
    Drawing::Brush brush;

    // drawCmdList num less than DRAWCMDLIST_DUMP_LIMIT
    // drawOpItems num more than OP_DUMP_LIMIT_PER_CMD
    size_t drawCmdListLimit = 3;
    size_t drawOpItemsLimit = OP_DUMP_LIMIT_PER_CMD + 1;
    
    auto drawCmdList = std::make_shared<Drawing::DrawCmdList>(Drawing::DrawCmdList::UnmarshalMode::DEFERRED);
    drawCmdList->width_ = 1;
    drawCmdList->height_ = 1;
    for (int i = 0; i < drawOpItemsLimit; ++i) {
        drawCmdList->AddDrawOp(std::make_shared<Drawing::DrawBackgroundOpItem>(brush));
    }
    auto opItemsSize = drawCmdList->GetDrawOpItems().size();
    EXPECT_EQ(opItemsSize, drawOpItemsLimit);

    for (int i = 0; i < drawCmdListLimit; ++i) {
        node->GetDrawOpItemInfo(drawCmdList, opItemsSize);
    }

    EXPECT_EQ(node->cachedReversedOpTypes_.front().opItemSize, drawOpItemsLimit);
    EXPECT_EQ(node->cachedReversedOpTypes_.front().drawOpTypes.size(), OP_DUMP_LIMIT_PER_CMD);
    EXPECT_EQ(node->cachedReversedOpTypes_.back().opItemSize, drawOpItemsLimit);
    EXPECT_EQ(node->cachedReversedOpTypes_.back().drawOpTypes.size(), OP_DUMP_LIMIT_PER_CMD);

    EXPECT_EQ(node->cachedReversedOpTypes_.size(), drawCmdListLimit);

    std::string outTest1;
    node->DumpSubClassNode(outTest1);

    std::string opTypesStr = "[";
    for (int i = 0; i < OP_DUMP_LIMIT_PER_CMD; ++i) {
        opTypesStr += "15,";
    }
    opTypesStr.pop_back();
    opTypesStr += "]";

    std::string ret = ", lastResetSurfaceTime: 0, opCountAfterReset: 0, drawOpInfo: [";
    for (int i = 0; i < drawCmdListLimit; ++i) {
        ret += "[1,1," + std::to_string(opItemsSize) + "]" + opTypesStr + ",";
    }
    ret.pop_back();
    ret += "]";
    EXPECT_EQ(outTest1, ret);
    std::cout << outTest1 << std::endl;
}

/**
 * @tc.name: GetDrawOpItemInfoTest007
 * @tc.desc: Test GetDrawOpItemInfo
 * @tc.type: FUNC
 */
HWTEST_F(RSCanvasDrawingRenderNodeTest, GetDrawOpItemInfoTest007, TestSize.Level1)
{
    auto node = std::make_shared<RSCanvasDrawingRenderNode>(nodeId_ + 1);
    Drawing::Brush brush;

    // drawCmdList num more than DRAWCMDLIST_DUMP_LIMIT
    // drawOpItems num more than OP_DUMP_LIMIT_PER_CMD
    size_t drawCmdListLimit = DRAWCMDLIST_DUMP_LIMIT + 1;
    size_t drawOpItemsLimit = OP_DUMP_LIMIT_PER_CMD + 1;
    
    auto drawCmdList = std::make_shared<Drawing::DrawCmdList>(Drawing::DrawCmdList::UnmarshalMode::DEFERRED);
    drawCmdList->width_ = 1;
    drawCmdList->height_ = 1;
    for (int i = 0; i < drawOpItemsLimit; ++i) {
        drawCmdList->AddDrawOp(std::make_shared<Drawing::DrawBackgroundOpItem>(brush));
    }
    auto opItemsSize = drawCmdList->GetDrawOpItems().size();
    EXPECT_EQ(opItemsSize, drawOpItemsLimit);

    for (int i = 0; i < drawCmdListLimit; ++i) {
        node->GetDrawOpItemInfo(drawCmdList, opItemsSize);
    }

    EXPECT_EQ(node->cachedReversedOpTypes_.front().opItemSize, drawOpItemsLimit);
    EXPECT_EQ(node->cachedReversedOpTypes_.front().drawOpTypes.size(), OP_DUMP_LIMIT_PER_CMD);
    EXPECT_EQ(node->cachedReversedOpTypes_.back().opItemSize, drawOpItemsLimit);
    EXPECT_EQ(node->cachedReversedOpTypes_.back().drawOpTypes.size(), OP_DUMP_LIMIT_PER_CMD);

    EXPECT_EQ(node->cachedReversedOpTypes_.size(), DRAWCMDLIST_DUMP_LIMIT);

    std::string outTest1;
    node->DumpSubClassNode(outTest1);

    std::string opTypesStr = "[";
    for (int i = 0; i < OP_DUMP_LIMIT_PER_CMD; ++i) {
        opTypesStr += "15,";
    }
    opTypesStr.pop_back();
    opTypesStr += "]";

    std::string ret = ", lastResetSurfaceTime: 0, opCountAfterReset: 0, drawOpInfo: [";
    for (int i = 0; i < DRAWCMDLIST_DUMP_LIMIT; ++i) {
        ret += "[1,1," + std::to_string(opItemsSize) + "]" + opTypesStr + ",";
    }
    ret.pop_back();
    ret += "]";
    EXPECT_EQ(outTest1, ret);
    std::cout << outTest1 << std::endl;
}
} // namespace OHOS::Rosen