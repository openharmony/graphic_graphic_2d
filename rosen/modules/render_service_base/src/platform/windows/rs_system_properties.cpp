/*
 * Copyright (c) 2021-2023 Huawei Device Co., Ltd.
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

#include "platform/common/rs_system_properties.h"

#include "transaction/rs_render_service_client.h"

namespace OHOS {
namespace Rosen {
#if (defined (ACE_ENABLE_GL) && defined (ACE_ENABLE_VK)) || (defined (RS_ENABLE_GL) && defined (RS_ENABLE_VK))
const GpuApiType RSSystemProperties::systemGpuApiType_ = GpuApiType::OPENGL;
#elif defined (ACE_ENABLE_GL) || defined (RS_ENABLE_GL)
const GpuApiType RSSystemProperties::systemGpuApiType_ = GpuApiType::OPENGL;
#else
const GpuApiType RSSystemProperties::systemGpuApiType_ = GpuApiType::VULKAN;
#endif

int RSSystemProperties::GetDumpFrameNum()
{
    return 0;
}

int RSSystemProperties::GetRecordingEnabled()
{
    return 0;
}

bool RSSystemProperties::IsSceneBoardEnabled()
{
    return false;
}

void RSSystemProperties::SetRecordingDisenabled()
{
    return;
}

std::string RSSystemProperties::GetRecordingFile()
{
    return "";
}

bool RSSystemProperties::GetUniRenderEnabled()
{
    return isUniRenderEnabled_;
}

bool RSSystemProperties::GetRenderNodeTraceEnabled()
{
    return {};
}

bool RSSystemProperties::GetAnimationTraceEnabled()
{
    return false;
}

bool RSSystemProperties::GetAnimationDelayOptimizeEnabled()
{
    return false;
}

bool RSSystemProperties::GetRSClientMultiInstanceEnabled()
{
    return false;
}

bool RSSystemProperties::GetDrawOpTraceEnabled()
{
    return false;
}

bool RSSystemProperties::GetRenderNodePurgeEnabled()
{
    return false;
}

bool RSSystemProperties::GetRSImagePurgeEnabled()
{
    return false;
}

bool RSSystemProperties::GetClosePixelMapFdEnabled()
{
    return false;
}

DirtyRegionDebugType RSSystemProperties::GetDirtyRegionDebugType()
{
    return {};
}

AdvancedDirtyRegionType RSSystemProperties::GetAdvancedDirtyRegionEnabled()
{
    return {};
}

DirtyAlignType RSSystemProperties::GetDirtyAlignEnabled()
{
    return {};
}

PartialRenderType RSSystemProperties::GetPartialRenderEnabled()
{
    return {};
}

PartialRenderType RSSystemProperties::GetUniPartialRenderEnabled()
{
    return {};
}

StencilPixelOcclusionCullingType RSSystemProperties::GetStencilPixelOcclusionCullingEnabled()
{
    return {};
}

float RSSystemProperties::GetClipRectThreshold()
{
    return 1.f;
}

bool RSSystemProperties::GetAllSurfaceVisibleDebugEnabled()
{
    return false;
}

bool RSSystemProperties::GetVirtualDirtyDebugEnabled()
{
    return {};
}

bool RSSystemProperties::GetVirtualDirtyEnabled()
{
    return {};
}

bool RSSystemProperties::GetExpandScreenDirtyEnabled()
{
    return false;
}

bool RSSystemProperties::GetVirtualExpandScreenSkipEnabled()
{
    return false;
}

bool RSSystemProperties::GetOcclusionEnabled()
{
    return {};
}

bool RSSystemProperties::GetVkQueuePriorityEnable()
{
    return false;
}

std::string RSSystemProperties::GetRSEventProperty(const std::string &paraName)
{
    return {};
}

bool RSSystemProperties::GetHighContrastStatus()
{
    return {};
}

uint32_t RSSystemProperties::GetCorrectionMode()
{
    return {};
}

DumpSurfaceType RSSystemProperties::GetDumpSurfaceType()
{
    return {};
}

long long int RSSystemProperties::GetDumpSurfaceId()
{
    return {};
}

void RSSystemProperties::SetDrawTextAsBitmap(bool flag)
{
}

bool RSSystemProperties::GetDrawTextAsBitmap()
{
    return {};
}

void RSSystemProperties::SetCacheEnabledForRotation(bool flag)
{
}

bool RSSystemProperties::GetCacheEnabledForRotation()
{
    return {};
}

ParallelRenderingType RSSystemProperties::GetPrepareParallelRenderingEnabled()
{
    return {};
}

ParallelRenderingType RSSystemProperties::GetParallelRenderingEnabled()
{
    return {};
}

HgmRefreshRates RSSystemProperties::GetHgmRefreshRatesEnabled()
{
    return {};
}

void RSSystemProperties::SetHgmRefreshRateModesEnabled(std::string param)
{
    return;
}

HgmRefreshRateModes RSSystemProperties::GetHgmRefreshRateModesEnabled()
{
    return {};
}

bool RSSystemProperties::GetHardCursorEnabled()
{
    return false;
}

bool RSSystemProperties::GetSLRScaleEnabled()
{
    return false;
}

bool RSSystemProperties::GetSkipForAlphaZeroEnabled()
{
    return {};
}

bool RSSystemProperties::GetSkipGeometryNotChangeEnabled()
{
    return {};
}

bool RSSystemProperties::GetAnimationCacheEnabled()
{
    return {};
}

float RSSystemProperties::GetAnimationScale()
{
    return 1.f;
}

bool RSSystemProperties::GetCacheOptimizeRotateEnable()
{
    return false;
}

bool RSSystemProperties::GetProxyNodeDebugEnabled()
{
    return false;
}

bool RSSystemProperties::GetFilterCacheEnabled()
{
    return false;
}

int RSSystemProperties::GetFilterCacheUpdateInterval()
{
    return 0;
}

bool RSSystemProperties::GetMaskLinearBlurEnabled()
{
    return true;
}

bool RSSystemProperties::GetMotionBlurEnabled()
{
    return true;
}

bool RSSystemProperties::GetDynamicBrightnessEnabled()
{
    return true;
}

bool RSSystemProperties::GetMagnifierEnabled()
{
    return true;
}

bool RSSystemProperties::GetDebugTraceEnabled()
{
    return false;
}

int RSSystemProperties::GetDebugTraceLevel()
{
    return 0;
}

bool RSSystemProperties::FindNodeInTargetList(std::string node)
{
    return false;
}

bool RSSystemProperties::IsFoldScreenFlag()
{
    return false;
}

bool RSSystemProperties::IsSmallFoldDevice()
{
    return false;
}

int RSSystemProperties::GetFilterCacheSizeThreshold()
{
    return 0;
}

bool RSSystemProperties::GetKawaseEnabled()
{
    return false;
}

void RSSystemProperties::SetForceHpsBlurDisabled(bool flag)
{
    forceHpsBlurDisabled_ = flag;
}

bool RSSystemProperties::GetHpsBlurEnabled()
{
    return false;
}

float RSSystemProperties::GetHpsBlurNoiseFactor()
{
    return 0.f;
}

bool RSSystemProperties::GetMESABlurFuzedEnabled()
{
    return false;
}

int RSSystemProperties::GetSimplifiedMesaEnabled()
{
    return 0;
}

bool RSSystemProperties::GetForceKawaseDisabled()
{
    return false;
}

float RSSystemProperties::GetKawaseRandomColorFactor()
{
    return 0.f;
}

bool RSSystemProperties::GetRandomColorEnabled()
{
    return false;
}

bool RSSystemProperties::GetKawaseOriginalEnabled()
{
    return false;
}

bool RSSystemProperties::GetBlurEnabled()
{
    return true;
}

bool RSSystemProperties::GetForegroundFilterEnabled()
{
    return true;
}

const std::vector<float>& RSSystemProperties::GetAiInvertCoef()
{
    // Configure AiInvertCoef: Low, High, Threshold, Opacity, Saturation, Filter Radius.
    std::vector<float> aiInvertCoef = {0.0, 1.0, 0.55, 0.4, 1.6, 45.0};
    return aiInvertCoef;
}

bool RSSystemProperties::GetBoolSystemProperty(const char* name, bool defaultValue)
{
    return {};
}

int RSSystemProperties::WatchSystemProperty(const char* name, OnSystemPropertyChanged func, void* context)
{
    return {};
}

bool RSSystemProperties::GetAFBCEnabled()
{
    return {};
}

bool RSSystemProperties::GetASTCEnabled()
{
    return false;
}

bool RSSystemProperties::GetCachedBlurPartialRenderEnabled()
{
    return false;
}

bool RSSystemProperties::GetImageGpuResourceCacheEnable(int width, int height)
{
    return false;
}

bool RSSystemProperties::IsPhoneType()
{
    return false;
}

bool RSSystemProperties::IsBetaRelease()
{
    return false;
}

bool RSSystemProperties::GetSyncTransactionEnabled()
{
    return false;
}

int RSSystemProperties::GetSyncTransactionWaitDelay()
{
    return 0;
}

bool RSSystemProperties::GetSingleFrameComposerEnabled()
{
    return false;
}

bool RSSystemProperties::GetSingleFrameComposerCanvasNodeEnabled()
{
    return false;
}

bool RSSystemProperties::GetDrawFilterWithoutSnapshotEnabled()
{
    return false;
}

bool RSSystemProperties::GetBlurExtraFilterEnabled()
{
    return false;
}

bool RSSystemProperties::GetDiscardCanvasBeforeFilterEnabled()
{
    return false;
}

bool RSSystemProperties::GetSecurityPermissionCheckEnabled()
{
    return false;
}

bool RSSystemProperties::GetEffectMergeEnabled()
{
    return true;
}

bool RSSystemProperties::GetDumpUICaptureEnabled()
{
    return false;
}

bool RSSystemProperties::GetDumpUIPixelmapEnabled()
{
    return false;
}

bool RSSystemProperties::GetTransactionTerminateEnabled()
{
    return false;
}

int RSSystemProperties::GetVirtualScreenScaleModeDFX()
{
    return {};
}

bool RSSystemProperties::GetHwcRegionDfxEnabled()
{
    return false;
}

bool RSSystemProperties::GetDrawMirrorCacheImageEnabled()
{
    return false;
}

bool RSSystemProperties::GetPixelmapDfxEnabled()
{
    return false;
}

SubTreePrepareCheckType RSSystemProperties::GetSubTreePrepareCheckType()
{
    return SubTreePrepareCheckType::ENABLED;
}

bool RSSystemProperties::GetSingleDrawableLockerEnabled()
{
    return true;
}

bool RSSystemProperties::GetTargetUIFirstDfxEnabled(std::vector<std::string>& SurfaceNames)
{
    return false;
}

bool RSSystemProperties::GetWideColorSpaceEnabled()
{
    return true;
}

bool RSSystemProperties::GetRenderParallelEnabled()
{
    return false;
}

bool RSSystemProperties::IsForceClient()
{
    return false;
}

uint32_t RSSystemProperties::GetBlurEffectTerminateLimit()
{
    return 0;
}

bool RSSystemProperties::GetTextBlobAsPixelMap()
{
    return false;
}

bool RSSystemProperties::GetUnmarshParallelFlag()
{
    return false;
}

uint32_t RSSystemProperties::GetUnMarshParallelSize()
{
    return UINT32_MAX;
}

bool RSSystemProperties::GetGpuOverDrawBufferOptimizeEnabled()
{
    return false;
}

bool RSSystemProperties::GetSkipDisplayIfScreenOffEnabled()
{
    return false;
}

bool RSSystemProperties::GetBatchRemovingOnRemoteDiedEnabled()
{
    return false;
}

std::string RSSystemProperties::GetVersionType()
{
    return "";
}

bool RSSystemProperties::GetHwcDirtyRegionEnabled()
{
    return false;
}

bool RSSystemProperties::GetDrmMarkedFilterEnabled()
{
    return false;
}

bool RSSystemProperties::GetHveFilterEnabled()
{
    return false;
}

bool RSSystemProperties::GetOptimizeParentNodeRegionEnabled()
{
    return true;
}

bool RSSystemProperties::GetOptimizeHwcComposeAreaEnabled()
{
    return true;
}

bool RSSystemProperties::GetSurfaceOffscreenEnadbled()
{
    return true;
}

bool RSSystemProperties::GetWindowKeyFrameEnabled()
{
    return true;
}

bool RSSystemProperties::GetNodeGroupGroupedByUIEnabled()
{
    return false;
}

bool RSSystemProperties::GetTimeVsyncDisabled()
{
    return false;
}

bool RSSystemProperties::GetTextureExportDFXEnabled()
{
    return false;
}

bool RSSystemProperties::GetHybridRenderEnabled()
{
    return false;
}

int32_t RSSystemProperties::GetHybridRenderCcmEnabled()
{
    return 0;
}

// The switch are for scheme debugging. After the scheme is stabilizated, the switch will be removed.
bool RSSystemProperties::GetHybridRenderSystemEnabled()
{
    return false;
}

bool RSSystemProperties::GetHybridRenderDfxEnabled()
{
    return false;
}

uint32_t RSSystemProperties::GetHybridRenderTextBlobLenCount()
{
    return 0;
}

bool RSSystemProperties::ViewDrawNodeType()
{
    return false;
}

bool RSSystemProperties::GetHybridRenderParallelConvertEnabled()
{
    return false;
}

// The switch are for scheme debugging. After the scheme is stabilizated, the switch will be removed.
bool RSSystemProperties::GetHybridRenderCanvasEnabled()
{
    return false;
}

bool RSSystemProperties::GetHybridRenderMemeoryReleaseEnabled()
{
    return false;
}

// The switch are for scheme debugging. After the scheme is stabilizated, the switch will be removed.
bool RSSystemProperties::GetHybridRenderTextBlobEnabled()
{
    return false;
}

// The switch are for scheme debugging. After the scheme is stabilizated, the switch will be removed.
bool RSSystemProperties::GetHybridRenderSvgEnabled()
{
    return false;
}

// The switch are for scheme debugging. After the scheme is stabilizated, the switch will be removed.
bool RSSystemProperties::GetHybridRenderHmsymbolEnabled()
{
    return false;
}

int32_t RSSystemProperties::GetHybridRenderSwitch(ComponentEnableSwitch bitSeq)
{
    return 0;
}

bool RSSystemProperties::GetVKImageUseEnabled()
{
    return false;
}

void RSSystemProperties::SetDebugFmtTraceEnabled(bool flag)
{
    debugFmtTraceEnable_ = flag;
}

bool RSSystemProperties::GetDebugFmtTraceEnabled()
{
    return GetDebugTraceEnabled();
}

void RSSystemProperties::SetBehindWindowFilterEnabled(bool enabled)
{
}

bool RSSystemProperties::GetBehindWindowFilterEnabled()
{
    return false;
}

bool RSSystemProperties::GetSubThreadControlFrameRate()
{
    return false;
}

int RSSystemProperties::GetSubThreadDropFrameInterval()
{
    return 0;
}

bool RSSystemProperties::GetCompositeLayerEnabled()
{
    return false;
}
} // namespace Rosen
} // namespace OHOS
