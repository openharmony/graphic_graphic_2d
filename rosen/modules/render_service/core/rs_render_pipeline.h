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

 #ifndef RS_RENDER_PIPELINE_H
 #define RS_RENDER_PIPELINE_H

 #include <event_handler.h>
 #include <map>
 #include <unordered_set>
 #include "common/rs_common_def.h"
 #include "common/rs_thread_handler.h"
 #include "dirty_region/rs_optimize_canvas_dirty_collector.h"
 #include "ipc_callbacks/rs_iocclusion_change_callback.h"
 #include "ipc_callbacks/rs_isurface_occlusion_change_callback.h"
 #include "info_collection/rs_gpu_dirty_region_collection.h"
 #include "info_collection/rs_hardware_compose_disabled_reason_collection.h"
 #include "info_collection/rs_hdr_collection.h"
 #include "info_collection/rs_layer_compose_collection.h"
 #include "irs_render_to_composer_connection.h"
 #include "memory/rs_memory_manager.h"
 #include "pipeline/image_enhance/image_enhance_manager.h"
 #include "platform/ohos/transaction/zidl/rs_iclient_to_service_connection.h"
 #include "rs_render_composer_client.h"
 #include "vsync_receiver.h"
 #include "screen_manager/screen_types.h"
 #include "feature/hyper_graphic_manager/hgm_client.h"
 #include "memory/rs_memory_manager.h"

namespace OHOS {
namespace Rosen {
class RSMainThread;
class RSUniRenderThread;
class RSBufferThread;
class RSScreenProperty;
class RSHwcContext;
class RSRenderPipeline final : public RefBase {
public:
    static std::shared_ptr<RSRenderPipeline> Create(const std::shared_ptr<AppExecFwk::EventHandler>& handler,
        const std::shared_ptr<VSyncReceiver>& receiver,
        const sptr<RSIRenderToServiceConnection>& renderToServiceConnection);

    void OnScreenConnected(const sptr<RSScreenProperty>& rsScreenProperty,
        const std::shared_ptr<RSRenderComposerClient>& composerClient);
    void OnScreenDisconnected(ScreenId screenId);
    void OnScreenPropertyChanged(const sptr<RSScreenProperty>& rsScreenProperty);
    void OnScreenRefresh(ScreenId screenId);
    void OnScreenBacklightChanged(ScreenId screenId, uint32_t level);

#ifdef RS_ENABLE_OVERLAY_DISPLAY
    ErrCode SetOverlayDisplayMode(int32_t mode);
#endif
    bool GetHighContrastTextState();
    ErrCode SetCurtainScreenUsingStatus(bool isCurtainScreenOn);
    ErrCode GetBitmap(NodeId id, Drawing::Bitmap& bitmap, bool& success);
    ErrCode SetDiscardJankFrames(bool discardJankFrames);
    ErrCode ReportJankStats();
    ErrCode ReportEventResponse(DataBaseRs info);
    ErrCode ReportEventComplete(DataBaseRs info);
    ErrCode ReportEventJankFrame(DataBaseRs info);
    ErrCode ReportRsSceneJankStart(AppInfo info);
    ErrCode ReportRsSceneJankEnd(AppInfo info);
    ErrCode AvcodecVideoStart(const std::vector<uint64_t>& uniqueIdList,
        const std::vector<std::string>& surfaceNameList, uint32_t fps, uint64_t reportTime);
    ErrCode AvcodecVideoStop(const std::vector<uint64_t>& uniqueIdList,
        const std::vector<std::string>& surfaceNameList, uint32_t fps);
    ErrCode CreateNodeAndSurface(const RSSurfaceRenderNodeConfig& config, sptr<Surface>& sfc,
        bool unobscured = false);
    void DoDump(std::unordered_set<std::u16string> &argSets);
    ErrCode CreatePixelMapFromSurface(sptr<Surface> surface, const Rect &srcRect,
        std::shared_ptr<Media::PixelMap> &pixelMap);
    void HandleHwcEvent(uint32_t deviceId, uint32_t eventId, const std::vector<int32_t>& eventData);

    void RegisterOcclusionChangeCallback(pid_t pid, sptr<RSIOcclusionChangeCallback> callback);
    void RegisterSurfaceOcclusionChangeCallback(
        NodeId id, pid_t pid, sptr<RSISurfaceOcclusionChangeCallback> callback, std::vector<float>& partitionPoints);
    void UnRegisterSurfaceOcclusionChangeCallback(NodeId id);
    ErrCode GetMemoryGraphic(int pid, MemoryGraphic& memoryGraphic);
    int32_t GetPidGpuMemoryInMB(pid_t pid, float &gpuMemInMB);
    ErrCode RepaintEverything() const;
    bool RegisterTypeface(uint64_t globalUniqueId, std::shared_ptr<Drawing::Typeface>& typeface);
    bool UnRegisterTypeface(uint64_t globalUniqueId);

    void NotifyPackageEvent(uint32_t listSize, const std::vector<std::string>& packageList);
    void HandleDisplayPackageEvent(uint32_t listSize, const std::vector<std::string>& packageList);
    void HgmForceUpdateTask(bool flag, const std::string& fromWhom);
    ErrCode SetLayerTop(const std::string &nodeIdStr, bool isTop);
    ErrCode GetTotalAppMemSize(float& cpuMemSize, float& gpuMemSize);
    ErrCode GetMemoryGraphics(std::vector<MemoryGraphic>& memoryGraphics);
    ErrCode GetPixelMapByProcessId(std::vector<PixelMapInfo>& pixelMapInfoVector, pid_t pid, int32_t& repCode);
    float GetRotationInfoFromSurfaceBuffer(const sptr<SurfaceBuffer>& buffer);
    ErrCode SetWatermark(pid_t callingPid, const std::string& name, std::shared_ptr<Media::PixelMap> watermark, bool& success);
    void ShowWatermark(const std::shared_ptr<Media::PixelMap> &watermarkImg, bool isShow);
    void GetSurfaceRootNodeId(NodeId& windowNodeId);
    ErrCode SetForceRefresh(const std::string &nodeIdStr, bool isForceRefresh);
    void SetBehindWindowFilterEnabled(bool enabled);
    void SetVmaCacheStatus(bool flag);
    bool GetBehindWindowFilterEnabled();
    ErrCode SetColorFollow(const std::string &nodeIdStr, bool isColorFollow);
    void SetFreeMultiWindowStatus(bool enable);
    int32_t RegisterSelfDrawingNodeRectChangeCallback(
        pid_t remotePid, const RectConstraint& constraint, sptr<RSISelfDrawingNodeRectChangeCallback> callback);
    int32_t UnRegisterSelfDrawingNodeRectChangeCallback(pid_t remotePid);

    uint32_t GetRealtimeRefreshRate(ScreenId screenId);
    int32_t RegisterUIExtensionCallback(pid_t pid, uint64_t userId, sptr<RSIUIExtensionCallback> callback,
        bool unobscured = false);
    void SetShowRefreshRateEnabled(bool enabled, int32_t type);
    ErrCode GetShowRefreshRateEnabled(bool& enable);
    ErrCode SetGpuCrcDirtyEnabledPidList(const std::vector<int32_t>& pidList);

    std::vector<ActiveDirtyRegionInfo> GetActiveDirtyRegionInfo();
    GlobalDirtyRegionInfo GetGlobalDirtyRegionInfo();
    LayerComposeInfo GetLayerComposeInfo();
    HwcDisabledReasonInfos GetHwcDisabledReasonInfo();
    ErrCode CleanResources(pid_t pid);
    ErrCode GetHdrOnDuration(int64_t& hdrOnDuration);
    ErrCode SetOptimizeCanvasDirtyPidList(const std::vector<int32_t>& pidList);
    void SetScreenFrameGravity(ScreenId id, Gravity gravity);

    void InitRsVsyncManagerAgent(const sptr<RSVsyncManagerAgent>& rsVsyncManagerAgent);
    void RegisterScreenSwitchFinishCallback(const sptr<RSIRenderToServiceConnection>& conn);

private:
    void Init(const std::shared_ptr<AppExecFwk::EventHandler>& handler,
        const std::shared_ptr<VSyncReceiver>& receiver,
        const sptr<RSIRenderToServiceConnection>& renderToServiceConnection);
    void InitEnvironment();
    void InitUniRenderConfig();
    void InitCCMConfig();
    // RS Filter CCM init
    void FilterCCMInit();
    void RegisterRcdMsg();
    void InitMainThread(const std::shared_ptr<AppExecFwk::EventHandler>& handler,
        const std::shared_ptr<VSyncReceiver>& receiver,
        const sptr<RSIRenderToServiceConnection>& renderToServiceConnection);
    void InitUniRenderThread();
    void InitDumper();

    bool isRcdServiceRegister_ = false;

    RSMainThread* mainThread_ = nullptr;
    RSUniRenderThread* uniRenderThread_ = nullptr;
    RSBufferThread* uniBufferThread_ = nullptr;

    std::shared_ptr<ImageEnhanceManager> imageEnhanceManager_ = nullptr;

    friend class RSServiceToRenderConnection;
    friend class RSRenderProcessAgent;
    friend class RSRenderServiceAgent;
    friend class RSRenderPipelineAgent;

#ifdef RS_PROFILER_ENABLED
    friend class RSProfiler;
#endif
};
} // Rosen
} // OHOS

#endif // RS_RENDER_PIPELINE_H