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

#include <gtest/gtest.h>
#include <atomic>
#include <thread>
#include <chrono>
#include "rs_render_composer.h"
#include "hdi_output.h"
#include "rs_render_composer_context.h"
#include "rs_layer.h"
#include "rs_layer_common_def.h"
#include "rs_layer_transaction_data.h"
#ifdef RS_ENABLE_VK
#include "platform/ohos/backend/rs_vulkan_context.h"
#endif
#include "platform/common/rs_system_properties.h"
#include "syspara/parameters.h"
#include "syspara/parameter.h"
#include "param/sys_param.h"
#include "feature/round_corner_display/rs_round_corner_display_manager.h"
#include "common/rs_singleton.h"
#include "pipeline/main_thread/rs_render_service_listener.h"
#include "pipeline/rs_render_node_gc.h"
#include "screen_manager/rs_screen.h"
#include "surface_buffer_impl.h"
#include "surface_type.h"
#include "v2_1/cm_color_space.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
class RsRenderComposerTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
    static inline std::shared_ptr<RSRenderComposer> rsRenderComposer_ = nullptr;
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
    output->Init();
    rsRenderComposer_ = std::make_shared<RSRenderComposer>(output);
    if (rsRenderComposer_->runner_) {
        rsRenderComposer_->runner_->Run();
    }
}
void RsRenderComposerTest::TearDownTestCase()
{
    rsRenderComposer_->frameBufferSurfaceOhosMap_.clear();
    rsRenderComposer_->uniRenderEngine_ = nullptr;
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

    void SetAlpha(const GraphicLayerAlpha &alpha) override { alpha_ = alpha; }
    const GraphicLayerAlpha &GetAlpha() const override { return alpha_; }
    void SetZorder(int32_t zOrder) override { zOrder_ = zOrder; }
    uint32_t GetZorder() const override { return static_cast<uint32_t>(zOrder_); }
    void SetType(const GraphicLayerType layerType) override { layerType_ = layerType; }
    GraphicLayerType GetType() const override { return layerType_; }
    void SetTransform(GraphicTransformType type) override { transform_ = type; }
    GraphicTransformType GetTransformType() const override { return transform_; }
    void SetCompositionType(GraphicCompositionType type) override { compType_ = type; }
    GraphicCompositionType GetCompositionType() const override { return compType_; }
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
    void SetMatrix(GraphicMatrix matrix) override { matrix_ = matrix; }
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
    bool IsSupportedPresentTimestamp() const override { return supportedPresentTimestamp_; }
    void SetPresentTimestamp(const GraphicPresentTimestamp &timestamp) override { presentTimestamp_ = timestamp; }
    const GraphicPresentTimestamp &GetPresentTimestamp() const override { return presentTimestamp_; }
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
    bool IsMaskLayer() const override { return isMaskLayer_; }
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

    void CopyLayerInfo(const RSLayerPtr& rsLayer) override { (void)rsLayer; }
    void Dump(std::string& result) const override { (void)result; }
    void DumpCurrentFrameLayer() const override {}

private:
    RSLayerId id_ = 0;
    bool isNeedComposition_ = false;
    std::string surfaceName_;

    GraphicLayerAlpha alpha_ {};
    int32_t zOrder_ = 0;
    GraphicLayerType layerType_ = GraphicLayerType::GRAPHIC_LAYER_TYPE_BUTT;
    GraphicTransformType transform_ = GraphicTransformType::GRAPHIC_ROTATE_NONE;
    GraphicCompositionType compType_ = GraphicCompositionType::GRAPHIC_COMPOSITION_CLIENT;
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
    EXPECT_GE(rsRenderComposer_->GetThreadTid(), 0);
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
    rsRenderComposer_->OnScreenConnected(out);
    std::this_thread::sleep_for(std::chrono::milliseconds(200));
    // Verify connected state
    EXPECT_EQ(rsRenderComposer_->hdiOutput_->GetScreenId(), 3u);
    EXPECT_NE(rsRenderComposer_->rsRenderComposerContext_, nullptr);
    EXPECT_EQ(rsRenderComposer_->screenId_, 0u);

    // 2. repeat connection (should maintain connection)
    rsRenderComposer_->OnScreenConnected(out);
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
    auto tmpRsRenderComposer = std::make_shared<RSRenderComposer>(output);
    EXPECT_EQ(tmpRsRenderComposer->hdiOutput_->GetScreenId(), 1u);

    std::vector<RSLayerPtr> newRsLayers;
    tmpRsRenderComposer->hdiOutput_->OnPrepareComplete(false, newRsLayers);


    tmpRsRenderComposer->runner_ = nullptr; // Disable runner thread for test control
    tmpRsRenderComposer->handler_ = nullptr;
    RefreshRateParam param;
    uint32_t currentRate = 0;
    bool hasGameScene = false;
    int64_t delayTime = 0;
    tmpRsRenderComposer->ComposerPrepare(param, currentRate, hasGameScene, delayTime);
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

    RefreshRateParam param;
    uint32_t currentRate = 0;
    bool hasGameScene = false;
    int64_t delayTime = 0;

    // Verify composer is created successfully
    ASSERT_NE(rsRenderComposer_, nullptr);
    EXPECT_NE(rsRenderComposer_->handler_, nullptr); // Handler should be initialized

    // normal path - prepare and process
    rsRenderComposer_->ComposerPrepare(param, currentRate, hasGameScene, delayTime);
    rsRenderComposer_->ComposerProcess(param, currentRate, hasGameScene, delayTime, tx);
    std::this_thread::sleep_for(std::chrono::milliseconds(200));
    // Verify composer still in valid state after processing
    EXPECT_NE(rsRenderComposer_->handler_, nullptr);

    // early return path after disconnection
    rsRenderComposer_->OnScreenDisconnected();
    std::this_thread::sleep_for(std::chrono::milliseconds(200));
    EXPECT_EQ(rsRenderComposer_->hdiOutput_, nullptr); // Verify disconnected
    rsRenderComposer_->ComposerPrepare(param, currentRate, hasGameScene, delayTime);
    rsRenderComposer_->ComposerProcess(param, currentRate, hasGameScene, delayTime, tx);
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
    output->fbSurface_ =nullptr;
    EXPECT_EQ(rsRenderComposer_->ClearFrameBuffers(true), GSERROR_INVALID_ARGUMENTS);
    rsRenderComposer_->hdiOutput_ = hdiOutput;
}

/**
 * Function: PreAllocateProtectedBuffer_NoCrash
 * Type: Function
 * Rank: Important(2)
 * EnvConditions: N/A
 * CaseDescription: 1. create RSRenderComposer
 *                  2. call PreAllocateProtectedBuffer with nullptr
 *                  3. verify composer remains in valid state after operation
 */
HWTEST_F(RsRenderComposerTest, PreAllocateProtectedBuffer_NoCrash, TestSize.Level1)
{
    // Call with nullptr should handle gracefully
    rsRenderComposer_->PreAllocateProtectedBuffer(nullptr);

    // Verify composer is still in valid state
    EXPECT_NE(rsRenderComposer_->handler_, nullptr);
}

/**
 * Function: GetUnExecuteTaskNum_Accessible
 * Type: Function
 * Rank: Important(2)
 * EnvConditions: N/A
 * CaseDescription: 1. create RSRenderComposer
 *                  2. call GetUnExecuteTaskNum to get task count
 *                  3. verify task count is accessible and valid before/after posting task
 */
HWTEST_F(RsRenderComposerTest, GetUnExecuteTaskNum_Accessible, TestSize.Level1)
{
    uint32_t taskNum = rsRenderComposer_->GetUnExecuteTaskNum();
    EXPECT_GE(taskNum, 0u);

    rsRenderComposer_->PostTask([]() { std::this_thread::sleep_for(std::chrono::milliseconds(10)); });
    // The count might increase (task not yet executed) or stay same (task executed quickly)
    uint32_t afterPost = rsRenderComposer_->GetUnExecuteTaskNum();
    EXPECT_GE(afterPost, 0u); // Valid count
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

    RefreshRateParam param;
    uint32_t currentRate = 0;
    bool hasGameScene = false;
    int64_t delayTime = 0;

    // Verify initial state
    EXPECT_NE(rsRenderComposer_->handler_, nullptr);
    auto initialHandler = rsRenderComposer_->handler_;

    rsRenderComposer_->ComposerPrepare(param, currentRate, hasGameScene, delayTime);
    rsRenderComposer_->ComposerProcess(param, currentRate, hasGameScene, delayTime, tx);

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
    rsRenderComposer_->OnScreenVBlankIdleCallback(timestamp);
    // Verify composer is still valid after callback with normal timestamp
    EXPECT_NE(rsRenderComposer_->handler_, nullptr);

    rsRenderComposer_->OnScreenVBlankIdleCallback(0);
    // Verify composer is still valid after callback with zero timestamp
    EXPECT_NE(rsRenderComposer_->handler_, nullptr);
}

#if defined(RS_ENABLE_VK)
/**
 * Function: CheckDumpStr
 * Type: Function
 * Rank: Important(2)
 * EnvConditions: N/A
 * CaseDescription: 1. create RSRenderComposer
 *                  2. call dump func
 *                  3. verify dump string
 */
HWTEST_F(RsRenderComposerTest, CheckDumpStr, TestSize.Level1)
{
    std::string dumpStr = "";
    rsRenderComposer_->GetAccumulatedBufferCount();
    std::set<uint64_t> bufferIds;
    bufferIds.insert(1);
    rsRenderComposer_->ClearRedrawGPUCompositionCache(bufferIds);
    std::this_thread::sleep_for(std::chrono::milliseconds(200));
    dumpStr = "";
    rsRenderComposer_->refreshRateCounts_.try_emplace(0, 1);
    rsRenderComposer_->RefreshRateCounts(dumpStr);
    EXPECT_NE(dumpStr.find("Refresh Rate:0, Count:1;"), std::string::npos);
    dumpStr = "";
    rsRenderComposer_->ClearRefreshRateCounts(dumpStr);
    EXPECT_NE(dumpStr.find("The refresh rate counts info is cleared successfully!"), std::string::npos);
    dumpStr = "";
    rsRenderComposer_->ClearRefreshRateCounts(dumpStr);
    EXPECT_TRUE(dumpStr.empty());
    dumpStr = "";
    rsRenderComposer_->RefreshRateCounts(dumpStr);
    EXPECT_TRUE(dumpStr.empty());
    dumpStr = "";
    rsRenderComposer_->DumpVkImageInfo(dumpStr);
    EXPECT_NE(dumpStr.find("DumpVkImageInfo"), std::string::npos);
}
#endif
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

    // set normoal layers
    RSLayerPtr l1 = std::make_shared<FakeRSLayer>(1, false, "L1");
    sptr<IConsumerSurface> cSurface = IConsumerSurface::Create("surface1");
    l1->SetSurface(cSurface);
    layers.emplace_back(l1);

    // set layer with max zorder
    RSLayerPtr l2 = std::make_shared<FakeRSLayer>(2, true, "L2");
    sptr<IConsumerSurface> cSurface2 = IConsumerSurface::Create("surface2");
    l2->SetSurface(cSurface2);
    l2->SetZorder(maxZorder);
    EXPECT_EQ(l2->GetZorder(), maxZorder);
    layers.emplace_back(l2);

    // set layer without surface
    RSLayerPtr l3 = std::make_shared<FakeRSLayer>(3, true, "L3");
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
    for (size_t length = 0; length < maxTotalSurfaceNameLength; length += 10) {
        maxLengthName += "LLLLLLLLLL";
    }
    maxLengthName += "_1";
    RSLayerPtr l4 = std::make_shared<FakeRSLayer>(4, true, "L4");
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
 *                  2. call ComposerProcess with transaction data where IsSuperFoldDisplay
 *                  3. verify normal path and early return path after disconnection
 */
HWTEST_F(RsRenderComposerTest, ComposerProcess_IsSuperFoldDisplay, TestSize.Level1)
{
    system::SetParameter("const.window.foldscreen.type", "6,0,0,0");
    auto tx = std::make_shared<RSLayerTransactionData>();
    ASSERT_NE(tx, nullptr);

    RefreshRateParam param;
    uint32_t currentRate = 0;
    bool hasGameScene = false;
    int64_t delayTime = 0;

    // Verify composer is created successfully
    ASSERT_NE(rsRenderComposer_, nullptr);
    EXPECT_NE(rsRenderComposer_->handler_, nullptr); // Handler should be initialized

    // normal path - prepare and process
    rsRenderComposer_->ComposerPrepare(param, currentRate, hasGameScene, delayTime);
    rsRenderComposer_->ComposerProcess(param, currentRate, hasGameScene, delayTime, tx);
    std::this_thread::sleep_for(std::chrono::milliseconds(200));
    // Verify composer still in valid state after processing
    EXPECT_NE(rsRenderComposer_->handler_, nullptr);

    // early return path after disconnection
    rsRenderComposer_->OnScreenDisconnected();
    std::this_thread::sleep_for(std::chrono::milliseconds(200));
    EXPECT_EQ(rsRenderComposer_->hdiOutput_, nullptr); // Verify disconnected
    rsRenderComposer_->ComposerPrepare(param, currentRate, hasGameScene, delayTime);
    rsRenderComposer_->ComposerProcess(param, currentRate, hasGameScene, delayTime, tx);
    std::this_thread::sleep_for(std::chrono::milliseconds(200));
    // Verify composer maintains valid state even after disconnection
    EXPECT_NE(rsRenderComposer_->handler_, nullptr);
    system::SetParameter("const.window.foldscreen.type", "0,0,0,0");
}


/**
 * Function: IsDropDirtyFrame_IsSuperFoldDisplay
 * Type: Function
 * Rank: Important(2)
 * EnvConditions: N/A
 * CaseDescription: 1. create RSRenderComposer
 *                  2. call IsDropDirtyFrame when IsSuperFoldDisplay
 *                  3. verify result
 */
HWTEST_F(RsRenderComposerTest, IsDropDirtyFrame_IsSuperFoldDisplay, TestSize.Level1)
{
    system::SetParameter("const.window.foldscreen.type", "6,0,0,0");
    std::vector<RSLayerPtr> layers;
    EXPECT_FALSE(rsRenderComposer_->IsDropDirtyFrame(layers));
    sptr<IConsumerSurface> cSurface = IConsumerSurface::Create("surface1");
    RSLayerPtr l1 = std::make_shared<FakeRSLayer>(1, false, "L1");
    l1->SetSurface(cSurface);
    layers.emplace_back(l1);
    RSLayerPtr l2 = std::make_shared<FakeRSLayer>(2, true, "L2");
    sptr<IConsumerSurface> cSurface2 = IConsumerSurface::Create("surface2");
    l2->SetSurface(cSurface2);
    layers.emplace_back(l2);
    layers.emplace_back(nullptr);
    EXPECT_FALSE(rsRenderComposer_->IsDropDirtyFrame(layers));

    auto screenManager = CreateOrGetScreenManager();
    ASSERT_NE(screenManager, nullptr);
    ScreenId screenId = rsRenderComposer_ ->screenId_;
    auto hdiOutput = HdiOutput::CreateHdiOutput(screenId);
    auto rsScreen = std::make_shared<impl::RSScreen>(screenId, true, hdiOutput, nullptr);

    auto tmpActiveRect = rsScreen->activeRect_;
    auto tmpReviseRect = rsScreen->reviseRect_;
    auto tmpMaskRect = rsScreen->maskRect_;

    rsScreen->activeRect_ = {0, 1008, 2232, 128};
    rsScreen->reviseRect_ = {0, 1008, 2232, 128};
    rsScreen->maskRect_ = {0, 1008, 2232, 128};
    screenManager->MockHdiScreenConnected(rsScreen);
    auto screenInfo = screenManager->QueryScreenInfo(screenId);
    const RectI& reviseRect = screenInfo.reviseRect;
    EXPECT_GT(reviseRect.GetWidth(), 0);
    EXPECT_GT(reviseRect.GetHeight(), 0);

    RSLayerPtr l3 = std::make_shared<FakeRSLayer>(3, false, "L3");
    l3->SetUniRenderFlag(false);
    layers.emplace_back(l3);
    EXPECT_FALSE(rsRenderComposer_->IsDropDirtyFrame(layers));

    RSLayerPtr l4 = std::make_shared<FakeRSLayer>(4, false, "L4");
    l4->SetUniRenderFlag(true);
    GraphicIRect layerRect = {0, 0, 0, 0};
    l4->SetLayerSize(layerRect);
    layers.emplace_back(l4);

    std::vector<RSLayerPtr> tmpLayers;
    EXPECT_FALSE(rsRenderComposer_->IsDropDirtyFrame(tmpLayers));

    system::SetParameter("const.window.foldscreen.type", "0,0,0,0");

    rsScreen->activeRect_ = tmpActiveRect;
    rsScreen->reviseRect_ = tmpReviseRect;
    rsScreen->maskRect_ = tmpMaskRect;
}

/**
 * Function: IsDropDirtyFrame_IsSuperFoldDisplay
 * Type: Function
 * Rank: Important(2)
 * EnvConditions: N/A
 * CaseDescription: 1. create RSRenderComposer
 *                  2. call IsDropDirtyFrame when IsSuperFoldDisplay
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
    RSLayerPtr l1 = std::make_shared<FakeRSLayer>(1, false, "L1");
    l1->SetSurface(cSurface);
    layers.push_back(l1);
    param.needFlushFramebuffer = true;
    param.layers = layers;
    rsRenderComposer_->OnPrepareComplete(pSurface, param, nullptr);
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
    ASSERT_EQ(rsRenderComposer_->exceptionCnt_, 1); // time count 1
    rsRenderComposer_->exceptionCnt_ = 0;
}

/**
 * Function: EndCheck_ShortDuration_ResetsCount
 * Type: Function
 * Rank: Important(2)
 * EnvConditions: N/A
 * CaseDescription: 1. create RSRenderComposer
 *                  2. call EndCheck when duration is short
 *                  3. verify result
 */
HWTEST_F(RsRenderComposerTest, EndCheck_ShortDuration_ResetsCount, TestSize.Level1)
{
    bool isUpload = rsRenderComposer_->exceptionCheck_.isUpload_;
    int cnt = rsRenderComposer_->exceptionCnt_;

    rsRenderComposer_->exceptionCheck_.isUpload_ = false;
    // Set a non-zero exception count
    rsRenderComposer_->exceptionCnt_ = 5;
    // Create timer and do not sleep (duration < COMPOSER_TIMEOUT)
    RSTimer timer("TestShort", 0);
    // Call EndCheck via test wrapper
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
    rsRenderComposer_->exceptionCheck_.isUpload_ = false; // avoid side effects

    const int composerTimeoutCnt = 15; // must match rs_render_composer.cpp
    rsRenderComposer_->exceptionCnt_ = composerTimeoutCnt - 1;
    // Create timer and sleep long enough to exceed COMPOSER_TIMEOUT (800ms)
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

    rsRenderComposer_->exceptionCheck_.isUpload_ = false; // avoid side effects

    const int comoserTimeoutAbortCnt = 30; // must match rs_render_composer.cpp
    rsRenderComposer_->exceptionCnt_ = comoserTimeoutAbortCnt - 1;
    RSTimer timer("TestAbort", 0);
    std::this_thread::sleep_for(std::chrono::milliseconds(900));

    rsRenderComposer_->EndCheck(timer);
    EXPECT_EQ(rsRenderComposer_->exceptionCnt_, comoserTimeoutAbortCnt);
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
    RSLayerPtr l1 = std::make_shared<FakeRSLayer>(1, false, "L1");
    sptr<IConsumerSurface> cSurface = IConsumerSurface::Create("surface1");
    l1->SetSurface(cSurface);
    layers.emplace_back(l1);

    RSLayerPtr l2 = std::make_shared<FakeRSLayer>(2, true, "L2");
    sptr<IConsumerSurface> cSurface2 = IConsumerSurface::Create("surface2");
    l2->SetSurface(cSurface2);
    l2->SetBuffer(nullptr);
    layers.emplace_back(l2);

    layers.emplace_back(nullptr);
    ASSERT_EQ(layers.size(), 3);
    EXPECT_EQ(layers[1]->GetBuffer(), nullptr);
    EXPECT_EQ(layers[2], nullptr);

    RSLayerPtr l3 = std::make_shared<FakeRSLayer>(3, true, "L3");
    sptr<IConsumerSurface> cSurface3 = IConsumerSurface::Create("surface3");
    l3->SetSurface(cSurface3);

    RSSurfaceRenderNodeConfig config;
    config.id = 3;
    config.name = "surface3";
    auto rsSurfaceRenderNode = std::make_shared<RSSurfaceRenderNode>(config);
    sptr<IConsumerSurface> csurf = IConsumerSurface::Create(config.name);
    sptr<IBufferConsumerListener> listener = new RSRenderServiceListener(rsSurfaceRenderNode);
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
        .width = 0x100,
        .height = 0x100,
        .strideAlignment = 0x8,
        .format = GRAPHIC_PIXEL_FMT_RGBA_8888,
        .usage = BUFFER_USAGE_CPU_READ | BUFFER_USAGE_CPU_WRITE | BUFFER_USAGE_MEM_DMA,
        .timeout = 0,
    };

    EXPECT_EQ(sProducer->RequestBuffer(buffer, requestFence, requestConfig), GSERROR_OK);
    l3->SetBuffer(buffer);
    layers.emplace_back(l3);
    ASSERT_EQ(layers.size(), 4);

    rsRenderComposer_->RecordTimestamp(layers);
    ASSERT_EQ(layers.size(), 4);
}

/**
 * Function: ChangeLayersForActiveRectOutside
 * Type: Function
 * Rank: Important(2)
 * EnvConditions: N/A
 * CaseDescription: 1. create RSRenderComposer
 *                  2. call ChangeLayersForActiveRectOutside
 *                  3. verify result
 */
HWTEST_F(RsRenderComposerTest, ChangeLayersForActiveRectOutside, TestSize.Level1)
{
    std::vector<RSLayerPtr> layers;
    rsRenderComposer_->ChangeLayersForActiveRectOutside(layers, 0u);

    RSLayerPtr l1 = std::make_shared<FakeRSLayer>(1, false, "L1");
    sptr<IConsumerSurface> cSurface = IConsumerSurface::Create("surface1");
    l1->SetSurface(cSurface);
    layers.emplace_back(l1);

    RSLayerPtr l2 = std::make_shared<FakeRSLayer>(2, true, "L2");
    sptr<IConsumerSurface> cSurface2 = IConsumerSurface::Create("surface2");
    l2->SetSurface(cSurface2);
    layers.emplace_back(l2);

    ASSERT_EQ(layers.size(), 2);
    EXPECT_EQ(layers[1]->GetBuffer(), nullptr);

    rsRenderComposer_->ChangeLayersForActiveRectOutside(layers, 0u);

    auto screenManager = CreateOrGetScreenManager();
    ASSERT_NE(screenManager, nullptr);
    ScreenId screenId = 3u;
    auto hdiOutput = HdiOutput::CreateHdiOutput(screenId);
    auto rsScreen = std::make_shared<impl::RSScreen>(screenId, true, hdiOutput, nullptr);

    auto tmpReviseRect = rsScreen->reviseRect_;
    auto tmpMaskRect = rsScreen->maskRect_;

    rsScreen->reviseRect_ = {0, 1008, 2232, 128};
    rsScreen->maskRect_ = {0, 1008, 2232, 128};
    screenManager->MockHdiScreenConnected(rsScreen);
    auto screenInfo = screenManager->QueryScreenInfo(screenId);
    const RectI& reviseRect = screenInfo.reviseRect;
    EXPECT_GT(reviseRect.GetWidth(), 0);
    EXPECT_GT(reviseRect.GetHeight(), 0);
    rsRenderComposer_->ChangeLayersForActiveRectOutside(layers, screenId);
    EXPECT_EQ(layers.size(), 2);

    auto tmpParameter = system::GetParameter("debug.foldscreen.shaft.color", "0");
    if (tmpParameter != "1") {
        system::SetParameter("debug.foldscreen.shaft.color", "1");
        rsRenderComposer_->ChangeLayersForActiveRectOutside(layers, screenId);
    } else {
        system::SetParameter("debug.foldscreen.shaft.color", "0");
        rsRenderComposer_->ChangeLayersForActiveRectOutside(layers, screenId);
    }

    system::SetParameter("debug.foldscreen.shaft.color", tmpParameter);
    rsScreen->reviseRect_ = tmpReviseRect;
    rsScreen->maskRect_ = tmpMaskRect;
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
    RefreshRateParam param = {
        .rate = 0,
        .frameTimestamp = 0,
        .vsyncId = 0,
        .constraintRelativeTime = 0,
        .isForceRefresh = true,
        .fastComposeTimeStampDiff = 0
    };
    EXPECT_FALSE(rsRenderComposer_->IsDelayRequired(hgmCore, param, false));

    param.isForceRefresh = false;
    bool hgmCoreIsLtpoMode = hgmCore.isLtpoMode_.load();

    hgmCore.isLtpoMode_.store(false);
    EXPECT_TRUE(rsRenderComposer_->IsDelayRequired(hgmCore, param, false));

    hgmCore.isLtpoMode_.store(true);
    auto frameRateMgr = hgmCore.GetFrameRateMgr();
    ASSERT_NE(frameRateMgr, nullptr);

    bool frameRateMgrIsAdaptive = frameRateMgr->isAdaptive_.load();
    frameRateMgr->isAdaptive_.store(SupportASStatus::SUPPORT_AS);
    EXPECT_FALSE(rsRenderComposer_->IsDelayRequired(hgmCore, param, false));

    frameRateMgr->isAdaptive_.store(SupportASStatus::GAME_SCENE_SKIP);
    EXPECT_FALSE(rsRenderComposer_->IsDelayRequired(hgmCore, param, true));

    hgmCore.isLtpoMode_.store(false);
    bool hgmCoreIsDelayMode = hgmCore.isDelayMode_;
    hgmCore.isDelayMode_ = false;
    EXPECT_FALSE(rsRenderComposer_->IsDelayRequired(hgmCore, param, true));

    hgmCore.isDelayMode_ = true;
    EXPECT_FALSE(rsRenderComposer_->IsDelayRequired(hgmCore, param, true));

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
 * Function: Redraw
 * Type: Function
 * Rank: Important(2)
 * EnvConditions: N/A
 * CaseDescription: 1. create RSRenderComposer
 *                  2. call Redraw
 *                  3. verify result
 */
HWTEST_F(RsRenderComposerTest, Redraw, TestSize.Level1)
{
    auto csurface = IConsumerSurface::Create();
    ASSERT_NE(csurface, nullptr);
    auto producer = csurface->GetProducer();
    ASSERT_NE(producer, nullptr);
    auto psurface = Surface::CreateSurfaceAsProducer(producer);
    ASSERT_NE(psurface, nullptr);

    std::vector<RSLayerPtr> layers;
    RSLayerPtr layer = std::make_shared<FakeRSLayer>(1, false, "L1");
    layers.emplace_back(layer);
    EXPECT_NE(layers.size(), 0u);

    rsRenderComposer_->Redraw(psurface, layers);
    rsRenderComposer_->Redraw(nullptr, layers);
}

#ifdef USE_VIDEO_PROCESSING_ENGINE
/**
 * Function: ComputeTargetColorGamut001
 * Type: Function
 * Rank: Important(2)
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
    sptr<IBufferConsumerListener> listener = new RSRenderServiceListener(rsSurfaceRenderNode);
    cSurface->RegisterConsumerListener(listener);
    rsSurfaceRenderNode->InitRenderParams();
    std::weak_ptr<RSSurfaceRenderNode> surfaceRenderNode(rsSurfaceRenderNode);

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
}

/**
 * Function: ComputeTargetColorGamut002
 * Type: Function
 * Rank: Important(2)
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
    sptr<IBufferConsumerListener> listener = new RSRenderServiceListener(rsSurfaceRenderNode);
    cSurface->RegisterConsumerListener(listener);
    rsSurfaceRenderNode->InitRenderParams();
    std::weak_ptr<RSSurfaceRenderNode> surfaceRenderNode(rsSurfaceRenderNode);

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
    auto reSet = MetadataHelper::SetColorSpaceInfo(buffer, infoSet);
    EXPECT_EQ(reSet, GSERROR_OK);
    GraphicColorGamut colorGamut = rsRenderComposer_->ComputeTargetColorGamut(buffer);
    EXPECT_EQ(colorGamut, GRAPHIC_COLOR_GAMUT_SRGB);
}

/**
 * Function: ComputeTargetPixelFormat001
 * Type: Function
 * Rank: Important(2)
 * EnvConditions: N/A
 * CaseDescription: 1. create RSRenderComposer
 *                  2. call ComputeTargetPixelFormat
 *                  3. verify result
 */
HWTEST_F(RsRenderComposerTest, ComputeTargetPixelFormat001, TestSize.Level1)
{
    NodeId id = 1;
    RSSurfaceRenderNodeConfig config;
    config.id = id;
    config.name = std::to_string(id);
    auto rsSurfaceRenderNode = std::make_shared<RSSurfaceRenderNode>(config);
    ASSERT_NE(rsSurfaceRenderNode, nullptr);
    sptr<IConsumerSurface> cSurface = IConsumerSurface::Create(config.name);
    rsSurfaceRenderNode->GetRSSurfaceHandler()->SetConsumer(cSurface);
    sptr<IBufferConsumerListener> listener = new RSRenderServiceListener(rsSurfaceRenderNode);
    cSurface->RegisterConsumerListener(listener);
    rsSurfaceRenderNode->InitRenderParams();
    std::weak_ptr<RSSurfaceRenderNode> surfaceRenderNode(rsSurfaceRenderNode);

    const auto& surfaceConsumer = rsSurfaceRenderNode->GetRSSurfaceHandler()->GetConsumer();
    auto producer = surfaceConsumer->GetProducer();
    sptr<Surface> sProducer = Surface::CreateSurfaceAsProducer(producer);
    sProducer->SetQueueSize(3);
    sptr<SurfaceBuffer> buffer;
    sptr<SyncFence> requestFence = SyncFence::INVALID_FENCE;
    BufferRequestConfig requestConfig = {
        .width = 0x100,
        .height = 0x100,
        .strideAlignment = 0x8,
        .format = GRAPHIC_PIXEL_FMT_RGBA_1010102,
        .usage = BUFFER_USAGE_CPU_READ | BUFFER_USAGE_CPU_WRITE | BUFFER_USAGE_MEM_DMA,
        .timeout = 0,
    };
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
}

/**
 * Function: ComputeTargetColorGamut003
 * Type: Function
 * Rank: Important(2)
 * EnvConditions: N/A
 * CaseDescription: 1. create RSRenderComposer
 *                  2. call ComputeTargetColorGamut
 *                  3. verify result
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
    RSLayerPtr l1 = std::make_shared<FakeRSLayer>(1, false, "L1");
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
    sptr<IBufferConsumerListener> listener = new RSRenderServiceListener(rsSurfaceRenderNode);
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

    RSLayerPtr l2 = std::make_shared<FakeRSLayer>(2, false, "L2");
    l2->SetBuffer(buffer1);
    layers.emplace_back(l2);
    EXPECT_EQ(layers.size(), 3);
    sptr<SurfaceBuffer> buffer2;
    requestConfig.colorGamut = GRAPHIC_COLOR_GAMUT_DISPLAY_P3;
    ret = sProducer->RequestBuffer(buffer2, requestFence, requestConfig);
    EXPECT_EQ(ret, GSERROR_OK);

    RSLayerPtr l3 = std::make_shared<FakeRSLayer>(3, false, "L3");
    l3->SetBuffer(buffer2);
    layers.emplace_back(l3);
    EXPECT_EQ(layers.size(), 4);

    auto colorGamut = rsRenderComposer_->ComputeTargetColorGamut(layers);
    EXPECT_EQ(colorGamut, GRAPHIC_COLOR_GAMUT_SRGB);
}

/**
 * Function: ComputeTargetColorGamut004
 * Type: Function
 * Rank: Important(2)
 * EnvConditions: N/A
 * CaseDescription: 1. create RSRenderComposer
 *                  2. call ComputeTargetColorGamut
 *                  3. verify result
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
    sptr<IBufferConsumerListener> listener = new RSRenderServiceListener(rsSurfaceRenderNode);
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
    auto reSet = MetaDataHelper::SetColorSpaceInfo(buffer, infoSet);
    EXPECT_EQ(reSet, GSERROR_OK);

    std::vector<RSLayerPtr> layers;
    RSLayerPtr l1 = std::make_shared<FakeRSLayer>(3, false, "L1");
    l1->SetBuffer(nullptr);
    layers.emplace_back(l1);
    auto colorGamut = rsRenderComposer_->ComputeTargetColorGamut(layers);
    EXPECT_EQ(colorGamut, GRAPHIC_COLOR_GAMUT_SRGB);
}

/**
 * Function: ComputeTargetPixelFormat_Layers
 * Type: Function
 * Rank: Important(2)
 * EnvConditions: N/A
 * CaseDescription: 1. create RSRenderComposer
 *                  2. call ComputeTargetPixelFormat with layers
 *                  3. verify result
 */
HWTEST_F(RsRenderComposerTest, ComputeTargetPixelFormat_Layers, TestSize.Level1)
{
    std::vector<RSLayerPtr> layers;
    RSLayerPtr l1 = std::make_shared<FakeRSLayer>(1, false, "L1");
    l1->SetBuffer(nullptr);
    layers.emplace_back(l1);

    auto pixelFormat = rsRenderComposer_->ComputeTargetPixelFormat(layers);
    EXPECT_EQ(pixelFormat, PIXEL_FMT_RGBA_8888);

    layers.emplace_back(nullptr);
    pixelFormat = rsRenderComposer_->ComputeTargetPixelFormat(layers);
    EXPECT_EQ(pixelFormat, PIXEL_FMT_RGBA_8888);

    std::vector<GraphicCompositionType> skipTypes = {
        GraphicCompositionType::GRAPHIC_COMPOSITION_DEVICE,
        GraphicCompositionType::GRAPHIC_COMPOSITION_DEVICE_CLEAR,
        GraphicCompositionType::GRAPHIC_COMPOSITION_SOLID_COLOR,
    };
    std::vector<std::string> layerNames = {"S1", "S2", "S3"};
    for (size_t i = 0; i < skipTypes.size(); ++i) {
        RSLayerPtr layer = std::make_shared<FakeRSLayer>(static_cast<NodeId>(i + 2), false, layerNames[i]);
        layer->SetCompositionType(skipTypes[i]);
        layers.emplace_back(layer);
    }
    pixelFormat = rsRenderComposer_->ComputeTargetPixelFormat(layers);
    EXPECT_EQ(pixelFormat, PIXEL_FMT_RGBA_8888);

    std::vector<RoundCornerDisplayManager::RCDLayerType> rcLayerTypes = {
        RoundCornerDisplayManager::RCDLayerType::INVALID,
        RoundCornerDisplayManager::RCDLayerType::TOP,
        RoundCornerDisplayManager::RCDLayerType::BOTTOM,
    };

    using RSRcdManager = RSSingleton<RoundCornerDisplayManager>;
    std::vector<std::string> rcLayerNames = {"RC1", "RC2", "RC3"};
    for (size_t i = 0; i < rcLayerTypes.size(); ++i) {
        RSLayerPtr layer = std::make_shared<FakeRSLayer>(static_cast<NodeId>(i + 10), false, rcLayerNames[i]);
        sptr<IConsumerSurface> cSurface = IConsumerSurface::Create("surface_" + rcLayerNames[i]);
        layer->SetSurface(cSurface);
        RSRcdManager::GetInstance().AddLayer(rcLayerNames[i], static_cast<NodeId>(i + 10), rcLayerTypes[i]);
        layers.emplace_back(layer);
    }
    pixelFormat = rsRenderComposer_->ComputeTargetPixelFormat(layers);
    EXPECT_EQ(pixelFormat, GRAPHIC_PIXEL_FMT_RGBA_8888);
    EXPECT_NE(layers.size(), 0u);
}

/**
 * Function: ComputeTargetPixelFormat_Layers_HasBuffer
 * Type: Function
 * Rank: Important(2)
 * EnvConditions: N/A
 * CaseDescription: 1. create RSRenderComposer
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
        RSLayerPtr layer = std::make_shared<FakeRSLayer>(static_cast<NodeId>(i + 20), false, layerNames[i]);
        RSSurfaceRenderNodeConfig config;
        config.id = static_cast<NodeId>(i + 20);
        config.name = "surface_" + layerNames[i];

        auto rsSurfaceRenderNode = std::make_shared<RSSurfaceRenderNode>(config);
        ASSERT_NE(rsSurfaceRenderNode, nullptr);
        sptr<IConsumerSurface> cSurface = IConsumerSurface::Create(config.name);
        auto surfaceBufferImpl = new SurfaceBufferImpl();
        ASSERT_NE(surfaceBufferImpl, nullptr);
        auto handle = new BufferHandle();
        ASSERT_NE(handle, nullptr);
        handle->format = formats[i];
        surfaceBufferImpl->SetBufferHandle(handle);
        ASSERT_NE(surfaceBufferImpl->GetBufferHandle(), nullptr);

        sptr<SyncFence> requestFence = SyncFence::INVALID_FENCE;
        layer->SetBuffer(surfaceBufferImpl);
        layer->SetSurface(cSurface);
        layers.emplace_back(layer);
    }
    auto pixelFormat = rsRenderComposer_->ComputeTargetPixelFormat(layers);
    EXPECT_EQ(pixelFormat, GRAPHIC_PIXEL_FMT_RGBA_1010102);
    EXPECT_NE(layers.size(), 0u);
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

    RSLayerPtr l1 = std::make_shared<FakeRSLayer>(777u, false, "777");
    l1->SetType(GraphicLayerType::GRAPHIC_LAYER_TYPE_CURSOR);
    sptr<IConsumerSurface> cSurface = IConsumerSurface::Create("777");
    l1->SetSurface(cSurface);
    using RSRcdManager = RSSingleton<RoundCornerDisplayManager>;
    RSRcdManager::GetInstance().AddLayer("777", 777u, RoundCornerDisplayManager::RCDLayerType::INVALID);
    layers.emplace_back(l1);

    RSLayerPtr l2 = std::make_shared<FakeRSLayer>(666u, false, "666");
    l2->SetType(GraphicLayerType::GRAPHIC_LAYER_TYPE_CURSOR);
    sptr<IConsumerSurface> cSurface2 = IConsumerSurface::Create("666");
    l2->SetSurface(cSurface2);
    RSRcdManager::GetInstance().AddLayer("666", 666u, RoundCornerDisplayManager::RCDLayerType::TOP);
    layers.emplace_back(l2);

    EXPECT_NE(layers.size(), 0u);
    EXPECT_TRUE(rsRenderComposer_->IsAllRedraw(layers));
}

/**
 * Function: ConvertColorGamutToSpaceType
 * Type: Function
 * Rank: Important(2)
 * EnvConditions: N/A
 * CaseDescription: 1. create RSRenderComposer
 *                  2. call ConvertColorGramutToSpaceType
 *                  3. verify result
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
        bool ret = rsRenderComposer_->ConvertColorGamutToSpaceType(colorGamut, colorSpace);
        EXPECT_TRUE(ret);
    }
    GraphicColorGamut colorGamut = GRAPHIC_COLOR_GAMUT_INVALID;
    bool ret = rsRenderComposer_->ConvertColorGamutToSpaceType(colorGamut, colorSpace);
    EXPECT_FALSE(ret);
}
#endif

/**
 * Function: Call_Interface_With_Null_Hdioutput
 * Type: Function
 * Rank: Important(2)
 * EnvConditions: N/A
 * CaseDescription: 1. create RSRenderComposer with null HdiOutput
 *                  2. call interfaces
 *                  3. verify no crash
 */
HWTEST_F(RsRenderComposerTest, Call_Interface_With_Null_Hdioutput, TestSize.Level1)
{
    auto hdiOutPut = rsRenderComposer_->hdiOutput_;
    rsRenderComposer_->hdiOutput_ = nullptr;
    rsRenderComposer_->CleanLayerBufferBySurfaceId(0u);

    std::string dumpString = "";
    std::string layerName = "test";
    rsRenderComposer_->FpsDump(dumpString, layerName);
    EXPECT_TRUE(dumpString.empty());
    EXPECT_EQ(layerName, "test");

    rsRenderComposer_->ClearFpsDump(dumpString, layerName);
    EXPECT_TRUE(dumpString.empty());
    EXPECT_EQ(layerName, "test");

    rsRenderComposer_->DumpCurrentFrameLayers();
    rsRenderComposer_->HitchsDump(dumpString, layerName);
    EXPECT_TRUE(dumpString.empty());
    EXPECT_EQ(layerName, "test");

    std::this_thread::sleep_for(std::chrono::milliseconds(200));
    rsRenderComposer_->hdiOutput_ = hdiOutPut;
}

/**
 * Function: Call_Interface_With_Null_Handler
 * Type: Function
 * Rank: Important(2)
 * EnvConditions: N/A
 * CaseDescription: 1. create RSRenderComposer with null Handler
 *                  2. call interfaces
 *                  3. verify no crash
 */
HWTEST_F(RsRenderComposerTest, Call_Interface_With_Null_Handler, TestSize.Level1)
{
    auto output = std::make_shared<HdiOutput>(1u);
    output->Init();
    auto tmpRsRenderComposer = std::make_shared<RSRenderComposer>(output);
    EXPECT_EQ(tmpRsRenderComposer->hdiOutput_->GetScreenId(), 1u);
    tmpRsRenderComposer->runner_ = nullptr;
    tmpRsRenderComposer->handler_ = nullptr;
    EXPECT_EQ(tmpRsRenderComposer->handler_, nullptr);

    std::string dumpString = "";
    std::string layerName = "test";
    rsRenderComposer_->FpsDump(dumpString, layerName);
    EXPECT_TRUE(dumpString.empty());
    EXPECT_EQ(layerName, "test");

    rsRenderComposer_->ClearFpsDump(dumpString, layerName);
    EXPECT_TRUE(dumpString.empty());
    EXPECT_EQ(layerName, "test");

    rsRenderComposer_->DumpCurrentFrameLayers();
    rsRenderComposer_->HitchsDump(dumpString, layerName);
    EXPECT_TRUE(dumpString.empty());
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
    RSLayerPtr layer = std::make_shared<FakeRSLayer>(1, false, "L1");
    layer->SetSurface(nullptr);
    layers.emplace_back(layer);

    layers.emplace_back(nullptr);

    std::vector<GraphicCompositionType> skipTypes = {
        GraphicCompositionType::GRAPHIC_COMPOSITION_DEVICE,
        GraphicCompositionType::GRAPHIC_COMPOSITION_DEVICE_CLEAR,
        GraphicCompositionType::GRAPHIC_COMPOSITION_SOLID_COLOR,
    };
    std::vector<std::string> layerNames = {"S1", "S2", "S3"};
    for (size_t i = 0; i < skipTypes.size(); ++i) {
        RSLayerPtr layer = std::make_shared<FakeRSLayer>(static_cast<NodeId>(i + 2), false, layerNames[i]);
        layer->SetCompositionType(skipTypes[i]);
        layers.emplace_back(layer);
    }

    std::vector<RoundCornerDisplayManager::RCDLayerType> rcLayerTypes = {
        RoundCornerDisplayManager::RCDLayerType::INVALID,
        RoundCornerDisplayManager::RCDLayerType::TOP,
        RoundCornerDisplayManager::RCDLayerType::BOTTOM,
    };

    using RSRcdManager = RSSingleton<RoundCornerDisplayManager>;
    std::vector<std::string> rcLayerNames = {"RC1", "RC2", "RC3"};
    for (size_t i = 0; i < rcLayerTypes.size(); ++i) {
        RSLayerPtr layer = std::make_shared<FakeRSLayer>(static_cast<NodeId>(i + 10), false, rcLayerNames[i]);
        sptr<IConsumerSurface> cSurface = IConsumerSurface::Create("surface_" + rcLayerNames[i]);
        layer->SetSurface(cSurface);
        RSRcdManager::GetInstance().AddLayer(rcLayerNames[i], static_cast<NodeId>(i + 10), rcLayerTypes[i]);
        layers.emplace_back(layer);
    }
    rsRenderComposer_->RedrawScreenRCD(paintFilterCanvas, layers);
    EXPECT_NE(layers.size(), 0u);
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
    auto tmpRsRenderComposer = std::make_shared<RSRenderComposer>(output);
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
 *                  2. call post task with null handler
 *                  3. check result
 */
HWTEST_F(RsRenderComposerTest, DumpVkImageInfo_Null_UniRenderEngine, TestSize.Level1)
{
    auto output = std::make_shared<HdiOutput>(1u);
    output->Init();
    auto tmpRsRenderComposer = std::make_shared<RSRenderComposer>(output);
    EXPECT_EQ(tmpRsRenderComposer->hdiOutput_->GetScreenId(), 1u);
    EXPECT_NE(tmpRsRenderComposer->handler_, nullptr);
    tmpRsRenderComposer->uniRenderEngine_ = nullptr;

    std::string dumpString = "";
    tmpRsRenderComposer->DumpVkImageInfo(dumpString);
    EXPECT_TRUE(dumpString.empty());
}

/**
 * Function: ClearRedrawGPUCompositionCache_Null_UniRenderEngine
 * Type: Function
 * Rank: Important(2)
 * EnvConditions: N/A
 * CaseDescription: 1. create RSRenderComposer
 *                  2. call ClearRedrawGPUCompositionCache with null handler
 *                  3. check result
 */
HWTEST_F(RsRenderComposerTest, ClearRedrawGPUCompositionCache_Null_UniRenderEngine, TestSize.Level1)
{
    auto output = std::make_shared<HdiOutput>(1u);
    output->Init();
    auto tmpRsRenderComposer = std::make_shared<RSRenderComposer>(output);
    EXPECT_EQ(tmpRsRenderComposer->hdiOutput_->GetScreenId(), 1u);
    EXPECT_NE(tmpRsRenderComposer->handler_, nullptr);
    tmpRsRenderComposer->uniRenderEngine_ = nullptr;

    std::set<uint64_t> bufferIds;
    bufferIds.insert(1u);
    tmpRsRenderComposer->ClearRedrawGPUCompositionCache(bufferIds);
    EXPECT_EQ(bufferIds.size(), 1u);
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
    auto tmpRsRenderComposer = std::make_shared<RSRenderComposer>(output);
    EXPECT_EQ(tmpRsRenderComposer->hdiOutput_->GetScreenId(), 1u);
    EXPECT_NE(tmpRsRenderComposer->handler_, nullptr);

    RefreshRateParam param;
    uint32_t currentRefreshRate = 0;

    tmpRsRenderComposer->ProcessComposerFrame(param, currentRefreshRate, true);
    std::this_thread::sleep_for(std::chrono::milliseconds(200));
    tmpRsRenderComposer->unExecuteTaskNum_.store(3);
    tmpRsRenderComposer->ProcessComposerFrame(param, currentRefreshRate, true);
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
    auto tmpRsRenderComposer = std::make_shared<RSRenderComposer>(output);
    EXPECT_EQ(tmpRsRenderComposer->hdiOutput_->GetScreenId(), 1u);
    EXPECT_NE(tmpRsRenderComposer->handler_, nullptr);

    RefreshRateParam param;
    param.isForceRefresh = true;
    uint32_t currentRefreshRate = 0;

    int64_t delayTime = tmpRsRenderComposer->UpdateDelayTime(param, currentRefreshRate, true);
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
    auto tmpRsRenderComposer = std::make_shared<RSRenderComposer>(output);
    EXPECT_EQ(tmpRsRenderComposer->hdiOutput_->GetScreenId(), 1u);
    EXPECT_NE(tmpRsRenderComposer->handler_, nullptr);

    RefreshRateParam param;
    uint32_t currentRefreshRate = 0;

    tmpRsRenderComposer->lastCommitTime_ = std::numeric_limits<int64_t>::max();
    int64_t delayTime = tmpRsRenderComposer->UpdateDelayTime(param, currentRefreshRate, true);
    EXPECT_EQ(delayTime, 0);

    tmpRsRenderComposer->lastCommitTime_ = std::numeric_limits<int64_t>::min();
    delayTime = tmpRsRenderComposer->UpdateDelayTime(param, currentRefreshRate, true);
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
    auto tmpRsRenderComposer = std::make_shared<RSRenderComposer>(output);
    EXPECT_EQ(tmpRsRenderComposer->hdiOutput_->GetScreenId(), 1u);
    EXPECT_NE(tmpRsRenderComposer->handler_, nullptr);

    RefreshRateParam param;
    auto& hgmCore = HgmCore::Instance();
    bool isLtpoMode = hgmCore.isLtpoMode_.load();
    hgmCore.isLtpoMode_.store(false);
    tmpRsRenderComposer->CalculateDelayTime(hgmCore, param, 0u, -1);
    EXPECT_GT(tmpRsRenderComposer->delayTime_, -1);

    hgmCore.isLtpoMode_.store(isLtpoMode);
    tmpRsRenderComposer->uniRenderEngine_ = nullptr;
}
} // namespace Rosen
} // namespace OHOS
