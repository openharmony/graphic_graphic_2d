/*
 * Copyright (c) 2025 Huawei Device Co., Ltd.
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
#include <parameter.h>
#include <parameters.h>
#include <unistd.h>
#include "common/rs_common_def.h"
#include "gtest/gtest.h"
#include "feature/hpae/rs_hpae_manager.h"
#include "render/rs_drawing_filter.h"
#include "render/rs_filter.h"
#include "render/rs_render_filter_base.h"
#include "drawable/rs_property_drawable.h"
#include "hpae_base/rs_hpae_base_data.h"
#include "param/sys_param.h"


using namespace testing;
using namespace testing::ext;

namespace OHOS::Rosen {
class RSChildrenDrawableAdapter : public RSDrawable {
public:
    RSChildrenDrawableAdapter() = default;
    ~RSChildrenDrawableAdapter() override = default;
    bool OnUpdate(const RSRenderNode& content) override { return true; }
    void OnSync() override {}
    Drawing::RecordingCanvas::DrawFunc CreateDrawFunc() const override
    {
        auto ptr = std::static_pointer_cast<const RSChildrenDrawableAdapter>(shared_from_this());
        return [ptr](Drawing::Canvas* canvas, const Drawing::Rect* rect) {};
    }

private:
    bool OnSharedTransition(const std::shared_ptr<RSRenderNode>& node) { return true; }
    friend class RSRenderNode;
};

class RSHpaeManagerTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
    static inline std::string hpaeSwitch;
    static inline std::string hpaeAaeSwitch;
};

void RSHpaeManagerTest::SetUpTestCase()
{
    hpaeSwitch = OHOS::system::GetParameter("debug.graphic.hpae.blur.enabled", "0");
    hpaeAaeSwitch = OHOS::system::GetParameter("rosen.graphic.hpae.blur.aae.enabled", "0");
    OHOS::system::SetParameter("debug.graphic.hpae.blur.enabled", "1");
    OHOS::system::SetParameter("rosen.graphic.hpae.blur.aae.enabled", "1");
}
void RSHpaeManagerTest::TearDownTestCase()
{
    OHOS::system::SetParameter("debug.graphic.hpae.blur.enabled", hpaeSwitch);
    OHOS::system::SetParameter("rosen.graphic.hpae.blur.aae.enabled", hpaeAaeSwitch);
}
void RSHpaeManagerTest::SetUp() {}
void RSHpaeManagerTest::TearDown() {}

static inline std::weak_ptr<RSContext> context = {};

BufferRequestConfig bufferConfig = {.width = 1000,
    .height = 2000,
    .strideAlignment = 0,
    .format = GraphicPixelFormat::GRAPHIC_PIXEL_FMT_YCBCR_420_SP,
    .usage = 0x1234567890,
    .timeout = 0,
    .colorGamut = GraphicColorGamut::GRAPHIC_COLOR_GAMUT_DISPLAY_P3,
    .transform = GraphicTransformType::GRAPHIC_ROTATE_NONE};

/**
 * @tc.name: OnSyncTest
 * @tc.desc: Test OnSync function
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSHpaeManagerTest, OnSyncTest, TestSize.Level1)
{
    RSHpaeManager::GetInstance().stagingHpaeStatus_.blurNodeId = 1;
    RSHpaeManager::GetInstance().hpaeStatus_.blurNodeId = 1;
    RSHpaeManager::GetInstance().stagingHpaeStatus_.gotHpaeBlurNode = true;

    RSHpaeManager::GetInstance().OnUniRenderStart();
    RSHpaeManager::GetInstance().OnSync(false);
    ASSERT_EQ(RSHpaeManager::GetInstance().HasHpaeBlurNode(), false);

    RSHpaeManager::GetInstance().OnUniRenderStart();
    RSHpaeManager::GetInstance().OnSync(true);
    ASSERT_EQ(RSHpaeManager::GetInstance().HasHpaeBlurNode(), false);
}

/**
 * @tc.name: HandleHpaeStatusChangeTest
 * @tc.desc: Test HandleHpaeStateChange
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSHpaeManagerTest, HandleHpaeStateChangeTest, TestSize.Level1)
{
    RSHpaeManager::GetInstance().stagingHpaeStatus_.blurNodeId = 1;
    RSHpaeManager::GetInstance().hpaeStatus_.blurNodeId = 1;

    RSHpaeManager::GetInstance().stagingHpaeStatus_.gotHpaeBlurNode = false;
    RSHpaeManager::GetInstance().hpaeStatus_.gotHpaeBlurNode = true;
    RSHpaeManager::GetInstance().HandleHpaeStateChange();

    RSHpaeManager::GetInstance().stagingHpaeStatus_.gotHpaeBlurNode = false;
    RSHpaeManager::GetInstance().hpaeStatus_.gotHpaeBlurNode = false;
    RSHpaeManager::GetInstance().HandleHpaeStateChange();

    RSHpaeManager::GetInstance().stagingHpaeStatus_.gotHpaeBlurNode = true;
    RSHpaeManager::GetInstance().hpaeStatus_.gotHpaeBlurNode = false;
    RSHpaeManager::GetInstance().HandleHpaeStateChange();

    RSHpaeManager::GetInstance().stagingHpaeStatus_.gotHpaeBlurNode = true;
    RSHpaeManager::GetInstance().hpaeStatus_.gotHpaeBlurNode = true;
    RSHpaeManager::GetInstance().HandleHpaeStateChange();

    RSHpaeManager::GetInstance().stagingHpaeStatus_.gotHpaeBlurNode = true;
    RSHpaeManager::GetInstance().hpaeStatus_.gotHpaeBlurNode = true;
    RSHpaeManager::GetInstance().stagingHpaeStatus_.blurNodeId = 2;
    RSHpaeManager::GetInstance().hpaeStatus_.blurNodeId = 1;
    RSHpaeManager::GetInstance().HandleHpaeStateChange();


    RSHpaeManager::GetInstance().stagingHpaeStatus_.gotHpaeBlurNode = true;
    RSHpaeManager::GetInstance().hpaeStatus_.gotHpaeBlurNode = true;
    RSHpaeManager::GetInstance().stagingHpaeStatus_.blurNodeId = 0;
    RSHpaeManager::GetInstance().hpaeStatus_.blurNodeId = 0;
    RSHpaeManager::GetInstance().HandleHpaeStateChange();


    RSHpaeManager::GetInstance().stagingHpaeStatus_.gotHpaeBlurNode = true;
    RSHpaeManager::GetInstance().hpaeStatus_.gotHpaeBlurNode = true;
    RSHpaeManager::GetInstance().stagingHpaeStatus_.blurNodeId = 2;
    RSHpaeManager::GetInstance().hpaeStatus_.blurNodeId = 0;
    RSHpaeManager::GetInstance().HandleHpaeStateChange();

    RSHpaeManager::GetInstance().stagingHpaeStatus_.gotHpaeBlurNode = true;
    RSHpaeManager::GetInstance().hpaeStatus_.gotHpaeBlurNode = true;
    RSHpaeManager::GetInstance().stagingHpaeStatus_.blurNodeId = 2;
    RSHpaeManager::GetInstance().hpaeStatus_.blurNodeId = 0;
    RSHpaeManager::GetInstance().prevBufferConfig_.width = 1000;
    RSHpaeManager::GetInstance().prevBufferConfig_.height = 2000;
    RSHpaeManager::GetInstance().hpaeBufferWidth_ = 1000;
    RSHpaeManager::GetInstance().hpaeBufferHeight_ = 2000;
    RSHpaeManager::GetInstance().HandleHpaeStateChange();

    RSHpaeManager::GetInstance().stagingHpaeStatus_.gotHpaeBlurNode = true;
    RSHpaeManager::GetInstance().hpaeStatus_.gotHpaeBlurNode = true;
    RSHpaeManager::GetInstance().stagingHpaeStatus_.blurNodeId = 2;
    RSHpaeManager::GetInstance().hpaeStatus_.blurNodeId = 0;
    RSHpaeManager::GetInstance().prevBufferConfig_.width = 1000;
    RSHpaeManager::GetInstance().prevBufferConfig_.height = 2000;
    RSHpaeManager::GetInstance().hpaeBufferWidth_ = 500;
    RSHpaeManager::GetInstance().hpaeBufferHeight_ = 2000;
    RSHpaeManager::GetInstance().HandleHpaeStateChange();

    RSHpaeManager::GetInstance().stagingHpaeStatus_.gotHpaeBlurNode = true;
    RSHpaeManager::GetInstance().hpaeStatus_.gotHpaeBlurNode = true;
    RSHpaeManager::GetInstance().stagingHpaeStatus_.blurNodeId = 2;
    RSHpaeManager::GetInstance().hpaeStatus_.blurNodeId = 0;
    RSHpaeManager::GetInstance().prevBufferConfig_.width = 1000;
    RSHpaeManager::GetInstance().prevBufferConfig_.height = 1500;
    RSHpaeManager::GetInstance().hpaeBufferWidth_ = 1000;
    RSHpaeManager::GetInstance().hpaeBufferHeight_ = 2000;
    RSHpaeManager::GetInstance().HandleHpaeStateChange();
    ASSERT_EQ(RSHpaeManager::GetInstance().releaseAllDone_, false);
}

/**
 * @tc.name: WaitPreviousReleaseAllTest
 * @tc.desc: Test WaitPreviousReleaseAll
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSHpaeManagerTest, WaitPreviousReleaseAllTest, TestSize.Level1)
{
    RSHpaeManager::GetInstance().releaseAllDone_ = false;
    RSHpaeManager::GetInstance().WaitPreviousReleaseAll();
    ASSERT_EQ(RSHpaeManager::GetInstance().releaseAllDone_, true);
}

/**
 * @tc.name: IsFirstFrameTest
 * @tc.desc: Test IsFirstFrame
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSHpaeManagerTest, IsFirstFrameTest, TestSize.Level1)
{
    RSHpaeManager::GetInstance().hpaeFrameState_ = HpaeFrameState::ACTIVE;
    ASSERT_EQ(RSHpaeManager::GetInstance().IsFirstFrame(), true);
}

/**
 * @tc.name: InitIoBuffersTest
 * @tc.desc: Test InitIoBuffers
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSHpaeManagerTest, InitIoBuffersTest, TestSize.Level1)
{
    RSHpaeManager::GetInstance().InitIoBuffers();
    ASSERT_NE(RSHpaeManager::GetInstance().hpaeBufferIn_, nullptr);

    auto hpaeBufferIn = std::make_shared<DrawableV2::RSHpaeBuffer>("HPAEInputLayer", 777);
    auto hpaeBufferOut = std::make_shared<DrawableV2::RSHpaeBuffer>("HPAEOutputLayer", 888);
    RSHpaeManager::GetInstance().hpaeBufferIn_ = hpaeBufferIn;
    RSHpaeManager::GetInstance().hpaeBufferOut_ = hpaeBufferOut;
    RSHpaeManager::GetInstance().InitIoBuffers();
    ASSERT_NE(RSHpaeManager::GetInstance().hpaeBufferIn_, nullptr);
}

/**
 * @tc.name: InitIoBufferIfNeedTest
 * @tc.desc: Test UpdateBufferIfNeed
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSHpaeManagerTest, UpdateBufferIfNeedTest, TestSize.Level1)
{
    RSHpaeManager::GetInstance().hpaeBufferIn_ = std::make_shared<DrawableV2::RSHpaeBuffer>("HPAEInputLayer2", 11);
    RSHpaeManager::GetInstance().inBufferVec_ = {HpaeBufferInfo()};

    RSHpaeManager::GetInstance().hpaeBufferOut_ = std::make_shared<DrawableV2::RSHpaeBuffer>("HPAEOutputLayer2", 22);
    RSHpaeManager::GetInstance().outBufferVec_ = {HpaeBufferInfo()};

    RSHpaeManager::GetInstance().curIndex_ = 1;
    RSHpaeManager::GetInstance().prevBufferConfig_.width = 500;
    RSHpaeManager::GetInstance().prevBufferConfig_.height = 2000;
    RSHpaeManager::GetInstance().UpdateBufferIfNeed(bufferConfig, false);
    ASSERT_NE(RSHpaeManager::GetInstance().curIndex_, 1);

    RSHpaeManager::GetInstance().prevBufferConfig_.width = 1000;
    RSHpaeManager::GetInstance().prevBufferConfig_.height = 1500;
    RSHpaeManager::GetInstance().UpdateBufferIfNeed(bufferConfig, false);
    ASSERT_NE(RSHpaeManager::GetInstance().curIndex_, 2);

    RSHpaeManager::GetInstance().prevBufferConfig_.width = 1000;
    RSHpaeManager::GetInstance().prevBufferConfig_.height = 2000;
    RSHpaeManager::GetInstance().prevBufferConfig_.colorGamut = GraphicColorGamut::GRAPHIC_COLOR_GAMUT_STANDARD_BT709;
    RSHpaeManager::GetInstance().UpdateBufferIfNeed(bufferConfig, false);
    ASSERT_NE(RSHpaeManager::GetInstance().curIndex_, 2);

    RSHpaeManager::GetInstance().prevBufferConfig_.width = 1000;
    RSHpaeManager::GetInstance().prevBufferConfig_.height = 2000;
    RSHpaeManager::GetInstance().prevBufferConfig_.colorGamut = GraphicColorGamut::GRAPHIC_COLOR_GAMUT_DISPLAY_P3;
    RSHpaeManager::GetInstance().prevBufferConfig_.format = GraphicPixelFormat::GRAPHIC_PIXEL_FMT_YCBCR_420_P;
    RSHpaeManager::GetInstance().UpdateBufferIfNeed(bufferConfig, false);
    ASSERT_NE(RSHpaeManager::GetInstance().curIndex_, 2);

    RSHpaeManager::GetInstance().prevBufferConfig_.width = 1000;
    RSHpaeManager::GetInstance().prevBufferConfig_.height = 2000;
    RSHpaeManager::GetInstance().prevBufferConfig_.colorGamut = GraphicColorGamut::GRAPHIC_COLOR_GAMUT_DISPLAY_P3;
    RSHpaeManager::GetInstance().prevBufferConfig_.format = GraphicPixelFormat::GRAPHIC_PIXEL_FMT_YCBCR_420_SP;
    RSHpaeManager::GetInstance().prevIsHebc_ = true;
    RSHpaeManager::GetInstance().UpdateBufferIfNeed(bufferConfig, false);
    ASSERT_NE(RSHpaeManager::GetInstance().curIndex_, 2);

    RSHpaeManager::GetInstance().prevBufferConfig_.width = 1000;
    RSHpaeManager::GetInstance().prevBufferConfig_.height = 2000;
    RSHpaeManager::GetInstance().prevBufferConfig_.colorGamut = GraphicColorGamut::GRAPHIC_COLOR_GAMUT_DISPLAY_P3;
    RSHpaeManager::GetInstance().prevBufferConfig_.format = GraphicPixelFormat::GRAPHIC_PIXEL_FMT_YCBCR_420_SP;
    RSHpaeManager::GetInstance().prevIsHebc_ = false;
    RSHpaeManager::GetInstance().UpdateBufferIfNeed(bufferConfig, false);
    ASSERT_NE(RSHpaeManager::GetInstance().curIndex_, 2);
}

/**
 * @tc.name: SetUpSurfaceInTest
 * @tc.desc: Test SetUpSurfaceIn
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSHpaeManagerTest, SetUpSurfaceInTest, TestSize.Level1)
{
    RSHpaeManager::GetInstance().hpaeBufferIn_ = nullptr;
    RSHpaeManager::GetInstance().SetUpSurfaceIn(bufferConfig, false);

    RSHpaeManager::GetInstance().curIndex_ = 0;
    RSHpaeManager::GetInstance().hpaeBufferIn_ = std::make_shared<DrawableV2::RSHpaeBuffer>("HPAEInputLayer2", 222);
    RSHpaeManager::GetInstance().inBufferVec_ = {HpaeBufferInfo()};
    RSHpaeManager::GetInstance().SetUpSurfaceIn(bufferConfig, false);

    RSHpaeManager::GetInstance().curIndex_ = 1;
    RSHpaeManager::GetInstance().layerFrameIn_[1] = std::make_unique<RSRenderFrame>(nullptr, nullptr);
    RSHpaeManager::GetInstance().SetUpSurfaceIn(bufferConfig, false);
    ASSERT_NE(RSHpaeManager::GetInstance().inBufferVec_.size(), 0);
}

/**
 * @tc.name: SetUpSurfaceOutTest
 * @tc.desc: Test SetUpSurfaceOut
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSHpaeManagerTest, SetUpSurfaceOutTest, TestSize.Level1)
{
    RSHpaeManager::GetInstance().hpaeBufferOut_ = nullptr;
    RSHpaeManager::GetInstance().SetUpSurfaceOut(bufferConfig, false);

    RSHpaeManager::GetInstance().curIndex_ = 0;
    RSHpaeManager::GetInstance().hpaeBufferOut_ = std::make_shared<DrawableV2::RSHpaeBuffer>("HPAEInputLayer3", 333);
    RSHpaeManager::GetInstance().outBufferVec_ = {HpaeBufferInfo()};
    RSHpaeManager::GetInstance().SetUpSurfaceOut(bufferConfig, false);

    RSHpaeManager::GetInstance().curIndex_ = 1;
    RSHpaeManager::GetInstance().layerFrameOut_[1] = std::make_unique<RSRenderFrame>(nullptr, nullptr);
    RSHpaeManager::GetInstance().SetUpSurfaceOut(bufferConfig, false);
    ASSERT_NE(RSHpaeManager::GetInstance().outBufferVec_.size(), 0);
}

/**
 * @tc.name: SetUpHpaeSurfaceTest
 * @tc.desc: Test SetUpHpaeSurface
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSHpaeManagerTest, SetUpHpaeSurfaceTest, TestSize.Level1)
{
    RSHpaeManager::GetInstance().hpaeBufferIn_ = nullptr;
    RSHpaeManager::GetInstance().hpaeBufferOut_ = nullptr;
    RSHpaeManager::GetInstance().SetUpHpaeSurface(
        GraphicPixelFormat::GRAPHIC_PIXEL_FMT_YCBCR_420_SP, GraphicColorGamut::GRAPHIC_COLOR_GAMUT_DISPLAY_P3, false);

    RSHpaeManager::GetInstance().hpaeBufferIn_ = std::make_shared<DrawableV2::RSHpaeBuffer>("HPAEInputLayer", 777);
    RSHpaeManager::GetInstance().hpaeBufferOut_ = nullptr;
    RSHpaeManager::GetInstance().SetUpHpaeSurface(
        GraphicPixelFormat::GRAPHIC_PIXEL_FMT_YCBCR_420_SP, GraphicColorGamut::GRAPHIC_COLOR_GAMUT_DISPLAY_P3, false);

    RSHpaeManager::GetInstance().prevBufferConfig_ = bufferConfig;
    RSHpaeManager::GetInstance().InitIoBuffers();
    RSHpaeBaseData::GetInstance().bufferUsed_ = false;
    RSHpaeManager::GetInstance().SetUpHpaeSurface(
        GraphicPixelFormat::GRAPHIC_PIXEL_FMT_YCBCR_420_SP, GraphicColorGamut::GRAPHIC_COLOR_GAMUT_DISPLAY_P3, false);

    RSHpaeManager::GetInstance().prevBufferConfig_ = bufferConfig;
    RSHpaeManager::GetInstance().InitIoBuffers();
    RSHpaeBaseData::GetInstance().bufferUsed_ = true;
    RSHpaeManager::GetInstance().SetUpHpaeSurface(
        GraphicPixelFormat::GRAPHIC_PIXEL_FMT_YCBCR_420_SP, GraphicColorGamut::GRAPHIC_COLOR_GAMUT_DISPLAY_P3, false);

    ASSERT_NE(RSHpaeManager::GetInstance().curIndex_, 1);
}

/**
 * @tc.name: RegisterHpaeCallbackTest
 * @tc.desc: Test RegisterHpaeCallback
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSHpaeManagerTest, RegisterHpaeCallbackTest, TestSize.Level1)
{
    NodeId id0 = 1;
    RSRenderNode node0(id0, context);
    RSHpaeManager::GetInstance().stagingHpaeStatus_.hpaeBlurEnabled = false;
    RSHpaeManager::GetInstance().RegisterHpaeCallback(node0, 1000, 2000);

    RSHpaeManager::GetInstance().stagingHpaeStatus_.hpaeBlurEnabled = true;
    RSHpaeManager::GetInstance().RegisterHpaeCallback(node0, 1000, 2000);

    NodeId id1 = 1;
    RSRenderNode node1(id1, context);
    RSHpaeManager::GetInstance().stagingHpaeStatus_.hpaeBlurEnabled = true;
    node1.renderProperties_.backgroundFilter_ = nullptr;
    RSHpaeManager::GetInstance().RegisterHpaeCallback(node1, 1000, 2000);

    NodeId id2 = 1;
    RSRenderNode node2(id2, context);
    RSHpaeManager::GetInstance().stagingHpaeStatus_.hpaeBlurEnabled = true;
    auto rsDrawingFilter2 = std::make_shared<RSDrawingFilter>(std::make_shared<RSRenderFilterParaBase>());
    node2.renderProperties_.backgroundFilter_ = std::static_pointer_cast<RSFilter>(rsDrawingFilter2);
    RSHpaeManager::GetInstance().RegisterHpaeCallback(node2, 0, 0);
    RSHpaeManager::GetInstance().RegisterHpaeCallback(node2, 0, 2000);
    RSHpaeManager::GetInstance().RegisterHpaeCallback(node2, 0, 9000);
    RSHpaeManager::GetInstance().RegisterHpaeCallback(node2, 1000, 0);
    RSHpaeManager::GetInstance().RegisterHpaeCallback(node2, 1000, 2000);
    RSHpaeManager::GetInstance().RegisterHpaeCallback(node2, 3000, 0);
    RSHpaeManager::GetInstance().RegisterHpaeCallback(node2, 3000, 2000);
    RSHpaeManager::GetInstance().RegisterHpaeCallback(node2, 3000, 9000);

    NodeId id3 = 1;
    RSRenderNode node3(id3, context);
    RSHpaeManager::GetInstance().stagingHpaeStatus_.hpaeBlurEnabled = true;
    auto rsDrawingFilter3 = std::make_shared<RSDrawingFilter>(std::make_shared<RSRenderFilterParaBase>());
    std::shared_ptr<RSFilter> backgroundFilter3 = std::static_pointer_cast<RSFilter>(rsDrawingFilter3);
    RectI rect3{0, 0, 1000, 2000};
    node3.renderProperties_.boundsGeo_->absRect_ = rect3;
    node3.renderProperties_.backgroundBlurRadius_ = 20.0f;
    node3.renderProperties_.backgroundFilter_ = backgroundFilter3;
    node3.renderProperties_.SetBoundsWidth(1000);
    node3.renderProperties_.SetBoundsHeight(2000);
    RSHpaeManager::GetInstance().RegisterHpaeCallback(node3, 1000, 2000);
    ASSERT_EQ(RSHpaeManager::GetInstance().stagingHpaeStatus_.gotHpaeBlurNode, true);

    RSHpaeManager::GetInstance().stagingHpaeStatus_.hpaeBlurEnabled = true;
    RSHpaeManager::GetInstance().stagingHpaeStatus_.gotHpaeBlurNode = false;
    NodeId id4 = 4;
    RSRenderNode node4(id4, context);
    RSDrawableSlot slot4 = RSDrawableSlot::BACKGROUND_FILTER;
    auto filterDrawable = std::make_shared<RSChildrenDrawableAdapter>();
    node4.drawableVec_[static_cast<uint32_t>(slot4)] = filterDrawable;
    auto rsDrawingFilter5 = std::make_shared<RSDrawingFilter>(std::make_shared<RSRenderFilterParaBase>());
    std::shared_ptr<RSFilter> backgroundFilter4 = std::static_pointer_cast<RSFilter>(rsDrawingFilter5);
    node4.renderProperties_.backgroundBlurRadius_ = 20.0f;
    node4.renderProperties_.backgroundFilter_ = backgroundFilter4;
    RectI rect1{0, 0, 1000, 2000};
    node4.renderProperties_.boundsGeo_->absRect_ = rect1;
    RSHpaeManager::GetInstance().RegisterHpaeCallback(node4, 1000, 2000);
    ASSERT_EQ(RSHpaeManager::GetInstance().stagingHpaeStatus_.gotHpaeBlurNode, true);

    RSDrawableSlot slot5 = RSDrawableSlot::PIXEL_STRETCH;
    node4.drawableVec_[static_cast<uint32_t>(slot5)] = filterDrawable;
    RSHpaeManager::GetInstance().stagingHpaeStatus_.gotHpaeBlurNode = false;
    RSHpaeManager::GetInstance().RegisterHpaeCallback(node4, 1000, 2000);
    ASSERT_EQ(RSHpaeManager::GetInstance().stagingHpaeStatus_.gotHpaeBlurNode, true);
}

/**
 * @tc.name: IsHpaeBlurNodeTest
 * @tc.desc: Test IsHpaeBlurNode
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSHpaeManagerTest, IsHpaeBlurNodeTest, TestSize.Level1)
{
    NodeId id0 = 10;
    RSRenderNode node0(id0, context);
    RSHpaeManager::GetInstance().stagingHpaeStatus_.gotHpaeBlurNode = true;
    ASSERT_TRUE(RSHpaeManager::GetInstance().IsHpaeBlurNode(node0, 1000, 2000) == false);

    RSHpaeManager::GetInstance().stagingHpaeStatus_.gotHpaeBlurNode = false;
    NodeId id1 = 1;
    RSRenderNode node1(id1, context);
    ASSERT_TRUE(RSHpaeManager::GetInstance().IsHpaeBlurNode(node1, 1000, 2000) == false);

    RSHpaeManager::GetInstance().stagingHpaeStatus_.gotHpaeBlurNode = false;
    NodeId id2 = 2;
    RSRenderNode node2(id2, context);
    RSDrawableSlot slot2 = RSDrawableSlot::BACKGROUND_FILTER;
    auto filterDrawable = std::make_shared<RSChildrenDrawableAdapter>();
    node2.drawableVec_[static_cast<uint32_t>(slot2)] = filterDrawable;
    ASSERT_TRUE(RSHpaeManager::GetInstance().IsHpaeBlurNode(node2, 1000, 2000) == false);

    RSHpaeManager::GetInstance().stagingHpaeStatus_.gotHpaeBlurNode = false;
    std::shared_ptr<RSRenderNode> nodeChild = std::make_shared<RSRenderNode>(1);
    std::vector<std::shared_ptr<RSRenderNode>> childs = {nodeChild};
    std::shared_ptr<const std::vector<std::shared_ptr<RSRenderNode>>> childList =
        std::make_shared<const std::vector<std::shared_ptr<RSRenderNode>>>(std::move(childs));
    auto EmptyChildrenList = std::make_shared<const std::vector<std::shared_ptr<RSRenderNode>>>();
    node2.fullChildrenList_ = childList;
    ASSERT_TRUE(RSHpaeManager::GetInstance().IsHpaeBlurNode(node2, 1000, 2000) == false);

    node2.fullChildrenList_ = nullptr;
    ASSERT_TRUE(RSHpaeManager::GetInstance().IsHpaeBlurNode(node2, 1000, 2000) == false);

    RSHpaeManager::GetInstance().stagingHpaeStatus_.gotHpaeBlurNode = false;
    NodeId id3 = 3;
    RSRenderNode node3(id3, context);
    RSDrawableSlot slot3 = RSDrawableSlot::BACKGROUND_FILTER;
    node3.drawableVec_[static_cast<uint32_t>(slot3)] = filterDrawable;
    auto rsDrawingFilter1 = std::make_shared<RSDrawingFilter>(std::make_shared<RSRenderFilterParaBase>());
    std::shared_ptr<RSFilter> backgroundFilter1 = std::static_pointer_cast<RSFilter>(rsDrawingFilter1);

    node3.renderProperties_.backgroundFilter_ = backgroundFilter1;
    node3.renderProperties_.backgroundBlurRadius_ = 20.0f;
    node3.renderProperties_.SetBoundsWidth(1000);
    node3.renderProperties_.SetBoundsHeight(2000);
    ASSERT_TRUE(RSHpaeManager::GetInstance().IsHpaeBlurNode(node3, 1000, 2000) == false);

    RSHpaeManager::GetInstance().stagingHpaeStatus_.gotHpaeBlurNode = false;
    NodeId id4 = 3;
    RSRenderNode node4(id4, context);
    RSDrawableSlot slot4 = RSDrawableSlot::BACKGROUND_FILTER;
    node4.drawableVec_[static_cast<uint32_t>(slot4)] = filterDrawable;
    auto rsDrawingFilter2 = std::make_shared<RSDrawingFilter>(std::make_shared<RSRenderFilterParaBase>());
    std::shared_ptr<RSFilter> backgroundFilter2 = std::static_pointer_cast<RSFilter>(rsDrawingFilter2);
    node4.renderProperties_.backgroundBlurRadius_ = 1.0f;
    node4.renderProperties_.backgroundFilter_ = backgroundFilter2;
    ASSERT_TRUE(RSHpaeManager::GetInstance().IsHpaeBlurNode(node4, 1000, 2000) == false);
    node4.renderProperties_.backgroundBlurRadius_ = 500.0f;
    ASSERT_TRUE(RSHpaeManager::GetInstance().IsHpaeBlurNode(node4, 1000, 2000) == false);

    RSHpaeManager::GetInstance().stagingHpaeStatus_.gotHpaeBlurNode = false;
    NodeId id5 = 5;
    RSRenderNode node5(id5, context);
    RSDrawableSlot slot5 = RSDrawableSlot::BACKGROUND_FILTER;
    node5.drawableVec_[static_cast<uint32_t>(slot5)] = filterDrawable;
    auto rsDrawingFilter5 = std::make_shared<RSDrawingFilter>(std::make_shared<RSRenderFilterParaBase>());
    std::shared_ptr<RSFilter> backgroundFilter5 = std::static_pointer_cast<RSFilter>(rsDrawingFilter5);
    node5.renderProperties_.backgroundBlurRadius_ = 20.0f;
    node5.renderProperties_.backgroundFilter_ = backgroundFilter5;
    RectI rect1{0, 0, 1000, 2000};
    node5.renderProperties_.boundsGeo_->absRect_ = rect1;
    ASSERT_TRUE(RSHpaeManager::GetInstance().IsHpaeBlurNode(node5, 1000, 2000) == true);
    RectI rect2{0, 0, 500, 2000};
    node5.renderProperties_.boundsGeo_->absRect_ = rect2;
    ASSERT_TRUE(RSHpaeManager::GetInstance().IsHpaeBlurNode(node5, 1000, 2000) == false);
    RectI rect3{0, 0, 1000, 1500};
    node5.renderProperties_.boundsGeo_->absRect_ = rect3;
    ASSERT_TRUE(RSHpaeManager::GetInstance().IsHpaeBlurNode(node5, 1000, 2000) == false);
    RectI rect4{0, 0, 500, 1500};
    node5.renderProperties_.boundsGeo_->absRect_ = rect4;
    ASSERT_TRUE(RSHpaeManager::GetInstance().IsHpaeBlurNode(node5, 1000, 2000) == false);
}

} // namespace OHOS::Rosen