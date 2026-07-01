/*
 * Copyright (c) 2025 Huawei Device Co., Ltd.
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

#include <cstdint>
#include <gtest/gtest.h>
#include <hilog/log.h>
#include <iservice_registry.h>
#include <memory>
#include <parameters.h>
#include <sys/mman.h>
#include <sys/wait.h>
#include <unistd.h>

#include "connection/rs_render_to_composer_connection.h"
#include "common/rs_tunnel_layer_utils.h"
#include "consumer_surface.h"
#include "feature/hyper_graphic_manager/hgm_context.h"
#include "feature/tunnel_layer/rs_tunnel_runtime_state.h"
#include "layer_backend/hdi_output.h"
#ifdef RS_ENABLE_VK
#include "platform/ohos/backend/rs_vulkan_context.h"
#endif
#include "layer/rs_surface_layer.h"
#include "surface_buffer.h"

#include "pipeline/rs_composer_client.h"
#include "pipeline/rs_render_composer_agent.h"
#include "pipeline/rs_render_composer_manager.h"
#include "pipeline/rs_surface_handler.h"
#include "screen_manager/rs_screen_property.h"

using namespace testing::ext;

namespace OHOS {
namespace Rosen {
namespace {
constexpr uint64_t TEST_TUNNEL_LAYER_ID = 789u;
constexpr uint64_t TEST_NODE_ID = 456u;
constexpr uint64_t TEST_LAYER_ID_BASE = 1200u;

class ScopedNewTunnelSwitch {
public:
    explicit ScopedNewTunnelSwitch(bool enabled) : oldValue_(Rosen::IsNewTunnelEnabled())
    {
        system::SetParameter("rosen.debug.new_tunnel", enabled ? "true" : "false");
    }

    ~ScopedNewTunnelSwitch()
    {
        system::SetParameter("rosen.debug.new_tunnel", oldValue_ ? "true" : "false");
    }

private:
    bool oldValue_;
};

std::shared_ptr<RSComposerContext> CreateContextWithLayerStateRecorder(
    const std::shared_ptr<RSRenderComposerManager>& manager, uint32_t screenId,
    std::vector<LayerStateChange>& reportedStates)
{
    if (manager == nullptr) {
        return nullptr;
    }
    auto conn = manager->GetRSComposerConnection(screenId);
    sptr<IRSRenderToComposerConnection> ifaceConn = conn;
    auto localClient = RSComposerClient::Create(ifaceConn, nullptr);
    if (localClient == nullptr) {
        return nullptr;
    }
    localClient->RegisterLayerStateChangedCB(
        [&reportedStates](uint64_t, LayerStateChange state, uint64_t) {
            reportedStates.emplace_back(state);
        });
    return localClient->GetComposerContext();
}
} // namespace

class RSSurfaceLayerTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;

    static inline uint32_t screenId = 0;
    static inline std::shared_ptr<RSComposerClient> client;
    static inline std::shared_ptr<RSSurfaceLayer> layer;
    static inline std::shared_ptr<RSRenderComposerManager> sMgr;
};

void RSSurfaceLayerTest::SetUpTestCase()
{
#ifdef RS_ENABLE_VK
    RsVulkanContext::SetRecyclable(false);
#endif
    std::shared_ptr<AppExecFwk::EventHandler> handler = nullptr;
    sMgr = std::make_shared<RSRenderComposerManager>(handler);
    auto output = std::make_shared<HdiOutput>(screenId);
    output->Init();
    sptr<RSScreenProperty> property = new RSScreenProperty();
    sMgr->OnScreenConnected(output, property);
    auto conn = sMgr->GetRSComposerConnection(screenId);
    sptr<IRSRenderToComposerConnection> ifaceConn = conn;
    client = RSComposerClient::Create(ifaceConn, nullptr);
}

void RSSurfaceLayerTest::TearDownTestCase()
{
    for (auto [screenId, agent] : sMgr->rsRenderComposerAgentMap_) {
        agent->rsRenderComposer_->uniRenderEngine_ = nullptr;
    }
}

void RSSurfaceLayerTest::SetUp() {}
void RSSurfaceLayerTest::TearDown() {}

/**
 * @tc.name: CreateTest
 * @tc.desc: Test RSSurfaceLayer::Create
 * @tc.type: FUNC
 * @tc.require: #I9NVOG
 */
HWTEST_F(RSSurfaceLayerTest, CreateTest, Function | SmallTest | Level2)
{
    std::shared_ptr<RSComposerContext> context = nullptr;
    EXPECT_EQ(RSSurfaceLayer::Create(0, context), nullptr);
}

/**
 * @tc.name: CreateLayerTest
 * @tc.desc: Test RSSurfaceLayer::Create
 * @tc.type: FUNC
 * @tc.require: #I9NVOG
 */
HWTEST_F(RSSurfaceLayerTest, CreateLayerTest, Function | SmallTest | Level2)
{
    auto layer1 = std::make_shared<RSSurfaceLayer>(0, nullptr);
    EXPECT_NE(layer1, nullptr);

    auto layer2 = std::make_shared<RSSurfaceLayer>(0, nullptr);
    EXPECT_NE(layer2, nullptr);
}

/**
 * @tc.name: TunnelLayerDestroyedCallback001
 * @tc.desc: Test tunnel layer destruction forwards DESTROYED through local layer state callback.
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSSurfaceLayerTest, TunnelLayerDestroyedCallback001, Function | SmallTest | Level2)
{
    ScopedNewTunnelSwitch scopedNewTunnelSwitch(true);
    auto consumer = IConsumerSurface::Create();
    ASSERT_NE(consumer, nullptr);
    sptr<IBufferProducer> producerObj = consumer->GetProducer();
    auto producer = Surface::CreateSurfaceAsProducer(producerObj);
    ASSERT_NE(producer, nullptr);

    uint64_t reportedNodeId = 0;
    uint64_t reportedGeneration = 0;
    constexpr uint64_t expectedGeneration = 321u;
    std::vector<LayerStateChange> reportedStates;
    auto conn = sMgr->GetRSComposerConnection(screenId);
    sptr<IRSRenderToComposerConnection> ifaceConn = conn;
    auto localClient = RSComposerClient::Create(ifaceConn, nullptr);
    ASSERT_NE(localClient, nullptr);
    localClient->RegisterLayerStateChangedCB(
        [&reportedNodeId, &reportedGeneration, &reportedStates](
            uint64_t nodeId, LayerStateChange state, uint64_t generation) {
            reportedNodeId = nodeId;
            reportedGeneration = generation;
            reportedStates.emplace_back(state);
        });

    auto context = localClient->GetComposerContext();
    ASSERT_NE(context, nullptr);
    {
        auto rsLayer = std::static_pointer_cast<RSSurfaceLayer>(RSSurfaceLayer::Create(123u, context));
        ASSERT_NE(rsLayer, nullptr);
        rsLayer->SetNodeId(456u);
        rsLayer->SetSurface(consumer);
        rsLayer->SetTunnelLayerId(789u);
        rsLayer->SetTunnelLayerProperty(TUNNEL_PROP_RS_FORCE);
        rsLayer->SetTunnelLayerGeneration(expectedGeneration);
    }

    ASSERT_EQ(reportedStates.size(), 1u);
    EXPECT_EQ(reportedNodeId, 456u);
    EXPECT_EQ(reportedGeneration, expectedGeneration);
    EXPECT_EQ(reportedStates[0], LayerStateChange::UNAVAILABLE);
}

/**
 * @tc.name: TunnelLayerDestroyedCallback002
 * @tc.desc: Test tunnel layer destruction keeps callback when property is invalid.
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSSurfaceLayerTest, TunnelLayerDestroyedCallback002, Function | SmallTest | Level2)
{
    ScopedNewTunnelSwitch scopedNewTunnelSwitch(true);
    auto consumer = IConsumerSurface::Create();
    ASSERT_NE(consumer, nullptr);
    sptr<IBufferProducer> producerObj = consumer->GetProducer();
    auto producer = Surface::CreateSurfaceAsProducer(producerObj);
    ASSERT_NE(producer, nullptr);

    uint64_t reportedNodeId = 0;
    std::vector<LayerStateChange> reportedStates;
    auto conn = sMgr->GetRSComposerConnection(screenId);
    sptr<IRSRenderToComposerConnection> ifaceConn = conn;
    auto localClient = RSComposerClient::Create(ifaceConn, nullptr);
    ASSERT_NE(localClient, nullptr);
    localClient->RegisterLayerStateChangedCB(
        [&reportedNodeId, &reportedStates](uint64_t nodeId, LayerStateChange state, uint64_t) {
            reportedNodeId = nodeId;
            reportedStates.emplace_back(state);
        });

    auto context = localClient->GetComposerContext();
    ASSERT_NE(context, nullptr);
    {
        auto rsLayer = std::static_pointer_cast<RSSurfaceLayer>(RSSurfaceLayer::Create(124u, context));
        ASSERT_NE(rsLayer, nullptr);
        rsLayer->SetNodeId(457u);
        rsLayer->SetSurface(consumer);
        rsLayer->SetTunnelLayerId(790u);
    }

    ASSERT_EQ(reportedStates.size(), 1u);
    EXPECT_EQ(reportedNodeId, 457u);
    EXPECT_EQ(reportedStates[0], LayerStateChange::UNAVAILABLE);
}

/**
 * @tc.name: TunnelLayerDestroyedCallback003
 * @tc.desc: Test tunnel layer destruction skips callback for disabled, null-surface, and invalid fallback states.
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSSurfaceLayerTest, TunnelLayerDestroyedCallback003, Function | SmallTest | Level2)
{
    struct TestCase {
        const char* name;
        bool newTunnelEnabled;
        bool setSurface;
        bool initSurface;
        uint64_t layerTunnelId;
        uint32_t layerProperty;
    };
    const std::vector<TestCase> testCases = {
        { "new_tunnel_disabled", false, true, true, TEST_TUNNEL_LAYER_ID, TUNNEL_PROP_BUFFER_ADDR },
        { "surface_null", true, false, false, TEST_TUNNEL_LAYER_ID, TUNNEL_PROP_BUFFER_ADDR },
        { "fallback_get_info_failed", true, true, false, 0, TUNNEL_PROP_INVALID },
        { "fallback_tunnel_layer_id_zero", true, true, true, 0, TUNNEL_PROP_INVALID },
    };

    for (size_t index = 0; index < testCases.size(); ++index) {
        const auto& testCase = testCases[index];
        SCOPED_TRACE(testing::Message() << "case=" << testCase.name);
        ScopedNewTunnelSwitch scopedNewTunnelSwitch(testCase.newTunnelEnabled);
        std::vector<LayerStateChange> reportedStates;
        auto context = CreateContextWithLayerStateRecorder(sMgr, screenId, reportedStates);
        ASSERT_NE(context, nullptr);

        sptr<IConsumerSurface> consumer = nullptr;
        if (testCase.initSurface) {
            consumer = IConsumerSurface::Create(testCase.name);
        } else {
            consumer = new ConsumerSurface(testCase.name);
        }
        ASSERT_NE(consumer, nullptr);

        {
            auto rsLayer = std::static_pointer_cast<RSSurfaceLayer>(
                RSSurfaceLayer::Create(TEST_LAYER_ID_BASE + index, context));
            ASSERT_NE(rsLayer, nullptr);
            rsLayer->SetNodeId(TEST_NODE_ID + index);
            if (testCase.setSurface) {
                rsLayer->SetSurface(consumer);
            }
            rsLayer->SetTunnelLayerId(testCase.layerTunnelId);
            rsLayer->SetTunnelLayerProperty(testCase.layerProperty);
        }

        EXPECT_TRUE(reportedStates.empty());
    }
}

/**
 * @tc.name: TunnelLayerDestroyedCallback_NodeIdFallback004
 * @tc.desc: Test tunnel layer destruction uses nodeId fallback
 *           when layer tunnelLayerId is zero but nodeId has stored info.
 * @tc.type: FUNC
 */
HWTEST_F(RSSurfaceLayerTest, TunnelLayerDestroyedCallback_NodeIdFallback004, Function | SmallTest | Level2)
{
    ScopedNewTunnelSwitch scopedNewTunnelSwitch(true);
    auto consumer = IConsumerSurface::Create();
    ASSERT_NE(consumer, nullptr);
    sptr<IBufferProducer> producerObj = consumer->GetProducer();
    auto producer = Surface::CreateSurfaceAsProducer(producerObj);
    ASSERT_NE(producer, nullptr);
 
    constexpr uint64_t testNodeId = 5001u;
    constexpr uint64_t fallbackTunnelLayerId = 8001u;
    constexpr uint32_t fallbackProperty = TUNNEL_PROP_BUFFER_ADDR | TUNNEL_PROP_DEVICE_COMMIT;
    constexpr uint64_t expectedGeneration = 555u;
 
    std::vector<LayerStateChange> reportedStates;
    uint64_t reportedNodeId = 0;
    uint64_t reportedGeneration = 0;
    auto conn = sMgr->GetRSComposerConnection(screenId);
    sptr<IRSRenderToComposerConnection> ifaceConn = conn;
    auto localClient = RSComposerClient::Create(ifaceConn, nullptr);
    ASSERT_NE(localClient, nullptr);
    localClient->RegisterLayerStateChangedCB(
        [&reportedNodeId, &reportedGeneration, &reportedStates](
            uint64_t nodeId, LayerStateChange state, uint64_t generation) {
            reportedNodeId = nodeId;
            reportedGeneration = generation;
            reportedStates.emplace_back(state);
        });
 
    auto context = localClient->GetComposerContext();
    ASSERT_NE(context, nullptr);
 
    RSTunnelRuntimeStore::SetLayerInfo(testNodeId, fallbackTunnelLayerId, fallbackProperty);
    RSTunnelRuntimeStore::GetOrCreate(testNodeId).SetBuilding();
    RSTunnelRuntimeStore::GetOrCreate(testNodeId).SetActiveFromTunnelLayerAvailable(
        RSTunnelRuntimeStore::GetOrCreate(testNodeId).GetTunnelLayerGeneration());
 
    {
        auto rsLayer = std::static_pointer_cast<RSSurfaceLayer>(RSSurfaceLayer::Create(125u, context));
        ASSERT_NE(rsLayer, nullptr);
        rsLayer->SetNodeId(testNodeId);
        rsLayer->SetSurface(consumer);
        rsLayer->SetTunnelLayerId(0);
        rsLayer->SetTunnelLayerProperty(TUNNEL_PROP_INVALID);
        rsLayer->SetTunnelLayerGeneration(expectedGeneration);
    }
 
    ASSERT_EQ(reportedStates.size(), 1u);
    EXPECT_EQ(reportedNodeId, testNodeId);
    EXPECT_EQ(reportedGeneration, expectedGeneration);
    EXPECT_EQ(reportedStates[0], LayerStateChange::UNAVAILABLE);
 
    RSTunnelRuntimeStore::Erase(testNodeId);
}
 
/**
 * @tc.name: TunnelLayerDestroyedCallback_NodeIdFallbackSkipped005
 * @tc.desc: Test tunnel layer destruction skips callback when both layer and nodeId store have zero tunnelLayerId.
 * @tc.type: FUNC
 */
HWTEST_F(RSSurfaceLayerTest, TunnelLayerDestroyedCallback_NodeIdFallbackSkipped005, Function | SmallTest | Level2)
{
    ScopedNewTunnelSwitch scopedNewTunnelSwitch(true);
    auto consumer = IConsumerSurface::Create();
    ASSERT_NE(consumer, nullptr);
 
    constexpr uint64_t testNodeId = 5002u;
    std::vector<LayerStateChange> reportedStates;
    auto context = CreateContextWithLayerStateRecorder(sMgr, screenId, reportedStates);
    ASSERT_NE(context, nullptr);
 
    RSTunnelRuntimeStore::SetLayerInfo(testNodeId, 0, TUNNEL_PROP_INVALID);
 
    {
        auto rsLayer = std::static_pointer_cast<RSSurfaceLayer>(RSSurfaceLayer::Create(126u, context));
        ASSERT_NE(rsLayer, nullptr);
        rsLayer->SetNodeId(testNodeId);
        rsLayer->SetSurface(consumer);
        rsLayer->SetTunnelLayerId(0);
        rsLayer->SetTunnelLayerProperty(TUNNEL_PROP_INVALID);
    }
 
    EXPECT_TRUE(reportedStates.empty());
 
    RSTunnelRuntimeStore::Erase(testNodeId);
}

/**
 * @tc.name: LayerPropertiesChangeTest
 * @tc.desc: Test Change RSLayer Properties
 * @tc.type: FUNC
 * @tc.require: #I9NVOG
 */
HWTEST_F(RSSurfaceLayerTest, LayerPropertiesChangeTest, Function | SmallTest | Level2)
{
    layer = std::make_shared<RSSurfaceLayer>(0, nullptr);
    EXPECT_NE(layer, nullptr);

    GraphicLayerAlpha alpha;
    alpha.enPixelAlpha = false;
    layer->SetAlpha(alpha);
    EXPECT_EQ(layer->GetAlpha().enPixelAlpha, false);

    layer->SetZorder(100);
    EXPECT_EQ(layer->GetZorder(), 100);

    GraphicLayerType type = GRAPHIC_LAYER_TYPE_GRAPHIC;
    layer->SetType(type);
    EXPECT_EQ(layer->GetType(), type);

    GraphicTransformType type2 = GRAPHIC_ROTATE_NONE;
    layer->SetTransform(type2);
    EXPECT_EQ(layer->GetTransform(), type2);

    GraphicCompositionType type3 = GRAPHIC_COMPOSITION_CLIENT;
    layer->SetCompositionType(type3);
    EXPECT_EQ(layer->GetCompositionType(), type3);

    std::vector<GraphicIRect> visibleRegions;
    layer->SetVisibleRegions(visibleRegions);
    EXPECT_EQ(layer->GetVisibleRegions(), visibleRegions);

    std::vector<GraphicIRect> dirtyRegions;
    layer->SetDirtyRegions(dirtyRegions);
    EXPECT_EQ(layer->GetDirtyRegions(), dirtyRegions);

    GraphicBlendType type4 = GRAPHIC_BLEND_NONE;
    layer->SetBlendType(type4);
    EXPECT_EQ(layer->GetBlendType(), type4);

    GraphicIRect crop;
    layer->SetCropRect(crop);
    EXPECT_EQ(layer->GetCropRect(), crop);

    layer->SetPreMulti(false);
    EXPECT_EQ(layer->IsPreMulti(), false);
}

/**
 * @tc.name: LayerPropertiesChangeTest
 * @tc.desc: Test Change RSLayer Properties
 * @tc.type: FUNC
 * @tc.require: #I9NVOG
 */
HWTEST_F(RSSurfaceLayerTest, LayerPropertiesChangeTest2, Function | SmallTest | Level2)
{
    GraphicIRect layerRect;
    layer->SetLayerSize(layerRect);
    EXPECT_EQ(layer->GetLayerSize(), layerRect);

    GraphicIRect boundRect;
    layer->SetBoundSize(boundRect);
    EXPECT_EQ(layer->GetBoundSize(), boundRect);

    GraphicLayerColor layerColor;
    layerColor.r = 100;
    layer->SetLayerColor(layerColor);
    EXPECT_EQ(layer->GetLayerColor().r, 100);

    GraphicLayerColor backgroundColor;
    backgroundColor.r = 10;
    layer->SetBackgroundColor(backgroundColor);
    EXPECT_EQ(layer->GetBackgroundColor().r, 10);

    std::vector<float> drmCornerRadiusInfo;
    layer->SetCornerRadiusInfoForDRM(drmCornerRadiusInfo);
    EXPECT_EQ(layer->GetCornerRadiusInfoForDRM(), drmCornerRadiusInfo);

    RSVcldParam vcldInfo;
    vcldInfo.enable = true;
    vcldInfo.radius = 25;
    layer->SetVcldInfo(vcldInfo);
    EXPECT_EQ(layer->GetVcldInfo(), vcldInfo);

    std::vector<float> matrix;
    layer->SetColorTransform(matrix);
    EXPECT_EQ(layer->GetColorTransform(), matrix);

    GraphicColorDataSpace colorSpace = GRAPHIC_COLOR_DATA_SPACE_UNKNOWN;
    layer->SetColorDataSpace(colorSpace);
    EXPECT_EQ(layer->GetColorDataSpace(), colorSpace);

    std::vector<GraphicHDRMetaData> metaData;
    GraphicHDRMetaData a;
    a.key = GRAPHIC_MATAKEY_RED_PRIMARY_Y;
    a.value = 100;
    metaData.push_back(a);
    layer->SetMetaData(metaData);
    EXPECT_EQ(layer->GetMetaData()[0].key, GRAPHIC_MATAKEY_RED_PRIMARY_Y);

    GraphicHDRMetaDataSet metaDataSet;
    metaDataSet.key = GRAPHIC_MATAKEY_BLUE_PRIMARY_X;
    layer->SetMetaDataSet(metaDataSet);
    EXPECT_EQ(layer->GetMetaDataSet().key, GRAPHIC_MATAKEY_BLUE_PRIMARY_X);

    GraphicMatrix matrix2;
    matrix2.scaleX = 2;
    layer->SetMatrix(matrix2);
    EXPECT_EQ(layer->GetMatrix().scaleX, 2);
}

/**
 * @tc.name: LayerPropertiesChangeTest
 * @tc.desc: Test Change RSLayer Properties
 * @tc.type: FUNC
 * @tc.require: #I9NVOG
 */
HWTEST_F(RSSurfaceLayerTest, LayerPropertiesChangeTest3, Function | SmallTest | Level2)
{
    int32_t gravity = 0;
    layer->SetGravity(gravity);
    EXPECT_EQ(layer->GetGravity(), gravity);

    layer->SetUniRenderFlag(false);
    EXPECT_EQ(layer->GetUniRenderFlag(), false);

    layer->SetTunnelHandleChange(false);
    EXPECT_EQ(layer->GetTunnelHandleChange(), false);

    sptr<SurfaceTunnelHandle> handle = sptr<SurfaceTunnelHandle>::MakeSptr();
    layer->SetTunnelHandle(handle);
    EXPECT_EQ(layer->GetTunnelHandle(), handle);

    layer->SetTunnelLayerId(100);
    EXPECT_EQ(layer->GetTunnelLayerId(), 100);

    layer->SetTunnelLayerProperty(1000);
    EXPECT_EQ(layer->GetTunnelLayerProperty(), 1000);

    GraphicPresentTimestamp timestamp;
    timestamp.type = GRAPHIC_DISPLAY_PTS_DELAY;
    layer->SetPresentTimestamp(timestamp);
    EXPECT_EQ(layer->GetPresentTimestamp().type, GRAPHIC_DISPLAY_PTS_DELAY);

    layer->SetIsSupportedPresentTimestamp(false);
    EXPECT_EQ(layer->GetIsSupportedPresentTimestamp(), false);

    layer->SetSdrNit(200);
    EXPECT_EQ(layer->GetSdrNit(), 200);

    layer->SetDisplayNit(300);
    EXPECT_EQ(layer->GetDisplayNit(), 300);

    layer->SetBrightnessRatio(300);
    EXPECT_EQ(layer->GetBrightnessRatio(), 300);

    std::vector<float> layerLinearMatrix;
    layerLinearMatrix.push_back(100);
    layer->SetLayerLinearMatrix(layerLinearMatrix);
    EXPECT_EQ(layer->GetLayerLinearMatrix()[0], 100);

    layer->SetLayerSourceTuning(100);
    EXPECT_EQ(layer->GetLayerSourceTuning(), 100);
}

/**
 * @tc.name: LayerPropertiesChangeTest
 * @tc.desc: Test Change RSLayer Properties
 * @tc.type: FUNC
 * @tc.require: #I9NVOG
 */
HWTEST_F(RSSurfaceLayerTest, LayerPropertiesChangeTest4, Function | SmallTest | Level2)
{
    std::vector<std::string> windowsName;
    windowsName.push_back("window");
    layer->SetWindowsName(windowsName);
    EXPECT_EQ(layer->GetWindowsName()[0], "window");

    layer->SetRotationFixed(false);
    EXPECT_EQ(layer->GetRotationFixed(), false);

    layer->SetLayerArsr(false);
    EXPECT_EQ(layer->GetLayerArsr(), false);

    layer->SetLayerCopybit(false);
    EXPECT_EQ(layer->GetLayerCopybit(), false);

    layer->SetNeedBilinearInterpolation(true);
    EXPECT_EQ(layer->GetNeedBilinearInterpolation(), true);

    layer->SetIsMaskLayer(false);
    EXPECT_EQ(layer->GetIsMaskLayer(), false);

    layer->SetNodeId(100);
    EXPECT_EQ(layer->GetNodeId(), 100);

    layer->SetAncoFlags(0);
    EXPECT_EQ(layer->GetAncoFlags(), 0);

    EXPECT_EQ(layer->IsAncoNative(), false);

    LayerMask mask = {};
    layer->SetLayerMaskInfo(mask);
    EXPECT_EQ(layer->GetLayerMaskInfo(), mask);

    sptr<IConsumerSurface> surface;
    layer->SetSurface(surface);
    EXPECT_EQ(layer->GetSurface(), surface);

    sptr<SurfaceBuffer> sbuffer;
    sptr<SyncFence> acquireFence;
    layer->SetBuffer(sbuffer, acquireFence);
    layer->SetBuffer(sbuffer);
    EXPECT_EQ(layer->GetBuffer(), sbuffer);
    EXPECT_EQ(layer->acquireFence_, acquireFence);

    sptr<SurfaceBuffer> pbuffer;
    layer->SetPreBuffer(pbuffer);
    EXPECT_EQ(layer->GetPreBuffer(), pbuffer);
}

/**
 * @tc.name: LayerPropertiesChangeTest
 * @tc.desc: Test Change RSLayer Properties
 * @tc.type: FUNC
 * @tc.require: #I9NVOG
 */
HWTEST_F(RSSurfaceLayerTest, LayerPropertiesChangeTest5, Function | SmallTest | Level2)
{
    sptr<SyncFence> acquireFence2;
    layer->SetAcquireFence(acquireFence2);
    EXPECT_EQ(layer->GetAcquireFence(), acquireFence2);

    layer->SetUseDeviceOffline(false);
    EXPECT_EQ(layer->GetUseDeviceOffline(), false);

    layer->SetIgnoreAlpha(false);
    EXPECT_EQ(layer->GetIgnoreAlpha(), false);

    auto layer1 = std::make_shared<RSSurfaceLayer>(0, nullptr);
    layer1->CopyLayerInfo(layer);

    layer->cSurface_ = nullptr;
    layer->DumpCurrentFrameLayer();
    layer->cSurface_ = IConsumerSurface::Create("test consumer");
    layer->DumpCurrentFrameLayer();
    EXPECT_NE(layer, nullptr);
}

/**
 * @tc.name: LayerPropertiesChangeTest
 * @tc.desc: Test Change RSLayer Properties
 * @tc.type: FUNC
 * @tc.require: #I9NVOG
 */
HWTEST_F(RSSurfaceLayerTest, LayerPropertiesChangeTest6, Function | SmallTest | Level2)
{
    std::string result = "";
    layer->transformType_ = GRAPHIC_ROTATE_NONE;
    layer->compositionType_ = GRAPHIC_COMPOSITION_CLIENT;
    layer->blendType_ = GRAPHIC_BLEND_NONE;
    layer->cSurface_ = IConsumerSurface::Create("test consumer");
    layer->Dump(result);
    EXPECT_NE(result, "");

    result = "";
    layer->transformType_ = static_cast<GraphicTransformType>(13);
    layer->compositionType_ = GRAPHIC_COMPOSITION_CLIENT;
    layer->blendType_ = GRAPHIC_BLEND_NONE;
    layer->cSurface_ = nullptr;
    layer->Dump(result);
    EXPECT_NE(result, "");

    result = "";
    layer->transformType_ = GRAPHIC_ROTATE_NONE;
    layer->compositionType_ = static_cast<GraphicCompositionType>(13);
    layer->blendType_ = GRAPHIC_BLEND_NONE;
    layer->Dump(result);
    EXPECT_NE(result, "");

    result = "";
    layer->transformType_ = static_cast<GraphicTransformType>(13);
    layer->compositionType_ = static_cast<GraphicCompositionType>(13);
    layer->blendType_ = GRAPHIC_BLEND_NONE;
    layer->Dump(result);
    EXPECT_NE(result, "");

    result = "";
    layer->transformType_ = GRAPHIC_ROTATE_NONE;
    layer->compositionType_ = static_cast<GraphicCompositionType>(13);
    layer->blendType_ = static_cast<GraphicBlendType>(20);
    layer->Dump(result);
    EXPECT_NE(result, "");
}

/**
 * @tc.name: LayerPropertiesChangeTest
 * @tc.desc: Test Change RSLayer Properties
 * @tc.type: FUNC
 * @tc.require: #I9NVOG
 */
HWTEST_F(RSSurfaceLayerTest, LayerPropertiesChangeTest7, Function | SmallTest | Level2)
{
    std::string result = "";
    layer->transformType_ = GRAPHIC_ROTATE_NONE;
    layer->compositionType_ = GRAPHIC_COMPOSITION_CLIENT;
    layer->blendType_ = GRAPHIC_BLEND_NONE;
    layer->cSurface_ = IConsumerSurface::Create("test consumer");
    layer->Dump(result);
    EXPECT_NE(result, "");

    result = "";
    layer->transformType_ = static_cast<GraphicTransformType>(13);
    layer->compositionType_ = static_cast<GraphicCompositionType>(13);
    layer->blendType_ = static_cast<GraphicBlendType>(20);
    layer->Dump(result);
    EXPECT_NE(result, "");

    result = "";
    layer->transformType_ = GRAPHIC_ROTATE_NONE;
    layer->compositionType_ = GRAPHIC_COMPOSITION_CLIENT;
    layer->blendType_ = static_cast<GraphicBlendType>(20);
    layer->Dump(result);
    EXPECT_NE(result, "");

    result = "";
    layer->transformType_ = static_cast<GraphicTransformType>(13);
    layer->compositionType_ = GRAPHIC_COMPOSITION_CLIENT;
    layer->blendType_ = static_cast<GraphicBlendType>(20);
    layer->Dump(result);
    EXPECT_NE(result, "");
}

/**
 * @tc.name: BufferOwnerCount_SetGetAndSeqRetrieve
 * @tc.desc: Verify BufferOwnerCount set/get and sequence retrieval logic
 * @tc.type: FUNC
 */
HWTEST_F(RSSurfaceLayerTest, BufferOwnerCount_SetGetAndSeqRetrieve, Function | SmallTest | Level2)
{
    auto lyr = std::make_shared<RSSurfaceLayer>(0, nullptr);
    ASSERT_NE(lyr, nullptr);

    auto boc = std::make_shared<RSSurfaceHandler::BufferOwnerCount>();
    boc->bufferId_ = 42u;
    std::atomic<int> released { 0 };
    boc->bufferReleaseCb_ = [&released](uint64_t seq) {
        (void)seq;
        released.fetch_add(1);
    };

    lyr->SetBufferOwnerCount(boc, true);
    auto cur = lyr->GetBufferOwnerCount();
    ASSERT_NE(cur, nullptr);
    EXPECT_EQ(cur->bufferId_, 42u);
    EXPECT_GE(cur->refCount_.load(), 2); // AddRef called when inserted first time

    // Repeat set with same seq should not AddRef again
    lyr->SetBufferOwnerCount(boc, true);
    EXPECT_GE(cur->refCount_.load(), 2);

    // Retrieve by seqNum and ensure it's the same object
    auto got = lyr->PopBufferOwnerCountById(42u);
    ASSERT_NE(got, nullptr);
    EXPECT_EQ(got.get(), boc.get());
}

/**
 * @tc.name: BufferOwnerCount_Null_NoCrash
 * @tc.desc: SetBufferOwnerCount with nullptr should be ignored harmlessly
 * @tc.type: FUNC
 */
HWTEST_F(RSSurfaceLayerTest, BufferOwnerCount_Null_NoCrash, Function | SmallTest | Level2)
{
    auto lyr = std::make_shared<RSSurfaceLayer>(0, nullptr);
    lyr->SetBufferOwnerCount(nullptr, true);
    EXPECT_EQ(lyr->GetBufferOwnerCount(), nullptr);
    EXPECT_EQ(lyr->PopBufferOwnerCountById(123u), nullptr);
}

/**
 * @tc.name: BufferOwnerCount_ReplaceSeq_UpdatesCurrentAndRetrieval
 * @tc.desc: Setting different seq updates current pointer and AddRef, retrieval removes entries
 * @tc.type: FUNC
 */
HWTEST_F(RSSurfaceLayerTest, BufferOwnerCount_ReplaceSeq_UpdatesCurrentAndRetrieval, Function | SmallTest | Level2)
{
    auto lyr = std::make_shared<RSSurfaceLayer>(0, nullptr);
    auto boc1 = std::make_shared<RSSurfaceHandler::BufferOwnerCount>();
    auto boc2 = std::make_shared<RSSurfaceHandler::BufferOwnerCount>();
    boc1->bufferId_ = 1u;
    boc2->bufferId_ = 2u;

    lyr->SetBufferOwnerCount(boc1, true);
    auto cur1 = lyr->GetBufferOwnerCount();
    ASSERT_NE(cur1, nullptr);
    EXPECT_EQ(cur1->bufferId_, 1u);
    EXPECT_GE(cur1->refCount_.load(), 2);

    lyr->SetBufferOwnerCount(boc2, true);
    auto cur2 = lyr->GetBufferOwnerCount();
    ASSERT_NE(cur2, nullptr);
    EXPECT_EQ(cur2->bufferId_, 2u);
    EXPECT_GE(cur2->refCount_.load(), 2);

    // Retrieve seq 1 then 2, ensure removal behavior
    auto got1 = lyr->PopBufferOwnerCountById(1u);
    ASSERT_NE(got1, nullptr);
    EXPECT_EQ(got1.get(), boc1.get());
    EXPECT_EQ(lyr->PopBufferOwnerCountById(1u), nullptr);

    auto got2 = lyr->PopBufferOwnerCountById(2u);
    ASSERT_NE(got2, nullptr);
    EXPECT_EQ(got2.get(), boc2.get());
    EXPECT_EQ(lyr->PopBufferOwnerCountById(2u), nullptr);

    // Current pointer stays as last set, independent from map erase
    auto stillCur = lyr->GetBufferOwnerCount();
    ASSERT_NE(stillCur, nullptr);
    EXPECT_EQ(stillCur.get(), boc2.get());
}

/**
 * @tc.name: BufferOwnerCount_UnknownSeq_NoRemoval
 * @tc.desc: Unknown seq retrieval returns nullptr and does not alter existing entries
 * @tc.type: FUNC
 */
HWTEST_F(RSSurfaceLayerTest, BufferOwnerCount_UnknownSeq_NoRemoval, Function | SmallTest | Level2)
{
    auto lyr = std::make_shared<RSSurfaceLayer>(0, nullptr);
    auto boc = std::make_shared<RSSurfaceHandler::BufferOwnerCount>();
    boc->bufferId_ = 9u;
    lyr->SetBufferOwnerCount(boc, true);

    EXPECT_EQ(lyr->PopBufferOwnerCountById(8u), nullptr);
    auto got = lyr->PopBufferOwnerCountById(9u);
    ASSERT_NE(got, nullptr);
    EXPECT_EQ(got.get(), boc.get());
}

/**
 * @tc.name: SetOriginalBufferOwnerCount_BasicTest
 * @tc.desc: Verify SetOriginalBufferOwnerCount stores and AddRefs for new buffer
 * @tc.type: FUNC
 */
HWTEST_F(RSSurfaceLayerTest, SetOriginalBufferOwnerCount_BasicTest, Function | SmallTest | Level2)
{
    auto lyr = std::make_shared<RSSurfaceLayer>(0, nullptr);
    ASSERT_NE(lyr, nullptr);
 
    auto boc = std::make_shared<RSSurfaceHandler::BufferOwnerCount>();
    boc->bufferId_ = 200u;
    int initialRef = boc->refCount_.load();
 
    lyr->SetOriginalBufferOwnerCount(boc);
 
    EXPECT_EQ(boc->refCount_.load(), initialRef + 1);
    EXPECT_EQ(lyr->GetOriginalBufferOwnerCount(), boc);
}
 
/**
 * @tc.name: SetOriginalBufferOwnerCount_DuplicateBufferNoAddRef
 * @tc.desc: Verify SetOriginalBufferOwnerCount does NOT AddRef when bufferId already exists
 * @tc.type: FUNC
 */
HWTEST_F(RSSurfaceLayerTest, SetOriginalBufferOwnerCount_DuplicateBufferNoAddRef, Function | SmallTest | Level2)
{
    auto lyr = std::make_shared<RSSurfaceLayer>(0, nullptr);
    ASSERT_NE(lyr, nullptr);
 
    auto boc = std::make_shared<RSSurfaceHandler::BufferOwnerCount>();
    boc->bufferId_ = 201u;
    lyr->SetOriginalBufferOwnerCount(boc);
 
    int refAfterFirst = boc->refCount_.load();
    lyr->SetOriginalBufferOwnerCount(boc);
 
    EXPECT_EQ(boc->refCount_.load(), refAfterFirst);
}
 
/**
 * @tc.name: SetOriginalBufferOwnerCount_NullptrIgnored
 * @tc.desc: Verify SetOriginalBufferOwnerCount ignores nullptr input
 * @tc.type: FUNC
 */
HWTEST_F(RSSurfaceLayerTest, SetOriginalBufferOwnerCount_NullptrIgnored, Function | SmallTest | Level2)
{
    auto lyr = std::make_shared<RSSurfaceLayer>(0, nullptr);
    ASSERT_NE(lyr, nullptr);
 
    lyr->SetOriginalBufferOwnerCount(nullptr);
    EXPECT_EQ(lyr->GetOriginalBufferOwnerCount(), nullptr);
}
} // namespace Rosen
} // namespace OHOS
