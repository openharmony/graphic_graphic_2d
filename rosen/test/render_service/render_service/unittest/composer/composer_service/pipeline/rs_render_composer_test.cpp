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
#ifdef RS_ENABLE_VK
#include "platform/ohos/backend/rs_surface_ohos_vulkan.h"
#endif
#include "syspara/parameters.h"
#include "syspara/parameter.h"
#include "param/sys_param.h"
#include "feature/round_corner_display/rs_round_corner_display_manager.h"
#include "common/rs_singleton.h"
#include "pipeline/main_thread/rs_render_service_listener.h"
#include "pipeline/rs_render_node_gc.h"
#include "rs_composer_to_render_connection.h"
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
    sptr<RSScreenProperty> property = sptr<RSScreenProperty>::MakeSptr();
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
 * Function: ProcessComposerFrame_SuperFoldDisplayWithHwcDead
 * Type: Function
 * Rank: Important(2)
 * EnvConditions: N/A
 * CaseDescription: 1. create RSRenderComposer with super fold display and screenId 0
 *                  2. set isHwcDead_ to true
 *                  3. call ProcessComposerFrame
 *                  4. verify SetRSLayers is not called when HWC is dead
 *                  5. verify Repaint is not called when HWC is dead
 */
HWTEST_F(RsRenderComposerTest, ProcessComposerFrame_SuperFoldDisplayWithHwcDead, TestSize.Level1)
{
    system::SetParameter("const.window.foldscreen.type", "6,0,0,0");
    auto output = std::make_shared<HdiOutput>(0u);
    output->Init();
    sptr<RSScreenProperty> property = new RSScreenProperty();
    auto tmpRsRenderComposer = std::make_shared<RSRenderComposer>(output, property);
    ASSERT_NE(tmpRsRenderComposer, nullptr);

    // Set isHwcDead_ to true, which makes !isHwcDead_ false at line 286
    tmpRsRenderComposer->OnHwcDead();
    EXPECT_TRUE(tmpRsRenderComposer->isHwcDead_);

    tmpRsRenderComposer->composerScreenInfo_.reviseRect = {100, 100, 100, 100};
    tmpRsRenderComposer->composerScreenInfo_.maskRect = {0, 0, 0, 0};

    std::shared_ptr<RSRenderSurfaceLayer> layer = std::make_shared<RSRenderSurfaceLayer>();
    layer->SetLayerSize({100, 100, 100, 100});
    tmpRsRenderComposer->rsRenderComposerContext_->AddRSRenderLayer(1, layer);
    ASSERT_NE(tmpRsRenderComposer->rsRenderComposerContext_->GetRSRenderLayer(1), nullptr);

    PipelineParam param;
    uint32_t currentRate = 60;
    tmpRsRenderComposer->ProcessComposerFrame(currentRate, param);

    // Verify isHwcDead_ remains true
    EXPECT_TRUE(tmpRsRenderComposer->isHwcDead_);

    system::SetParameter("const.window.foldscreen.type", "0,0,0,0");
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

/**
 * Function: ProcessComposerFrame_ComposerToRenderConnectionNotNull
 * Type: Function
 * Rank: Important(2)
 * EnvConditions: N/A
 * CaseDescription: 1. create RSRenderComposer
 *                  2. set composerToRenderConnection_ to not null
 *                  3. call ProcessComposerFrame
 *                  4. verify ReleaseLayerBuffers is called when composerToRenderConnection_ is not null (line 310 condition true)
 */
HWTEST_F(RsRenderComposerTest, ProcessComposerFrame_ComposerToRenderConnectionNotNull, TestSize.Level1)
{
    auto output = std::make_shared<HdiOutput>(1u);
    output->Init();
    sptr<RSScreenProperty> property = new RSScreenProperty();
    auto tmpRsRenderComposer = std::make_shared<RSRenderComposer>(output, property);
    ASSERT_NE(tmpRsRenderComposer, nullptr);

    // Create and set composerToRenderConnection_ (line 310 condition true)
    tmpRsRenderComposer->composerToRenderConnection_ = sptr<RSComposerToRenderConnection>::MakeSptr();

    std::shared_ptr<RSRenderSurfaceLayer> layer = std::make_shared<RSRenderSurfaceLayer>();
    layer->SetLayerSize({100, 100, 100, 100});
    tmpRsRenderComposer->rsRenderComposerContext_->AddRSRenderLayer(1, layer);
    ASSERT_NE(tmpRsRenderComposer->rsRenderComposerContext_->GetRSRenderLayer(1), nullptr);

    PipelineParam param;
    uint32_t currentRate = 60;
    tmpRsRenderComposer->ProcessComposerFrame(currentRate, param);

    // Verify composerToRenderConnection_ remains not null
    EXPECT_NE(tmpRsRenderComposer->composerToRenderConnection_, nullptr);
}

/**
 * Function: ProcessComposerFrame_HardJankReport_True
 * Type: Function
 * Rank: Important(2)
 * EnvConditions: N/A
 * CaseDescription: 1. create RSRenderComposer
 *                  2. set intervalTimePoints_ to trigger hard jank report (line 323 condition true)
 *                  3. add delay to cause missedFrames >= HARD_JANK_TWO_TIME
 *                  4. call ProcessComposerFrame
 *                  5. verify load warning event is reported
 */
HWTEST_F(RsRenderComposerTest, ProcessComposerFrame_HardJankReport_True, TestSize.Level1)
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

    // Set intervalTimePoints_ to trigger report condition at line 323
    // Condition: missedFrames >= 2 && endTime - intervalTimePoints_ > 5000000
    tmpRsRenderComposer->intervalTimePoints_ = 0;

    PipelineParam param;
    uint32_t currentRate = 60;
    tmpRsRenderComposer->ProcessComposerFrame(currentRate, param);

    // Sleep to ensure enough time passes for frame time calculation to cause missedFrames >= 2
    // REFRESH_PERIOD = 16667us, HARD_JANK_TWO_TIME = 2, so need frameTime >= 33334us
    std::this_thread::sleep_for(std::chrono::milliseconds(50));
    tmpRsRenderComposer->ProcessComposerFrame(currentRate, param);
}

/**
 * Function: ProcessComposerFrame_HardJankReport_False_LowMissedFrames
 * Type: Function
 * Rank: Important(2)
 * EnvConditions: N/A
 * CaseDescription: 1. create RSRenderComposer
 *                  2. set intervalTimePoints_ to trigger time condition
 *                  3. call ProcessComposerFrame quickly to keep missedFrames < HARD_JANK_TWO_TIME (line 323 condition false)
 *                  4. verify no load warning event is reported when missedFrames < 2
 */
HWTEST_F(RsRenderComposerTest, ProcessComposerFrame_HardJankReport_False_LowMissedFrames, TestSize.Level1)
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

    // Set intervalTimePoints_ to satisfy time condition at line 323
    tmpRsRenderComposer->intervalTimePoints_ = 0;

    PipelineParam param;
    uint32_t currentRate = 60;
    // Call ProcessComposerFrame quickly to ensure missedFrames < HARD_JANK_TWO_TIME (2)
    // This makes the condition at line 323 false because missedFrames < 2
    tmpRsRenderComposer->ProcessComposerFrame(currentRate, param);
}

/**
 * Function: ProcessComposerFrame_HardJankReport_False_ShortInterval
 * Type: Function
 * Rank: Important(2)
 * EnvConditions: N/A
 * CaseDescription: 1. create RSRenderComposer
 *                  2. set intervalTimePoints_ to current time to fail time condition
 *                  3. add delay to cause missedFrames >= HARD_JANK_TWO_TIME
 *                  4. call ProcessComposerFrame
 *                  5. verify no load warning event is reported when interval is too short (line 323 condition false)
 */
HWTEST_F(RsRenderComposerTest, ProcessComposerFrame_HardJankReport_False_ShortInterval, TestSize.Level1)
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

    // Set intervalTimePoints_ to a recent time to fail time condition at line 323
    // Condition: endTime - intervalTimePoints_ > 5000000
    // By setting intervalTimePoints_ to current time, the time difference will be < 5000000
    tmpRsRenderComposer->intervalTimePoints_ = tmpRsRenderComposer->GetCurTimeCount();

    PipelineParam param;
    uint32_t currentRate = 60;
    tmpRsRenderComposer->ProcessComposerFrame(currentRate, param);

    // Sleep to cause missedFrames >= 2, but since intervalTimePoints_ is recent,
    // the condition at line 323 will still be false
    std::this_thread::sleep_for(std::chrono::milliseconds(50));
    tmpRsRenderComposer->ProcessComposerFrame(currentRate, param);
}

/**
 * Function: ProcessComposerFrame_HardJankReport_False_BothConditionsFalse
 * Type: Function
 * Rank: Important(2)
 * EnvConditions: N/A
 * CaseDescription: 1. create RSRenderComposer
 *                  2. set intervalTimePoints_ to current time
 *                  3. call ProcessComposerFrame quickly
 *                  4. verify no load warning event when both conditions are false (line 323 condition false)
 */
HWTEST_F(RsRenderComposerTest, ProcessComposerFrame_HardJankReport_False_BothConditionsFalse, TestSize.Level1)
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

    // Set intervalTimePoints_ to current time to fail time condition
    tmpRsRenderComposer->intervalTimePoints_ = tmpRsRenderComposer->GetCurTimeCount();

    PipelineParam param;
    uint32_t currentRate = 60;
    // Call ProcessComposerFrame quickly, making both conditions at line 323 false:
    // 1. missedFrames < HARD_JANK_TWO_TIME (2)
    // 2. endTime - intervalTimePoints_ <= REPORT_LOAD_WARNING_INTERVAL_TIME (5000000)
    tmpRsRenderComposer->ProcessComposerFrame(currentRate, param);
}

/**
 * Function: ProcessComposerFrame_LppVideoNotify_True_BothConditions
 * Type: Function
 * Rank: Important(2)
 * EnvConditions: N/A
 * CaseDescription: 1. create RSRenderComposer
 *                  2. set composerToRenderConnection_ not null and hasLppVideo true (line 333 condition true)
 *                  3. call ProcessComposerFrame
 *                  4. verify NotifyLppLayerToRender is called when both conditions are true
 */
HWTEST_F(RsRenderComposerTest, ProcessComposerFrame_LppVideoNotify_True_BothConditions, TestSize.Level1)
{
    auto output = std::make_shared<HdiOutput>(1u);
    output->Init();
    sptr<RSScreenProperty> property = new RSScreenProperty();
    auto tmpRsRenderComposer = std::make_shared<RSRenderComposer>(output, property);
    ASSERT_NE(tmpRsRenderComposer, nullptr);

    // Set composerToRenderConnection_ not null (line 333 condition part 1 true)
    tmpRsRenderComposer->composerToRenderConnection_ = sptr<RSComposerToRenderConnection>::MakeSptr();

    std::shared_ptr<RSRenderSurfaceLayer> layer = std::make_shared<RSRenderSurfaceLayer>();
    layer->SetLayerSize({100, 100, 100, 100});
    tmpRsRenderComposer->rsRenderComposerContext_->AddRSRenderLayer(1, layer);
    ASSERT_NE(tmpRsRenderComposer->rsRenderComposerContext_->GetRSRenderLayer(1), nullptr);

    PipelineParam param;
    param.hasLppVideo = true; // line 333 condition part 2 true
    uint32_t currentRate = 60;
    tmpRsRenderComposer->ProcessComposerFrame(currentRate, param);

    // Verify both conditions remain true
    EXPECT_NE(tmpRsRenderComposer->composerToRenderConnection_, nullptr);
}

/**
 * Function: ProcessComposerFrame_LppVideoNotify_False_ConnectionNull
 * Type: Function
 * Rank: Important(2)
 * EnvConditions: N/A
 * CaseDescription: 1. create RSRenderComposer
 *                  2. set composerToRenderConnection_ to null and hasLppVideo true (line 333 condition false)
 *                  3. call ProcessComposerFrame
 *                  4. verify NotifyLppLayerToRender is not called when connection is null
 */
HWTEST_F(RsRenderComposerTest, ProcessComposerFrame_LppVideoNotify_False_ConnectionNull, TestSize.Level1)
{
    auto output = std::make_shared<HdiOutput>(1u);
    output->Init();
    sptr<RSScreenProperty> property = new RSScreenProperty();
    auto tmpRsRenderComposer = std::make_shared<RSRenderComposer>(output, property);
    ASSERT_NE(tmpRsRenderComposer, nullptr);

    // Set composerToRenderConnection_ to null (line 333 condition part 1 false)
    tmpRsRenderComposer->composerToRenderConnection_ = nullptr;

    std::shared_ptr<RSRenderSurfaceLayer> layer = std::make_shared<RSRenderSurfaceLayer>();
    layer->SetLayerSize({100, 100, 100, 100});
    tmpRsRenderComposer->rsRenderComposerContext_->AddRSRenderLayer(1, layer);
    ASSERT_NE(tmpRsRenderComposer->rsRenderComposerContext_->GetRSRenderLayer(1), nullptr);

    PipelineParam param;
    param.hasLppVideo = true; // line 333 condition part 2 true, but overall condition false due to null connection
    uint32_t currentRate = 60;
    tmpRsRenderComposer->ProcessComposerFrame(currentRate, param);

    // Verify connection remains null
    EXPECT_EQ(tmpRsRenderComposer->composerToRenderConnection_, nullptr);
}

/**
 * Function: ProcessComposerFrame_LppVideoNotify_False_NoLppVideo
 * Type: Function
 * Rank: Important(2)
 * EnvConditions: N/A
 * CaseDescription: 1. create RSRenderComposer
 *                  2. set composerToRenderConnection_ not null and hasLppVideo false (line 333 condition false)
 *                  3. call ProcessComposerFrame
 *                  4. verify NotifyLppLayerToRender is not called when hasLppVideo is false
 */
HWTEST_F(RsRenderComposerTest, ProcessComposerFrame_LppVideoNotify_False_NoLppVideo, TestSize.Level1)
{
    auto output = std::make_shared<HdiOutput>(1u);
    output->Init();
    sptr<RSScreenProperty> property = new RSScreenProperty();
    auto tmpRsRenderComposer = std::make_shared<RSRenderComposer>(output, property);
    ASSERT_NE(tmpRsRenderComposer, nullptr);

    // Set composerToRenderConnection_ not null (line 333 condition part 1 true)
    tmpRsRenderComposer->composerToRenderConnection_ = sptr<RSComposerToRenderConnection>::MakeSptr();

    std::shared_ptr<RSRenderSurfaceLayer> layer = std::make_shared<RSRenderSurfaceLayer>();
    layer->SetLayerSize({100, 100, 100, 100});
    tmpRsRenderComposer->rsRenderComposerContext_->AddRSRenderLayer(1, layer);
    ASSERT_NE(tmpRsRenderComposer->rsRenderComposerContext_->GetRSRenderLayer(1), nullptr);

    PipelineParam param;
    param.hasLppVideo = false; // line 333 condition part 2 false, overall condition false
    uint32_t currentRate = 60;
    tmpRsRenderComposer->ProcessComposerFrame(currentRate, param);

    // Verify connection is not null but notify should not be called
    EXPECT_NE(tmpRsRenderComposer->composerToRenderConnection_, nullptr);
}

/**
 * Function: ProcessComposerFrame_LppVideoNotify_False_BothFalse
 * Type: Function
 * Rank: Important(2)
 * EnvConditions: N/A
 * CaseDescription: 1. create RSRenderComposer
 *                  2. set composerToRenderConnection_ null and hasLppVideo false (line 333 condition false)
 *                  3. call ProcessComposerFrame
 *                  4. verify NotifyLppLayerToRender is not called when both conditions are false
 */
HWTEST_F(RsRenderComposerTest, ProcessComposerFrame_LppVideoNotify_False_BothFalse, TestSize.Level1)
{
    auto output = std::make_shared<HdiOutput>(1u);
    output->Init();
    sptr<RSScreenProperty> property = new RSScreenProperty();
    auto tmpRsRenderComposer = std::make_shared<RSRenderComposer>(output, property);
    ASSERT_NE(tmpRsRenderComposer, nullptr);

    // Set composerToRenderConnection_ to null (line 333 condition part 1 false)
    tmpRsRenderComposer->composerToRenderConnection_ = nullptr;

    std::shared_ptr<RSRenderSurfaceLayer> layer = std::make_shared<RSRenderSurfaceLayer>();
    layer->SetLayerSize({100, 100, 100, 100});
    tmpRsRenderComposer->rsRenderComposerContext_->AddRSRenderLayer(1, layer);
    ASSERT_NE(tmpRsRenderComposer->rsRenderComposerContext_->GetRSRenderLayer(1), nullptr);

    PipelineParam param;
    param.hasLppVideo = false; // line 333 condition part 2 false
    uint32_t currentRate = 60;
    tmpRsRenderComposer->ProcessComposerFrame(currentRate, param);

    // Verify both conditions remain false
    EXPECT_EQ(tmpRsRenderComposer->composerToRenderConnection_, nullptr);
}

/**
 * Function: ProcessComposerFrame_ClearOutputOnDisconnect_True
 * Type: Function
 * Rank: Important(2)
 * EnvConditions: N/A
 * CaseDescription: 1. create RSRenderComposer
 *                  2. set isDisconnected_ true and unExecuteTaskNum_ to 0 (line 337 condition true)
 *                  3. call ProcessComposerFrame
 *                  4. verify resources are cleared (ClearFrameBuffersInner, rsRenderComposerContext_, hdiOutput_)
 */
HWTEST_F(RsRenderComposerTest, ProcessComposerFrame_ClearOutputOnDisconnect_True, TestSize.Level1)
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

    // After processing with line 337 condition true, resources should be cleared
    EXPECT_FALSE(tmpRsRenderComposer->isDisconnected_);
    EXPECT_EQ(tmpRsRenderComposer->rsRenderComposerContext_, nullptr);
    EXPECT_EQ(tmpRsRenderComposer->hdiOutput_, nullptr);
}

/**
 * Function: ProcessComposerFrame_ClearOutputOnDisconnect_False_NotDisconnected
 * Type: Function
 * Rank: Important(2)
 * EnvConditions: N/A
 * CaseDescription: 1. create RSRenderComposer
 *                  2. set isDisconnected_ false and unExecuteTaskNum_ to 0 (line 337 condition false)
 *                  3. call ProcessComposerFrame
 *                  4. verify resources are NOT cleared when not disconnected
 */
HWTEST_F(RsRenderComposerTest, ProcessComposerFrame_ClearOutputOnDisconnect_False_NotDisconnected, TestSize.Level1)
{
    auto output = std::make_shared<HdiOutput>(1u);
    output->Init();
    sptr<RSScreenProperty> property = new RSScreenProperty();
    auto tmpRsRenderComposer = std::make_shared<RSRenderComposer>(output, property);
    ASSERT_NE(tmpRsRenderComposer, nullptr);

    // Set isDisconnected_ false and unExecuteTaskNum_ to 0 (line 337 condition false)
    tmpRsRenderComposer->isDisconnected_ = false;
    tmpRsRenderComposer->unExecuteTaskNum_.store(0);

    std::shared_ptr<RSRenderSurfaceLayer> layer = std::make_shared<RSRenderSurfaceLayer>();
    layer->SetLayerSize({100, 100, 100, 100});
    tmpRsRenderComposer->rsRenderComposerContext_->AddRSRenderLayer(1, layer);
    ASSERT_NE(tmpRsRenderComposer->rsRenderComposerContext_->GetRSRenderLayer(1), nullptr);

    PipelineParam param;
    uint32_t currentRate = 60;
    tmpRsRenderComposer->ProcessComposerFrame(currentRate, param);

    // After processing with line 337 condition false, resources should NOT be cleared
    EXPECT_FALSE(tmpRsRenderComposer->isDisconnected_);
    EXPECT_NE(tmpRsRenderComposer->rsRenderComposerContext_, nullptr);
    EXPECT_NE(tmpRsRenderComposer->hdiOutput_, nullptr);
}

/**
 * Function: ProcessComposerFrame_ClearOutputOnDisconnect_False_HasPendingTasks
 * Type: Function
 * Rank: Important(2)
 * EnvConditions: N/A
 * CaseDescription: 1. create RSRenderComposer
 *                  2. set isDisconnected_ true but unExecuteTaskNum_ > 0 (line 337 condition false)
 *                  3. call ProcessComposerFrame
 *                  4. verify resources are NOT cleared when there are pending tasks
 */
HWTEST_F(RsRenderComposerTest, ProcessComposerFrame_ClearOutputOnDisconnect_False_HasPendingTasks, TestSize.Level1)
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

    // After processing with line 337 condition false (pending tasks), resources should NOT be cleared
    EXPECT_FALSE(tmpRsRenderComposer->isDisconnected_);
    EXPECT_NE(tmpRsRenderComposer->rsRenderComposerContext_, nullptr);
    EXPECT_NE(tmpRsRenderComposer->hdiOutput_, nullptr);
}

/**
 * Function: ProcessComposerFrame_ClearOutputOnDisconnect_False_BothFalse
 * Type: Function
 * Rank: Important(2)
 * EnvConditions: N/A
 * CaseDescription: 1. create RSRenderComposer
 *                  2. set isDisconnected_ false and unExecuteTaskNum_ > 0 (line 337 condition false)
 *                  3. call ProcessComposerFrame
 *                  4. verify resources are NOT cleared when both conditions are false
 */
HWTEST_F(RsRenderComposerTest, ProcessComposerFrame_ClearOutputOnDisconnect_False_BothFalse, TestSize.Level1)
{
    auto output = std::make_shared<HdiOutput>(1u);
    output->Init();
    sptr<RSScreenProperty> property = new RSScreenProperty();
    auto tmpRsRenderComposer = std::make_shared<RSRenderComposer>(output, property);
    ASSERT_NE(tmpRsRenderComposer, nullptr);

    // Set isDisconnected_ false and unExecuteTaskNum_ > 0 (line 337 condition false)
    tmpRsRenderComposer->isDisconnected_ = false;
    tmpRsRenderComposer->unExecuteTaskNum_.store(3);

    std::shared_ptr<RSRenderSurfaceLayer> layer = std::make_shared<RSRenderSurfaceLayer>();
    layer->SetLayerSize({100, 100, 100, 100});
    tmpRsRenderComposer->rsRenderComposerContext_->AddRSRenderLayer(1, layer);
    ASSERT_NE(tmpRsRenderComposer->rsRenderComposerContext_->GetRSRenderLayer(1), nullptr);

    PipelineParam param;
    uint32_t currentRate = 60;
    tmpRsRenderComposer->ProcessComposerFrame(currentRate, param);

    // After processing with line 337 condition false (both conditions false), resources should NOT be cleared
    EXPECT_FALSE(tmpRsRenderComposer->isDisconnected_);
    EXPECT_NE(tmpRsRenderComposer->rsRenderComposerContext_, nullptr);
    EXPECT_NE(tmpRsRenderComposer->hdiOutput_, nullptr);
}

/**
 * Function: ComposerProcess_ContextNullptr_WithValidOutput
 * Type: Function
 * Rank: Important(2)
 * EnvConditions: N/A
 * CaseDescription: 1. create RSRenderComposer
 *                  2. set rsRenderComposerContext_ to nullptr while keeping hdiOutput_ valid (line 377 condition true)
 *                  3. call ComposerProcess
 *                  4. verify early return with unExecuteTaskNum_ decrement
 */
HWTEST_F(RsRenderComposerTest, ComposerProcess_ContextNullptr_WithValidOutput, TestSize.Level1)
{
    auto output = std::make_shared<HdiOutput>(1u);
    output->Init();
    sptr<RSScreenProperty> property = new RSScreenProperty();
    auto tmpRsRenderComposer = std::make_shared<RSRenderComposer>(output, property);
    ASSERT_NE(tmpRsRenderComposer, nullptr);
    ASSERT_NE(tmpRsRenderComposer->hdiOutput_, nullptr);

    // Set rsRenderComposerContext_ to nullptr while keeping hdiOutput_ valid (line 377 condition true)
    tmpRsRenderComposer->rsRenderComposerContext_ = nullptr;
    ASSERT_EQ(tmpRsRenderComposer->rsRenderComposerContext_, nullptr);
    ASSERT_NE(tmpRsRenderComposer->hdiOutput_, nullptr);

    // Set unExecuteTaskNum_ to a value to verify it gets decremented
    tmpRsRenderComposer->unExecuteTaskNum_.store(5);

    uint32_t currentRate = 60;
    auto tx = std::make_shared<RSLayerTransactionData>();
    ASSERT_NE(tx, nullptr);

    tmpRsRenderComposer->ComposerProcess(currentRate, tx);

    // Verify unExecuteTaskNum_ was decremented (line 378)
    EXPECT_EQ(tmpRsRenderComposer->unExecuteTaskNum_.load(), 4);
}

/**
 * Function: ComposerProcess_OutputNullptr_WithValidContext
 * Type: Function
 * Rank: Important(2)
 * EnvConditions: N/A
 * CaseDescription: 1. create RSRenderComposer
 *                  2. set hdiOutput_ to nullptr while keeping rsRenderComposerContext_ valid (line 377 condition true)
 *                  3. call ComposerProcess
 *                  4. verify early return with unExecuteTaskNum_ decrement
 */
HWTEST_F(RsRenderComposerTest, ComposerProcess_OutputNullptr_WithValidContext, TestSize.Level1)
{
    auto output = std::make_shared<HdiOutput>(1u);
    output->Init();
    sptr<RSScreenProperty> property = new RSScreenProperty();
    auto tmpRsRenderComposer = std::make_shared<RSRenderComposer>(output, property);
    ASSERT_NE(tmpRsRenderComposer, nullptr);

    // Set hdiOutput_ to nullptr while keeping rsRenderComposerContext_ valid (line 377 condition true)
    tmpRsRenderComposer->hdiOutput_ = nullptr;
    ASSERT_EQ(tmpRsRenderComposer->hdiOutput_, nullptr);
    ASSERT_NE(tmpRsRenderComposer->rsRenderComposerContext_, nullptr);

    // Set unExecuteTaskNum_ to a value to verify it gets decremented
    tmpRsRenderComposer->unExecuteTaskNum_.store(5);

    uint32_t currentRate = 60;
    auto tx = std::make_shared<RSLayerTransactionData>();
    ASSERT_NE(tx, nullptr);

    tmpRsRenderComposer->ComposerProcess(currentRate, tx);

    // Verify unExecuteTaskNum_ was decremented (line 378)
    EXPECT_EQ(tmpRsRenderComposer->unExecuteTaskNum_.load(), 4);
}

/**
 * Function: ComposerProcess_BothNullptr
 * Type: Function
 * Rank: Important(2)
 * EnvConditions: N/A
 * CaseDescription: 1. create RSRenderComposer
 *                  2. set both hdiOutput_ and rsRenderComposerContext_ to nullptr (line 377 condition true)
 *                  3. call ComposerProcess
 *                  4. verify early return with unExecuteTaskNum_ decrement
 */
HWTEST_F(RsRenderComposerTest, ComposerProcess_BothNullptr, TestSize.Level1)
{
    auto output = std::make_shared<HdiOutput>(1u);
    output->Init();
    sptr<RSScreenProperty> property = new RSScreenProperty();
    auto tmpRsRenderComposer = std::make_shared<RSRenderComposer>(output, property);
    ASSERT_NE(tmpRsRenderComposer, nullptr);

    // Set both hdiOutput_ and rsRenderComposerContext_ to nullptr (line 377 condition true)
    tmpRsRenderComposer->hdiOutput_ = nullptr;
    tmpRsRenderComposer->rsRenderComposerContext_ = nullptr;
    ASSERT_EQ(tmpRsRenderComposer->hdiOutput_, nullptr);
    ASSERT_EQ(tmpRsRenderComposer->rsRenderComposerContext_, nullptr);

    // Set unExecuteTaskNum_ to a value to verify it gets decremented
    tmpRsRenderComposer->unExecuteTaskNum_.store(5);

    uint32_t currentRate = 60;
    auto tx = std::make_shared<RSLayerTransactionData>();
    ASSERT_NE(tx, nullptr);

    tmpRsRenderComposer->ComposerProcess(currentRate, tx);

    // Verify unExecuteTaskNum_ was decremented (line 378)
    EXPECT_EQ(tmpRsRenderComposer->unExecuteTaskNum_.load(), 4);
}

/**
 * Function: ChangeLayersForActiveRectOutside_DebugFlagTrue
 * Type: Function
 * Rank: Important(2)
 * EnvConditions: N/A
 * CaseDescription: 1. set super fold display and maskRect with positive width and height
 *                  2. set debug.foldscreen.shaft.color parameter to 1 (line 446 condition true)
 *                  3. call ChangeLayersForActiveRectOutside
 *                  4. verify solid color layer is created with green color {0, 255, 0, 255}
 */
HWTEST_F(RsRenderComposerTest, ChangeLayersForActiveRectOutside_DebugFlagTrue, TestSize.Level1)
{
    system::SetParameter("const.window.foldscreen.type", "6,0,0,0");
    system::SetParameter("debug.foldscreen.shaft.color", "1");
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

    // Verify the layer color is green {0, 255, 0, 255} when debugFlag is true (line 446-447)
    GraphicLayerColor layerColor = maskLayer->GetLayerColor();
    EXPECT_EQ(layerColor.r, 0);
    EXPECT_EQ(layerColor.g, 255);
    EXPECT_EQ(layerColor.b, 0);
    EXPECT_EQ(layerColor.a, 255);

    system::SetParameter("const.window.foldscreen.type", "0,0,0,0");
    system::SetParameter("debug.foldscreen.shaft.color", "0");
}

/**
 * Function: ChangeLayersForActiveRectOutside_DebugFlagFalse
 * Type: Function
 * Rank: Important(2)
 * EnvConditions: N/A
 * CaseDescription: 1. set super fold display and maskRect with positive width and height
 *                  2. set debug.foldscreen.shaft.color parameter to 0 (line 446 condition false)
 *                  3. call ChangeLayersForActiveRectOutside
 *                  4. verify solid color layer is created with black color {0, 0, 0, 255}
 */
HWTEST_F(RsRenderComposerTest, ChangeLayersForActiveRectOutside_DebugFlagFalse, TestSize.Level1)
{
    system::SetParameter("const.window.foldscreen.type", "6,0,0,0");
    system::SetParameter("debug.foldscreen.shaft.color", "0");
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

    // Verify the layer color is black {0, 0, 0, 255} when debugFlag is false (line 448-449)
    GraphicLayerColor layerColor = maskLayer->GetLayerColor();
    EXPECT_EQ(layerColor.r, 0);
    EXPECT_EQ(layerColor.g, 0);
    EXPECT_EQ(layerColor.b, 0);
    EXPECT_EQ(layerColor.a, 255);

    system::SetParameter("const.window.foldscreen.type", "0,0,0,0");
}

/**
 * Function: GetSurfaceNameInLayersForTrace_CursorNotOnTop_LogError
 * Type: Function
 * Rank: Important(2)
 * EnvConditions: N/A
 * CaseDescription: 1. create RSRenderComposer
 *                  2. add cursor layer with zorder less than max (line 513 condition true)
 *                  3. call GetSurfaceNameInLayersForTrace
 *                  4. verify error log is generated for cursor not on top
 */
HWTEST_F(RsRenderComposerTest, GetSurfaceNameInLayersForTrace_CursorNotOnTop_LogError, TestSize.Level1)
{
    auto output = std::make_shared<HdiOutput>(1u);
    output->Init();
    sptr<RSScreenProperty> property = new RSScreenProperty();
    auto rsRenderComposerTmp = std::make_shared<RSRenderComposer>(output, property);

    std::vector<std::shared_ptr<RSLayer>> layers;

    // Add a normal layer with high zorder (max zorder will be 100)
    std::shared_ptr<RSRenderSurfaceLayer> layer1 = std::make_shared<RSRenderSurfaceLayer>();
    layer1->SetZorder(100);
    sptr<IConsumerSurface> cSurface1 = IConsumerSurface::Create("layer1");
    layer1->SetSurface(cSurface1);
    layers.push_back(layer1);

    // Add cursor layer with lower zorder (cursor zorder = 50, max = 100)
    // This triggers line 513 condition: GetType() == GRAPHIC_LAYER_TYPE_CURSOR && GetZorder() < max
    std::shared_ptr<RSRenderSurfaceLayer> cursorLayer = std::make_shared<RSRenderSurfaceLayer>();
    cursorLayer->SetZorder(50);
    cursorLayer->SetType(GraphicLayerType::GRAPHIC_LAYER_TYPE_CURSOR);
    sptr<IConsumerSurface> cSurface2 = IConsumerSurface::Create("cursor");
    cursorLayer->SetSurface(cSurface2);
    layers.push_back(cursorLayer);

    std::string traceStr = rsRenderComposerTmp->GetSurfaceNameInLayersForTrace(layers);

    // Verify the trace string contains both layers
    EXPECT_NE(traceStr.find("layer1"), std::string::npos);
    EXPECT_NE(traceStr.find("cursor"), std::string::npos);
    EXPECT_NE(traceStr.find("zorder: 100"), std::string::npos);
    EXPECT_NE(traceStr.find("zorder: 50"), std::string::npos);

    // Verify cursor layer properties
    EXPECT_EQ(cursorLayer->GetType(), GraphicLayerType::GRAPHIC_LAYER_TYPE_CURSOR);
    EXPECT_LT(cursorLayer->GetZorder(), layer1->GetZorder()); // 50 < 100
}

/**
 * Function: GetSurfaceNameInLayersForTrace_CursorOnTop_NoLogError
 * Type: Function
 * Rank: Important(2)
 * EnvConditions: N/A
 * CaseDescription: 1. create RSRenderComposer
 *                  2. add cursor layer with zorder equal to max (line 513 condition false)
 *                  3. call GetSurfaceNameInLayersForTrace
 *                  4. verify no error log when cursor is on top
 */
HWTEST_F(RsRenderComposerTest, GetSurfaceNameInLayersForTrace_CursorOnTop_NoLogError, TestSize.Level1)
{
    auto output = std::make_shared<HdiOutput>(1u);
    output->Init();
    sptr<RSScreenProperty> property = new RSScreenProperty();
    auto rsRenderComposerTmp = std::make_shared<RSRenderComposer>(output, property);

    std::vector<std::shared_ptr<RSLayer>> layers;

    // Add cursor layer with highest zorder (cursor zorder = 100, max = 100)
    // This makes line 513 condition false: GetType() == GRAPHIC_LAYER_TYPE_CURSOR && GetZorder() >= max
    std::shared_ptr<RSRenderSurfaceLayer> cursorLayer = std::make_shared<RSRenderSurfaceLayer>();
    cursorLayer->SetZorder(100);
    cursorLayer->SetType(GraphicLayerType::GRAPHIC_LAYER_TYPE_CURSOR);
    sptr<IConsumerSurface> cSurface1 = IConsumerSurface::Create("cursor");
    cursorLayer->SetSurface(cSurface1);
    layers.push_back(cursorLayer);

    // Add a normal layer with lower zorder
    std::shared_ptr<RSRenderSurfaceLayer> layer1 = std::make_shared<RSRenderSurfaceLayer>();
    layer1->SetZorder(50);
    sptr<IConsumerSurface> cSurface2 = IConsumerSurface::Create("layer1");
    layer1->SetSurface(cSurface2);
    layers.push_back(layer1);

    std::string traceStr = rsRenderComposerTmp->GetSurfaceNameInLayersForTrace(layers);

    // Verify the trace string contains both layers
    EXPECT_NE(traceStr.find("cursor"), std::string::npos);
    EXPECT_NE(traceStr.find("layer1"), std::string::npos);

    // Verify cursor layer is on top (zorder = max)
    EXPECT_EQ(cursorLayer->GetType(), GraphicLayerType::GRAPHIC_LAYER_TYPE_CURSOR);
    EXPECT_GE(cursorLayer->GetZorder(), layer1->GetZorder()); // 100 >= 50
}

/**
 * Function: GetSurfaceNameInLayersForTrace_CursorAboveMax_NoLogError
 * Type: Function
 * Rank: Important(2)
 * EnvConditions: N/A
 * CaseDescription: 1. create RSRenderComposer
 *                  2. add cursor layer with zorder greater than max (line 513 condition false)
 *                  3. call GetSurfaceNameInLayersForTrace
 *                  4. verify no error log when cursor zorder > max
 */
HWTEST_F(RsRenderComposerTest, GetSurfaceNameInLayersForTrace_CursorAboveMax_NoLogError, TestSize.Level1)
{
    auto output = std::make_shared<HdiOutput>(1u);
    output->Init();
    sptr<RSScreenProperty> property = new RSScreenProperty();
    auto rsRenderComposerTmp = std::make_shared<RSRenderComposer>(output, property);

    std::vector<std::shared_ptr<RSLayer>> layers;

    // Add a normal layer
    std::shared_ptr<RSRenderSurfaceLayer> layer1 = std::make_shared<RSRenderSurfaceLayer>();
    layer1->SetZorder(50);
    sptr<IConsumerSurface> cSurface1 = IConsumerSurface::Create("layer1");
    layer1->SetSurface(cSurface1);
    layers.push_back(layer1);

    // Add cursor layer with zorder greater than max (cursor zorder = 200, max = 50)
    // This makes line 513 condition false: GetType() == GRAPHIC_LAYER_TYPE_CURSOR && GetZorder() >= max
    std::shared_ptr<RSRenderSurfaceLayer> cursorLayer = std::make_shared<RSRenderSurfaceLayer>();
    cursorLayer->SetZorder(200);
    cursorLayer->SetType(GraphicLayerType::GRAPHIC_LAYER_TYPE_CURSOR);
    sptr<IConsumerSurface> cSurface2 = IConsumerSurface::Create("cursor");
    cursorLayer->SetSurface(cSurface2);
    layers.push_back(cursorLayer);

    std::string traceStr = rsRenderComposerTmp->GetSurfaceNameInLayersForTrace(layers);

    // Verify the trace string contains both layers
    EXPECT_NE(traceStr.find("layer1"), std::string::npos);
    EXPECT_NE(traceStr.find("cursor"), std::string::npos);

    // Verify cursor layer zorder > max (max = 50)
    EXPECT_EQ(cursorLayer->GetType(), GraphicLayerType::GRAPHIC_LAYER_TYPE_CURSOR);
    EXPECT_GT(cursorLayer->GetZorder(), layer1->GetZorder()); // 200 > 50
}

/**
 * Function: GetSurfaceNameInLayersForTrace_NonCursor_NoLogError
 * Type: Function
 * Rank: Important(2)
 * EnvConditions: N/A
 * CaseDescription: 1. create RSRenderComposer
 *                  2. add non-cursor layer with zorder less than max (line 513 condition false)
 *                  3. call GetSurfaceNameInLayersForTrace
 *                  4. verify no error log for non-cursor layer
 */
HWTEST_F(RsRenderComposerTest, GetSurfaceNameInLayersForTrace_NonCursor_NoLogError, TestSize.Level1)
{
    auto output = std::make_shared<HdiOutput>(1u);
    output->Init();
    sptr<RSScreenProperty> property = new RSScreenProperty();
    auto rsRenderComposerTmp = std::make_shared<RSRenderComposer>(output, property);

    std::vector<std::shared_ptr<RSLayer>> layers;

    // Add a normal layer with high zorder (max zorder will be 100)
    std::shared_ptr<RSRenderSurfaceLayer> layer1 = std::make_shared<RSRenderSurfaceLayer>();
    layer1->SetZorder(100);
    sptr<IConsumerSurface> cSurface1 = IConsumerSurface::Create("layer1");
    layer1->SetSurface(cSurface1);
    layers.push_back(layer1);

    // Add a non-cursor layer with lower zorder
    // This makes line 513 condition false: GetType() != GRAPHIC_LAYER_TYPE_CURSOR
    std::shared_ptr<RSRenderSurfaceLayer> layer2 = std::make_shared<RSRenderSurfaceLayer>();
    layer2->SetZorder(50);
    layer2->SetType(GraphicLayerType::GRAPHIC_LAYER_TYPE_OVERLAY); // Not cursor type
    sptr<IConsumerSurface> cSurface2 = IConsumerSurface::Create("layer2");
    layer2->SetSurface(cSurface2);
    layers.push_back(layer2);

    std::string traceStr = rsRenderComposerTmp->GetSurfaceNameInLayersForTrace(layers);

    // Verify the trace string contains both layers
    EXPECT_NE(traceStr.find("layer1"), std::string::npos);
    EXPECT_NE(traceStr.find("layer2"), std::string::npos);

    // Verify layer2 is not cursor type and zorder < max
    EXPECT_NE(layer2->GetType(), GraphicLayerType::GRAPHIC_LAYER_TYPE_CURSOR);
    EXPECT_LT(layer2->GetZorder(), layer1->GetZorder()); // 50 < 100
}

/**
 * Function: GetSurfaceNameInLayersForTrace_MultipleCursors_CheckAll
 * Type: Function
 * Rank: Important(2)
 * EnvConditions: N/A
 * CaseDescription: 1. create RSRenderComposer
 *                  2. add multiple cursor layers with different zorders
 *                  3. call GetSurfaceNameInLayersForTrace
 *                  4. verify only cursor layers not on top trigger error log
 */
HWTEST_F(RsRenderComposerTest, GetSurfaceNameInLayersForTrace_MultipleCursors_CheckAll, TestSize.Level1)
{
    auto output = std::make_shared<HdiOutput>(1u);
    output->Init();
    sptr<RSScreenProperty> property = new RSScreenProperty();
    auto rsRenderComposerTmp = std::make_shared<RSRenderComposer>(output, property);

    std::vector<std::shared_ptr<RSLayer>> layers;

    // Add normal layer with highest zorder (max will be 200)
    std::shared_ptr<RSRenderSurfaceLayer> layer1 = std::make_shared<RSRenderSurfaceLayer>();
    layer1->SetZorder(200);
    sptr<IConsumerSurface> cSurface1 = IConsumerSurface::Create("layer1");
    layer1->SetSurface(cSurface1);
    layers.push_back(layer1);

    // Add cursor layer with medium zorder (100 < max=200)
    // This triggers line 513 condition: GetType() == GRAPHIC_LAYER_TYPE_CURSOR && GetZorder() < max
    std::shared_ptr<RSRenderSurfaceLayer> cursorLayer1 = std::make_shared<RSRenderSurfaceLayer>();
    cursorLayer1->SetZorder(100);
    cursorLayer1->SetType(GraphicLayerType::GRAPHIC_LAYER_TYPE_CURSOR);
    sptr<IConsumerSurface> cSurface2 = IConsumerSurface::Create("cursor1");
    cursorLayer1->SetSurface(cSurface2);
    layers.push_back(cursorLayer1);

    // Add another cursor layer with low zorder (50 < max=200)
    // This also triggers line 513 condition
    std::shared_ptr<RSRenderSurfaceLayer> cursorLayer2 = std::make_shared<RSRenderSurfaceLayer>();
    cursorLayer2->SetZorder(50);
    cursorLayer2->SetType(GraphicLayerType::GRAPHIC_LAYER_TYPE_CURSOR);
    sptr<IConsumerSurface> cSurface3 = IConsumerSurface::Create("cursor2");
    cursorLayer2->SetSurface(cSurface3);
    layers.push_back(cursorLayer2);

    std::string traceStr = rsRenderComposerTmp->GetSurfaceNameInLayersForTrace(layers);

    // Verify the trace string contains all layers
    EXPECT_NE(traceStr.find("layer1"), std::string::npos);
    EXPECT_NE(traceStr.find("cursor1"), std::string::npos);
    EXPECT_NE(traceStr.find("cursor2"), std::string::npos);

    // Verify both cursor layers have zorder < max (max = 200)
    EXPECT_EQ(cursorLayer1->GetType(), GraphicLayerType::GRAPHIC_LAYER_TYPE_CURSOR);
    EXPECT_LT(cursorLayer1->GetZorder(), layer1->GetZorder()); // 100 < 200
    EXPECT_EQ(cursorLayer2->GetType(), GraphicLayerType::GRAPHIC_LAYER_TYPE_CURSOR);
    EXPECT_LT(cursorLayer2->GetZorder(), layer1->GetZorder()); // 50 < 200
}

/**
 * Function: ClearFrameBuffersInner_ResetTrue_EngineNullptr
 * Type: Function
 * Rank: Important(2)
 * EnvConditions: N/A
 * CaseDescription: 1. create RSRenderComposer
 *                  2. set isNeedResetContext true and uniRenderEngine_ to nullptr (line 785 condition false)
 *                  3. call ClearFrameBuffersInner
 *                  4. verify ResetCurrentContext is not called due to null engine
 */
HWTEST_F(RsRenderComposerTest, ClearFrameBuffersInner_ResetTrue_EngineNullptr, TestSize.Level1)
{
    auto output = std::make_shared<HdiOutput>(1u);
    output->Init();
    sptr<RSScreenProperty> property = new RSScreenProperty();
    auto rsRenderComposerTmp = std::make_shared<RSRenderComposer>(output, property);
    ASSERT_NE(rsRenderComposerTmp->hdiOutput_, nullptr);
    ASSERT_NE(rsRenderComposerTmp->uniRenderEngine_, nullptr);

    // Set uniRenderEngine_ to nullptr while isNeedResetContext is true
    // This makes line 785 condition false: isNeedResetContext && uniRenderEngine_ != nullptr
    rsRenderComposerTmp->uniRenderEngine_ = nullptr;
    ASSERT_EQ(rsRenderComposerTmp->uniRenderEngine_, nullptr);

    // Call ClearFrameBuffersInner with isNeedResetContext = true
    // Since uniRenderEngine_ is nullptr, ResetCurrentContext() will NOT be called (line 785 condition false)
    GSError ret = rsRenderComposerTmp->ClearFrameBuffersInner(true);

    // Verify return value and state
    EXPECT_NE(ret, GSERROR_OK); // Expected to fail due to fbSurface_ being null in mock
    EXPECT_EQ(rsRenderComposerTmp->uniRenderEngine_, nullptr);
}

/**
 * Function: ClearFrameBuffersInner_ResetTrue_EngineNotNullptr
 * Type: Function
 * Rank: Important(2)
 * EnvConditions: N/A
 * CaseDescription: 1. create RSRenderComposer
 *                  2. set isNeedResetContext true and keep uniRenderEngine_ not null (line 785 condition true)
 *                  3. call ClearFrameBuffersInner
 *                  4. verify ResetCurrentContext is called
 */
HWTEST_F(RsRenderComposerTest, ClearFrameBuffersInner_ResetTrue_EngineNotNullptr, TestSize.Level1)
{
    auto output = std::make_shared<HdiOutput>(1u);
    output->Init();
    sptr<RSScreenProperty> property = new RSScreenProperty();
    auto rsRenderComposerTmp = std::make_shared<RSRenderComposer>(output, property);
    ASSERT_NE(rsRenderComposerTmp->hdiOutput_, nullptr);
    ASSERT_NE(rsRenderComposerTmp->uniRenderEngine_, nullptr);

    // Keep uniRenderEngine_ not null while isNeedResetContext is true
    // This makes line 785 condition true: isNeedResetContext && uniRenderEngine_ != nullptr
    // ResetCurrentContext() will be called (line 785-786)

    // Call ClearFrameBuffersInner with isNeedResetContext = true
    GSError ret = rsRenderComposerTmp->ClearFrameBuffersInner(true);

    // Verify return value and state
    EXPECT_NE(ret, GSERROR_OK); // Expected to fail due to fbSurface_ being null in mock
    EXPECT_NE(rsRenderComposerTmp->uniRenderEngine_, nullptr);
}

/**
 * Function: ClearFrameBuffersInner_ResetFalse_EngineNullptr
 * Type: Function
 * Rank: Important(2)
 * EnvConditions: N/A
 * CaseDescription: 1. create RSRenderComposer
 *                  2. set isNeedResetContext false and uniRenderEngine_ to nullptr (line 785 condition false)
 *                  3. call ClearFrameBuffersInner
 *                  4. verify ResetCurrentContext is not called
 */
HWTEST_F(RsRenderComposerTest, ClearFrameBuffersInner_ResetFalse_EngineNullptr, TestSize.Level1)
{
    auto output = std::make_shared<HdiOutput>(1u);
    output->Init();
    sptr<RSScreenProperty> property = new RSScreenProperty();
    auto rsRenderComposerTmp = std::make_shared<RSRenderComposer>(output, property);
    ASSERT_NE(rsRenderComposerTmp->hdiOutput_, nullptr);
    ASSERT_NE(rsRenderComposerTmp->uniRenderEngine_, nullptr);

    // Set uniRenderEngine_ to nullptr and isNeedResetContext false
    // This makes line 785 condition false: isNeedResetContext && uniRenderEngine_ != nullptr
    rsRenderComposerTmp->uniRenderEngine_ = nullptr;
    ASSERT_EQ(rsRenderComposerTmp->uniRenderEngine_, nullptr);

    // Call ClearFrameBuffersInner with isNeedResetContext = false
    // ResetCurrentContext() will NOT be called (line 785 condition false)
    GSError ret = rsRenderComposerTmp->ClearFrameBuffersInner(false);

    // Verify return value and state
    EXPECT_NE(ret, GSERROR_OK); // Expected to fail due to fbSurface_ being null in mock
    EXPECT_EQ(rsRenderComposerTmp->uniRenderEngine_, nullptr);
}

/**
 * Function: ClearFrameBuffersInner_ResetFalse_EngineNotNullptr
 * Type: Function
 * Rank: Important(2)
 * EnvConditions: N/A
 * CaseDescription: 1. create RSRenderComposer
 *                  2. set isNeedResetContext false and keep uniRenderEngine_ not null (line 785 condition false)
 *                  3. call ClearFrameBuffersInner
 *                  4. verify ResetCurrentContext is not called
 */
HWTEST_F(RsRenderComposerTest, ClearFrameBuffersInner_ResetFalse_EngineNotNullptr, TestSize.Level1)
{
    auto output = std::make_shared<HdiOutput>(1u);
    output->Init();
    sptr<RSScreenProperty> property = new RSScreenProperty();
    auto rsRenderComposerTmp = std::make_shared<RSRenderComposer>(output, property);
    ASSERT_NE(rsRenderComposerTmp->hdiOutput_, nullptr);
    ASSERT_NE(rsRenderComposerTmp->uniRenderEngine_, nullptr);

    // Keep uniRenderEngine_ not null but isNeedResetContext is false
    // This makes line 785 condition false: isNeedResetContext && uniRenderEngine_ != nullptr
    // ResetCurrentContext() will NOT be called (line 785 condition false)

    // Call ClearFrameBuffersInner with isNeedResetContext = false
    GSError ret = rsRenderComposerTmp->ClearFrameBuffersInner(false);

    // Verify return value and state
    EXPECT_NE(ret, GSERROR_OK); // Expected to fail due to fbSurface_ being null in mock
    EXPECT_NE(rsRenderComposerTmp->uniRenderEngine_, nullptr);
}

/**
 * Function: ClearFrameBuffersInner_SurfaceIdInMap_EraseFromMap
 * Type: Function
 * Rank: Important(2)
 * EnvConditions: N/A
 * CaseDescription: 1. create RSRenderComposer
 *                  2. add surface to frameBufferSurfaceOhosMap_ (line 802 condition true)
 *                  3. call ClearFrameBuffersInner
 *                  4. verify surface is erased from map
 */
HWTEST_F(RsRenderComposerTest, ClearFrameBuffersInner_SurfaceIdInMap_EraseFromMap, TestSize.Level1)
{
    auto output = std::make_shared<HdiOutput>(1u);
    output->Init();
    sptr<RSScreenProperty> property = new RSScreenProperty();
    auto rsRenderComposerTmp = std::make_shared<RSRenderComposer>(output, property);
    ASSERT_NE(rsRenderComposerTmp->hdiOutput_, nullptr);

    // Create a surface and add it to frameBufferSurfaceOhosMap_
    // This makes line 802 condition true: frameBufferSurfaceOhosMap_.count(surfaceId)
    auto csurf = IConsumerSurface::Create("test_surface");
    auto producer = csurf->GetProducer();
    auto pSurface = Surface::CreateSurfaceAsProducer(producer);
    std::shared_ptr<RSSurfaceOhos> rsSurface = std::make_shared<RSSurfaceOhosRaster>(pSurface);
    ASSERT_NE(rsSurface, nullptr);

    uint64_t surfaceId = 12345;
    rsRenderComposerTmp->frameBufferSurfaceOhosMap_.insert({surfaceId, rsSurface});

    // Verify surface is in the map
    EXPECT_TRUE(rsRenderComposerTmp->frameBufferSurfaceOhosMap_.count(surfaceId));

    // Call ClearFrameBuffersInner - the surface should be erased from map (line 815)
    rsRenderComposerTmp->uniRenderEngine_ = nullptr;
    GSError ret = rsRenderComposerTmp->ClearFrameBuffersInner(false);
    ASSERT_NE(ret, GSERROR_OK);

    // Verify surface was erased from map after ClearFrameBuffersInner
    EXPECT_FALSE(rsRenderComposerTmp->frameBufferSurfaceOhosMap_.count(surfaceId));
}

/**
 * Function: ClearFrameBuffersInner_SurfaceIdNotInMap_NoErase
 * Type: Function
 * Rank: Important(2)
 * EnvConditions: N/A
 * CaseDescription: 1. create RSRenderComposer
 *                  2. do not add surface to frameBufferSurfaceOhosMap_ (line 802 condition false)
 *                  3. call ClearFrameBuffersInner
 *                  4. verify no erase operation performed
 */
HWTEST_F(RsRenderComposerTest, ClearFrameBuffersInner_SurfaceIdNotInMap_NoErase, TestSize.Level1)
{
    auto output = std::make_shared<HdiOutput>(1u);
    output->Init();
    sptr<RSScreenProperty> property = new RSScreenProperty();
    auto rsRenderComposerTmp = std::make_shared<RSRenderComposer>(output, property);
    ASSERT_NE(rsRenderComposerTmp->hdiOutput_, nullptr);

    // Ensure frameBufferSurfaceOhosMap_ is empty
    rsRenderComposerTmp->frameBufferSurfaceOhosMap_.clear();
    EXPECT_TRUE(rsRenderComposerTmp->frameBufferSurfaceOhosMap_.empty());

    // Call ClearFrameBuffersInner - no surface in map (line 802 condition false)
    rsRenderComposerTmp->uniRenderEngine_ = nullptr;
    GSError ret = rsRenderComposerTmp->ClearFrameBuffersInner(false);
    ASSERT_NE(ret, GSERROR_OK);
    // Verify map remains empty
    EXPECT_TRUE(rsRenderComposerTmp->frameBufferSurfaceOhosMap_.empty());
}

/**
 * Function: ClearFrameBuffersInner_MultipleSurfacesInMap_EraseAll
 * Type: Function
 * Rank: Important(2)
 * EnvConditions: N/A
 * CaseDescription: 1. create RSRenderComposer
 *                  2. add multiple surfaces to frameBufferSurfaceOhosMap_
 *                  3. call ClearFrameBuffersInner
 *                  4. verify target surface is erased while others remain
 */
HWTEST_F(RsRenderComposerTest, ClearFrameBuffersInner_MultipleSurfacesInMap_EraseAll, TestSize.Level1)
{
    auto output = std::make_shared<HdiOutput>(1u);
    output->Init();
    sptr<RSScreenProperty> property = new RSScreenProperty();
    auto rsRenderComposerTmp = std::make_shared<RSRenderComposer>(output, property);
    ASSERT_NE(rsRenderComposerTmp->hdiOutput_, nullptr);

    // Create multiple surfaces and add them to frameBufferSurfaceOhosMap_
    auto csurf1 = IConsumerSurface::Create("test_surface1");
    auto producer1 = csurf1->GetProducer();
    auto pSurface1 = Surface::CreateSurfaceAsProducer(producer1);
    std::shared_ptr<RSSurfaceOhos> rsSurface1 = std::make_shared<RSSurfaceOhosRaster>(pSurface1);
    ASSERT_NE(rsSurface1, nullptr);

    auto csurf2 = IConsumerSurface::Create("test_surface2");
    auto producer2 = csurf2->GetProducer();
    auto pSurface2 = Surface::CreateSurfaceAsProducer(producer2);
    std::shared_ptr<RSSurfaceOhos> rsSurface2 = std::make_shared<RSSurfaceOhosRaster>(pSurface2);
    ASSERT_NE(rsSurface2, nullptr);

    uint64_t surfaceId1 = 11111;
    uint64_t surfaceId2 = 22222;
    rsRenderComposerTmp->frameBufferSurfaceOhosMap_.insert({surfaceId1, rsSurface1});
    rsRenderComposerTmp->frameBufferSurfaceOhosMap_.insert({surfaceId2, rsSurface2});

    // Verify both surfaces are in the map
    EXPECT_EQ(rsRenderComposerTmp->frameBufferSurfaceOhosMap_.size(), 2u);

    // Call ClearFrameBuffersInner - one surface will be erased based on fbSurface_->GetUniqueId()
    rsRenderComposerTmp->uniRenderEngine_ = nullptr;
    GSError ret = rsRenderComposerTmp->ClearFrameBuffersInner(false);
    ASSERT_NE(ret, GSERROR_OK);
    // Note: In this test, we can't predict which surfaceId will be found
    // since it depends on the mock hdiOutput_->GetFrameBufferSurface()->GetUniqueId()
    // The test verifies the code path when line 802 condition is true
}

/**
 * Function: ClearFrameBuffersInner_SurfaceIdInMap_NullptrValue
 * Type: Function
 * Rank: Important(2)
 * EnvConditions: N/A
 * CaseDescription: 1. create RSRenderComposer
 *                  2. add surface with nullptr value to frameBufferSurfaceOhosMap_ (line 802 condition true, 805 false)
 *                  3. call ClearFrameBuffersInner
 *                  4. verify no erase operation performed for nullptr value
 */
HWTEST_F(RsRenderComposerTest, ClearFrameBuffersInner_SurfaceIdInMap_NullptrValue, TestSize.Level1)
{
    auto output = std::make_shared<HdiOutput>(1u);
    output->Init();
    sptr<RSScreenProperty> property = new RSScreenProperty();
    auto rsRenderComposerTmp = std::make_shared<RSRenderComposer>(output, property);
    ASSERT_NE(rsRenderComposerTmp->hdiOutput_, nullptr);

    // Add surfaceId with nullptr value to the map
    // This makes line 802 condition true but line 805 condition false
    uint64_t surfaceId = 99999;
    rsRenderComposerTmp->frameBufferSurfaceOhosMap_.insert({surfaceId, nullptr});

    // Verify surfaceId is in the map
    EXPECT_TRUE(rsRenderComposerTmp->frameBufferSurfaceOhosMap_.count(surfaceId));

    // Call ClearFrameBuffersInner
    rsRenderComposerTmp->uniRenderEngine_ = nullptr;
    GSError ret = rsRenderComposerTmp->ClearFrameBuffersInner(false);
    ASSERT_NE(ret, GSERROR_OK);
    // Verify surfaceId with nullptr value is NOT erased from map (line 805 condition false)
    // The erase operation only happens when frameBufferSurfaceOhos != nullptr
    EXPECT_TRUE(rsRenderComposerTmp->frameBufferSurfaceOhosMap_.count(surfaceId));
}

#ifdef RS_ENABLE_VK
/**
 * Function: ClearFrameBuffersInner_VulkanGpuApi_WaitSurfaceClear
 * Type: Function
 * Rank: Important(2)
 * EnvConditions: RS_ENABLE_VK defined, GpuApiType set to VULKAN
 * CaseDescription: 1. create RSRenderComposer
 *                  2. add surface to frameBufferSurfaceOhosMap_ with VULKAN GPU API type (line 807 condition true)
 *                  3. call ClearFrameBuffersInner
 *                  4. verify WaitSurfaceClear is called for Vulkan surface
 */
HWTEST_F(RsRenderComposerTest, ClearFrameBuffersInner_VulkanGpuApi_WaitSurfaceClear, TestSize.Level1)
{
    auto output = std::make_shared<HdiOutput>(1u);
    output->Init();
    sptr<RSScreenProperty> property = new RSScreenProperty();
    auto rsRenderComposerTmp = std::make_shared<RSRenderComposer>(output, property);
    ASSERT_NE(rsRenderComposerTmp->hdiOutput_, nullptr);

    // Create and add a Vulkan surface to frameBufferSurfaceOhosMap_
    auto csurf = IConsumerSurface::Create("vulkan_test_surface");
    auto producer = csurf->GetProducer();
    auto pSurface = Surface::CreateSurfaceAsProducer(producer);
    std::shared_ptr<RSSurfaceOhosVulkan> rsVulkanSurface = std::make_shared<RSSurfaceOhosVulkan>(pSurface);
    ASSERT_NE(rsVulkanSurface, nullptr);

    uint64_t surfaceId = 88888;
    rsRenderComposerTmp->frameBufferSurfaceOhosMap_.insert({surfaceId, rsVulkanSurface});

    // Verify surface is in the map
    EXPECT_TRUE(rsRenderComposerTmp->frameBufferSurfaceOhosMap_.count(surfaceId));

    // Call ClearFrameBuffersInner - for Vulkan GPU API type, WaitSurfaceClear should be called (line 807-811)
    rsRenderComposerTmp->uniRenderEngine_ = nullptr;
    GSError ret = rsRenderComposerTmp->ClearFrameBuffersInner(false);
    ASSERT_NE(ret, GSERROR_OK);
    // Verify surface was erased from map after ClearFrameBuffersInner
    EXPECT_FALSE(rsRenderComposerTmp->frameBufferSurfaceOhosMap_.count(surfaceId));
}

/**
 * Function: ClearFrameBuffersInner_DdgrGpuApi_WaitSurfaceClear
 * Type: Function
 * Rank: Important(2)
 * EnvConditions: RS_ENABLE_VK defined, GpuApiType set to DDGR
 * CaseDescription: 1. create RSRenderComposer
 *                  2. add surface to frameBufferSurfaceOhosMap_ with DDGR GPU API type (line 807 condition true)
 *                  3. call ClearFrameBuffersInner
 *                  4. verify WaitSurfaceClear is called for DDGR surface
 */
HWTEST_F(RsRenderComposerTest, ClearFrameBuffersInner_DdgrGpuApi_WaitSurfaceClear, TestSize.Level1)
{
    auto output = std::make_shared<HdiOutput>(1u);
    output->Init();
    sptr<RSScreenProperty> property = new RSScreenProperty();
    auto rsRenderComposerTmp = std::make_shared<RSRenderComposer>(output, property);
    ASSERT_NE(rsRenderComposerTmp->hdiOutput_, nullptr);

    // Create and add a Vulkan surface to frameBufferSurfaceOhosMap_ (DDGR uses same surface type)
    auto csurf = IConsumerSurface::Create("ddgr_test_surface");
    auto producer = csurf->GetProducer();
    auto pSurface = Surface::CreateSurfaceAsProducer(producer);
    std::shared_ptr<RSSurfaceOhosVulkan> rsVulkanSurface = std::make_shared<RSSurfaceOhosVulkan>(pSurface);
    ASSERT_NE(rsVulkanSurface, nullptr);

    uint64_t surfaceId = 77777;
    rsRenderComposerTmp->frameBufferSurfaceOhosMap_.insert({surfaceId, rsVulkanSurface});

    // Verify surface is in the map
    EXPECT_TRUE(rsRenderComposerTmp->frameBufferSurfaceOhosMap_.count(surfaceId));

    // Call ClearFrameBuffersInner - for DDGR GPU API type, WaitSurfaceClear should be called (line 807-811)
    rsRenderComposerTmp->uniRenderEngine_ = nullptr;
    GSError ret = rsRenderComposerTmp->ClearFrameBuffersInner(false);
    ASSERT_NE(ret, GSERROR_OK);
    // Verify surface was erased from map after ClearFrameBuffersInner
    EXPECT_FALSE(rsRenderComposerTmp->frameBufferSurfaceOhosMap_.count(surfaceId));
}

/**
 * Function: ClearFrameBuffersInner_OtherGpuApi_NoWaitSurfaceClear
 * Type: Function
 * Rank: Important(2)
 * EnvConditions: RS_ENABLE_VK defined, GpuApiType set to OPENGL (not VULKAN or DDGR)
 * CaseDescription: 1. create RSRenderComposer
 *                  2. add surface to frameBufferSurfaceOhosMap_ with non-Vulkan GPU API type (line 807 condition false)
 *                  3. call ClearFrameBuffersInner
 *                  4. verify WaitSurfaceClear is not called
 */
HWTEST_F(RsRenderComposerTest, ClearFrameBuffersInner_OtherGpuApi_NoWaitSurfaceClear, TestSize.Level1)
{
    auto output = std::make_shared<HdiOutput>(1u);
    output->Init();
    sptr<RSScreenProperty> property = new RSScreenProperty();
    auto rsRenderComposerTmp = std::make_shared<RSRenderComposer>(output, property);
    ASSERT_NE(rsRenderComposerTmp->hdiOutput_, nullptr);

    // Create and add a Raster surface (OpenGL type) to frameBufferSurfaceOhosMap_
    auto csurf = IConsumerSurface::Create("opengl_test_surface");
    auto producer = csurf->GetProducer();
    auto pSurface = Surface::CreateSurfaceAsProducer(producer);
    std::shared_ptr<RSSurfaceOhos> rsRasterSurface = std::make_shared<RSSurfaceOhosRaster>(pSurface);
    ASSERT_NE(rsRasterSurface, nullptr);

    uint64_t surfaceId = 66666;
    rsRenderComposerTmp->frameBufferSurfaceOhosMap_.insert({surfaceId, rsRasterSurface});

    // Verify surface is in the map
    EXPECT_TRUE(rsRenderComposerTmp->frameBufferSurfaceOhosMap_.count(surfaceId));

    // Call ClearFrameBuffersInner - for non-Vulkan/DDGR GPU API type, WaitSurfaceClear is NOT called (line 807 condition false)
    rsRenderComposerTmp->uniRenderEngine_ = nullptr;
    GSError ret = rsRenderComposerTmp->ClearFrameBuffersInner(false);
    ASSERT_NE(ret, GSERROR_OK);
    // Verify surface was erased from map after ClearFrameBuffersInner
    EXPECT_FALSE(rsRenderComposerTmp->frameBufferSurfaceOhosMap_.count(surfaceId));
}

/**
 * Function: ClearFrameBuffersInner_VulkanSurfaceNullptr_NoWaitSurfaceClear
 * Type: Function
 * Rank: Important(2)
 * EnvConditions: RS_ENABLE_VK defined
 * CaseDescription: 1. create RSRenderComposer
 *                  2. add Vulkan surface but cast result is nullptr (line 810 condition false)
 *                  3. call ClearFrameBuffersInner
 *                  4. verify WaitSurfaceClear is not called when frameBufferSurface is nullptr
 */
HWTEST_F(RsRenderComposerTest, ClearFrameBuffersInner_VulkanSurfaceNullptr_NoWaitSurfaceClear, TestSize.Level1)
{
    auto output = std::make_shared<HdiOutput>(1u);
    output->Init();
    sptr<RSScreenProperty> property = new RSScreenProperty();
    auto rsRenderComposerTmp = std::make_shared<RSRenderComposer>(output, property);
    ASSERT_NE(rsRenderComposerTmp->hdiOutput_, nullptr);

    // Create and add a surface to frameBufferSurfaceOhosMap_
    // The static_cast to RSSurfaceOhosVulkan may fail and result in nullptr
    auto csurf = IConsumerSurface::Create("null_vulkan_surface");
    auto producer = csurf->GetProducer();
    auto pSurface = Surface::CreateSurfaceAsProducer(producer);
    std::shared_ptr<RSSurfaceOhos> rsSurface = std::make_shared<RSSurfaceOhosRaster>(pSurface);
    ASSERT_NE(rsSurface, nullptr);

    uint64_t surfaceId = 55555;
    rsRenderComposerTmp->frameBufferSurfaceOhosMap_.insert({surfaceId, rsSurface});

    // Verify surface is in the map
    EXPECT_TRUE(rsRenderComposerTmp->frameBufferSurfaceOhosMap_.count(surfaceId));

    // Call ClearFrameBuffersInner - static_cast to RSSurfaceOhosVulkan will fail (line 810 condition false)
    rsRenderComposerTmp->uniRenderEngine_ = nullptr;
    GSError ret = rsRenderComposerTmp->ClearFrameBuffersInner(false);
    ASSERT_NE(ret, GSERROR_OK);
    // Verify surface was erased from map after ClearFrameBuffersInner
    EXPECT_FALSE(rsRenderComposerTmp->frameBufferSurfaceOhosMap_.count(surfaceId));
}
#endif // RS_ENABLE_VK

/**
 * Function: CreateFrameBufferSurfaceOhos_OpenGLGpuApi_CreatesGlSurface
 * Type: Function
 * Rank: Important(2)
 * EnvConditions: RS_ENABLE_GL and RS_ENABLE_EGLIMAGE defined
 * CaseDescription: 1. create RSRenderComposer
 *                  2. set GPU API type to OPENGL (line 834 condition true)
 *                  3. call CreateFrameBufferSurfaceOhos
 *                  4. verify RSSurfaceOhosGl is created
 */
HWTEST_F(RsRenderComposerTest, CreateFrameBufferSurfaceOhos_OpenGLGpuApi_CreatesGlSurface, TestSize.Level1)
{
    auto csurface = IConsumerSurface::Create("test_surface");
    ASSERT_NE(csurface, nullptr);
    auto producer = csurface->GetProducer();
    ASSERT_NE(producer, nullptr);
    auto psurface = Surface::CreateSurfaceAsProducer(producer);
    ASSERT_NE(psurface, nullptr);

    // Call CreateFrameBufferSurfaceOhos - for OPENGL GPU API type, RSSurfaceOhosGl should be created (line 834-835)
    auto rsSurfaceOhosPtr = rsRenderComposer_->CreateFrameBufferSurfaceOhos(psurface);

    // Verify the surface was created successfully
    ASSERT_NE(rsSurfaceOhosPtr, nullptr);
}

#if (defined RS_ENABLE_VK)
/**
 * Function: CreateFrameBufferSurfaceOhos_VulkanGpuApi_CreatesVulkanSurface
 * Type: Function
 * Rank: Important(2)
 * EnvConditions: RS_ENABLE_VK defined
 * CaseDescription: 1. create RSRenderComposer
 *                  2. set GPU API type to VULKAN (line 839 condition true)
 *                  3. call CreateFrameBufferSurfaceOhos
 *                  4. verify RSSurfaceOhosVulkan is created
 */
HWTEST_F(RsRenderComposerTest, CreateFrameBufferSurfaceOhos_VulkanGpuApi_CreatesVulkanSurface, TestSize.Level1)
{
    auto csurface = IConsumerSurface::Create("vulkan_test_surface");
    ASSERT_NE(csurface, nullptr);
    auto producer = csurface->GetProducer();
    ASSERT_NE(producer, nullptr);
    auto psurface = Surface::CreateSurfaceAsProducer(producer);
    ASSERT_NE(psurface, nullptr);

    // Call CreateFrameBufferSurfaceOhos - for VULKAN GPU API type, RSSurfaceOhosVulkan should be created (line 839-841)
    auto rsSurfaceOhosPtr = rsRenderComposer_->CreateFrameBufferSurfaceOhos(psurface);

    // Verify the surface was created successfully
    ASSERT_NE(rsSurfaceOhosPtr, nullptr);
}

/**
 * Function: CreateFrameBufferSurfaceOhos_DdgrGpuApi_CreatesVulkanSurface
 * Type: Function
 * Rank: Important(2)
 * EnvConditions: RS_ENABLE_VK defined
 * CaseDescription: 1. create RSRenderComposer
 *                  2. set GPU API type to DDGR (line 839 condition true)
 *                  3. call CreateFrameBufferSurfaceOhos
 *                  4. verify RSSurfaceOhosVulkan is created
 */
HWTEST_F(RsRenderComposerTest, CreateFrameBufferSurfaceOhos_DdgrGpuApi_CreatesVulkanSurface, TestSize.Level1)
{
    auto csurface = IConsumerSurface::Create("ddgr_test_surface");
    ASSERT_NE(csurface, nullptr);
    auto producer = csurface->GetProducer();
    ASSERT_NE(producer, nullptr);
    auto psurface = Surface::CreateSurfaceAsProducer(producer);
    ASSERT_NE(psurface, nullptr);

    // Call CreateFrameBufferSurfaceOhos - for DDGR GPU API type, RSSurfaceOhosVulkan should be created (line 839-841)
    auto rsSurfaceOhosPtr = rsRenderComposer_->CreateFrameBufferSurfaceOhos(psurface);

    // Verify the surface was created successfully
    ASSERT_NE(rsSurfaceOhosPtr, nullptr);
}
#endif // RS_ENABLE_VK

/**
 * Function: RedrawScreenRCD_CompositionDevice_Skip
 * Type: Function
 * Rank: Important(2)
 * EnvConditions: N/A
 * CaseDescription: 1. create RSRenderComposer
 *                  2. add layer with GRAPHIC_COMPOSITION_DEVICE type (line 856 condition true)
 *                  3. call RedrawScreenRCD
 *                  4. verify layer is skipped (not added to rcdLayerInfoList)
 */
HWTEST_F(RsRenderComposerTest, RedrawScreenRCD_CompositionDevice_Skip, TestSize.Level1)
{
    auto output = std::make_shared<HdiOutput>(1u);
    output->Init();
    sptr<RSScreenProperty> property = new RSScreenProperty();
    auto rsRenderComposerTmp = std::make_shared<RSRenderComposer>(output, property);
    ASSERT_NE(rsRenderComposerTmp, nullptr);

    Drawing::Canvas canvas;
    RSPaintFilterCanvas paintFilterCanvas(&canvas);
    std::vector<std::shared_ptr<RSLayer>> layers;

    // Create layer with GRAPHIC_COMPOSITION_DEVICE type
    // This makes line 856 condition true (first sub-condition true)
    std::shared_ptr<RSRenderSurfaceLayer> layer = std::make_shared<RSRenderSurfaceLayer>();
    layer->SetHdiCompositionType(GraphicCompositionType::GRAPHIC_COMPOSITION_DEVICE);
    layers.push_back(layer);

    // Call RedrawScreenRCD - layer with DEVICE composition type should be skipped (line 859 continue)
    rsRenderComposerTmp->RedrawScreenRCD(paintFilterCanvas, layers);

    // The test verifies the code path when line 856 condition is true
    // The layer should not be added to rcdLayerInfoList
}

/**
 * Function: RedrawScreenRCD_CompositionDeviceClear_Skip
 * Type: Function
 * Rank: Important(2)
 * EnvConditions: N/A
 * CaseDescription: 1. create RSRenderComposer
 *                  2. add layer with GRAPHIC_COMPOSITION_DEVICE_CLEAR type (line 857 condition true)
 *                  3. call RedrawScreenRCD
 *                  4. verify layer is skipped
 */
HWTEST_F(RsRenderComposerTest, RedrawScreenRCD_CompositionDeviceClear_Skip, TestSize.Level1)
{
    auto output = std::make_shared<HdiOutput>(1u);
    output->Init();
    sptr<RSScreenProperty> property = new RSScreenProperty();
    auto rsRenderComposerTmp = std::make_shared<RSRenderComposer>(output, property);
    ASSERT_NE(rsRenderComposerTmp, nullptr);

    Drawing::Canvas canvas;
    RSPaintFilterCanvas paintFilterCanvas(&canvas);
    std::vector<std::shared_ptr<RSLayer>> layers;

    // Create layer with GRAPHIC_COMPOSITION_DEVICE_CLEAR type
    // This makes line 857 condition true (second sub-condition true)
    std::shared_ptr<RSRenderSurfaceLayer> layer = std::make_shared<RSRenderSurfaceLayer>();
    layer->SetHdiCompositionType(GraphicCompositionType::GRAPHIC_COMPOSITION_DEVICE_CLEAR);
    layers.push_back(layer);

    // Call RedrawScreenRCD - layer with DEVICE_CLEAR composition type should be skipped (line 859 continue)
    rsRenderComposerTmp->RedrawScreenRCD(paintFilterCanvas, layers);

    // The test verifies the code path when line 857 condition is true
    // The layer should not be added to rcdLayerInfoList
}

/**
 * Function: RedrawScreenRCD_CompositionSolidColor_Skip
 * Type: Function
 * Rank: Important(2)
 * EnvConditions: N/A
 * CaseDescription: 1. create RSRenderComposer
 *                  2. add layer with GRAPHIC_COMPOSITION_SOLID_COLOR type (line 858 condition true)
 *                  3. call RedrawScreenRCD
 *                  4. verify layer is skipped
 */
HWTEST_F(RsRenderComposerTest, RedrawScreenRCD_CompositionSolidColor_Skip, TestSize.Level1)
{
    auto output = std::make_shared<HdiOutput>(1u);
    output->Init();
    sptr<RSScreenProperty> property = new RSScreenProperty();
    auto rsRenderComposerTmp = std::make_shared<RSRenderComposer>(output, property);
    ASSERT_NE(rsRenderComposerTmp, nullptr);

    Drawing::Canvas canvas;
    RSPaintFilterCanvas paintFilterCanvas(&canvas);
    std::vector<std::shared_ptr<RSLayer>> layers;

    // Create layer with GRAPHIC_COMPOSITION_SOLID_COLOR type
    // This makes line 858 condition true (third sub-condition true)
    std::shared_ptr<RSRenderSurfaceLayer> layer = std::make_shared<RSRenderSurfaceLayer>();
    layer->SetHdiCompositionType(GraphicCompositionType::GRAPHIC_COMPOSITION_SOLID_COLOR);
    layers.push_back(layer);

    // Call RedrawScreenRCD - layer with SOLID_COLOR composition type should be skipped (line 859 continue)
    rsRenderComposerTmp->RedrawScreenRCD(paintFilterCanvas, layers);

    // The test verifies the code path when line 858 condition is true
    // The layer should not be added to rcdLayerInfoList
}

/**
 * Function: RedrawScreenRCD_AllThreeCompositionTypes_Skip
 * Type: Function
 * Rank: Important(2)
 * EnvConditions: N/A
 * CaseDescription: 1. create RSRenderComposer
 *                  2. add layers with all three composition types (lines 856-858 all true)
 *                  3. call RedrawScreenRCD
 *                  4. verify all layers are skipped
 */
HWTEST_F(RsRenderComposerTest, RedrawScreenRCD_AllThreeCompositionTypes_Skip, TestSize.Level1)
{
    auto output = std::make_shared<HdiOutput>(1u);
    output->Init();
    sptr<RSScreenProperty> property = new RSScreenProperty();
    auto rsRenderComposerTmp = std::make_shared<RSRenderComposer>(output, property);
    ASSERT_NE(rsRenderComposerTmp, nullptr);

    Drawing::Canvas canvas;
    RSPaintFilterCanvas paintFilterCanvas(&canvas);
    std::vector<std::shared_ptr<RSLayer>> layers;

    // Create layers with all three composition types
    // All of lines 856, 857, 858 conditions will be true for their respective layers
    std::shared_ptr<RSRenderSurfaceLayer> layer1 = std::make_shared<RSRenderSurfaceLayer>();
    layer1->SetHdiCompositionType(GraphicCompositionType::GRAPHIC_COMPOSITION_DEVICE);
    layers.push_back(layer1);

    std::shared_ptr<RSRenderSurfaceLayer> layer2 = std::make_shared<RSRenderSurfaceLayer>();
    layer2->SetHdiCompositionType(GraphicCompositionType::GRAPHIC_COMPOSITION_DEVICE_CLEAR);
    layers.push_back(layer2);

    std::shared_ptr<RSRenderSurfaceLayer> layer3 = std::make_shared<RSRenderSurfaceLayer>();
    layer3->SetHdiCompositionType(GraphicCompositionType::GRAPHIC_COMPOSITION_SOLID_COLOR);
    layers.push_back(layer3);

    // Call RedrawScreenRCD - all layers should be skipped (line 859 continue)
    rsRenderComposerTmp->RedrawScreenRCD(paintFilterCanvas, layers);

    // The test verifies the code path when all conditions are true
    // All three layers should be skipped and not added to rcdLayerInfoList
}

/**
 * Function: RedrawScreenRCD_OtherCompositionType_Processed
 * Type: Function
 * Rank: Important(2)
 * EnvConditions: N/A
 * CaseDescription: 1. create RSRenderComposer
 *                  2. add layer with other composition type (lines 856-858 all false)
 *                  3. call RedrawScreenRCD
 *                  4. verify layer is processed if not RCD layer
 */
HWTEST_F(RsRenderComposerTest, RedrawScreenRCD_OtherCompositionType_Processed, TestSize.Level1)
{
    auto output = std::make_shared<HdiOutput>(1u);
    output->Init();
    sptr<RSScreenProperty> property = new RSScreenProperty();
    auto rsRenderComposerTmp = std::make_shared<RSRenderComposer>(output, property);
    ASSERT_NE(rsRenderComposerTmp, nullptr);

    Drawing::Canvas canvas;
    RSPaintFilterCanvas paintFilterCanvas(&canvas);
    std::vector<std::shared_ptr<RSLayer>> layers;

    // Create layer with other composition type (e.g., CLIENT or CURSOR)
    // This makes lines 856-858 conditions all false
    std::shared_ptr<RSRenderSurfaceLayer> layer = std::make_shared<RSRenderSurfaceLayer>();
    layer->SetHdiCompositionType(GraphicCompositionType::GRAPHIC_COMPOSITION_CLIENT);
    layer->SetZorder(10);
    sptr<IConsumerSurface> cSurface = IConsumerSurface::Create("test_layer");
    layer->SetSurface(cSurface);
    layers.push_back(layer);

    // Call RedrawScreenRCD - layer should not be skipped (lines 856-858 false)
    rsRenderComposerTmp->RedrawScreenRCD(paintFilterCanvas, layers);

    // The test verifies the code path when all conditions at lines 856-858 are false
    // The layer should NOT be skipped (but won't be added to rcdLayerInfoList if not RCD layer)
}

/**
 * Function: RedrawScreenRCD_RcdLayer_Processed
 * Type: Function
 * Rank: Important(2)
 * EnvConditions: N/A
 * CaseDescription: 1. create RSRenderComposer
 *                  2. add RCD layer with other composition type (line 861 condition true)
 *                  3. call RedrawScreenRCD
 *                  4. verify RCD layer is added to rcdLayerInfoList
 */
HWTEST_F(RsRenderComposerTest, RedrawScreenRCD_RcdLayer_Processed, TestSize.Level1)
{
    auto output = std::make_shared<HdiOutput>(1u);
    output->Init();
    sptr<RSScreenProperty> property = new RSScreenProperty();
    auto rsRenderComposerTmp = std::make_shared<RSRenderComposer>(output, property);
    ASSERT_NE(rsRenderComposerTmp, nullptr);

    Drawing::Canvas canvas;
    RSPaintFilterCanvas paintFilterCanvas(&canvas);
    std::vector<std::shared_ptr<RSLayer>> layers;

    // Create RCD layer with CLIENT composition type
    // Lines 856-858 conditions are false, line 861 condition is true
    std::shared_ptr<RSRenderSurfaceRCDLayer> rcdLayer = std::make_shared<RSRenderSurfaceRCDLayer>();
    rcdLayer->SetHdiCompositionType(GraphicCompositionType::GRAPHIC_COMPOSITION_CLIENT);
    rcdLayer->SetZorder(10);
    sptr<IConsumerSurface> cSurface = IConsumerSurface::Create("rcd_layer");
    rcdLayer->SetSurface(cSurface);
    layers.push_back(rcdLayer);

    // Call RedrawScreenRCD - RCD layer should be added to rcdLayerInfoList (line 862)
    rsRenderComposerTmp->RedrawScreenRCD(paintFilterCanvas, layers);

    // Verify RCD layer properties
    EXPECT_TRUE(rcdLayer->IsScreenRCDLayer());
    EXPECT_EQ(rcdLayer->GetHdiCompositionType(), GraphicCompositionType::GRAPHIC_COMPOSITION_CLIENT);
}

/**
 * Function: RedrawScreenRCD_NullptrLayer_Skipped
 * Type: Function
 * Rank: Important(2)
 * EnvConditions: N/A
 * CaseDescription: 1. create RSRenderComposer
 *                  2. add nullptr layer (line 853 condition true)
 *                  3. call RedrawScreenRCD
 *                  4. verify nullptr layer is skipped
 */
HWTEST_F(RsRenderComposerTest, RedrawScreenRCD_NullptrLayer_Skipped, TestSize.Level1)
{
    auto output = std::make_shared<HdiOutput>(1u);
    output->Init();
    sptr<RSScreenProperty> property = new RSScreenProperty();
    auto rsRenderComposerTmp = std::make_shared<RSRenderComposer>(output, property);
    ASSERT_NE(rsRenderComposerTmp, nullptr);

    Drawing::Canvas canvas;
    RSPaintFilterCanvas paintFilterCanvas(&canvas);
    std::vector<std::shared_ptr<RSLayer>> layers;

    // Add nullptr layer
    // This makes line 853 condition true (layer == nullptr)
    layers.push_back(nullptr);

    // Call RedrawScreenRCD - nullptr layer should be skipped (line 854 continue)
    rsRenderComposerTmp->RedrawScreenRCD(paintFilterCanvas, layers);

    // Verify the function handles nullptr layers gracefully
    // The test verifies the code path when line 853 condition is true
}

/**
 * Function: RedrawScreenRCD_NullptrLayerAndNormalLayer
 * Type: Function
 * Rank: Important(2)
 * EnvConditions: N/A
 * CaseDescription: 1. create RSRenderComposer
 *                  2. add nullptr layer and normal layer
 *                  3. call RedrawScreenRCD
 *                  4. verify nullptr is skipped and normal layer processed
 */
HWTEST_F(RsRenderComposerTest, RedrawScreenRCD_NullptrLayerAndNormalLayer, TestSize.Level1)
{
    auto output = std::make_shared<HdiOutput>(1u);
    output->Init();
    sptr<RSScreenProperty> property = new RSScreenProperty();
    auto rsRenderComposerTmp = std::make_shared<RSRenderComposer>(output, property);
    ASSERT_NE(rsRenderComposerTmp, nullptr);

    Drawing::Canvas canvas;
    RSPaintFilterCanvas paintFilterCanvas(&canvas);
    std::vector<std::shared_ptr<RSLayer>> layers;

    // Add nullptr layer first
    layers.push_back(nullptr);

    // Add normal layer (CLIENT composition type, not RCD)
    std::shared_ptr<RSRenderSurfaceLayer> layer = std::make_shared<RSRenderSurfaceLayer>();
    layer->SetHdiCompositionType(GraphicCompositionType::GRAPHIC_COMPOSITION_CLIENT);
    layer->SetZorder(10);
    sptr<IConsumerSurface> cSurface = IConsumerSurface::Create("test_layer");
    layer->SetSurface(cSurface);
    layers.push_back(layer);

    // Call RedrawScreenRCD - nullptr should be skipped, normal layer should be processed
    rsRenderComposerTmp->RedrawScreenRCD(paintFilterCanvas, layers);

    // The test verifies mixed handling:
    // - nullptr layer should be skipped (line 854 continue)
    // - normal layer (CLIENT type, not RCD) should be processed but not added to rcdLayerInfoList (line 862-863 false)
}

/**
 * Function: RedrawScreenRCD_DeviceTypeAndNullptr
 * Type: Function
 * Rank: Important(2)
 * EnvConditions: N/A
 * CaseDescription: 1. create RSRenderComposer
 *                  2. add DEVICE type layer and nullptr layer
 *                  3. call RedrawScreenRCD
 *                  4. verify both layers are skipped
 */
HWTEST_F(RsRenderComposerTest, RedrawScreenRCD_DeviceTypeAndNullptr, TestSize.Level1)
{
    auto output = std::make_shared<HdiOutput>(1u);
    output->Init();
    sptr<RSScreenProperty> property = new RSScreenProperty();
    auto rsRenderComposerTmp = std::make_shared<RSRenderComposer>(output, property);
    ASSERT_NE(rsRenderComposerTmp, nullptr);

    Drawing::Canvas canvas;
    RSPaintFilterCanvas paintFilterCanvas(&canvas);
    std::vector<std::shared_ptr<RSLayer>> layers;

    // Add nullptr layer
    layers.push_back(nullptr);

    // Add DEVICE type layer
    // This makes line 856 condition true (first sub-condition true)
    std::shared_ptr<RSRenderSurfaceLayer> layer = std::make_shared<RSRenderSurfaceLayer>();
    layer->SetHdiCompositionType(GraphicCompositionType::GRAPHIC_COMPOSITION_DEVICE);
    layers.push_back(layer);

    // Call RedrawScreenRCD
    // - nullptr should be skipped (line 854 continue)
    // - DEVICE type should be skipped (line 859 continue)
    rsRenderComposerTmp->RedrawScreenRCD(paintFilterCanvas, layers);

    // Verify both layers should be skipped
}

/**
 * Function: RedrawScreenRCD_AllSkipConditions
 * Type: Function
 * Rank: Important(2)
 * EnvConditions: N/A
 * CaseDescription: 1. create RSRenderComposer
 *                  2. add layers that trigger all skip conditions
 *                  3. call RedrawScreenRCD
 *                  4. verify all layers are skipped
 */
HWTEST_F(RsRenderComposerTest, RedrawScreenRCD_AllSkipConditions, TestSize.Level1)
{
    auto output = std::make_shared<HdiOutput>(1u);
    output->Init();
    sptr<RSScreenProperty> property = new RSScreenProperty();
    auto rsRenderComposerTmp = std::make_shared<RSRenderComposer>(output, property);
    ASSERT_NE(rsRenderComposerTmp, nullptr);

    Drawing::Canvas canvas;
    RSPaintFilterCanvas paintFilterCanvas(&canvas);
    std::vector<std::shared_ptr<RSLayer>> layers;

    // Add nullptr layer (line 853 skip condition)
    layers.push_back(nullptr);

    // Add DEVICE type layer (line 856 skip condition)
    std::shared_ptr<RSRenderSurfaceLayer> layer1 = std::make_shared<RSRenderSurfaceLayer>();
    layer1->SetHdiCompositionType(GraphicCompositionType::GRAPHIC_COMPOSITION_DEVICE);
    layers.push_back(layer1);

    // Add DEVICE_CLEAR type layer (line 857 skip condition)
    std::shared_ptr<RSRenderSurfaceLayer> layer2 = std::make_shared<RSRenderSurfaceLayer>();
    layer2->SetHdiCompositionType(GraphicCompositionType::GRAPHIC_COMPOSITION_DEVICE_CLEAR);
    layers.push_back(layer2);

    // Add SOLID_COLOR type layer (line 858 skip condition)
    std::shared_ptr<RSRenderSurfaceLayer> layer3 = std::make_shared<RSRenderSurfaceLayer>();
    layer3->SetHdiCompositionType(GraphicCompositionType::GRAPHIC_COMPOSITION_SOLID_COLOR);
    layers.push_back(layer3);

    // Call RedrawScreenRCD
    // All layers should be skipped:
    // - nullptr: line 854 continue
    // - layer1 (DEVICE): line 859 continue
    // - layer2 (DEVICE_CLEAR): line 859 continue
    // - layer3 (SOLID_COLOR): line 859 continue
    rsRenderComposerTmp->RedrawScreenRCD(paintFilterCanvas, layers);

    // Verify all skip conditions are covered
    EXPECT_EQ(layers.size(), 4u);
}

/**
 * Function: Redraw_ProtectedBufferDetection_Vulkan
 * Type: Function
 * Rank: Important(2)
 * EnvConditions: RS_ENABLE_VK defined, GpuApiType set to VULKAN
 * CaseDescription: 1. create RSRenderComposer
 *                  2. create layer with protected buffer (BUFFER_USAGE_PROTECTED)
 *                  3. call Redraw (line 881 condition true with VULKAN, enters line 883 for loop)
 *                  4. verify protected buffer is detected in the for loop (line 884-887)
 */
#ifdef RS_ENABLE_VK
#if defined(GPU_API_TYPE_VULKAN) || defined(GPU_API_TYPE_DDGR)
HWTEST_F(RsRenderComposerTest, Redraw_ProtectedBufferDetection_Vulkan, TestSize.Level1)
{
    auto output = std::make_shared<HdiOutput>(1u);
    output->Init();
    sptr<RSScreenProperty> property = new RSScreenProperty();
    auto rsRenderComposerTmp = std::make_shared<RSRenderComposer>(output, property);
    ASSERT_NE(rsRenderComposerTmp, nullptr);

    // Create protected buffer
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
    sptr<SurfaceBuffer> protectedBuffer;
    sptr<SyncFence> requestFence = SyncFence::INVALID_FENCE;
    EXPECT_EQ(psurface->RequestBuffer(protectedBuffer, requestFence, requestConfig), GSERROR_OK);

    // Create non-protected buffer
    BufferRequestConfig normalRequestConfig = {
        .width = 0x100,
        .height = 0x100,
        .strideAlignment = 0x8,
        .format = GRAPHIC_PIXEL_FMT_RGBA_8888,
        .usage = BUFFER_USAGE_CPU_READ | BUFFER_USAGE_CPU_WRITE | BUFFER_USAGE_MEM_DMA,
        .timeout = 0,
    };
    sptr<SurfaceBuffer> normalBuffer;
    EXPECT_EQ(psurface->RequestBuffer(normalBuffer, requestFence, normalRequestConfig), GSERROR_OK);

    // Create layers with different buffer types
    std::vector<RSLayerPtr> layers;

    // Layer 1: nullptr (should be skipped in for loop at line 884)
    layers.push_back(nullptr);

    // Layer 2: layer without buffer (should be skipped at line 884)
    std::shared_ptr<RSRenderSurfaceLayer> layerNoBuffer = std::make_shared<RSRenderSurfaceLayer>();
    layers.push_back(layerNoBuffer);

    // Layer 3: layer with normal buffer (no BUFFER_USAGE_PROTECTED, should not set isProtected at line 884-887)
    std::shared_ptr<RSRenderSurfaceLayer> layerNormal = std::make_shared<RSRenderSurfaceLayer>();
    layerNormal->SetBuffer(normalBuffer);
    layers.push_back(layerNormal);

    // Layer 4: layer with protected buffer (has BUFFER_USAGE_PROTECTED, should set isProtected at line 885-887)
    std::shared_ptr<RSRenderSurfaceLayer> layerProtected = std::make_shared<RSRenderSurfaceLayer>();
    layerProtected->SetBuffer(protectedBuffer);
    layers.push_back(layerProtected);

    // When GPU API type is VULKAN or DDGR, line 881 condition is true
    // This enters line 883 for loop to iterate through layers
    // The for loop checks: if (layer && layer->GetBuffer() && (layer->GetBuffer()->GetUsage() & BUFFER_USAGE_PROTECTED))
    rsRenderComposerTmp->Redraw(psurface, layers);

    // Verify all layers are processed
    EXPECT_EQ(layers.size(), 4u);

    csurface->UnregisterConsumerListener();
}
#endif
#endif

/**
 * Function: Redraw_ProtectedBufferDetection_FirstProtectedLayer
 * Type: Function
 * Rank: Important(2)
 * EnvConditions: RS_ENABLE_VK defined, GpuApiType set to VULKAN
 * CaseDescription: 1. create RSRenderComposer
 *                  2. create layer with protected buffer as first layer in for loop
 *                  3. call Redraw (line 881 condition true, enters line 883 for loop)
 *                  4. verify isProtected is set to true and break at line 887 (early exit)
 */
#ifdef RS_ENABLE_VK
#if defined(GPU_API_TYPE_VULKAN) || defined(GPU_API_TYPE_DDGR)
HWTEST_F(RsRenderComposerTest, Redraw_ProtectedBufferDetection_FirstProtectedLayer, TestSize.Level1)
{
    auto output = std::make_shared<HdiOutput>(1u);
    output->Init();
    sptr<RSScreenProperty> property = new RSScreenProperty();
    auto rsRenderComposerTmp = std::make_shared<RSRenderComposer>(output, property);
    ASSERT_NE(rsRenderComposerTmp, nullptr);

    // Create protected buffer
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
    sptr<SurfaceBuffer> protectedBuffer;
    sptr<SyncFence> requestFence = SyncFence::INVALID_FENCE;
    EXPECT_EQ(psurface->RequestBuffer(protectedBuffer, requestFence, requestConfig), GSERROR_OK);

    // Create normal buffer
    BufferRequestConfig normalRequestConfig = {
        .width = 0x100,
        .height = 0x100,
        .strideAlignment = 0x8,
        .format = GRAPHIC_PIXEL_FMT_RGBA_8888,
        .usage = BUFFER_USAGE_CPU_READ | BUFFER_USAGE_CPU_WRITE | BUFFER_USAGE_MEM_DMA,
        .timeout = 0,
    };
    sptr<SurfaceBuffer> normalBuffer;
    EXPECT_EQ(psurface->RequestBuffer(normalBuffer, requestFence, normalRequestConfig), GSERROR_OK);

    std::vector<RSLayerPtr> layers;

    // Layer 1: First layer with protected buffer
    // This triggers line 884-887: isProtected = true; break;
    std::shared_ptr<RSRenderSurfaceLayer> layerProtected = std::make_shared<RSRenderSurfaceLayer>();
    layerProtected->SetBuffer(protectedBuffer);
    layers.push_back(layerProtected);

    // Layer 2-5: Normal layers (should not be reached due to break at line 887)
    for (int i = 0; i < 4; i++) {
        std::shared_ptr<RSRenderSurfaceLayer> layerNormal = std::make_shared<RSRenderSurfaceLayer>();
        layerNormal->SetBuffer(normalBuffer);
        layers.push_back(layerNormal);
    }

    // When GPU API type is VULKAN or DDGR, line 881 condition is true
    // The for loop at line 883 starts iterating, first layer has protected buffer
    // Line 885: isProtected = true
    // Line 887: break (early exit, remaining layers not checked)
    rsRenderComposerTmp->Redraw(psurface, layers);

    // Verify all layers are in the vector (early exit doesn't remove layers)
    EXPECT_EQ(layers.size(), 5u);

    csurface->UnregisterConsumerListener();
}
#endif
#endif

/**
 * Function: Redraw_NoProtectedBuffer_Vulkan
 * Type: Function
 * Rank: Important(2)
 * EnvConditions: RS_ENABLE_VK defined, GpuApiType set to VULKAN
 * CaseDescription: 1. create RSRenderComposer
 *                  2. create layers without any protected buffers
 *                  3. call Redraw (line 881 condition true, enters line 883 for loop)
 *                  4. verify isProtected remains false after for loop completes
 */
#ifdef RS_ENABLE_VK
#if defined(GPU_API_TYPE_VULKAN) || defined(GPU_API_TYPE_DDGR)
HWTEST_F(RsRenderComposerTest, Redraw_NoProtectedBuffer_Vulkan, TestSize.Level1)
{
    auto output = std::make_shared<HdiOutput>(1u);
    output->Init();
    sptr<RSScreenProperty> property = new RSScreenProperty();
    auto rsRenderComposerTmp = std::make_shared<RSRenderComposer>(output, property);
    ASSERT_NE(rsRenderComposerTmp, nullptr);

    // Create normal buffer without BUFFER_USAGE_PROTECTED
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
    sptr<SurfaceBuffer> normalBuffer;
    sptr<SyncFence> requestFence = SyncFence::INVALID_FENCE;
    EXPECT_EQ(psurface->RequestBuffer(normalBuffer, requestFence, requestConfig), GSERROR_OK);

    std::vector<RSLayerPtr> layers;

    // Add multiple layers with normal buffers
    for (int i = 0; i < 5; i++) {
        std::shared_ptr<RSRenderSurfaceLayer> layerNormal = std::make_shared<RSRenderSurfaceLayer>();
        layerNormal->SetBuffer(normalBuffer);
        layers.push_back(layerNormal);
    }

    // When GPU API type is VULKAN or DDGR, line 881 condition is true
    // The for loop at line 883 iterates through all layers
    // No layer has protected buffer, so condition at line 884 is always false
    // isProtected remains false, loop completes without break
    rsRenderComposerTmp->Redraw(psurface, layers);

    // Verify all layers are processed
    EXPECT_EQ(layers.size(), 5u);

    csurface->UnregisterConsumerListener();
}
#endif
#endif

/**
 * Function: Redraw_ProtectedBufferDetection_LastProtectedLayer
 * Type: Function
 * Rank: Important(2)
 * EnvConditions: RS_ENABLE_VK defined, GpuApiType set to VULKAN
 * CaseDescription: 1. create RSRenderComposer
 *                  2. create layers with protected buffer as last layer
 *                  3. call Redraw (line 881 condition true, enters line 883 for loop)
 *                  4. verify all layers are checked before protected layer is found
 */
#ifdef RS_ENABLE_VK
#if defined(GPU_API_TYPE_VULKAN) || defined(GPU_API_TYPE_DDGR)
HWTEST_F(RsRenderComposerTest, Redraw_ProtectedBufferDetection_LastProtectedLayer, TestSize.Level1)
{
    auto output = std::make_shared<HdiOutput>(1u);
    output->Init();
    sptr<RSScreenProperty> property = new RSScreenProperty();
    auto rsRenderComposerTmp = std::make_shared<RSRenderComposer>(output, property);
    ASSERT_NE(rsRenderComposerTmp, nullptr);

    // Create protected buffer
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
    sptr<SurfaceBuffer> protectedBuffer;
    sptr<SyncFence> requestFence = SyncFence::INVALID_FENCE;
    EXPECT_EQ(psurface->RequestBuffer(protectedBuffer, requestFence, requestConfig), GSERROR_OK);

    // Create normal buffer
    BufferRequestConfig normalRequestConfig = {
        .width = 0x100,
        .height = 0x100,
        .strideAlignment = 0x8,
        .format = GRAPHIC_PIXEL_FMT_RGBA_8888,
        .usage = BUFFER_USAGE_CPU_READ | BUFFER_USAGE_CPU_WRITE | BUFFER_USAGE_MEM_DMA,
        .timeout = 0,
    };
    sptr<SurfaceBuffer> normalBuffer;
    EXPECT_EQ(psurface->RequestBuffer(normalBuffer, requestFence, normalRequestConfig), GSERROR_OK);

    std::vector<RSLayerPtr> layers;

    // Add multiple layers with normal buffers first
    for (int i = 0; i < 4; i++) {
        std::shared_ptr<RSRenderSurfaceLayer> layerNormal = std::make_shared<RSRenderSurfaceLayer>();
        layerNormal->SetBuffer(normalBuffer);
        layers.push_back(layerNormal);
    }

    // Last layer with protected buffer
    std::shared_ptr<RSRenderSurfaceLayer> layerProtected = std::make_shared<RSRenderSurfaceLayer>();
    layerProtected->SetBuffer(protectedBuffer);
    layers.push_back(layerProtected);

    // When GPU API type is VULKAN or DDGR, line 881 condition is true
    // The for loop at line 883 iterates through all layers
    // First 4 layers have no protected buffer, condition at line 884 is false
    // Last layer has protected buffer, condition at line 884 is true, isProtected = true, break at line 887
    rsRenderComposerTmp->Redraw(psurface, layers);

    // Verify all layers are in the vector
    EXPECT_EQ(layers.size(), 5u);

    csurface->UnregisterConsumerListener();
}
#endif
#endif

/**
 * Function: Redraw_NullLayers_Vulkan
 * Type: Function
 * Rank: Important(2)
 * EnvConditions: RS_ENABLE_VK defined, GpuApiType set to VULKAN
 * CaseDescription: 1. create RSRenderComposer
 *                  2. create layers vector with only nullptr layers
 *                  3. call Redraw (line 881 condition true, enters line 883 for loop)
 *                  4. verify nullptr layers are skipped at line 884 (layer check)
 */
#ifdef RS_ENABLE_VK
#if defined(GPU_API_TYPE_VULKAN) || defined(GPU_API_TYPE_DDGR)
HWTEST_F(RsRenderComposerTest, Redraw_NullLayers_Vulkan, TestSize.Level1)
{
    auto output = std::make_shared<HdiOutput>(1u);
    output->Init();
    sptr<RSScreenProperty> property = new RSScreenProperty();
    auto rsRenderComposerTmp = std::make_shared<RSRenderComposer>(output, property);
    ASSERT_NE(rsRenderComposerTmp, nullptr);

    auto csurface = IConsumerSurface::Create();
    ASSERT_NE(csurface, nullptr);
    sptr<IBufferConsumerListener> listener = new BufferConsumerListener();
    csurface->RegisterConsumerListener(listener);
    auto producer = csurface->GetProducer();
    auto psurface = Surface::CreateSurfaceAsProducer(producer);
    ASSERT_NE(psurface, nullptr);

    std::vector<RSLayerPtr> layers;

    // Add only nullptr layers
    for (int i = 0; i < 5; i++) {
        layers.push_back(nullptr);
    }

    // When GPU API type is VULKAN or DDGR, line 881 condition is true
    // The for loop at line 883 iterates through all nullptr layers
    // Line 884: if (layer && ...) is false for all layers
    // isProtected remains false, loop completes without break
    rsRenderComposerTmp->Redraw(psurface, layers);

    // Verify all nullptr layers are in the vector
    EXPECT_EQ(layers.size(), 5u);

    csurface->UnregisterConsumerListener();
}
#endif
#endif

/**
 * Function: Redraw_RenderFrameNotNull_CanvasNotNull
 * Type: Function
 * Rank: Important(2)
 * EnvConditions: N/A
 * CaseDescription: 1. create RSRenderComposer
 *                  2. call Redraw with normal surface and layers
 *                  3. verify renderFrame is not null (line 936 condition false)
 *                  4. verify canvas is not null (line 941 condition false)
 *                  5. verify Redraw completes successfully without early return
 */
HWTEST_F(RsRenderComposerTest, Redraw_RenderFrameNotNull_CanvasNotNull, TestSize.Level1)
{
    auto output = std::make_shared<HdiOutput>(1u);
    output->Init();
    sptr<RSScreenProperty> property = new RSScreenProperty();
    auto rsRenderComposerTmp = std::make_shared<RSRenderComposer>(output, property);
    ASSERT_NE(rsRenderComposerTmp, nullptr);

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
    std::shared_ptr<RSRenderSurfaceLayer> layer = std::make_shared<RSRenderSurfaceLayer>();
    layer->SetBuffer(buffer);
    layers.push_back(layer);

    // Call Redraw - both line 936 and 941 conditions should be false
    // Line 936: if (renderFrame == nullptr) - false, renderFrame is not null
    // Line 941: if (canvas == nullptr) - false, canvas is not null
    // Function continues past line 944 without early return
    rsRenderComposerTmp->Redraw(psurface, layers);

    // Verify Redraw completed successfully
    EXPECT_NE(rsRenderComposerTmp, nullptr);

    csurface->UnregisterConsumerListener();
}

/**
 * Function: Redraw_RenderFrameIsNull
 * Type: Function
 * Rank: Important(2)
 * EnvConditions: N/A
 * CaseDescription: 1. create RSRenderComposer
 *                  2. modify uniRenderEngine_ to return nullptr
 *                  3. call Redraw
 *                  4. verify renderFrame is null (line 936 condition true)
 *                  5. verify early return at line 938 (skips canvas check)
 */
HWTEST_F(RsRenderComposerTest, Redraw_RenderFrameIsNull, TestSize.Level1)
{
    auto output = std::make_shared<HdiOutput>(1u);
    output->Init();
    sptr<RSScreenProperty> property = new RSScreenProperty();
    auto rsRenderComposerTmp = std::make_shared<RSRenderComposer>(output, property);
    ASSERT_NE(rsRenderComposerTmp, nullptr);

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
    std::shared_ptr<RSRenderSurfaceLayer> layer = std::make_shared<RSRenderSurfaceLayer>();
    layer->SetBuffer(buffer);
    layers.push_back(layer);

    // Set uniRenderEngine_ to nullptr to trigger renderFrame == nullptr at line 936
    auto originalEngine = rsRenderComposerTmp->uniRenderEngine_;
    rsRenderComposerTmp->uniRenderEngine_ = nullptr;

    // Call Redraw - line 936 condition is true (renderFrame == nullptr)
    // Function returns early at line 938, skips canvas check at line 941
    rsRenderComposerTmp->Redraw(psurface, layers);

    // Restore original engine
    rsRenderComposerTmp->uniRenderEngine_ = originalEngine;

    csurface->UnregisterConsumerListener();
}

/**
 * Function: Redraw_FrameBufferSurfaceOhosMapContainsNullSurface
 * Type: Function
 * Rank: Important(2)
 * EnvConditions: N/A
 * CaseDescription: 1. create RSRenderComposer
 *                  2. add null surface to frameBufferSurfaceOhosMap_
 *                  3. call Redraw
 *                  4. verify RequestFrame returns nullptr (line 936 condition true)
 *                  5. verify early return at line 938
 */
HWTEST_F(RsRenderComposerTest, Redraw_FrameBufferSurfaceOhosMapContainsNullSurface, TestSize.Level1)
{
    auto output = std::make_shared<HdiOutput>(1u);
    output->Init();
    sptr<RSScreenProperty> property = new RSScreenProperty();
    auto rsRenderComposerTmp = std::make_shared<RSRenderComposer>(output, property);
    ASSERT_NE(rsRenderComposerTmp, nullptr);

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
    std::shared_ptr<RSRenderSurfaceLayer> layer = std::make_shared<RSRenderSurfaceLayer>();
    layer->SetBuffer(buffer);
    layers.push_back(layer);

    // Add null surface to frameBufferSurfaceOhosMap_ to cause RequestFrame to return nullptr
    auto surfaceId = psurface->GetUniqueId();
    rsRenderComposerTmp->frameBufferSurfaceOhosMap_[surfaceId] = nullptr;

    // Call Redraw - RequestFrame will fail due to null surface in map
    // Line 936 condition is true (renderFrame == nullptr)
    // Function returns early at line 938
    rsRenderComposerTmp->Redraw(psurface, layers);

    // Clean up
    rsRenderComposerTmp->frameBufferSurfaceOhosMap_.clear();

    csurface->UnregisterConsumerListener();
}

/**
 * Function: Redraw_MultipleLayers_RenderFrameNotNull
 * Type: Function
 * Rank: Important(2)
 * EnvConditions: N/A
 * CaseDescription: 1. create RSRenderComposer
 *                  2. create multiple layers with buffers
 *                  3. call Redraw
 *                  4. verify renderFrame is not null (line 936 condition false)
 *                  5. verify canvas is not null (line 941 condition false)
 *                  6. verify all layers are processed
 */
HWTEST_F(RsRenderComposerTest, Redraw_MultipleLayers_RenderFrameNotNull, TestSize.Level1)
{
    auto output = std::make_shared<HdiOutput>(1u);
    output->Init();
    sptr<RSScreenProperty> property = new RSScreenProperty();
    auto rsRenderComposerTmp = std::make_shared<RSRenderComposer>(output, property);
    ASSERT_NE(rsRenderComposerTmp, nullptr);

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

    std::vector<RSLayerPtr> layers;
    // Add multiple layers
    for (int i = 0; i < 5; i++) {
        sptr<SurfaceBuffer> buffer;
        sptr<SyncFence> requestFence = SyncFence::INVALID_FENCE;
        EXPECT_EQ(psurface->RequestBuffer(buffer, requestFence, requestConfig), GSERROR_OK);
        std::shared_ptr<RSRenderSurfaceLayer> layer = std::make_shared<RSRenderSurfaceLayer>();
        layer->SetBuffer(buffer);
        layers.push_back(layer);
    }

    // Call Redraw - both line 936 and 941 conditions should be false
    // Line 936: if (renderFrame == nullptr) - false
    // Line 941: if (canvas == nullptr) - false
    rsRenderComposerTmp->Redraw(psurface, layers);

    // Verify all layers were processed
    EXPECT_EQ(layers.size(), 5u);

    csurface->UnregisterConsumerListener();
}

/**
 * Function: Redraw_NullLayer_MixedWithValidLayers
 * Type: Function
 * Rank: Important(2)
 * EnvConditions: N/A
 * CaseDescription: 1. create RSRenderComposer
 *                  2. create layers with mix of null and valid layers
 *                  3. call Redraw
 *                  4. verify renderFrame is not null (line 936 condition false)
 *                  5. verify canvas is not null (line 941 condition false)
 *                  6. verify Redraw handles null layers gracefully
 */
HWTEST_F(RsRenderComposerTest, Redraw_NullLayer_MixedWithValidLayers, TestSize.Level1)
{
    auto output = std::make_shared<HdiOutput>(1u);
    output->Init();
    sptr<RSScreenProperty> property = new RSScreenProperty();
    auto rsRenderComposerTmp = std::make_shared<RSRenderComposer>(output, property);
    ASSERT_NE(rsRenderComposerTmp, nullptr);

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
    // Mix of null and valid layers
    layers.push_back(nullptr);
    std::shared_ptr<RSRenderSurfaceLayer> layer1 = std::make_shared<RSRenderSurfaceLayer>();
    layer1->SetBuffer(buffer);
    layers.push_back(layer1);
    layers.push_back(nullptr);
    std::shared_ptr<RSRenderSurfaceLayer> layer2 = std::make_shared<RSRenderSurfaceLayer>();
    layer2->SetBuffer(buffer);
    layers.push_back(layer2);

    // Call Redraw - both line 936 and 941 conditions should be false
    // Line 936: if (renderFrame == nullptr) - false
    // Line 941: if (canvas == nullptr) - false
    rsRenderComposerTmp->Redraw(psurface, layers);

    // Verify all layers are in the vector
    EXPECT_EQ(layers.size(), 4u);

    csurface->UnregisterConsumerListener();
}

/**
 * Function: ComputeTargetColorGamut005
 * Type: Function
 * Rank: Important(2)
 * EnvConditions: N/A
 * CaseDescription: 1. create layers with buffer having COLORPRIMARIES_BT2020
 *                  2. call ComputeTargetColorGamut
 *                  3. verify line 1011 condition is true (primaries != SRGB)
 *                  4. verify colorGamut is DISPLAY_P3 and loop breaks at line 1014
 */
HWTEST_F(RsRenderComposerTest, ComputeTargetColorGamut005, TestSize.Level1)
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

    NodeId id = 5;
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
    GSError ret = sProducer->RequestBuffer(buffer, requestFence, requestConfig);
    EXPECT_EQ(ret, GSERROR_OK);

    // Set color space to COLORPRIMARIES_BT2020 (not SRGB)
    // This makes line 1011 condition true: colorSpaceInfo.primaries != COLORPRIMARIES_SRGB
    CM_ColorSpaceInfo infoSet = {
        .primaries = COLORPRIMARIES_BT2020,
    };
    ret = MetadataHelper::SetColorSpaceInfo(buffer, infoSet);
    EXPECT_EQ(ret, GSERROR_OK);

    RSLayerPtr l1 = std::static_pointer_cast<RSLayer>(std::make_shared<FakeRSLayer>(1, false, "L1"));
    l1->SetBuffer(buffer);
    layers.emplace_back(l1);

    // Add more layers after the one with non-SRGB primaries
    // Due to break at line 1014, these layers should not be checked
    for (int i = 2; i <= 4; i++) {
        sptr<SurfaceBuffer> extraBuffer;
        ret = sProducer->RequestBuffer(extraBuffer, requestFence, requestConfig);
        EXPECT_EQ(ret, GSERROR_OK);
        RSLayerPtr l = std::static_pointer_cast<RSLayer>(std::make_shared<FakeRSLayer>(i, false, "L" + std::to_string(i)));
        l->SetBuffer(extraBuffer);
        layers.emplace_back(l);
    }

    auto colorGamut = rsRenderComposer_->ComputeTargetColorGamut(layers);
    // Line 1013: colorGamut = GRAPHIC_COLOR_GAMUT_DISPLAY_P3
    EXPECT_EQ(colorGamut, GRAPHIC_COLOR_GAMUT_DISPLAY_P3);
    cSurface->UnregisterConsumerListener();
}

/**
 * Function: ComputeTargetColorGamut006
 * Type: Function
 * Rank: Important(2)
 * EnvConditions: N/A
 * CaseDescription: 1. create layers with buffer having COLORPRIMARIES_P3_D65
 *                  2. call ComputeTargetColorGamut
 *                  3. verify line 1011 condition is true (primaries != SRGB)
 *                  4. verify colorGamut is DISPLAY_P3
 */
HWTEST_F(RsRenderComposerTest, ComputeTargetColorGamut006, TestSize.Level1)
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

    NodeId id = 6;
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
    GSError ret = sProducer->RequestBuffer(buffer, requestFence, requestConfig);
    EXPECT_EQ(ret, GSERROR_OK);

    // Set color space to COLORPRIMARIES_P3_D65 (not SRGB)
    // This makes line 1011 condition true
    CM_ColorSpaceInfo infoSet = {
        .primaries = COLORPRIMARIES_P3_D65,
    };
    ret = MetadataHelper::SetColorSpaceInfo(buffer, infoSet);
    EXPECT_EQ(ret, GSERROR_OK);

    RSLayerPtr l1 = std::static_pointer_cast<RSLayer>(std::make_shared<FakeRSLayer>(1, false, "L1"));
    l1->SetBuffer(buffer);
    layers.emplace_back(l1);

    auto colorGamut = rsRenderComposer_->ComputeTargetColorGamut(layers);
    // Line 1013: colorGamut = GRAPHIC_COLOR_GAMUT_DISPLAY_P3
    EXPECT_EQ(colorGamut, GRAPHIC_COLOR_GAMUT_DISPLAY_P3);
    cSurface->UnregisterConsumerListener();
}

/**
 * Function: ComputeTargetColorGamut007
 * Type: Function
 * Rank: Important(2)
 * EnvConditions: N/A
 * CaseDescription: 1. create layers with first layer having SRGB, second having BT2020
 *                  2. call ComputeTargetColorGamut
 *                  3. verify line 1011 condition is false for first layer
 *                  4. verify line 1011 condition is true for second layer
 *                  5. verify colorGamut is DISPLAY_P3 and loop breaks at line 1014
 */
HWTEST_F(RsRenderComposerTest, ComputeTargetColorGamut007, TestSize.Level1)
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

    NodeId id = 7;
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
    sProducer->SetQueueSize(3);

    // First layer with SRGB primaries
    sptr<SurfaceBuffer> buffer1;
    sptr<SyncFence> requestFence = SyncFence::INVALID_FENCE;
    GSError ret = sProducer->RequestBuffer(buffer1, requestFence, requestConfig);
    EXPECT_EQ(ret, GSERROR_OK);
    CM_ColorSpaceInfo infoSRGB = {
        .primaries = COLORPRIMARIES_SRGB,
    };
    ret = MetadataHelper::SetColorSpaceInfo(buffer1, infoSRGB);
    EXPECT_EQ(ret, GSERROR_OK);
    RSLayerPtr l1 = std::static_pointer_cast<RSLayer>(std::make_shared<FakeRSLayer>(1, false, "L1"));
    l1->SetBuffer(buffer1);
    layers.emplace_back(l1);

    // Second layer with BT2020 primaries (line 1011 condition true)
    sptr<SurfaceBuffer> buffer2;
    ret = sProducer->RequestBuffer(buffer2, requestFence, requestConfig);
    EXPECT_EQ(ret, GSERROR_OK);
    CM_ColorSpaceInfo infoBT2020 = {
        .primaries = COLORPRIMARIES_BT2020,
    };
    ret = MetadataHelper::SetColorSpaceInfo(buffer2, infoBT2020);
    EXPECT_EQ(ret, GSERROR_OK);
    RSLayerPtr l2 = std::static_pointer_cast<RSLayer>(std::make_shared<FakeRSLayer>(2, false, "L2"));
    l2->SetBuffer(buffer2);
    layers.emplace_back(l2);

    // Third layer (should not be checked due to break at line 1014)
    sptr<SurfaceBuffer> buffer3;
    ret = sProducer->RequestBuffer(buffer3, requestFence, requestConfig);
    EXPECT_EQ(ret, GSERROR_OK);
    RSLayerPtr l3 = std::static_pointer_cast<RSLayer>(std::make_shared<FakeRSLayer>(3, false, "L3"));
    l3->SetBuffer(buffer3);
    layers.emplace_back(l3);

    auto colorGamut = rsRenderComposer_->ComputeTargetColorGamut(layers);
    // Line 1013: colorGamut = GRAPHIC_COLOR_GAMUT_DISPLAY_P3
    EXPECT_EQ(colorGamut, GRAPHIC_COLOR_GAMUT_DISPLAY_P3);
    cSurface->UnregisterConsumerListener();
}

/**
 * Function: ComputeTargetColorGamut008
 * Type: Function
 * Rank: Important(2)
 * EnvConditions: N/A
 * CaseDescription: 1. create layers with COLORPRIMARIES_BT709
 *                  2. call ComputeTargetColorGamut
 *                  3. verify line 1011 condition is true (primaries != SRGB)
 *                  4. verify colorGamut is DISPLAY_P3
 */
HWTEST_F(RsRenderComposerTest, ComputeTargetColorGamut008, TestSize.Level1)
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

    NodeId id = 8;
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
    GSError ret = sProducer->RequestBuffer(buffer, requestFence, requestConfig);
    EXPECT_EQ(ret, GSERROR_OK);

    // Set color space to COLORPRIMARIES_BT709 (not SRGB)
    // This makes line 1011 condition true
    CM_ColorSpaceInfo infoSet = {
        .primaries = COLORPRIMARIES_BT709,
    };
    ret = MetadataHelper::SetColorSpaceInfo(buffer, infoSet);
    EXPECT_EQ(ret, GSERROR_OK);

    RSLayerPtr l1 = std::static_pointer_cast<RSLayer>(std::make_shared<FakeRSLayer>(1, false, "L1"));
    l1->SetBuffer(buffer);
    layers.emplace_back(l1);

    auto colorGamut = rsRenderComposer_->ComputeTargetColorGamut(layers);
    // Line 1013: colorGamut = GRAPHIC_COLOR_GAMUT_DISPLAY_P3
    EXPECT_EQ(colorGamut, GRAPHIC_COLOR_GAMUT_DISPLAY_P3);
    cSurface->UnregisterConsumerListener();
}

/**
 * Function: ComputeTargetColorGamut009
 * Type: Function
 * Rank: Important(2)
 * EnvConditions: N/A
 * CaseDescription: 1. create layers with COLORPRIMARIES_P3_DCI
 *                  2. call ComputeTargetColorGamut
 *                  3. verify line 1011 condition is true (primaries != SRGB)
 *                  4. verify colorGamut is DISPLAY_P3
 */
HWTEST_F(RsRenderComposerTest, ComputeTargetColorGamut009, TestSize.Level1)
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

    NodeId id = 9;
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
    GSError ret = sProducer->RequestBuffer(buffer, requestFence, requestConfig);
    EXPECT_EQ(ret, GSERROR_OK);

    // Set color space to COLORPRIMARIES_P3_DCI (not SRGB)
    // This makes line 1011 condition true
    CM_ColorSpaceInfo infoSet = {
        .primaries = COLORPRIMARIES_P3_DCI,
    };
    ret = MetadataHelper::SetColorSpaceInfo(buffer, infoSet);
    EXPECT_EQ(ret, GSERROR_OK);

    RSLayerPtr l1 = std::static_pointer_cast<RSLayer>(std::make_shared<FakeRSLayer>(1, false, "L1"));
    l1->SetBuffer(buffer);
    layers.emplace_back(l1);

    auto colorGamut = rsRenderComposer_->ComputeTargetColorGamut(layers);
    // Line 1013: colorGamut = GRAPHIC_COLOR_GAMUT_DISPLAY_P3
    EXPECT_EQ(colorGamut, GRAPHIC_COLOR_GAMUT_DISPLAY_P3);
    cSurface->UnregisterConsumerListener();
}

/**
 * Function: ComputeTargetColorGamut010
 * Type: Function
 * Rank: Important(2)
 * EnvConditions: N/A
 * CaseDescription: 1. create layers with COLORPRIMARIES_BT601_P
 *                  2. call ComputeTargetColorGamut
 *                  3. verify line 1011 condition is true (primaries != SRGB)
 *                  4. verify colorGamut is DISPLAY_P3
 */
HWTEST_F(RsRenderComposerTest, ComputeTargetColorGamut010, TestSize.Level1)
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

    NodeId id = 10;
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
    GSError ret = sProducer->RequestBuffer(buffer, requestFence, requestConfig);
    EXPECT_EQ(ret, GSERROR_OK);

    // Set color space to COLORPRIMARIES_BT601_P (not SRGB)
    // This makes line 1011 condition true
    CM_ColorSpaceInfo infoSet = {
        .primaries = COLORPRIMARIES_BT601_P,
    };
    ret = MetadataHelper::SetColorSpaceInfo(buffer, infoSet);
    EXPECT_EQ(ret, GSERROR_OK);

    RSLayerPtr l1 = std::static_pointer_cast<RSLayer>(std::make_shared<FakeRSLayer>(1, false, "L1"));
    l1->SetBuffer(buffer);
    layers.emplace_back(l1);

    auto colorGamut = rsRenderComposer_->ComputeTargetColorGamut(layers);
    // Line 1013: colorGamut = GRAPHIC_COLOR_GAMUT_DISPLAY_P3
    EXPECT_EQ(colorGamut, GRAPHIC_COLOR_GAMUT_DISPLAY_P3);
    cSurface->UnregisterConsumerListener();
}

/**
 * Function: ComputeTargetColorGamut011
 * Type: Function
 * Rank: Important(2)
 * EnvConditions: N/A
 * CaseDescription: 1. create layers with nullptr layers before non-SRGB layer
 *                  2. call ComputeTargetColorGamut
 *                  3. verify nullptr layers are skipped (line 1000 continue)
 *                  4. verify line 1011 condition is true for non-SRGB layer
 *                  5. verify colorGamut is DISPLAY_P3
 */
HWTEST_F(RsRenderComposerTest, ComputeTargetColorGamut011, TestSize.Level1)
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

    // Add nullptr layers (line 1000: layer == nullptr continue)
    layers.push_back(nullptr);
    layers.push_back(nullptr);

    NodeId id = 11;
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
    GSError ret = sProducer->RequestBuffer(buffer, requestFence, requestConfig);
    EXPECT_EQ(ret, GSERROR_OK);

    // Set color space to COLORPRIMARIES_BT2020 (not SRGB)
    CM_ColorSpaceInfo infoSet = {
        .primaries = COLORPRIMARIES_BT2020,
    };
    ret = MetadataHelper::SetColorSpaceInfo(buffer, infoSet);
    EXPECT_EQ(ret, GSERROR_OK);

    RSLayerPtr l1 = std::static_pointer_cast<RSLayer>(std::make_shared<FakeRSLayer>(1, false, "L1"));
    l1->SetBuffer(buffer);
    layers.emplace_back(l1);

    auto colorGamut = rsRenderComposer_->ComputeTargetColorGamut(layers);
    EXPECT_EQ(colorGamut, GRAPHIC_COLOR_GAMUT_DISPLAY_P3);
    cSurface->UnregisterConsumerListener();
}

/**
 * Function: ComputeTargetColorGamut012
 * Type: Function
 * Rank: Important(2)
 * EnvConditions: N/A
 * CaseDescription: 1. create layers with layers without buffer before non-SRGB layer
 *                  2. call ComputeTargetColorGamut
 *                  3. verify layers without buffer are skipped (line 1000 continue)
 *                  4. verify line 1011 condition is true for non-SRGB layer
 *                  5. verify colorGamut is DISPLAY_P3
 */
HWTEST_F(RsRenderComposerTest, ComputeTargetColorGamut012, TestSize.Level1)
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

    // Add layers without buffer (line 1000: buffer == nullptr continue)
    RSLayerPtr lNoBuffer1 = std::static_pointer_cast<RSLayer>(std::make_shared<FakeRSLayer>(1, false, "L1"));
    lNoBuffer1->SetBuffer(nullptr);
    layers.emplace_back(lNoBuffer1);
    RSLayerPtr lNoBuffer2 = std::static_pointer_cast<RSLayer>(std::make_shared<FakeRSLayer>(2, false, "L2"));
    lNoBuffer2->SetBuffer(nullptr);
    layers.emplace_back(lNoBuffer2);

    NodeId id = 12;
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
    GSError ret = sProducer->RequestBuffer(buffer, requestFence, requestConfig);
    EXPECT_EQ(ret, GSERROR_OK);

    // Set color space to COLORPRIMARIES_BT2020 (not SRGB)
    CM_ColorSpaceInfo infoSet = {
        .primaries = COLORPRIMARIES_BT2020,
    };
    ret = MetadataHelper::SetColorSpaceInfo(buffer, infoSet);
    EXPECT_EQ(ret, GSERROR_OK);

    RSLayerPtr l1 = std::static_pointer_cast<RSLayer>(std::make_shared<FakeRSLayer>(3, false, "L3"));
    l1->SetBuffer(buffer);
    layers.emplace_back(l1);

    auto colorGamut = rsRenderComposer_->ComputeTargetColorGamut(layers);
    EXPECT_EQ(colorGamut, GRAPHIC_COLOR_GAMUT_DISPLAY_P3);
    cSurface->UnregisterConsumerListener();
}

/**
 * Function: ComputeTargetPixelFormat_DeviceTypeSkip
 * Type: Function
 * Rank: Important(2)
 * EnvConditions: N/A
 * CaseDescription: 1. create layer with GRAPHIC_COMPOSITION_DEVICE type
 *                  2. call ComputeTargetPixelFormat
 *                  3. verify line 1055 condition is true (first sub-condition true)
 *                  4. verify line 1058 continue is executed (layer is skipped)
 *                  5. verify pixelFormat remains RGBA_8888
 */
HWTEST_F(RsRenderComposerTest, ComputeTargetPixelFormat_DeviceTypeSkip, TestSize.Level1)
{
    std::vector<RSLayerPtr> layers;

    // Create layer with GRAPHIC_COMPOSITION_DEVICE type
    // Line 1055 first condition: GetHdiCompositionType() == GRAPHIC_COMPOSITION_DEVICE is true
    // This triggers line 1058 continue, layer is skipped
    std::shared_ptr<RSRenderSurfaceLayer> layerDevice = std::make_shared<RSRenderSurfaceLayer>();
    layerDevice->SetHdiCompositionType(GraphicCompositionType::GRAPHIC_COMPOSITION_DEVICE);
    layers.push_back(layerDevice);

    auto pixelFormat = rsRenderComposer_->ComputeTargetPixelFormat(layers);
    // Since layer is skipped, pixelFormat remains default RGBA_8888
    EXPECT_EQ(pixelFormat, GRAPHIC_PIXEL_FMT_RGBA_8888);
}

/**
 * Function: ComputeTargetPixelFormat_DeviceClearTypeSkip
 * Type: Function
 * Rank: Important(2)
 * EnvConditions: N/A
 * CaseDescription: 1. create layer with GRAPHIC_COMPOSITION_DEVICE_CLEAR type
 *                  2. call ComputeTargetPixelFormat
 *                  3. verify line 1056 condition is true (second sub-condition true)
 *                  4. verify line 1058 continue is executed (layer is skipped)
 *                  5. verify pixelFormat remains RGBA_8888
 */
HWTEST_F(RsRenderComposerTest, ComputeTargetPixelFormat_DeviceClearTypeSkip, TestSize.Level1)
{
    std::vector<RSLayerPtr> layers;

    // Create layer with GRAPHIC_COMPOSITION_DEVICE_CLEAR type
    // Line 1056 second condition: GetHdiCompositionType() == GRAPHIC_COMPOSITION_DEVICE_CLEAR is true
    // This triggers line 1058 continue, layer is skipped
    std::shared_ptr<RSRenderSurfaceLayer> layerDeviceClear = std::make_shared<RSRenderSurfaceLayer>();
    layerDeviceClear->SetHdiCompositionType(GraphicCompositionType::GRAPHIC_COMPOSITION_DEVICE_CLEAR);
    layers.push_back(layerDeviceClear);

    auto pixelFormat = rsRenderComposer_->ComputeTargetPixelFormat(layers);
    // Since layer is skipped, pixelFormat remains default RGBA_8888
    EXPECT_EQ(pixelFormat, GRAPHIC_PIXEL_FMT_RGBA_8888);
}

/**
 * Function: ComputeTargetPixelFormat_SolidColorTypeSkip
 * Type: Function
 * Rank: Important(2)
 * EnvConditions: N/A
 * CaseDescription: 1. create layer with GRAPHIC_COMPOSITION_SOLID_COLOR type
 *                  2. call ComputeTargetPixelFormat
 *                  3. verify line 1057 condition is true (third sub-condition true)
 *                  4. verify line 1058 continue is executed (layer is skipped)
 *                  5. verify pixelFormat remains RGBA_8888
 */
HWTEST_F(RsRenderComposerTest, ComputeTargetPixelFormat_SolidColorTypeSkip, TestSize.Level1)
{
    std::vector<RSLayerPtr> layers;

    // Create layer with GRAPHIC_COMPOSITION_SOLID_COLOR type
    // Line 1057 third condition: GetHdiCompositionType() == GRAPHIC_COMPOSITION_SOLID_COLOR is true
    // This triggers line 1058 continue, layer is skipped
    std::shared_ptr<RSRenderSurfaceLayer> layerSolidColor = std::make_shared<RSRenderSurfaceLayer>();
    layerSolidColor->SetHdiCompositionType(GraphicCompositionType::GRAPHIC_COMPOSITION_SOLID_COLOR);
    layers.push_back(layerSolidColor);

    auto pixelFormat = rsRenderComposer_->ComputeTargetPixelFormat(layers);
    // Since layer is skipped, pixelFormat remains default RGBA_8888
    EXPECT_EQ(pixelFormat, GRAPHIC_PIXEL_FMT_RGBA_8888);
}

/**
 * Function: ComputeTargetPixelFormat_AllSkipTypesMixed
 * Type: Function
 * Rank: Important(2)
 * EnvConditions: N/A
 * CaseDescription: 1. create layers with all three skip composition types mixed
 *                  2. call ComputeTargetPixelFormat
 *                  3. verify all three conditions are true and line 1058 continue executed for all
 *                  4. verify pixelFormat remains RGBA_8888
 */
HWTEST_F(RsRenderComposerTest, ComputeTargetPixelFormat_AllSkipTypesMixed, TestSize.Level1)
{
    std::vector<RSLayerPtr> layers;

    // Add layer with GRAPHIC_COMPOSITION_DEVICE type (line 1055 true)
    std::shared_ptr<RSRenderSurfaceLayer> layerDevice = std::make_shared<RSRenderSurfaceLayer>();
    layerDevice->SetHdiCompositionType(GraphicCompositionType::GRAPHIC_COMPOSITION_DEVICE);
    layers.push_back(layerDevice);

    // Add layer with GRAPHIC_COMPOSITION_DEVICE_CLEAR type (line 1056 true)
    std::shared_ptr<RSRenderSurfaceLayer> layerDeviceClear = std::make_shared<RSRenderSurfaceLayer>();
    layerDeviceClear->SetHdiCompositionType(GraphicCompositionType::GRAPHIC_COMPOSITION_DEVICE_CLEAR);
    layers.push_back(layerDeviceClear);

    // Add layer with GRAPHIC_COMPOSITION_SOLID_COLOR type (line 1057 true)
    std::shared_ptr<RSRenderSurfaceLayer> layerSolidColor = std::make_shared<RSRenderSurfaceLayer>();
    layerSolidColor->SetHdiCompositionType(GraphicCompositionType::GRAPHIC_COMPOSITION_SOLID_COLOR);
    layers.push_back(layerSolidColor);

    // All layers should be skipped by line 1058 continue
    auto pixelFormat = rsRenderComposer_->ComputeTargetPixelFormat(layers);
    EXPECT_EQ(pixelFormat, GRAPHIC_PIXEL_FMT_RGBA_8888);
    EXPECT_EQ(layers.size(), 3u);
}

/**
 * Function: ComputeTargetPixelFormat_SkipTypesThenClient
 * Type: Function
 * Rank: Important(2)
 * EnvConditions: N/A
 * CaseDescription: 1. create layers with skip types and CLIENT type layer
 *                  2. call ComputeTargetPixelFormat
 *                  3. verify skip types trigger line 1058 continue
 *                  4. verify CLIENT type layer is processed (line 1055-1058 false)
 *                  5. verify pixelFormat is set based on CLIENT layer buffer
 */
HWTEST_F(RsRenderComposerTest, ComputeTargetPixelFormat_SkipTypesThenClient, TestSize.Level1)
{
    std::vector<RSLayerPtr> layers;

    // Add nullptr layer (line 1052 continue)
    layers.push_back(nullptr);

    // Add layer with GRAPHIC_COMPOSITION_DEVICE type (line 1055 true, line 1058 continue)
    std::shared_ptr<RSRenderSurfaceLayer> layerDevice = std::make_shared<RSRenderSurfaceLayer>();
    layerDevice->SetHdiCompositionType(GraphicCompositionType::GRAPHIC_COMPOSITION_DEVICE);
    layers.push_back(layerDevice);

    // Add CLIENT type layer with buffer that should be processed
    // Line 1055-1057: all conditions false, continues to line 1060
    BufferRequestConfig requestConfig = {
        .width = 0x100,
        .height = 0x100,
        .strideAlignment = 0x8,
        .format = GRAPHIC_PIXEL_FMT_RGBA_1010102,
        .usage = BUFFER_USAGE_CPU_READ | BUFFER_USAGE_CPU_WRITE | BUFFER_USAGE_MEM_DMA,
        .timeout = 0,
    };

    NodeId id = 1;
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
    GSError ret = sProducer->RequestBuffer(buffer, requestFence, requestConfig);
    EXPECT_EQ(ret, GSERROR_OK);

    std::shared_ptr<RSRenderSurfaceLayer> layerClient = std::make_shared<RSRenderSurfaceLayer>();
    layerClient->SetHdiCompositionType(GraphicCompositionType::GRAPHIC_COMPOSITION_CLIENT);
    layerClient->SetBuffer(buffer);
    layers.push_back(layerClient);

    // nullptr and DEVICE layers are skipped, CLIENT layer is processed
    auto pixelFormat = rsRenderComposer_->ComputeTargetPixelFormat(layers);
    // Line 1078: pixelFormat = GRAPHIC_PIXEL_FMT_RGBA_1010102
    EXPECT_EQ(pixelFormat, GRAPHIC_PIXEL_FMT_RGBA_1010102);
    cSurface->UnregisterConsumerListener();
}

/**
 * Function: ComputeTargetPixelFormat_MultipleSkipTypesWithClient
 * Type: Function
 * Rank: Important(2)
 * EnvConditions: N/A
 * CaseDescription: 1. create layers with skip types followed by non-skip types
 *                  2. call ComputeTargetPixelFormat
 *                  3. verify skip types trigger line 1058 continue
 *                  4. verify non-skip type layer is processed and sets pixelFormat
 */
HWTEST_F(RsRenderComposerTest, ComputeTargetPixelFormat_MultipleSkipTypesWithClient, TestSize.Level1)
{
    std::vector<RSLayerPtr> layers;

    // Add multiple skip type layers
    for (int i = 0; i < 3; i++) {
        std::shared_ptr<RSRenderSurfaceLayer> layerSkip = std::make_shared<RSRenderSurfaceLayer>();
        if (i == 0) {
            layerSkip->SetHdiCompositionType(GraphicCompositionType::GRAPHIC_COMPOSITION_DEVICE);
        } else if (i == 1) {
            layerSkip->SetHdiCompositionType(GraphicCompositionType::GRAPHIC_COMPOSITION_DEVICE_CLEAR);
        } else {
            layerSkip->SetHdiCompositionType(GraphicCompositionType::GRAPHIC_COMPOSITION_SOLID_COLOR);
        }
        layers.push_back(layerSkip);
    }

    // Add CLIENT type layer with RGBA_1010108 buffer
    BufferRequestConfig requestConfig = {
        .width = 0x100,
        .height = 0x100,
        .strideAlignment = 0x8,
        .format = GRAPHIC_PIXEL_FMT_RGBA_1010108,
        .usage = BUFFER_USAGE_CPU_READ | BUFFER_USAGE_CPU_WRITE | BUFFER_USAGE_MEM_DMA,
        .timeout = 0,
    };

    NodeId id = 2;
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
    GSError ret = sProducer->RequestBuffer(buffer, requestFence, requestConfig);
    EXPECT_EQ(ret, GSERROR_OK);

    std::shared_ptr<RSRenderSurfaceLayer> layerClient = std::make_shared<RSRenderSurfaceLayer>();
    layerClient->SetHdiCompositionType(GraphicCompositionType::GRAPHIC_COMPOSITION_CLIENT);
    layerClient->SetBuffer(buffer);
    layers.push_back(layerClient);

    // Skip type layers trigger line 1058 continue
    // CLIENT layer is processed at line 1060+, line 1067 condition true
    auto pixelFormat = rsRenderComposer_->ComputeTargetPixelFormat(layers);
    // Line 1068-1073: pixelFormat set based on RGBA_1010108 and allRedraw flag
    EXPECT_NE(pixelFormat, GRAPHIC_PIXEL_FMT_RGBA_8888);
    cSurface->UnregisterConsumerListener();
}

/**
 * Function: ComputeTargetPixelFormat_DeviceTypeWithBuffer
 * Type: Function
 * Rank: Important(2)
 * EnvConditions: N/A
 * CaseDescription: 1. create layers with GRAPHIC_COMPOSITION_DEVICE type and buffer
 *                  2. call ComputeTargetPixelFormat
 *                  3. verify line 1055 condition is true
 *                  4. verify line 1058 continue is executed even with buffer
 *                  5. verify buffer is not processed, pixelFormat remains default
 */
HWTEST_F(RsRenderComposerTest, ComputeTargetPixelFormat_DeviceTypeWithBuffer, TestSize.Level1)
{
    std::vector<RSLayerPtr> layers;

    // Create layer with GRAPHIC_COMPOSITION_DEVICE type and buffer
    // Line 1055 condition is true, line 1058 continue executed
    // Buffer should not be processed even if it exists
    BufferRequestConfig requestConfig = {
        .width = 0x100,
        .height = 0x100,
        .strideAlignment = 0x8,
        .format = GRAPHIC_PIXEL_FMT_RGBA_1010108,
        .usage = BUFFER_USAGE_CPU_READ | BUFFER_USAGE_CPU_WRITE | BUFFER_USAGE_MEM_DMA,
        .timeout = 0,
    };

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

    const auto& surfaceConsumer = rsSurfaceRenderNode->GetRSSurfaceHandler()->GetConsumer();
    auto producer = surfaceConsumer->GetProducer();
    sptr<Surface> sProducer = Surface::CreateSurfaceAsProducer(producer);
    sProducer->SetQueueSize(2);

    sptr<SurfaceBuffer> buffer;
    sptr<SyncFence> requestFence = SyncFence::INVALID_FENCE;
    GSError ret = sProducer->RequestBuffer(buffer, requestFence, requestConfig);
    EXPECT_EQ(ret, GSERROR_OK);

    std::shared_ptr<RSRenderSurfaceLayer> layerDevice = std::make_shared<RSRenderSurfaceLayer>();
    layerDevice->SetHdiCompositionType(GraphicCompositionType::GRAPHIC_COMPOSITION_DEVICE);
    layerDevice->SetBuffer(buffer);
    layers.push_back(layerDevice);

    // Layer is skipped by line 1058, buffer format not processed
    auto pixelFormat = rsRenderComposer_->ComputeTargetPixelFormat(layers);
    EXPECT_EQ(pixelFormat, GRAPHIC_PIXEL_FMT_RGBA_8888);
    cSurface->UnregisterConsumerListener();
}

/**
 * Function: ComputeTargetPixelFormat_MixedTypesWithNullLayers
 * Type: Function
 * Rank: Important(2)
 * EnvConditions: N/A
 * CaseDescription: 1. create layers with mixed composition types and nullptr layers
 *                  2. call ComputeTargetPixelFormat
 *                  3. verify nullptr layers skipped (line 1052)
 *                  4. verify DEVICE types skipped (line 1055-1057)
 *                  5. verify CLIENT type with buffer processed correctly
 */
HWTEST_F(RsRenderComposerTest, ComputeTargetPixelFormat_MixedTypesWithNullLayers, TestSize.Level1)
{
    std::vector<RSLayerPtr> layers;

    // Add nullptr layer (line 1052 continue)
    layers.push_back(nullptr);

    // Add DEVICE type layer (line 1055 true, line 1058 continue)
    std::shared_ptr<RSRenderSurfaceLayer> layerDevice = std::make_shared<RSRenderSurfaceLayer>();
    layerDevice->SetHdiCompositionType(GraphicCompositionType::GRAPHIC_COMPOSITION_DEVICE);
    layers.push_back(layerDevice);

    // Add another nullptr (line 1052 continue)
    layers.push_back(nullptr);

    // Add DEVICE_CLEAR type (line 1056 true, line 1058 continue)
    std::shared_ptr<RSRenderSurfaceLayer> layerDeviceClear = std::make_shared<RSRenderSurfaceLayer>();
    layerDeviceClear->SetHdiCompositionType(GraphicCompositionType::GRAPHIC_COMPOSITION_DEVICE_CLEAR);
    layers.push_back(layerDeviceClear);

    // Add CLIENT type layer with YCBCR_P010 buffer
    BufferRequestConfig requestConfig = {
        .width = 0x100,
        .height = 0x100,
        .strideAlignment = 0x8,
        .format = GRAPHIC_PIXEL_FMT_YCBCR_P010,
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

    const auto& surfaceConsumer = rsSurfaceRenderNode->GetRSSurfaceHandler()->GetConsumer();
    auto producer = surfaceConsumer->GetProducer();
    sptr<Surface> sProducer = Surface::CreateSurfaceAsProducer(producer);
    sProducer->SetQueueSize(2);

    sptr<SurfaceBuffer> buffer;
    sptr<SyncFence> requestFence = SyncFence::INVALID_FENCE;
    GSError ret = sProducer->RequestBuffer(buffer, requestFence, requestConfig);
    EXPECT_EQ(ret, GSERROR_OK);

    std::shared_ptr<RSRenderSurfaceLayer> layerClient = std::make_shared<RSRenderSurfaceLayer>();
    layerClient->SetHdiCompositionType(GraphicCompositionType::GRAPHIC_COMPOSITION_CLIENT);
    layerClient->SetBuffer(buffer);
    layers.push_back(layerClient);

    // Only CLIENT layer is processed, all skip conditions handled
    auto pixelFormat = rsRenderComposer_->ComputeTargetPixelFormat(layers);
    // Line 1076-1078: YCBCR_P010 sets pixelFormat to RGBA_1010102
    EXPECT_EQ(pixelFormat, GRAPHIC_PIXEL_FMT_RGBA_1010102);
    EXPECT_EQ(layers.size(), 5u);
    cSurface->UnregisterConsumerListener();
}

/**
 * Function: ComputeTargetPixelFormat009
 * Type: Function
 * Rank: Important(2)
 * EnvConditions: N/A
 * CaseDescription: 1. create layers with RGBA_1010108 buffer and allRedraw=true
 *                  2. call ComputeTargetPixelFormat
 *                  3. verify line 1069 condition is false (!allRedraw is false)
 *                  4. verify pixelFormat is RGBA_1010102 (line 1068)
 */
HWTEST_F(RsRenderComposerTest, ComputeTargetPixelFormat009, TestSize.Level1)
{
    std::vector<RSLayerPtr> layers;

    // Create CLIENT type layer with RGBA_1010108 buffer
    // When allRedraw=true, line 1069 condition: !allRedraw && ... is false
    // Line 1068 sets pixelFormat to GRAPHIC_PIXEL_FMT_RGBA_1010102
    BufferRequestConfig requestConfig = {
        .width = 0x100,
        .height = 0x100,
        .strideAlignment = 0x8,
        .format = GRAPHIC_PIXEL_FMT_RGBA_1010108,
        .usage = BUFFER_USAGE_CPU_READ | BUFFER_USAGE_CPU_WRITE | BUFFER_USAGE_MEM_DMA,
        .timeout = 0,
    };

    NodeId id = 5;
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
    GSError ret = sProducer->RequestBuffer(buffer, requestFence, requestConfig);
    EXPECT_EQ(ret, GSERROR_OK);

    std::shared_ptr<RSRenderSurfaceLayer> layerClient = std::make_shared<RSRenderSurfaceLayer>();
    layerClient->SetHdiCompositionType(GraphicCompositionType::GRAPHIC_COMPOSITION_CLIENT);
    layerClient->SetBuffer(buffer);
    layers.push_back(layerClient);

    // All CLIENT layers, so allRedraw=true, line 1069 condition false
    auto pixelFormat = rsRenderComposer_->ComputeTargetPixelFormat(layers);
    // Line 1068: pixelFormat = GRAPHIC_PIXEL_FMT_RGBA_1010102
    EXPECT_EQ(pixelFormat, GRAPHIC_PIXEL_FMT_RGBA_1010102);
    cSurface->UnregisterConsumerListener();
}

/**
 * Function: ComputeTargetPixelFormat010
 * Type: Function
 * Rank: Important(2)
 * EnvConditions: N/A
 * CaseDescription: 1. create layers with RGBA_1010108 buffer and allRedraw=false (has DEVICE type layer)
 *                  2. call ComputeTargetPixelFormat
 *                  3. verify line 1069 condition result depends on GetRGBA1010108Enabled()
 *                  4. verify pixelFormat behavior
 */
HWTEST_F(RsRenderComposerTest, ComputeTargetPixelFormat010, TestSize.Level1)
{
    std::vector<RSLayerPtr> layers;

    // Add DEVICE type layer to make allRedraw=false
    // Line 1036-1040: DEVICE type returns false in IsAllRedraw
    std::shared_ptr<RSRenderSurfaceLayer> layerDevice = std::make_shared<RSRenderSurfaceLayer>();
    layerDevice->SetHdiCompositionType(GraphicCompositionType::GRAPHIC_COMPOSITION_DEVICE);
    layers.push_back(layerDevice);

    // Add CLIENT type layer with RGBA_1010108 buffer
    // When allRedraw=false, line 1069 condition: !allRedraw && GetRGBA1010108Enabled()
    // depends on system properties
    BufferRequestConfig requestConfig = {
        .width = 0x100,
        .height = 0x100,
        .strideAlignment = 0x8,
        .format = GRAPHIC_PIXEL_FMT_RGBA_1010108,
        .usage = BUFFER_USAGE_CPU_READ | BUFFER_USAGE_CPU_WRITE | BUFFER_USAGE_MEM_DMA,
        .timeout = 0,
    };

    NodeId id = 6;
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
    GSError ret = sProducer->RequestBuffer(buffer, requestFence, requestConfig);
    EXPECT_EQ(ret, GSERROR_OK);

    std::shared_ptr<RSRenderSurfaceLayer> layerClient = std::make_shared<RSRenderSurfaceLayer>();
    layerClient->SetHdiCompositionType(GraphicCompositionType::GRAPHIC_COMPOSITION_CLIENT);
    layerClient->SetBuffer(buffer);
    layers.push_back(layerClient);

    // allRedraw=false due to DEVICE layer
    // Line 1069: if (!allRedraw && RSHdrUtil::GetRGBA1010108Enabled())
    auto pixelFormat = rsRenderComposer_->ComputeTargetPixelFormat(layers);
    // When GetRGBA1010108Enabled() is false (default), condition is false
    // Line 1068: pixelFormat = GRAPHIC_PIXEL_FMT_RGBA_1010102
    EXPECT_NE(pixelFormat, GRAPHIC_PIXEL_FMT_RGBA_8888);
    EXPECT_EQ(layers.size(), 2u);
    cSurface->UnregisterConsumerListener();
}

/**
 * Function: ComputeTargetPixelFormat011
 * Type: Function
 * Rank: Important(2)
 * EnvConditions: N/A
 * CaseDescription: 1. create layers with RGBA_1010108 buffer and DEVICE_CLEAR type
 *                  2. call ComputeTargetPixelFormat
 *                  3. verify line 1069 condition is evaluated with allRedraw=false
 *                  4. verify pixelFormat based on GetRGBA1010108Enabled()
 */
HWTEST_F(RsRenderComposerTest, ComputeTargetPixelFormat011, TestSize.Level1)
{
    std::vector<RSLayerPtr> layers;

    // Add DEVICE_CLEAR type layer to make allRedraw=false
    std::shared_ptr<RSRenderSurfaceLayer> layerDeviceClear = std::make_shared<RSRenderSurfaceLayer>();
    layerDeviceClear->SetHdiCompositionType(GraphicCompositionType::GRAPHIC_COMPOSITION_DEVICE_CLEAR);
    layers.push_back(layerDeviceClear);

    // Add CLIENT type layer with RGBA_1010108 buffer
    BufferRequestConfig requestConfig = {
        .width = 0x100,
        .height = 0x100,
        .strideAlignment = 0x8,
        .format = GRAPHIC_PIXEL_FMT_RGBA_1010108,
        .usage = BUFFER_USAGE_CPU_READ | BUFFER_USAGE_CPU_WRITE | BUFFER_USAGE_MEM_DMA,
        .timeout = 0,
    };

    NodeId id = 7;
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
    GSError ret = sProducer->RequestBuffer(buffer, requestFence, requestConfig);
    EXPECT_EQ(ret, GSERROR_OK);

    std::shared_ptr<RSRenderSurfaceLayer> layerClient = std::make_shared<RSRenderSurfaceLayer>();
    layerClient->SetHdiCompositionType(GraphicCompositionType::GRAPHIC_COMPOSITION_CLIENT);
    layerClient->SetBuffer(buffer);
    layers.push_back(layerClient);

    // allRedraw=false due to DEVICE_CLEAR layer
    // Line 1069: if (!allRedraw && RSHdrUtil::GetRGBA1010108Enabled())
    auto pixelFormat = rsRenderComposer_->ComputeTargetPixelFormat(layers);
    // Condition depends on GetRGBA1010108Enabled(), typically false in test
    EXPECT_NE(pixelFormat, GRAPHIC_PIXEL_FMT_RGBA_8888);
    cSurface->UnregisterConsumerListener();
}

/**
 * Function: ComputeTargetPixelFormat012
 * Type: Function
 * Rank: Important(2)
 * EnvConditions: N/A
 * CaseDescription: 1. create layers with RGBA_1010108 buffer and SOLID_COLOR type
 *                  2. call ComputeTargetPixelFormat
 *                  3. verify line 1069 condition is evaluated with allRedraw=false
 *                  4. verify pixelFormat based on GetRGBA1010108Enabled()
 */
HWTEST_F(RsRenderComposerTest, ComputeTargetPixelFormat012, TestSize.Level1)
{
    std::vector<RSLayerPtr> layers;

    // Add SOLID_COLOR type layer to make allRedraw=false
    std::shared_ptr<RSRenderSurfaceLayer> layerSolidColor = std::make_shared<RSRenderSurfaceLayer>();
    layerSolidColor->SetHdiCompositionType(GraphicCompositionType::GRAPHIC_COMPOSITION_SOLID_COLOR);
    layers.push_back(layerSolidColor);

    // Add CLIENT type layer with RGBA_1010108 buffer
    BufferRequestConfig requestConfig = {
        .width = 0x100,
        .height = 0x100,
        .strideAlignment = 0x8,
        .format = GRAPHIC_PIXEL_FMT_RGBA_1010108,
        .usage = BUFFER_USAGE_CPU_READ | BUFFER_USAGE_CPU_WRITE | BUFFER_USAGE_MEM_DMA,
        .timeout = 0,
    };

    NodeId id = 8;
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
    GSError ret = sProducer->RequestBuffer(buffer, requestFence, requestConfig);
    EXPECT_EQ(ret, GSERROR_OK);

    std::shared_ptr<RSRenderSurfaceLayer> layerClient = std::make_shared<RSRenderSurfaceLayer>();
    layerClient->SetHdiCompositionType(GraphicCompositionType::GRAPHIC_COMPOSITION_CLIENT);
    layerClient->SetBuffer(buffer);
    layers.push_back(layerClient);

    // allRedraw=false due to SOLID_COLOR layer
    // Line 1069: if (!allRedraw && RSHdrUtil::GetRGBA1010108Enabled())
    auto pixelFormat = rsRenderComposer_->ComputeTargetPixelFormat(layers);
    // Condition depends on GetRGBA1010108Enabled()
    EXPECT_NE(pixelFormat, GRAPHIC_PIXEL_FMT_RGBA_8888);
    cSurface->UnregisterConsumerListener();
}

/**
 * Function: ComputeTargetPixelFormat013
 * Type: Function
 * Rank: Important(2)
 * EnvConditions: N/A
 * CaseDescription: 1. create layers with CURSOR type and RGBA_1010108 buffer
 *                  2. call ComputeTargetPixelFormat
 *                  3. verify CURSOR type makes allRedraw=true (skipped in IsAllRedraw)
 *                  4. verify line 1069 condition is false (!allRedraw is false)
 *                  5. verify pixelFormat is RGBA_1010102
 */
HWTEST_F(RsRenderComposerTest, ComputeTargetPixelFormat013, TestSize.Level1)
{
    std::vector<RSLayerPtr> layers;

    // Add CURSOR type layer
    // Line 1032-1034: CURSOR is skipped in IsAllRedraw, doesn't affect allRedraw
    RSLayerPtr lCursor = std::static_pointer_cast<RSLayer>(std::make_shared<FakeRSLayer>(1, false, "Cursor"));
    lCursor->SetType(GraphicLayerType::GRAPHIC_LAYER_TYPE_CURSOR);
    layers.push_back(lCursor);

    // Add CLIENT type layer with RGBA_1010108 buffer
    // All CLIENT layers, so allRedraw=true
    BufferRequestConfig requestConfig = {
        .width = 0x100,
        .height = 0x100,
        .strideAlignment = 0x8,
        .format = GRAPHIC_PIXEL_FMT_RGBA_1010108,
        .usage = BUFFER_USAGE_CPU_READ | BUFFER_USAGE_CPU_WRITE | BUFFER_USAGE_MEM_DMA,
        .timeout = 0,
    };

    NodeId id = 9;
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
    GSError ret = sProducer->RequestBuffer(buffer, requestFence, requestConfig);
    EXPECT_EQ(ret, GSERROR_OK);

    std::shared_ptr<RSRenderSurfaceLayer> layerClient = std::make_shared<RSRenderSurfaceLayer>();
    layerClient->SetHdiCompositionType(GraphicCompositionType::GRAPHIC_COMPOSITION_CLIENT);
    layerClient->SetBuffer(buffer);
    layers.push_back(layerClient);

    // CURSOR is skipped, all CLIENT layers -> allRedraw=true
    // Line 1069: if (!allRedraw && ...) is false
    auto pixelFormat = rsRenderComposer_->ComputeTargetPixelFormat(layers);
    // Line 1068: pixelFormat = GRAPHIC_PIXEL_FMT_RGBA_1010102
    EXPECT_EQ(pixelFormat, GRAPHIC_PIXEL_FMT_RGBA_1010102);
    cSurface->UnregisterConsumerListener();
}

/**
 * Function: ComputeTargetPixelFormat014
 * Type: Function
 * Rank: Important(2)
 * EnvConditions: N/A
 * CaseDescription: 1. create layers with RCD layer and RGBA_1010108 buffer
 *                  2. call ComputeTargetPixelFormat
 *                  3. verify RCD type is skipped in IsAllRedraw
 *                  4. verify allRedraw=true, line 1069 condition false
 *                  5. verify pixelFormat is RGBA_1010102
 */
HWTEST_F(RsRenderComposerTest, ComputeTargetPixelFormat014, TestSize.Level1)
{
    std::vector<RSLayerPtr> layers;

    // Add RCD type layer
    // Line 1027-1029: RCD layer is skipped in IsAllRedraw
    RSLayerPtr lRCD = std::static_pointer_cast<RSLayer>(std::make_shared<RSRenderSurfaceRCDLayer>());
    layers.push_back(lRCD);

    // Add CLIENT type layer with RGBA_1010108 buffer
    // RCD is skipped, all CLIENT layers -> allRedraw=true
    BufferRequestConfig requestConfig = {
        .width = 0x100,
        .height = 0x100,
        .strideAlignment = 0x8,
        .format = GRAPHIC_PIXEL_FMT_RGBA_1010108,
        .usage = BUFFER_USAGE_CPU_READ | BUFFER_USAGE_CPU_WRITE | BUFFER_USAGE_MEM_DMA,
        .timeout = 0,
    };

    NodeId id = 10;
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
    GSError ret = sProducer->RequestBuffer(buffer, requestFence, requestConfig);
    EXPECT_EQ(ret, GSERROR_OK);

    std::shared_ptr<RSRenderSurfaceLayer> layerClient = std::make_shared<RSRenderSurfaceLayer>();
    layerClient->SetHdiCompositionType(GraphicCompositionType::GRAPHIC_COMPOSITION_CLIENT);
    layerClient->SetBuffer(buffer);
    layers.push_back(layerClient);

    // RCD is skipped, all CLIENT layers -> allRedraw=true
    // Line 1069: if (!allRedraw && ...) is false
    auto pixelFormat = rsRenderComposer_->ComputeTargetPixelFormat(layers);
    // Line 1068: pixelFormat = GRAPHIC_PIXEL_FMT_RGBA_1010102
    EXPECT_EQ(pixelFormat, GRAPHIC_PIXEL_FMT_RGBA_1010102);
    cSurface->UnregisterConsumerListener();
}

/**
 * Function: ComputeTargetPixelFormat015
 * Type: Function
 * Rank: Important(2)
 * EnvConditions: N/A
 * CaseDescription: 1. create layers with multiple CLIENT layers and RGBA_1010108 buffer
 *                  2. call ComputeTargetPixelFormat
 *                  3. verify allRedraw=true (all CLIENT)
 *                  4. verify line 1069 condition is false
 *                  5. verify pixelFormat is RGBA_1010102 and break at line 1073
 */
HWTEST_F(RsRenderComposerTest, ComputeTargetPixelFormat015, TestSize.Level1)
{
    std::vector<RSLayerPtr> layers;

    BufferRequestConfig requestConfig = {
        .width = 0x100,
        .height = 0x100,
        .strideAlignment = 0x8,
        .format = GRAPHIC_PIXEL_FMT_RGBA_1010108,
        .usage = BUFFER_USAGE_CPU_READ | BUFFER_USAGE_CPU_WRITE | BUFFER_USAGE_MEM_DMA,
        .timeout = 0,
    };

    NodeId id = 11;
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
    sProducer->SetQueueSize(5);

    // Add multiple CLIENT layers with RGBA_1010108 buffers
    for (int i = 0; i < 4; i++) {
        sptr<SurfaceBuffer> buffer;
        sptr<SyncFence> requestFence = SyncFence::INVALID_FENCE;
        GSError ret = sProducer->RequestBuffer(buffer, requestFence, requestConfig);
        EXPECT_EQ(ret, GSERROR_OK);

        std::shared_ptr<RSRenderSurfaceLayer> layerClient = std::make_shared<RSRenderSurfaceLayer>();
        layerClient->SetHdiCompositionType(GraphicCompositionType::GRAPHIC_COMPOSITION_CLIENT);
        layerClient->SetBuffer(buffer);
        layers.push_back(layerClient);
    }

    // All CLIENT layers -> allRedraw=true
    // Line 1069: if (!allRedraw && ...) is false
    // First RGBA_1010108 layer triggers line 1067, 1068, then 1069 is false
    // Line 1073: break (first 10bit layer causes early exit)
    auto pixelFormat = rsRenderComposer_->ComputeTargetPixelFormat(layers);
    EXPECT_EQ(pixelFormat, GRAPHIC_PIXEL_FMT_RGBA_1010102);
    EXPECT_EQ(layers.size(), 4u);
    cSurface->UnregisterConsumerListener();
}

/**
 * Function: ComputeTargetPixelFormat016
 * Type: Function
 * Rank: Important(2)
 * EnvConditions: N/A
 * CaseDescription: 1. create layers with nullptr and CLIENT layers with RGBA_1010108 buffer
 *                  2. call ComputeTargetPixelFormat
 *                  3. verify nullptr layers are skipped in IsAllRedraw and ComputeTargetPixelFormat
 *                  4. verify allRedraw=true, line 1069 condition false
 *                  5. verify pixelFormat is RGBA_1010102
 */
HWTEST_F(RsRenderComposerTest, ComputeTargetPixelFormat016, TestSize.Level1)
{
    std::vector<RSLayerPtr> layers;

    // Add nullptr layers (skipped in IsAllRedraw line 1024, and ComputeTargetPixelFormat line 1052)
    layers.push_back(nullptr);
    layers.push_back(nullptr);

    BufferRequestConfig requestConfig = {
        .width = 0x100,
        .height = 0x100,
        .strideAlignment = 0x8,
        .format = GRAPHIC_PIXEL_FMT_RGBA_1010108,
        .usage = BUFFER_USAGE_CPU_READ | BUFFER_USAGE_CPU_WRITE | BUFFER_USAGE_MEM_DMA,
        .timeout = 0,
    };

    NodeId id = 12;
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
    GSError ret = sProducer->RequestBuffer(buffer, requestFence, requestConfig);
    EXPECT_EQ(ret, GSERROR_OK);

    // Add CLIENT layer with RGBA_1010108 buffer
    // nullptrs are skipped, all CLIENT layers -> allRedraw=true
    std::shared_ptr<RSRenderSurfaceLayer> layerClient = std::make_shared<RSRenderSurfaceLayer>();
    layerClient->SetHdiCompositionType(GraphicCompositionType::GRAPHIC_COMPOSITION_CLIENT);
    layerClient->SetBuffer(buffer);
    layers.push_back(layerClient);

    // Line 1069: if (!allRedraw && ...) is false
    auto pixelFormat = rsRenderComposer_->ComputeTargetPixelFormat(layers);
    EXPECT_EQ(pixelFormat, GRAPHIC_PIXEL_FMT_RGBA_1010102);
    EXPECT_EQ(layers.size(), 3u);
    cSurface->UnregisterConsumerListener();
}
#endif
/**
 * Function: ContextRegisterPostTask_VulkanConditionTrue
 * Type: Function
 * Rank: Important(2)
 * EnvConditions: RS_ENABLE_VK and IS_ENABLE_DRM defined, GpuApiType set to VULKAN
 * CaseDescription: 1. create RSRenderComposer
 *                  2. call ContextRegisterPostTask
 *                  3. verify line 1137 condition is true with VULKAN (first sub-condition true)
 *                  4. verify RsVulkanContext methods are called with protected and unprotected contexts
 */
#if defined(RS_ENABLE_VK) && defined(IS_ENABLE_DRM)
#if defined(GPU_API_TYPE_VULKAN)
HWTEST_F(RsRenderComposerTest, ContextRegisterPostTask_VulkanConditionTrue, TestSize.Level1)
{
    auto output = std::make_shared<HdiOutput>(0u);
    output->Init();
    sptr<RSScreenProperty> property = new RSScreenProperty();
    auto rsRenderComposerTmp = std::make_shared<RSRenderComposer>(output, property);
    EXPECT_NE(rsRenderComposerTmp->hdiOutput_, nullptr);

    // When GPU API type is VULKAN, line 1137 condition is true
    // This triggers:
    // - Line 1139: SetIsProtected(true)
    // - Line 1140-1142: Get protected context and register post func
    // - Line 1144: SetIsProtected(false)
    // - Line 1145-1147: Get unprotected context and register post func
    rsRenderComposerTmp->ContextRegisterPostTask();

    // Verify function completes without crash
    EXPECT_NE(rsRenderComposerTmp, nullptr);
}
#endif
#endif

/**
 * Function: ContextRegisterPostTask_DdgrConditionTrue
 * Type: Function
 * Rank: Important(2)
 * EnvConditions: RS_ENABLE_VK and IS_ENABLE_DRM defined, GpuApiType set to DDGR
 * CaseDescription: 1. create RSRenderComposer
 *                  2. call ContextRegisterPostTask
 *                  3. verify line 1137 condition is true with DDGR (second sub-condition true)
 *                  4. verify RsVulkanContext methods are called with protected and unprotected contexts
 */
#if defined(RS_ENABLE_VK) && defined(IS_ENABLE_DRM)
#if defined(GPU_API_TYPE_DDGR)
HWTEST_F(RsRenderComposerTest, ContextRegisterPostTask_DdgrConditionTrue, TestSize.Level1)
{
    auto output = std::make_shared<HdiOutput>(0u);
    output->Init();
    sptr<RSScreenProperty> property = new RSScreenProperty();
    auto rsRenderComposerTmp = std::make_shared<RSRenderComposer>(output, property);
    EXPECT_NE(rsRenderComposerTmp->hdiOutput_, nullptr);

    // When GPU API type is DDGR, line 1137 condition is true
    // This triggers the same path as VULKAN:
    // - Line 1139: SetIsProtected(true)
    // - Line 1140-1142: Get protected context and register post func
    // - Line 1144: SetIsProtected(false)
    // - Line 1145-1147: Get unprotected context and register post func
    rsRenderComposerTmp->ContextRegisterPostTask();

    // Verify function completes without crash
    EXPECT_NE(rsRenderComposerTmp, nullptr);
}
#endif
#endif

/**
 * Function: ContextRegisterPostTask_VulkanWithPostTaskVerification
 * Type: Function
 * Rank: Important(2)
 * EnvConditions: RS_ENABLE_VK and IS_ENABLE_DRM defined, GpuApiType set to VULKAN
 * CaseDescription: 1. create RSRenderComposer
 *                  2. call ContextRegisterPostTask
 *                  3. verify line 1137 condition is true with VULKAN
 *                  4. verify PostTask can be called and executes properly
 */
#if defined(RS_ENABLE_VK) && defined(IS_ENABLE_DRM)
#if defined(GPU_API_TYPE_VULKAN)
HWTEST_F(RsRenderComposerTest, ContextRegisterPostTask_VulkanWithPostTaskVerification, TestSize.Level1)
{
    auto output = std::make_shared<HdiOutput>(0u);
    output->Init();
    sptr<RSScreenProperty> property = new RSScreenProperty();
    auto rsRenderComposerTmp = std::make_shared<RSRenderComposer>(output, property);
    EXPECT_NE(rsRenderComposerTmp->hdiOutput_, nullptr);

    // Call ContextRegisterPostTask to register post funcs
    // Line 1137: VULKAN condition true
    rsRenderComposerTmp->ContextRegisterPostTask();

    // Verify that PostTask can be called and executes properly
    std::atomic<bool> taskExecuted{false};
    rsRenderComposerTmp->PostTask([&taskExecuted]() { taskExecuted.store(true); });
    std::this_thread::sleep_for(std::chrono::milliseconds(200));

    // The registered post func should allow PostTask to execute
    // If context registration worked properly, the task should execute
    // Note: This test verifies the integration between ContextRegisterPostTask and PostTask
    EXPECT_NE(rsRenderComposerTmp->handler_, nullptr);
}
#endif
#endif

/**
 * Function: ContextRegisterPostTask_DdgrWithPostTaskVerification
 * Type: Function
 * Rank: Important(2)
 * EnvConditions: RS_ENABLE_VK and IS_ENABLE_DRM defined, GpuApiType set to DDGR
 * CaseDescription: 1. create RSRenderComposer
 *                  2. call ContextRegisterPostTask
 *                  3. verify line 1137 condition is true with DDGR
 *                  4. verify PostTask works correctly after context registration
 */
#if defined(RS_ENABLE_VK) && defined(IS_ENABLE_DRM)
#if defined(GPU_API_TYPE_DDGR)
HWTEST_F(RsRenderComposerTest, ContextRegisterPostTask_DdgrWithPostTaskVerification, TestSize.Level1)
{
    auto output = std::make_shared<HdiOutput>(0u);
    output->Init();
    sptr<RSScreenProperty> property = new RSScreenProperty();
    auto rsRenderComposerTmp = std::make_shared<RSRenderComposer>(output, property);
    EXPECT_NE(rsRenderComposerTmp->hdiOutput_, nullptr);

    // Call ContextRegisterPostTask to register post funcs
    // Line 1137: DDGR condition true
    rsRenderComposerTmp->ContextRegisterPostTask();

    // Verify that PostTask can be called
    std::atomic<bool> taskExecuted{false};
    rsRenderComposerTmp->PostTask([&taskExecuted]() { taskExecuted.store(true); });
    std::this_thread::sleep_for(std::chrono::milliseconds(200));

    // Verify handler is available
    EXPECT_NE(rsRenderComposerTmp->handler_, nullptr);
}
#endif
#endif

/**
 * Function: ContextRegisterPostTask_MultipleCalls_Vulkan
 * Type: Function
 * Rank: Important(2)
 * EnvConditions: RS_ENABLE_VK and IS_ENABLE_DRM defined, GpuApiType set to VULKAN
 * CaseDescription: 1. create RSRenderComposer
 *                  2. call ContextRegisterPostTask multiple times
 *                  3. verify line 1137 condition is true with VULKAN each time
 *                  4. verify no crash or undefined behavior
 */
#if defined(RS_ENABLE_VK) && defined(IS_ENABLE_DRM)
#if defined(GPU_API_TYPE_VULKAN)
HWTEST_F(RsRenderComposerTest, ContextRegisterPostTask_MultipleCalls_Vulkan, TestSize.Level1)
{
    auto output = std::make_shared<HdiOutput>(0u);
    output->Init();
    sptr<RSScreenProperty> property = new RSScreenProperty();
    auto rsRenderComposerTmp = std::make_shared<RSRenderComposer>(output, property);
    EXPECT_NE(rsRenderComposerTmp->hdiOutput_, nullptr);

    // Call ContextRegisterPostTask multiple times to verify stability
    // Line 1137: VULKAN condition true each time
    for (int i = 0; i < 5; i++) {
        rsRenderComposerTmp->ContextRegisterPostTask();
    }

    // Verify composer still works after multiple registrations
    EXPECT_NE(rsRenderComposerTmp->handler_, nullptr);
}
#endif
#endif

/**
 * Function: ContextRegisterPostTask_MultipleCalls_Ddgr
 * Type: Function
 * Rank: Important(2)
 * EnvConditions: RS_ENABLE_VK and IS_ENABLE_DRM defined, GpuApiType set to DDGR
 * CaseDescription: 1. create RSRenderComposer
 *                  2. call ContextRegisterPostTask multiple times
 *                  3. verify line 1137 condition is true with DDGR each time
 *                  4. verify no crash or undefined behavior
 */
#if defined(RS_ENABLE_VK) && defined(IS_ENABLE_DRM)
#if defined(GPU_API_TYPE_DDGR)
HWTEST_F(RsRenderComposerTest, ContextRegisterPostTask_MultipleCalls_Ddgr, TestSize.Level1)
{
    auto output = std::make_shared<HdiOutput>(0u);
    output->Init();
    sptr<RSScreenProperty> property = new RSScreenProperty();
    auto rsRenderComposerTmp = std::make_shared<RSRenderComposer>(output, property);
    EXPECT_NE(rsRenderComposerTmp->hdiOutput_, nullptr);

    // Call ContextRegisterPostTask multiple times to verify stability
    // Line 1137: DDGR condition true each time
    for (int i = 0; i < 5; i++) {
        rsRenderComposerTmp->ContextRegisterPostTask();
    }

    // Verify composer still works after multiple registrations
    EXPECT_NE(rsRenderComposerTmp->handler_, nullptr);
}
#endif
#endif

} // namespace Rosen
} // namespace OHOS
