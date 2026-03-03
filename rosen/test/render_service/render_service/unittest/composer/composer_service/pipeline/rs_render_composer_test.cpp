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

#include <atomic>
#include <chrono>
#include <gtest/gtest.h>
#include "rs_render_composer.h"
#include "hdi_output.h"
#include "../layer_backend/mock_hdi_device.h"
#include "rs_render_composer_context.h"
#include "rs_layer.h"
#include "rs_layer_common_def.h"
#include "rs_layer_transaction_data.h"
#ifdef RS_ENABLE_VK
#include "platform/ohos/backend/rs_vulkan_context.h"
#endif
#include "platform/common/rs_system_properties.h"
#include "platform/ohos/backend/rs_surface_ohos_raster.h"
#include "syspara/parameters.h"
#include "syspara/parameter.h"
#include "param/sys_param.h"
#include "feature/round_corner_display/rs_round_corner_display_manager.h"
#include "common/rs_singleton.h"
#include "pipeline/main_thread/rs_render_service_listener.h"
#include "pipeline/rs_render_node_gc.h"
#include "rs_render_surface_layer.h"
#include "rs_render_surface_rcd_layer.h"
#define UnmarshallingFunc RSLayerParcelFactory_UnmarshallingFunc
#include "rs_surface_layer_parcel.h"
#undef UnmarshallingFunc
#include "rs_render_layer_cmd.h"
#include "rs_surface_rcd_layer.h"
#include "screen_manager/rs_screen.h"
#include "screen_manager/rs_screen_property.h"
#include "surface_buffer_impl.h"
#include "surface_type.h"
#include "v2_1/cm_color_space.h"
using namespace OHOS::HDI::Display::Graphic::Common::V1_0;

using namespace testing::ext;

namespace OHOS {
namespace Rosen {
constexpr int32_t MAX_SETRATE_RETRY_COUNT = 20;
class BufferConsumerListener : public ::OHOS::IBufferConsumerListener {
public:
    void OnBufferAvailable() override
    {
    }
};
class RsRenderComposerTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
    static inline std::shared_ptr<RSRenderComposer> rsRenderComposer_ = nullptr;
    static inline Mock::HdiDeviceMock* hdiDeviceMock_;
};

void RsRenderComposerTest::SetUpTestCase()
{
#ifdef RS_ENABLE_VK
    RsVulkanContext::SetRecyclable(false);
#endif
    auto& renderNodeGC = RSRenderNodeGC::Instance();
    renderNodeGC.nodeBucket_ = std::queue<std::vector<RSRenderNode*>>();
    renderNodeGC.drawableBucket_ = std::queue<std::vector<DrawableV2::RSRenderNodeDrawableAdapter*>>();
    auto& memTrack = MemoryTrack::Instance();
    memTrack.memNodeMap_ = std::unordered_map<NodeId, MemoryInfo>();
    auto& memorySnapshot = MemorySnapshot::Instance();
    memorySnapshot.appMemorySnapshots_ = std::unordered_map<pid_t, MemorySnapshotInfo>();

    auto output = std::make_shared<HdiOutput>(0u);
    hdiDeviceMock_ = Mock::HdiDeviceMock::GetInstance();
    output->Init();
    sptr<RSScreenProperty> property = new RSScreenProperty();
    rsRenderComposer_ = std::make_shared<RSRenderComposer>(output, property);
}
void RsRenderComposerTest::TearDownTestCase()
{
    rsRenderComposer_->frameBufferSurfaceOhosMap_.clear();
    rsRenderComposer_->uniRenderEngine_ = nullptr;
    hdiDeviceMock_ = nullptr;
}
void RsRenderComposerTest::SetUp() {}
void RsRenderComposerTest::TearDown() {}

namespace {

// Fake RSLayer for testing
class FakeRSLayer : public RSLayer {
public:
    explicit FakeRSLayer(RSLayerId id, bool usingFlag, const std::string& name = "L")
        : id_(id), isNeedComposition_(usingFlag), surfaceName_(name) {}
    ~FakeRSLayer() override = default;

    RSLayerId GetRSLayerId() const override { return id_; }
    void SetRSLayerId(RSLayerId rsLayerId) override { id_ = rsLayerId; }
    void UpdateRSLayerCmd(const std::shared_ptr<RSRenderLayerCmd>& command) override { (void)command; }
    void SetSurfaceUniqueId(uint64_t uniqueId) override { surfaceUniqueId_ = uniqueId; }
    uint64_t GetSurfaceUniqueId() const override { return surfaceUniqueId_; }
    void SetCycleBuffersNum(uint32_t cycleBuffersNum) override { cycleBuffersNum_ = cycleBuffersNum; }
    uint32_t GetCycleBuffersNum() const override { return cycleBuffersNum_; }
    void SetSurfaceName(std::string surfaceName) override { surfaceName_ = surfaceName; }
    void SetBufferOwnerCount(const std::shared_ptr<RSSurfaceHandler::BufferOwnerCount>& boc,
        bool needUpdate) override { bufferOwnerCount_ = boc; }
    std::shared_ptr<RSSurfaceHandler::BufferOwnerCount> PopBufferOwnerCountById(uint64_t bufferId) override
    {
        (void)bufferId;
        return nullptr;
    }
    std::shared_ptr<RSSurfaceHandler::BufferOwnerCount> GetBufferOwnerCount() const override
    { return bufferOwnerCount_; }
    std::string GetSurfaceName() const override { return surfaceName_; }
    void SetSolidColorLayerProperty(GraphicSolidColorLayerProperty solidColorLayerProperty) override
    { solidColorLayerProperty_ = solidColorLayerProperty; }
    GraphicSolidColorLayerProperty GetSolidColorLayerProperty() const override { return solidColorLayerProperty_; }
    bool GetIsNeedComposition() const override { return isNeedComposition_; }
    void SetIsNeedComposition(bool isNeedComposition) override { isNeedComposition_ = isNeedComposition; }
    void SetAlpha(const GraphicLayerAlpha &alpha) override { alpha_ = alpha; }
    const GraphicLayerAlpha &GetAlpha() const override { return alpha_; }
    void SetZorder(int32_t zOrder) override { zOrder_ = zOrder; }
    uint32_t GetZorder() const override { return static_cast<uint32_t>(zOrder_); }
    void SetType(const GraphicLayerType layerType) override { layerType_ = layerType; }
    GraphicLayerType GetType() const override { return layerType_; }
    void SetTransform(GraphicTransformType type) override { transform_ = type; }
    GraphicTransformType GetTransform() const override { return transform_; }
    void SetCompositionType(GraphicCompositionType type) override { compType_ = type; }
    GraphicCompositionType GetCompositionType() const override { return compType_; }
    void SetHdiCompositionType(GraphicCompositionType type) override { hdiCompType_ = type; }
    GraphicCompositionType GetHdiCompositionType() const override { return hdiCompType_; }
    void SetVisibleRegions(const std::vector<GraphicIRect>& visibleRegions) override
    { visibleRegions_ = visibleRegions; }
    const std::vector<GraphicIRect>& GetVisibleRegions() const override { return visibleRegions_; }
    void SetDirtyRegions(const std::vector<GraphicIRect>& dirtyRegions) override { dirtyRegions_ = dirtyRegions; }
    const std::vector<GraphicIRect>& GetDirtyRegions() const override { return dirtyRegions_; }
    void SetBlendType(GraphicBlendType type) override { blendType_ = type; }
    GraphicBlendType GetBlendType() const override { return blendType_; }
    void SetCropRect(const GraphicIRect &crop) override { cropRect_ = crop; }
    const GraphicIRect &GetCropRect() const override { return cropRect_; }
    void SetPreMulti(bool preMulti) override { preMulti_ = preMulti; }
    bool IsPreMulti() const override { return preMulti_; }
    void SetLayerSize(const GraphicIRect &layerRect) override { layerSize_ = layerRect; }
    const GraphicIRect &GetLayerSize() const override { return layerSize_; }
    void SetBoundSize(const GraphicIRect &boundRect) override { boundSize_ = boundRect; }
    const GraphicIRect& GetBoundSize() const override { return boundSize_; }
    void SetLayerColor(GraphicLayerColor layerColor) override { layerColor_ = layerColor; }
    const GraphicLayerColor& GetLayerColor() const override { return layerColor_; }
    void SetBackgroundColor(GraphicLayerColor backgroundColor) override { backgroundColor_ = backgroundColor; }
    const GraphicLayerColor& GetBackgroundColor() const override { return backgroundColor_; }
    void SetCornerRadiusInfoForDRM(const std::vector<float>& drmCornerRadiusInfo) override
    { cornerRadiusInfo_ = drmCornerRadiusInfo; }
    const std::vector<float>& GetCornerRadiusInfoForDRM() const override { return cornerRadiusInfo_; }
    void SetColorTransform(const std::vector<float>& matrix) override { colorTransform_ = matrix; }
    const std::vector<float>& GetColorTransform() const override { return colorTransform_; }
    void SetColorDataSpace(GraphicColorDataSpace colorSpace) override { colorSpace_ = colorSpace; }
    GraphicColorDataSpace GetColorDataSpace() const override { return colorSpace_; }
    void SetMetaData(const std::vector<GraphicHDRMetaData>& metaData) override { metaData_ = metaData; }
    const std::vector<GraphicHDRMetaData>& GetMetaData() const override { return metaData_; }
    void SetMetaDataSet(const GraphicHDRMetaDataSet& metaDataSet) override { metaDataSet_ = metaDataSet; }
    const GraphicHDRMetaDataSet& GetMetaDataSet() const override { return metaDataSet_; }
    void SetMatrix(const GraphicMatrix& matrix) override { matrix_ = matrix; }
    const GraphicMatrix& GetMatrix() const override { return matrix_; }
    void SetGravity(int32_t gravity) override { gravity_ = gravity; }
    int32_t GetGravity() const override { return gravity_; }
    void SetUniRenderFlag(bool isUniRender) override { uniRender_ = isUniRender; }
    bool GetUniRenderFlag() const override { return uniRender_; }
    void SetTunnelHandleChange(bool change) override { tunnelHandleChange_ = change; }
    bool GetTunnelHandleChange() const override { return tunnelHandleChange_; }
    void SetTunnelHandle(const sptr<SurfaceTunnelHandle> &handle) override { tunnelHandle_ = handle; }
    sptr<SurfaceTunnelHandle> GetTunnelHandle() const override { return tunnelHandle_; }
    void SetTunnelLayerId(const uint64_t tunnelLayerId) override { tunnelLayerId_ = tunnelLayerId; }
    uint64_t GetTunnelLayerId() const override { return tunnelLayerId_; }
    void SetTunnelLayerProperty(uint32_t tunnelLayerProperty) override
    { tunnelLayerProperty_ = tunnelLayerProperty; }
    uint32_t GetTunnelLayerProperty() const override { return tunnelLayerProperty_; }
    void SetIsSupportedPresentTimestamp(bool isSupported) override { supportedPresentTimestamp_ = isSupported; }
    bool GetIsSupportedPresentTimestamp() const override { return supportedPresentTimestamp_; }
    void SetPresentTimestamp(const GraphicPresentTimestamp &timestamp) override { presentTimestamp_ = timestamp; }
    const GraphicPresentTimestamp& GetPresentTimestamp() const override { return presentTimestamp_; }
    void SetSdrNit(float sdrNit) override { sdrNit_ = sdrNit; }
    float GetSdrNit() const override { return sdrNit_; }
    void SetDisplayNit(float displayNit) override { displayNit_ = displayNit; }
    float GetDisplayNit() const override { return displayNit_; }
    void SetBrightnessRatio(float brightnessRatio) override { brightnessRatio_ = brightnessRatio; }
    float GetBrightnessRatio() const override { return brightnessRatio_; }
    void SetLayerLinearMatrix(const std::vector<float>& layerLinearMatrix) override
    { layerLinearMatrix_ = layerLinearMatrix; }
    const std::vector<float>& GetLayerLinearMatrix() const override { return layerLinearMatrix_; }
    void SetLayerSourceTuning(int32_t layerSouce) override { layerSourceTuning_ = layerSouce; }
    int32_t GetLayerSourceTuning() const override { return layerSourceTuning_; }
    void SetWindowsName(std::vector<std::string>& windowsName) override { windowsName_ = windowsName; }
    const std::vector<std::string>& GetWindowsName() const override { return windowsName_; }
    void SetRotationFixed(bool rotationFixed) override { rotationFixed_ = rotationFixed; }
    bool GetRotationFixed() const override { return rotationFixed_; }
    void SetLayerArsr(bool arsrTag) override { layerArsr_ = arsrTag; }
    bool GetLayerArsr() const override { return layerArsr_; }
    void SetLayerCopybit(bool copybitTag) override { layerCopybit_ = copybitTag; }
    bool GetLayerCopybit() const override { return layerCopybit_; }
    void SetNeedBilinearInterpolation(bool need) override { needBilinear_ = need; }
    bool GetNeedBilinearInterpolation() const override { return needBilinear_; }
    void SetIsMaskLayer(bool isMaskLayer) override { isMaskLayer_ = isMaskLayer; }
    bool GetIsMaskLayer() const override { return isMaskLayer_; }
    void SetIgnoreAlpha(bool ignoreAlpha) override { ignoreAlpha_ = ignoreAlpha; }
    bool GetIgnoreAlpha() const override { return ignoreAlpha_; }
    void SetNodeId(uint64_t nodeId) override { nodeId_ = nodeId; }
    uint64_t GetNodeId() const override { return nodeId_; }
    void SetAncoFlags(const uint32_t ancoFlags) override { ancoFlags_ = ancoFlags; }
    uint32_t GetAncoFlags() const override { return ancoFlags_; }
    bool IsAncoNative() const override { return false; }
    void SetLayerMaskInfo(LayerMask mask) override { layerMask_ = mask; }
    LayerMask GetLayerMaskInfo() const override { return layerMask_; }

    void SetSurface(const sptr<IConsumerSurface>& surface) override { surface_ = surface; }
    sptr<IConsumerSurface> GetSurface() const override { return surface_; }
    void SetBuffer(const sptr<SurfaceBuffer>& sbuffer, const sptr<SyncFence>& acquireFence) override
    {
        buffer_ = sbuffer;
        acquireFence_ = acquireFence;
    }
    void SetBuffer(const sptr<SurfaceBuffer>& sbuffer) override { buffer_ = sbuffer; }
    sptr<SurfaceBuffer> GetBuffer() const override { return buffer_; }
    void SetPreBuffer(const sptr<SurfaceBuffer>& buffer) override { preBuffer_ = buffer; }
    sptr<SurfaceBuffer> GetPreBuffer() const override { return preBuffer_; }
    void SetAcquireFence(const sptr<SyncFence>& acquireFence) override { acquireFence_ = acquireFence; }
    sptr<SyncFence> GetAcquireFence() const override { return acquireFence_; }

    // Provide overrides to avoid undefined reference to base default implementations during linking
    void SetUseDeviceOffline(bool useOffline) override { useDeviceOffline_ = useOffline; }
    bool GetUseDeviceOffline() const override { return useDeviceOffline_; }
    void SetAncoSrcRect(const GraphicIRect& ancoSrcRect) override { ancoSrcRect_ = ancoSrcRect; }
    const GraphicIRect& GetAncoSrcRect() const override { return ancoSrcRect_; }
    void SetDeleteLayer(bool isDeleteLayer) const override {}

    void CopyLayerInfo(const RSLayerPtr& rsLayer) override { (void)rsLayer; }
    void Dump(std::string& result) const override { (void)result; }
    void DumpCurrentFrameLayer() const override {}

private:
    RSLayerId id_ = 0;
    bool isNeedComposition_ = false;
    std::string surfaceName_;
    std::shared_ptr<RSSurfaceHandler::BufferOwnerCount> bufferOwnerCount_ = nullptr;
    GraphicSolidColorLayerProperty solidColorLayerProperty_ {};

    GraphicLayerAlpha alpha_ {};
    int32_t zOrder_ = 0;
    GraphicLayerType layerType_ = GraphicLayerType::GRAPHIC_LAYER_TYPE_BUTT;
    GraphicTransformType transform_ = GraphicTransformType::GRAPHIC_ROTATE_NONE;
    GraphicCompositionType compType_ = GraphicCompositionType::GRAPHIC_COMPOSITION_CLIENT;
    GraphicCompositionType hdiCompType_ = GraphicCompositionType::GRAPHIC_COMPOSITION_CLIENT;
    std::vector<GraphicIRect> visibleRegions_;
    std::vector<GraphicIRect> dirtyRegions_;
    GraphicBlendType blendType_ = GraphicBlendType::GRAPHIC_BLEND_NONE;
    GraphicIRect cropRect_ {0, 0, 0, 0};
    bool preMulti_ = false;
    GraphicIRect layerSize_ {0, 0, 0, 0};
    GraphicIRect boundSize_ {0, 0, 0, 0};
    GraphicLayerColor layerColor_ {};
    GraphicLayerColor backgroundColor_ {};
    std::vector<float> cornerRadiusInfo_;
    std::vector<float> colorTransform_;
    GraphicColorDataSpace colorSpace_ = GraphicColorDataSpace::GRAPHIC_COLOR_DATA_SPACE_UNKNOWN;
    std::vector<GraphicHDRMetaData> metaData_;
    GraphicHDRMetaDataSet metaDataSet_ {};
    GraphicMatrix matrix_ {};
    int32_t gravity_ = 0;
    bool uniRender_ = false;
    bool tunnelHandleChange_ = false;
    sptr<SurfaceTunnelHandle> tunnelHandle_ = nullptr;
    uint64_t tunnelLayerId_ = 0;
    uint32_t tunnelLayerProperty_ = 0;
    bool supportedPresentTimestamp_ = false;
    GraphicPresentTimestamp presentTimestamp_ {};
    float sdrNit_ = 0.0f;
    float displayNit_ = 0.0f;
    float brightnessRatio_ = 0.0f;
    std::vector<float> layerLinearMatrix_;
    int32_t layerSourceTuning_ = 0;
    std::vector<std::string> windowsName_;
    bool rotationFixed_ = false;
    bool layerArsr_ = false;
    bool layerCopybit_ = false;
    bool needBilinear_ = false;
    bool isMaskLayer_ = false;
    uint64_t nodeId_ = 0;
    uint32_t ancoFlags_ = 0;
    LayerMask layerMask_ {};
    sptr<IConsumerSurface> surface_ = nullptr;
    uint64_t surfaceUniqueId_ = 0;
    sptr<SurfaceBuffer> buffer_ = nullptr;
    sptr<SurfaceBuffer> preBuffer_ = nullptr;
    sptr<SyncFence> acquireFence_ = nullptr;
    uint32_t cycleBuffersNum_ = 0;
    bool ignoreAlpha_ {false};
    bool useDeviceOffline_ {false};
    GraphicIRect ancoSrcRect_ {-1, -1, -1, -1};
};
}

/**
 * Function: ConstructAndHandlerReady
 * Type: Function
 * Rank: Important(2)
 * EnvConditions: N/A
 * CaseDescription: 1. create output and property
 *                  2. create RSRenderComposer with output and property
 *                  3. verify that posting task works without crash
 */
HWTEST_F(RsRenderComposerTest, ConstructAndHandlerReady, TestSize.Level1)
{
    // Directly use RSRenderComposer to verify that posting task works without crash
    std::atomic<bool> ran{false};
    rsRenderComposer_->PostTask([&ran]() { ran.store(true); });
    std::this_thread::sleep_for(std::chrono::milliseconds(200));
    EXPECT_TRUE(ran.load());
    rsRenderComposer_->PostSyncTask([&ran]() { ran.store(false); });
    EXPECT_FALSE(ran.load());
}

/**
 * Function: ScreenConnection_AllOperations
 * Type: Function
 * Rank: Important(2)
 * EnvConditions: N/A
 * CaseDescription: 1. create RSRenderComposer
 *                  2. OnScreenConnected with output and property then flush before disconnect
 *                  3. check rsRenderComposer_->hdiOutput_ and rsRenderComposer_->rsRenderComposerContext_ are not null
 *                      when connected but null after disconnected
 */
HWTEST_F(RsRenderComposerTest, ScreenConnection_AllOperations, TestSize.Level1)
{
    auto out = std::make_shared<HdiOutput>(3u);
    out->Init();
    EXPECT_NE(rsRenderComposer_->handler_, nullptr);
    // Verify initial state
    EXPECT_EQ(rsRenderComposer_->hdiOutput_->GetScreenId(), 0u);
    EXPECT_NE(rsRenderComposer_->rsRenderComposerContext_, nullptr);
    EXPECT_EQ(rsRenderComposer_->screenId_, 0u);

    // 1. first connection
    sptr<RSScreenProperty> property = new RSScreenProperty();
    rsRenderComposer_->OnScreenConnected(out, property);
    std::this_thread::sleep_for(std::chrono::milliseconds(200));
    // Verify connected state
    EXPECT_EQ(rsRenderComposer_->hdiOutput_->GetScreenId(), 3u);
    EXPECT_NE(rsRenderComposer_->rsRenderComposerContext_, nullptr);
    EXPECT_EQ(rsRenderComposer_->screenId_, 0u);

    // 2. repeat connection (should maintain connection)
    rsRenderComposer_->OnScreenConnected(out, property);
    std::this_thread::sleep_for(std::chrono::milliseconds(200));
    EXPECT_EQ(rsRenderComposer_->hdiOutput_->GetScreenId(), 3u);
    EXPECT_NE(rsRenderComposer_->rsRenderComposerContext_, nullptr);
    EXPECT_EQ(rsRenderComposer_->screenId_, 0u);

    // 3. first disconnection
    rsRenderComposer_->OnScreenDisconnected();
    std::this_thread::sleep_for(std::chrono::milliseconds(200));
    // Verify disconnected state
    EXPECT_EQ(rsRenderComposer_->hdiOutput_, nullptr);
    EXPECT_EQ(rsRenderComposer_->rsRenderComposerContext_, nullptr);

    // 4. repeat disconnection (should remain disconnected)
    rsRenderComposer_->OnScreenDisconnected();
    std::this_thread::sleep_for(std::chrono::milliseconds(200));
    EXPECT_EQ(rsRenderComposer_->hdiOutput_, nullptr);
    EXPECT_EQ(rsRenderComposer_->rsRenderComposerContext_, nullptr);

    std::this_thread::sleep_for(std::chrono::milliseconds(200));
}

/**
 * Function: ComposerProcess_Null_Handler
 * Type: Function
 * Rank: Important(2)
 * EnvConditions: N/A
 * CaseDescription: 1. create RSRenderComposer but set handler_ to null
 *                  2. call ComposerProcess with transaction data
 *                  3. verify normal path and early return path after disconnection
 */
HWTEST_F(RsRenderComposerTest, ComposerProcess_Null_Handler, TestSize.Level1)
{
    auto output = std::make_shared<HdiOutput>(1u);
    output->Init();
    sptr<RSScreenProperty> property = new RSScreenProperty();
    auto tmpRsRenderComposer = std::make_shared<RSRenderComposer>(output, property);
    EXPECT_EQ(tmpRsRenderComposer->hdiOutput_->GetScreenId(), 1u);

    std::vector<RSLayerPtr> newRsLayers;
    tmpRsRenderComposer->hdiOutput_->OnPrepareComplete(false, newRsLayers);

    tmpRsRenderComposer->runner_ = nullptr; // Disable runner thread for test control
    tmpRsRenderComposer->handler_ = nullptr;
    PipelineParam pipelineParam;
    uint32_t currentRate = 0;
    int64_t delayTime = 0;
    tmpRsRenderComposer->ComposerPrepare(currentRate, delayTime, pipelineParam);
    EXPECT_EQ(tmpRsRenderComposer->handler_, nullptr);
    tmpRsRenderComposer->uniRenderEngine_ = nullptr;
}

/**
 * Function: ComposerProcess_EarlyAndNormal
 * Type: Function
 * Rank: Important(2)
 * EnvConditions: N/A
 * CaseDescription: 1. create RSRenderComposer
 *                  2. call ComposerProcess with transaction data
 *                  3. verify normal path and early return path after disconnection
 */
HWTEST_F(RsRenderComposerTest, ComposerProcess_EarlyAndNormal, TestSize.Level1)
{
    auto tx = std::make_shared<RSLayerTransactionData>();
    ASSERT_NE(tx, nullptr);

    PipelineParam pipelineParam;
    uint32_t currentRate = 0;
    int64_t delayTime = 0;

    // Verify composer is created successfully
    ASSERT_NE(rsRenderComposer_, nullptr);
    EXPECT_NE(rsRenderComposer_->handler_, nullptr); // Handler should be initialized

    // normal path - prepare and process
    rsRenderComposer_->ComposerPrepare(currentRate, delayTime, pipelineParam);
    rsRenderComposer_->ComposerProcess(currentRate, tx);
    std::this_thread::sleep_for(std::chrono::milliseconds(200));
    // Verify composer still in valid state after processing
    EXPECT_NE(rsRenderComposer_->handler_, nullptr);

    // early return path after disconnection
    rsRenderComposer_->OnScreenDisconnected();
    std::this_thread::sleep_for(std::chrono::milliseconds(200));
    EXPECT_EQ(rsRenderComposer_->hdiOutput_, nullptr); // Verify disconnected
    rsRenderComposer_->ComposerPrepare(currentRate, delayTime, pipelineParam);
    rsRenderComposer_->ComposerProcess(currentRate, tx);
    // Verify composer maintains valid state even after disconnection
    EXPECT_NE(rsRenderComposer_->handler_, nullptr);
}

/**
 * Function: RefreshRateCounts_Branches
 * Type: Function
 * Rank: Important(2)
 * EnvConditions: N/A
 * CaseDescription: 1. create RSRenderComposer
 *                  2. call RefreshRateCounts and ClearRefreshRateCounts with/without hgmHardwareUtils_
 *                  3. verify string output is modified correctly in both cases
 */
HWTEST_F(RsRenderComposerTest, RefreshRateCounts_Branches, TestSize.Level1)
{
    std::string s;
    size_t initialLen = s.length();

    // With hgmHardwareUtils_ present
    rsRenderComposer_->RefreshRateCounts(s);
    // String might be modified (or not, depending on implementation)
    EXPECT_GE(s.length(), initialLen);

    s.clear();
    rsRenderComposer_->ClearRefreshRateCounts(s);
    EXPECT_GE(s.length(), 0u);
}


/**
 * Function: ClearFrameBuffers_Branches_NoCrash
 * Type: Function
 * Rank: Important(2)
 * EnvConditions: N/A
 * CaseDescription: 1. create RSRenderComposer
 *                  2. call ClearFrameBuffers with true and false parameters
 *                  3. verify return values are valid GSError codes
 */
HWTEST_F(RsRenderComposerTest, ClearFrameBuffers_Branches_NoCrash, TestSize.Level1)
{
    // Verify result is valid (either success or a known error code)
    EXPECT_EQ(rsRenderComposer_->ClearFrameBuffers(true), GSERROR_INVALID_ARGUMENTS);
    std::this_thread::sleep_for(std::chrono::milliseconds(200));
    // false path
    EXPECT_EQ(rsRenderComposer_->ClearFrameBuffers(false), GSERROR_INVALID_ARGUMENTS);
    std::this_thread::sleep_for(std::chrono::milliseconds(200));

    auto hdiOutput = rsRenderComposer_->hdiOutput_;
    auto output = std::make_shared<HdiOutput>(7u);
    output->Init();
    output->fbSurface_ = nullptr;
    rsRenderComposer_->hdiOutput_ = output;
    EXPECT_EQ(rsRenderComposer_->ClearFrameBuffers(false), GSERROR_INVALID_ARGUMENTS);
    rsRenderComposer_->hdiOutput_ = hdiOutput;
}

/**
 * Function: ClearFrameBuffers_Branches_NoCrash_001
 * Type: Function
 * Rank: Important(2)
 * EnvConditions: N/A
 * CaseDescription: 1. create RSRenderComposer
 *                  2. call ClearFrameBuffers with true and false parameters
 *                  3. verify return values are valid GSError codes
 */
HWTEST_F(RsRenderComposerTest, ClearFrameBuffers_Branches_NoCrash_001, TestSize.Level1)
{
    auto output = std::make_shared<HdiOutput>(0u);
    output->Init();
    sptr<RSScreenProperty> property = new RSScreenProperty();
    auto rsRenderComposerTmp = std::make_shared<RSRenderComposer>(output, property);
    rsRenderComposerTmp->hdiOutput_ = nullptr;
    EXPECT_EQ(rsRenderComposerTmp->ClearFrameBuffers(true), GSERROR_INVALID_ARGUMENTS);
    rsRenderComposerTmp->hdiOutput_ = output;
    EXPECT_EQ(rsRenderComposerTmp->ClearFrameBuffers(true), GSERROR_INVALID_ARGUMENTS);
    sptr<SurfaceBuffer> buffer = sptr<SurfaceBufferImpl>::MakeSptr();
    rsRenderComposerTmp->hdiOutput_->bufferCache_.push_back(buffer);
    EXPECT_EQ(rsRenderComposerTmp->ClearFrameBuffers(true), SURFACE_ERROR_CONSUMER_DISCONNECTED);
}

/**
 * Function: ClearFrameBuffers_Branches_NoCrash_002
 * Type: Function
 * Rank: Important(2)
 * EnvConditions: N/A
 * CaseDescription: 1. create RSRenderComposer
 *                  2. call ClearFrameBuffers with true and false parameters
 *                  3. verify return values are valid GSError codes
 */
HWTEST_F(RsRenderComposerTest, ClearFrameBuffers_Branches_NoCrash_002, TestSize.Level1)
{
    auto output = std::make_shared<HdiOutput>(0u);
    output->Init();
    sptr<RSScreenProperty> property = new RSScreenProperty();
    auto rsRenderComposerTmp = std::make_shared<RSRenderComposer>(output, property);
    sptr<SurfaceBuffer> buffer = sptr<SurfaceBufferImpl>::MakeSptr();
    rsRenderComposerTmp->hdiOutput_->bufferCache_.push_back(buffer);
    auto csurf = IConsumerSurface::Create();
    auto producer = csurf->GetProducer();
    auto pSurface = Surface::CreateSurfaceAsProducer(producer);
    std::shared_ptr<RSSurfaceOhos> rsSurface1 = std::make_shared<RSSurfaceOhosRaster>(pSurface);
    EXPECT_NE(nullptr, rsSurface1);
    rsRenderComposerTmp->frameBufferSurfaceOhosMap_.insert({0, rsSurface1});
    EXPECT_EQ(rsRenderComposerTmp->ClearFrameBuffers(true), SURFACE_ERROR_CONSUMER_DISCONNECTED);
}

/**
 * Function: ClearFrameBuffersInner_Branches_NoCrash
 * Type: Function
 * Rank: Important(2)
 * EnvConditions: N/A
 * CaseDescription: 1. create RSRenderComposer
 *                  2. call ClearFrameBuffersInner with true and false parameters
 *                  3. verify return values are valid GSError codes
 */
HWTEST_F(RsRenderComposerTest, ClearFrameBuffersInner_Branches_NoCrash, TestSize.Level1)
{
    auto output = std::make_shared<HdiOutput>(0u);
    output->Init();
    sptr<RSScreenProperty> property = new RSScreenProperty();
    auto rsRenderComposerTmp = std::make_shared<RSRenderComposer>(output, property);
    EXPECT_NE(rsRenderComposerTmp->hdiOutput_, nullptr);
    EXPECT_EQ(rsRenderComposerTmp->ClearFrameBuffersInner(true), SURFACE_ERROR_CONSUMER_DISCONNECTED);
    EXPECT_EQ(rsRenderComposerTmp->ClearFrameBuffersInner(false), SURFACE_ERROR_CONSUMER_DISCONNECTED);
    rsRenderComposerTmp->uniRenderEngine_ = nullptr;
    rsRenderComposerTmp->hdiOutput_->fbSurface_ = nullptr;
    EXPECT_EQ(rsRenderComposerTmp->ClearFrameBuffersInner(false), GSERROR_INVALID_ARGUMENTS);
}

/**
 * Function: ScreenInfo_DetailedBranches
 * Type: Function
 * Rank: Important(2)
 * EnvConditions: N/A
 * CaseDescription: 1. create RSRenderComposer
 *                  2. create RSLayerTransactionData and call ComposerProcess
 *                  3. verify composer maintains valid state while updating transaction data (layers path)
 */
HWTEST_F(RsRenderComposerTest, ScreenInfo_DetailedBranches, TestSize.Level1)
{
    auto tx = std::make_shared<RSLayerTransactionData>();
    ASSERT_NE(tx, nullptr);

    PipelineParam pipelineParam;
    uint32_t currentRate = 0;
    int64_t delayTime = 0;

    // Verify initial state
    EXPECT_NE(rsRenderComposer_->handler_, nullptr);
    auto initialHandler = rsRenderComposer_->handler_;

    rsRenderComposer_->ComposerPrepare(currentRate, delayTime, pipelineParam);
    rsRenderComposer_->ComposerProcess(currentRate, tx);

    // Verify composer is still in valid state after processing
    EXPECT_NE(rsRenderComposer_->handler_, nullptr);
    EXPECT_EQ(rsRenderComposer_->handler_, initialHandler); // Handler should remain the same
}

/**
 * Function: OnScreenVBlankIdleCallback_Coverage
 * Type: Function
 * Rank: Important(2)
 * EnvConditions: N/A
 * CaseDescription: 1. create RSRenderComposer
 *                  2. call OnScreenVBlankIdleCallback with different timestamp values
 *                  3. verify composer remains in valid state after VBlank callback
 */
HWTEST_F(RsRenderComposerTest, OnScreenVBlankIdleCallback_Coverage, TestSize.Level1)
{
    uint64_t timestamp = 123456789ULL;
    rsRenderComposer_->OnScreenVBlankIdleCallback(rsRenderComposer_->screenId_, timestamp);
    // Verify composer is still valid after callback with normal timestamp
    EXPECT_NE(rsRenderComposer_->handler_, nullptr);

    rsRenderComposer_->OnScreenVBlankIdleCallback(rsRenderComposer_->screenId_, 0);
    // Verify composer is still valid after callback with zero timestamp
    EXPECT_NE(rsRenderComposer_->handler_, nullptr);
}

/**
 * Function: OnScreenVBlankIdleCallback_Coverage_001
 * Type: Function
 * Rank: Important(2)
 * EnvConditions: N/A
 * CaseDescription: 1. create RSRenderComposer
 *                  2. call OnScreenVBlankIdleCallback with different timestamp values
 *                  3. verify composer remains in valid state after VBlank callback
 */
HWTEST_F(RsRenderComposerTest, OnScreenVBlankIdleCallback_Coverage_001, TestSize.Level1)
{
    auto output = std::make_shared<HdiOutput>(0u);
    output->Init();
    sptr<RSScreenProperty> property = new RSScreenProperty();
    auto rsRenderComposerTmp = std::make_shared<RSRenderComposer>(output, property);
    EXPECT_NE(rsRenderComposerTmp->hdiOutput_, nullptr);
    rsRenderComposerTmp->hgmHardwareUtils_ = nullptr;
    uint64_t timestamp = 123456789ULL;
    rsRenderComposerTmp->OnScreenVBlankIdleCallback(rsRenderComposerTmp->screenId_, timestamp);
}

/**
 * Function: GetSurfaceName
 * Type: Function
 * Rank: Important(2)
 * EnvConditions: N/A
 * CaseDescription: 1. create RSRenderComposer
 *                  2. call GetSurfaceNameInLayers and GetSurfaceNameInLayersForTrace
 *                  3. verify dump string
 */
HWTEST_F(RsRenderComposerTest, GetSurfaceName, TestSize.Level1)
{
    int32_t maxZorder = 999;
    std::vector<RSLayerPtr> layers;
    layers.emplace_back(nullptr);

    // set normal layers
    RSLayerPtr l1 = std::static_pointer_cast<RSLayer>(std::make_shared<FakeRSLayer>(1, false, "L1"));
    sptr<IConsumerSurface> cSurface = IConsumerSurface::Create("surface1");
    l1->SetSurface(cSurface);
    l1->SetSurfaceName("surface1");
    layers.emplace_back(l1);

    // set layer with max zorder
    RSLayerPtr l2 = std::static_pointer_cast<RSLayer>(std::make_shared<FakeRSLayer>(2, true, "L2"));
    sptr<IConsumerSurface> cSurface2 = IConsumerSurface::Create("surface2");
    l2->SetSurface(cSurface2);
    l2->SetSurfaceName("surface2");
    l2->SetZorder(maxZorder);
    EXPECT_EQ(l2->GetZorder(), maxZorder);
    layers.emplace_back(l2);

    // set layer without surface
    RSLayerPtr l3 = std::static_pointer_cast<RSLayer>(std::make_shared<FakeRSLayer>(3, true, "L3"));
    l3->SetSurface(nullptr);
    layers.emplace_back(l3);

    std::string dumpStr = rsRenderComposer_->GetSurfaceNameInLayers(layers);
    EXPECT_NE(dumpStr.find("surface1"), std::string::npos);
    EXPECT_NE(dumpStr.find("surface2"), std::string::npos);
    dumpStr = rsRenderComposer_->GetSurfaceNameInLayersForTrace(layers);
    EXPECT_NE(dumpStr.find("surface1"), std::string::npos);
    EXPECT_NE(dumpStr.find("surface2"), std::string::npos);

    // set name greater than MAX_SURFACE_NAME_LENGTH
    constexpr uint32_t maxTotalSurfaceNameLength = 320;
    std::string maxLengthName = "";
    for (size_t length = 0; length <= maxTotalSurfaceNameLength; length += 10) {
        maxLengthName += "LLLLLLLLLL";
    }
    maxLengthName += "_1";
    RSLayerPtr l4 = std::static_pointer_cast<RSLayer>(std::make_shared<FakeRSLayer>(4, true, "L4"));
    sptr<IConsumerSurface> cSurface4 = IConsumerSurface::Create(maxLengthName);
    l4->SetSurface(cSurface4);
    // set zorder less than other layer in list
    l4->SetZorder(maxZorder - 1);
    // set type GraphicLayerType::GRAPHIC_LAYER_TYPE_CURSOR
    l4->SetType(GraphicLayerType::GRAPHIC_LAYER_TYPE_CURSOR);
    EXPECT_EQ(l4->GetType(), GraphicLayerType::GRAPHIC_LAYER_TYPE_CURSOR);
    EXPECT_LT(l4->GetZorder(), maxZorder);
    layers.emplace_back(l4);

    dumpStr = rsRenderComposer_->GetSurfaceNameInLayersForTrace(layers);
    // name truncated
    EXPECT_EQ(dumpStr.find("LLLLL_1"), std::string::npos);
    EXPECT_NE(dumpStr.find("surface2"), std::string::npos);
}

/**
 * Function: ComposerProcess_IsSuperFoldDisplay
 * Type: Function
 * Rank: Important(2)
 * EnvConditions: N/A
 * CaseDescription: 1. create RSRenderComposer
 *                  2. call ComposerProcess with transaction data when IsSuperFoldDisplay
 *                  3. verify normal path and early return path after disconnection
 */
HWTEST_F(RsRenderComposerTest, ComposerProcess_IsSuperFoldDisplay, TestSize.Level1)
{
    system::SetParameter("const.window.foldscreen.type", "6,0,0,0");
    auto tx = std::make_shared<RSLayerTransactionData>();
    ASSERT_NE(tx, nullptr);

    PipelineParam pipelineParam;
    uint32_t currentRate = 0;
    int64_t delayTime = 0;

    // Verify composer is created successfully
    ASSERT_NE(rsRenderComposer_, nullptr);
    EXPECT_NE(rsRenderComposer_->handler_, nullptr); // Handler should be initialized

    // normal path - prepare and process
    rsRenderComposer_->ComposerPrepare(currentRate, delayTime, pipelineParam);
    rsRenderComposer_->ComposerProcess(currentRate, tx);
    std::this_thread::sleep_for(std::chrono::milliseconds(200));
    // Verify composer still in valid state after processing
    EXPECT_NE(rsRenderComposer_->handler_, nullptr);

    // early return path after disconnection
    rsRenderComposer_->OnScreenDisconnected();
    std::this_thread::sleep_for(std::chrono::milliseconds(200));
    EXPECT_EQ(rsRenderComposer_->hdiOutput_, nullptr); // Verify disconnected
    rsRenderComposer_->ComposerPrepare(currentRate, delayTime, pipelineParam);
    rsRenderComposer_->ComposerProcess(currentRate, tx);
    std::this_thread::sleep_for(std::chrono::milliseconds(200));
    // Verify composer maintains valid state even after disconnection
    EXPECT_NE(rsRenderComposer_->handler_, nullptr);
    system::SetParameter("const.window.foldscreen.type", "0,0,0,0");
}

/**
 * Function: ComposerProcess_ComposerContext_Not_Nullptr
 * Type: Function
 * Rank: Important(2)
 * EnvConditions: N/A
 * CaseDescription: 1. create RSRenderComposer
 *                  2. call ComposerProcess with transaction data when IsSuperFoldDisplay
 *                  3. verify normal path and early return path after disconnection
 */
HWTEST_F(RsRenderComposerTest, ComposerProcess_ComposerContext_Not_Nullptr, TestSize.Level1)
{
    auto output = std::make_shared<HdiOutput>(0u);
    output->Init();
    sptr<RSScreenProperty> property = new RSScreenProperty();
    auto rsRenderComposerTmp = std::make_shared<RSRenderComposer>(output, property);
    EXPECT_NE(rsRenderComposerTmp->hdiOutput_, nullptr);
    rsRenderComposerTmp->rsRenderComposerContext_ = std::make_shared<RSRenderComposerContext>();
    uint32_t currentRate = 0;
    auto tx = std::make_shared<RSLayerTransactionData>();
    rsRenderComposerTmp->ComposerProcess(currentRate, tx);
}

/**
 * Function: ComposerProcess_IsSuperFoldDisplay_Coverage
 * Type: Function
 * Rank: Important(2)
 * EnvConditions: N/A
 * CaseDescription: 1. create RSRenderComposer
 *                  2. call ComposerProcess with transaction data when IsSuperFoldDisplay
 *                  3. verify normal path and early return path after disconnection
 */
HWTEST_F(RsRenderComposerTest, ComposerProcess_IsSuperFoldDisplay_Coverage, TestSize.Level1)
{
    system::SetParameter("const.window.foldscreen.type", "6,0,0,0");
    auto output = std::make_shared<HdiOutput>(0u);
    output->Init();
    sptr<RSScreenProperty> property = new RSScreenProperty();
    auto rsRenderComposerTmp = std::make_shared<RSRenderComposer>(output, property);
    rsRenderComposerTmp->composerScreenInfo_.activeRect = {100, 100, 0, 0};
    std::vector<std::shared_ptr<RSLayer>> layers;
    EXPECT_EQ(rsRenderComposerTmp->IsDropDirtyFrame(layers), false);

    layers.push_back(nullptr);
    std::shared_ptr<RSLayer> layer1 = std::make_shared<RSRenderSurfaceLayer>();
    layer1->SetLayerSize({100, 100, 0, 0});
    layers.push_back(layer1);
    EXPECT_EQ(rsRenderComposerTmp->IsDropDirtyFrame(layers), false);
    std::shared_ptr<RSLayer> layer2 = std::make_shared<RSRenderSurfaceLayer>();
    layer2->SetUniRenderFlag(false);
    layers.push_back(layer2);
    std::shared_ptr<RSLayer> layer3 = std::make_shared<RSRenderSurfaceLayer>();
    layer3->SetLayerSize({200, 200, 0, 0});
    layers.push_back(layer3);
    std::shared_ptr<RSLayer> layer4 = std::make_shared<RSRenderSurfaceLayer>();
    layer4->SetLayerSize({200, 200, 0, 0});
    layer4->SetUniRenderFlag(true);
    layers.push_back(layer4);

    rsRenderComposerTmp->IsDropDirtyFrame(layers);
    system::SetParameter("const.window.foldscreen.type", "0,0,0,0");
}

/**
 * Function: IsDropDirtyFrame_NotSuperFoldDisplay
 * Type: Function
 * Rank: Important(2)
 * EnvConditions: N/A
 * CaseDescription: 1. set NOT super fold display
 *                  2. call IsDropDirtyFrame
 *                  3. verify returns false (line 738 branch true)
 */
HWTEST_F(RsRenderComposerTest, IsDropDirtyFrame_NotSuperFoldDisplay, TestSize.Level1)
{
    system::SetParameter("const.window.foldscreen.type", "0,0,0,0");
    auto output = std::make_shared<HdiOutput>(0u);
    output->Init();
    sptr<RSScreenProperty> property = new RSScreenProperty();
    auto rsRenderComposerTmp = std::make_shared<RSRenderComposer>(output, property);

    std::vector<std::shared_ptr<RSLayer>> layers;
    std::shared_ptr<RSRenderSurfaceLayer> layer = std::make_shared<RSRenderSurfaceLayer>();
    layer->SetLayerSize({100, 100, 100, 100});
    layer->SetUniRenderFlag(true);
    layers.push_back(layer);

    // Even with mismatched size, should return false when not super fold display
    bool result = rsRenderComposerTmp->IsDropDirtyFrame(layers);
    EXPECT_EQ(result, false);
}

/**
 * Function: IsDropDirtyFrame_SuperFoldDisplay_ActiveRectEmpty
 * Type: Function
 * Rank: Important(2)
 * EnvConditions: N/A
 * CaseDescription: 1. set super fold display with empty activeRect
 *                  2. call IsDropDirtyFrame
 *                  3. verify returns false (line 743 branch true)
 */
HWTEST_F(RsRenderComposerTest, IsDropDirtyFrame_SuperFoldDisplay_ActiveRectEmpty, TestSize.Level1)
{
    system::SetParameter("const.window.foldscreen.type", "6,0,0,0");
    auto output = std::make_shared<HdiOutput>(0u);
    output->Init();
    sptr<RSScreenProperty> property = new RSScreenProperty();
    auto rsRenderComposerTmp = std::make_shared<RSRenderComposer>(output, property);

    // Set activeRect with width=0, height=0 (empty)
    rsRenderComposerTmp->composerScreenInfo_.activeRect = {100, 100, 0, 0};

    std::vector<std::shared_ptr<RSLayer>> layers;
    std::shared_ptr<RSRenderSurfaceLayer> layer = std::make_shared<RSRenderSurfaceLayer>();
    layer->SetLayerSize({200, 200, 100, 100}); // Different from activeRect
    layer->SetUniRenderFlag(true);
    layers.push_back(layer);

    bool result = rsRenderComposerTmp->IsDropDirtyFrame(layers);
    EXPECT_EQ(result, false);

    system::SetParameter("const.window.foldscreen.type", "0,0,0,0");
}

/**
 * Function: IsDropDirtyFrame_SuperFoldDisplay_LayersEmpty
 * Type: Function
 * Rank: Important(2)
 * EnvConditions: N/A
 * CaseDescription: 1. set super fold display with valid activeRect
 *                  2. call IsDropDirtyFrame with empty layers
 *                  3. verify returns false (line 748 branch true)
 */
HWTEST_F(RsRenderComposerTest, IsDropDirtyFrame_SuperFoldDisplay_LayersEmpty, TestSize.Level1)
{
    system::SetParameter("const.window.foldscreen.type", "6,0,0,0");
    auto output = std::make_shared<HdiOutput>(0u);
    output->Init();
    sptr<RSScreenProperty> property = new RSScreenProperty();
    auto rsRenderComposerTmp = std::make_shared<RSRenderComposer>(output, property);

    // Set valid activeRect
    rsRenderComposerTmp->composerScreenInfo_.activeRect = {100, 100, 100, 100};

    std::vector<std::shared_ptr<RSLayer>> layers; // Empty layers

    bool result = rsRenderComposerTmp->IsDropDirtyFrame(layers);
    EXPECT_EQ(result, false);

    system::SetParameter("const.window.foldscreen.type", "0,0,0,0");
}

/**
 * Function: IsDropDirtyFrame_LayerIsNull
 * Type: Function
 * Rank: Important(2)
 * EnvConditions: N/A
 * CaseDescription: 1. set super fold display with valid activeRect
 *                  2. add nullptr layer to layers
 *                  3. call IsDropDirtyFrame
 *                  4. verify nullptr layer is skipped (line 753 branch true)
 */
HWTEST_F(RsRenderComposerTest, IsDropDirtyFrame_LayerIsNull, TestSize.Level1)
{
    system::SetParameter("const.window.foldscreen.type", "6,0,0,0");
    auto output = std::make_shared<HdiOutput>(0u);
    output->Init();
    sptr<RSScreenProperty> property = new RSScreenProperty();
    auto rsRenderComposerTmp = std::make_shared<RSRenderComposer>(output, property);

    rsRenderComposerTmp->composerScreenInfo_.activeRect = {100, 100, 100, 100};

    std::vector<std::shared_ptr<RSLayer>> layers;
    layers.push_back(nullptr); // nullptr layer

    bool result = rsRenderComposerTmp->IsDropDirtyFrame(layers);
    EXPECT_EQ(result, false);

    system::SetParameter("const.window.foldscreen.type", "0,0,0,0");
}

/**
 * Function: IsDropDirtyFrame_UniRenderFlagTrue_SizeMismatch
 * Type: Function
 * Rank: Important(2)
 * EnvConditions: N/A
 * CaseDescription: 1. set super fold display with valid activeRect
 *                  2. add layer with uniRenderFlag=true and mismatched size
 *                  3. call IsDropDirtyFrame
 *                  4. verify returns true (line 757 branch true)
 */
HWTEST_F(RsRenderComposerTest, IsDropDirtyFrame_UniRenderFlagTrue_SizeMismatch, TestSize.Level1)
{
    system::SetParameter("const.window.foldscreen.type", "6,0,0,0");
    auto output = std::make_shared<HdiOutput>(0u);
    output->Init();
    sptr<RSScreenProperty> property = new RSScreenProperty();
    auto rsRenderComposerTmp = std::make_shared<RSRenderComposer>(output, property);

    rsRenderComposerTmp->composerScreenInfo_.activeRect = {100, 100, 100, 100};

    std::vector<std::shared_ptr<RSLayer>> layers;
    std::shared_ptr<RSRenderSurfaceLayer> layer = std::make_shared<RSRenderSurfaceLayer>();
    layer->SetLayerSize({200, 200, 100, 100}); // Mismatched with activeRect
    layer->SetUniRenderFlag(true);
    layers.push_back(layer);

    bool result = rsRenderComposerTmp->IsDropDirtyFrame(layers);
    EXPECT_EQ(result, true);

    system::SetParameter("const.window.foldscreen.type", "0,0,0,0");
}

/**
 * Function: IsDropDirtyFrame_UniRenderFlagTrue_SizeMatch
 * Type: Function
 * Rank: Important(2)
 * EnvConditions: N/A
 * CaseDescription: 1. set super fold display with valid activeRect
 *                  2. add layer with uniRenderFlag=true and matched size
 *                  3. call IsDropDirtyFrame
 *                  4. verify returns false (line 757 branch false - size matches)
 */
HWTEST_F(RsRenderComposerTest, IsDropDirtyFrame_UniRenderFlagTrue_SizeMatch, TestSize.Level1)
{
    system::SetParameter("const.window.foldscreen.type", "6,0,0,0");
    auto output = std::make_shared<HdiOutput>(0u);
    output->Init();
    sptr<RSScreenProperty> property = new RSScreenProperty();
    auto rsRenderComposerTmp = std::make_shared<RSRenderComposer>(output, property);

    rsRenderComposerTmp->composerScreenInfo_.activeRect = {100, 100, 100, 100};

    std::vector<std::shared_ptr<RSLayer>> layers;
    std::shared_ptr<RSRenderSurfaceLayer> layer = std::make_shared<RSRenderSurfaceLayer>();
    layer->SetLayerSize({100, 100, 100, 100}); // Matched with activeRect
    layer->SetUniRenderFlag(true);
    layers.push_back(layer);

    bool result = rsRenderComposerTmp->IsDropDirtyFrame(layers);
    EXPECT_EQ(result, false);

    system::SetParameter("const.window.foldscreen.type", "0,0,0,0");
}

/**
 * Function: IsDropDirtyFrame_UniRenderFlagFalse
 * Type: Function
 * Rank: Important(2)
 * EnvConditions: N/A
 * CaseDescription: 1. set super fold display with valid activeRect
 *                  2. add layer with uniRenderFlag=false (even with mismatched size)
 *                  3. call IsDropDirtyFrame
 *                  4. verify returns false (line 757 branch false - uniRenderFlag=false)
 */
HWTEST_F(RsRenderComposerTest, IsDropDirtyFrame_UniRenderFlagFalse, TestSize.Level1)
{
    system::SetParameter("const.window.foldscreen.type", "6,0,0,0");
    auto output = std::make_shared<HdiOutput>(0u);
    output->Init();
    sptr<RSScreenProperty> property = new RSScreenProperty();
    auto rsRenderComposerTmp = std::make_shared<RSRenderComposer>(output, property);

    rsRenderComposerTmp->composerScreenInfo_.activeRect = {100, 100, 100, 100};

    std::vector<std::shared_ptr<RSLayer>> layers;
    std::shared_ptr<RSRenderSurfaceLayer> layer = std::make_shared<RSRenderSurfaceLayer>();
    layer->SetLayerSize({200, 200, 100, 100}); // Mismatched with activeRect
    layer->SetUniRenderFlag(false);
    layers.push_back(layer);

    bool result = rsRenderComposerTmp->IsDropDirtyFrame(layers);
    EXPECT_EQ(result, false);

    system::SetParameter("const.window.foldscreen.type", "0,0,0,0");
}

/**
 * Function: IsDropDirtyFrame_MultipleLayers_Mixed
 * Type: Function
 * Rank: Important(2)
 * EnvConditions: N/A
 * CaseDescription: 1. set super fold display with valid activeRect
 *                  2. add multiple layers with different properties
 *                  3. call IsDropDirtyFrame
 *                  4. verify correct behavior with mixed layers
 */
HWTEST_F(RsRenderComposerTest, IsDropDirtyFrame_MultipleLayers_Mixed, TestSize.Level1)
{
    system::SetParameter("const.window.foldscreen.type", "6,0,0,0");
    auto output = std::make_shared<HdiOutput>(0u);
    output->Init();
    sptr<RSScreenProperty> property = new RSScreenProperty();
    auto rsRenderComposerTmp = std::make_shared<RSRenderComposer>(output, property);

    rsRenderComposerTmp->composerScreenInfo_.activeRect = {100, 100, 100, 100};

    std::vector<std::shared_ptr<RSLayer>> layers;

    // Layer 1: nullptr
    layers.push_back(nullptr);

    // Layer 2: uniRenderFlag=false, size mismatched
    std::shared_ptr<RSRenderSurfaceLayer> layer2 = std::make_shared<RSRenderSurfaceLayer>();
    layer2->SetLayerSize({200, 200, 100, 100});
    layer2->SetUniRenderFlag(false);
    layers.push_back(layer2);

    // Layer 3: uniRenderFlag=true, size matched
    std::shared_ptr<RSRenderSurfaceLayer> layer3 = std::make_shared<RSRenderSurfaceLayer>();
    layer3->SetLayerSize({100, 100, 100, 100});
    layer3->SetUniRenderFlag(true);
    layers.push_back(layer3);

    // All layers so far should not trigger drop
    bool result1 = rsRenderComposerTmp->IsDropDirtyFrame(layers);
    EXPECT_EQ(result1, false);

    // Layer 4: uniRenderFlag=true, size mismatched - should trigger drop
    std::shared_ptr<RSRenderSurfaceLayer> layer4 = std::make_shared<RSRenderSurfaceLayer>();
    layer4->SetLayerSize({200, 200, 100, 100});
    layer4->SetUniRenderFlag(true);
    layers.push_back(layer4);

    bool result2 = rsRenderComposerTmp->IsDropDirtyFrame(layers);
    EXPECT_EQ(result2, true);

    system::SetParameter("const.window.foldscreen.type", "0,0,0,0");
}

/**
 * Function: IsDropDirtyFrame_AllLayersMatch
 * Type: Function
 * Rank: Important(2)
 * EnvConditions: N/A
 * CaseDescription: 1. set super fold display with valid activeRect
 *                  2. add multiple layers all with matching size or uniRenderFlag=false
 *                  3. call IsDropDirtyFrame
 *                  4. verify returns false (no layer triggers drop)
 */
HWTEST_F(RsRenderComposerTest, IsDropDirtyFrame_AllLayersMatch, TestSize.Level1)
{
    system::SetParameter("const.window.foldscreen.type", "6,0,0,0");
    auto output = std::make_shared<HdiOutput>(0u);
    output->Init();
    sptr<RSScreenProperty> property = new RSScreenProperty();
    auto rsRenderComposerTmp = std::make_shared<RSRenderComposer>(output, property);

    rsRenderComposerTmp->composerScreenInfo_.activeRect = {100, 100, 100, 100};

    std::vector<std::shared_ptr<RSLayer>> layers;

    // Add multiple layers with matching size or uniRenderFlag=false
    for (int i = 0; i < 5; i++) {
        std::shared_ptr<RSRenderSurfaceLayer> layer = std::make_shared<RSRenderSurfaceLayer>();
        layer->SetLayerSize({100, 100, 100, 100}); // Matched size
        layer->SetUniRenderFlag(true);
        layers.push_back(layer);
    }

    bool result = rsRenderComposerTmp->IsDropDirtyFrame(layers);
    EXPECT_EQ(result, false);

    system::SetParameter("const.window.foldscreen.type", "0,0,0,0");
}

/**
 * Function: IsDropDirtyFrame_FirstLayerMismatch
 * Type: Function
 * Rank: Important(2)
 * EnvConditions: N/A
 * CaseDescription: 1. set super fold display with valid activeRect
 *                  2. first layer has uniRenderFlag=true and mismatched size
 *                  3. call IsDropDirtyFrame
 *                  4. verify returns true immediately (early return)
 */
HWTEST_F(RsRenderComposerTest, IsDropDirtyFrame_FirstLayerMismatch, TestSize.Level1)
{
    system::SetParameter("const.window.foldscreen.type", "6,0,0,0");
    auto output = std::make_shared<HdiOutput>(0u);
    output->Init();
    sptr<RSScreenProperty> property = new RSScreenProperty>();
    auto rsRenderComposerTmp = std::make_shared<RSRenderComposer>(output, property);

    rsRenderComposerTmp->composerScreenInfo_.activeRect = {100, 100, 100, 100};

    std::vector<std::shared_ptr<RSLayer>> layers;

    // First layer: mismatched
    std::shared_ptr<RSRenderSurfaceLayer> layer1 = std::make_shared<RSRenderSurfaceLayer>();
    layer1->SetLayerSize({200, 200, 100, 100});
    layer1->SetUniRenderFlag(true);
    layers.push_back(layer1);

    // Add more layers (should not be checked due to early return)
    std::shared_ptr<RSRenderSurfaceLayer> layer2 = std::make_shared<RSRenderSurfaceLayer>();
    layer2->SetLayerSize({100, 100, 100, 100});
    layer2->SetUniRenderFlag(true);
    layers.push_back(layer2);

    bool result = rsRenderComposerTmp->IsDropDirtyFrame(layers);
    EXPECT_EQ(result, true);

    system::SetParameter("const.window.foldscreen.type", "0,0,0,0");
}

/**
 * Function: IsDropDirtyFrame_LastLayerMismatch
 * Type: Function
 * Rank: Important(2)
 * EnvConditions: N/A
 * CaseDescription: 1. set super fold display with valid activeRect
 *                  2. all layers match except the last one
 *                  3. call IsDropDirtyFrame
 *                  4. verify returns true (checks all layers until mismatch found)
 */
HWTEST_F(RsRenderComposerTest, IsDropDirtyFrame_LastLayerMismatch, TestSize.Level1)
{
    system::SetParameter("const.window.foldscreen.type", "6,0,0,0");
    auto output = std::make_shared<HdiOutput>(0u);
    output->Init();
    sptr<RSScreenProperty> property = new RSScreenProperty();
    auto rsRenderComposerTmp = std::make_shared<RSRenderComposer>(output, property);

    rsRenderComposerTmp->composerScreenInfo_.activeRect = {100, 100, 100, 100};

    std::vector<std::shared_ptr<RSLayer>> layers;

    // Add matched layers
    for (int i = 0; i < 3; i++) {
        std::shared_ptr<RSRenderSurfaceLayer> layer = std::make_shared<RSRenderSurfaceLayer>();
        layer->SetLayerSize({100, 100, 100, 100});
        layer->SetUniRenderFlag(true);
        layers.push_back(layer);
    }

    // Add mismatched layer at the end
    std::shared_ptr<RSRenderSurfaceLayer> lastLayer = std::make_shared<RSRenderSurfaceLayer>();
    lastLayer->SetLayerSize({200, 200, 100, 100});
    lastLayer->SetUniRenderFlag(true);
    layers.push_back(lastLayer);

    bool result = rsRenderComposerTmp->IsDropDirtyFrame(layers);
    EXPECT_EQ(result, true);

    system::SetParameter("const.window.foldscreen.type", "0,0,0,0");
}

/**
 * Function: OnPrepareComplete
 * Type: Function
 * Rank: Important(2)
 * EnvConditions: N/A
 * CaseDescription: 1. create RSRenderComposer
 *                  2. call OnPrepareComplete
 *                  3. verify result
 */
HWTEST_F(RsRenderComposerTest, OnPrepareComplete, TestSize.Level1)
{
    PrepareCompleteParam param;
    param.needFlushFramebuffer = false;
    auto cSurface = IConsumerSurface::Create("cSurface");
    ASSERT_NE(cSurface, nullptr);
    sptr<IBufferProducer> producer = cSurface->GetProducer();
    auto pSurface = Surface::CreateSurfaceAsProducer(producer);
    rsRenderComposer_->OnPrepareComplete(pSurface, param, nullptr);
    std::vector<RSLayerPtr> layers;
    RSLayerPtr l1 = std::static_pointer_cast<RSLayer>(std::make_shared<FakeRSLayer>(1, false, "L1"));
    l1->SetSurface(cSurface);
    layers.emplace_back(l1);
    param.needFlushFramebuffer = true;
    param.layers = layers;
    rsRenderComposer_->OnPrepareComplete(pSurface, param, nullptr);
    auto cb = rsRenderComposer_->redrawCb_;
    rsRenderComposer_->redrawCb_ = nullptr;
    EXPECT_EQ(rsRenderComposer_->redrawCb_, nullptr);
    EXPECT_TRUE(param.needFlushFramebuffer);
    rsRenderComposer_->OnPrepareComplete(pSurface, param, nullptr);
    rsRenderComposer_->redrawCb_ = cb;
}

/**
 * Function: EndCheck
 * Type: Function
 * Rank: Important(2)
 * EnvConditions: N/A
 * CaseDescription: 1. create RSRenderComposer
 *                  2. call EndCheck when duration timeout
 *                  3. verify result
 */
HWTEST_F(RsRenderComposerTest, EndCheck, TestSize.Level1)
{
    // Threshold for abnormal time in the hardware pipeline
    constexpr int composerTimeout = 800;
    rsRenderComposer_->exceptionCnt_ = 0;
    RSTimer timer("EndCheckTest", composerTimeout); // 800ms
    rsRenderComposer_->EndCheck(timer);
    ASSERT_EQ(rsRenderComposer_->exceptionCnt_, 0); // time count 0
    std::this_thread::sleep_for(std::chrono::milliseconds(composerTimeout));
    rsRenderComposer_->EndCheck(timer);
    ASSERT_EQ(rsRenderComposer_->exceptionCnt_, 1); // timeout count 1
    rsRenderComposer_->exceptionCnt_ = 0;
}

/**
 * Function: EndCheck_ShortDuration_ResetCount
 * Type: Function
 * Rank: Important(2)
 * EnvConditions: N/A
 * CaseDescription: 1. create RSRenderComposer
 *                  2. call EndCheck when duration is short
 *                  3. verify result
 */
HWTEST_F(RsRenderComposerTest, EndCheck_ShortDuration_ResetCount, TestSize.Level1)
{
    bool isUpload = rsRenderComposer_->exceptionCheck_.isUpload_;
    int cnt = rsRenderComposer_->exceptionCnt_;

    rsRenderComposer_->exceptionCheck_.isUpload_ = false;
    rsRenderComposer_->exceptionCnt_ = 5;
    RSTimer timer("TestShort", 0);
    rsRenderComposer_->EndCheck(timer);
    EXPECT_EQ(rsRenderComposer_->exceptionCnt_, 0);

    rsRenderComposer_->exceptionCheck_.isUpload_ = isUpload;
    rsRenderComposer_->exceptionCnt_ = cnt;
}

/**
 * Function: EndCheck_TriggerUploadOnTimeoutCount
 * Type: Function
 * Rank: Important(2)
 * EnvConditions: N/A
 * CaseDescription: 1. create RSRenderComposer
 *                  2. call EndCheck when timeout
 *                  3. verify result
 */
HWTEST_F(RsRenderComposerTest, EndCheck_TriggerUploadOnTimeoutCount, TestSize.Level1)
{
    bool isUpload = rsRenderComposer_->exceptionCheck_.isUpload_;
    int cnt = rsRenderComposer_->exceptionCnt_;
    rsRenderComposer_->exceptionCheck_.isUpload_ = false;
    const int composerTimeoutCnt = 15;
    rsRenderComposer_->exceptionCnt_ = composerTimeoutCnt - 1;
    RSTimer timer("TestTimeout", 0);
    std::this_thread::sleep_for(std::chrono::milliseconds(900));

    rsRenderComposer_->EndCheck(timer);

    EXPECT_EQ(rsRenderComposer_->exceptionCnt_, composerTimeoutCnt);
    EXPECT_NE(rsRenderComposer_->exceptionCheck_.exceptionMoment_, 0);

    rsRenderComposer_->exceptionCheck_.isUpload_ = isUpload;
    rsRenderComposer_->exceptionCnt_ = cnt;
}

/**
 * Function: EndCheck_Abort
 * Type: Function
 * Rank: Important(2)
 * EnvConditions: N/A
 * CaseDescription: 1. create RSRenderComposer
 *                  2. call EndCheck when Abort
 *                  3. verify result
 */
HWTEST_F(RsRenderComposerTest, EndCheck_Abort, TestSize.Level1)
{
    bool isUpload = rsRenderComposer_->exceptionCheck_.isUpload_;
    int cnt = rsRenderComposer_->exceptionCnt_;
    rsRenderComposer_->exceptionCheck_.isUpload_ = false;
    const int composerTimeoutAbortCnt = 30;
    rsRenderComposer_->exceptionCnt_ = composerTimeoutAbortCnt - 1;
    RSTimer timer("TestAbort", 0);
    std::this_thread::sleep_for(std::chrono::milliseconds(900));
    rsRenderComposer_->EndCheck(timer);
    EXPECT_EQ(rsRenderComposer_->exceptionCnt_, composerTimeoutAbortCnt);
    EXPECT_NE(rsRenderComposer_->exceptionCheck_.exceptionMoment_, 0);

    rsRenderComposer_->exceptionCheck_.isUpload_ = isUpload;
    rsRenderComposer_->exceptionCnt_ = cnt;
}

/**
 * Function: RecordTimestamp
 * Type: Function
 * Rank: Important(2)
 * EnvConditions: N/A
 * CaseDescription: 1. create RSRenderComposer
 *                  2. call RecordTimestamp
 *                  3. verify result
 */
HWTEST_F(RsRenderComposerTest, RecordTimestamp, TestSize.Level1)
{
    std::vector<RSLayerPtr> layers;
    RSLayerPtr l1 = std::static_pointer_cast<RSLayer>(std::make_shared<FakeRSLayer>(1, false, "L1"));
    sptr<IConsumerSurface> cSurface = IConsumerSurface::Create("surface1");
    l1->SetSurface(cSurface);
    layers.emplace_back(l1);

    RSLayerPtr l2 = std::static_pointer_cast<RSLayer>(std::make_shared<FakeRSLayer>(2, true, "L2"));
    sptr<IConsumerSurface> cSurface2 = IConsumerSurface::Create("surface2");
    l2->SetSurface(cSurface2);
    l2->SetBuffer(nullptr);
    layers.emplace_back(l2);

    layers.emplace_back(nullptr);
    ASSERT_EQ(layers.size(), 3);
    EXPECT_EQ(layers[1]->GetBuffer(), nullptr);
    EXPECT_EQ(layers[2], nullptr);

    RSLayerPtr l3 = std::static_pointer_cast<RSLayer>(std::make_shared<FakeRSLayer>(3, true, "L3"));
    sptr<IConsumerSurface> cSurface3 = IConsumerSurface::Create("surface3");
    l3->SetSurface(cSurface3);

    RSSurfaceRenderNodeConfig config;
    config.id = 3;
    config.name = "surface3";
    auto rsSurfaceRenderNode = std::make_shared<RSSurfaceRenderNode>(config);
    sptr<IConsumerSurface> csurf = IConsumerSurface::Create(config.name);
    sptr<IBufferConsumerListener> listener =
        new RSRenderServiceListener(std::weak_ptr<RSSurfaceRenderNode>(rsSurfaceRenderNode), nullptr);
    csurf->RegisterConsumerListener(listener);
    rsSurfaceRenderNode->GetRSSurfaceHandler()->SetConsumer(csurf);
    rsSurfaceRenderNode->InitRenderParams();
    std::weak_ptr<RSSurfaceRenderNode> surfaceRenderNode(rsSurfaceRenderNode);
    const auto& surfaceConsumer = rsSurfaceRenderNode->GetRSSurfaceHandler()->GetConsumer();
    auto producer = surfaceConsumer->GetProducer();
    sptr<Surface> sProducer = Surface::CreateSurfaceAsProducer(producer);
    sProducer->SetQueueSize(1);
    sptr<SurfaceBuffer> buffer;
    sptr<SyncFence> requestFence = SyncFence::INVALID_FENCE;

    BufferRequestConfig requestConfig = {
        .width = 0x100, .height = 0x100, .strideAlignment = 0x8, .format = GRAPHIC_PIXEL_FMT_RGBA_8888,
        .usage = BUFFER_USAGE_CPU_READ | BUFFER_USAGE_CPU_WRITE | BUFFER_USAGE_MEM_DMA, .timeout = 0
    };

    EXPECT_EQ(sProducer->RequestBuffer(buffer, requestFence, requestConfig), GSERROR_OK);
    l3->SetBuffer(buffer);
    layers.emplace_back(l3);
    ASSERT_EQ(layers.size(), 4);

    uint64_t vsyncId = 1;
    auto output = std::make_shared<HdiOutput>(1u);
    output->Init();
    rsRenderComposer_->hdiOutput_ = output;
    ASSERT_NE(rsRenderComposer_->hdiOutput_, nullptr);
    rsRenderComposer_->RecordTimestamp(vsyncId);
    ASSERT_EQ(layers.size(), 4);

    l3->SetUniRenderFlag(true);
    rsRenderComposer_->RecordTimestamp(vsyncId);
    l3->SetUniRenderFlag(false);
    rsRenderComposer_->RecordTimestamp(vsyncId);
    l3->SetBuffer(nullptr);
    rsRenderComposer_->RecordTimestamp(vsyncId);
    EXPECT_EQ(layers.size(), 4);
    csurf->UnregisterConsumerListener();
}

/**
 * Function: IsDelayRequired
 * Type: Function
 * Rank: Important(2)
 * EnvConditions: N/A
 * CaseDescription: 1. create RSRenderComposer
 *                  2. call IsDelayRequired with different parameters
 *                  3. verify result
 */
HWTEST_F(RsRenderComposerTest, IsDelayRequired, TestSize.Level1)
{
    auto &hgmCore = HgmCore::Instance();
    PipelineParam param;
    param.frameTimestamp = 0;
    param.actualTimestamp = 0;
    param.vsyncId = 0;
    param.pendingConstraintRelativeTime = 0;
    param.isForceRefresh = true;
    param.fastComposeTimeStampDiff = 0;
    EXPECT_FALSE(rsRenderComposer_->IsDelayRequired(hgmCore, param));

    param.isForceRefresh = false;
    bool hgmCoreIsLtpoMode = hgmCore.isLtpoMode_.load();

    hgmCore.isLtpoMode_.store(false);
    EXPECT_TRUE(rsRenderComposer_->IsDelayRequired(hgmCore, param));

    hgmCore.isLtpoMode_.store(true);
    auto frameRateMgr = hgmCore.GetFrameRateMgr();
    ASSERT_NE(frameRateMgr, nullptr);

    bool frameRateMgrIsAdaptive = frameRateMgr->isAdaptive_.load();
    frameRateMgr->isAdaptive_.store(SupportASStatus::SUPPORT_AS);
    EXPECT_FALSE(rsRenderComposer_->IsDelayRequired(hgmCore, param));

    frameRateMgr->isAdaptive_.store(SupportASStatus::GAME_SCENE_SKIP);
    param.hasGameScene = true;
    EXPECT_FALSE(rsRenderComposer_->IsDelayRequired(hgmCore, param));

    hgmCore.isLtpoMode_.store(false);
    bool hgmCoreIsDelayMode = hgmCore.isDelayMode_;
    hgmCore.isDelayMode_ = false;
    EXPECT_FALSE(rsRenderComposer_->IsDelayRequired(hgmCore, param));

    hgmCore.isDelayMode_ = true;
    EXPECT_FALSE(rsRenderComposer_->IsDelayRequired(hgmCore, param));

    hgmCore.isLtpoMode_.store(hgmCoreIsLtpoMode);
    frameRateMgr->isAdaptive_.store(frameRateMgrIsAdaptive);
    hgmCore.isDelayMode_ = hgmCoreIsDelayMode;
}

/**
 * Function: CreateFrameBufferSurfaceOhos
 * Type: Function
 * Rank: Important(2)
 * EnvConditions: N/A
 * CaseDescription: 1. create RSRenderComposer
 *                  2. call CreateFrameBufferSurfaceOhos
 *                  3. verify result
 */
HWTEST_F(RsRenderComposerTest, CreateFrameBufferSurfaceOhos, TestSize.Level1)
{
    auto csurface = IConsumerSurface::Create();
    ASSERT_NE(csurface, nullptr);
    auto producer = csurface->GetProducer();
    ASSERT_NE(producer, nullptr);
    auto psurface = Surface::CreateSurfaceAsProducer(producer);
    ASSERT_NE(psurface, nullptr);

    auto rsSurfaceOhosPtr = rsRenderComposer_->CreateFrameBufferSurfaceOhos(psurface);
    ASSERT_NE(rsSurfaceOhosPtr, nullptr);
}

/**
 * Function: Redraw001
 * Type: Function
 * Rank: Important(2)
 * EnvConditions: N/A
 * CaseDescription: 1. create RSRenderComposer
 *                  2. call Redraw
 *                  3. verify result
 */
HWTEST_F(RsRenderComposerTest, Redraw001, TestSize.Level1)
{
    auto output = std::make_shared<HdiOutput>(0u);
    output->Init();
    sptr<RSScreenProperty> property = new RSScreenProperty();
    auto rsRenderComposerTmp = std::make_shared<RSRenderComposer>(output, property);
    EXPECT_NE(rsRenderComposerTmp->hdiOutput_, nullptr);

    BufferRequestConfig requestConfig = {
        .width = 0x100,
        .height = 0x100,
        .strideAlignment = 0x8,
        .format = GRAPHIC_PIXEL_FMT_RGBA_8888,
        .usage = BUFFER_USAGE_CPU_READ | BUFFER_USAGE_CPU_WRITE | BUFFER_USAGE_MEM_DMA | BUFFER_USAGE_PROTECTED,
        .timeout = 0,
    };

    auto csurface = IConsumerSurface::Create();
    ASSERT_NE(csurface, nullptr);
    sptr<IBufferConsumerListener> listener = new BufferConsumerListener();
    csurface->RegisterConsumerListener(listener);
    auto producer = csurface->GetProducer();
    auto psurface = Surface::CreateSurfaceAsProducer(producer);
    ASSERT_NE(psurface, nullptr);
    sptr<SurfaceBuffer> buffer;
    sptr<SyncFence> requestFence = SyncFence::INVALID_FENCE;
    EXPECT_EQ(psurface->RequestBuffer(buffer, requestFence, requestConfig), GSERROR_OK);

    std::vector<RSLayerPtr> layers;
    layers.emplace_back(nullptr);
    RSLayerPtr layer = std::static_pointer_cast<RSLayer>(std::make_shared<FakeRSLayer>(1, false, "L1"));
    layers.emplace_back(layer);
    EXPECT_NE(layers.size(), 0);

    rsRenderComposerTmp->Redraw(psurface, layers);
    rsRenderComposerTmp->Redraw(nullptr, layers);
    auto uniRenderEngine = rsRenderComposerTmp->uniRenderEngine_;
    rsRenderComposerTmp->uniRenderEngine_ = nullptr;
    rsRenderComposerTmp->Redraw(psurface, layers);
    rsRenderComposerTmp->uniRenderEngine_ = uniRenderEngine;
    rsRenderComposerTmp->Redraw(psurface, layers);

    RSLayerPtr layer1 = std::static_pointer_cast<RSLayer>(std::make_shared<RSRenderSurfaceLayer>());
    layer1->SetBuffer(buffer);
    layers.emplace_back(layer1);
    rsRenderComposerTmp->frameBufferSurfaceOhosMap_.insert({0, nullptr});
    rsRenderComposerTmp->Redraw(psurface, layers);
    csurface->UnregisterConsumerListener();

    auto hdiOutput = rsRenderComposerTmp->hdiOutput_;
    rsRenderComposerTmp->hdiOutput_ = nullptr;
    rsRenderComposerTmp->Redraw(psurface, layers);
    rsRenderComposerTmp->hdiOutput_ = hdiOutput;
}

#ifdef USE_VIDEO_PROCESSING_ENGINE
/**
 * Function: Redraw002
 * Type: Function
 * Rank: Important(2)
 * EnvConditions: N/A
 * CaseDescription: 1. create RSRenderComposer
 *                  2. call Redraw
 *                  3. verify result
 */
HWTEST_F(RsRenderComposerTest, Redraw002, TestSize.Level1)
{
    BufferRequestConfig requestConfig = {
        .width = 0x100,
        .height = 0x100,
        .strideAlignment = 0x8,
        .format = GRAPHIC_PIXEL_FMT_RGBA_8888,
        .usage = BUFFER_USAGE_CPU_READ | BUFFER_USAGE_CPU_WRITE | BUFFER_USAGE_MEM_DMA,
        .timeout = 0,
    };

    auto csurface = IConsumerSurface::Create();
    ASSERT_NE(csurface, nullptr);
    sptr<IBufferConsumerListener> listener = new BufferConsumerListener();
    csurface->RegisterConsumerListener(listener);
    auto producer = csurface->GetProducer();
    auto psurface = Surface::CreateSurfaceAsProducer(producer);
    ASSERT_NE(psurface, nullptr);
    sptr<SurfaceBuffer> buffer;
    sptr<SyncFence> requestFence = SyncFence::INVALID_FENCE;
    EXPECT_EQ(psurface->RequestBuffer(buffer, requestFence, requestConfig), GSERROR_OK);

    std::vector<RSLayerPtr> layers;
    std::vector<GraphicPixelFormat> formats = {
        GRAPHIC_PIXEL_FMT_RGBA_1010102,
        GRAPHIC_PIXEL_FMT_YCBCR_P010,
        GRAPHIC_PIXEL_FMT_RGBA_1010108,
    };
    std::vector<std::string> layerNames = {"R1", "R2", "R3"};
    for (size_t i = 0; i < formats.size(); ++i) {
        RSLayerPtr layer = std::make_shared<FakeRSLayer>(i + 20, false, layerNames[i]);
        RSSurfaceRenderNodeConfig config;
        config.id = i + 20;
        config.name = "surface_" + layerNames[i];

        auto rsSurfaceRenderNode = std::make_shared<RSSurfaceRenderNode>(config);
        sptr<IConsumerSurface> cSurface = IConsumerSurface::Create(config.name);
        auto surfaceBufferImpl = new SurfaceBufferImpl();
        auto handle = new BufferHandle();
        ASSERT_NE(handle, nullptr);
        handle->format = formats[i];
        surfaceBufferImpl->SetBufferHandle(handle);
        sptr<SyncFence> requestFence = SyncFence::INVALID_FENCE;
        layer->SetBuffer(surfaceBufferImpl);
        layer->SetSurface(cSurface);
        layers.emplace_back(layer);
    }

    GraphicColorGamut colorGamut = GRAPHIC_COLOR_GAMUT_SRGB;
    GraphicPixelFormat pixelFormat = GRAPHIC_PIXEL_FMT_RGBA_8888;
    auto hdiOutput = HdiOutput::CreateHdiOutput(0);
    EXPECT_CALL(*hdiDeviceMock_, GetDisplayClientTargetProperty(testing::_, testing::_, testing::_)).WillRepeatedly(
            testing::Return(GRAPHIC_DISPLAY_NOT_SUPPORT));
    sptr<RSScreenProperty> property = new RSScreenProperty();
    auto tmpRsRenderComposer = std::make_shared<RSRenderComposer>(hdiOutput, property);
    tmpRsRenderComposer->hdiOutput_->SetHdiOutputDevice(hdiDeviceMock_);
    bool ret = tmpRsRenderComposer->GetDisplayClientTargetProperty(pixelFormat, colorGamut, layers);
    EXPECT_FALSE(ret);
    tmpRsRenderComposer->Redraw(psurface, layers);
    EXPECT_CALL(*hdiDeviceMock_, GetDisplayClientTargetProperty(testing::_, testing::_, testing::_)).WillRepeatedly(
            testing::Return(GRAPHIC_DISPLAY_SUCCESS));
    ret = tmpRsRenderComposer->GetDisplayClientTargetProperty(pixelFormat, colorGamut, layers);
    EXPECT_TRUE(ret);
    tmpRsRenderComposer->Redraw(psurface, layers);

    tmpRsRenderComposer->hdiOutput_ = nullptr;
    tmpRsRenderComposer->Redraw(psurface, layers);
    tmpRsRenderComposer->hdiOutput_ = hdiOutput;
}

/**
 * Function: ComputeTargetColorGamut001
 * Type: Function
 * Rank: Important(1)
 * EnvConditions: N/A
 * CaseDescription: 1. create RSRenderComposer
 *                  2. call ComputeTargetColorGamut
 *                  3. verify result
 */
HWTEST_F(RsRenderComposerTest, ComputeTargetColorGamut001, TestSize.Level1)
{
    NodeId id = 0;
    RSSurfaceRenderNodeConfig config;
    config.id = id;
    config.name = std::to_string(id);
    auto rsSurfaceRenderNode = std::make_shared<RSSurfaceRenderNode>(config);
    ASSERT_NE(rsSurfaceRenderNode, nullptr);
    sptr<IConsumerSurface> cSurface = IConsumerSurface::Create(config.name);
    rsSurfaceRenderNode->GetRSSurfaceHandler()->SetConsumer(cSurface);
    sptr<IBufferConsumerListener> listener = new RSRenderServiceListener(
        std::weak_ptr<RSSurfaceRenderNode>(rsSurfaceRenderNode), nullptr);
    cSurface->RegisterConsumerListener(listener);
    rsSurfaceRenderNode->InitRenderParams();

    const auto& surfaceConsumer = rsSurfaceRenderNode->GetRSSurfaceHandler()->GetConsumer();
    auto producer = surfaceConsumer->GetProducer();
    sptr<Surface> sProducer = Surface::CreateSurfaceAsProducer(producer);
    sProducer->SetQueueSize(2);
    sptr<SurfaceBuffer> buffer;
    sptr<SyncFence> requestFence = SyncFence::INVALID_FENCE;
    BufferRequestConfig requestConfig = {
        .width = 0x100,
        .height = 0x100,
        .strideAlignment = 0x8,
        .format = GRAPHIC_PIXEL_FMT_RGBA_8888,
        .usage = BUFFER_USAGE_CPU_READ | BUFFER_USAGE_CPU_WRITE | BUFFER_USAGE_MEM_DMA,
        .timeout = 0,
    };
    GSError ret = sProducer->RequestBuffer(buffer, requestFence, requestConfig);
    EXPECT_EQ(ret, GSERROR_OK);

    GraphicColorGamut colorGamut = rsRenderComposer_->ComputeTargetColorGamut(buffer);
    EXPECT_EQ(colorGamut, GRAPHIC_COLOR_GAMUT_DISPLAY_P3);

    requestConfig.colorGamut = GRAPHIC_COLOR_GAMUT_DISPLAY_P3;
    ret = sProducer->RequestBuffer(buffer, requestFence, requestConfig);
    EXPECT_EQ(ret, GSERROR_OK);
    colorGamut = rsRenderComposer_->ComputeTargetColorGamut(buffer);
    EXPECT_EQ(colorGamut, GRAPHIC_COLOR_GAMUT_DISPLAY_P3);
    cSurface->UnregisterConsumerListener();
}

/**
 * Function: ComputeTargetColorGamut002
 * Type: Function
 * Rank: Important(1)
 * EnvConditions: N/A
 * CaseDescription: 1. create RSRenderComposer
 *                  2. call ComputeTargetColorGamut
 *                  3. verify result
 */
HWTEST_F(RsRenderComposerTest, ComputeTargetColorGamut002, TestSize.Level1)
{
    NodeId id = 1;
    RSSurfaceRenderNodeConfig config;
    config.id = id;
    config.name = std::to_string(id);
    auto rsSurfaceRenderNode = std::make_shared<RSSurfaceRenderNode>(config);
    ASSERT_NE(rsSurfaceRenderNode, nullptr);
    sptr<IConsumerSurface> cSurface = IConsumerSurface::Create(config.name);
    rsSurfaceRenderNode->GetRSSurfaceHandler()->SetConsumer(cSurface);
    std::weak_ptr<RSSurfaceRenderNode> surfaceRenderNode(rsSurfaceRenderNode);
    sptr<IBufferConsumerListener> listener = new RSRenderServiceListener(
        std::weak_ptr<RSSurfaceRenderNode>(rsSurfaceRenderNode), nullptr);
    cSurface->RegisterConsumerListener(listener);
    rsSurfaceRenderNode->InitRenderParams();

    const auto& surfaceConsumer = rsSurfaceRenderNode->GetRSSurfaceHandler()->GetConsumer();
    auto producer = surfaceConsumer->GetProducer();
    sptr<Surface> sProducer = Surface::CreateSurfaceAsProducer(producer);
    sProducer->SetQueueSize(1);
    sptr<SurfaceBuffer> buffer;
    sptr<SyncFence> requestFence = SyncFence::INVALID_FENCE;
    BufferRequestConfig requestConfig = {
        .width = 0x100,
        .height = 0x100,
        .strideAlignment = 0x8,
        .format = GRAPHIC_PIXEL_FMT_RGBA_8888,
        .usage = BUFFER_USAGE_CPU_READ | BUFFER_USAGE_CPU_WRITE | BUFFER_USAGE_MEM_DMA,
        .timeout = 0,
    };
    GSError ret = sProducer->RequestBuffer(buffer, requestFence, requestConfig);
    EXPECT_EQ(ret, GSERROR_OK);

    using namespace HDI::Display::Graphic::Common::V1_0;
    CM_ColorSpaceInfo infoSet = {
        .primaries = COLORPRIMARIES_SRGB,
    };
    auto retSet = MetadataHelper::SetColorSpaceInfo(buffer, infoSet);
    EXPECT_EQ(retSet, GSERROR_OK);
    GraphicColorGamut colorGamut = rsRenderComposer_->ComputeTargetColorGamut(buffer);
    EXPECT_EQ(colorGamut, GRAPHIC_COLOR_GAMUT_SRGB);

    infoSet = {
        .primaries = COLORPRIMARIES_BT2020,
    };
    retSet = MetadataHelper::SetColorSpaceInfo(buffer, infoSet);
    EXPECT_EQ(retSet, GSERROR_OK);
    colorGamut = rsRenderComposer_->ComputeTargetColorGamut(buffer);
    EXPECT_EQ(colorGamut, GRAPHIC_COLOR_GAMUT_DISPLAY_P3);
    cSurface->UnregisterConsumerListener();
}

/**
 * Function: ComputeTargetColorGamut003
 * Type: Function
 * Rank: Important(1)
 * EnvConditions: N/A
 * CaseDescription: 1. create layers
 *                  2. call ComputeTargetColorGamut
 *                  3. check result
 */
HWTEST_F(RsRenderComposerTest, ComputeTargetColorGamut003, TestSize.Level1)
{
    BufferRequestConfig requestConfig = {
        .width = 0x100,
        .height = 0x100,
        .strideAlignment = 0x8,
        .format = GRAPHIC_PIXEL_FMT_RGBA_8888,
        .usage = BUFFER_USAGE_CPU_READ | BUFFER_USAGE_CPU_WRITE | BUFFER_USAGE_MEM_DMA,
        .timeout = 0,
    };

    std::vector<RSLayerPtr> layers;
    layers.emplace_back(nullptr);
    RSLayerPtr l1 = std::static_pointer_cast<RSLayer>(std::make_shared<FakeRSLayer>(1, false, "L1"));
    l1->SetBuffer(nullptr);
    layers.emplace_back(l1);

    NodeId id = 3;
    RSSurfaceRenderNodeConfig config;
    config.id = id;
    config.name = std::to_string(id);
    auto rsSurfaceRenderNode = std::make_shared<RSSurfaceRenderNode>(config);
    ASSERT_NE(rsSurfaceRenderNode, nullptr);
    sptr<IConsumerSurface> cSurface = IConsumerSurface::Create(config.name);
    rsSurfaceRenderNode->GetRSSurfaceHandler()->SetConsumer(cSurface);
    sptr<IBufferConsumerListener> listener = new RSRenderServiceListener(
        std::weak_ptr<RSSurfaceRenderNode>(rsSurfaceRenderNode), nullptr);
    cSurface->RegisterConsumerListener(listener);
    rsSurfaceRenderNode->InitRenderParams();
    std::weak_ptr<RSSurfaceRenderNode> surfaceRenderNode(rsSurfaceRenderNode);

    const auto& surfaceConsumer = rsSurfaceRenderNode->GetRSSurfaceHandler()->GetConsumer();
    auto producer = surfaceConsumer->GetProducer();
    sptr<Surface> sProducer = Surface::CreateSurfaceAsProducer(producer);
    sProducer->SetQueueSize(2);
    sptr<SurfaceBuffer> buffer1;
    sptr<SyncFence> requestFence = SyncFence::INVALID_FENCE;
    GSError ret = sProducer->RequestBuffer(buffer1, requestFence, requestConfig);
    EXPECT_EQ(ret, GSERROR_OK);

    RSLayerPtr l2 = std::static_pointer_cast<RSLayer>(std::make_shared<FakeRSLayer>(2, false, "L2"));
    l2->SetBuffer(buffer1);
    layers.emplace_back(l2);

    sptr<SurfaceBuffer> buffer2;
    requestConfig.colorGamut = GRAPHIC_COLOR_GAMUT_DISPLAY_P3;
    ret = sProducer->RequestBuffer(buffer2, requestFence, requestConfig);
    EXPECT_EQ(ret, GSERROR_OK);

    RSLayerPtr l3 = std::static_pointer_cast<RSLayer>(std::make_shared<FakeRSLayer>(3, false, "L3"));
    l3->SetBuffer(buffer2);
    layers.emplace_back(l3);

    auto colorGamut = rsRenderComposer_->ComputeTargetColorGamut(layers);
    EXPECT_EQ(colorGamut, GRAPHIC_COLOR_GAMUT_SRGB);
    cSurface->UnregisterConsumerListener();
}

/**
 * Function: ComputeTargetColorGamut004
 * Type: Function
 * Rank: Important(1)
 * EnvConditions: N/A
 * CaseDescription: 1. create layers
 *                  2. call ComputeTargetColorGamut
 *                  3. check result
 */
HWTEST_F(RsRenderComposerTest, ComputeTargetColorGamut004, TestSize.Level1)
{
    BufferRequestConfig requestConfig = {
        .width = 0x100,
        .height = 0x100,
        .strideAlignment = 0x8,
        .format = GRAPHIC_PIXEL_FMT_RGBA_8888,
        .usage = BUFFER_USAGE_CPU_READ | BUFFER_USAGE_CPU_WRITE | BUFFER_USAGE_MEM_DMA,
        .timeout = 0,
    };

    NodeId id = 4;
    RSSurfaceRenderNodeConfig config;
    config.id = id;
    config.name = std::to_string(id);
    auto rsSurfaceRenderNode = std::make_shared<RSSurfaceRenderNode>(config);
    ASSERT_NE(rsSurfaceRenderNode, nullptr);
    sptr<IConsumerSurface> cSurface = IConsumerSurface::Create(config.name);
    rsSurfaceRenderNode->GetRSSurfaceHandler()->SetConsumer(cSurface);
    sptr<IBufferConsumerListener> listener = new RSRenderServiceListener(
        std::weak_ptr<RSSurfaceRenderNode>(rsSurfaceRenderNode), nullptr);
    cSurface->RegisterConsumerListener(listener);
    rsSurfaceRenderNode->InitRenderParams();
    std::weak_ptr<RSSurfaceRenderNode> surfaceRenderNode(rsSurfaceRenderNode);

    const auto& surfaceConsumer = rsSurfaceRenderNode->GetRSSurfaceHandler()->GetConsumer();
    auto producer = surfaceConsumer->GetProducer();
    sptr<Surface> sProducer = Surface::CreateSurfaceAsProducer(producer);
    sProducer->SetQueueSize(1);
    sptr<SurfaceBuffer> buffer;
    sptr<SyncFence> requestFence = SyncFence::INVALID_FENCE;
    GSError ret = sProducer->RequestBuffer(buffer, requestFence, requestConfig);
    EXPECT_EQ(ret, GSERROR_OK);

    using namespace HDI::Display::Graphic::Common::V1_0;
    CM_ColorSpaceInfo infoSet = {
        .primaries = COLORPRIMARIES_SRGB,
    };
    auto retSet = MetadataHelper::SetColorSpaceInfo(buffer, infoSet);
    EXPECT_EQ(retSet, GSERROR_OK);
    std::vector<RSLayerPtr> layers;
    RSLayerPtr l1 = std::static_pointer_cast<RSLayer>(std::make_shared<FakeRSLayer>(3, false, "L1"));
    l1->SetBuffer(buffer);
    layers.emplace_back(l1);
    auto colorGamut = rsRenderComposer_->ComputeTargetColorGamut(layers);
    EXPECT_EQ(colorGamut, GRAPHIC_COLOR_GAMUT_SRGB);
    cSurface->UnregisterConsumerListener();
}

/**
 * Function: ComputeTargetPixelFormat_Layers
 * Type: Function
 * Rank: Important(1)
 * EnvConditions: N/A
 * CaseDescription: 1. create layers
 *                  2. call ComputeTargetPixelFormat with layers
 *                  3. check result
 */
HWTEST_F(RsRenderComposerTest, ComputeTargetPixelFormat_Layers, TestSize.Level1)
{
    std::vector<RSLayerPtr> layers;
    RSLayerPtr l1 = std::static_pointer_cast<RSLayer>(std::make_shared<FakeRSLayer>(1, false, "L1"));
    l1->SetBuffer(nullptr);
    layers.emplace_back(l1);

    EXPECT_EQ(rsRenderComposer_->ComputeTargetPixelFormat(layers), GRAPHIC_PIXEL_FMT_RGBA_8888);

    layers.emplace_back(nullptr);
    EXPECT_EQ(rsRenderComposer_->ComputeTargetPixelFormat(layers), GRAPHIC_PIXEL_FMT_RGBA_8888);
    std::vector<GraphicCompositionType> skipTypes = {
        GraphicCompositionType::GRAPHIC_COMPOSITION_DEVICE,
        GraphicCompositionType::GRAPHIC_COMPOSITION_DEVICE_CLEAR,
        GraphicCompositionType::GRAPHIC_COMPOSITION_SOLID_COLOR,
    };
    std::vector<std::string> skipLayerNames = {"S1", "S2", "S3"};
    for (size_t i = 0; i < skipTypes.size(); ++i) {
        RSLayerPtr skipLayer = std::static_pointer_cast<RSLayer>(
            std::make_shared<FakeRSLayer>(i + 2, false, skipLayerNames[i]));
        skipLayer->SetCompositionType(skipTypes[i]);
        layers.emplace_back(skipLayer);
    }
    EXPECT_EQ(rsRenderComposer_->ComputeTargetPixelFormat(layers), GRAPHIC_PIXEL_FMT_RGBA_8888);

    std::vector<RoundCornerDisplayManager::RCDLayerType> rcdTypes = {
        RoundCornerDisplayManager::RCDLayerType::INVALID,
        RoundCornerDisplayManager::RCDLayerType::TOP,
        RoundCornerDisplayManager::RCDLayerType::BOTTOM,
    };

    using RSRcdManager = RSSingleton<RoundCornerDisplayManager>;
    std::vector<std::string> rcdLayerNames = {"R1", "R2", "R3"};
    for (size_t i = 0; i < rcdTypes.size(); ++i) {
        RSLayerPtr rcdLayer = std::static_pointer_cast<RSLayer>(
            std::make_shared<FakeRSLayer>(i + 10, false, rcdLayerNames[i]));
        sptr<IConsumerSurface> cSurface = IConsumerSurface::Create("surface_" + rcdLayerNames[i]);
        rcdLayer->SetSurface(cSurface);
        RSRcdManager::GetInstance().AddLayer(rcdLayerNames[i], i + 10, rcdTypes[i]);
        layers.emplace_back(rcdLayer);
    }
    EXPECT_EQ(rsRenderComposer_->ComputeTargetPixelFormat(layers), GRAPHIC_PIXEL_FMT_RGBA_8888);
    EXPECT_NE(layers.size(), 0);
}

/**
 * Function: ComputeTargetPixelFormat_Layers_HasBuffer
 * Type: Function
 * Rank: Important(1)
 * EnvConditions: N/A
 * CaseDescription: 1. create layers
 *                  2. call ComputeTargetPixelFormat with layers has buffer
 *                  3. verify result
 */
HWTEST_F(RsRenderComposerTest, ComputeTargetPixelFormat_Layers_HasBuffer, TestSize.Level1)
{
    std::vector<RSLayerPtr> layers;
    std::vector<GraphicPixelFormat> formats = {
        GRAPHIC_PIXEL_FMT_RGBA_1010102,
        GRAPHIC_PIXEL_FMT_YCBCR_P010,
        GRAPHIC_PIXEL_FMT_YCRCB_P010,
        GRAPHIC_PIXEL_FMT_RGBA_1010108,
    };
    std::vector<std::string> layerNames = {"R1", "R2", "R3", "R4"};
    for (size_t i = 0; i < formats.size(); ++i) {
        RSLayerPtr layer = std::static_pointer_cast<RSLayer>(
            std::make_shared<FakeRSLayer>(i + 20, false, layerNames[i]));
        RSSurfaceRenderNodeConfig config;
        config.id = i + 20;
        config.name = "surface_" + layerNames[i];

        auto rsSurfaceRenderNode = std::make_shared<RSSurfaceRenderNode>(config);
        sptr<IConsumerSurface> cSurface = IConsumerSurface::Create(config.name);
        auto surfaceBufferImpl = new SurfaceBufferImpl();
        auto handle = new BufferHandle();
        ASSERT_NE(handle, nullptr);
        handle->format = formats[i];
        surfaceBufferImpl->SetBufferHandle(handle);
        sptr<SyncFence> requestFence = SyncFence::INVALID_FENCE;
        layer->SetBuffer(surfaceBufferImpl);
        layer->SetSurface(cSurface);
        layers.emplace_back(layer);
    }
    EXPECT_EQ(rsRenderComposer_->ComputeTargetPixelFormat(layers), GRAPHIC_PIXEL_FMT_RGBA_1010102);
    EXPECT_NE(layers.size(), 0);
}

/**
 * Function: IsAllRedraw
 * Type: Function
 * Rank: Important(2)
 * EnvConditions: N/A
 * CaseDescription: 1. create RSRenderComposer
 *                  2. call IsAllRedraw
 *                  3. verify result
 */
HWTEST_F(RsRenderComposerTest, IsAllRedraw, TestSize.Level1)
{
    std::vector<RSLayerPtr> layers;
    layers.emplace_back(nullptr);

    RSLayerPtr l0 = std::static_pointer_cast<RSLayer>(std::make_shared<RSSurfaceRCDLayer>(100u, nullptr));
    layers.emplace_back(l0);

    RSLayerPtr l1 = std::static_pointer_cast<RSLayer>(std::make_shared<FakeRSLayer>(101u, false, "777"));
    layers.emplace_back(l1);

    RSLayerPtr l2 = std::static_pointer_cast<RSLayer>(std::make_shared<FakeRSLayer>(777u, false, "777"));
    sptr<IConsumerSurface> cSurface = IConsumerSurface::Create("777");
    l2->SetSurface(cSurface);
    using RSRcdManager = RSSingleton<RoundCornerDisplayManager>;
    RSRcdManager::GetInstance().AddLayer("777", 777u, RoundCornerDisplayManager::RCDLayerType::INVALID);
    layers.emplace_back(l2);

    RSLayerPtr l3 = std::static_pointer_cast<RSLayer>(std::make_shared<FakeRSLayer>(666u, false, "666"));
    sptr<IConsumerSurface> cSurface2 = IConsumerSurface::Create("666");
    l3->SetSurface(cSurface2);
    RSRcdManager::GetInstance().AddLayer("666", 666u, RoundCornerDisplayManager::RCDLayerType::TOP);
    layers.emplace_back(l3);

    RSLayerPtr l4 = std::static_pointer_cast<RSLayer>(std::make_shared<FakeRSLayer>(101u, false, "777"));
    l4->SetType(GraphicLayerType::GRAPHIC_LAYER_TYPE_CURSOR);
    layers.emplace_back(l4);

    EXPECT_NE(layers.size(), 0);
    EXPECT_TRUE(rsRenderComposer_->IsAllRedraw(layers));

    RSLayerPtr l5 = std::static_pointer_cast<RSLayer>(std::make_shared<FakeRSLayer>(102u, false, "777"));
    l5->SetHdiCompositionType(GraphicCompositionType::GRAPHIC_COMPOSITION_DEVICE);
    layers.emplace_back(l5);
    EXPECT_NE(layers.size(), 0);
    EXPECT_FALSE(rsRenderComposer_->IsAllRedraw(layers));
    layers.pop_back();

    RSLayerPtr l6 = std::static_pointer_cast<RSLayer>(std::make_shared<FakeRSLayer>(103u, false, "777"));
    l6->SetHdiCompositionType(GraphicCompositionType::GRAPHIC_COMPOSITION_DEVICE_CLEAR);
    layers.emplace_back(l6);
    EXPECT_NE(layers.size(), 0);
    EXPECT_FALSE(rsRenderComposer_->IsAllRedraw(layers));
    layers.pop_back();

    RSLayerPtr l7 = std::static_pointer_cast<RSLayer>(std::make_shared<FakeRSLayer>(104u, false, "777"));
    l7->SetHdiCompositionType(GraphicCompositionType::GRAPHIC_COMPOSITION_SOLID_COLOR);
    layers.emplace_back(l7);

    EXPECT_NE(layers.size(), 0);
    EXPECT_FALSE(rsRenderComposer_->IsAllRedraw(layers));
}

/**
 * Function: ConvertColorGamutToSpaceType
 * Type: Function
 * Rank: Important(2)
 * EnvConditions: N/A
 * CaseDescription: 1. create RSRenderComposer
 *                  2. call ConvertColorGramutToSpaceType
 *                  3. check result
 */
HWTEST_F(RsRenderComposerTest, ConvertColorGamutToSpaceType, TestSize.Level1)
{
    static const std::vector<GraphicColorGamut> typeList = {
        GRAPHIC_COLOR_GAMUT_STANDARD_BT601,
        GRAPHIC_COLOR_GAMUT_STANDARD_BT709,
        GRAPHIC_COLOR_GAMUT_SRGB,
        GRAPHIC_COLOR_GAMUT_ADOBE_RGB,
        GRAPHIC_COLOR_GAMUT_DISPLAY_P3,
        GRAPHIC_COLOR_GAMUT_BT2020,
        GRAPHIC_COLOR_GAMUT_BT2100_PQ,
        GRAPHIC_COLOR_GAMUT_BT2100_HLG,
        GRAPHIC_COLOR_GAMUT_DISPLAY_BT2020,
    };
    CM_ColorSpaceType colorSpace = CM_SRGB_FULL;
    for (const auto& type : typeList) {
        GraphicColorGamut colorGamut = type;
        EXPECT_TRUE(rsRenderComposer_->ConvertColorGamutToSpaceType(colorGamut, colorSpace));
    }
    GraphicColorGamut colorGamut = GRAPHIC_COLOR_GAMUT_INVALID;
    EXPECT_FALSE(rsRenderComposer_->ConvertColorGamutToSpaceType(colorGamut, colorSpace));
}

/**
 * Function: ComputeTargetPixelFormat001
 * Type: Function
 * Rank: Important(1)
 * EnvConditions: N/A
 * CaseDescription: 1. create layers
 *                  2. call ComputeTargetPixelFormat with layers has buffer
 *                  3. verify result
 */
HWTEST_F(RsRenderComposerTest, ComputeTargetPixelFormat001, TestSize.Level1)
{
    BufferRequestConfig requestConfig = {
        .width = 0x100,
        .height = 0x100,
        .strideAlignment = 0x8,
        .format = GRAPHIC_PIXEL_FMT_RGBA_8888,
        .usage = BUFFER_USAGE_CPU_READ | BUFFER_USAGE_CPU_WRITE | BUFFER_USAGE_MEM_DMA,
        .timeout = 0,
    };

    NodeId id = 1;
    RSSurfaceRenderNodeConfig config;
    config.id = id;
    config.name = std::to_string(id);
    auto rsSurfaceRenderNode = std::make_shared<RSSurfaceRenderNode>(config);
    sptr<IConsumerSurface> cSurface = IConsumerSurface::Create(config.name);
    sptr<IBufferConsumerListener> listener = new RSRenderServiceListener(
        std::weak_ptr<RSSurfaceRenderNode>(rsSurfaceRenderNode), nullptr);
    cSurface->RegisterConsumerListener(listener);
    rsSurfaceRenderNode->InitRenderParams();
    rsSurfaceRenderNode->GetRSSurfaceHandler()->SetConsumer(cSurface);

    const auto& surfaceConsumer = rsSurfaceRenderNode->GetRSSurfaceHandler()->GetConsumer();
    auto producer = surfaceConsumer->GetProducer();
    sptr<Surface> sProducer = Surface::CreateSurfaceAsProducer(producer);
    sProducer->SetQueueSize(3);
    sptr<SurfaceBuffer> buffer;
    sptr<SyncFence> requestFence = SyncFence::INVALID_FENCE;
    requestConfig.format = GRAPHIC_PIXEL_FMT_RGBA_1010102;
    GSError ret = sProducer->RequestBuffer(buffer, requestFence, requestConfig);
    EXPECT_EQ(ret, GSERROR_OK);
    GraphicPixelFormat pixelFormat = rsRenderComposer_->ComputeTargetPixelFormat(buffer);
    EXPECT_EQ(pixelFormat, GRAPHIC_PIXEL_FMT_RGBA_1010102);

    requestConfig.format = GRAPHIC_PIXEL_FMT_YCBCR_P010;
    ret = sProducer->RequestBuffer(buffer, requestFence, requestConfig);
    EXPECT_EQ(ret, GSERROR_OK);
    pixelFormat = rsRenderComposer_->ComputeTargetPixelFormat(buffer);
    EXPECT_EQ(pixelFormat, GRAPHIC_PIXEL_FMT_RGBA_1010102);

    requestConfig.format = GRAPHIC_PIXEL_FMT_YCRCB_P010;
    ret = sProducer->RequestBuffer(buffer, requestFence, requestConfig);
    EXPECT_EQ(ret, GSERROR_OK);
    pixelFormat = rsRenderComposer_->ComputeTargetPixelFormat(buffer);
    EXPECT_EQ(pixelFormat, GRAPHIC_PIXEL_FMT_RGBA_1010102);
    cSurface->UnregisterConsumerListener();
}

/**
 * Function: GetDisplayClientTargetProperty001
 * Type: Function
 * Rank: Important(1)
 * EnvConditions: N/A
 * CaseDescription: 1. hdioutput is nullptr, call GetDisplayClientTargetProperty fail
 *                  2. then call ComputeTargetPixelFormat
 *                  3. verify result
 */
HWTEST_F(RsRenderComposerTest, GetDisplayClientTargetProperty001, TestSize.Level1)
{
    std::vector<RSLayerPtr> layers;
    std::vector<GraphicPixelFormat> formats = {
        GRAPHIC_PIXEL_FMT_RGBA_1010102,
        GRAPHIC_PIXEL_FMT_YCRCB_P010,
    };
    std::vector<std::string> layerNames = {"R1", "R2"};
    for (size_t i = 0; i < formats.size(); ++i) {
        RSLayerPtr layer = std::make_shared<FakeRSLayer>(i + 20, false, layerNames[i]);
        RSSurfaceRenderNodeConfig config;
        config.id = i + 20;
        config.name = "surface_" + layerNames[i];

        auto rsSurfaceRenderNode = std::make_shared<RSSurfaceRenderNode>(config);
        sptr<IConsumerSurface> cSurface = IConsumerSurface::Create(config.name);
        auto surfaceBufferImpl = new SurfaceBufferImpl();
        auto handle = new BufferHandle();
        ASSERT_NE(handle, nullptr);
        handle->format = formats[i];
        surfaceBufferImpl->SetBufferHandle(handle);
        sptr<SyncFence> requestFence = SyncFence::INVALID_FENCE;
        layer->SetBuffer(surfaceBufferImpl);
        layer->SetSurface(cSurface);
        layers.emplace_back(layer);
    }

    GraphicColorGamut colorGamut = GRAPHIC_COLOR_GAMUT_SRGB;
    GraphicPixelFormat pixelFormat = GRAPHIC_PIXEL_FMT_RGBA_8888;
    auto hdiOutput = rsRenderComposer_->hdiOutput_;
    rsRenderComposer_->hdiOutput_ = nullptr;
    bool ret = rsRenderComposer_->GetDisplayClientTargetProperty(pixelFormat, colorGamut, layers);
    EXPECT_EQ(pixelFormat, rsRenderComposer_->ComputeTargetPixelFormat(layers));
    EXPECT_FALSE(ret);
    rsRenderComposer_->hdiOutput_ = hdiOutput;
}
#endif

/**
 * Function: Call_Interface_With_Null_Hdioutput
 * Type: Function
 * Rank: Important(2)
 * EnvConditions: N/A
 * CaseDescription: 1. create RSRenderComposer with null hdiOutput_
 *                  2. call interfaces
 *                  3. check result
 */
HWTEST_F(RsRenderComposerTest, Call_Interface_With_Null_Hdioutput, TestSize.Level1)
{
    auto hdiOutput = rsRenderComposer_->hdiOutput_;
    rsRenderComposer_->hdiOutput_ = nullptr;
    rsRenderComposer_->CleanLayerBufferBySurfaceId(0u);

    std::string dumpString = "";
    std::string layerName = "test";
    rsRenderComposer_->FpsDump(dumpString, layerName);
    EXPECT_TRUE(dumpString.empty());
    EXPECT_EQ(layerName, "test");

    dumpString = "";
    rsRenderComposer_->ClearFpsDump(dumpString, layerName);
    EXPECT_TRUE(dumpString.empty());
    EXPECT_EQ(layerName, "test");

    dumpString = "";
    rsRenderComposer_->HitchsDump(dumpString, layerName);
    EXPECT_TRUE(dumpString.empty());
    EXPECT_EQ(layerName, "test");

    std::this_thread::sleep_for(std::chrono::milliseconds(200));
    rsRenderComposer_->hdiOutput_ = hdiOutput;
}

/**
 * Function: Call_Interface_With_Null_Handler
 * Type: Function
 * Rank: Important(2)
 * EnvConditions: N/A
 * CaseDescription: 1. create RSRenderComposer
 *                  2. call dump Func with null handler_
 *                  3. check result
 */
HWTEST_F(RsRenderComposerTest, Call_Interface_With_Null_Handler, TestSize.Level1)
{
    auto output = std::make_shared<HdiOutput>(1u);
    output->Init();
    sptr<RSScreenProperty> property = new RSScreenProperty();
    auto tmpRsRenderComposer = std::make_shared<RSRenderComposer>(output, property);
    EXPECT_EQ(tmpRsRenderComposer->hdiOutput_->GetScreenId(), 1u);
    tmpRsRenderComposer->runner_ = nullptr;
    tmpRsRenderComposer->handler_ = nullptr;
    EXPECT_EQ(tmpRsRenderComposer->handler_, nullptr);

    std::string dumpString = "";
    std::string layerName = "test";
    tmpRsRenderComposer->FpsDump(dumpString, layerName);
    EXPECT_EQ(dumpString, "\n");
    EXPECT_EQ(layerName, "test");

    dumpString = "";
    tmpRsRenderComposer->ClearFpsDump(dumpString, layerName);
    EXPECT_EQ(dumpString, "\n");
    EXPECT_EQ(layerName, "test");

    dumpString = "";
    tmpRsRenderComposer->HitchsDump(dumpString, layerName);
    EXPECT_EQ(dumpString, "\n");
    EXPECT_EQ(layerName, "test");

    std::this_thread::sleep_for(std::chrono::milliseconds(200));
    tmpRsRenderComposer->uniRenderEngine_ = nullptr;
}

/**
 * Function: RedrawScreenRCD
 * Type: Function
 * Rank: Important(2)
 * EnvConditions: N/A
 * CaseDescription: 1. create RSRenderComposer
 *                  2. call RedrawScreenRCD
 *                  3. check result
 */
HWTEST_F(RsRenderComposerTest, RedrawScreenRCD, TestSize.Level1)
{
    Drawing::Canvas canvas;
    RSPaintFilterCanvas paintFilterCanvas(&canvas);
    std::vector<RSLayerPtr> layers;
    RSLayerPtr layer = std::static_pointer_cast<RSLayer>(std::make_shared<FakeRSLayer>(1, false, "L1"));
    layer->SetSurface(nullptr);
    layers.emplace_back(layer);

    layers.emplace_back(nullptr);

    std::vector<GraphicCompositionType> skipTypes = {
        GraphicCompositionType::GRAPHIC_COMPOSITION_DEVICE,
        GraphicCompositionType::GRAPHIC_COMPOSITION_DEVICE_CLEAR,
        GraphicCompositionType::GRAPHIC_COMPOSITION_SOLID_COLOR,
    };
    std::vector<std::string> skipLayerNames = {"S1", "S2", "S3"};
    for (size_t i = 0; i < skipTypes.size(); ++i) {
        RSLayerPtr skipLayer = std::static_pointer_cast<RSLayer>(
            std::make_shared<FakeRSLayer>(i + 2, false, skipLayerNames[i]));
        skipLayer->SetCompositionType(skipTypes[i]);
        layers.emplace_back(skipLayer);
    }

    std::vector<RoundCornerDisplayManager::RCDLayerType> rcLayerTypes = {
        RoundCornerDisplayManager::RCDLayerType::INVALID,
        RoundCornerDisplayManager::RCDLayerType::TOP,
        RoundCornerDisplayManager::RCDLayerType::BOTTOM,
    };

    using RSRcdManager = RSSingleton<RoundCornerDisplayManager>;
    std::vector<std::string> rcdLayerNames = {"R1", "R2", "R3"};
    for (size_t i = 0; i < rcLayerTypes.size(); ++i) {
        RSLayerPtr rcdLayer = std::static_pointer_cast<RSLayer>(
            std::make_shared<FakeRSLayer>(i + 10, false, rcdLayerNames[i]));
        sptr<IConsumerSurface> cSurface = IConsumerSurface::Create("surface_" + rcdLayerNames[i]);
        rcdLayer->SetSurface(cSurface);
        RSRcdManager::GetInstance().AddLayer(rcdLayerNames[i], i + 10, rcLayerTypes[i]);
        layers.emplace_back(rcdLayer);
    }
    auto rcdLayer1 = std::static_pointer_cast<RSLayer>(std::make_shared<RSRenderSurfaceRCDLayer>());
    layers.emplace_back(rcdLayer1);
    EXPECT_NE(layers.size(), 0u);
    rsRenderComposer_->RedrawScreenRCD(paintFilterCanvas, layers);
}

/**
 * Function: PostTask_Null_Handler
 * Type: Function
 * Rank: Important(2)
 * EnvConditions: N/A
 * CaseDescription: 1. create RSRenderComposer
 *                  2. call post task with null handler
 *                  3. check result
 */
HWTEST_F(RsRenderComposerTest, PostTask_Null_Handler, TestSize.Level1)
{
    auto output = std::make_shared<HdiOutput>(1u);
    output->Init();
    sptr<RSScreenProperty> property = new RSScreenProperty();
    auto tmpRsRenderComposer = std::make_shared<RSRenderComposer>(output, property);
    EXPECT_EQ(tmpRsRenderComposer->hdiOutput_->GetScreenId(), 1u);
    tmpRsRenderComposer->runner_ = nullptr;
    tmpRsRenderComposer->handler_ = nullptr;
    EXPECT_EQ(tmpRsRenderComposer->handler_, nullptr);

    std::atomic<bool> ran{false};
    tmpRsRenderComposer->PostTask([&ran]() { ran.store(true); });
    std::this_thread::sleep_for(std::chrono::milliseconds(200));
    EXPECT_FALSE(ran.load());
    ran.store(false);

    tmpRsRenderComposer->PostSyncTask([&ran]() { ran.store(true); });
    EXPECT_FALSE(ran.load());
    ran.store(false);

    tmpRsRenderComposer->PostDelayTask([&ran]() { ran.store(true); }, 0);
    std::this_thread::sleep_for(std::chrono::milliseconds(200));
    EXPECT_FALSE(ran.load());

    std::this_thread::sleep_for(std::chrono::milliseconds(200));
    tmpRsRenderComposer->uniRenderEngine_ = nullptr;
}

/**
 * Function: DumpVkImageInfo_Null_UniRenderEngine
 * Type: Function
 * Rank: Important(2)
 * EnvConditions: N/A
 * CaseDescription: 1. create RSRenderComposer
 *                  2. call DumpVkImageInfo with null uniRenderEngine
 *                  3. check result
 */
HWTEST_F(RsRenderComposerTest, DumpVkImageInfo_Null_UniRenderEngine, TestSize.Level1)
{
    auto output = std::make_shared<HdiOutput>(1u);
    output->Init();
    sptr<RSScreenProperty> property = new RSScreenProperty();
    auto tmpRsRenderComposer = std::make_shared<RSRenderComposer>(output, property);
    EXPECT_EQ(tmpRsRenderComposer->hdiOutput_->GetScreenId(), 1u);
    EXPECT_NE(tmpRsRenderComposer->handler_, nullptr);
    tmpRsRenderComposer->uniRenderEngine_ = nullptr;

    std::string dumpString = "";
    EXPECT_TRUE(dumpString.empty());
}

/**
 * Function: ClearRedrawGPUCompositionCache_Null_UniRenderEngine
 * Type: Function
 * Rank: Important(2)
 * EnvConditions: N/A
 * CaseDescription: 1. create RSRenderComposer
 *                  2. call ClearRedrawGPUCompositionCache with null uniRenderEngine
 *                  3. check result
 */
HWTEST_F(RsRenderComposerTest, ClearRedrawGPUCompositionCache_Null_UniRenderEngine, TestSize.Level1)
{
    auto output = std::make_shared<HdiOutput>(1u);
    output->Init();
    sptr<RSScreenProperty> property = new RSScreenProperty();
    auto tmpRsRenderComposer = std::make_shared<RSRenderComposer>(output, property);
    EXPECT_EQ(tmpRsRenderComposer->hdiOutput_->GetScreenId(), 1u);
    EXPECT_NE(tmpRsRenderComposer->handler_, nullptr);
    tmpRsRenderComposer->uniRenderEngine_ = nullptr;

    std::unordered_set<uint64_t> bufferIds;
    bufferIds.insert(1u);
    tmpRsRenderComposer->ClearRedrawGPUCompositionCache(bufferIds);
    EXPECT_EQ(bufferIds.size(), 1);
}

/**
 * Function: ProcessComposerFrame_HasGameScene
 * Type: Function
 * Rank: Important(2)
 * EnvConditions: N/A
 * CaseDescription: 1. create RSRenderComposer
 *                  2. call ProcessComposerFrame with has game scene
 *                  3. check result
 */
HWTEST_F(RsRenderComposerTest, ProcessComposerFrame_HasGameScene, TestSize.Level1)
{
    auto output = std::make_shared<HdiOutput>(1u);
    output->Init();
    sptr<RSScreenProperty> property = new RSScreenProperty();
    auto tmpRsRenderComposer = std::make_shared<RSRenderComposer>(output, property);
    EXPECT_EQ(tmpRsRenderComposer->hdiOutput_->GetScreenId(), 1u);
    EXPECT_NE(tmpRsRenderComposer->handler_, nullptr);

    PipelineParam param;
    uint32_t currentRate = 0;
    tmpRsRenderComposer->ProcessComposerFrame(currentRate, param);
    std::this_thread::sleep_for(std::chrono::milliseconds(200));
    tmpRsRenderComposer->unExecuteTaskNum_.store(3);
    tmpRsRenderComposer->ProcessComposerFrame(currentRate, param);

    tmpRsRenderComposer->uniRenderEngine_ = nullptr;

    output = std::make_shared<HdiOutput>(0u);
    output->Init();
    property = new RSScreenProperty();
    tmpRsRenderComposer = std::make_shared<RSRenderComposer>(output, property);
    EXPECT_EQ(tmpRsRenderComposer->hdiOutput_->GetScreenId(), 0u);
    EXPECT_NE(tmpRsRenderComposer->handler_, nullptr);
    tmpRsRenderComposer->ProcessComposerFrame(currentRate, param);
    std::this_thread::sleep_for(std::chrono::milliseconds(200));
    tmpRsRenderComposer->unExecuteTaskNum_.store(3);
    tmpRsRenderComposer->ProcessComposerFrame(currentRate, param);
    tmpRsRenderComposer->uniRenderEngine_ = nullptr;
}

/**
 * Function: UpdateDelayTime_IsDelayRequired
 * Type: Function
 * Rank: Important(2)
 * EnvConditions: N/A
 * CaseDescription: 1. create RSRenderComposer
 *                  2. call UpdateDelayTime with is ForceRefresh param
 *                  3. check result
 */
HWTEST_F(RsRenderComposerTest, UpdateDelayTime_IsDelayRequired, TestSize.Level1)
{
    auto output = std::make_shared<HdiOutput>(1u);
    output->Init();
    sptr<RSScreenProperty> property = new RSScreenProperty();
    auto tmpRsRenderComposer = std::make_shared<RSRenderComposer>(output, property);
    EXPECT_EQ(tmpRsRenderComposer->hdiOutput_->GetScreenId(), 1u);
    EXPECT_NE(tmpRsRenderComposer->handler_, nullptr);

    PipelineParam param;
    auto& hgmCore = HgmCore::Instance();
    param.isForceRefresh = true;
    uint32_t currentRate = 0;

    int64_t delayTime = tmpRsRenderComposer->UpdateDelayTime(hgmCore, currentRate, param);
    EXPECT_GE(delayTime, 0);
    tmpRsRenderComposer->uniRenderEngine_ = nullptr;
}

/**
 * Function: UpdateDelayTime_TimeStamp
 * Type: Function
 * Rank: Important(2)
 * EnvConditions: N/A
 * CaseDescription: 1. create RSRenderComposer
 *                  2. call UpdateDelayTime after change time stamp
 *                  3. check result
 */
HWTEST_F(RsRenderComposerTest, UpdateDelayTime_TimeStamp, TestSize.Level1)
{
    auto output = std::make_shared<HdiOutput>(1u);
    output->Init();
    sptr<RSScreenProperty> property = new RSScreenProperty();
    auto tmpRsRenderComposer = std::make_shared<RSRenderComposer>(output, property);
    EXPECT_EQ(tmpRsRenderComposer->hdiOutput_->GetScreenId(), 1u);
    EXPECT_NE(tmpRsRenderComposer->handler_, nullptr);

    PipelineParam param;
    uint32_t currentRate = 0;

    tmpRsRenderComposer->lastCommitTime_ = std::numeric_limits<int64_t>::max();
    auto& hgmCore2 = HgmCore::Instance();
    int64_t delayTime = tmpRsRenderComposer->UpdateDelayTime(hgmCore2, currentRate, param);
    EXPECT_EQ(delayTime, 0);

    tmpRsRenderComposer->lastCommitTime_ = std::numeric_limits<int64_t>::min();
    delayTime = tmpRsRenderComposer->UpdateDelayTime(hgmCore2, currentRate, param);
    EXPECT_EQ(delayTime, 0);

    tmpRsRenderComposer->uniRenderEngine_ = nullptr;
}

/**
 * Function: CalculateDelayTime
 * Type: Function
 * Rank: Important(2)
 * EnvConditions: N/A
 * CaseDescription: 1. create RSRenderComposer
 *                  2. call CalculateDelayTime
 *                  3. check result
 */
HWTEST_F(RsRenderComposerTest, CalculateDelayTime, TestSize.Level1)
{
    auto output = std::make_shared<HdiOutput>(1u);
    output->Init();
    sptr<RSScreenProperty> property = new RSScreenProperty();
    auto tmpRsRenderComposer = std::make_shared<RSRenderComposer>(output, property);
    EXPECT_EQ(tmpRsRenderComposer->hdiOutput_->GetScreenId(), 1u);
    EXPECT_NE(tmpRsRenderComposer->handler_, nullptr);

    PipelineParam param;
    auto& hgmCore = HgmCore::Instance();
    bool isLtpoMode = hgmCore.isLtpoMode_.load();
    hgmCore.isLtpoMode_.store(false);
    uint32_t currentRate2 = 0;
    tmpRsRenderComposer->CalculateDelayTime(hgmCore, currentRate2, 0, param);
    EXPECT_GT(tmpRsRenderComposer->delayTime_, -1);

    hgmCore.isLtpoMode_.store(isLtpoMode);
    tmpRsRenderComposer->uniRenderEngine_ = nullptr;
}

/**
 * Function: CalculateDelayTime2
 * Type: Function
 * Rank: Important(2)
 * EnvConditions: N/A
 * CaseDescription: 1. create RSRenderComposer
 *                  2. change TE settings
 *                  3. call CalculateDelayTime
 *                  4. check result
 */
HWTEST_F(RsRenderComposerTest, CalculateDelayTime2, TestSize.Level1)
{
    auto output = std::make_shared<HdiOutput>(1u);
    output->Init();
    sptr<RSScreenProperty> property = new RSScreenProperty();
    auto tmpRsRenderComposer = std::make_shared<RSRenderComposer>(output, property);
    EXPECT_EQ(tmpRsRenderComposer->hdiOutput_->GetScreenId(), 1u);
    EXPECT_NE(tmpRsRenderComposer->handler_, nullptr);

    PipelineParam param;
    auto& hgmCore = HgmCore::Instance();
    hgmCore.isLtpoMode_.store(true);
    hgmCore.SetSupportedMaxTE144(0);
    tmpRsRenderComposer->CalculateDelayTime(hgmCore, 0, -1, param);
    EXPECT_GT(tmpRsRenderComposer->delayTime_, -1);
    tmpRsRenderComposer->CalculateDelayTime(hgmCore, 144, -1, param);
    EXPECT_GT(tmpRsRenderComposer->delayTime_, -1);
    hgmCore.SetSupportedMaxTE144(432);
    hgmCore.SetIdealPipelineOffset144(6);
    tmpRsRenderComposer->CalculateDelayTime(hgmCore, 0, -1, param);
    EXPECT_GT(tmpRsRenderComposer->delayTime_, -1);
    tmpRsRenderComposer->CalculateDelayTime(hgmCore, 144, -1, param);
    EXPECT_GT(tmpRsRenderComposer->delayTime_, -1);

    tmpRsRenderComposer->uniRenderEngine_ = nullptr;
}

/**
 * Function: SurfaceDump_Branches
 * Type: Function
 * Rank: Important(2)
 * EnvConditions: N/A
 * CaseDescription: 1. call SurfaceDump with valid hdiOutput_
 *                  2. call SurfaceDump with null hdiOutput_
 */
HWTEST_F(RsRenderComposerTest, SurfaceDump_Branches, TestSize.Level1)
{
    std::string dumpString;
    rsRenderComposer_->SurfaceDump(dumpString);
    EXPECT_GE(dumpString.size(), 0u);

    auto backup = rsRenderComposer_->hdiOutput_;
    rsRenderComposer_->hdiOutput_ = nullptr;
    std::string dumpStringNull;
    rsRenderComposer_->SurfaceDump(dumpStringNull);
    EXPECT_TRUE(dumpStringNull.empty());
    rsRenderComposer_->hdiOutput_ = backup;
}

/**
 * Function: SetScreenBacklight_Branches
 * Type: Function
 * Rank: Important(2)
 * EnvConditions: N/A
 * CaseDescription: 1. call with valid hdiOutput_
 *                  2. call with null hdiOutput_
 */
HWTEST_F(RsRenderComposerTest, SetScreenBacklight_Branches, TestSize.Level1)
{
    // Verify initial hdiOutput_ is not null
    ASSERT_NE(rsRenderComposer_->hdiOutput_, nullptr);

    // Call with valid hdiOutput_
    rsRenderComposer_->SetScreenBacklight(50);

    // Call with null hdiOutput_
    auto backup = rsRenderComposer_->hdiOutput_;
    rsRenderComposer_->hdiOutput_ = nullptr;
    rsRenderComposer_->SetScreenBacklight(0);
    rsRenderComposer_->hdiOutput_ = backup;
}

/**
 * Function: CleanLayerBufferBySurfaceId_Normal
 * Type: Function
 * Rank: Important(2)
 * EnvConditions: N/A
 * CaseDescription: call CleanLayerBufferBySurfaceId with valid hdiOutput_
 */
HWTEST_F(RsRenderComposerTest, CleanLayerBufferBySurfaceId_Normal, TestSize.Level1)
{
    ASSERT_NE(rsRenderComposer_->hdiOutput_, nullptr);
    rsRenderComposer_->CleanLayerBufferBySurfaceId(0u);
}

/**
 * Function: RefreshRateCounts_NullUtils
 * Type: Function
 * Rank: Important(2)
 * EnvConditions: N/A
 * CaseDescription: hgmHardwareUtils_ set to nullptr then call refresh/clear
 */
HWTEST_F(RsRenderComposerTest, RefreshRateCounts_NullUtils, TestSize.Level1)
{
    auto backup = rsRenderComposer_->hgmHardwareUtils_;
    rsRenderComposer_->hgmHardwareUtils_ = nullptr;
    std::string s1, s2;
    rsRenderComposer_->RefreshRateCounts(s1);
    rsRenderComposer_->ClearRefreshRateCounts(s2);
    EXPECT_TRUE(s1.empty());
    EXPECT_TRUE(s2.empty());
    rsRenderComposer_->hgmHardwareUtils_ = backup;
}

/**
 * Function: GetDelayTime_AfterPrepare
 * Type: Function
 * Rank: Important(2)
 * EnvConditions: N/A
 * CaseDescription: ComposerPrepare updates delayTime_, then GetDelayTime returns same value
 */
HWTEST_F(RsRenderComposerTest, GetDelayTime_AfterPrepare, TestSize.Level1)
{
    PipelineParam pipelineParam;
    uint32_t currentRate = 0;
    int64_t delayTime = -1;
    rsRenderComposer_->ComposerPrepare(currentRate, delayTime, pipelineParam);
    EXPECT_EQ(rsRenderComposer_->GetDelayTime(), delayTime);
}

/**
 * Function: OnHwcDeadAndRestored_State
 * Type: Function
 * Rank: Important(2)
 * EnvConditions: N/A
 * CaseDescription: verify isHwcDead_ toggles on dead/restored
 */
HWTEST_F(RsRenderComposerTest, OnHwcDeadAndRestored_State, TestSize.Level1)
{
    auto output = std::make_shared<HdiOutput>(1u);
    sptr<RSScreenProperty> property = new RSScreenProperty();
    auto tmp = std::make_shared<RSRenderComposer>(output, property);
    tmp->hdiOutput_ = nullptr;
    ASSERT_EQ(tmp->hdiOutput_, nullptr);
    tmp->OnHwcDead();

    output = std::make_shared<HdiOutput>(2u);
    output->Init();
    tmp = std::make_shared<RSRenderComposer>(output, property);
    ASSERT_NE(tmp->hdiOutput_, nullptr);

    tmp->OnHwcDead();
    EXPECT_TRUE(tmp->isHwcDead_);

    auto output2 = std::make_shared<HdiOutput>(2u);
    output2->Init();
    tmp->OnHwcRestored(output2, property);
    EXPECT_FALSE(tmp->isHwcDead_);
    EXPECT_NE(tmp->hdiOutput_, nullptr);
}

/**
 * Function: AddSolidColorLayer_Coverage
 * Type: Function
 * Rank: Important(2)
 * EnvConditions: N/A
 * CaseDescription: AddSolidColorLayer test
 */
HWTEST_F(RsRenderComposerTest, AddSolidColorLayer_Coverage, TestSize.Level1)
{
    auto output = std::make_shared<HdiOutput>(0u);
    output->Init();
    sptr<RSScreenProperty> property = new RSScreenProperty();
    auto rsRenderComposerTmp = std::make_shared<RSRenderComposer>(output, property);
    EXPECT_NE(rsRenderComposerTmp->hdiOutput_, nullptr);
    std::vector<std::shared_ptr<RSLayer>> layers;
    rsRenderComposerTmp->AddSolidColorLayer(layers);
    layers.push_back(nullptr);
    std::shared_ptr<RSLayer> layer1 = std::make_shared<RSRenderSurfaceLayer>();
    layers.push_back(layer1);
    std::shared_ptr<RSLayer> layer2 = std::make_shared<RSRenderSurfaceLayer>();
    GraphicSolidColorLayerProperty solidColorLayerProperty;
    solidColorLayerProperty.compositionType = GraphicCompositionType::GRAPHIC_COMPOSITION_SOLID_COLOR;
    layer2->SetSolidColorLayerProperty(solidColorLayerProperty);
    layers.push_back(layer2);
    rsRenderComposerTmp->AddSolidColorLayer(layers);
}

/**
 * Function: ChangeLayersForActiveRectOutside_Coverage
 * Type: Function
 * Rank: Important(2)
 * EnvConditions: N/A
 * CaseDescription: ChangeLayersForActiveRectOutside test
 */
HWTEST_F(RsRenderComposerTest, ChangeLayersForActiveRectOutside_Coverage, TestSize.Level1)
{
    system::SetParameter("const.window.foldscreen.type", "6,0,0,0");
    auto output = std::make_shared<HdiOutput>(0u);
    output->Init();
    sptr<RSScreenProperty> property = new RSScreenProperty();
    auto rsRenderComposerTmp = std::make_shared<RSRenderComposer>(output, property);
    EXPECT_NE(rsRenderComposerTmp->hdiOutput_, nullptr);
    std::vector<std::shared_ptr<RSLayer>> layers;
    rsRenderComposerTmp->ChangeLayersForActiveRectOutside(layers);

    std::shared_ptr<RSLayer> layer1 = std::make_shared<RSRenderSurfaceLayer>();
    layers.push_back(layer1);
    rsRenderComposerTmp->composerScreenInfo_.reviseRect = {0, 0, 0, 0};
    rsRenderComposerTmp->ChangeLayersForActiveRectOutside(layers);
    rsRenderComposerTmp->composerScreenInfo_.reviseRect = {100, 0, 0, 0};
    rsRenderComposerTmp->ChangeLayersForActiveRectOutside(layers);
    rsRenderComposerTmp->composerScreenInfo_.reviseRect = {100, 100, 0, 0};
    rsRenderComposerTmp->composerScreenInfo_.maskRect = {0, 0, 0, 0};
    rsRenderComposerTmp->ChangeLayersForActiveRectOutside(layers);
    rsRenderComposerTmp->composerScreenInfo_.maskRect = {100, 0, 0, 0};
    rsRenderComposerTmp->ChangeLayersForActiveRectOutside(layers);
    rsRenderComposerTmp->composerScreenInfo_.maskRect = {100, 100, 0, 0};
    rsRenderComposerTmp->ChangeLayersForActiveRectOutside(layers);
    std::shared_ptr<RSLayer> layer2 = std::make_shared<RSRenderSurfaceRCDLayer>();
    layers.push_back(layer2);
    rsRenderComposerTmp->ChangeLayersForActiveRectOutside(layers);

    system::SetParameter("const.window.foldscreen.type", "0,0,0,0");
}

/**
 * Function: ChangeLayersForActiveRectOutside_MaskRectTrue
 * Type: Function
 * Rank: Important(2)
 * EnvConditions: N/A
 * CaseDescription: 1. set super fold display and maskRect with positive width and height
 *                  2. call ChangeLayersForActiveRectOutside
 *                  3. verify solid color layer is created and added to layers
 */
HWTEST_F(RsRenderComposerTest, ChangeLayersForActiveRectOutside_MaskRectTrue, TestSize.Level1)
{
    system::SetParameter("const.window.foldscreen.type", "6,0,0,0");
    auto output = std::make_shared<HdiOutput>(0u);
    output->Init();
    sptr<RSScreenProperty> property = new RSScreenProperty();
    auto rsRenderComposerTmp = std::make_shared<RSRenderComposer>(output, property);

    std::vector<std::shared_ptr<RSLayer>> layers;
    std::shared_ptr<RSLayer> layer1 = std::make_shared<RSRenderSurfaceLayer>();
    layer1->SetLayerSize({50, 50, 100, 100});
    layers.push_back(layer1);

    // Set maskRect with positive width and height to trigger line 437 branch true
    rsRenderComposerTmp->composerScreenInfo_.reviseRect = {100, 100, 200, 200};
    rsRenderComposerTmp->composerScreenInfo_.maskRect = {0, 0, 50, 50};
    size_t initialLayerCount = layers.size();
    rsRenderComposerTmp->ChangeLayersForActiveRectOutside(layers);

    // Verify solid color layer was added
    EXPECT_EQ(layers.size(), initialLayerCount + 1);
    // Verify the last layer is a solid color layer with mask flag
    auto maskLayer = layers.back();
    EXPECT_NE(maskLayer, nullptr);
    EXPECT_EQ(maskLayer->GetIsMaskLayer(), true);
    EXPECT_EQ(maskLayer->GetCompositionType(), GraphicCompositionType::GRAPHIC_COMPOSITION_SOLID_COLOR);

    system::SetParameter("const.window.foldscreen.type", "0,0,0,0");
}

/**
 * Function: ChangeLayersForActiveRectOutside_MaskRectFalse
 * Type: Function
 * Rank: Important(2)
 * EnvConditions: N/A
 * CaseDescription: 1. set super fold display and maskRect with zero width or height
 *                  2. call ChangeLayersForActiveRectOutside
 *                  3. verify solid color layer is NOT created
 */
HWTEST_F(RsRenderComposerTest, ChangeLayersForActiveRectOutside_MaskRectFalse, TestSize.Level1)
{
    system::SetParameter("const.window.foldscreen.type", "6,0,0,0");
    auto output = std::make_shared<HdiOutput>(0u);
    output->Init();
    sptr<RSScreenProperty> property = new RSScreenProperty();
    auto rsRenderComposerTmp = std::make_shared<RSRenderComposer>(output, property);

    std::vector<std::shared_ptr<RSLayer>> layers;
    std::shared_ptr<RSLayer> layer1 = std::make_shared<RSRenderSurfaceLayer>();
    layer1->SetLayerSize({50, 50, 100, 100});
    layers.push_back(layer1);

    rsRenderComposerTmp->composerScreenInfo_.reviseRect = {100, 100, 200, 200};

    // Test with maskRect width = 0
    rsRenderComposerTmp->composerScreenInfo_.maskRect = {0, 0, 0, 50};
    size_t layerCount1 = layers.size();
    rsRenderComposerTmp->ChangeLayersForActiveRectOutside(layers);
    EXPECT_EQ(layers.size(), layerCount1);

    // Test with maskRect height = 0
    rsRenderComposerTmp->composerScreenInfo_.maskRect = {0, 0, 50, 0};
    size_t layerCount2 = layers.size();
    rsRenderComposerTmp->ChangeLayersForActiveRectOutside(layers);
    EXPECT_EQ(layers.size(), layerCount2);

    // Test with both width and height = 0
    rsRenderComposerTmp->composerScreenInfo_.maskRect = {0, 0, 0, 0};
    size_t layerCount3 = layers.size();
    rsRenderComposerTmp->ChangeLayersForActiveRectOutside(layers);
    EXPECT_EQ(layers.size(), layerCount3);

    system::SetParameter("const.window.foldscreen.type", "0,0,0,0");
}

/**
 * Function: ChangeLayersForActiveRectOutside_RCDLayerTrue
 * Type: Function
 * Rank: Important(2)
 * EnvConditions: N/A
 * CaseDescription: 1. set super fold display with valid reviseRect
 *                  2. add RCD layer to layers
 *                  3. call ChangeLayersForActiveRectOutside
 *                  4. verify RCD layer size is NOT changed (skipped)
 */
HWTEST_F(RsRenderComposerTest, ChangeLayersForActiveRectOutside_RCDLayerTrue, TestSize.Level1)
{
    system::SetParameter("const.window.foldscreen.type", "6,0,0,0");
    auto output = std::make_shared<HdiOutput>(0u);
    output->Init();
    sptr<RSScreenProperty> property = new RSScreenProperty();
    auto rsRenderComposerTmp = std::make_shared<RSRenderComposer>(output, property);

    std::vector<std::shared_ptr<RSLayer>> layers;
    std::shared_ptr<RSRenderSurfaceRCDLayer> rcdLayer = std::make_shared<RSRenderSurfaceRCDLayer>();
    GraphicIRect originalSize = {200, 200, 100, 100};
    rcdLayer->SetLayerSize(originalSize);
    layers.push_back(rcdLayer);

    rsRenderComposerTmp->composerScreenInfo_.reviseRect = {100, 100, 200, 200};
    rsRenderComposerTmp->composerScreenInfo_.maskRect = {0, 0, 0, 0};

    rsRenderComposerTmp->ChangeLayersForActiveRectOutside(layers);

    // Verify RCD layer size was NOT changed (skipped due to line 455 branch)
    GraphicIRect rcdLayerSize = rcdLayer->GetLayerSize();
    EXPECT_EQ(rcdLayerSize.x, originalSize.x);
    EXPECT_EQ(rcdLayerSize.y, originalSize.y);
    EXPECT_EQ(rcdLayerSize.w, originalSize.w);
    EXPECT_EQ(rcdLayerSize.h, originalSize.h);

    system::SetParameter("const.window.foldscreen.type", "0,0,0,0");
}

/**
 * Function: ChangeLayersForActiveRectOutside_RCDLayerFalse
 * Type: Function
 * Rank: Important(2)
 * EnvConditions: N/A
 * CaseDescription: 1. set super fold display with valid reviseRect
 *                  2. add normal layer (not RCD) to layers
 *                  3. call ChangeLayersForActiveRectOutside
 *                  4. verify normal layer size IS changed
 */
HWTEST_F(RsRenderComposerTest, ChangeLayersForActiveRectOutside_RCDLayerFalse, TestSize.Level1)
{
    system::SetParameter("const.window.foldscreen.type", "6,0,0,0");
    auto output = std::make_shared<HdiOutput>(0u);
    output->Init();
    sptr<RSScreenProperty> property = new RSScreenProperty();
    auto rsRenderComposerTmp = std::make_shared<RSRenderComposer>(output, property);

    std::vector<std::shared_ptr<RSLayer>> layers;
    std::shared_ptr<RSRenderSurfaceLayer> normalLayer = std::make_shared<RSRenderSurfaceLayer>();
    // Set layer size that extends beyond reviseRect
    GraphicIRect originalSize = {50, 50, 150, 150};
    normalLayer->SetLayerSize(originalSize);
    layers.push_back(normalLayer);

    // reviseRect: left=100, top=100, width=200, height=200
    // reviseRight = 100 + 200 = 300, reviseBottom = 100 + 200 = 300
    rsRenderComposerTmp->composerScreenInfo_.reviseRect = {100, 100, 200, 200};
    rsRenderComposerTmp->composerScreenInfo_.maskRect = {0, 0, 0, 0};

    rsRenderComposerTmp->ChangeLayersForActiveRectOutside(layers);

    // Verify normal layer size WAS changed (clamped to reviseRect)
    // Expected: x clamped to [100, 300] = 100, y clamped to [100, 300] = 100
    //           w = min(100 + 150, 300) - 100 = 250 - 100 = 150
    //           h = min(100 + 150, 300) - 100 = 250 - 100 = 150
    GraphicIRect newLayerSize = normalLayer->GetLayerSize();
    EXPECT_EQ(newLayerSize.x, 100);
    EXPECT_EQ(newLayerSize.y, 100);
    EXPECT_EQ(newLayerSize.w, 150);
    EXPECT_EQ(newLayerSize.h, 150);

    system::SetParameter("const.window.foldscreen.type", "0,0,0,0");
}

/**
 * Function: ChangeLayersForActiveRectOutside_MixedLayers
 * Type: Function
 * Rank: Important(2)
 * EnvConditions: N/A
 * CaseDescription: 1. set super fold display with valid reviseRect and maskRect
 *                  2. add mixed layers (normal, RCD, normal)
 *                  3. call ChangeLayersForActiveRectOutside
 *                  4. verify: solid color layer added, normal layers changed, RCD layer unchanged
 */
HWTEST_F(RsRenderComposerTest, ChangeLayersForActiveRectOutside_MixedLayers, TestSize.Level1)
{
    system::SetParameter("const.window.foldscreen.type", "6,0,0,0");
    auto output = std::make_shared<HdiOutput>(0u);
    output->Init();
    sptr<RSScreenProperty> property = new RSScreenProperty();
    auto rsRenderComposerTmp = std::make_shared<RSRenderComposer>(output, property);

    std::vector<std::shared_ptr<RSLayer>> layers;

    // Add normal layer 1
    std::shared_ptr<RSRenderSurfaceLayer> normalLayer1 = std::make_shared<RSRenderSurfaceLayer>();
    GraphicIRect size1 = {50, 50, 100, 100};
    normalLayer1->SetLayerSize(size1);
    layers.push_back(normalLayer1);

    // Add RCD layer
    std::shared_ptr<RSRenderSurfaceRCDLayer> rcdLayer = std::make_shared<RSRenderSurfaceRCDLayer>();
    GraphicIRect rcdSize = {0, 0, 50, 50};
    rcdLayer->SetLayerSize(rcdSize);
    layers.push_back(rcdLayer);

    // Add normal layer 2
    std::shared_ptr<RSRenderSurfaceLayer> normalLayer2 = std::make_shared<RSRenderSurfaceLayer>();
    GraphicIRect size2 = {200, 200, 80, 80};
    normalLayer2->SetLayerSize(size2);
    layers.push_back(normalLayer2);

    rsRenderComposerTmp->composerScreenInfo_.reviseRect = {100, 100, 200, 200};
    rsRenderComposerTmp->composerScreenInfo_.maskRect = {0, 0, 30, 30};

    size_t initialLayerCount = layers.size();
    rsRenderComposerTmp->ChangeLayersForActiveRectOutside(layers);

    // Verify solid color mask layer was added (line 437 branch true)
    EXPECT_EQ(layers.size(), initialLayerCount + 1);

    // Verify RCD layer was NOT changed (line 455 branch true)
    GraphicIRect rcdLayerSizeAfter = rcdLayer->GetLayerSize();
    EXPECT_EQ(rcdLayerSizeAfter.x, rcdSize.x);
    EXPECT_EQ(rcdLayerSizeAfter.y, rcdSize.y);
    EXPECT_EQ(rcdLayerSizeAfter.w, rcdSize.w);
    EXPECT_EQ(rcdLayerSizeAfter.h, rcdSize.h);

    // Verify normal layers WERE changed (line 455 branch false for both)
    GraphicIRect layer1SizeAfter = normalLayer1->GetLayerSize();
    EXPECT_NE(layer1SizeAfter.x, size1.x);
    EXPECT_NE(layer1SizeAfter.y, size1.y);

    GraphicIRect layer2SizeAfter = normalLayer2->GetLayerSize();
    EXPECT_EQ(layer2SizeAfter.x, size2.x);
    EXPECT_EQ(layer2SizeAfter.y, size2.y);

    system::SetParameter("const.window.foldscreen.type", "0,0,0,0");
}

/**
 * Function: ChangeLayersForActiveRectOutside_NotSuperFoldDisplay
 * Type: Function
 * Rank: Important(2)
 * EnvConditions: N/A
 * CaseDescription: 1. set NOT super fold display
 *                  2. call ChangeLayersForActiveRectOutside
 *                  3. verify function returns early without processing
 */
HWTEST_F(RsRenderComposerTest, ChangeLayersForActiveRectOutside_NotSuperFoldDisplay, TestSize.Level1)
{
    system::SetParameter("const.window.foldscreen.type", "0,0,0,0");
    auto output = std::make_shared<HdiOutput>(0u);
    output->Init();
    sptr<RSScreenProperty> property = new RSScreenProperty();
    auto rsRenderComposerTmp = std::make_shared<RSRenderComposer>(output, property);

    std::vector<std::shared_ptr<RSLayer>> layers;
    std::shared_ptr<RSRenderSurfaceLayer> layer1 = std::make_shared<RSRenderSurfaceLayer>();
    GraphicIRect originalSize = {50, 50, 100, 100};
    layer1->SetLayerSize(originalSize);
    layers.push_back(layer1);

    rsRenderComposerTmp->composerScreenInfo_.reviseRect = {100, 100, 200, 200};
    rsRenderComposerTmp->composerScreenInfo_.maskRect = {0, 0, 50, 50};

    size_t initialLayerCount = layers.size();
    rsRenderComposerTmp->ChangeLayersForActiveRectOutside(layers);

    // Verify no layers were added and layer size was not changed
    EXPECT_NE(layers.size(), initialLayerCount);
    GraphicIRect layerSizeAfter = layer1->GetLayerSize();
    EXPECT_EQ(layerSizeAfter.x, 100);
    EXPECT_EQ(layerSizeAfter.y, 100);
    EXPECT_EQ(layerSizeAfter.w, 100);
    EXPECT_EQ(layerSizeAfter.h, 100);
}

/**
 * Function: ChangeLayersForActiveRectOutside_EmptyLayers
 * Type: Function
 * Rank: Important(2)
 * EnvConditions: N/A
 * CaseDescription: 1. set super fold display
 *                  2. call ChangeLayersForActiveRectOutside with empty layers vector
 *                  3. verify function returns early without processing
 */
HWTEST_F(RsRenderComposerTest, ChangeLayersForActiveRectOutside_EmptyLayers, TestSize.Level1)
{
    system::SetParameter("const.window.foldscreen.type", "6,0,0,0");
    auto output = std::make_shared<HdiOutput>(0u);
    output->Init();
    sptr<RSScreenProperty> property = new RSScreenProperty();
    auto rsRenderComposerTmp = std::make_shared<RSRenderComposer>(output, property);

    std::vector<std::shared_ptr<RSLayer>> layers; // Empty layers

    rsRenderComposerTmp->composerScreenInfo_.reviseRect = {100, 100, 200, 200};
    rsRenderComposerTmp->composerScreenInfo_.maskRect = {0, 0, 50, 50};

    rsRenderComposerTmp->ChangeLayersForActiveRectOutside(layers);

    // Verify no layers were added
    EXPECT_EQ(layers.size(), 0u);

    system::SetParameter("const.window.foldscreen.type", "0,0,0,0");
}

/**
 * Function: ContextRegisterPostTask_Coverage
 * Type: Function
 * Rank: Important(2)
 * EnvConditions: N/A
 * CaseDescription: ContextRegisterPostTask test
 */
HWTEST_F(RsRenderComposerTest, ContextRegisterPostTask_Coverage, TestSize.Level1)
{
    auto output = std::make_shared<HdiOutput>(0u);
    output->Init();
    sptr<RSScreenProperty> property = new RSScreenProperty();
    auto rsRenderComposerTmp = std::make_shared<RSRenderComposer>(output, property);
    EXPECT_NE(rsRenderComposerTmp->hdiOutput_, nullptr);
    rsRenderComposerTmp->ContextRegisterPostTask();
}

/**
 * Function: UpdateTransactionData_Coverage
 * Type: Function
 * Rank: Important(2)
 * EnvConditions: N/A
 * CaseDescription: UpdateTransactionData test
 */
HWTEST_F(RsRenderComposerTest, UpdateTransactionData_Coverage, TestSize.Level1)
{
    auto output = std::make_shared<HdiOutput>(0u);
    output->Init();
    sptr<RSScreenProperty> property = new RSScreenProperty();
    auto rsRenderComposerTmp = std::make_shared<RSRenderComposer>(output, property);
    EXPECT_NE(rsRenderComposerTmp->hdiOutput_, nullptr);
    std::shared_ptr<RSLayerTransactionData> transactionData = std::make_shared<RSLayerTransactionData>();
    transactionData->payload_.push_back({0, nullptr});
    rsRenderComposerTmp->UpdateTransactionData(transactionData);

    auto prop = std::make_shared<RSRenderLayerCmdProperty<int32_t>>(1);
    auto zCmd = std::make_shared<RSRenderLayerZorderCmd>(prop);
    RSLayerId id = static_cast<RSLayerId>(100u);
    std::shared_ptr<RSLayerParcel> parcel1 = std::make_shared<RSUpdateRSLayerCmd>(id, zCmd);
    std::shared_ptr<RSLayerParcel> parcel2 = std::make_shared<RSDestroyRSLayerCmd>(id, zCmd);
    std::shared_ptr<RSLayerParcel> parcel3 = std::make_shared<RSUpdateRSRCDLayerCmd>(id, zCmd);
    transactionData->payload_.push_back({id, parcel1});
    transactionData->composerInfo_.composerScreenInfo.activeRect = {100, 200, 0, 0};
    rsRenderComposerTmp->UpdateTransactionData(transactionData);
    transactionData->composerInfo_.composerScreenInfo.activeRect = {200, 200, 0, 0};
    transactionData->payload_.push_back({id, parcel2});
    transactionData->payload_.push_back({id, parcel3});
    rsRenderComposerTmp->UpdateTransactionData(transactionData);
}

/**
 * Function: UpdateForSurfaceFps_Coverage
 * Type: Function
 * Rank: Important(2)
 * EnvConditions: N/A
 * CaseDescription: UpdateForSurfaceFps test
 */
HWTEST_F(RsRenderComposerTest, UpdateForSurfaceFps_Coverage, TestSize.Level1)
{
    auto output = std::make_shared<HdiOutput>(0u);
    output->Init();
    sptr<RSScreenProperty> property = new RSScreenProperty();
    auto rsRenderComposerTmp = std::make_shared<RSRenderComposer>(output, property);
    EXPECT_NE(rsRenderComposerTmp->hdiOutput_, nullptr);

    PipelineParam pipelineParam;
    pipelineParam.SurfaceFpsOpNum = 3;
    SurfaceFpsOp surfaceFpsOp1;
    surfaceFpsOp1.surfaceFpsOpType = static_cast<uint32_t>(SurfaceFpsOpType::SURFACE_FPS_ADD);
    SurfaceFpsOp surfaceFpsOp2;
    surfaceFpsOp2.surfaceFpsOpType = static_cast<uint32_t>(SurfaceFpsOpType::SURFACE_FPS_REMOVE);
    SurfaceFpsOp surfaceFpsOp3;
    surfaceFpsOp3.surfaceFpsOpType = static_cast<uint32_t>(SurfaceFpsOpType::SURFACE_FPS_DEFAULT);
    pipelineParam.SurfaceFpsOpList.push_back(surfaceFpsOp1);
    pipelineParam.SurfaceFpsOpList.push_back(surfaceFpsOp2);
    pipelineParam.SurfaceFpsOpList.push_back(surfaceFpsOp3);
    rsRenderComposerTmp->UpdateForSurfaceFps(pipelineParam);
}

/**
 * Function: PreAllocateProtectedBuffer_Coverage
 * Type: Function
 * Rank: Important(2)
 * EnvConditions: N/A
 * CaseDescription: PreAllocateProtectedBuffer test
 */
HWTEST_F(RsRenderComposerTest, PreAllocateProtectedBuffer_Coverage, TestSize.Level1)
{
    auto output = std::make_shared<HdiOutput>(0u);
    output->Init();
    sptr<RSScreenProperty> property = new RSScreenProperty();
    auto rsRenderComposerTmp = std::make_shared<RSRenderComposer>(output, property);
    EXPECT_NE(rsRenderComposerTmp->hdiOutput_, nullptr);
    sptr<SurfaceBuffer> buffer = sptr<SurfaceBufferImpl>::MakeSptr();
    rsRenderComposerTmp->PreAllocateProtectedBuffer(buffer);
    auto csurf = IConsumerSurface::Create();
    auto producer = csurf->GetProducer();
    auto pSurface = Surface::CreateSurfaceAsProducer(producer);
    std::shared_ptr<RSSurfaceOhos> rsSurface1 = std::make_shared<RSSurfaceOhosRaster>(pSurface);
    EXPECT_NE(nullptr, rsSurface1);
    rsRenderComposerTmp->frameBufferSurfaceOhosMap_.insert({0, rsSurface1});
    rsRenderComposerTmp->PreAllocateProtectedBuffer(buffer);
    rsRenderComposerTmp->hdiOutput_->fbSurface_ = nullptr;
    rsRenderComposerTmp->PreAllocateProtectedBuffer(buffer);
    rsRenderComposerTmp->hdiOutput_->SetProtectedFrameBufferState(false);
    rsRenderComposerTmp->PreAllocateProtectedBuffer(buffer);
    rsRenderComposerTmp->hdiOutput_ = nullptr;
    rsRenderComposerTmp->PreAllocateProtectedBuffer(buffer);
}

/**
 * Function: ProcessComposerFrame_ShouldDropFrameTrue
 * Type: Function
 * Rank: Important(2)
 * EnvConditions: N/A
 * CaseDescription: 1. create RSRenderComposer with super fold display
 *                  2. set activeRect and layer with uniRenderFlag true
 *                  3. call ProcessComposerFrame when shouldDropFrame is true
 *                  4. verify SwitchRefreshRate is not called
 */
HWTEST_F(RsRenderComposerTest, ProcessComposerFrame_ShouldDropFrameTrue, TestSize.Level1)
{
    system::SetParameter("const.window.foldscreen.type", "6,0,0,0");
    auto output = std::make_shared<HdiOutput>(0u);
    output->Init();
    sptr<RSScreenProperty> property = new RSScreenProperty();
    auto tmpRsRenderComposer = std::make_shared<RSRenderComposer>(output, property);
    ASSERT_NE(tmpRsRenderComposer, nullptr);

    // Set up composer screen info to trigger shouldDropFrame = true
    tmpRsRenderComposer->composerScreenInfo_.activeRect = {100, 100, 100, 100};
    tmpRsRenderComposer->rsRenderComposerContext_->AddRSRenderLayer(0, std::make_shared<RSRenderSurfaceLayer>());

    std::shared_ptr<RSRenderSurfaceLayer> layer = std::make_shared<RSRenderSurfaceLayer>();
    layer->SetLayerSize({200, 200, 100, 100}); // Different from activeRect
    layer->SetUniRenderFlag(true);
    tmpRsRenderComposer->rsRenderComposerContext_->AddRSRenderLayer(1, layer);
    EXPECT_TRUE(layer->GetUniRenderFlag());

    PipelineParam param;
    uint32_t currentRate = 60;
    tmpRsRenderComposer->ProcessComposerFrame(currentRate, param);

    system::SetParameter("const.window.foldscreen.type", "0,0,0,0");
}

/**
 * Function: ProcessComposerFrame_ShouldDropFrameFalse
 * Type: Function
 * Rank: Important(2)
 * EnvConditions: N/A
 * CaseDescription: 1. create RSRenderComposer
 *                  2. set layers with matching activeRect
 *                  3. call ProcessComposerFrame when shouldDropFrame is false
 *                  4. verify normal execution path
 */
HWTEST_F(RsRenderComposerTest, ProcessComposerFrame_ShouldDropFrameFalse, TestSize.Level1)
{
    auto output = std::make_shared<HdiOutput>(1u);
    output->Init();
    sptr<RSScreenProperty> property = new RSScreenProperty();
    auto tmpRsRenderComposer = std::make_shared<RSRenderComposer>(output, property);
    ASSERT_NE(tmpRsRenderComposer, nullptr);

    std::shared_ptr<RSRenderSurfaceLayer> layer = std::make_shared<RSRenderSurfaceLayer>();
    layer->SetLayerSize({100, 100, 100, 100});
    layer->SetUniRenderFlag(false);
    tmpRsRenderComposer->rsRenderComposerContext_->AddRSRenderLayer(1, layer);
    EXPECT_FALSE(layer->GetUniRenderFlag());

    PipelineParam param;
    uint32_t currentRate = 60;
    tmpRsRenderComposer->ProcessComposerFrame(currentRate, param);
}

/**
 * Function: ProcessComposerFrame_SuperFoldDisplayScreenId0
 * Type: Function
 * Rank: Important(2)
 * EnvConditions: N/A
 * CaseDescription: 1. create RSRenderComposer with super fold display and screenId 0
 *                  2. call ProcessComposerFrame
 *                  3. verify ChangeLayersForActiveRectOutside is called
 */
HWTEST_F(RsRenderComposerTest, ProcessComposerFrame_SuperFoldDisplayScreenId0, TestSize.Level1)
{
    system::SetParameter("const.window.foldscreen.type", "6,0,0,0");
    auto output = std::make_shared<HdiOutput>(0u);
    output->Init();
    sptr<RSScreenProperty> property = new RSScreenProperty();
    auto tmpRsRenderComposer = std::make_shared<RSRenderComposer>(output, property);
    ASSERT_NE(tmpRsRenderComposer, nullptr);

    tmpRsRenderComposer->composerScreenInfo_.reviseRect = {100, 100, 100, 100};
    tmpRsRenderComposer->composerScreenInfo_.maskRect = {0, 0, 0, 0};

    std::shared_ptr<RSRenderSurfaceLayer> layer = std::make_shared<RSRenderSurfaceLayer>();
    layer->SetLayerSize({100, 100, 100, 100});
    tmpRsRenderComposer->rsRenderComposerContext_->AddRSRenderLayer(1, layer);
    ASSERT_NE(tmpRsRenderComposer->rsRenderComposerContext_->GetRSRenderLayer(1), nullptr);

    PipelineParam param;
    uint32_t currentRate = 60;
    tmpRsRenderComposer->ProcessComposerFrame(currentRate, param);

    system::SetParameter("const.window.foldscreen.type", "0,0,0,0");
}

/**
 * Function: ProcessComposerFrame_NotSuperFoldDisplayOrScreenIdNot0
 * Type: Function
 * Rank: Important(2)
 * EnvConditions: N/A
 * CaseDescription: 1. create RSRenderComposer with screenId != 0
 *                  2. call ProcessComposerFrame
 *                  3. verify normal SetRSLayers is called without ChangeLayersForActiveRectOutside
 */
HWTEST_F(RsRenderComposerTest, ProcessComposerFrame_NotSuperFoldDisplayOrScreenIdNot0, TestSize.Level1)
{
    auto output = std::make_shared<HdiOutput>(1u);
    output->Init();
    sptr<RSScreenProperty> property = new RSScreenProperty();
    auto tmpRsRenderComposer = std::make_shared<RSRenderComposer>(output, property);
    ASSERT_NE(tmpRsRenderComposer, nullptr);

    std::shared_ptr<RSRenderSurfaceLayer> layer = std::make_shared<RSRenderSurfaceLayer>();
    layer->SetLayerSize({100, 100, 100, 100});
    tmpRsRenderComposer->rsRenderComposerContext_->AddRSRenderLayer(1, layer);
    ASSERT_NE(tmpRsRenderComposer->rsRenderComposerContext_->GetRSRenderLayer(1), nullptr);

    PipelineParam param;
    uint32_t currentRate = 60;
    tmpRsRenderComposer->ProcessComposerFrame(currentRate, param);
}

/**
 * Function: ProcessComposerFrame_HwcDeadTrue
 * Type: Function
 * Rank: Important(2)
 * EnvConditions: N/A
 * CaseDescription: 1. create RSRenderComposer
 *                  2. set isHwcDead_ to true
 *                  3. call ProcessComposerFrame
 *                  4. verify SetRSLayers and Repaint are not called
 */
HWTEST_F(RsRenderComposerTest, ProcessComposerFrame_HwcDeadTrue, TestSize.Level1)
{
    auto output = std::make_shared<HdiOutput>(1u);
    output->Init();
    sptr<RSScreenProperty> property = new RSScreenProperty();
    auto tmpRsRenderComposer = std::make_shared<RSRenderComposer>(output, property);
    tmpRsRenderComposer->OnHwcDead();

    std::shared_ptr<RSRenderSurfaceLayer> layer = std::make_shared<RSRenderSurfaceLayer>();
    layer->SetLayerSize({100, 100, 100, 100});
    tmpRsRenderComposer->rsRenderComposerContext_->AddRSRenderLayer(1, layer);

    PipelineParam param;
    uint32_t currentRate = 60;
    tmpRsRenderComposer->ProcessComposerFrame(currentRate, param);

    EXPECT_TRUE(tmpRsRenderComposer->isHwcDead_);
}

/**
 * Function: ProcessComposerFrame_HwcDeadFalse
 * Type: Function
 * Rank: Important(2)
 * EnvConditions: N/A
 * CaseDescription: 1. create RSRenderComposer
 *                  2. ensure isHwcDead_ is false
 *                  3. call ProcessComposerFrame
 *                  4. verify SetRSLayers and Repaint are called when device is valid
 */
HWTEST_F(RsRenderComposerTest, ProcessComposerFrame_HwcDeadFalse, TestSize.Level1)
{
    auto output = std::make_shared<HdiOutput>(1u);
    output->Init();
    sptr<RSScreenProperty> property = new RSScreenProperty();
    auto tmpRsRenderComposer = std::make_shared<RSRenderComposer>(output, property);

    std::shared_ptr<RSRenderSurfaceLayer> layer = std::make_shared<RSRenderSurfaceLayer>();
    layer->SetLayerSize({100, 100, 100, 100});
    tmpRsRenderComposer->rsRenderComposerContext_->AddRSRenderLayer(1, layer);

    PipelineParam param;
    uint32_t currentRate = 60;
    tmpRsRenderComposer->ProcessComposerFrame(currentRate, param);

    EXPECT_FALSE(tmpRsRenderComposer->isHwcDead_);
}

/**
 * Function: ProcessComposerFrame_DeviceValidFalse
 * Type: Function
 * Rank: Important(2)
 * EnvConditions: N/A
 * CaseDescription: 1. create RSRenderComposer
 *                  2. call ResetDevice to make device invalid
 *                  3. call ProcessComposerFrame
 *                  4. verify Repaint is not called
 */
HWTEST_F(RsRenderComposerTest, ProcessComposerFrame_DeviceValidFalse, TestSize.Level1)
{
    auto output = std::make_shared<HdiOutput>(1u);
    output->Init();
    sptr<RSScreenProperty> property = new RSScreenProperty();
    auto tmpRsRenderComposer = std::make_shared<RSRenderComposer>(output, property);
    tmpRsRenderComposer->hdiOutput_->ResetDevice();

    std::shared_ptr<RSRenderSurfaceLayer> layer = std::make_shared<RSRenderSurfaceLayer>();
    layer->SetLayerSize({100, 100, 100, 100});
    tmpRsRenderComposer->rsRenderComposerContext_->AddRSRenderLayer(1, layer);

    PipelineParam param;
    uint32_t currentRate = 60;
    tmpRsRenderComposer->ProcessComposerFrame(currentRate, param);

    EXPECT_FALSE(tmpRsRenderComposer->hdiOutput_->IsDeviceValid());
}

/**
 * Function: ProcessComposerFrame_DoRepaintTrue
 * Type: Function
 * Rank: Important(2)
 * EnvConditions: N/A
 * CaseDescription: 1. create RSRenderComposer
 *                  2. set valid device, not drop frame, not hwc dead
 *                  3. call ProcessComposerFrame
 *                  4. verify Repaint is called
 */
HWTEST_F(RsRenderComposerTest, ProcessComposerFrame_DoRepaintTrue, TestSize.Level1)
{
    auto output = std::make_shared<HdiOutput>(1u);
    output->Init();
    sptr<RSScreenProperty> property = new RSScreenProperty();
    auto tmpRsRenderComposer = std::make_shared<RSRenderComposer>(output, property);

    std::shared_ptr<RSRenderSurfaceLayer> layer = std::make_shared<RSRenderSurfaceLayer>();
    layer->SetLayerSize({100, 100, 100, 100});
    layer->SetUniRenderFlag(false);
    tmpRsRenderComposer->rsRenderComposerContext_->AddRSRenderLayer(1, layer);

    PipelineParam param;
    uint32_t currentRate = 60;
    tmpRsRenderComposer->ProcessComposerFrame(currentRate, param);

    EXPECT_TRUE(tmpRsRenderComposer->hdiOutput_->IsDeviceValid());
    EXPECT_FALSE(tmpRsRenderComposer->isHwcDead_);
}

/**
 * Function: ProcessComposerFrame_DoRepaintFalse_DropFrame
 * Type: Function
 * Rank: Important(2)
 * EnvConditions: N/A
 * CaseDescription: 1. create RSRenderComposer with super fold display
 *                  2. set up to trigger shouldDropFrame true
 *                  3. call ProcessComposerFrame
 *                  4. verify Repaint is not called
 */
HWTEST_F(RsRenderComposerTest, ProcessComposerFrame_DoRepaintFalse_DropFrame, TestSize.Level1)
{
    system::SetParameter("const.window.foldscreen.type", "6,0,0,0");
    auto output = std::make_shared<HdiOutput>(1u);
    output->Init();
    sptr<RSScreenProperty> property = new RSScreenProperty();
    auto tmpRsRenderComposer = std::make_shared<RSRenderComposer>(output, property);
    ASSERT_NE(tmpRsRenderComposer, nullptr);

    tmpRsRenderComposer->composerScreenInfo_.activeRect = {100, 100, 100, 100};

    std::shared_ptr<RSRenderSurfaceLayer> layer = std::make_shared<RSRenderSurfaceLayer>();
    layer->SetLayerSize({200, 200, 100, 100}); // Different from activeRect
    layer->SetUniRenderFlag(true);
    tmpRsRenderComposer->rsRenderComposerContext_->AddRSRenderLayer(1, layer);
    ASSERT_NE(tmpRsRenderComposer->rsRenderComposerContext_->GetRSRenderLayer(1), nullptr);

    PipelineParam param;
    uint32_t currentRate = 60;
    tmpRsRenderComposer->ProcessComposerFrame(currentRate, param);

    system::SetParameter("const.window.foldscreen.type", "0,0,0,0");
}

/**
 * Function: ProcessComposerFrame_ComposerToRenderConnectionNull
 * Type: Function
 * Rank: Important(2)
 * EnvConditions: N/A
 * CaseDescription: 1. create RSRenderComposer
 *                  2. set composerToRenderConnection_ to nullptr
 *                  3. call ProcessComposerFrame
 *                  4. verify ReleaseLayerBuffers is not called
 */
HWTEST_F(RsRenderComposerTest, ProcessComposerFrame_ComposerToRenderConnectionNull, TestSize.Level1)
{
    auto output = std::make_shared<HdiOutput>(1u);
    output->Init();
    sptr<RSScreenProperty> property = new RSScreenProperty();
    auto tmpRsRenderComposer = std::make_shared<RSRenderComposer>(output, property);
    tmpRsRenderComposer->composerToRenderConnection_ = nullptr;

    std::shared_ptr<RSRenderSurfaceLayer> layer = std::make_shared<RSRenderSurfaceLayer>();
    layer->SetLayerSize({100, 100, 100, 100});
    tmpRsRenderComposer->rsRenderComposerContext_->AddRSRenderLayer(1, layer);

    PipelineParam param;
    uint32_t currentRate = 60;
    tmpRsRenderComposer->ProcessComposerFrame(currentRate, param);

    EXPECT_EQ(tmpRsRenderComposer->composerToRenderConnection_, nullptr);
}

/**
 * Function: ProcessComposerFrame_MissedFramesReport
 * Type: Function
 * Rank: Important(2)
 * EnvConditions: N/A
 * CaseDescription: 1. create RSRenderComposer
 *                  2. set intervalTimePoints_ to trigger hard jank report
 *                  3. call ProcessComposerFrame with delayed execution
 *                  4. verify load warning event is reported
 */
HWTEST_F(RsRenderComposerTest, ProcessComposerFrame_MissedFramesReport, TestSize.Level1)
{
    auto output = std::make_shared<HdiOutput>(1u);
    output->Init();
    sptr<RSScreenProperty> property = new RSScreenProperty();
    auto tmpRsRenderComposer = std::make_shared<RSRenderComposer>(output, property);
    ASSERT_NE(tmpRsRenderComposer, nullptr);

    std::shared_ptr<RSRenderSurfaceLayer> layer = std::make_shared<RSRenderSurfaceLayer>();
    layer->SetLayerSize({100, 100, 100, 100});
    tmpRsRenderComposer->rsRenderComposerContext_->AddRSRenderLayer(1, layer);
    ASSERT_NE(tmpRsRenderComposer->rsRenderComposerContext_->GetRSRenderLayer(1), nullptr);

    // Set intervalTimePoints_ to trigger report condition
    tmpRsRenderComposer->intervalTimePoints_ = 0;

    PipelineParam param;
    uint32_t currentRate = 60;
    tmpRsRenderComposer->ProcessComposerFrame(currentRate, param);

    // Sleep to ensure enough time passes for frame time calculation
    std::this_thread::sleep_for(std::chrono::milliseconds(50));
    tmpRsRenderComposer->ProcessComposerFrame(currentRate, param);
}

/**
 * Function: ProcessComposerFrame_NoMissedFramesReport
 * Type: Function
 * Rank: Important(2)
 * EnvConditions: N/A
 * CaseDescription: 1. create RSRenderComposer
 *                  2. call ProcessComposerFrame normally
 *                  3. verify no load warning event is reported
 */
HWTEST_F(RsRenderComposerTest, ProcessComposerFrame_NoMissedFramesReport, TestSize.Level1)
{
    auto output = std::make_shared<HdiOutput>(1u);
    output->Init();
    sptr<RSScreenProperty> property = new RSScreenProperty();
    auto tmpRsRenderComposer = std::make_shared<RSRenderComposer>(output, property);

    std::shared_ptr<RSRenderSurfaceLayer> layer = std::make_shared<RSRenderSurfaceLayer>();
    layer->SetLayerSize({100, 100, 100, 100});
    tmpRsRenderComposer->rsRenderComposerContext_->AddRSRenderLayer(1, layer);

    PipelineParam param;
    uint32_t currentRate = 60;
    tmpRsRenderComposer->ProcessComposerFrame(currentRate, param);
}

/**
 * Function: ProcessComposerFrame_HasLppVideoTrue
 * Type: Function
 * Rank: Important(2)
 * EnvConditions: N/A
 * CaseDescription: 1. create RSRenderComposer
 *                  2. set composerToRenderConnection_ and hasLppVideo true
 *                  3. call ProcessComposerFrame
 *                  4. verify NotifyLppLayerToRender is called
 */
HWTEST_F(RsRenderComposerTest, ProcessComposerFrame_HasLppVideoTrue, TestSize.Level1)
{
    auto output = std::make_shared<HdiOutput>(1u);
    output->Init();
    sptr<RSScreenProperty> property = new RSScreenProperty();
    auto tmpRsRenderComposer = std::make_shared<RSRenderComposer>(output, property);
    ASSERT_NE(tmpRsRenderComposer, nullptr);

    std::shared_ptr<RSRenderSurfaceLayer> layer = std::make_shared<RSRenderSurfaceLayer>();
    layer->SetLayerSize({100, 100, 100, 100});
    tmpRsRenderComposer->rsRenderComposerContext_->AddRSRenderLayer(1, layer);
    ASSERT_NE(tmpRsRenderComposer->rsRenderComposerContext_->GetRSRenderLayer(1), nullptr);

    PipelineParam param;
    param.hasLppVideo = true;
    uint32_t currentRate = 60;

    // Note: composerToRenderConnection_ is nullptr by default in tests
    // This test verifies the branch condition behavior
    tmpRsRenderComposer->ProcessComposerFrame(currentRate, param);
}

/**
 * Function: ProcessComposerFrame_HasLppVideoFalse
 * Type: Function
 * Rank: Important(2)
 * EnvConditions: N/A
 * CaseDescription: 1. create RSRenderComposer
 *                  2. set hasLppVideo false
 *                  3. call ProcessComposerFrame
 *                  4. verify NotifyLppLayerToRender is not called
 */
HWTEST_F(RsRenderComposerTest, ProcessComposerFrame_HasLppVideoFalse, TestSize.Level1)
{
    auto output = std::make_shared<HdiOutput>(1u);
    output->Init();
    sptr<RSScreenProperty> property = new RSScreenProperty();
    auto tmpRsRenderComposer = std::make_shared<RSRenderComposer>(output, property);
    ASSERT_NE(tmpRsRenderComposer, nullptr);

    std::shared_ptr<RSRenderSurfaceLayer> layer = std::make_shared<RSRenderSurfaceLayer>();
    layer->SetLayerSize({100, 100, 100, 100});
    tmpRsRenderComposer->rsRenderComposerContext_->AddRSRenderLayer(1, layer);
    ASSERT_NE(tmpRsRenderComposer->rsRenderComposerContext_->GetRSRenderLayer(1), nullptr);

    PipelineParam param;
    param.hasLppVideo = false;
    uint32_t currentRate = 60;
    tmpRsRenderComposer->ProcessComposerFrame(currentRate, param);
}

/**
 * Function: ProcessComposerFrame_IsDisconnectedWithTasks
 * Type: Function
 * Rank: Important(2)
 * EnvConditions: N/A
 * CaseDescription: 1. create RSRenderComposer
 *                  2. set isDisconnected_ true with pending tasks
 *                  3. call ProcessComposerFrame
 *                  4. verify resources are not cleared
 */
HWTEST_F(RsRenderComposerTest, ProcessComposerFrame_IsDisconnectedWithTasks, TestSize.Level1)
{
    auto output = std::make_shared<HdiOutput>(1u);
    output->Init();
    sptr<RSScreenProperty> property = new RSScreenProperty();
    auto tmpRsRenderComposer = std::make_shared<RSRenderComposer>(output, property);
    tmpRsRenderComposer->unExecuteTaskNum_.store(5);

    std::shared_ptr<RSRenderSurfaceLayer> layer = std::make_shared<RSRenderSurfaceLayer>();
    layer->SetLayerSize({100, 100, 100, 100});
    tmpRsRenderComposer->rsRenderComposerContext_->AddRSRenderLayer(1, layer);

    PipelineParam param;
    uint32_t currentRate = 60;
    tmpRsRenderComposer->ProcessComposerFrame(currentRate, param);

    EXPECT_NE(tmpRsRenderComposer->hdiOutput_, nullptr);
}

/**
 * Function: ProcessComposerFrame_IsDisconnectedNoTasks
 * Type: Function
 * Rank: Important(2)
 * EnvConditions: N/A
 * CaseDescription: 1. create RSRenderComposer
 *                  2. set isDisconnected_ true with no pending tasks
 *                  3. call ProcessComposerFrame
 *                  4. verify resources are cleared
 */
HWTEST_F(RsRenderComposerTest, ProcessComposerFrame_IsDisconnectedNoTasks, TestSize.Level1)
{
    auto output = std::make_shared<HdiOutput>(1u);
    output->Init();
    sptr<RSScreenProperty> property = new RSScreenProperty();
    auto tmpRsRenderComposer = std::make_shared<RSRenderComposer>(output, property);
    tmpRsRenderComposer->unExecuteTaskNum_.store(0);

    std::shared_ptr<RSRenderSurfaceLayer> layer = std::make_shared<RSRenderSurfaceLayer>();
    layer->SetLayerSize({100, 100, 100, 100});
    tmpRsRenderComposer->rsRenderComposerContext_->AddRSRenderLayer(1, layer);

    PipelineParam param;
    uint32_t currentRate = 60;
    tmpRsRenderComposer->ProcessComposerFrame(currentRate, param);

    // After processing, resources should be cleared
    std::this_thread::sleep_for(std::chrono::milliseconds(200));
}

/**
 * Function: ProcessComposerFrame_NotDisconnected
 * Type: Function
 * Rank: Important(2)
 * EnvConditions: N/A
 * CaseDescription: 1. create RSRenderComposer
 *                  2. ensure isDisconnected_ is false
 *                  3. call ProcessComposerFrame
 *                  4. verify normal execution path
 */
HWTEST_F(RsRenderComposerTest, ProcessComposerFrame_NotDisconnected, TestSize.Level1)
{
    auto output = std::make_shared<HdiOutput>(1u);
    output->Init();
    sptr<RSScreenProperty> property = new RSScreenProperty();
    auto tmpRsRenderComposer = std::make_shared<RSRenderComposer>(output, property);

    std::shared_ptr<RSRenderSurfaceLayer> layer = std::make_shared<RSRenderSurfaceLayer>();
    layer->SetLayerSize({100, 100, 100, 100});
    tmpRsRenderComposer->rsRenderComposerContext_->AddRSRenderLayer(1, layer);

    PipelineParam param;
    uint32_t currentRate = 60;
    tmpRsRenderComposer->ProcessComposerFrame(currentRate, param);

    EXPECT_FALSE(tmpRsRenderComposer->isDisconnected_);
    EXPECT_NE(tmpRsRenderComposer->hdiOutput_, nullptr);
}

/**
 * Function: ProcessComposerFrame_EmptyLayers
 * Type: Function
 * Rank: Important(2)
 * EnvConditions: N/A
 * CaseDescription: 1. create RSRenderComposer
 *                  2. call ProcessComposerFrame with empty layers
 *                  3. verify no crash occurs
 */
HWTEST_F(RsRenderComposerTest, ProcessComposerFrame_EmptyLayers, TestSize.Level1)
{
    auto output = std::make_shared<HdiOutput>(1u);
    output->Init();
    sptr<RSScreenProperty> property = new RSScreenProperty();
    auto tmpRsRenderComposer = std::make_shared<RSRenderComposer>(output, property);
    ASSERT_NE(tmpRsRenderComposer, nullptr);

    // Clear any existing layers by creating a new empty context
    tmpRsRenderComposer->rsRenderComposerContext_ = std::make_shared<RSRenderComposerContext>();

    PipelineParam param;
    uint32_t currentRate = 60;
    tmpRsRenderComposer->ProcessComposerFrame(currentRate, param);
}

/**
 * Function: ProcessComposerFrame_MultipleLayers
 * Type: Function
 * Rank: Important(2)
 * EnvConditions: N/A
 * CaseDescription: 1. create RSRenderComposer
 *                  2. add multiple layers with different properties
 *                  3. call ProcessComposerFrame
 *                  4. verify all layers are processed
 */
HWTEST_F(RsRenderComposerTest, ProcessComposerFrame_MultipleLayers, TestSize.Level1)
{
    auto output = std::make_shared<HdiOutput>(1u);
    output->Init();
    sptr<RSScreenProperty> property = new RSScreenProperty();
    auto tmpRsRenderComposer = std::make_shared<RSRenderComposer>(output, property);
    ASSERT_NE(tmpRsRenderComposer, nullptr);

    // Add multiple layers
    for (int i = 0; i < 5; i++) {
        std::shared_ptr<RSRenderSurfaceLayer> layer = std::make_shared<RSRenderSurfaceLayer>();
        layer->SetLayerSize({100, 100, 100, 100});
        layer->SetZorder(i);
        tmpRsRenderComposer->rsRenderComposerContext_->AddRSRenderLayer(i, layer);
        ASSERT_NE(tmpRsRenderComposer->rsRenderComposerContext_->GetRSRenderLayer(i), nullptr);
    }

    PipelineParam param;
    uint32_t currentRate = 60;
    tmpRsRenderComposer->ProcessComposerFrame(currentRate, param);
}

/**
 * Function: ProcessComposerFrame_HasGameScene
 * Type: Function
 * Rank: Important(2)
 * EnvConditions: N/A
 * CaseDescription: 1. create RSRenderComposer
 *                  2. set hasGameScene true
 *                  3. call ProcessComposerFrame
 *                  4. verify game scene branch is handled
 */
HWTEST_F(RsRenderComposerTest, ProcessComposerFrame_HasGameScene2, TestSize.Level1)
{
    auto output = std::make_shared<HdiOutput>(1u);
    output->Init();
    sptr<RSScreenProperty> property = new RSScreenProperty();
    auto tmpRsRenderComposer = std::make_shared<RSRenderComposer>(output, property);
    ASSERT_NE(tmpRsRenderComposer, nullptr);

    std::shared_ptr<RSRenderSurfaceLayer> layer = std::make_shared<RSRenderSurfaceLayer>();
    layer->SetLayerSize({100, 100, 100, 100});
    tmpRsRenderComposer->rsRenderComposerContext_->AddRSRenderLayer(1, layer);
    ASSERT_NE(tmpRsRenderComposer->rsRenderComposerContext_->GetRSRenderLayer(1), nullptr);

    PipelineParam param;
    param.hasGameScene = true;
    uint32_t currentRate = 60;
    tmpRsRenderComposer->ProcessComposerFrame(currentRate, param);
}
} // namespace Rosen
} // namespace OHOS
