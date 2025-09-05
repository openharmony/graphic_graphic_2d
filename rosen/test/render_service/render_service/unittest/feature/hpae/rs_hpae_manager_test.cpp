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
#include "hgm_core.h"
#include "render/rs_drawing_filter.h"
#include "render/rs_filter.h"
#include "render/rs_render_filter_base.h"
#include "drawable/rs_property_drawable.h"
#include "hpae_base/rs_hpae_base_data.h"
#include "param/sys_param.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Rosen {

constexpr NodeId DEFAULT_ID = 0xFFFF;
constexpr ScreenId DEFAULT_SCREEN_ID = 0xFFFF;

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
void RSHpaeManagerTest::TearDown()
{
    usleep(5000);
}

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
 * @tc.name: OnSyncTestReleaseAllDone
 * @tc.desc: Test OnSync function
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSHpaeManagerTest, OnSyncTestReleaseAllDone, TestSize.Level1)
{
    RSHpaeManager hpaeManager;
    hpaeManager.releaseAllDone_.store(false);
    hpaeManager.OnSync(false);
    ASSERT_EQ(hpaeManager.stagingHpaeStatus_.gotHpaeBlurNode, false);
    hpaeManager.releaseAllDone_.store(true);
}

/**
 * @tc.name: OnSyncTestVsyncId
 * @tc.desc: Test OnSync function
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSHpaeManagerTest, OnSyncTestVsyncId, TestSize.Level1)
{
    RSHpaeManager hpaeManager;
    auto vsyncId = OHOS::Rosen::HgmCore::Instance().GetVsyncId();
    OHOS::Rosen::HgmCore::Instance().SetVsyncId(0);
    hpaeManager.OnSync(true);
    ASSERT_EQ(hpaeManager.stagingHpaeStatus_.gotHpaeBlurNode, false);

    hpaeManager.OnSync(false);
    ASSERT_EQ(hpaeManager.stagingHpaeStatus_.gotHpaeBlurNode, false);

    OHOS::Rosen::HgmCore::Instance().SetVsyncId(1);
    hpaeManager.OnSync(true);
    ASSERT_EQ(hpaeManager.stagingHpaeStatus_.gotHpaeBlurNode, false);

    hpaeManager.OnSync(false);
    ASSERT_EQ(hpaeManager.stagingHpaeStatus_.gotHpaeBlurNode, false);

    OHOS::Rosen::HgmCore::Instance().SetVsyncId(vsyncId);
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
    RSHpaeManager::GetInstance().stagingHpaeStatus_.bufferWidth = 1000;
    RSHpaeManager::GetInstance().stagingHpaeStatus_.bufferHeight = 2000;
    RSHpaeManager::GetInstance().HandleHpaeStateChange();

    RSHpaeManager::GetInstance().stagingHpaeStatus_.gotHpaeBlurNode = true;
    RSHpaeManager::GetInstance().hpaeStatus_.gotHpaeBlurNode = true;
    RSHpaeManager::GetInstance().stagingHpaeStatus_.blurNodeId = 2;
    RSHpaeManager::GetInstance().hpaeStatus_.blurNodeId = 0;
    RSHpaeManager::GetInstance().prevBufferConfig_.width = 1000;
    RSHpaeManager::GetInstance().prevBufferConfig_.height = 2000;
    RSHpaeManager::GetInstance().stagingHpaeStatus_.bufferWidth = 500;
    RSHpaeManager::GetInstance().stagingHpaeStatus_.bufferHeight = 2000;
    RSHpaeManager::GetInstance().HandleHpaeStateChange();

    RSHpaeManager::GetInstance().stagingHpaeStatus_.gotHpaeBlurNode = true;
    RSHpaeManager::GetInstance().hpaeStatus_.gotHpaeBlurNode = true;
    RSHpaeManager::GetInstance().stagingHpaeStatus_.blurNodeId = 2;
    RSHpaeManager::GetInstance().hpaeStatus_.blurNodeId = 0;
    RSHpaeManager::GetInstance().prevBufferConfig_.width = 1000;
    RSHpaeManager::GetInstance().prevBufferConfig_.height = 1500;
    RSHpaeManager::GetInstance().stagingHpaeStatus_.bufferWidth = 1000;
    RSHpaeManager::GetInstance().stagingHpaeStatus_.bufferHeight = 2000;
    RSHpaeManager::GetInstance().HandleHpaeStateChange();
    ASSERT_EQ(RSHpaeManager::GetInstance().hpaeStatus_.gotHpaeBlurNode, true);
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

HWTEST_F(RSHpaeManagerTest, UpdateHpaeStateTest01, TestSize.Level1)
{
    RSHpaeManager hpaeManager;
    hpaeManager.UpdateHpaeState();

    hpaeManager.hpaeStatus_.gotHpaeBlurNode = false;

    hpaeManager.stagingHpaeStatus_.gotHpaeBlurNode = true;
    hpaeManager.hpaeBufferNeedInit_.store(true);
    hpaeManager.UpdateHpaeState();
    EXPECT_EQ(hpaeManager.hpaeFrameState_, HpaeFrameState::ALLOC_BUF);

    hpaeManager.stagingHpaeStatus_.gotHpaeBlurNode = true;
    hpaeManager.hpaeBufferNeedInit_.store(false);
    hpaeManager.UpdateHpaeState();
    EXPECT_EQ(hpaeManager.hpaeFrameState_, HpaeFrameState::ACTIVE);

    hpaeManager.stagingHpaeStatus_.gotHpaeBlurNode = false;
    hpaeManager.hpaeBufferNeedInit_.store(true);
    hpaeManager.UpdateHpaeState();
    EXPECT_EQ(hpaeManager.hpaeFrameState_, HpaeFrameState::IDLE);

    hpaeManager.stagingHpaeStatus_.gotHpaeBlurNode = false;
    hpaeManager.hpaeBufferNeedInit_.store(false);
    hpaeManager.UpdateHpaeState();
    EXPECT_EQ(hpaeManager.hpaeFrameState_, HpaeFrameState::DEACTIVE);
}

HWTEST_F(RSHpaeManagerTest, UpdateHpaeStateTest02, TestSize.Level1)
{
    RSHpaeManager hpaeManager;

    hpaeManager.hpaeStatus_.gotHpaeBlurNode = true;
    hpaeManager.stagingHpaeStatus_.gotHpaeBlurNode = true;
    hpaeManager.hpaeBufferNeedInit_.store(false);
    hpaeManager.UpdateHpaeState();
    EXPECT_EQ(hpaeManager.hpaeFrameState_, HpaeFrameState::WORKING);

    hpaeManager.prevBufferConfig_.width = 100;
    hpaeManager.stagingHpaeStatus_.bufferWidth = 200;
    hpaeManager.prevBufferConfig_.height = 100;
    hpaeManager.stagingHpaeStatus_.bufferHeight = 100;
    hpaeManager.UpdateHpaeState();
    EXPECT_EQ(hpaeManager.hpaeFrameState_, HpaeFrameState::CHANGE_CONFIG);

    hpaeManager.prevBufferConfig_.width = 100;
    hpaeManager.stagingHpaeStatus_.bufferWidth = 100;
    hpaeManager.prevBufferConfig_.height = 100;
    hpaeManager.stagingHpaeStatus_.bufferHeight = 200;
    hpaeManager.UpdateHpaeState();
    EXPECT_EQ(hpaeManager.hpaeFrameState_, HpaeFrameState::CHANGE_CONFIG);

    hpaeManager.prevBufferConfig_.width = 100;
    hpaeManager.stagingHpaeStatus_.bufferWidth = 200;
    hpaeManager.prevBufferConfig_.height = 100;
    hpaeManager.stagingHpaeStatus_.bufferHeight = 200;
    hpaeManager.UpdateHpaeState();
    EXPECT_EQ(hpaeManager.hpaeFrameState_, HpaeFrameState::CHANGE_CONFIG);
}

HWTEST_F(RSHpaeManagerTest, UpdateHpaeStateTest03, TestSize.Level1)
{
    RSHpaeManager hpaeManager;

    hpaeManager.hpaeBufferAllocating_.store(true);
    hpaeManager.releaseAllDone_.store(true);
    hpaeManager.UpdateHpaeState();
    EXPECT_EQ(hpaeManager.hpaeFrameState_, HpaeFrameState::WAITING);

    hpaeManager.hpaeBufferAllocating_.store(true);
    hpaeManager.releaseAllDone_.store(false);
    hpaeManager.UpdateHpaeState();
    EXPECT_EQ(hpaeManager.hpaeFrameState_, HpaeFrameState::WAITING);

    hpaeManager.hpaeBufferAllocating_.store(false);
    hpaeManager.releaseAllDone_.store(false);
    hpaeManager.UpdateHpaeState();
    EXPECT_EQ(hpaeManager.hpaeFrameState_, HpaeFrameState::WAITING);

    hpaeManager.hpaeBufferAllocating_.store(false);
    hpaeManager.releaseAllDone_.store(true);
    hpaeManager.UpdateHpaeState();
    EXPECT_EQ(hpaeManager.hpaeFrameState_, HpaeFrameState::IDLE);
}

HWTEST_F(RSHpaeManagerTest, OnActiveTest01, TestSize.Level1)
{
    RSHpaeManager hpaeManager;
    hpaeManager.hpaeFrameState_ = HpaeFrameState::ACTIVE;
    hpaeManager.OnActive();
    EXPECT_EQ(hpaeManager.hpaeFrameState_, HpaeFrameState::ACTIVE);
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
 * @tc.name: CheckHpaeBlurTest
 * @tc.desc: Test CheckHpaeBlur
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSHpaeManagerTest, CheckHpaeBlurTest01, TestSize.Level1)
{
    GraphicPixelFormat pixelFormat = GraphicPixelFormat::GRAPHIC_PIXEL_FMT_RGBA_8888;
    GraphicColorGamut colorSpace = GraphicColorGamut::GRAPHIC_COLOR_GAMUT_SRGB;

    RSHpaeManager::GetInstance().hpaeStatus_.gotHpaeBlurNode = true;
    RSHpaeManager::GetInstance().CheckHpaeBlur(true, pixelFormat, colorSpace, true);
    RSHpaeManager::GetInstance().CheckHpaeBlur(false, pixelFormat, colorSpace, true);
    RSHpaeManager::GetInstance().CheckHpaeBlur(true, pixelFormat, colorSpace, false);
    RSHpaeManager::GetInstance().CheckHpaeBlur(false, pixelFormat, colorSpace, false);

    RSHpaeManager::GetInstance().hpaeStatus_.gotHpaeBlurNode = false;
    RSHpaeManager::GetInstance().CheckHpaeBlur(true, pixelFormat, colorSpace, true);
    RSHpaeManager::GetInstance().CheckHpaeBlur(false, pixelFormat, colorSpace, true);
    RSHpaeManager::GetInstance().CheckHpaeBlur(true, pixelFormat, colorSpace, false);
    RSHpaeManager::GetInstance().CheckHpaeBlur(false, pixelFormat, colorSpace, false);
    ASSERT_EQ(RSHpaeManager::GetInstance().HasHpaeBlurNode(), false);
}

HWTEST_F(RSHpaeManagerTest, CheckHpaeBlurTest02, TestSize.Level1)
{
    GraphicPixelFormat pixelFormat = GraphicPixelFormat::GRAPHIC_PIXEL_FMT_RGBA_8888;
    GraphicColorGamut colorSpace = GraphicColorGamut::GRAPHIC_COLOR_GAMUT_SRGB;

    RSHpaeManager hpaeManager;
    hpaeManager.hpaeFrameState_ = HpaeFrameState::ALLOC_BUF;
    hpaeManager.hpaeStatus_.gotHpaeBlurNode = true;
    hpaeManager.CheckHpaeBlur(true, pixelFormat, colorSpace, true);
    hpaeManager.CheckHpaeBlur(false, pixelFormat, colorSpace, true);
    hpaeManager.CheckHpaeBlur(true, pixelFormat, colorSpace, false);
    hpaeManager.CheckHpaeBlur(false, pixelFormat, colorSpace, false);
    ASSERT_EQ(hpaeManager.HasHpaeBlurNode(), true);

    hpaeManager.hpaeStatus_.gotHpaeBlurNode = false;
    hpaeManager.CheckHpaeBlur(true, pixelFormat, colorSpace, true);
    hpaeManager.CheckHpaeBlur(false, pixelFormat, colorSpace, true);
    hpaeManager.CheckHpaeBlur(true, pixelFormat, colorSpace, false);
    hpaeManager.CheckHpaeBlur(false, pixelFormat, colorSpace, false);
    ASSERT_EQ(hpaeManager.HasHpaeBlurNode(), false);
}

/**
 * @tc.name: SetUpHpaeSurfaceTest
 * @tc.desc: Test SetUpHpaeSurface
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSHpaeManagerTest, SetUpHpaeSurfaceTest01, TestSize.Level1)
{
    RSHpaeManager hpaeManager;
    BufferRequestConfig prevConfig = hpaeManager.prevBufferConfig_;
    GraphicPixelFormat pixelFormat = GraphicPixelFormat::GRAPHIC_PIXEL_FMT_RGBA_8888;
    RSHpaeBaseData::GetInstance().NotifyBufferUsed(false);
    hpaeManager.SetUpHpaeSurface(pixelFormat, prevConfig.colorGamut, hpaeManager.prevIsHebc_);

    RSHpaeBaseData::GetInstance().NotifyBufferUsed(true); // consume buffer
    hpaeManager.SetUpHpaeSurface(pixelFormat, prevConfig.colorGamut, hpaeManager.prevIsHebc_);

    RSHpaeBaseData::GetInstance().NotifyBufferUsed(false); // don't consume buffer
    hpaeManager.SetUpHpaeSurface(pixelFormat, prevConfig.colorGamut, hpaeManager.prevIsHebc_);

    ASSERT_NE(hpaeManager.curIndex_, 0);
}

/**
 * @tc.name: RegisterHpaeCallbackTest
 * @tc.desc: Test RegisterHpaeCallback
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSHpaeManagerTest, RegisterHpaeCallbackTest, TestSize.Level1)
{
    auto screenNode = std::make_shared<RSScreenRenderNode>(DEFAULT_ID, DEFAULT_SCREEN_ID, context);

    NodeId id0 = 1;
    RSRenderNode node0(id0, context);
    RSHpaeManager::GetInstance().stagingHpaeStatus_.hpaeBlurEnabled = false;
    screenNode->screenInfo_.phyWidth = 1000;
    screenNode->screenInfo_.phyHeight = 2000;
    RSHpaeManager::GetInstance().RegisterHpaeCallback(node0, screenNode);

    RSHpaeManager::GetInstance().stagingHpaeStatus_.hpaeBlurEnabled = true;
    screenNode->screenInfo_.phyWidth = 1000;
    screenNode->screenInfo_.phyHeight = 2000;
    RSHpaeManager::GetInstance().RegisterHpaeCallback(node0, screenNode);

    NodeId id1 = 1;
    RSRenderNode node1(id1, context);
    RSHpaeManager::GetInstance().stagingHpaeStatus_.hpaeBlurEnabled = true;
    node1.renderProperties_.backgroundFilter_ = nullptr;
    screenNode->screenInfo_.phyWidth = 1000;
    screenNode->screenInfo_.phyHeight = 2000;
    RSHpaeManager::GetInstance().RegisterHpaeCallback(node1, screenNode);

    NodeId id2 = 1;
    RSRenderNode node2(id2, context);
    RSHpaeManager::GetInstance().stagingHpaeStatus_.hpaeBlurEnabled = true;
    auto rsDrawingFilter2 = std::make_shared<RSDrawingFilter>(std::make_shared<RSRenderFilterParaBase>());
    node2.renderProperties_.backgroundFilter_ = std::static_pointer_cast<RSFilter>(rsDrawingFilter2);
    screenNode->screenInfo_.phyWidth = 0;
    screenNode->screenInfo_.phyHeight = 0;
    RSHpaeManager::GetInstance().RegisterHpaeCallback(node2, screenNode);
    screenNode->screenInfo_.phyWidth = 0;
    screenNode->screenInfo_.phyHeight = 2000;
    RSHpaeManager::GetInstance().RegisterHpaeCallback(node2, screenNode);
    screenNode->screenInfo_.phyWidth = 0;
    screenNode->screenInfo_.phyHeight = 9000;
    RSHpaeManager::GetInstance().RegisterHpaeCallback(node2, screenNode);
    screenNode->screenInfo_.phyWidth = 1000;
    screenNode->screenInfo_.phyHeight = 0;
    RSHpaeManager::GetInstance().RegisterHpaeCallback(node2, screenNode);
    screenNode->screenInfo_.phyWidth = 1000;
    screenNode->screenInfo_.phyHeight = 2000;
    RSHpaeManager::GetInstance().RegisterHpaeCallback(node2, screenNode);
    screenNode->screenInfo_.phyWidth = 3000;
    screenNode->screenInfo_.phyHeight = 0;
    RSHpaeManager::GetInstance().RegisterHpaeCallback(node2, screenNode);
    screenNode->screenInfo_.phyWidth = 3000;
    screenNode->screenInfo_.phyHeight = 2000;
    RSHpaeManager::GetInstance().RegisterHpaeCallback(node2, screenNode);
    screenNode->screenInfo_.phyWidth = 3000;
    screenNode->screenInfo_.phyHeight = 9000;
    RSHpaeManager::GetInstance().RegisterHpaeCallback(node2, screenNode);

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
    screenNode->screenInfo_.phyWidth = 1000;
    screenNode->screenInfo_.phyHeight = 2000;
    RSHpaeManager::GetInstance().RegisterHpaeCallback(node3, screenNode);
    ASSERT_EQ(RSHpaeManager::GetInstance().stagingHpaeStatus_.gotHpaeBlurNode, false); // no BACKGROUND_FILTER

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
    screenNode->screenInfo_.phyWidth = 1000;
    screenNode->screenInfo_.phyHeight = 2000;
    RSHpaeManager::GetInstance().RegisterHpaeCallback(node4, screenNode);
    ASSERT_EQ(RSHpaeManager::GetInstance().stagingHpaeStatus_.gotHpaeBlurNode, true);

    RSDrawableSlot slot5 = RSDrawableSlot::PIXEL_STRETCH;
    node4.drawableVec_[static_cast<uint32_t>(slot5)] = filterDrawable;
    RSHpaeManager::GetInstance().stagingHpaeStatus_.gotHpaeBlurNode = false;
    screenNode->screenInfo_.phyWidth = 1000;
    screenNode->screenInfo_.phyHeight = 2000;
    RSHpaeManager::GetInstance().RegisterHpaeCallback(node4, screenNode);
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

/**
 * @tc.name: RegisterHpaeCallbackTest1
 * @tc.desc: Test RegisterHpaeCallback
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSHpaeManagerTest, RegisterHpaeCallbackTest1, TestSize.Level1)
{
    auto screenNode = std::make_shared<RSScreenRenderNode>(DEFAULT_ID, DEFAULT_SCREEN_ID, context);
    RSHpaeManager hpaeManager;
    RSRenderNode node(1, context);
    auto filter = std::make_shared<RSDrawingFilter>(std::make_shared<RSRenderFilterParaBase>());
    node.renderProperties_.backgroundFilter_ = std::static_pointer_cast<RSFilter>(filter);

    hpaeManager.stagingHpaeStatus_.hpaeBlurEnabled = false;
    hpaeManager.RegisterHpaeCallback(node, screenNode);
    EXPECT_FALSE(hpaeManager.stagingHpaeStatus_.gotHpaeBlurNode);

    hpaeManager.stagingHpaeStatus_.hpaeBlurEnabled = true;
    hpaeManager.stagingHpaeStatus_.gotHpaeBlurNode = true;
    hpaeManager.RegisterHpaeCallback(node, screenNode);
    EXPECT_TRUE(hpaeManager.stagingHpaeStatus_.gotHpaeBlurNode);

    hpaeManager.stagingHpaeStatus_.hpaeBlurEnabled = true;
    hpaeManager.stagingHpaeStatus_.gotHpaeBlurNode = false;
    hpaeManager.RegisterHpaeCallback(node, nullptr);
    EXPECT_FALSE(hpaeManager.stagingHpaeStatus_.gotHpaeBlurNode);
}

/**
 * @tc.name: ReleaseIoBuffersTest
 * @tc.desc: Test ReleaseIoBuffers
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSHpaeManagerTest, ReleaseIoBuffersTest, TestSize.Level1)
{
    RSHpaeManager hpaeManager;
    hpaeManager.hpaeBufferIn_ = std::make_shared<DrawableV2::RSHpaeBuffer>("HPAEInputLayer4", 33);
    hpaeManager.inBufferVec_ = {HpaeBufferInfo()};

    hpaeManager.hpaeBufferOut_ = std::make_shared<DrawableV2::RSHpaeBuffer>("HPAEOutputLayer5", 44);
    hpaeManager.outBufferVec_ = {HpaeBufferInfo()};

    hpaeManager.curIndex_ = 1;
    hpaeManager.layerFrameIn_[0] = std::make_unique<RSRenderFrame>(nullptr, nullptr);
    hpaeManager.layerFrameOut_[0] = std::make_unique<RSRenderFrame>(nullptr, nullptr);
    hpaeManager.ReleaseIoBuffers();
    EXPECT_EQ(hpaeManager.curIndex_, 0);
}

} // namespace OHOS::Rosen