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
#include "pipeline/rs_hardware_thread.h"
#include "pipeline/rs_uni_render_composer_adapter.h"
#include "rs_test_util.h"

using namespace testing;
using namespace testing::ext;

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
    sptr<RSScreenManager> screenManager_;

    int32_t offsetX = 0; // screenOffset on x axis equals to 0
    int32_t offsetY = 0; // screenOffset on y axis equals to 0
    float mirrorAdaptiveCoefficient = 1.0f;

    const uint32_t screenId_ = 0;

    static inline BufferRequestConfig requestConfig = {
        .width = 0x100,
        .height = 0x100,
        .strideAlignment = 0x8,
        .format = GRAPHIC_PIXEL_FMT_RGBA_8888,
        .usage = BUFFER_USAGE_CPU_READ | BUFFER_USAGE_CPU_WRITE | BUFFER_USAGE_MEM_DMA,
        .timeout = 0,
    };

    static inline BufferFlushConfig flushConfig = {
        .damage = { .w = 0x100, .h = 0x100, },
    };
};

void RSHardwareThreadTest::SetUpTestCase() {}
void RSHardwareThreadTest::TearDownTestCase() {}
void RSHardwareThreadTest::TearDown() {}
void RSHardwareThreadTest::SetUp()
{
    screenManager_ = CreateOrGetScreenManager();
    screenManager_->Init();
    CreateComposerAdapterWithScreenInfo();
}

void RSHardwareThreadTest::CreateComposerAdapterWithScreenInfo(uint32_t width, uint32_t height,
    ScreenColorGamut colorGamut, ScreenState state, ScreenRotation rotation)
{
    std::unique_ptr<impl::RSScreen> rsScreen =
        std::make_unique<impl::RSScreen>(screenId_, false, HdiOutput::CreateHdiOutput(screenId_), nullptr);
    screenManager_->MockHdiScreenConnected(rsScreen);
    auto info = screenManager_->QueryScreenInfo(screenId_);
    info.width = width;
    info.height = height;
    info.colorGamut = colorGamut;
    info.state = state;
    info.rotation = rotation;
    composerAdapter_ = std::make_unique<RSComposerAdapter>();
    composerAdapter_->Init(info, offsetX, offsetY, mirrorAdaptiveCoefficient, nullptr);
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
    hardwareThread.PostTask([&]() {});
    hardwareThread.ScheduleTask([=]() {}).wait();
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
    hardwareThread.CommitAndReleaseLayers(composerAdapter_->output_, layers);
}

/**
 * @tc.name: Start004
 * @tc.desc: Test RSHardwareThreadTest.ReleaseBuffer
 * @tc.type: FUNC
 * @tc.require: issueI6R49K
 */
HWTEST_F(RSHardwareThreadTest, Start004, TestSize.Level1)
{
    auto& hardwareThread = RSHardwareThread::Instance();
    auto rsSurfaceRenderNode = RSTestUtil::CreateSurfaceNode();
    const auto& surfaceConsumer = rsSurfaceRenderNode->GetConsumer();
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
    sptr<SyncFence> releaseFence = SyncFence::INVALID_FENCE;
    hardwareThread.ReleaseBuffer(buffer, releaseFence, surfaceConsumer);
}
} // namespace OHOS::Rosen