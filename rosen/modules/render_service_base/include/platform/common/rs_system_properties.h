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

#ifndef RENDER_SERVICE_BASE_COMMON_RS_COMMON_DEF_H
#define RENDER_SERVICE_BASE_COMMON_RS_COMMON_DEF_H

#include <atomic>
#include <string>
#include <vector>

#include "common/rs_macros.h"
#include "utils/system_properties.h"

namespace OHOS {
namespace Rosen {

enum class CrossNodeOffScreenRenderDebugType {
    DISABLED = 0,
    ENABLE,
    ENABLE_DFX
};

enum class DirtyRegionDebugType {
    DISABLED = 0,
    CURRENT_SUB,
    CURRENT_WHOLE,
    MULTI_HISTORY,
    CURRENT_SUB_AND_WHOLE,
    CURRENT_WHOLE_AND_MULTI_HISTORY,
    EGL_DAMAGE, // all dirty region information, includes detailed app dirty region and global dirty.
    DISPLAY_DIRTY, // detailed global dirty (before merge rect).
    MERGED_DIRTY, // advanced dirty region information, includes merged dirty region
    CUR_DIRTY_DETAIL_ONLY_TRACE = 10,
    UPDATE_DIRTY_REGION, // dirty region for each node.
    OVERLAY_RECT,   // drawRegion
    FILTER_RECT,
    SHADOW_RECT,
    PREPARE_CLIP_RECT,
    REMOVE_CHILD_RECT,
    RENDER_PROPERTIES_RECT,
    CANVAS_NODE_SKIP_RECT,
    OUTLINE_RECT,
    SUBTREE_SKIP_RECT, // dirty region of the subtree if subtree is skipped in preparation.
    SUBTREE_SKIP_OUT_OF_PARENT_RECT, // dirty region of the subtree when subtree is out of parent
};

enum class SurfaceRegionDebugType {
    DISABLED = 0,
    OPAQUE_REGION,
    VISIBLE_REGION,
};

enum class PartialRenderType {
    DISABLED = 0,                               // 0, disable partial render, including set damage region
    SET_DAMAGE,                                 // 1, set damageregion, without draw_op dropping
    SET_DAMAGE_AND_DROP_OP,                     // 2, drop draw_op if node is not in dirty region
    SET_DAMAGE_AND_DROP_OP_OCCLUSION,           // 3, drop draw_op if node is not in visible region (unirender)
    SET_DAMAGE_AND_DROP_OP_NOT_VISIBLEDIRTY,    // 4, drop draw_op if node is not in visible dirty region (unirender)
    SET_DAMAGE_BUT_COMPLETE_RENDER,             // 5, set full screen dirty region and set damage
};

enum class StencilPixelOcclusionCullingType {
    DEFAULT = -1, // follow the ccm configuration
    DISABLED = 0,
    ENABLED = 1
};

enum class AdvancedDirtyRegionType {
    DISABLED = 0,
    SET_ADVANCED_SURFACE_AND_DISPLAY,
    SET_ADVANCED_DISPLAY,
};

enum class DirtyAlignType {
    DISABLED = 0,
    ENABLED = 1,
};

enum class DumpSurfaceType {
    DISABLED = 0,
    SINGLESURFACE,
    ALLSURFACES,
    PIXELMAP,
    SURFACEBUFFER
};

enum class ParallelRenderingType {
    AUTO = 0,
    DISABLE = 1,
    ENABLE = 2
};

enum class HgmRefreshRates {
    SET_RATE_NULL = 0,
    SET_RATE_30 = 30,
    SET_RATE_60 = 60,
    SET_RATE_90 = 90,
    SET_RATE_120 = 120
};

enum class HgmRefreshRateModes {
    SET_RATE_MODE_AUTO = -1,
    SET_RATE_MODE_NULL = 0,
    SET_RATE_MODE_LOW = 1,
    SET_RATE_MODE_MEDIUN = 2,
    SET_RATE_MODE_HIGH = 3
};

enum class SubTreePrepareCheckType {
    DISABLED = 0,                       // 0, Disable the IsSubTreeNeedPrepare check, prepare the whole subtree
    DISABLE_SUBTREE_DIRTY_CHECK = 1,    // 1, Disable the IsSubTreeDirty check
    ENABLED = 2,                        // 2, Enable the IsSubTreeNeedPrepare check
};

enum class DdgrOpincType {
    OPINC_NONE = 0,
    OPINC_AUTOCACHE,
    OPINC_AUTOCACHE_REALDRAW,
};

enum class DdgrOpincDfxType {
    OPINC_DFX_NONE,
    OPINC_DFX_AUTO,
};

enum class ComponentEnableSwitch {
    TEXTBLOB = 0,
    SVG,
    HMSYMBOL,
    CANVAS,
    SWITCH_MAX,
};

struct GetComponentSwitch {
    ComponentEnableSwitch type;
    bool (*ComponentHybridSwitch)();
};

using OnSystemPropertyChanged = void(*)(const char*, const char*, void*);

class RSB_EXPORT RSSystemProperties final {
public:
    ~RSSystemProperties() = default;

    // used by clients
    static std::string GetRecordingFile();
    static bool IsSceneBoardEnabled();
    static int GetDumpFrameNum();
    static int GetSceneJankFrameThreshold();
    static void SetRecordingDisenabled();
    static int GetRecordingEnabled();
    static bool GetVkQueuePriorityEnable();

    static bool GetProfilerEnabled();
    static void SetProfilerDisabled();
    static bool GetInstantRecording();
    static bool GetProfilerPixelCheckMode();
    static void SetInstantRecording(bool flag);
    static uint32_t GetBetaRecordingMode();
    static void SetBetaRecordingMode(uint32_t param);
    static bool GetSaveRDC();
    static void SetSaveRDC(bool flag);

    static bool GetUniRenderEnabled();
    static bool GetRenderNodeTraceEnabled();
    static bool GetDrawOpTraceEnabled();
    static bool GetAnimationTraceEnabled();
    static bool GetAnimationDelayOptimizeEnabled();
    static bool GetRSClientMultiInstanceEnabled();
    static bool GetRenderNodePurgeEnabled();
    static bool GetRSImagePurgeEnabled();
    static bool GetClosePixelMapFdEnabled();
    static DirtyRegionDebugType GetDirtyRegionDebugType();
    static AdvancedDirtyRegionType GetAdvancedDirtyRegionEnabled();
    static DirtyAlignType GetDirtyAlignEnabled();
    static PartialRenderType GetPartialRenderEnabled();
    static PartialRenderType GetUniPartialRenderEnabled();
    static StencilPixelOcclusionCullingType GetStencilPixelOcclusionCullingEnabled();
    static float GetClipRectThreshold();
    static bool GetAllSurfaceVisibleDebugEnabled();
    static bool GetVirtualDirtyDebugEnabled();
    static bool GetVirtualDirtyEnabled();
    static bool GetExpandScreenDirtyEnabled();
    static bool GetVirtualExpandScreenSkipEnabled();
    static bool GetOcclusionEnabled();
    static std::string GetRSEventProperty(const std::string &paraName);
    static bool GetHighContrastStatus();
    static uint32_t GetCorrectionMode();
    static DumpSurfaceType GetDumpSurfaceType();
    static long long int GetDumpSurfaceId();
    static bool GetTargetDirtyRegionDfxEnabled(std::vector<std::string>& dfxTargetSurfaceNames_);
    static bool GetOpaqueRegionDfxEnabled();
    static bool GetVisibleRegionDfxEnabled();
    static SurfaceRegionDebugType GetSurfaceRegionDfxType();
    static bool GetDumpLayersEnabled();
    static bool GetHardwareComposerEnabled();
    static bool GetDoDirectCompositionEnabled();
    static bool GetDumpRsTreeDetailEnabled();
    static bool GetHardwareComposerEnabledForMirrorMode();
    static bool GetHwcRegionDfxEnabled();
    static bool GetDrawMirrorCacheImageEnabled();
    static bool GetPixelmapDfxEnabled();
    static bool GetAFBCEnabled();
    static bool GetReleaseResourceEnabled();
    static bool GetReclaimMemoryEnabled();
    static bool GetRSScreenRoundCornerEnable();

    static void SetDrawTextAsBitmap(bool flag);
    static bool GetDrawTextAsBitmap();
    static void SetCacheEnabledForRotation(bool flag);
    static bool GetCacheEnabledForRotation();
    static ParallelRenderingType GetPrepareParallelRenderingEnabled();
    static ParallelRenderingType GetParallelRenderingEnabled();
    static HgmRefreshRates GetHgmRefreshRatesEnabled();
    static void SetHgmRefreshRateModesEnabled(std::string param);
    static HgmRefreshRateModes GetHgmRefreshRateModesEnabled();
    static bool GetHardCursorEnabled();
    static bool GetSLRScaleEnabled();
    static float GetAnimationScale();
    static bool GetProxyNodeDebugEnabled();
    static bool GetFilterCacheEnabled();
    static int GetFilterCacheUpdateInterval();
    static int GetFilterCacheSizeThreshold();
    static bool GetMaskLinearBlurEnabled();
    static bool GetMotionBlurEnabled();
    static bool GetMagnifierEnabled();
    static bool GetDynamicBrightnessEnabled();
    static bool GetKawaseEnabled();
    static void SetForceHpsBlurDisabled(bool flag);
    static float GetHpsBlurNoiseFactor();
    static bool GetHpsBlurEnabled();
    static bool GetMESABlurFuzedEnabled();
    static int GetSimplifiedMesaEnabled();
    static bool GetForceKawaseDisabled();
    static float GetKawaseRandomColorFactor();
    static bool GetRandomColorEnabled();
    static bool GetImageReleaseUsingPostTask();
    static bool GetKawaseOriginalEnabled();
    static bool GetBlurEnabled();
    static bool GetForegroundFilterEnabled();
    static const std::vector<float>& GetAiInvertCoef();
    static bool GetSkipForAlphaZeroEnabled();
    static bool GetSkipGeometryNotChangeEnabled();
    static bool GetRenderParallelEnabled();
    static bool GetPropertyDrawableEnable();

    static bool GetDrawFilterWithoutSnapshotEnabled();
    static bool GetBlurExtraFilterEnabled();
    static bool GetDiscardCanvasBeforeFilterEnabled();
    static bool GetPurgeBetweenFramesEnabled();
    static bool GetGpuMemoryAsyncReclaimerEnabled();
    static bool GetGpuCacheSuppressWindowEnabled();

    static bool GetAnimationCacheEnabled();

    static bool GetBoolSystemProperty(const char* name, bool defaultValue);
    static int WatchSystemProperty(const char* name, OnSystemPropertyChanged func, void* context);
    static bool GetCacheOptimizeRotateEnable();
    static CrossNodeOffScreenRenderDebugType GetCrossNodeOffScreenStatus();
    static bool GetSingleDrawableLockerEnabled();
    static bool GetUIFirstEnabled();
    static bool GetUIFirstDebugEnabled();
    static bool GetUIFirstOptScheduleEnabled();
    static bool GetUIFirstDirtyEnabled();
    static bool GetUIFirstDirtyDebugEnabled();
    static bool GetTargetUIFirstDfxEnabled(std::vector<std::string>& SurfaceNames);
    static bool GetUIFirstBehindWindowFilterEnabled();
    static bool GetWideColorSpaceEnabled();
    static bool GetSurfaceOffscreenEnadbled();
    static bool GetDebugTraceEnabled();
    static int GetDebugTraceLevel();
    static bool FindNodeInTargetList(std::string node);
    static bool IsFoldScreenFlag();
    static bool IsSmallFoldDevice();
    static bool GetCacheCmdEnabled();
    static bool GetASTCEnabled();
    static bool GetCachedBlurPartialRenderEnabled();
    static bool GetImageGpuResourceCacheEnable(int width, int height);
    static bool GetDrmEnabled();
    static bool GetSurfaceNodeWatermarkEnabled();
    static bool IsPhoneType();
    static bool IsSuperFoldDisplay();
    static bool IsBetaRelease();
    static bool GetSyncTransactionEnabled();
    static int GetSyncTransactionWaitDelay();
    static bool GetSingleFrameComposerEnabled();
    static bool GetSingleFrameComposerCanvasNodeEnabled();
    static bool GetSecurityPermissionCheckEnabled();
    static bool GetParallelUploadTexture();
    static bool GetEffectMergeEnabled();
    static SubTreePrepareCheckType GetSubTreePrepareCheckType();
    static bool GetHdrImageEnabled();
    static bool GetHdrVideoEnabled();
    static bool IsForceClient();
    static bool GetDrmMarkedFilterEnabled();
    static bool GetUnmarshParallelFlag();
    static uint32_t GetUnMarshParallelSize();
    static bool GetGpuOverDrawBufferOptimizeEnabled();

    static DdgrOpincType GetDdgrOpincType();
    static bool IsDdgrOpincEnable();
    static bool GetAutoCacheDebugEnabled();
    static DdgrOpincDfxType GetDdgrOpincDfxType();
    static bool IsOpincRealDrawCacheEnable();
    static bool GetSkipDisplayIfScreenOffEnabled();
    static bool GetBatchRemovingOnRemoteDiedEnabled();
#ifdef RS_ENABLE_STACK_CULLING
    static bool GetViewOcclusionCullingEnabled();
#endif

    static bool GetDumpUICaptureEnabled();
    static bool GetDumpUIPixelmapEnabled();
    static bool GetDumpImgEnabled();

    static bool GetTransactionTerminateEnabled();
    static uint32_t GetBlurEffectTerminateLimit();

    static int GetVirtualScreenScaleModeDFX();
    static bool GetTextBlobAsPixelMap();
    static inline GpuApiType GetGpuApiType()
    {
        return RSSystemProperties::systemGpuApiType_;
    }

    static inline bool IsUseVulkan()
    {
        return RSSystemProperties::GetGpuApiType() != GpuApiType::OPENGL;
    }

    static bool ViewDrawNodeType();
    static bool GetJankLoadOptimizeEnabled();
    static int GetRSNodeLimit();
    static std::string GetVersionType();
    static bool GetHwcDirtyRegionEnabled();
    static bool GetHveFilterEnabled();
    static bool GetDmaReclaimParam();
    static bool GetOptimizeParentNodeRegionEnabled();
    static bool GetOptimizeHwcComposeAreaEnabled();
    static bool GetWindowKeyFrameEnabled();
    static bool GetNodeGroupGroupedByUIEnabled();
    static bool GetTimeVsyncDisabled();
    static void SetDebugFmtTraceEnabled(bool flag);
    static bool GetTextureExportDFXEnabled();
    static bool GetDebugFmtTraceEnabled();

    static bool GetHybridRenderEnabled();
    static bool GetHybridRenderDfxEnabled();
    static uint32_t GetHybridRenderTextBlobLenCount();
    static bool GetHybridRenderParallelConvertEnabled();
    static bool GetHybridRenderCanvasEnabled();
    static bool GetHybridRenderMemeoryReleaseEnabled();
    static bool GetHybridRenderSystemEnabled();
    static int32_t GetHybridRenderCcmEnabled();
    static int32_t GetHybridRenderSwitch(ComponentEnableSwitch bitSeq);
    static bool GetHybridRenderTextBlobEnabled();
    static bool GetHybridRenderSvgEnabled();
    static bool GetHybridRenderHmsymbolEnabled();

    static bool GetVKImageUseEnabled();
    static void SetBehindWindowFilterEnabled(bool enabled);
    static bool GetBehindWindowFilterEnabled();
    static bool GetSubThreadControlFrameRate();
    static int GetSubThreadDropFrameInterval();

private:
    RSSystemProperties() = default;

    static inline bool isUniRenderEnabled_ = false;
    inline static bool isDrawTextAsBitmap_ = false;
    inline static bool cacheEnabledForRotation_ = false;
    static inline bool forceHpsBlurDisabled_ = false;
    static inline bool debugFmtTraceEnable_ = false;
    static inline bool isBehindWindowFilterEnabled_ = true;
    static const GpuApiType systemGpuApiType_;
    static const DdgrOpincType ddgrOpincType_;
    static const DdgrOpincDfxType ddgrOpincDfxType_;
};

} // namespace Rosen
} // namespace OHOS

#endif // RENDER_SERVICE_BASE_COMMON_RS_COMMON_DEF_H
