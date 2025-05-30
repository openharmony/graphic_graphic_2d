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

#include "platform/common/rs_system_properties.h"

namespace OHOS {
namespace Rosen {
const GpuApiType RSSystemProperties::systemGpuApiType_ = GpuApiType::OPENGL;
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
    return false;
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

DirtyRegionDebugType RSSystemProperties::GetDirtyRegionDebugType()
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

#ifndef NEW_SKIA
bool RSSystemProperties::GetReleaseResourceEnabled()
{
    return {};
}
#endif

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

bool RSSystemProperties::GetMESABlurFuzedEnabled()
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

bool RSSystemProperties::GetTargetUIFirstDfxEnabled(std::vector<std::string>& SurfaceNames)
{
    return false;
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
} // namespace Rosen
} // namespace OHOS
