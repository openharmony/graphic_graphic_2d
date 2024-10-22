/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
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

#include "rssystemproperties_fuzzer.h"

#include <climits>
#include <cstddef>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <fcntl.h>
#include <hilog/log.h>
#include <securec.h>
#include <unistd.h>

#include "platform/common/rs_system_properties.h"
namespace OHOS {
namespace Rosen {
namespace {
const uint8_t* g_data = nullptr;
size_t g_size = 0;
size_t g_pos;
} // namespace

template<class T>
T GetData()
{
    T object {};
    size_t objectSize = sizeof(object);
    if (g_data == nullptr || objectSize > g_size - g_pos) {
        return object;
    }
    errno_t ret = memcpy_s(&object, objectSize, g_data + g_pos, objectSize);
    if (ret != EOK) {
        return {};
    }
    g_pos += objectSize;
    return object;
}
bool DoSomethingInterestingWithMyAPI(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;
    std::string paraName = "text";
    std::string param = "text";
    std::string node = "text";
    int width = GetData<int>();
    int height = GetData<int>();
    RSSystemProperties::GetRecordingFile();
    RSSystemProperties::IsSceneBoardEnabled();
    RSSystemProperties::GetDumpFrameNum();
    RSSystemProperties::SetRecordingDisenabled();
    RSSystemProperties::GetRecordingEnabled();
    RSSystemProperties::GetProfilerEnabled();
    RSSystemProperties::GetInstantRecording();
    RSSystemProperties::SetInstantRecording(true);
    RSSystemProperties::GetSaveRDC();
    RSSystemProperties::SetSaveRDC(true);
    RSSystemProperties::GetUniRenderEnabled();
    RSSystemProperties::GetRenderNodeTraceEnabled();
    RSSystemProperties::GetDrawOpTraceEnabled();
    RSSystemProperties::GetAnimationTraceEnabled();
    RSSystemProperties::GetDirtyRegionDebugType();
    RSSystemProperties::GetPartialRenderEnabled();
    RSSystemProperties::GetUniPartialRenderEnabled();
    RSSystemProperties::GetOcclusionEnabled();
    RSSystemProperties::GetRSEventProperty(paraName);
    RSSystemProperties::GetDirectClientCompEnableStatus();
    RSSystemProperties::GetHighContrastStatus();
    RSSystemProperties::GetCorrectionMode();
    RSSystemProperties::GetDumpSurfaceType();
    RSSystemProperties::GetDumpSurfaceId();
    RSSystemProperties::GetOpaqueRegionDfxEnabled();
    RSSystemProperties::GetVisibleRegionDfxEnabled();
    RSSystemProperties::GetSurfaceRegionDfxType();
    RSSystemProperties::GetDumpLayersEnabled();
    RSSystemProperties::GetHardwareComposerEnabled();
    RSSystemProperties::GetHardwareComposerEnabledForMirrorMode();
    RSSystemProperties::GetHwcRegionDfxEnabled();
    RSSystemProperties::GetDrawMirrorCacheImageEnabled();
    RSSystemProperties::GetPixelmapDfxEnabled();
    RSSystemProperties::GetAFBCEnabled();
    RSSystemProperties::GetReleaseResourceEnabled();
    RSSystemProperties::GetRSScreenRoundCornerEnable();
    RSSystemProperties::SetDrawTextAsBitmap(true);
    RSSystemProperties::GetDrawTextAsBitmap();
    RSSystemProperties::SetCacheEnabledForRotation(true);
    RSSystemProperties::GetCacheEnabledForRotation();
    RSSystemProperties::GetPrepareParallelRenderingEnabled();
    RSSystemProperties::GetParallelRenderingEnabled();
    RSSystemProperties::GetHgmRefreshRatesEnabled();
    RSSystemProperties::SetHgmRefreshRateModesEnabled(param);
    RSSystemProperties::GetHgmRefreshRateModesEnabled();
    RSSystemProperties::GetAnimationScale();
    RSSystemProperties::GetProxyNodeDebugEnabled();
    RSSystemProperties::GetFilterCacheEnabled();
    RSSystemProperties::GetFilterCacheUpdateInterval();
    RSSystemProperties::GetFilterCacheSizeThreshold();
    RSSystemProperties::GetMaskLinearBlurEnabled();
    RSSystemProperties::GetMotionBlurEnabled();
    RSSystemProperties::GetKawaseEnabled();
    RSSystemProperties::SetForceHpsBlurDisabled(GetData<bool>());
    RSSystemProperties::GetHpsBlurEnabled();
    RSSystemProperties::GetKawaseRandomColorFactor();
    RSSystemProperties::GetRandomColorEnabled();
    RSSystemProperties::GetImageReleaseUsingPostTask();
    RSSystemProperties::GetKawaseOriginalEnabled();
    RSSystemProperties::GetBlurEnabled();
    RSSystemProperties::GetForegroundFilterEnabled();
    RSSystemProperties::GetAiInvertCoef();
    RSSystemProperties::GetSkipForAlphaZeroEnabled();
    RSSystemProperties::GetSkipGeometryNotChangeEnabled();
    RSSystemProperties::GetRenderParallelEnabled();
    RSSystemProperties::GetDrawFilterWithoutSnapshotEnabled();
    RSSystemProperties::GetBlurExtraFilterEnabled();
    RSSystemProperties::GetAnimationCacheEnabled();
    RSSystemProperties::GetBoolSystemProperty(nullptr, true);
    RSSystemProperties::GetUIFirstEnabled();
    RSSystemProperties::GetUIFirstDebugEnabled();
    RSSystemProperties::GetDebugTraceEnabled();
    RSSystemProperties::GetDebugTraceLevel();
    RSSystemProperties::FindNodeInTargetList(node);
    RSSystemProperties::IsFoldScreenFlag();
    RSSystemProperties::GetCacheCmdEnabled();
    RSSystemProperties::GetASTCEnabled();
    RSSystemProperties::GetCachedBlurPartialRenderEnabled();
    RSSystemProperties::GetImageGpuResourceCacheEnable(width, height);
    RSSystemProperties::GetSnapshotWithDMAEnabled();
    RSSystemProperties::GetDrmEnabled();
    RSSystemProperties::IsPhoneType();
    RSSystemProperties::IsPcType();
    RSSystemProperties::GetSyncTransactionEnabled();
    RSSystemProperties::GetSyncTransactionWaitDelay();
    RSSystemProperties::GetSingleFrameComposerEnabled();
    RSSystemProperties::GetSingleFrameComposerCanvasNodeEnabled();
    RSSystemProperties::GetSubSurfaceEnabled();
    RSSystemProperties::GetSecurityPermissionCheckEnabled();
    RSSystemProperties::GetParallelUploadTexture();
    RSSystemProperties::GetEffectMergeEnabled();
    RSSystemProperties::GetSubTreePrepareCheckType();
    RSSystemProperties::GetHDRImageEnable();
    RSSystemProperties::IsForceClient();
    RSSystemProperties::GetUnmarshParallelFlag();
    RSSystemProperties::GetUnMarshParallelSize();
    RSSystemProperties::GetGpuOverDrawBufferOptimizeEnabled();
    RSSystemProperties::GetDdgrOpincType();
    RSSystemProperties::IsDdgrOpincEnable();
    RSSystemProperties::GetAutoCacheDebugEnabled();
    RSSystemProperties::GetDdgrOpincDfxType();
    RSSystemProperties::IsOpincRealDrawCacheEnable();
    RSSystemProperties::GetDumpUICaptureEnabled();
    RSSystemProperties::GetDumpUIPixelmapEnabled();
    RSSystemProperties::GetDumpImgEnabled();
    RSSystemProperties::GetVirtualScreenScaleModeDFX();
    RSSystemProperties::GetGpuApiType();
    RSSystemProperties::IsUseVulkan();
    RSSystemProperties::GetRSNodeLimit();
    RSSystemProperties::IsTabletType();
    RSSystemProperties::GetTransactionTerminateEnabled();
    RSSystemProperties::GetHwcDirtyRegionEnabled();
    RSSystemProperties::GetVersionType();
    RSSystemProperties::GetVkQueuePriorityEnable();
    RSSystemProperties::SetInstantRecording(GetData<bool>());
    RSSystemProperties::GetBetaRecordingMode();
    RSSystemProperties::SetBetaRecordingMode(GetData<uint32_t>());
    RSSystemProperties::GetClipRectThreshold();
    RSSystemProperties::GetAllSurfaceVisibleDebugEnabled();
    RSSystemProperties::GetVirtualDirtyDebugEnabled();
    RSSystemProperties::GetVirtualDirtyEnabled();
    RSSystemProperties::GetExpandScreenDirtyEnabled();
    RSSystemProperties::GetAceDebugBoundaryEnabled();
    RSSystemProperties::SetDefaultDeviceRotationOffset(GetData<uint32_t>());
    RSSystemProperties::GetSurfaceNodeWatermarkEnabled();
    RSSystemProperties::GetDynamicBrightnessEnabled();
    RSSystemProperties::GetMagnifierEnabled();
    RSSystemProperties::GetSurfaceOffscreenEnadbled();    
    return true;
}
} // namespace Rosen
} // namespace OHOS

/* Fuzzer entry point */
extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size)
{
    /* Run your code on data */
    OHOS::Rosen::DoSomethingInterestingWithMyAPI(data, size);
    return 0;
}
