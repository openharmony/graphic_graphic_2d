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

#include "common/rs_obj_geometry.h"
#include "common/rs_rect.h"
#include "drawable/rs_property_drawable.h"
#include "effect/rs_render_shape_base.h"
#include "feature/filter_cache_manager/rs_filter_cache_manager.h"
#include "ge_visual_effect_container.h"
#include "pipeline/rs_simple_draw_cmd_list.h"
#include "pipeline/rs_recording_canvas.h"
#include "pipeline/rs_render_node.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Rosen {
class RSPropertyDrawableTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

class ConcreteRSRenderNodeDrawableAdapter : public DrawableV2::RSRenderNodeDrawableAdapter {
public:
    explicit ConcreteRSRenderNodeDrawableAdapter(std::shared_ptr<const RSRenderNode> node)
        : RSRenderNodeDrawableAdapter(std::move(node))
    {}
    void Draw(Drawing::Canvas& canvas) {}
};

void RSPropertyDrawableTest::SetUpTestCase() {}
void RSPropertyDrawableTest::TearDownTestCase() {}
void RSPropertyDrawableTest::SetUp() {}
void RSPropertyDrawableTest::TearDown() {}

/**
 * @tc.name: OnSyncAndOnDrawFuncTest001
 * @tc.desc: class RSPropertyDrawable OnSync and OnDraw test
 * @tc.type:FUNC
 * @tc.require: issueI9VSPU
 */
HWTEST_F(RSPropertyDrawableTest, OnSyncAndOnDrawFuncTest001, TestSize.Level1)
{
    std::shared_ptr<DrawableV2::RSPropertyDrawable> propertyDrawable =
        std::make_shared<DrawableV2::RSPropertyDrawable>();
    EXPECT_NE(propertyDrawable, nullptr);

    propertyDrawable->needSync_ = false;
    propertyDrawable->OnSync();

    propertyDrawable->needSync_ = true;
    propertyDrawable->drawCmdList_ = nullptr;
    propertyDrawable->stagingDrawCmdList_ = nullptr;
    propertyDrawable->OnSync();
    EXPECT_FALSE(propertyDrawable->needSync_);

    auto drawCmdList = std::make_shared<Drawing::DrawCmdList>();
    auto simpleDrawCmdList = RSSimpleDrawCmdList::CreateFromDrawCmdList(drawCmdList);
    ASSERT_NE(simpleDrawCmdList, nullptr);
    propertyDrawable->drawCmdList_ = simpleDrawCmdList;
    Drawing::Canvas canvas;
    Drawing::Rect rect(0.0f, 0.0f, 1.0f, 1.0f);
    propertyDrawable->OnDraw(&canvas, &rect);
    propertyDrawable->propertyDescription_ = "RSPropertyDrawable test";
    propertyDrawable->OnDraw(&canvas, &rect);

    propertyDrawable->drawCmdList_ = nullptr;
    propertyDrawable->OnDraw(&canvas, &rect);
    EXPECT_EQ(propertyDrawable->drawCmdList_, nullptr);
}

/**
 * @tc.name: RSPropertyDrawCmdListUpdaterTest002
 * @tc.desc: ~RSPropertyDrawCmdListUpdater test
 * @tc.type:FUNC
 * @tc.require: issueI9VSPU
 */
HWTEST_F(RSPropertyDrawableTest, RSPropertyDrawCmdListUpdaterTest002, TestSize.Level1)
{
    DrawableV2::RSPropertyDrawable targetTest1;
    std::shared_ptr<DrawableV2::RSPropertyDrawCmdListUpdater> propertyDrawCmdListUpdaterTest1 =
        std::make_shared<DrawableV2::RSPropertyDrawCmdListUpdater>(0, 0, &targetTest1);
    EXPECT_NE(propertyDrawCmdListUpdaterTest1, nullptr);

    std::unique_ptr<ExtendRecordingCanvas> recordingCanvas = std::make_unique<ExtendRecordingCanvas>(0, 0);
    EXPECT_NE(recordingCanvas, nullptr);
    propertyDrawCmdListUpdaterTest1->recordingCanvas_ = std::move(recordingCanvas);
    DrawableV2::RSPropertyDrawable target;
    propertyDrawCmdListUpdaterTest1->target_ = &target;

    DrawableV2::RSPropertyDrawable targetTest2;
    std::shared_ptr<DrawableV2::RSPropertyDrawCmdListUpdater> propertyDrawCmdListUpdaterTest2 =
        std::make_shared<DrawableV2::RSPropertyDrawCmdListUpdater>(0, 0, &targetTest2);
    propertyDrawCmdListUpdaterTest2->recordingCanvas_ = nullptr;
}

/**
 * @tc.name: OnGenerateAndOnUpdateTest003
 * @tc.desc: class RSFrameOffsetDrawable OnGenerate and OnUpdate test
 * @tc.type:FUNC
 * @tc.require: issueI9VSPU
 */
HWTEST_F(RSPropertyDrawableTest, OnGenerateAndOnUpdateTest003, TestSize.Level1)
{
    std::shared_ptr<DrawableV2::RSFrameOffsetDrawable> frameOffsetDrawable =
        std::make_shared<DrawableV2::RSFrameOffsetDrawable>();
    EXPECT_NE(frameOffsetDrawable, nullptr);

    RSRenderNode node(0);
    node.renderProperties_.frameOffsetX_ = 0.0f;
    node.renderProperties_.frameOffsetY_ = 0.0f;
    EXPECT_EQ(frameOffsetDrawable->OnGenerate(node), nullptr);

    node.renderProperties_.frameOffsetX_ = 1.0f;
    EXPECT_NE(frameOffsetDrawable->OnGenerate(node), nullptr);
}

/**
 * @tc.name: OnUpdateTest004
 * @tc.desc: class RSClipToBoundsDrawable OnUpdate test
 * @tc.type:FUNC
 * @tc.require: issueIA61E9
 */
HWTEST_F(RSPropertyDrawableTest, OnUpdateTest004, TestSize.Level1)
{
    std::shared_ptr<DrawableV2::RSClipToBoundsDrawable> clipToBoundsDrawable =
        std::make_shared<DrawableV2::RSClipToBoundsDrawable>();
    EXPECT_NE(clipToBoundsDrawable, nullptr);

    RSRenderNode nodeTest1(0);
    nodeTest1.renderProperties_.clipPath_ = std::make_shared<RSPath>();
    EXPECT_NE(nodeTest1.renderProperties_.clipPath_, nullptr);
    EXPECT_TRUE(clipToBoundsDrawable->OnUpdate(nodeTest1));

    RSRenderNode nodeTest2(0);
    RectT<float> rect(1.0f, 1.0f, 1.0f, 1.0f);
    RRectT<float> rectt(rect, 1.0f, 1.0f);
    nodeTest2.renderProperties_.clipRRect_ = std::make_unique<RRect>(rectt);
    EXPECT_TRUE(clipToBoundsDrawable->OnUpdate(nodeTest2));

    RSRenderNode nodeTest3(0);
    nodeTest3.renderProperties_.cornerRadius_ = 1.0f;
    EXPECT_TRUE(clipToBoundsDrawable->OnUpdate(nodeTest3));

    RSRenderNode nodeTest4(0);
    EXPECT_TRUE(clipToBoundsDrawable->OnUpdate(nodeTest4));
}

/**
 * @tc.name: RSClipToBoundsDrawableOnSyncTest
 * @tc.desc: class RSClipToBoundsDrawable OnSync test
 * @tc.type: FUNC
 * @tc.require: issue20176
 */
HWTEST_F(RSPropertyDrawableTest, RSClipToBoundsDrawableOnSyncTest, TestSize.Level1)
{
    std::shared_ptr<DrawableV2::RSClipToBoundsDrawable> clipToBoundsDrawable =
        std::make_shared<DrawableV2::RSClipToBoundsDrawable>();
    EXPECT_NE(clipToBoundsDrawable, nullptr);
    clipToBoundsDrawable->needSync_ = false;
    clipToBoundsDrawable->OnSync();

    clipToBoundsDrawable->needSync_ = true;
    clipToBoundsDrawable->OnSync();
    EXPECT_FALSE(clipToBoundsDrawable->needSync_);
}

/**
 * @tc.name: RSClipToBoundsDrawableOnDrawFuncTest
 * @tc.desc: class RSClipToBoundsDrawable OnDraw test
 * @tc.type: FUNC
 * @tc.require: issue20176
 */
HWTEST_F(RSPropertyDrawableTest, RSClipToBoundsDrawableOnDrawFuncTest, TestSize.Level1)
{
    std::shared_ptr<DrawableV2::RSClipToBoundsDrawable> clipToBoundsDrawable =
        std::make_shared<DrawableV2::RSClipToBoundsDrawable>();
    EXPECT_NE(clipToBoundsDrawable, nullptr);
    Drawing::Canvas canvas;
    Drawing::Rect rect(0.0f, 0.0f, 1.0f, 1.0f);
    clipToBoundsDrawable->OnDraw(&canvas, &rect);

    clipToBoundsDrawable->type_ = RSClipToBoundsType::CLIP_PATH;
    clipToBoundsDrawable->OnDraw(&canvas, &rect);

    clipToBoundsDrawable->type_ = RSClipToBoundsType::CLIP_RRECT;
    clipToBoundsDrawable->isClipRRectOptimization_ = true;
    clipToBoundsDrawable->OnDraw(&canvas, &rect);

    clipToBoundsDrawable->type_ = RSClipToBoundsType::CLIP_RRECT;
    clipToBoundsDrawable->isClipRRectOptimization_ = false;
    clipToBoundsDrawable->OnDraw(&canvas, &rect);

    clipToBoundsDrawable->type_ = RSClipToBoundsType::CLIP_IRECT;
    clipToBoundsDrawable->OnDraw(&canvas, &rect);

    clipToBoundsDrawable->type_ = RSClipToBoundsType::CLIP_RECT;
    clipToBoundsDrawable->OnDraw(&canvas, &rect);

    clipToBoundsDrawable->type_ = RSClipToBoundsType::INVALID;
    clipToBoundsDrawable->OnDraw(&canvas, &rect);
}

/**
 * @tc.name: RSClipToBoundsDrawableTestSDF
 * @tc.desc: Test RSClipToBoundsDrawable with sdf
 * @tc.type: FUNC
 */
HWTEST_F(RSPropertyDrawableTest, RSClipToBoundsDrawableTestSDF, TestSize.Level1)
{
    auto drawable = std::make_shared<DrawableV2::RSClipToBoundsDrawable>();
    drawable->type_ = RSClipToBoundsType::CLIP_SDF;
    Drawing::Canvas canvas;
    // 1.0f, 1.0f, 2.0f, 2.0f is left top right bottom
    Drawing::Rect rect { 1.0f, 1.0f, 2.0f, 2.0f };
    drawable->OnDraw(&canvas, &rect);
    EXPECT_EQ(drawable->geContainer_, nullptr);

    NodeId id = 3;
    RSRenderNode node(id);
    auto sdfShape = RSNGRenderShapeBase::Create(RSNGEffectType::SDF_UNION_OP_SHAPE);
    node.GetMutableRenderProperties().SetSDFShape(sdfShape);
    drawable->OnUpdate(node);
    drawable->OnSync();
    RSPaintFilterCanvas paintFilterCanvas(&canvas);
    drawable->OnDraw(&paintFilterCanvas, &rect);
    EXPECT_NE(drawable->geContainer_, nullptr);
}

/**
 * @tc.name: OnGenerateAndOnUpdateTest005
 * @tc.desc: class RSClipToFrameDrawable OnGenerate and OnUpdate test
 * @tc.type:FUNC
 * @tc.require: issueI9VSPU
 */
HWTEST_F(RSPropertyDrawableTest, OnGenerateAndOnUpdateTest005, TestSize.Level1)
{
    std::shared_ptr<DrawableV2::RSClipToFrameDrawable> clipToFrameDrawable =
        std::make_shared<DrawableV2::RSClipToFrameDrawable>();
    EXPECT_NE(clipToFrameDrawable, nullptr);

    RSRenderNode node(0);
    node.renderProperties_.clipToFrame_ = false;
    EXPECT_EQ(clipToFrameDrawable->OnGenerate(node), nullptr);

    RSObjGeometry geometry;

    geometry.width_ = 1.0f;
    geometry.height_ = 1.0f;
    node.renderProperties_.frameGeo_ = geometry;
    node.renderProperties_.clipToFrame_ = true;
    EXPECT_NE(clipToFrameDrawable->OnGenerate(node), nullptr);
}

/**
 * @tc.name: RSClipToBoundsDrawableClipShapeOnUpdateTest
 * @tc.desc: Test RSClipToBoundsDrawable OnUpdate with ClipShape (converted to CLIP_RRECT in OnUpdate)
 * @tc.type: FUNC
 * @tc.require: SR20260808951364
 */
HWTEST_F(RSPropertyDrawableTest, RSClipToBoundsDrawableClipShapeOnUpdateTest, TestSize.Level1)
{
    // OnUpdate reads clipShape_ and converts to RRect on the fly
    RSRenderNode node(0);
    Vector4f radius(10.0f, 20.0f, 30.0f, 40.0f);
    RRect shape(RectT<float>(0.0f, 0.0f, 100.0f, 200.0f), radius);
    node.GetMutableRenderProperties().SetClipShape(shape);

    auto drawable = std::make_shared<DrawableV2::RSClipToBoundsDrawable>();
    EXPECT_TRUE(drawable->OnUpdate(node));
    EXPECT_EQ(drawable->stagingType_, RSClipToBoundsType::CLIP_RRECT);
    EXPECT_TRUE(drawable->needSync_);
}

/**
 * @tc.name: RSClipToBoundsDrawableClipShapeOnUpdateNotSetTest
 * @tc.desc: Test RSClipToBoundsDrawable OnUpdate when clipShape is not set
 * @tc.type: FUNC
 * @tc.require: SR20260808951364
 */
HWTEST_F(RSPropertyDrawableTest, RSClipToBoundsDrawableClipShapeOnUpdateNotSetTest, TestSize.Level1)
{
    RSRenderNode node(0);
    // no clipShape set — should not be CLIP_RRECT
    auto drawable = std::make_shared<DrawableV2::RSClipToBoundsDrawable>();
    EXPECT_TRUE(drawable->OnUpdate(node));
    EXPECT_NE(drawable->stagingType_, RSClipToBoundsType::CLIP_RRECT);
}

/**
 * @tc.name: RSClipToBoundsDrawableClipShapeOnDrawOptTest
 * @tc.desc: Test RSClipToBoundsDrawable OnDraw with CLIP_RRECT and optimization enabled
 * @tc.type: FUNC
 * @tc.require: SR20260808951364
 */
HWTEST_F(RSPropertyDrawableTest, RSClipToBoundsDrawableClipShapeOnDrawOptTest, TestSize.Level1)
{
    auto drawable = std::make_shared<DrawableV2::RSClipToBoundsDrawable>();
    drawable->type_ = RSClipToBoundsType::CLIP_RRECT;
    drawable->isClipRRectOptimization_ = true;
    Drawing::Rect rect(0.0f, 0.0f, 100.0f, 100.0f);
    Drawing::Canvas canvas;
    int saveCountBefore = canvas.GetSaveCount();
    drawable->OnDraw(&canvas, &rect);
    int saveCountAfter = canvas.GetSaveCount();
    EXPECT_TRUE(saveCountAfter >= saveCountBefore);
}

/**
 * @tc.name: RSClipToBoundsDrawableClipShapeOnDrawNoOptTest
 * @tc.desc: Test RSClipToBoundsDrawable OnDraw with CLIP_RRECT and optimization disabled
 * @tc.type: FUNC
 * @tc.require: SR20260808951364
 */
HWTEST_F(RSPropertyDrawableTest, RSClipToBoundsDrawableClipShapeOnDrawNoOptTest, TestSize.Level1)
{
    auto drawable = std::make_shared<DrawableV2::RSClipToBoundsDrawable>();
    drawable->type_ = RSClipToBoundsType::CLIP_RRECT;
    drawable->isClipRRectOptimization_ = false;
    Drawing::Rect rect(0.0f, 0.0f, 100.0f, 100.0f);
    Drawing::Canvas canvas;
    int saveCountBefore = canvas.GetSaveCount();
    drawable->OnDraw(&canvas, &rect);
    int saveCountAfter = canvas.GetSaveCount();
    EXPECT_TRUE(saveCountAfter >= saveCountBefore);
}

/**
 * @tc.name: RSClipToBoundsDrawableClipShapeEmptyRRectTest
 * @tc.desc: Test OnUpdate with ClipShape and zero-size RRect still goes CLIP_RRECT path
 * @tc.type: FUNC
 * @tc.require: SR20260808951364
 */
HWTEST_F(RSPropertyDrawableTest, RSClipToBoundsDrawableClipShapeEmptyRRectTest, TestSize.Level1)
{
    RSRenderNode node(0);
    // Zero-size RRect → OnUpdate computes RRect from clipShape_ + bounds
    RRect zeroShape(RectT<float>(0.0f, 0.0f, 0.0f, 0.0f), Vector4f(0.0f));
    node.GetMutableRenderProperties().SetClipShape(zeroShape);

    auto drawable = std::make_shared<DrawableV2::RSClipToBoundsDrawable>();
    EXPECT_TRUE(drawable->OnUpdate(node));
    EXPECT_EQ(drawable->stagingType_, RSClipToBoundsType::CLIP_RRECT);
}

/**
 * @tc.name: RSClipToBoundsDrawableClipShapeRRectValueTest
 * @tc.desc: Test OnUpdate with ClipShape verifies stagingClipRRect_ values are correct
 * @tc.type: FUNC
 * @tc.require: SR20260808951364
 */
HWTEST_F(RSPropertyDrawableTest, RSClipToBoundsDrawableClipShapeRRectValueTest, TestSize.Level1)
{
    RSRenderNode node(0);
    // Set bounds so we can verify RRect dimensions; position is always (0,0) in local space
    constexpr float BOUNDS_X = 10.0f;
    constexpr float BOUNDS_Y = 20.0f;
    constexpr float SHAPE_WIDTH = 100.0f;
    constexpr float SHAPE_HEIGHT = 200.0f;
    Vector4f radius(10.0f, 20.0f, 30.0f, 40.0f);
    RRect shape(RectT<float>(0.0f, 0.0f, SHAPE_WIDTH, SHAPE_HEIGHT), radius);
    node.GetMutableRenderProperties().SetClipShape(shape);
    node.GetMutableRenderProperties().SetBounds(
        Vector4f(BOUNDS_X, BOUNDS_Y, SHAPE_WIDTH, SHAPE_HEIGHT));

    auto drawable = std::make_shared<DrawableV2::RSClipToBoundsDrawable>();
    EXPECT_TRUE(drawable->OnUpdate(node));
    EXPECT_EQ(drawable->stagingType_, RSClipToBoundsType::CLIP_RRECT);

    // RRect uses GetBoundsRect() which returns (0,0,w,h) — clip executes in node-local space
    const auto& rrectRect = drawable->stagingClipRRect_.GetRect();
    EXPECT_FLOAT_EQ(rrectRect.GetLeft(), 0.0f);
    EXPECT_FLOAT_EQ(rrectRect.GetTop(), 0.0f);
    EXPECT_FLOAT_EQ(rrectRect.GetWidth(), SHAPE_WIDTH);
    EXPECT_FLOAT_EQ(rrectRect.GetHeight(), SHAPE_HEIGHT);
}

/**
 * @tc.name: RSClipToBoundsDrawableClipShapeCornerOptTest
 * @tc.desc: Test OnUpdate with ClipShape verifies stagingIsClipRRectOptimization_ value
 * @tc.type: FUNC
 * @tc.require: SR20260808951364
 */
HWTEST_F(RSPropertyDrawableTest, RSClipToBoundsDrawableClipShapeCornerOptTest, TestSize.Level1)
{
    RSRenderNode node(0);
    Vector4f radius(50.0f, 50.0f, 50.0f, 50.0f);
    RRect shape(RectT<float>(0.0f, 0.0f, 400.0f, 400.0f), radius);
    node.GetMutableRenderProperties().SetClipShape(shape);
    node.GetMutableRenderProperties().SetBounds(Vector4f(0.0f, 0.0f, 400.0f, 400.0f));

    auto drawable = std::make_shared<DrawableV2::RSClipToBoundsDrawable>();
    EXPECT_TRUE(drawable->OnUpdate(node));
    EXPECT_EQ(drawable->stagingType_, RSClipToBoundsType::CLIP_RRECT);
    // NeedCornerOptimization should be set from properties
    bool expectedOpt = node.GetRenderProperties().NeedCornerOptimization();
    EXPECT_EQ(drawable->stagingIsClipRRectOptimization_, expectedOpt);
}

/**
 * @tc.name: RSClipToBoundsDrawableClipShapePriorityTest
 * @tc.desc: Test OnUpdate with both ClipShape and ClipBounds set, ClipShape takes priority
 * @tc.type: FUNC
 * @tc.require: SR20260808951364
 */
HWTEST_F(RSPropertyDrawableTest, RSClipToBoundsDrawableClipShapePriorityTest, TestSize.Level1)
{
    RSRenderNode node(0);
    Vector4f radius(50.0f, 50.0f, 50.0f, 50.0f);
    RRect shape(RectT<float>(0.0f, 0.0f, 400.0f, 400.0f), radius);
    node.GetMutableRenderProperties().SetClipShape(shape);
    // Also set ClipBounds — ClipShape has higher priority
    auto clipPath = std::make_shared<RSPath>();
    node.GetMutableRenderProperties().SetClipBounds(clipPath);

    auto drawable = std::make_shared<DrawableV2::RSClipToBoundsDrawable>();
    EXPECT_TRUE(drawable->OnUpdate(node));
    // ClipShape is first branch, should get CLIP_RRECT not CLIP_PATH
    EXPECT_EQ(drawable->stagingType_, RSClipToBoundsType::CLIP_RRECT);
    EXPECT_NE(drawable->stagingType_, RSClipToBoundsType::CLIP_PATH);
}
 
/**
 * @tc.name: RSClipToBoundsDrawableClipShapeOnGenerateTest
 * @tc.desc: Test OnGenerate with ClipShape returns non-null drawable
 * @tc.type: FUNC
 * @tc.require: SR20260808951364
 */
HWTEST_F(RSPropertyDrawableTest, RSClipToBoundsDrawableClipShapeOnGenerateTest, TestSize.Level1)
{
    RSRenderNode node(0);
    Vector4f radius(50.0f, 50.0f, 50.0f, 50.0f);
    RRect shape(RectT<float>(0.0f, 0.0f, 400.0f, 400.0f), radius);
    node.GetMutableRenderProperties().SetClipShape(shape);
    node.GetMutableRenderProperties().SetBounds(Vector4f(0.0f, 0.0f, 400.0f, 400.0f));

    auto drawable = DrawableV2::RSClipToBoundsDrawable::OnGenerate(node);
    EXPECT_NE(drawable, nullptr);
    // OnGenerate calls OnUpdate + OnSync, so type_ should be synced
    auto clipDrawable = std::static_pointer_cast<DrawableV2::RSClipToBoundsDrawable>(drawable);
    EXPECT_EQ(clipDrawable->type_, RSClipToBoundsType::CLIP_RRECT);
}
 
/**
 * @tc.name: RSClipToBoundsDrawableClipShapeOnSyncRRectTest
 * @tc.desc: Test OnSync with ClipShape verifies clipRRect_ is synced from stagingClipRRect_
 * @tc.type: FUNC
 * @tc.require: SR20260808951364
 */
HWTEST_F(RSPropertyDrawableTest, RSClipToBoundsDrawableClipShapeOnSyncRRectTest, TestSize.Level1)
{
    RSRenderNode node(0);
    constexpr float SHAPE_WIDTH = 300.0f;
    constexpr float SHAPE_HEIGHT = 400.0f;
    Vector4f radius(10.0f, 20.0f, 30.0f, 40.0f);
    RRect shape(RectT<float>(0.0f, 0.0f, SHAPE_WIDTH, SHAPE_HEIGHT), radius);
    node.GetMutableRenderProperties().SetClipShape(shape);
    node.GetMutableRenderProperties().SetBounds(Vector4f(0.0f, 0.0f, SHAPE_WIDTH, SHAPE_HEIGHT));

    auto drawable = std::make_shared<DrawableV2::RSClipToBoundsDrawable>();
    EXPECT_TRUE(drawable->OnUpdate(node));
    EXPECT_TRUE(drawable->needSync_);
 
    // Before sync, clipRRect_ should not match stagingClipRRect_
    drawable->OnSync();
    EXPECT_FALSE(drawable->needSync_);
    EXPECT_EQ(drawable->type_, RSClipToBoundsType::CLIP_RRECT);

    // After sync, clipRRect_ should match stagingClipRRect_ values
    const auto& syncedRect = drawable->clipRRect_.GetRect();
    EXPECT_FLOAT_EQ(syncedRect.GetWidth(), SHAPE_WIDTH);
    EXPECT_FLOAT_EQ(syncedRect.GetHeight(), SHAPE_HEIGHT);
    EXPECT_EQ(drawable->isClipRRectOptimization_, drawable->stagingIsClipRRectOptimization_);
}
} // namespace OHOS::Rosen
