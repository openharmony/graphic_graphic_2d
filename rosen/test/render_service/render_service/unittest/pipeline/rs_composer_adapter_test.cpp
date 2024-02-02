/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
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
#include "limit_number.h"
#include "pipeline/rs_composer_adapter.h"
#include "screen_manager/rs_screen_manager.h"
#include "rs_test_util.h"
#include "transaction/rs_interfaces.h"
#include "pipeline/rs_hardware_thread.h"
#include "mock/mock_hdi_device.h"
#include "surface_buffer_impl.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Rosen {
class RSComposerAdapterTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
    void CreateComposerAdapterWithScreenInfo(uint32_t width = 2560, uint32_t height = 1080,
        ScreenColorGamut colorGamut = ScreenColorGamut::COLOR_GAMUT_SRGB,
        ScreenState state = ScreenState::UNKNOWN,
        ScreenRotation rotation = ScreenRotation::ROTATION_0);
    static inline Mock::HdiDeviceMock* hdiDeviceMock_;
    static inline std::unique_ptr<RSComposerAdapter> composerAdapter_;
    static inline sptr<RSScreenManager> screenManager_;
    static inline std::shared_ptr<HdiOutput> hdiOutput_;
    static inline std::unique_ptr<impl::RSScreen> rsScreen_;
    int32_t offsetX = 0; // screenOffset on x axis equals to 0
    int32_t offsetY = 0; // screenOffset on y axis equals to 0
    float mirrorAdaptiveCoefficient = 1.0f;
    static uint32_t screenId_;
};

uint32_t RSComposerAdapterTest::screenId_ = 0;

void RSComposerAdapterTest::SetUpTestCase()
{
    hdiOutput_ = HdiOutput::CreateHdiOutput(screenId_);
    rsScreen_ = std::make_unique<impl::RSScreen>(screenId_, true, hdiOutput_, nullptr);
    screenManager_ = CreateOrGetScreenManager();
    screenManager_->MockHdiScreenConnected(rsScreen_);
    hdiDeviceMock_ = Mock::HdiDeviceMock::GetInstance();
    std::vector<uint32_t> layersId = {0};
    EXPECT_CALL(*hdiDeviceMock_, GetScreenReleaseFence(_, layersId, _)).WillRepeatedly(testing::Return(0));
    EXPECT_CALL(*hdiDeviceMock_, RegHotPlugCallback(_, _)).WillRepeatedly(testing::Return(0));
    EXPECT_CALL(*hdiDeviceMock_, RegHwcDeadCallback(_, _)).WillRepeatedly(testing::Return(false));
}

void RSComposerAdapterTest::TearDownTestCase()
{
    hdiOutput_ = nullptr;
    rsScreen_ = nullptr;
    composerAdapter_ = nullptr;
    screenManager_ = nullptr;
    hdiDeviceMock_ = nullptr;
}

void RSComposerAdapterTest::SetUp() {}
void RSComposerAdapterTest::TearDown() {}

void RSComposerAdapterTest::CreateComposerAdapterWithScreenInfo(uint32_t width, uint32_t height,
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
}

/**
 * @tc.name: CommitLayersTest001
 * @tc.desc: commitLayer when csurface is nullptr
 * @tc.type: FUNC
 * @tc.require: issueI60QXK
 */
HWTEST_F(RSComposerAdapterTest, CommitLayersTest001, Function | SmallTest | Level2)
{
    CreateComposerAdapterWithScreenInfo(2160, 1080, ScreenColorGamut::COLOR_GAMUT_SRGB, ScreenState::UNKNOWN,
        ScreenRotation::ROTATION_0);
    composerAdapter_->SetHdiBackendDevice(hdiDeviceMock_);
    std::vector<std::shared_ptr<HdiLayerInfo>> layers;
    auto surfaceNode1 = RSTestUtil::CreateSurfaceNodeWithBuffer();
    auto surfaceNode2 = RSTestUtil::CreateSurfaceNode();
    ASSERT_NE(surfaceNode1, nullptr);
    ASSERT_NE(surfaceNode2, nullptr);
    auto infoPtr1 = composerAdapter_->CreateLayer(*surfaceNode1);
    auto infoPtr2 = composerAdapter_->CreateLayer(*surfaceNode2);
    layers.emplace_back(infoPtr1);
    layers.emplace_back(infoPtr2);
    composerAdapter_->CommitLayers(layers);
    composerAdapter_ = nullptr;
}

/**
 * @tc.name: CommitLayersTest002
 * @tc.desc: commitTunnelLayer when csurface is nullptr
 * @tc.type: FUNC
 * @tc.require: issueI60QXK
 */
HWTEST_F(RSComposerAdapterTest, CommitLayersTest002, Function | SmallTest | Level2)
{
    CreateComposerAdapterWithScreenInfo(2160, 1080, ScreenColorGamut::COLOR_GAMUT_SRGB, ScreenState::UNKNOWN,
        ScreenRotation::ROTATION_0);
    composerAdapter_->SetHdiBackendDevice(hdiDeviceMock_);
    std::vector<std::shared_ptr<HdiLayerInfo>> layers;
    auto surfaceNode1 = RSTestUtil::CreateSurfaceNode();
    auto& consumer = surfaceNode1->GetConsumer();
    GraphicExtDataHandle handle;
    handle.fd = -1;
    handle.reserveInts = 1;
    consumer->SetTunnelHandle(&handle);
    auto infoPtr = composerAdapter_->CreateLayer(*surfaceNode1);
    layers.emplace_back(infoPtr);
    composerAdapter_->CommitLayers(layers);
    composerAdapter_ = nullptr;
}

/**
 * @tc.name: CommitLayersTest003
 * @tc.desc: commitLayer when csurface is nullptr
 * @tc.type: FUNC
 * @tc.require: issueI794H6
 */
HWTEST_F(RSComposerAdapterTest, CommitLayersTest003, Function | SmallTest | Level2)
{
    CreateComposerAdapterWithScreenInfo(2160, 1080, ScreenColorGamut::COLOR_GAMUT_SRGB, ScreenState::UNKNOWN,
        ScreenRotation::ROTATION_0);
    composerAdapter_->SetHdiBackendDevice(hdiDeviceMock_);
    std::vector<std::shared_ptr<HdiLayerInfo>> layers;
    auto surfaceNode1 = RSTestUtil::CreateSurfaceNodeWithBuffer();
    auto surfaceNode2 = RSTestUtil::CreateSurfaceNode();
    ASSERT_NE(surfaceNode1, nullptr);
    ASSERT_NE(surfaceNode2, nullptr);
    auto infoPtr1 = composerAdapter_->CreateLayer(*surfaceNode1);
    auto infoPtr2 = composerAdapter_->CreateLayer(*surfaceNode2);
    layers.emplace_back(infoPtr1);
    layers.emplace_back(infoPtr2);
    RSHardwareThread::Instance().Start();
    composerAdapter_->CommitLayers(layers);
    composerAdapter_ = nullptr;
}

/**
 * @tc.name: CreateLayersTest001
 * @tc.desc: CreateLayers when surfaceNode has valid buffer
 * @tc.type: FUNC
 * @tc.require: issueI60QXK
 */
HWTEST_F(RSComposerAdapterTest, CreateLayersTest001, Function | SmallTest | Level2)
{
    auto surfaceNode1 = RSTestUtil::CreateSurfaceNodeWithBuffer();
    auto surfaceNode2 = RSTestUtil::CreateSurfaceNodeWithBuffer();
    auto surfaceNode3 = RSTestUtil::CreateSurfaceNodeWithBuffer();
    RectI dstRect{0, 0, 400, 600};
    surfaceNode1->SetSrcRect(dstRect);
    surfaceNode1->SetDstRect(dstRect);
    surfaceNode3->SetSrcRect(dstRect);
    surfaceNode3->SetDstRect(dstRect);
    auto& property = surfaceNode3->GetMutableRenderProperties();
    EXPECT_NE(&property, nullptr);
    property.SetBounds({ 0, 0, 400, 600 });
    CreateComposerAdapterWithScreenInfo(2160, 1080, ScreenColorGamut::COLOR_GAMUT_SRGB, ScreenState::UNKNOWN,
        ScreenRotation::ROTATION_0);
    composerAdapter_->SetHdiBackendDevice(hdiDeviceMock_);
    auto infoPtr1 = composerAdapter_->CreateLayer(*surfaceNode1);
    auto infoPtr2 = composerAdapter_->CreateLayer(*surfaceNode2);
    auto infoPtr3 = composerAdapter_->CreateLayer(*surfaceNode3);
    composerAdapter_ = nullptr;
}

/**
 * @tc.name: CreateLayersTest002
 * @tc.desc: CreateLayers when surfaceNode is out of screen region
 * @tc.type: FUNC
 * @tc.require: issueI60QXK
 */
HWTEST_F(RSComposerAdapterTest, CreateLayersTest002, Function | SmallTest | Level2)
{
    std::vector<std::shared_ptr<HdiLayerInfo>> layers;
    auto surfaceNode1 = RSTestUtil::CreateSurfaceNodeWithBuffer();
    auto surfaceNode2 = RSTestUtil::CreateSurfaceNode();
    RectI dstRect1{500, 600, 5000, 6000};
    RectI dstRect2{0, 0, 500, 600};
    surfaceNode1->SetDstRect(dstRect1);
    surfaceNode2->SetDstRect(dstRect2);
    surfaceNode1->SetSrcRect(dstRect1);
    surfaceNode2->SetSrcRect(dstRect2);
    auto& property1 = surfaceNode1->GetMutableRenderProperties();
    EXPECT_NE(&property1, nullptr);
    property1.SetBounds({ 0, 0, 400, 600 });
    auto& property2 = surfaceNode2->GetMutableRenderProperties();
    EXPECT_NE(&property2, nullptr);
    property2.SetBounds({ 0, 0, 400, 600 });
    CreateComposerAdapterWithScreenInfo(2160, 1080, ScreenColorGamut::COLOR_GAMUT_SRGB, ScreenState::UNKNOWN,
        ScreenRotation::ROTATION_0);
    composerAdapter_->SetHdiBackendDevice(hdiDeviceMock_);
    auto infoPtr1 = composerAdapter_->CreateLayer(*surfaceNode1);
    auto infoPtr2 = composerAdapter_->CreateLayer(*surfaceNode2);
    composerAdapter_ = nullptr;
}

/**
 * @tc.name: CreateLayersTest003
 * @tc.desc: CreateLayers with screen rotation
 * @tc.type: FUNC
 * @tc.require: issueI60QXK
 */
HWTEST_F(RSComposerAdapterTest, CreateLayersTest003, Function | SmallTest | Level2)
{
    std::vector<std::shared_ptr<HdiLayerInfo>> layers;
    auto surfaceNode1 = RSTestUtil::CreateSurfaceNodeWithBuffer();
    RectI dstRect{0, 0, 400, 600};
    surfaceNode1->SetSrcRect(dstRect);
    surfaceNode1->SetDstRect(dstRect);
    auto& property = surfaceNode1->GetMutableRenderProperties();
    EXPECT_NE(&property, nullptr);
    property.SetBounds({ 0, 0, 200, 400 });
    CreateComposerAdapterWithScreenInfo(2160, 1080, ScreenColorGamut::COLOR_GAMUT_ADOBE_RGB, ScreenState::UNKNOWN,
        ScreenRotation::ROTATION_90);
    composerAdapter_->SetHdiBackendDevice(hdiDeviceMock_);
    auto infoPtr1 = composerAdapter_->CreateLayer(*surfaceNode1);
    composerAdapter_ = nullptr;
}

/**
 * @tc.name: CreateLayersTest004
 * @tc.desc: CreateLayers with screen rotation
 * @tc.type: FUNC
 * @tc.require: issueI60QXK
 */
HWTEST_F(RSComposerAdapterTest, CreateLayersTest004, Function | SmallTest | Level2)
{
    std::vector<std::shared_ptr<HdiLayerInfo>> layers;
    auto surfaceNode1 = RSTestUtil::CreateSurfaceNodeWithBuffer();
    RectI dstRect{0, 0, 400, 600};
    surfaceNode1->SetSrcRect(dstRect);
    surfaceNode1->SetDstRect(dstRect);
    auto& property = surfaceNode1->GetMutableRenderProperties();
    EXPECT_NE(&property, nullptr);
    property.SetBounds({ 0, 0, 200, 400 });
    CreateComposerAdapterWithScreenInfo(2160, 1080, ScreenColorGamut::COLOR_GAMUT_SRGB, ScreenState::UNKNOWN,
        ScreenRotation::ROTATION_180);
    composerAdapter_->SetHdiBackendDevice(hdiDeviceMock_);
    auto infoPtr1 = composerAdapter_->CreateLayer(*surfaceNode1);
    composerAdapter_ = nullptr;
}

/**
 * @tc.name: CreateLayersTest005
 * @tc.desc: CreateLayers with screen rotation
 * @tc.type: FUNC
 * @tc.require: issueI60QXK
 */
HWTEST_F(RSComposerAdapterTest, CreateLayersTest005, Function | SmallTest | Level2)
{
    std::vector<std::shared_ptr<HdiLayerInfo>> layers;
    auto surfaceNode1 = RSTestUtil::CreateSurfaceNodeWithBuffer();
    RectI dstRect{0, 0, 400, 600};
    surfaceNode1->SetSrcRect(dstRect);
    surfaceNode1->SetDstRect(dstRect);
    auto& property = surfaceNode1->GetMutableRenderProperties();
    EXPECT_NE(&property, nullptr);
    property.SetBounds({ 0, 0, 200, 400 });
    CreateComposerAdapterWithScreenInfo(2160, 1080, ScreenColorGamut::COLOR_GAMUT_SRGB, ScreenState::UNKNOWN,
        ScreenRotation::ROTATION_270);
    composerAdapter_->SetHdiBackendDevice(hdiDeviceMock_);
    auto infoPtr1 = composerAdapter_->CreateLayer(*surfaceNode1);
    composerAdapter_ = nullptr;
}

/**
 * @tc.name: CreateLayersTest006
 * @tc.desc: CreateLayers with screen rotation and scale
 * @tc.type: FUNC
 * @tc.require: issueI60QXK
 */
HWTEST_F(RSComposerAdapterTest, CreateLayersTest006, Function | SmallTest | Level2)
{
    std::vector<std::shared_ptr<HdiLayerInfo>> layers;
    auto surfaceNode1 = RSTestUtil::CreateSurfaceNodeWithBuffer();
    RectI scrRect{0, 0, 400, 60};
    RectI dstRect{0, 0, 40, 600};
    surfaceNode1->SetSrcRect(scrRect);
    surfaceNode1->SetDstRect(dstRect);
    auto& property = surfaceNode1->GetMutableRenderProperties();
    EXPECT_NE(&property, nullptr);
    property.SetBounds({ 0, 0, 200, 400 });
    surfaceNode1->GetConsumer()->SetScalingMode(surfaceNode1->GetBuffer()->GetSeqNum(),
        ScalingMode::SCALING_MODE_SCALE_CROP);
    CreateComposerAdapterWithScreenInfo(2160, 1080, ScreenColorGamut::COLOR_GAMUT_SRGB, ScreenState::UNKNOWN,
        ScreenRotation::ROTATION_180);
    composerAdapter_->SetHdiBackendDevice(hdiDeviceMock_);
    auto infoPtr1 = composerAdapter_->CreateLayer(*surfaceNode1);
    composerAdapter_ = nullptr;
}

/**
 * @tc.name: CreateLayersTest007
 * @tc.desc: CreateLayers when surfaceNode has metadate
 * @tc.type: FUNC
 * @tc.require: issueI60QXK
 */
HWTEST_F(RSComposerAdapterTest, CreateLayersTest007, Function | SmallTest | Level2)
{
    std::vector<std::shared_ptr<HdiLayerInfo>> layers;
    auto surfaceNode1 = RSTestUtil::CreateSurfaceNodeWithBuffer();
    RectI dstRect{0, 0, 400, 600};
    surfaceNode1->SetSrcRect(dstRect);
    surfaceNode1->SetDstRect(dstRect);
    GraphicHDRMetaData metaData;
    metaData.key = GraphicHDRMetadataKey::GRAPHIC_MATAKEY_GREEN_PRIMARY_X;
    metaData.value = 1.0f;
    std::vector<uint8_t> metaDataVec(2, 128); // mock virtual metaData;
    surfaceNode1->GetConsumer()->SetMetaData(surfaceNode1->GetBuffer()->GetSeqNum(), {{metaData}});
    auto& property = surfaceNode1->GetMutableRenderProperties();
    EXPECT_NE(&property, nullptr);
    property.SetBounds({ 0, 0, 200, 400 });
    CreateComposerAdapterWithScreenInfo(2160, 1080, ScreenColorGamut::COLOR_GAMUT_SRGB, ScreenState::UNKNOWN,
        ScreenRotation::ROTATION_180);
    composerAdapter_->SetHdiBackendDevice(hdiDeviceMock_);
    auto infoPtr1 = composerAdapter_->CreateLayer(*surfaceNode1);
    composerAdapter_ = nullptr;
}

/**
 * @tc.name: CreateLayersTest008
 * @tc.desc: CreateLayers when surfaceNode has gravity
 * @tc.type: FUNC
 * @tc.require: issueI60QXK
 */
HWTEST_F(RSComposerAdapterTest, CreateLayersTest008, Function | SmallTest | Level2)
{
    std::vector<std::shared_ptr<HdiLayerInfo>> layers;
    auto surfaceNode1 = RSTestUtil::CreateSurfaceNodeWithBuffer();
    RectI dstRect{0, 0, 400, 600};
    surfaceNode1->SetSrcRect(dstRect);
    surfaceNode1->SetDstRect(dstRect);
    GraphicHDRMetaData metaData;
    metaData.key = GraphicHDRMetadataKey::GRAPHIC_MATAKEY_GREEN_PRIMARY_X;
    metaData.value = 1.0f;
    std::vector<uint8_t> metaDataVec(2, 128); // mock virtual metaData;
    surfaceNode1->GetConsumer()->SetMetaData(surfaceNode1->GetBuffer()->GetSeqNum(), {{metaData}});
    auto& property = surfaceNode1->GetMutableRenderProperties();
    EXPECT_NE(&property, nullptr);
    property.SetBounds({ 0, 0, 200, 400 });
    property.SetFrameGravity(Gravity::CENTER);
    property.SetFrameSize({ 100, 400 });
    CreateComposerAdapterWithScreenInfo(2160, 1080, ScreenColorGamut::COLOR_GAMUT_SRGB, ScreenState::UNKNOWN,
        ScreenRotation::ROTATION_180);
    composerAdapter_->SetHdiBackendDevice(hdiDeviceMock_);
    auto infoPtr1 = composerAdapter_->CreateLayer(*surfaceNode1);
    composerAdapter_ = nullptr;
}

/**
 * @tc.name: CreateLayersTest009
 * @tc.desc: CreateLayers when surfacenode has scaling mode
 * @tc.type: FUNC
 * @tc.require: issueI60QXK
 */
HWTEST_F(RSComposerAdapterTest, CreateLayersTest009, Function | SmallTest | Level2)
{
    std::vector<std::shared_ptr<HdiLayerInfo>> layers;
    auto surfaceNode1 = RSTestUtil::CreateSurfaceNodeWithBuffer();
    RectI scrRect{0, 0, 40, 600};
    RectI dstRect{0, 0, 400, 60};
    surfaceNode1->SetSrcRect(scrRect);
    surfaceNode1->SetDstRect(dstRect);
    auto& property = surfaceNode1->GetMutableRenderProperties();
    EXPECT_NE(&property, nullptr);
    property.SetBounds({ 0, 0, 200, 400 });
    surfaceNode1->GetConsumer()->SetScalingMode(surfaceNode1->GetBuffer()->GetSeqNum(),
        ScalingMode::SCALING_MODE_SCALE_CROP);
    CreateComposerAdapterWithScreenInfo(2160, 1080, ScreenColorGamut::COLOR_GAMUT_SRGB, ScreenState::UNKNOWN,
        ScreenRotation::ROTATION_180);
    composerAdapter_->SetHdiBackendDevice(hdiDeviceMock_);
    auto infoPtr1 = composerAdapter_->CreateLayer(*surfaceNode1);
    composerAdapter_ = nullptr;
}

/**
 * @tc.name: CreateLayersTest010
 * @tc.desc: CreateLayers (DisplayNode)
 * @tc.type: FUNC
 * @tc.require: issueI60QXK
 */
HWTEST_F(RSComposerAdapterTest, CreateLayersTest010, Function | SmallTest | Level2)
{
    uint32_t width = 2160;
    uint32_t height = 1080;
    CreateComposerAdapterWithScreenInfo(
        width, height, ScreenColorGamut::COLOR_GAMUT_SRGB, ScreenState::UNKNOWN, ScreenRotation::ROTATION_0);
    composerAdapter_->output_ = nullptr;
    RSDisplayNodeConfig config;
    NodeId id = 1;
    RSDisplayRenderNode node(id, config);
    auto infoPtr = composerAdapter_->CreateLayer(node);
    composerAdapter_ = nullptr;
}

/**
 * @tc.name: CreateLayer
 * @tc.desc: RSComposerAdapter.CreateLayer test
 * @tc.type: FUNC
 * @tc.require: issueI7HDVG
 */
HWTEST_F(RSComposerAdapterTest, CreateLayer, Function | SmallTest | Level2)
{
    uint32_t width = 2160;
    uint32_t height = 1080;
    CreateComposerAdapterWithScreenInfo(
        width, height, ScreenColorGamut::COLOR_GAMUT_SRGB, ScreenState::UNKNOWN, ScreenRotation::ROTATION_0);
    RSDisplayNodeConfig config;
    constexpr NodeId nodeId = TestSrc::limitNumber::Uint64[4];
    RSDisplayRenderNode node(nodeId, config);
    sptr<IConsumerSurface> consumer = IConsumerSurface::Create("test");
    node.SetConsumer(consumer);
    sptr<SyncFence> acquireFence = SyncFence::INVALID_FENCE;
    int64_t timestamp = 0;
    Rect damage;
    sptr<OHOS::SurfaceBuffer> buffer = new SurfaceBufferImpl(0);
    node.SetBuffer(buffer, acquireFence, damage, timestamp);
    composerAdapter_->CreateLayer(node);
}

/**
 * @tc.name: LayerPresentTimestamp001
 * @tc.desc: RSComposerAdapter.LayerPresentTimestamp test, not SupportedPresentTimestamp
 * @tc.type: FUNC
 * @tc.require: issueI7HDVG
 */
HWTEST_F(RSComposerAdapterTest, LayerPresentTimestamp001, Function | SmallTest | Level2)
{
    uint32_t width = 2160;
    uint32_t height = 1080;
    CreateComposerAdapterWithScreenInfo(
        width, height, ScreenColorGamut::COLOR_GAMUT_SRGB, ScreenState::UNKNOWN, ScreenRotation::ROTATION_0);
    auto surfaceNode = RSTestUtil::CreateSurfaceNodeWithBuffer();
    ASSERT_NE(surfaceNode, nullptr);
    auto buffer = surfaceNode->GetBuffer();
    LayerInfoPtr layer = HdiLayerInfo::CreateHdiLayerInfo();
    layer->SetBuffer(buffer, surfaceNode->GetAcquireFence());
    sptr<IConsumerSurface> consumer = IConsumerSurface::Create("test");
    layer->IsSupportedPresentTimestamp_ = false;
    composerAdapter_->LayerPresentTimestamp(layer, consumer);
}

/**
 * @tc.name: LayerPresentTimestamp002
 * @tc.desc: RSComposerAdapter.LayerPresentTimestamp test, SupportedPresentTimestamp
 * @tc.type: FUNC
 * @tc.require: issueI7HDVG
 */
HWTEST_F(RSComposerAdapterTest, LayerPresentTimestamp002, Function | SmallTest | Level2)
{
    uint32_t width = 2160;
    uint32_t height = 1080;
    CreateComposerAdapterWithScreenInfo(
        width, height, ScreenColorGamut::COLOR_GAMUT_SRGB, ScreenState::UNKNOWN, ScreenRotation::ROTATION_0);
    auto surfaceNode = RSTestUtil::CreateSurfaceNodeWithBuffer();
    ASSERT_NE(surfaceNode, nullptr);
    auto buffer = surfaceNode->GetBuffer();
    LayerInfoPtr layer = HdiLayerInfo::CreateHdiLayerInfo();
    layer->SetBuffer(buffer, surfaceNode->GetAcquireFence());
    sptr<IConsumerSurface> consumer = IConsumerSurface::Create("test");
    layer->IsSupportedPresentTimestamp_ = true;
    composerAdapter_->LayerPresentTimestamp(layer, consumer);
}

/**
 * @tc.name: LayerPresentTimestamp003
 * @tc.desc: RSComposerAdapter.LayerPresentTimestamp test with null buffer
 * @tc.type: FUNC
 * @tc.require: issueI7HDVG
 */
HWTEST_F(RSComposerAdapterTest, LayerPresentTimestamp003, Function | SmallTest | Level2)
{
    uint32_t width = 2160;
    uint32_t height = 1080;
    CreateComposerAdapterWithScreenInfo(
        width, height, ScreenColorGamut::COLOR_GAMUT_SRGB, ScreenState::UNKNOWN, ScreenRotation::ROTATION_0);
    LayerInfoPtr layer = HdiLayerInfo::CreateHdiLayerInfo();
    sptr<IConsumerSurface> consumer = IConsumerSurface::Create("test");
    layer->IsSupportedPresentTimestamp_ = true;
    composerAdapter_->LayerPresentTimestamp(layer, consumer);
}

/**
 * @tc.name: OnPrepareComplete
 * @tc.desc: RSComposerAdapter.OnPrepareComplete test
 * @tc.type: FUNC
 * @tc.require: issueI7HDVG
 */
HWTEST_F(RSComposerAdapterTest, OnPrepareComplete, Function | SmallTest | Level2)
{
    uint32_t width = 2160;
    uint32_t height = 1080;
    CreateComposerAdapterWithScreenInfo(
        width, height, ScreenColorGamut::COLOR_GAMUT_SRGB, ScreenState::UNKNOWN, ScreenRotation::ROTATION_0);
    PrepareCompleteParam para;
    auto rsSurfaceRenderNode = RSTestUtil::CreateSurfaceNode();
    const auto& surfaceConsumer = rsSurfaceRenderNode->GetConsumer();
    auto producer = surfaceConsumer->GetProducer();
    sptr<Surface> sProducer = Surface::CreateSurfaceAsProducer(producer);
    composerAdapter_->OnPrepareComplete(sProducer, para, nullptr);
}
} // namespace OHOS::Rosen
