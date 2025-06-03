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

#include "gtest/gtest.h"
#include "pipeline/hardware_thread/rs_hardware_thread.h"
#include "pipeline/render_thread/rs_uni_render_composer_adapter.h"
#include "pipeline/render_thread/rs_uni_render_engine.h"
#include "screen_manager/rs_screen.h"
#include "foundation/graphic/graphic_2d/rosen/test/render_service/render_service/unittest/pipeline/rs_test_util.h"
#include "foundation/graphic/graphic_2d/rosen/test/render_service/render_service/unittest/pipeline/mock/mock_hdi_device.h"
#include "gfx/fps_info/rs_surface_fps_manager.h"

using namespace testing;
using namespace testing::ext;

namespace {
    const int DEFAULT_WIDTH = 2160;
    const int DEFAULT_HEIGHT = 1080;
};

namespace OHOS::Rosen {
class RSHardwareThreadTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
    void CreateComposerAdapterWithScreenInfo(uint32_t width = 2560, uint32_t height = 1080,
        ScreenColorGamut colorGamut = ScreenColorGamut::COLOR_GAMUT_SRGB,
        ScreenState state = ScreenState::UNKNOWN,
        ScreenRotation rotation = ScreenRotation::ROTATION_0);
public:
    std::unique_ptr<RSComposerAdapter> composerAdapter_;
    sptr<OHOS::Rosen::RSScreenManager> screenManager_;

    int32_t offsetX = 0; // screenOffset on x axis equals to 0
    int32_t offsetY = 0; // screenOffset on y axis equals to 0
    float mirrorAdaptiveCoefficient = 1.0f;

    uint32_t screenId_ = 10;

    static inline BufferRequestConfig requestConfig = {
        .width = 0x100,
        .height = 0x100,
        .strideAlignment = 0x8,
        .format = GRAPHIC_PIXEL_FMT_RGBA_8888,
        .usage = BUFFER_USAGE_CPU_READ | BUFFER_USAGE_CPU_WRITE | BUFFER_USAGE_MEM_DMA,
        .timeout = 0,
        .colorGamut = GRAPHIC_COLOR_GAMUT_SRGB,
    };

    static inline BufferFlushConfig flushConfig = {
        .damage = { .w = 0x100, .h = 0x100, },
    };
    static inline Mock::HdiDeviceMock* hdiDeviceMock_;
};

void RSHardwareThreadTest::SetUpTestCase()
{
#ifdef RS_ENABLE_VK
    RsVulkanContext::SetRecyclable(false);
#endif
    hdiDeviceMock_ = Mock::HdiDeviceMock::GetInstance();
    EXPECT_CALL(*hdiDeviceMock_, RegHotPlugCallback(_, _)).WillRepeatedly(testing::Return(0));
    EXPECT_CALL(*hdiDeviceMock_, RegHwcDeadCallback(_, _)).WillRepeatedly(testing::Return(false));
    EXPECT_CALL(*hdiDeviceMock_, PrepareScreenLayers(_, _)).WillRepeatedly(testing::Return(0));
    EXPECT_CALL(*hdiDeviceMock_, GetScreenCompChange(_, _, _)).WillRepeatedly(testing::Return(0));
    EXPECT_CALL(*hdiDeviceMock_, Commit(_, _)).WillRepeatedly(testing::Return(0));
    RSTestUtil::InitRenderNodeGC();
}
void RSHardwareThreadTest::TearDownTestCase() {}
void RSHardwareThreadTest::TearDown()
{
    screenManager_ = OHOS::Rosen::impl::RSScreenManager::GetInstance();
    OHOS::Rosen::impl::RSScreenManager& screenManager =
        static_cast<OHOS::Rosen::impl::RSScreenManager&>(*screenManager_);
    screenManager.screens_.erase(screenId_);
}
void RSHardwareThreadTest::SetUp()
{
    screenManager_ = OHOS::Rosen::impl::RSScreenManager::GetInstance();
    auto rsScreen = std::make_shared<impl::RSScreen>(screenId_, true, HdiOutput::CreateHdiOutput(screenId_), nullptr);
    screenId_ = rsScreen->Id();
    screenManager_->MockHdiScreenConnected(rsScreen);
    CreateComposerAdapterWithScreenInfo(DEFAULT_WIDTH, DEFAULT_HEIGHT,
        ScreenColorGamut::COLOR_GAMUT_SRGB, ScreenState::UNKNOWN, ScreenRotation::ROTATION_0);
    auto mainThread = RSMainThread::Instance();
    if (mainThread->rsVSyncDistributor_ == nullptr) {
        auto vsyncGenerator = CreateVSyncGenerator();
        auto vsyncController = new VSyncController(vsyncGenerator, 0);
        mainThread->rsVSyncDistributor_ = new VSyncDistributor(vsyncController, "rs");
        vsyncGenerator->SetRSDistributor(mainThread->rsVSyncDistributor_);
    }
}

void RSHardwareThreadTest::CreateComposerAdapterWithScreenInfo(uint32_t width, uint32_t height,
    ScreenColorGamut colorGamut, ScreenState state, ScreenRotation rotation)
{
    auto info = screenManager_->QueryScreenInfo(screenId_);
    info.width = width;
    info.height = height;
    info.phyWidth = width;
    info.phyHeight = height;
    info.colorGamut = colorGamut;
    info.state = state;
    info.rotation = rotation;
    composerAdapter_ = std::make_unique<RSComposerAdapter>();
    composerAdapter_->Init(info, offsetX, offsetY, mirrorAdaptiveCoefficient, nullptr);
    composerAdapter_->SetHdiBackendDevice(hdiDeviceMock_);
}

/**
 * @tc.name: ClearFrameBuffers001
 * @tc.desc: Test RSHardwareThreadTest.ClearFrameBuffers
 * @tc.type: FUNC
 * @tc.require: issueI6R49K
 */
HWTEST_F(RSHardwareThreadTest, ClearFrameBuffers001, TestSize.Level1)
{
    auto& hardwareThread = RSHardwareThread::Instance();
    auto hdiOutput = HdiOutput::CreateHdiOutput(screenId_);
    ASSERT_NE(hdiOutput, nullptr);
    if (hdiOutput->GetFrameBufferSurface()) {
        GSError ret = hardwareThread.ClearFrameBuffers(hdiOutput);
        ASSERT_EQ(ret, GSERROR_OK);
    }
}

/**
 * @tc.name: Start001
 * @tc.desc: Test RSHardwareThreadTest.Start
 * @tc.type: FUNC
 * @tc.require: issueI6R49K
 */
HWTEST_F(RSHardwareThreadTest, Start001, TestSize.Level1)
{
    auto& hardwareThread = RSHardwareThread::Instance();
    hardwareThread.Start();
    ASSERT_NE(hardwareThread.hdiBackend_, nullptr);
}

/**
 * @tc.name: Start002
 * @tc.desc: Test RSHardwareThreadTest.PostTask
 * @tc.type: FUNC
 * @tc.require: issueI6R49K
 */
HWTEST_F(RSHardwareThreadTest, Start002, TestSize.Level1)
{
    auto& hardwareThread = RSHardwareThread::Instance();
    hardwareThread.PostTask([&]() {});
    hardwareThread.Start();
    ASSERT_NE(hardwareThread.hdiBackend_, nullptr);
    hardwareThread.PostTask([&]() {});
    hardwareThread.ScheduleTask([=]() {}).wait();
    hardwareThread.PostSyncTask([&]() {});
}

/**
 * @tc.name: Start003
 * @tc.desc: Test RSHardwareThreadTest.CommitAndReleaseLayers
 * @tc.type: FUNC
 * @tc.require: issueI6R49K
 */
HWTEST_F(RSHardwareThreadTest, Start003, TestSize.Level1)
{
    auto& hardwareThread = RSHardwareThread::Instance();
    hardwareThread.Start();
    SetUp();
    auto surfaceNode1 = RSTestUtil::CreateSurfaceNodeWithBuffer();
    auto surfaceNode2 = RSTestUtil::CreateSurfaceNodeWithBuffer();
    auto surfaceNode3 = RSTestUtil::CreateSurfaceNodeWithBuffer();
    RectI dstRect{0, 0, 400, 600};
    surfaceNode1->SetSrcRect(dstRect);
    surfaceNode1->SetDstRect(dstRect);
    surfaceNode2->SetSrcRect(dstRect);
    surfaceNode2->SetDstRect(dstRect);
    surfaceNode3->SetSrcRect(dstRect);
    surfaceNode3->SetDstRect(dstRect);
    auto layer1 = composerAdapter_->CreateLayer(*surfaceNode1);
    ASSERT_NE(layer1, nullptr);
    auto layer2 = composerAdapter_->CreateLayer(*surfaceNode2);
    ASSERT_NE(layer2, nullptr);
    auto layer3 = composerAdapter_->CreateLayer(*surfaceNode3);
    ASSERT_NE(layer3, nullptr);

    std::vector<LayerInfoPtr> layers;
    layers.emplace_back(layer1);
    layers.emplace_back(layer2);
    layers.emplace_back(layer3);
    auto& uniRenderThread = RSUniRenderThread::Instance();
    uniRenderThread.Sync(std::make_unique<RSRenderThreadParams>());
    hardwareThread.CommitAndReleaseLayers(composerAdapter_->output_, layers);
    auto &hgmCore = HgmCore::Instance();
    ScreenId curScreenId = hgmCore.GetFrameRateMgr()->GetCurScreenId();
    ScreenId lastCurScreenId = hgmCore.GetFrameRateMgr()->GetLastCurScreenId();
    ASSERT_EQ(curScreenId, lastCurScreenId);
}

/**
 * @tc.name: Start004
 * @tc.desc: Test RSHardwareThreadTest.ReleaseBuffer
 * @tc.type: FUNC
 * @tc.require: issueI6R49K
 */
HWTEST_F(RSHardwareThreadTest, Start004, TestSize.Level1)
{
    auto rsSurfaceRenderNode = RSTestUtil::CreateSurfaceNode();
    const auto& surfaceConsumer = rsSurfaceRenderNode->GetRSSurfaceHandler()->GetConsumer();
    auto producer = surfaceConsumer->GetProducer();
    sptr<Surface> sProducer = Surface::CreateSurfaceAsProducer(producer);
    sProducer->SetQueueSize(1);
    sptr<SurfaceBuffer> buffer;
    sptr<SyncFence> requestFence = SyncFence::INVALID_FENCE;
    GSError ret = sProducer->RequestBuffer(buffer, requestFence, requestConfig);
    ASSERT_EQ(ret, GSERROR_OK);
    sptr<SyncFence> flushFence = SyncFence::INVALID_FENCE;
    ret = sProducer->FlushBuffer(buffer, flushFence, flushConfig);
    ASSERT_EQ(ret, GSERROR_OK);
    OHOS::sptr<SurfaceBuffer> cbuffer;
    Rect damage;
    sptr<SyncFence> acquireFence = SyncFence::INVALID_FENCE;
    int64_t timestamp = 0;
    ret = surfaceConsumer->AcquireBuffer(cbuffer, acquireFence, timestamp, damage);
    ASSERT_EQ(ret, GSERROR_OK);
}

/**
 * @tc.name: Start005
 * @tc.desc: Test RSHardwareThreadTest.AddRefreshRateCount
 * @tc.type: FUNC
 * @tc.require: issueI8K4HE
 */
HWTEST_F(RSHardwareThreadTest, Start005, TestSize.Level1)
{
    OutputPtr output = HdiOutput::CreateHdiOutput(screenId_);
    auto& hardwareThread = RSHardwareThread::Instance();
    auto count = hardwareThread.refreshRateCounts_;
    hardwareThread.AddRefreshRateCount(output);
    ASSERT_TRUE(count != hardwareThread.refreshRateCounts_);
}


/**
 * @tc.name: HardcursorLayerTest001
 * @tc.desc: Test Hardcursor layer zorder is not max
 * @tc.type: FUNC
 * @tc.require: issueI6R49K
 */
HWTEST_F(RSHardwareThreadTest, HardcursorLayerTest001, TestSize.Level1)
{
    auto& hardwareThread = RSHardwareThread::Instance();
    hardwareThread.Start();
    SetUp();
    auto surfaceNode1 = RSTestUtil::CreateSurfaceNodeWithBuffer();
    auto surfaceNode2 = RSTestUtil::CreateSurfaceNodeWithBuffer();
    auto surfaceNode3 = RSTestUtil::CreateSurfaceNodeWithBuffer();
    RectI dstRect{0, 0, 400, 600};
    surfaceNode1->SetSrcRect(dstRect);
    surfaceNode1->SetDstRect(dstRect);
    surfaceNode2->SetSrcRect(dstRect);
    surfaceNode2->SetDstRect(dstRect);
    surfaceNode3->SetSrcRect(dstRect);
    surfaceNode3->SetDstRect(dstRect);
    auto layer1 = composerAdapter_->CreateLayer(*surfaceNode1);
    ASSERT_NE(layer1, nullptr);
    auto layer2 = composerAdapter_->CreateLayer(*surfaceNode2);
    ASSERT_NE(layer2, nullptr);
    auto layer3 = composerAdapter_->CreateLayer(*surfaceNode3);
    ASSERT_NE(layer3, nullptr);

    layer1->SetZorder(1);
    layer2->SetZorder(2);
    layer2->SetType(GraphicLayerType::GRAPHIC_LAYER_TYPE_CURSOR);
    layer3->SetZorder(3);

    std::vector<LayerInfoPtr> layers;
    layers.emplace_back(layer1);
    layers.emplace_back(layer2);
    layers.emplace_back(layer3);
    std::string res = hardwareThread.GetSurfaceNameInLayersForTrace(layers);
    ASSERT_NE(res.find("zorder: 2"), -1);
}

/**
 * @tc.name: HardcursorLayerTest002
 * @tc.desc: Test Hardcursor layer zorder is max
 * @tc.type: FUNC
 * @tc.require: issueI6R49K
 */
HWTEST_F(RSHardwareThreadTest, HardcursorLayerTest002, TestSize.Level1)
{
    auto& hardwareThread = RSHardwareThread::Instance();
    hardwareThread.Start();
    SetUp();
    auto surfaceNode1 = RSTestUtil::CreateSurfaceNodeWithBuffer();
    auto surfaceNode2 = RSTestUtil::CreateSurfaceNodeWithBuffer();
    auto surfaceNode3 = RSTestUtil::CreateSurfaceNodeWithBuffer();
    RectI dstRect{0, 0, 400, 600};
    surfaceNode1->SetSrcRect(dstRect);
    surfaceNode1->SetDstRect(dstRect);
    surfaceNode2->SetSrcRect(dstRect);
    surfaceNode2->SetDstRect(dstRect);
    surfaceNode3->SetSrcRect(dstRect);
    surfaceNode3->SetDstRect(dstRect);
    auto layer1 = composerAdapter_->CreateLayer(*surfaceNode1);
    ASSERT_NE(layer1, nullptr);
    auto layer2 = composerAdapter_->CreateLayer(*surfaceNode2);
    ASSERT_NE(layer2, nullptr);
    auto layer3 = composerAdapter_->CreateLayer(*surfaceNode3);
    ASSERT_NE(layer3, nullptr);

    layer1->SetZorder(1);
    layer2->SetZorder(8);
    layer2->SetType(GraphicLayerType::GRAPHIC_LAYER_TYPE_CURSOR);
    layer3->SetZorder(3);

    std::vector<LayerInfoPtr> layers;
    layers.emplace_back(layer1);
    layers.emplace_back(layer2);
    layers.emplace_back(layer3);
    std::string res = hardwareThread.GetSurfaceNameInLayersForTrace(layers);
    ASSERT_NE(res.find("zorder: 8"), -1);
}


/**
 * @tc.name: ClearFrameBuffers002
 * @tc.desc: Test RSHardwareThreadTest.ClearFrameBuffers
 * @tc.type: FUNC
 * @tc.require: issueI6R49K
 */
HWTEST_F(RSHardwareThreadTest, ClearFrameBuffers002, TestSize.Level1)
{
    auto& hardwareThread = RSHardwareThread::Instance();
    GSError ret = hardwareThread.ClearFrameBuffers(nullptr);
    ASSERT_EQ(ret, GSERROR_INVALID_ARGUMENTS);
    auto hdiOutput = HdiOutput::CreateHdiOutput(screenId_);
    if (hdiOutput->GetFrameBufferSurface()) {
        GSError ret = hardwareThread.ClearFrameBuffers(hdiOutput);
        ASSERT_EQ(ret, GSERROR_OK);
    }
}

/**
 * @tc.name: IsDelayRequired001
 * @tc.desc: Test RSHardwareThreadTest.IsDelayRequired
 * @tc.type: FUNC
 * @tc.require: issueI6R49K
 */
HWTEST_F(RSHardwareThreadTest, IsDelayRequired001, TestSize.Level1)
{
    auto &hardwareThread = RSHardwareThread::Instance();
    auto &hgmCore = HgmCore::Instance();
    RefreshRateParam param = {
        .rate = 0,
        .frameTimestamp = 0,
        .actualTimestamp = 0,
        .vsyncId = 0,
        .constraintRelativeTime = 0,
        .isForceRefresh = true,
        .fastComposeTimeStampDiff = 0
    };
    OutputPtr output = HdiOutput::CreateHdiOutput(0);
    bool hasGameScene = true;
    bool isDelayRequired = hardwareThread.IsDelayRequired(hgmCore, param, output, hasGameScene);
    EXPECT_EQ(isDelayRequired == false, true);

    param.isForceRefresh = false;
    bool getLtpoEnabled = hgmCore.GetLtpoEnabled();
    if (getLtpoEnabled) {
        isDelayRequired = hardwareThread.IsDelayRequired(hgmCore, param, output, hasGameScene);
        if (hardwareThread.AdaptiveModeStatus(output) == SupportASStatus::SUPPORT_AS) {
            EXPECT_EQ(isDelayRequired == false, true);
        } else {
            EXPECT_EQ(isDelayRequired == true, true);
        }
    } else {
        isDelayRequired = hardwareThread.IsDelayRequired(hgmCore, param, output, hasGameScene);
        EXPECT_EQ(isDelayRequired == false, true);
        if (hgmCore.IsDelayMode()) {
            hasGameScene = false;
            isDelayRequired = hardwareThread.IsDelayRequired(hgmCore, param, output, hasGameScene);
            EXPECT_EQ(isDelayRequired == true, true);
        }
    }
}

/**
 * @tc.name: CalculateDelayTime001
 * @tc.desc: Test RSHardwareThreadTest.CalculateDelayTime
 * @tc.type: FUNC
 * @tc.require: issueI6R49K
 */
HWTEST_F(RSHardwareThreadTest, CalculateDelayTime001, TestSize.Level1)
{
    auto &hardwareThread = RSHardwareThread::Instance();
    auto &hgmCore = HgmCore::Instance();
    RefreshRateParam param = {
        .rate = 0,
        .frameTimestamp = 0,
        .actualTimestamp = 0,
        .vsyncId = 0,
        .constraintRelativeTime = 0,
        .isForceRefresh = true,
        .fastComposeTimeStampDiff = 0
    };
    uint32_t currentRate = 120;
    int64_t currTime = 1000000000;

    hardwareThread.CalculateDelayTime(hgmCore, param, currentRate, currTime);
    EXPECT_EQ(hardwareThread.delayTime_ == 0, true);
}

/**
 * @tc.name: RecordTimestamp
 * @tc.desc: Test RSHardwareThreadTest.RecordTimestamp
 * @tc.type: FUNC
 * @tc.require: IBE7GI
 */
HWTEST_F(RSHardwareThreadTest, RecordTimestamp, TestSize.Level1)
{
    auto& hardwareThread = RSHardwareThread::Instance();
    auto surfaceNode1 = RSTestUtil::CreateSurfaceNodeWithBuffer();
    auto surfaceNode2 = RSTestUtil::CreateSurfaceNodeWithBuffer();
    auto surfaceNode3 = RSTestUtil::CreateSurfaceNodeWithBuffer();
    RectI dstRect{0, 0, 400, 600};
    surfaceNode1->SetSrcRect(dstRect);
    surfaceNode1->SetDstRect(dstRect);
    surfaceNode2->SetSrcRect(dstRect);
    surfaceNode2->SetDstRect(dstRect);
    surfaceNode3->SetSrcRect(dstRect);
    surfaceNode3->SetDstRect(dstRect);
    auto layer1 = composerAdapter_->CreateLayer(*surfaceNode1);
    ASSERT_NE(layer1, nullptr);
    auto layer2 = composerAdapter_->CreateLayer(*surfaceNode2);
    ASSERT_NE(layer2, nullptr);
    auto layer3 = composerAdapter_->CreateLayer(*surfaceNode3);
    ASSERT_NE(layer3, nullptr);

    std::vector<LayerInfoPtr> layers;
    layers.emplace_back(layer1);
    layers.emplace_back(layer2);
    layers.emplace_back(layer3);

    auto& surfaceFpsManager = RSSurfaceFpsManager::GetInstance();
    surfaceFpsManager.RegisterSurfaceFps(layer1->GetNodeId(), layer1->GetSurface()->GetName());
    hardwareThread.RecordTimestamp(layers);
    surfaceFpsManager.UnregisterSurfaceFps(layer1->GetNodeId());
}

/**
 * @tc.name: ExecuteSwitchRefreshRate
 * @tc.desc: Test RSHardwareThreadTest.ExecuteSwitchRefreshRate
 * @tc.type: FUNC
 * @tc.require: issueIBH6WN
 */
HWTEST_F(RSHardwareThreadTest, ExecuteSwitchRefreshRate, TestSize.Level1)
{
    auto &hardwareThread = RSHardwareThread::Instance();
    OutputPtr output = HdiOutput::CreateHdiOutput(screenId_);
    ASSERT_NE(output, nullptr);
    auto &hgmCore = HgmCore::Instance();
    auto frameRateMgr = hgmCore.GetFrameRateMgr();
    ASSERT_NE(frameRateMgr, nullptr);
    hgmCore.hgmFrameRateMgr_ = nullptr;
    hardwareThread.ExecuteSwitchRefreshRate(output, 0);

    hgmCore.hgmFrameRateMgr_ = frameRateMgr;
    hardwareThread.ExecuteSwitchRefreshRate(output, 0);

    //  设置屏幕尺寸为1080p，物理屏尺寸包含1080p即可
    ScreenSize sSize = {720, 1080, 685, 1218};
    hgmCore.AddScreen(screenId_, 0, sSize);
    auto screen = hgmCore.GetScreen(screenId_);
    screen->SetSelfOwnedScreenFlag(true);
    hgmCore.SetScreenRefreshRateImme(1);
    hardwareThread.ExecuteSwitchRefreshRate(output, 0);

    int32_t status = hgmCore.SetScreenRefreshRate(0, screenId_, 0);
    ASSERT_TRUE(status < EXEC_SUCCESS);
}

/**
 * @tc.name: PerformSetActiveMode
 * @tc.desc: Test RSHardwareThreadTest.PerformSetActiveMode
 * @tc.type: FUNC
 * @tc.require: issueIBH6WN
 */
HWTEST_F(RSHardwareThreadTest, PerformSetActiveMode, TestSize.Level1)
{
    auto &hardwareThread = RSHardwareThread::Instance();
    OutputPtr output = HdiOutput::CreateHdiOutput(screenId_);
    ASSERT_NE(output, nullptr);

    auto screenManager = CreateOrGetScreenManager();
    ASSERT_NE(screenManager, nullptr);
    OHOS::Rosen::impl::RSScreenManager::instance_ = nullptr;
    hardwareThread.PerformSetActiveMode(output, 0, 0);

    OHOS::Rosen::impl::RSScreenManager::instance_ = screenManager;
    hardwareThread.hgmRefreshRates_ = HgmRefreshRates::SET_RATE_120;
    hardwareThread.PerformSetActiveMode(output, 0, 0);

    auto &hgmCore = HgmCore::Instance();
    hgmCore.modeListToApply_ = std::make_unique<std::unordered_map<ScreenId, int32_t>>();
    int32_t rate = 3;
    hgmCore.modeListToApply_->insert({screenId_, rate});
    hardwareThread.PerformSetActiveMode(output, 0, 0);

    uint64_t timestamp = 0;
    auto supportedModes = screenManager->GetScreenSupportedModes(screenId_);
    ASSERT_EQ(supportedModes.size(), 0);
    HgmCore::Instance().hgmFrameRateMgr_->isAdaptive_ = true;
    HgmCore::Instance().hgmFrameRateMgr_->isGameNodeOnTree_ = true;
    hardwareThread.PerformSetActiveMode(output, 0, 0);
    HgmCore::Instance().hgmFrameRateMgr_ = nullptr;
    hardwareThread.PerformSetActiveMode(output, 0, 0);
    HgmCore::Instance().vBlankIdleCorrectSwitch_.store(true);
    hardwareThread.vblankIdleCorrector_.isVBlankIdle_ = true;
    hardwareThread.OnScreenVBlankIdleCallback(screenId_, timestamp);
    hardwareThread.PerformSetActiveMode(output, 0, 0);
}

/**
 * @tc.name: OnPrepareComplete
 * @tc.desc: Test RSHardwareThreadTest.OnPrepareComplete
 * @tc.type: FUNC
 * @tc.require: issueIBH6WN
 */
HWTEST_F(RSHardwareThreadTest, OnPrepareComplete, TestSize.Level1)
{
    auto &hardwareThread = RSHardwareThread::Instance();
    PrepareCompleteParam para;

    auto csurface = IConsumerSurface::Create();
    ASSERT_NE(csurface, nullptr);
    auto producer = csurface->GetProducer();
    ASSERT_NE(producer, nullptr);
    auto psurface = Surface::CreateSurfaceAsProducer(producer);
    ASSERT_NE(psurface, nullptr);
    hardwareThread.OnPrepareComplete(psurface, para, nullptr);

    para.needFlushFramebuffer = true;
    hardwareThread.OnPrepareComplete(psurface, para, nullptr);

    ASSERT_TRUE(hardwareThread.redrawCb_ != nullptr);
}

/**
 * @tc.name: CreateFrameBufferSurfaceOhos
 * @tc.desc: Test RSHardwareThreadTest.CreateFrameBufferSurfaceOhos
 * @tc.type: FUNC
 * @tc.require: issueIBH6WN
 */
HWTEST_F(RSHardwareThreadTest, CreateFrameBufferSurfaceOhos, TestSize.Level1)
{
    auto &hardwareThread = RSHardwareThread::Instance();
    auto csurface = IConsumerSurface::Create();
    ASSERT_NE(csurface, nullptr);
    auto producer = csurface->GetProducer();
    ASSERT_NE(producer, nullptr);
    auto psurface = Surface::CreateSurfaceAsProducer(producer);
    ASSERT_NE(psurface, nullptr);

    auto rsSurfaceOhosPtr = hardwareThread.CreateFrameBufferSurfaceOhos(psurface);
    ASSERT_NE(rsSurfaceOhosPtr, nullptr);
}
/*
 * Function: PreAllocateProtectedBuffer
 * Type: Function
 * Rank: Important(1)
 * EnvConditions: N/A
 * CaseDescription: 1. preSetup: create bufferqueue(1 buffer)
 *                  2. operation: RequestBuffer and PreallocateProtectedBuffer
 *                  3. result: return GSERROR_OK
 */
HWTEST_F(RSHardwareThreadTest, PreAllocateProtectedBuffer001, TestSize.Level1)
{
    auto &hardwareThread = RSHardwareThread::Instance();
    auto rsSurfaceRenderNode = RSTestUtil::CreateSurfaceNode();
    const auto& surfaceConsumer = rsSurfaceRenderNode->GetRSSurfaceHandler()->GetConsumer();
    auto producer = surfaceConsumer->GetProducer();
    sptr<Surface> sProducer = Surface::CreateSurfaceAsProducer(producer);
    sProducer->SetQueueSize(1);
    sptr<SurfaceBuffer> buffer;
    sptr<SyncFence> requestFence = SyncFence::INVALID_FENCE;
    GSError ret = sProducer->RequestBuffer(buffer, requestFence, requestConfig);
    EXPECT_EQ(ret, GSERROR_OK);
    hardwareThread.PreAllocateProtectedBuffer(buffer, screenId_);
}

/**
 * @tc.name: DumpEventQueue
 * @tc.desc: Test RSHardwareThreadTest.DumpEventQueue
 * @tc.type: FUNC
 * @tc.require: issueIBIDU9
 */
HWTEST_F(RSHardwareThreadTest, DumpEventQueue001, TestSize.Level1)
{
    auto& hardwareThread = RSHardwareThread::Instance();
    hardwareThread.PostTask([&]() {});
    hardwareThread.Start();
    ASSERT_NE(hardwareThread.hdiBackend_, nullptr);
    hardwareThread.PostTask([&]() {});
    hardwareThread.ScheduleTask([=]() {}).wait();
    hardwareThread.DumpEventQueue();
}

#ifdef RS_ENABLE_VK
/*
 * Function: ComputeTargetColorGamut
 * Type: Function
 * Rank: Important(1)
 * EnvConditions: N/A
 * CaseDescription: 1. preSetup: create bufferqueue(2 buffer)
 *                  2. operation: change BufferRequestConfig, RequestBuffer and ComputeTargetColorGamut
 *                  3. result: return colorGamut which is GRAPHIC_COLOR_GAMUT_DISPLAY_P3
 */
HWTEST_F(RSHardwareThreadTest, ComputeTargetColorGamut001, TestSize.Level1)
{
    auto &hardwareThread = RSHardwareThread::Instance();
    auto rsSurfaceRenderNode = RSTestUtil::CreateSurfaceNode();
    const auto& surfaceConsumer = rsSurfaceRenderNode->GetRSSurfaceHandler()->GetConsumer();
    auto producer = surfaceConsumer->GetProducer();
    sptr<Surface> sProducer = Surface::CreateSurfaceAsProducer(producer);
    sProducer->SetQueueSize(2);
    sptr<SurfaceBuffer> buffer;
    sptr<SyncFence> requestFence = SyncFence::INVALID_FENCE;
    GSError ret = sProducer->RequestBuffer(buffer, requestFence, requestConfig);
    EXPECT_EQ(ret, GSERROR_OK);
    GraphicColorGamut colorGamut = hardwareThread.ComputeTargetColorGamut(buffer);
    EXPECT_EQ(colorGamut, GRAPHIC_COLOR_GAMUT_DISPLAY_P3);

    requestConfig.colorGamut = GRAPHIC_COLOR_GAMUT_DISPLAY_P3;
    ret = sProducer->RequestBuffer(buffer, requestFence, requestConfig);
    EXPECT_EQ(ret, GSERROR_OK);
    colorGamut = hardwareThread.ComputeTargetColorGamut(buffer);
    EXPECT_EQ(colorGamut, GRAPHIC_COLOR_GAMUT_DISPLAY_P3);
}

/*
 * Function: ComputeTargetColorGamut
 * Type: Function
 * Rank: Important(1)
 * EnvConditions: N/A
 * CaseDescription: 1. preSetup: create bufferqueue(1 buffer)
 *                  2. operation: RequestBuffer, SetColorSpaceInfo and ComputeTargetColorGamut
 *                  3. result: return colorGamut which is GRAPHIC_COLOR_GAMUT_SRGB
 */
HWTEST_F(RSHardwareThreadTest, ComputeTargetColorGamut002, TestSize.Level1)
{
    auto rsSurfaceRenderNode = RSTestUtil::CreateSurfaceNode();
    const auto& surfaceConsumer = rsSurfaceRenderNode->GetRSSurfaceHandler()->GetConsumer();
    auto producer = surfaceConsumer->GetProducer();
    sptr<Surface> sProducer = Surface::CreateSurfaceAsProducer(producer);
    sProducer->SetQueueSize(1);
    sptr<SurfaceBuffer> buffer;
    sptr<SyncFence> requestFence = SyncFence::INVALID_FENCE;
    GSError ret = sProducer->RequestBuffer(buffer, requestFence, requestConfig);
    EXPECT_EQ(ret, GSERROR_OK);
    using namespace HDI::Display::Graphic::Common::V1_0;
    auto &hardwareThread = RSHardwareThread::Instance();
    CM_ColorSpaceInfo infoSet = {
        .primaries = COLORPRIMARIES_SRGB,
    };
    auto retSet = MetadataHelper::SetColorSpaceInfo(buffer, infoSet);
    EXPECT_EQ(retSet, GSERROR_OK);
    GraphicColorGamut colorGamut = hardwareThread.ComputeTargetColorGamut(buffer);
    EXPECT_EQ(colorGamut, GRAPHIC_COLOR_GAMUT_SRGB);
}

/*
 * Function: ComputeTargetPixelFormat
 * Type: Function
 * Rank: Important(1)
 * EnvConditions: N/A
 * CaseDescription: 1. preSetup: create bufferqueue(3 buffer)
 *                  2. operation: change BufferRequestConfig, RequestBuffer and ComputeTargetPixelFormat
 *                  3. result: return pixelFormat which is GRAPHIC_PIXEL_FMT_RGBA_1010102
 */
HWTEST_F(RSHardwareThreadTest, ComputeTargetPixelFormat001, TestSize.Level1)
{
    auto &hardwareThread = RSHardwareThread::Instance();
    auto rsSurfaceRenderNode = RSTestUtil::CreateSurfaceNode();
    const auto& surfaceConsumer = rsSurfaceRenderNode->GetRSSurfaceHandler()->GetConsumer();
    auto producer = surfaceConsumer->GetProducer();
    sptr<Surface> sProducer = Surface::CreateSurfaceAsProducer(producer);
    sProducer->SetQueueSize(3);
    sptr<SurfaceBuffer> buffer;
    sptr<SyncFence> requestFence = SyncFence::INVALID_FENCE;
    requestConfig.format = GRAPHIC_PIXEL_FMT_RGBA_1010102;
    GSError ret = sProducer->RequestBuffer(buffer, requestFence, requestConfig);
    EXPECT_EQ(ret, GSERROR_OK);
    GraphicPixelFormat pixelFormat = hardwareThread.ComputeTargetPixelFormat(buffer);
    EXPECT_EQ(pixelFormat, GRAPHIC_PIXEL_FMT_RGBA_1010102);

    requestConfig.format = GRAPHIC_PIXEL_FMT_YCBCR_P010;
    ret = sProducer->RequestBuffer(buffer, requestFence, requestConfig);
    EXPECT_EQ(ret, GSERROR_OK);
    pixelFormat = hardwareThread.ComputeTargetPixelFormat(buffer);
    EXPECT_EQ(pixelFormat, GRAPHIC_PIXEL_FMT_RGBA_1010102);

    requestConfig.format = GRAPHIC_PIXEL_FMT_YCRCB_P010;
    ret = sProducer->RequestBuffer(buffer, requestFence, requestConfig);
    EXPECT_EQ(ret, GSERROR_OK);
    pixelFormat = hardwareThread.ComputeTargetPixelFormat(buffer);
    EXPECT_EQ(pixelFormat, GRAPHIC_PIXEL_FMT_RGBA_1010102);
}
#endif

/*
 * @tc.name: ChangeLayersForActiveRectOutside001
 * @tc.desc: Test RSHardwareThreadTest.ChangeLayersForActiveRectOutside
 * @tc.type: FUNC
 * @tc.require: issuesIBLTM5
 */
HWTEST_F(RSHardwareThreadTest, ChangeLayersForActiveRectOutside001, TestSize.Level1)
{
    auto &hardwareThread = RSHardwareThread::Instance();
    std::vector<LayerInfoPtr> layers;
    LayerInfoPtr layer = HdiLayerInfo::CreateHdiLayerInfo();
    layers.emplace_back(layer);
    hardwareThread.ChangeLayersForActiveRectOutside(layers, screenId_);
    EXPECT_NE(layers.size(), 0);
}

/*
 * @tc.name: ClearRedrawGPUCompositionCache001
 * @tc.desc: Test RSHardwareThreadTest.ClearRedrawGPUCompositionCache
 * @tc.type: FUNC
 * @tc.require: issuesIBYE2H
 */
HWTEST_F(RSHardwareThreadTest, ClearRedrawGPUCompositionCache001, TestSize.Level1)
{
    auto &hardwareThread = RSHardwareThread::Instance();
    hardwareThread.Start();
    ASSERT_NE(hardwareThread.hdiBackend_, nullptr);
    std::set<uint32_t> bufferIds = {1};
    hardwareThread.ClearRedrawGPUCompositionCache(bufferIds);
}

/*
 * @tc.name: RefreshRateCounts001
 * @tc.desc: Test RSHardwareThreadTest.RefreshRateCounts
 * @tc.type: FUNC
 * @tc.require: issuesIBYE2H
 */
HWTEST_F(RSHardwareThreadTest, RefreshRateCounts001, TestSize.Level1)
{
    OutputPtr output = HdiOutput::CreateHdiOutput(screenId_);
    auto &hardwareThread = RSHardwareThread::Instance();
    auto count = hardwareThread.refreshRateCounts_;
    hardwareThread.AddRefreshRateCount(output);
    ASSERT_TRUE(count != hardwareThread.refreshRateCounts_);

    std::string dumpString = "";
    hardwareThread.RefreshRateCounts(dumpString);
    ASSERT_NE(dumpString, "");
}

/*
 * @tc.name: ClearRefreshRateCounts001
 * @tc.desc: Test RSHardwareThreadTest.ClearRefreshRateCounts
 * @tc.type: FUNC
 * @tc.require: issuesIBYE2H
 */
HWTEST_F(RSHardwareThreadTest, ClearRefreshRateCounts001, TestSize.Level1)
{
    OutputPtr output = HdiOutput::CreateHdiOutput(screenId_);
    auto &hardwareThread = RSHardwareThread::Instance();
    auto count = hardwareThread.refreshRateCounts_;
    hardwareThread.AddRefreshRateCount(output);
    ASSERT_TRUE(count != hardwareThread.refreshRateCounts_);

    std::string dumpString = "";
    hardwareThread.ClearRefreshRateCounts(dumpString);
    ASSERT_EQ(hardwareThread.refreshRateCounts_.empty(), true);
}

/*
 * @tc.name: OnScreenVBlankIdleCallback001
 * @tc.desc: Test RSHardwareThreadTest.OnScreenVBlankIdleCallback
 * @tc.type: FUNC
 * @tc.require: issuesIBYE2H
 */
HWTEST_F(RSHardwareThreadTest, OnScreenVBlankIdleCallback001, TestSize.Level1)
{
    auto &hardwareThread = RSHardwareThread::Instance();
    hardwareThread.Start();
    ASSERT_NE(hardwareThread.hdiBackend_, nullptr);
    uint64_t timestamp = 10;
    hardwareThread.OnScreenVBlankIdleCallback(screenId_, timestamp);
}

/*
 * @tc.name: ChangeDssRefreshRate001
 * @tc.desc: Test RSHardwareThreadTest.ChangeDssRefreshRate
 * @tc.type: FUNC
 * @tc.require: issuesIBYE2H
 */
HWTEST_F(RSHardwareThreadTest, ChangeDssRefreshRate001, TestSize.Level1)
{
    auto &hardwareThread = RSHardwareThread::Instance();
    hardwareThread.Start();
    ASSERT_NE(hardwareThread.hdiBackend_, nullptr);
    bool followPipline = true;
    uint32_t refreshRate = 100;
    hardwareThread.ChangeDssRefreshRate(screenId_, refreshRate, followPipline);

    followPipline = false;
    hardwareThread.ChangeDssRefreshRate(screenId_, refreshRate, followPipline);
}

/*
 * @tc.name: RedrawScreenRCD001
 * @tc.desc: Test RSHardwareThreadTest.RedrawScreenRCD
 * @tc.type: FUNC
 * @tc.require: issuesIBYE2H
 */
HWTEST_F(RSHardwareThreadTest, RedrawScreenRCD001, TestSize.Level1)
{
    auto &hardwareThread = RSHardwareThread::Instance();
    hardwareThread.Start();
    Drawing::Canvas canvas;
    RSPaintFilterCanvas rsPaintFilterCanvas(&canvas);
    std::vector<LayerInfoPtr> layers;
    LayerInfoPtr layer = HdiLayerInfo::CreateHdiLayerInfo();
    layers.emplace_back(layer);
    EXPECT_NE(layers.size(), 0);
    hardwareThread.RedrawScreenRCD(rsPaintFilterCanvas, layers);
}

/*
 * @tc.name: Redraw001
 * @tc.desc: Test RSHardwareThreadTest.Redraw
 * @tc.type: FUNC
 * @tc.require: issuesIBYE2H
 */
HWTEST_F(RSHardwareThreadTest, Redraw001, TestSize.Level1)
{
    auto &hardwareThread = RSHardwareThread::Instance();
    auto csurface = IConsumerSurface::Create();
    ASSERT_NE(csurface, nullptr);
    auto producer = csurface->GetProducer();
    auto psurface = Surface::CreateSurfaceAsProducer(producer);
    ASSERT_NE(psurface, nullptr);

    std::vector<LayerInfoPtr> layers;
    LayerInfoPtr layer = HdiLayerInfo::CreateHdiLayerInfo();
    layers.emplace_back(layer);
    EXPECT_NE(layers.size(), 0);

    hardwareThread.Redraw(psurface, layers, screenId_);
}

/*
 * @tc.name: EndCheck001
 * @tc.desc: Test RSHardwareThreadTest.EndCheck
 * @tc.type: FUNC
 * @tc.require: issuesIC3DAI
 */
HWTEST_F(RSHardwareThreadTest, EndCheck001, TestSize.Level1)
{
    auto &hardwareThread = RSHardwareThread::Instance();
    hardwareThread.hardwareCount_ = 0;
    RSTimer timer("EndCheckTest", 800); // 800ms
    hardwareThread.EndCheck(timer);
    ASSERT_EQ(hardwareThread.hardwareCount_, 0); // timeout count 0
    usleep(800 * 1000); // 800ms
    hardwareThread.EndCheck(timer);
    ASSERT_EQ(hardwareThread.hardwareCount_, 1); // timeout count 1
}
} // namespace OHOS::Rosen