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
    std::unique_ptr<RSComposerAdapter> composerAdapter_;
    int32_t offsetX = 0; // screenOffset on x axis equals to 0
    int32_t offsetY = 0; // screenOffset on y axis equals to 0
    float mirrorAdaptiveCoefficient = 1.0f;
    sptr<RSScreenManager> screenManager_;
    static uint32_t screenId_;
};

uint32_t RSComposerAdapterTest::screenId_ = 0;

void RSComposerAdapterTest::SetUpTestCase() {}
void RSComposerAdapterTest::TearDownTestCase() {}
void RSComposerAdapterTest::CreateComposerAdapterWithScreenInfo(uint32_t width, uint32_t height,
    ScreenColorGamut colorGamut, ScreenState state, ScreenRotation rotation)
{
    auto rsScreen = std::make_unique<impl::RSScreen>(screenId_, false, HdiOutput::CreateHdiOutput(screenId_), nullptr);
    screenManager_->MockHdiScreenConnected(rsScreen);
    auto info = screenManager_->QueryScreenInfo(screenId_);
    info.width = width;
    info.height = height;
    info.colorGamut = colorGamut;
    info.state = state;
    info.rotation = rotation;
    composerAdapter_ = std::make_unique<RSComposerAdapter>();
    composerAdapter_->Init(info, offsetX, offsetY, mirrorAdaptiveCoefficient, nullptr);
    screenId_++;
}

void RSComposerAdapterTest::SetUp()
{
    screenManager_ = CreateOrGetScreenManager();
    screenManager_->Init();
    CreateComposerAdapterWithScreenInfo();
}
void RSComposerAdapterTest::TearDown()
{
    composerAdapter_ = nullptr;
    screenManager_ = nullptr;
}

/**
 * @tc.name: CommitLayers
 * @tc.desc: commitLayer when csurface is nullptr
 */
HWTEST_F(RSComposerAdapterTest, CommitLayersTest001, Function | SmallTest | Level2)
{
    SetUp();
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
 * @tc.name: CommitLayers
 * @tc.desc: commitTunnelLayer when csurface is nullptr
 */
HWTEST_F(RSComposerAdapterTest, CommitLayersTest002, Function | SmallTest | Level2)
{
    std::vector<std::shared_ptr<HdiLayerInfo>> layers;
    auto surfaceNode1 = RSTestUtil::CreateSurfaceNode();
    auto& consumer = surfaceNode1->GetConsumer();
    GraphicExtDataHandle handle;
    handle.fd = -1;
    handle.reserveInts = 1;
    consumer->SetTunnelHandle(&handle);
    CreateComposerAdapterWithScreenInfo(2160, 1080, ScreenColorGamut::COLOR_GAMUT_SRGB, ScreenState::UNKNOWN,
        ScreenRotation::ROTATION_0);
    auto infoPtr = composerAdapter_->CreateLayer(*surfaceNode1);
    layers.emplace_back(infoPtr);
    composerAdapter_->CommitLayers(layers);
    composerAdapter_ = nullptr;
}

/**
 * @tc.name: CreateLayers
 * @tc.desc: CreateLayers when surfaceNode has valid buffer
 */
HWTEST_F(RSComposerAdapterTest, CreateLayersTest001, Function | SmallTest | Level2)
{
    std::vector<std::shared_ptr<HdiLayerInfo>> layers;
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
    auto infoPtr1 = composerAdapter_->CreateLayer(*surfaceNode1);
    auto infoPtr2 = composerAdapter_->CreateLayer(*surfaceNode2);
    auto infoPtr3 = composerAdapter_->CreateLayer(*surfaceNode3);
    layers.emplace_back(infoPtr1);
    layers.emplace_back(infoPtr2);
    layers.emplace_back(infoPtr3);
    composerAdapter_->CommitLayers(layers);
    composerAdapter_ = nullptr;
}

/**
 * @tc.name: CreateLayers
 * @tc.desc: CreateLayers when surfaceNode is out of screen region
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
    auto infoPtr1 = composerAdapter_->CreateLayer(*surfaceNode1);
    auto infoPtr2 = composerAdapter_->CreateLayer(*surfaceNode2);
    composerAdapter_ = nullptr;
}

/**
 * @tc.name: CreateLayers
 * @tc.desc: CreateLayers with screen rotation
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
    auto infoPtr1 = composerAdapter_->CreateLayer(*surfaceNode1);
    composerAdapter_ = nullptr;
}

/**
 * @tc.name: CreateLayers
 * @tc.desc: CreateLayers with screen rotation
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
    auto infoPtr1 = composerAdapter_->CreateLayer(*surfaceNode1);
    composerAdapter_ = nullptr;
}

/**
 * @tc.name: CreateLayers
 * @tc.desc: CreateLayers with screen rotation
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
    auto infoPtr1 = composerAdapter_->CreateLayer(*surfaceNode1);
    composerAdapter_ = nullptr;
}

/**
 * @tc.name: CreateLayers
 * @tc.desc: CreateLayers with screen rotation and scale
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
    auto infoPtr1 = composerAdapter_->CreateLayer(*surfaceNode1);
    composerAdapter_ = nullptr;
}

/**
 * @tc.name: CreateLayers
 * @tc.desc: CreateLayers when surfaceNode has metadate
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
    auto infoPtr1 = composerAdapter_->CreateLayer(*surfaceNode1);
    composerAdapter_ = nullptr;
}

/**
 * @tc.name: CreateLayers
 * @tc.desc: CreateLayers when surfaceNode has gravity
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
    auto infoPtr1 = composerAdapter_->CreateLayer(*surfaceNode1);
    composerAdapter_ = nullptr;
}

/**
 * @tc.name: CreateLayers
 * @tc.desc: CreateLayers when surfacenode has scaling mode
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
    auto infoPtr1 = composerAdapter_->CreateLayer(*surfaceNode1);
    TearDown();
}

} // namespace OHOS::Rosen
