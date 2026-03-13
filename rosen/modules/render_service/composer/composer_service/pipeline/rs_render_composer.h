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

#ifndef RENDER_SERVICE_COMPOSER_SERVICE_PIPELINE_RS_RENDER_COMPOSER_H
#define RENDER_SERVICE_COMPOSER_SERVICE_PIPELINE_RS_RENDER_COMPOSER_H

#include <mutex>
#include <set>
#include <unordered_map>
#include "common/rs_exception_check.h"
#include "event_handler.h"
#include "feature/hyper_graphic_manager/hgm_hardware_utils.h"
#include "feature/lpp/render_server/lpp_layer_collector.h"
#include "hgm_core.h"
#include "irs_composer_to_render_connection.h"
#include "pipeline/render_thread/rs_base_render_engine.h"
#include "rs_render_composer_context.h"
#include "screen_manager/rs_screen_property.h"
#include "vsync_manager_agent.h"
#ifdef RES_SCHED_ENABLE
#include "vsync_system_ability_listener.h"
#endif

namespace OHOS::Rosen {
using ComposerFallbackCallback = std::function<void(const sptr<Surface>& surface,
    const std::vector<RSLayerPtr>& layers)>;

namespace Composer {
template<typename Task>
class ScheduledTask : public RefBase {
public:
    static auto Create(Task&& task)
    {
        sptr<ScheduledTask<Task>> t(new ScheduledTask(std::move(task)));
        return std::make_pair(t, t->task_.get_future());
    }
    void Run() { task_(); }

private:
    explicit ScheduledTask(Task&& task) : task_(std::move(task)) {}
    ~ScheduledTask() override = default;

    using Return = std::invoke_result_t<Task>;
    std::packaged_task<Return()> task_;
};
} // namespace Composer

class RSRenderComposer {
public:
    RSRenderComposer(const std::shared_ptr<HdiOutput>& output, const sptr<RSScreenProperty>& property);
    ~RSRenderComposer() = default;
    void InitRsVsyncManagerAgent(const sptr<RSVsyncManagerAgent>& rsVsyncManagerAgent);

protected:
    void ComposerPrepare(uint32_t& currentRate, int64_t& delayTime, const PipelineParam& pipelineParam);
    void ComposerProcess(uint32_t currentRate, std::shared_ptr<RSLayerTransactionData> transactionData);
    void SetComposerToRenderConnection(const sptr<IRSComposerToRenderConnection>& composerToRenderConn);
    void PostTask(const std::function<void()>& task);
    void PostSyncTask(const std::function<void()>& task);
    void PostDelayTask(const std::function<void()>& task, int64_t delayTime);
    void OnScreenConnected(const std::shared_ptr<HdiOutput>& output, const sptr<RSScreenProperty>& property);
    void OnScreenDisconnected();
    void OnHwcRestored(const std::shared_ptr<HdiOutput>& output, const sptr<RSScreenProperty>& property);
    void OnHwcDead();
    void CleanLayerBufferBySurfaceId(uint64_t surfaceId);
    void SurfaceDump(std::string& dumpString);
    void FpsDump(std::string& dumpString, const std::string& layerName);
    void GetRefreshInfoToSP(std::string& dumpString, NodeId nodeId);
    void ClearFpsDump(std::string& dumpString, std::string& layerName);
    void HitchsDump(std::string& dumpString, std::string& layerArg);
    void RefreshRateCounts(std::string& dumpString);
    void ClearRefreshRateCounts(std::string& dumpString);
    void HandlePowerStatus(ScreenPowerStatus status);
    void SetAFBCEnabled(bool enabled);
    int64_t GetDelayTime() const;

private:
    void CreateAndInitComposer(const std::shared_ptr<HdiOutput>& output, const sptr<RSScreenProperty>& property);
#ifdef USE_VIDEO_PROCESSING_ENGINE
    static GraphicColorGamut ComputeTargetColorGamut(const std::vector<std::shared_ptr<RSLayer>>& layers);
    static bool IsAllRedraw(const std::vector<std::shared_ptr<RSLayer>>& layers);
    static GraphicPixelFormat ComputeTargetPixelFormat(const std::vector<std::shared_ptr<RSLayer>>& layers);
    static bool ConvertColorGamutToSpaceType(const GraphicColorGamut& colorGamut,
        HDI::Display::Graphic::Common::V1_0::CM_ColorSpaceType& colorSpaceType);
#endif
    std::shared_ptr<RSSurfaceOhos> CreateFrameBufferSurfaceOhos(const sptr<Surface>& surface);
    void RedrawScreenRCD(RSPaintFilterCanvas& canvas, const std::vector<std::shared_ptr<RSLayer>>& layers);
    void Redraw(const sptr<Surface>& surface, const std::vector<std::shared_ptr<RSLayer>>& layers);
    GraphicColorGamut ComputeTargetColorGamut(const sptr<SurfaceBuffer>& buffer);
    GraphicPixelFormat ComputeTargetPixelFormat(const sptr<SurfaceBuffer>& buffer);
    void OnPrepareComplete(sptr<Surface>& surface, const PrepareCompleteParam& param, void* data);
    void ContextRegisterPostTask();
#ifdef RES_SCHED_ENABLE
    void SubScribeSystemAbility();
    sptr<VSyncSystemAbilityListener> saStatusChangeListener_ = nullptr;
#endif
    int32_t AdaptiveModeStatus();
    bool IsDelayRequired(HgmCore& hgmCore, const PipelineParam& pipelineParam);
    int64_t CalculateDelayTime(HgmCore& hgmCore, uint32_t currentRate,
        int64_t currTime, const PipelineParam& pipelineParam);
    int64_t UpdateDelayTime(HgmCore& hgmCore, uint32_t currentRate, const PipelineParam& pipelineParam);
    void EndCheck(RSTimer timer);
    void DestroyComposerLayer(std::shared_ptr<RSLayerParcel> rsLayerParcel);
    void UpdateComposerLayer(std::shared_ptr<RSLayerParcel> rsLayerParcel);
    void UpdateTransactionData(std::shared_ptr<RSLayerTransactionData> transactionData);
    int64_t GetCurTimeCount();
    std::string GetSurfaceNameInLayers(const std::vector<std::shared_ptr<RSLayer>>& layers);
    std::string GetSurfaceNameInLayersForTrace(const std::vector<std::shared_ptr<RSLayer>>& layers);
    void ChangeLayersForActiveRectOutside(std::vector<std::shared_ptr<RSLayer>>& layers);
    bool IsDropDirtyFrame(const std::vector<std::shared_ptr<RSLayer>>& layers);
    void RecordTimestamp(uint64_t vsyncId);
    void ProcessComposerFrame(uint32_t currentRate, const PipelineParam& pipelineParam);
    template<typename Task, typename Return = std::invoke_result_t<Task>>
    std::future<Return> ScheduleTask(Task&& task)
    {
        auto [scheduledTask, taskFuture] = Composer::ScheduledTask<Task>::Create(std::move(task));
#ifdef RS_ENABLE_GPU
        PostTask([t(std::move(scheduledTask))]() { t->Run(); });
#endif
        return std::move(taskFuture);
    }
    void PreAllocateProtectedBuffer(sptr<SurfaceBuffer> buffer);
    GSError ClearFrameBuffers(bool isNeedResetContext = true);
    GSError ClearFrameBuffersInner(bool isNeedResetContext = true);
    void ClearRedrawGPUCompositionCache(const std::unordered_set<uint64_t>& bufferIds);
    void OnScreenVBlankIdleCallback(ScreenId screenId, uint64_t timestamp);
    void UpdateForSurfaceFps(const PipelineParam&);
    void AddSolidColorLayer(std::vector<std::shared_ptr<RSLayer>>& layers);
    void SetScreenBacklight(uint32_t level);
    void ReInit(const std::shared_ptr<HdiOutput>& output, const sptr<RSScreenProperty>& property);
    bool GetDisplayClientTargetProperty(GraphicPixelFormat& pixelFormat,
        GraphicColorGamut& colorGamut, const std::vector<std::shared_ptr<RSLayer>>& layers);
    std::shared_ptr<AppExecFwk::EventRunner> runner_ = nullptr;
    std::shared_ptr<AppExecFwk::EventHandler> handler_ = nullptr;
    std::shared_ptr<HdiOutput> hdiOutput_;
    int32_t threadTid_ = -1;
    ScreenId screenId_ = INVALID_SCREEN_ID;
    std::shared_ptr<RSRenderComposerContext> rsRenderComposerContext_;
    std::shared_ptr<RSBaseRenderEngine> uniRenderEngine_;
    std::shared_ptr<HgmHardwareUtils> hgmHardwareUtils_;
    LppLayerCollector lppLayerCollector_;
    ComposerFallbackCallback redrawCb_;
    std::mutex frameBufferSurfaceOhosMapMutex_;
    std::mutex surfaceMutex_;
    std::mutex preAllocMutex_;
    std::atomic<uint32_t> unExecuteTaskNum_ = 0;
    std::atomic<int64_t> delayTime_ = 0;
    int64_t lastCommitTime_ = 0;
    int64_t intervalTimePoints_ = 0;
    int64_t lastActualTime_ = 0;
    std::unordered_map<uint64_t, std::shared_ptr<RSSurfaceOhos>> frameBufferSurfaceOhosMap_;
    int exceptionCnt_ = 0;
    ExceptionCheck exceptionCheck_;
    sptr<IRSComposerToRenderConnection> composerToRenderConnection_;
    ComposerScreenInfo composerScreenInfo_;
    sptr<RSVsyncManagerAgent> rsVsyncManagerAgent_ = nullptr;
    bool isDisconnected_ = false;
    bool isHwcDead_ = false;
    bool enableAFBC_ = true;
    friend class RSRenderComposerAgent;
};
} // namespace OHOS::Rosen

#endif // RENDER_SERVICE_COMPOSER_SERVICE_PIPELINE_RS_RENDER_COMPOSER_H
