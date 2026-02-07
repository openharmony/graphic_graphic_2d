/*
 * Copyright (c) 2021-2025 Huawei Device Co., Ltd.
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
#include <chrono>
#include <thread>
#include <vector>
#include <fcntl.h>
#include <sys/wait.h>
#include <sys/mman.h>
#include <unistd.h>
#include <gtest/gtest.h>

#include <iservice_registry.h>
#include "accesstoken_kit.h"
#include "layer/rs_surface_rcd_layer.h"
#include "rs_composer_to_render_connection.h"
#include "rs_render_to_composer_connection.h"
#include "pipeline/rs_composer_client.h"
#include "nativetoken_kit.h"
#include "rs_render_composer_manager.h"
#include "token_setproc.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Rosen {
class RSRenderComposerClientRemoteTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();

    static inline sptr<IRemoteObject> robj_ = nullptr;
    static inline std::shared_ptr<RSComposerClient> composerClient_ = nullptr;
    static inline pid_t pid_ = 0;
    static inline int pipeFd_[2] = {};
    static inline int pipe1Fd_[2] = {};
    static inline int32_t systemAbilityID_ = 345135;
};

static void InitNativeTokenInfo()
{
    uint64_t tokenId;
    const char *perms[2];
    perms[0] = "ohos.permission.DISTRIBUTED_DATASYNC";
    perms[1] = "ohos.permission.CAMERA";
    NativeTokenInfoParams infoInstance = {
        .dcapsNum = 0,
        .permsNum = 2,
        .aclsNum = 0,
        .dcaps = NULL,
        .perms = perms,
        .acls = NULL,
        .processName = "dcamera_client_demo",
        .aplStr = "system_basic",
    };
    tokenId = GetAccessTokenId(&infoInstance);
    SetSelfTokenID(tokenId);
    int32_t ret = Security::AccessToken::AccessTokenKit::ReloadNativeTokenInfo();
    ASSERT_EQ(ret, Security::AccessToken::RET_SUCCESS);
    std::this_thread::sleep_for(std::chrono::milliseconds(50));  // wait 50ms
}

void RSRenderComposerClientRemoteTest::SetUpTestCase()
{
    if (pipe(pipeFd_) < 0) {
        exit(1);
    }
    if (pipe(pipe1Fd_) < 0) {
        exit(0);
    }
    pid_ = fork();
    if (pid_ < 0) {
        exit(1);
    }
    if (pid_ == 0) {
        // render_service
#ifdef RS_ENABLE_VK
        RsVulkanContext::SetRecyclable(false);
#endif
        InitNativeTokenInfo();
        uint32_t screenId = 10;
        std::shared_ptr<HdiOutput> output = std::make_shared<HdiOutput>(screenId);
        sptr<RSScreenProperty> property = sptr<RSScreenProperty>::MakeSptr();
        property->Set<ScreenPropertyType::RENDER_RESOLUTION>(std::make_pair(2048, 1024));
        std::shared_ptr<RSRenderComposer> renderComposer = std::make_shared<RSRenderComposer>(output, property);
        auto renderComposerAgent = std::make_shared<RSRenderComposerAgent>(renderComposer);
        sptr<RSRenderToComposerConnection> renderToComposer =
            sptr<RSRenderToComposerConnection>::MakeSptr("composer_conn_test", screenId, renderComposerAgent);
        ASSERT_NE(renderToComposer, nullptr);

        auto sam = SystemAbilityManagerClient::GetInstance().GetSystemAbilityManager();
        sam->AddSystemAbility(systemAbilityID_, renderToComposer);
        close(pipeFd_[1]);
        close(pipe1Fd_[0]);
        char buf[10] = "start";
        write(pipe1Fd_[1], buf, sizeof(buf));
        sleep(0);
        read(pipeFd_[0], buf, sizeof(buf));
        sam->RemoveSystemAbility(systemAbilityID_);
        close(pipeFd_[0]);
        close(pipe1Fd_[1]);
        exit(0);
    } else {
        // render_process
#ifdef RS_ENABLE_VK
        RsVulkanContext::SetRecyclable(false);
#endif
        close(pipeFd_[0]);
        close(pipe1Fd_[1]);
        char buf[10];
        read(pipe1Fd_[0], buf, sizeof(buf));
        auto sam = SystemAbilityManagerClient::GetInstance().GetSystemAbilityManager();
        robj_ = sam->GetSystemAbility(systemAbilityID_);
        sptr<IRSRenderToComposerConnection> renderToComposer = iface_cast<IRSRenderToComposerConnection>(robj_);
        sptr<IRSComposerToRenderConnection> composerToRender = sptr<RSComposerToRenderConnection>::MakeSptr();
        composerClient_ = RSComposerClient::Create(renderToComposer, composerToRender);
    }
}

void RSRenderComposerClientRemoteTest::TearDownTestCase()
{
    composerClient_ = nullptr;
    robj_ = nullptr;

    char buf[10] = "over";
    write(pipeFd_[1], buf, sizeof(buf));
    close(pipeFd_[1]);
    close(pipe1Fd_[0]);

    int32_t ret = 0;
    do {
        waitpid(pid_, nullptr, 0);
    } while (ret == -1 && errno == EINTR);
}

/*
 * Function: CommitLayers001
 * Type: Function
 * Rank: Important(2)
 * EnvConditions: N/A
 * CaseDescription: 1. check ret for CommitLayers func
 */
HWTEST_F(RSRenderComposerClientRemoteTest, CommitLayers001, TestSize.Level0)
{
    std::shared_ptr<RSLayer> rsLayer = RSSurfaceLayer::Create(1, composerClient_->GetComposerContext());
    EXPECT_NE(rsLayer, nullptr);
    GraphicLayerAlpha alpha = {1, 0, 1, 0, 1};
    rsLayer->SetAlpha(alpha);
    rsLayer->SetZorder(1);
    rsLayer->SetIsNeedComposition(true);
    rsLayer->SetType(GRAPHIC_LAYER_TYPE_OVERLAY);
    rsLayer->SetTransform(GRAPHIC_ROTATE_90);
    rsLayer->SetCompositionType(GRAPHIC_COMPOSITION_DEVICE);
    std::vector<GraphicIRect> visibleRegions;
    GraphicIRect iRect = {1, 0, 1, 0};
    visibleRegions.push_back(iRect);
    rsLayer->SetVisibleRegions(visibleRegions);
    std::vector<GraphicIRect> dirtyRegions;
    dirtyRegions.push_back(iRect);
    rsLayer->SetDirtyRegions(dirtyRegions);
    rsLayer->SetBlendType(GRAPHIC_BLEND_CLEAR);
    rsLayer->SetCropRect(iRect);
    rsLayer->SetPreMulti(true);
    rsLayer->SetLayerSize(iRect);
    rsLayer->SetBoundSize(iRect);
    GraphicLayerColor layerColor = {1, 0, 1, 0};
    rsLayer->SetLayerColor(layerColor);
    rsLayer->SetBackgroundColor(layerColor);
    std::vector<float> drmCornerRadiusInfo;
    drmCornerRadiusInfo.push_back(1.0);
    rsLayer->SetCornerRadiusInfoForDRM(drmCornerRadiusInfo);
    std::vector<float> matrixVec;
    matrixVec.push_back(1.0);
    rsLayer->SetColorTransform(matrixVec);
    rsLayer->SetColorDataSpace(GRAPHIC_GAMUT_BT601);
    std::vector<GraphicHDRMetaData> metaData;
    metaData.push_back({GRAPHIC_MATAKEY_RED_PRIMARY_Y, 1.0});
    rsLayer->SetMetaData(metaData);
    GraphicHDRMetaDataSet metaDataSet = {GRAPHIC_MATAKEY_RED_PRIMARY_Y};
    rsLayer->SetMetaDataSet(metaDataSet);
    GraphicMatrix matrix = {1.0, 0, 1.0, 0};
    rsLayer->SetMatrix(matrix);
    rsLayer->SetGravity(1);
    rsLayer->SetUniRenderFlag(true);
    rsLayer->SetTunnelHandleChange(true);
    sptr<SurfaceTunnelHandle> handle = sptr<SurfaceTunnelHandle>::MakeSptr();
    rsLayer->SetTunnelHandle(handle);
    rsLayer->SetTunnelLayerId(1);
    rsLayer->SetTunnelLayerProperty(1);
    rsLayer->SetPresentTimestamp({GRAPHIC_DISPLAY_PTS_DELAY, 1});
    rsLayer->SetIsSupportedPresentTimestamp(true);
    rsLayer->SetSdrNit(1.0);
    rsLayer->SetDisplayNit(1.0);
    rsLayer->SetBrightnessRatio(1.0);
    std::vector<float> layerLinearMatrix;
    layerLinearMatrix.push_back(1.0);
    rsLayer->SetLayerLinearMatrix(layerLinearMatrix);
    rsLayer->SetLayerSourceTuning(1);
    std::vector<std::string> windowsName;
    windowsName.push_back("");
    rsLayer->SetWindowsName(windowsName);
    rsLayer->SetRotationFixed(true);
    rsLayer->SetLayerArsr(true);
    rsLayer->SetLayerCopybit(true);
    rsLayer->SetNeedBilinearInterpolation(true);
    rsLayer->SetIsMaskLayer(true);
    rsLayer->SetNodeId(1);
    rsLayer->SetAncoFlags(1);
    rsLayer->SetLayerMaskInfo(LayerMask::LAYER_MASK_HBM_SYNC);
    sptr<IConsumerSurface> surface = IConsumerSurface::Create("test");
    rsLayer->SetSurface(surface);
    rsLayer->SetSurfaceUniqueId(1);
    sptr<SurfaceBuffer> sbuffer;
    sptr<SyncFence> acquireFence;
    rsLayer->SetBuffer(sbuffer, acquireFence);
    rsLayer->SetBuffer(sbuffer);
    rsLayer->SetPreBuffer(sbuffer);
    rsLayer->SetAcquireFence(acquireFence);
    rsLayer->SetCycleBuffersNum(1);
    rsLayer->SetSurfaceName("test");
    GraphicSolidColorLayerProperty solidColorLayerProperty = {1};
    rsLayer->SetSolidColorLayerProperty(solidColorLayerProperty);
    rsLayer->SetUseDeviceOffline(true);
    rsLayer->SetIgnoreAlpha(true);
    rsLayer->SetAncoSrcRect(iRect);
    std::string result;
    rsLayer->Dump(result);
    rsLayer->DumpCurrentFrameLayer();
    std::shared_ptr<RSSurfaceHandler::BufferOwnerCount> bufferOwnerCount = std::make_shared<RSSurfaceHandler::BufferOwnerCount>();
    rsLayer->SetBufferOwnerCount(bufferOwnerCount, true);
    ComposerInfo composerInfo;
    composerClient_->CommitLayers(composerInfo);
}

/*
 * Function: CommitLayers002
 * Type: Function
 * Rank: Important(2)
 * EnvConditions: N/A
 * CaseDescription: 1. check ret for CommitLayers func
 */
HWTEST_F(RSRenderComposerClientRemoteTest, CommitLayers002, TestSize.Level0)
{
    std::shared_ptr<RSLayer> rsLayer = RSSurfaceRCDLayer::Create(2, composerClient_->GetComposerContext());
    EXPECT_NE(rsLayer, nullptr);
    auto rcdLayer = std::static_pointer_cast<RSSurfaceRCDLayer>(rsLayer);
    std::shared_ptr<Media::PixelMap> pixelMap = std::make_shared<Media::PixelMap>();
    rcdLayer->SetPixelMap(pixelMap);
    EXPECT_EQ(rcdLayer->GetPixelMap() == pixelMap, true);
    ComposerInfo composerInfo;
    composerClient_->CommitLayers(composerInfo);
}

/*
 * Function: GetLayerInfo001
 * Type: Function
 * Rank: Important(2)
 * EnvConditions: N/A
 * CaseDescription: 1. check ret for SetLayerInfo func
 */
HWTEST_F(RSRenderComposerClientRemoteTest, GetLayerInfo001, TestSize.Level0)
{
    std::shared_ptr<RSLayer> rsLayer = RSSurfaceLayer::Create(3, composerClient_->GetComposerContext());
    EXPECT_NE(rsLayer, nullptr);
    const GraphicLayerAlpha alpha = {1, 0, 1, 0, 1};
    rsLayer->SetAlpha(alpha);
    bool isSame = (rsLayer->GetAlpha().enGlobalAlpha == alpha.enGlobalAlpha &&
        rsLayer->GetAlpha().enPixelAlpha == alpha.enPixelAlpha &&
        rsLayer->GetAlpha().alpha0 == alpha.alpha0 && rsLayer->GetAlpha().alpha1 == alpha.alpha1 &&
        rsLayer->GetAlpha().gAlpha == alpha.gAlpha);
    EXPECT_EQ(isSame, true);
    rsLayer->SetZorder(1);
    EXPECT_EQ(rsLayer->GetZorder(), 1);
    rsLayer->SetIsNeedComposition(true);
    EXPECT_EQ(rsLayer->GetIsNeedComposition(), true);
    rsLayer->SetType(GRAPHIC_LAYER_TYPE_OVERLAY);
    EXPECT_EQ(rsLayer->GetType(), GRAPHIC_LAYER_TYPE_OVERLAY);
    rsLayer->SetTransform(GRAPHIC_ROTATE_90);
    EXPECT_EQ(rsLayer->GetTransform(), GRAPHIC_ROTATE_90);
    rsLayer->SetCompositionType(GRAPHIC_COMPOSITION_DEVICE);
    EXPECT_EQ(rsLayer->GetCompositionType(), GRAPHIC_COMPOSITION_DEVICE);
    std::vector<GraphicIRect> visibleRegions;
    GraphicIRect iRect = {1, 0, 1, 0};
    visibleRegions.push_back(iRect);
    rsLayer->SetVisibleRegions(visibleRegions);
    EXPECT_EQ(rsLayer->GetVisibleRegions() == visibleRegions, true);
    std::vector<GraphicIRect> dirtyRegions;
    dirtyRegions.push_back(iRect);
    rsLayer->SetDirtyRegions(dirtyRegions);
    EXPECT_EQ(rsLayer->GetDirtyRegions() == dirtyRegions, true);
    rsLayer->SetBlendType(GRAPHIC_BLEND_CLEAR);
    EXPECT_EQ(rsLayer->GetBlendType(), GRAPHIC_BLEND_CLEAR);
    rsLayer->SetCropRect(iRect);
    EXPECT_EQ(rsLayer->GetCropRect() == iRect, true);
    rsLayer->SetPreMulti(true);
    EXPECT_EQ(rsLayer->IsPreMulti(), true);
    rsLayer->SetLayerSize(iRect);
    EXPECT_EQ(rsLayer->GetLayerSize() == iRect, true);
    rsLayer->SetBoundSize(iRect);
    EXPECT_EQ(rsLayer->GetBoundSize() == iRect, true);
    GraphicLayerColor layerColor = {1, 0, 1, 0};
    rsLayer->SetLayerColor(layerColor);
    EXPECT_EQ(rsLayer->GetLayerColor() == layerColor, true);
    rsLayer->SetBackgroundColor(layerColor);
    EXPECT_EQ(rsLayer->GetBackgroundColor() == layerColor, true);
    std::vector<float> drmCornerRadiusInfo;
    drmCornerRadiusInfo.push_back(1.0);
    rsLayer->SetCornerRadiusInfoForDRM(drmCornerRadiusInfo);
    EXPECT_EQ(rsLayer->GetCornerRadiusInfoForDRM() == drmCornerRadiusInfo, true);
    std::vector<float> matrixVec;
    matrixVec.push_back(1.0);
    rsLayer->SetColorTransform(matrixVec);
    EXPECT_EQ(rsLayer->GetColorTransform() == matrixVec, true);
    rsLayer->SetColorDataSpace(GRAPHIC_GAMUT_BT601);
    EXPECT_EQ(rsLayer->GetColorDataSpace() == GRAPHIC_GAMUT_BT601, true);
    std::vector<GraphicHDRMetaData> metaData;
    metaData.push_back({GRAPHIC_MATAKEY_RED_PRIMARY_Y, 1.0});
    rsLayer->SetMetaData(metaData);
    EXPECT_EQ(rsLayer->GetMetaData() == metaData, true);
    GraphicHDRMetaDataSet metaDataSet = {GRAPHIC_MATAKEY_RED_PRIMARY_Y};
    rsLayer->SetMetaDataSet(metaDataSet);
    EXPECT_EQ(rsLayer->GetMetaDataSet() == metaDataSet, true);
    GraphicMatrix matrix = {1.0, 0, 1.0, 0};
    rsLayer->SetMatrix(matrix);
    EXPECT_EQ(rsLayer->GetMatrix() == matrix, true);
    rsLayer->SetGravity(1);
    EXPECT_EQ(rsLayer->GetGravity(), 1);
    rsLayer->SetUniRenderFlag(true);
    EXPECT_EQ(rsLayer->GetUniRenderFlag(), true);
    rsLayer->SetTunnelHandleChange(true);
    EXPECT_EQ(rsLayer->GetTunnelHandleChange(), true);
    sptr<SurfaceTunnelHandle> handle = sptr<SurfaceTunnelHandle>::MakeSptr();
    rsLayer->SetTunnelHandle(handle);
    EXPECT_EQ(rsLayer->GetTunnelHandle() == handle, true);
    rsLayer->SetTunnelLayerId(1);
    EXPECT_EQ(rsLayer->GetTunnelLayerId(), 1);
    rsLayer->SetTunnelLayerProperty(1);
    EXPECT_EQ(rsLayer->GetTunnelLayerProperty(), 1);
    GraphicPresentTimestamp timestamp = {GRAPHIC_DISPLAY_PTS_DELAY, 1};
    rsLayer->SetPresentTimestamp(timestamp);
    EXPECT_EQ(rsLayer->GetPresentTimestamp() == timestamp, true);
    rsLayer->SetIsSupportedPresentTimestamp(true);
    EXPECT_EQ(rsLayer->GetIsSupportedPresentTimestamp(), true);
    rsLayer->SetSdrNit(1.0);
    EXPECT_EQ(rsLayer->GetSdrNit(), 1.0);
    rsLayer->SetDisplayNit(1.0);
    EXPECT_EQ(rsLayer->GetDisplayNit(), 1.0);
    rsLayer->SetBrightnessRatio(1.0);
    EXPECT_EQ(rsLayer->GetBrightnessRatio(), 1.0);
    std::vector<float> layerLinearMatrix;
    layerLinearMatrix.push_back(1.0);
    rsLayer->SetLayerLinearMatrix(layerLinearMatrix);
    EXPECT_EQ(rsLayer->GetLayerLinearMatrix() == layerLinearMatrix, true);
    rsLayer->SetLayerSourceTuning(1);
    EXPECT_EQ(rsLayer->GetLayerSourceTuning(), 1);
    std::vector<std::string> windowsName;
    windowsName.push_back("");
    rsLayer->SetWindowsName(windowsName);
    EXPECT_EQ(rsLayer->GetWindowsName() == windowsName, true);
    rsLayer->SetRotationFixed(true);
    EXPECT_EQ(rsLayer->GetRotationFixed(), true);
    rsLayer->SetLayerArsr(true);
    EXPECT_EQ(rsLayer->GetLayerArsr(), true);
    rsLayer->SetLayerCopybit(true);
    EXPECT_EQ(rsLayer->GetLayerCopybit(), true);
    rsLayer->SetNeedBilinearInterpolation(true);
    EXPECT_EQ(rsLayer->GetNeedBilinearInterpolation(), true);
    rsLayer->SetIsMaskLayer(true);
    EXPECT_EQ(rsLayer->GetIsMaskLayer(), true);
    rsLayer->SetNodeId(1);
    EXPECT_EQ(rsLayer->GetNodeId(), 1);
    rsLayer->SetAncoFlags(1);
    EXPECT_EQ(rsLayer->GetAncoFlags(), 1);
    rsLayer->SetLayerMaskInfo(LayerMask::LAYER_MASK_HBM_SYNC);
    EXPECT_EQ(rsLayer->GetLayerMaskInfo(), LayerMask::LAYER_MASK_HBM_SYNC);
    sptr<IConsumerSurface> surface = IConsumerSurface::Create("test");
    rsLayer->SetSurface(surface);
    EXPECT_EQ(rsLayer->GetSurface() == surface, true);
    rsLayer->SetSurfaceUniqueId(1);
    EXPECT_EQ(rsLayer->GetSurfaceUniqueId(), 1);
    sptr<SurfaceBuffer> sbuffer;
    sptr<SyncFence> acquireFence;
    rsLayer->SetBuffer(sbuffer, acquireFence);
    rsLayer->SetBuffer(sbuffer);
    EXPECT_EQ(rsLayer->GetBuffer() == sbuffer, true);
    rsLayer->SetPreBuffer(sbuffer);
    EXPECT_EQ(rsLayer->GetPreBuffer() == sbuffer, true);
    rsLayer->SetAcquireFence(acquireFence);
    EXPECT_EQ(rsLayer->GetAcquireFence() == acquireFence, true);
    rsLayer->SetCycleBuffersNum(1);
    EXPECT_EQ(rsLayer->GetCycleBuffersNum(), 1);
    rsLayer->SetSurfaceName("test");
    EXPECT_EQ(rsLayer->GetSurfaceName() == "test", true);
    GraphicSolidColorLayerProperty solidColorLayerProperty = {1};
    rsLayer->SetSolidColorLayerProperty(solidColorLayerProperty);
    EXPECT_EQ(rsLayer->GetSolidColorLayerProperty() == solidColorLayerProperty, true);
    rsLayer->SetUseDeviceOffline(true);
    EXPECT_EQ(rsLayer->GetUseDeviceOffline(), true);
    rsLayer->SetIgnoreAlpha(true);
    EXPECT_EQ(rsLayer->GetIgnoreAlpha(), true);
    rsLayer->SetAncoSrcRect(iRect);
    EXPECT_EQ(rsLayer->GetAncoSrcRect() == iRect, true);
    std::string result;
    rsLayer->Dump(result);
    rsLayer->DumpCurrentFrameLayer();
    std::shared_ptr<RSSurfaceHandler::BufferOwnerCount> bufferOwnerCount = std::make_shared<RSSurfaceHandler::BufferOwnerCount>();
    rsLayer->SetBufferOwnerCount(bufferOwnerCount, true);
    EXPECT_EQ(rsLayer->GetBufferOwnerCount() == bufferOwnerCount, true);
}

/*
 * Function: GetLayerInfo002
 * Type: Function
 * Rank: Important(2)
 * EnvConditions: N/A
 * CaseDescription: 1. check ret for SetLayerInfo func
 */
HWTEST_F(RSRenderComposerClientRemoteTest, GetLayerInfo002, TestSize.Level0)
{
    std::shared_ptr<RSLayer> rsLayer = RSSurfaceLayer::Create(4, composerClient_->GetComposerContext());
    EXPECT_NE(rsLayer, nullptr);
    GraphicLayerAlpha alpha = {1, 0, 1, 0, 1};
    rsLayer->SetAlpha(alpha);
    rsLayer->SetZorder(1);
    rsLayer->SetIsNeedComposition(true);
    rsLayer->SetType(GRAPHIC_LAYER_TYPE_OVERLAY);
    rsLayer->SetTransform(GRAPHIC_ROTATE_90);
    rsLayer->SetCompositionType(GRAPHIC_COMPOSITION_DEVICE);

    rsLayer->SetAlpha(alpha);
    rsLayer->SetZorder(1);
    rsLayer->SetIsNeedComposition(true);
    rsLayer->SetType(GRAPHIC_LAYER_TYPE_OVERLAY);
    rsLayer->SetTransform(GRAPHIC_ROTATE_90);
    rsLayer->SetCompositionType(GRAPHIC_COMPOSITION_DEVICE);

    std::vector<GraphicIRect> visibleRegions;
    GraphicIRect iRect = {1, 0, 1, 0};
    visibleRegions.push_back(iRect);
    rsLayer->SetVisibleRegions(visibleRegions);
    rsLayer->SetVisibleRegions(visibleRegions);
    std::vector<GraphicIRect> dirtyRegions;
    dirtyRegions.push_back(iRect);
    rsLayer->SetDirtyRegions(dirtyRegions);
    rsLayer->SetBlendType(GRAPHIC_BLEND_CLEAR);
    rsLayer->SetCropRect(iRect);
    rsLayer->SetPreMulti(true);
    rsLayer->SetLayerSize(iRect);
    rsLayer->SetBoundSize(iRect);

    rsLayer->SetDirtyRegions(dirtyRegions);
    rsLayer->SetBlendType(GRAPHIC_BLEND_CLEAR);
    rsLayer->SetCropRect(iRect);
    rsLayer->SetPreMulti(true);
    rsLayer->SetLayerSize(iRect);
    rsLayer->SetBoundSize(iRect);

    GraphicLayerColor layerColor = {1, 0, 1, 0};
    rsLayer->SetLayerColor(layerColor);
    rsLayer->SetBackgroundColor(layerColor);

    rsLayer->SetLayerColor(layerColor);
    rsLayer->SetBackgroundColor(layerColor);

    std::vector<float> drmCornerRadiusInfo;
    drmCornerRadiusInfo.push_back(1.0);
    rsLayer->SetCornerRadiusInfoForDRM(drmCornerRadiusInfo);
    rsLayer->SetCornerRadiusInfoForDRM(drmCornerRadiusInfo);
    std::vector<float> matrixVec;
    matrixVec.push_back(1.0);
    rsLayer->SetColorTransform(matrixVec);
    rsLayer->SetColorDataSpace(GRAPHIC_GAMUT_BT601);
    rsLayer->SetColorTransform(matrixVec);
    rsLayer->SetColorDataSpace(GRAPHIC_GAMUT_BT601);
    std::vector<GraphicHDRMetaData> metaData;
    metaData.push_back({GRAPHIC_MATAKEY_RED_PRIMARY_Y, 1.0});
    rsLayer->SetMetaData(metaData);
    rsLayer->SetMetaData(metaData);
    GraphicHDRMetaDataSet metaDataSet = {GRAPHIC_MATAKEY_RED_PRIMARY_Y};
    rsLayer->SetMetaDataSet(metaDataSet);
    rsLayer->SetMetaDataSet(metaDataSet);
    GraphicMatrix matrix = {1.0, 0, 1.0, 0};
    rsLayer->SetMatrix(matrix);
    rsLayer->SetGravity(1);
    rsLayer->SetUniRenderFlag(true);
    rsLayer->SetTunnelHandleChange(true);
    rsLayer->SetMatrix(matrix);
    rsLayer->SetGravity(1);
    rsLayer->SetUniRenderFlag(true);
    rsLayer->SetTunnelHandleChange(true);
    sptr<SurfaceTunnelHandle> handle = sptr<SurfaceTunnelHandle>::MakeSptr();
    rsLayer->SetTunnelHandle(handle);
    rsLayer->SetTunnelLayerId(1);
    rsLayer->SetTunnelLayerProperty(1);
    GraphicPresentTimestamp timestamp = {GRAPHIC_DISPLAY_PTS_DELAY, 1};
    rsLayer->SetPresentTimestamp(timestamp);
    rsLayer->SetIsSupportedPresentTimestamp(true);
    rsLayer->SetSdrNit(1.0);
    rsLayer->SetDisplayNit(1.0);
    rsLayer->SetBrightnessRatio(1.0);
    rsLayer->SetTunnelHandle(handle);
    rsLayer->SetTunnelLayerId(1);
    rsLayer->SetTunnelLayerProperty(1);
    rsLayer->SetPresentTimestamp(timestamp);
    rsLayer->SetIsSupportedPresentTimestamp(true);
    rsLayer->SetSdrNit(1.0);
    rsLayer->SetDisplayNit(1.0);
    rsLayer->SetBrightnessRatio(1.0);
    std::vector<float> layerLinearMatrix;
    layerLinearMatrix.push_back(1.0);
    rsLayer->SetLayerLinearMatrix(layerLinearMatrix);
    rsLayer->SetLayerSourceTuning(1);
    rsLayer->SetLayerLinearMatrix(layerLinearMatrix);
    rsLayer->SetLayerSourceTuning(1);
    std::vector<std::string> windowsName;
    windowsName.push_back("");
    rsLayer->SetWindowsName(windowsName);
    rsLayer->SetRotationFixed(true);
    rsLayer->SetLayerArsr(true);
    rsLayer->SetLayerCopybit(true);
    rsLayer->SetNeedBilinearInterpolation(true);
    rsLayer->SetIsMaskLayer(true);
    rsLayer->SetNodeId(1);
    rsLayer->SetAncoFlags(1);
    rsLayer->SetLayerMaskInfo(LayerMask::LAYER_MASK_HBM_SYNC);
    rsLayer->SetWindowsName(windowsName);
    rsLayer->SetRotationFixed(true);
    rsLayer->SetLayerArsr(true);
    rsLayer->SetLayerCopybit(true);
    rsLayer->SetNeedBilinearInterpolation(true);
    rsLayer->SetIsMaskLayer(true);
    rsLayer->SetNodeId(1);
    rsLayer->SetAncoFlags(1);
    rsLayer->SetLayerMaskInfo(LayerMask::LAYER_MASK_HBM_SYNC);
    sptr<IConsumerSurface> surface = IConsumerSurface::Create("test");
    rsLayer->SetSurface(surface);
    rsLayer->SetSurfaceUniqueId(1);
    rsLayer->SetSurface(surface);
    rsLayer->SetSurfaceUniqueId(1);
    sptr<SurfaceBuffer> sbuffer;
    sptr<SyncFence> acquireFence;
    rsLayer->SetBuffer(sbuffer, acquireFence);
    rsLayer->SetBuffer(sbuffer);
    rsLayer->SetPreBuffer(sbuffer);
    rsLayer->SetAcquireFence(acquireFence);
    rsLayer->SetCycleBuffersNum(1);
    rsLayer->SetSurfaceName("test");
    rsLayer->SetBuffer(sbuffer, acquireFence);
    rsLayer->SetBuffer(sbuffer);
    rsLayer->SetPreBuffer(sbuffer);
    rsLayer->SetAcquireFence(acquireFence);
    rsLayer->SetCycleBuffersNum(1);
    rsLayer->SetSurfaceName("test");
    GraphicSolidColorLayerProperty solidColorLayerProperty = {1};
    rsLayer->SetSolidColorLayerProperty(solidColorLayerProperty);
    rsLayer->SetUseDeviceOffline(true);
    rsLayer->SetIgnoreAlpha(true);
    rsLayer->SetAncoSrcRect(iRect);
    rsLayer->SetSolidColorLayerProperty(solidColorLayerProperty);
    rsLayer->SetUseDeviceOffline(true);
    rsLayer->SetIgnoreAlpha(true);
    rsLayer->SetAncoSrcRect(iRect);
    std::shared_ptr<RSSurfaceHandler::BufferOwnerCount> bufferOwnerCount = std::make_shared<RSSurfaceHandler::BufferOwnerCount>();
    rsLayer->SetBufferOwnerCount(bufferOwnerCount, true);
    rsLayer->SetBufferOwnerCount(bufferOwnerCount, true);
}
}
