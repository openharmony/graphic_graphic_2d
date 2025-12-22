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

#include "rs_render_pipeline.h"

#include <iservice_registry.h>
#include <malloc.h>
#include <parameters.h>
#include <string>
#include <unistd.h>

#include "common/rs_singleton.h"
#include "drawable/rs_canvas_drawing_render_node_drawable.h"
#include "feature/hwc/rs_uni_hwc_prevalidate_util.h"
#ifdef RS_ENABLE_GPU
#include "feature/round_corner_display/rs_message_bus.h"
#include "feature/round_corner_display/rs_rcd_render_manager.h"
#include "feature/round_corner_display/rs_round_corner_display_manager.h"
#include "feature/uifirst/rs_sub_thread_manager.h"
#include "feature/uifirst/rs_uifirst_frame_rate_control.h"
#include "feature/uifirst/rs_uifirst_manager.h"
#include "feature/capture/rs_ui_capture.h"
#endif
#ifdef RS_ENABLE_OVERLAY_DISPLAY
#include "feature/overlay_display/rs_overlay_display_manager.h"
#endif

#include "ge_mesa_blur_shader_filter.h"
#include "graphic_feature_param_manager.h"
#include "main/render_process/dfx/rs_process_dumper.h"
#include "parameter.h"
#include "pipeline/main_thread/rs_main_thread.h"
#include "pipeline/main_thread/rs_render_service_listener.h"
#include "pipeline/render_thread/rs_uni_render_thread.h"
#include "pipeline/rs_uni_render_judgement.h"
#include <platform/common/rs_log.h>
#include "platform/common/rs_system_properties.h"
#include "render/rs_render_kawase_blur_filter.h"
#include "rs_profiler.h"
#include "rs_trace.h"
#include "screen_manager/rs_screen_property.h"
#include "system/rs_system_parameters.h"
#include "text/font_mgr.h"
#include "pipeline/hwc/rs_hwc_context.h"

#undef LOG_TAG
#define LOG_TAG "RSRenderPipeline"

namespace OHOS{
namespace Rosen {

std::shared_ptr<RSRenderPipeline> RSRenderPipeline::Create(const std::shared_ptr<AppExecFwk::EventHandler>& handler,
    const std::shared_ptr<VSyncReceiver>& receiver,
    const sptr<RSIRenderToServiceConnection>& renderToServiceConnection,
    const sptr<RSVsyncManagerAgent>& rsVsyncManagerAgent)
{
    RS_LOGD("%{public}s: renderPipeline create", __func__);
    std::shared_ptr<RSRenderPipeline> render = std::make_shared<RSRenderPipeline>();
    render->Init(handler, receiver, renderToServiceConnection, rsVsyncManagerAgent);
    return render;
}

void RSRenderPipeline::Init(const std::shared_ptr<AppExecFwk::EventHandler>& handler,
    const std::shared_ptr<VSyncReceiver>& receiver,
    const sptr<RSIRenderToServiceConnection>& renderToServiceConnection,
    const sptr<RSVsyncManagerAgent>& rsVsyncManagerAgent)
{
    InitEnvironment();

    InitUniRenderConfig();

    // init CCM config
    InitCCMConfig();

    InitUniRenderThread();

    // todo
    RegisterRcdMsg();

    InitMainThread(handler, receiver, renderToServiceConnection, rsVsyncManagerAgent);

    // Gfx init
    InitDumper();

    // todo
    // RS_PROFILER_INIT(this);

    RS_LOGD("%{public}s: render pipeline int success.", __func__);
}

void RSRenderPipeline::PostMainThreadTask(RSTaskMessage::RSTask task)
{
    if (mainThread_ == nullptr) {
        return;
    }
    mainThread_->PostTask(task);
}

void RSRenderPipeline::PostUniRenderThreadTask(RSTaskMessage::RSTask task)
{
    if (uniRenderThread_ == nullptr) {
        return;
    }
    uniRenderThread_->PostTask(task);
}

void RSRenderPipeline::PostMainThreadSyncTask(RSTaskMessage::RSTask task)
{
    if (mainThread_ == nullptr) {
        return;
    }
    mainThread_->PostSyncTask(task);
}

void RSRenderPipeline::PostUniRenderThreadSyncTask(RSTaskMessage::RSTask task)
{
    if (uniRenderThread_ == nullptr) {
        return;
    }
    uniRenderThread_->PostSyncTask(task);
}

void RSRenderPipeline::PostMainThreadTask(RSTaskMessage::RSTask task, const std::string& name, int64_t delayTime,
    AppExecFwk::EventQueue::Priority priority)
{
    if (mainThread_ == nullptr) {
        return;
    }
    mainThread_->PostTask(task, name, delayTime, priority);
}

void RSRenderPipeline::OnScreenConnected(const sptr<RSScreenProperty>& rsScreenProperty,
    const std::shared_ptr<RSRenderComposerClient>& composerClient)
{
    RS_LOGI("RSRenderPipeline %{public}s, screen id: %{public}" PRIu64, __func__,
        rsScreenProperty ? rsScreenProperty->GetScreenId() : INVALID_SCREEN_ID);
    if (!mainThread_) {
        RS_LOGE("%{public}s mainThread_ is nullptr, return", __func__);
        return;
    }
    mainThread_->OnScreenConnected(rsScreenProperty);

    if (rsScreenProperty->IsVirtual()) {
        RS_LOGI("RSRenderPipeline %{public}s, virtual screen connected.", __func__);
        return;
    }
    if (!uniRenderThread_) {
        RS_LOGE("%{public}s uniRenderThread_ is nullptr, return", __func__);
        return;
    }
    uniRenderThread_->OnScreenConnected(rsScreenProperty->GetScreenId(), composerClient);
}

void RSRenderPipeline::OnScreenDisconnected(ScreenId screenId)
{
    if (!mainThread_) {
        RS_LOGE("%{public}s mainThread_ is nullptr, return", __func__);
        return;
    }
    mainThread_->OnScreenDisconnected(screenId);
    if (!uniRenderThread_) {
        RS_LOGE("%{public}s uniRenderThread_ is nullptr, return", __func__);
        return;
    }
    uniRenderThread_->OnScreenDisconnected(screenId);
}

void RSRenderPipeline::OnScreenPropertyChanged(const sptr<RSScreenProperty>& rsScreenProperty)
{
    if (!mainThread_) {
        RS_LOGE("%{public}s mainThread_ is nullptr, return", __func__);
        return;
    }
    mainThread_->OnScreenPropertyChanged(rsScreenProperty);
    RS_LOGD("RSRenderPipeline %{public}s, screen id: %{public}" PRIu64, __func__,
        rsScreenProperty ? rsScreenProperty->GetScreenId() : INVALID_SCREEN_ID);
}

void RSRenderPipeline::OnScreenRefresh(ScreenId screenId)
{
    if (mainThread_) {
        mainThread_->PostTask([mainThread = mainThread_]() {
            mainThread->SetForceUpdateUniRenderFlag(true);
            mainThread->RequestNextVSync();
        });
    }
}

void RSRenderPipeline::InitEnvironment()
{
    std::thread preLoadSysTTFThread([]() {
        Drawing::FontMgr::CreateDefaultFontMgr();
    });
    preLoadSysTTFThread.detach();

#ifdef RS_ENABLE_VK
if (Drawing::SystemProperties::IsUseVulkan()) {
    RsVulkanContext::SetRecyclable(false);
}
#endif
}

void RSRenderPipeline::InitUniRenderConfig()
{
    RSUniRenderJudgement::InitUniRenderConfig();
}

void RSRenderPipeline::InitCCMConfig()
{
    // feature param parse
    GraphicFeatureParamManager::GetInstance().Init();

    // need called after GraphicFeatureParamManager::GetInstance().Init();
    FilterCCMInit();

    imageEnhanceManager_ = std::make_shared<ImageEnhanceManager>();
}

void RSRenderPipeline::FilterCCMInit()
{
    RSFilterCacheManager::isCCMFilterCacheEnable_ = FilterParam::IsFilterCacheEnable();
    RSFilterCacheManager::isCCMEffectMergeEnable_ = FilterParam::IsEffectMergeEnable();
    RSProperties::SetFilterCacheEnabledByCCM(RSFilterCacheManager::isCCMFilterCacheEnable_);
    RSProperties::SetBlurAdaptiveAdjustEnabledByCCM(FilterParam::IsBlurAdaptiveAdjust());
    RSKawaseBlurShaderFilter::SetMesablurAllEnabledByCCM(FilterParam::IsMesablurAllEnable());
    GEMESABlurShaderFilter::SetMesaModeByCCM(FilterParam::GetSimplifiedMesaMode());
}

void RSRenderPipeline::RegisterRcdMsg()
{
    if (RSSingleton<RoundCornerDisplayManager>::GetInstance().GetRcdEnable()) {
        RS_LOGD("RSRenderPipeline::RegisterRcdMsg");
        if (isRcdServiceRegister_) {
            auto& rcdInstance = RSSingleton<RoundCornerDisplayManager>::GetInstance();
            auto& rcdHardManager = RSRcdRenderManager::GetInstance();
            auto& msgBus = RSSingleton<RsMessageBus>::GetInstance();
            msgBus.RegisterTopic<NodeId, uint32_t, uint32_t, uint32_t, uint32_t>(
                TOPIC_RCD_DISPLAY_SIZE, &rcdInstance,
                &RoundCornerDisplayManager::UpdateDisplayParameter);
            msgBus.RegisterTopic<NodeId, ScreenRotation>(
                TOPIC_RCD_DISPLAY_ROTATION, &rcdInstance,
                &RoundCornerDisplayManager::UpdateOrientationStatus);
            msgBus.RegisterTopic<NodeId, int>(
                TOPIC_RCD_DISPLAY_NOTCH, &rcdInstance,
                &RoundCornerDisplayManager::UpdateNotchStatus);
            msgBus.RegisterTopic<NodeId, bool>(
                TOPIC_RCD_DISPLAY_HWRESOURCE, &rcdInstance,
                &RoundCornerDisplayManager::UpdateHardwareResourcePrepared);
            RS_LOGD("RSRenderPipeline::RegisterRcdMsg Registed rcd renderservice end.");
            return;
        }
        RS_LOGD("RSRenderPipeline::RegisterRcdMsg Registed rcd renderservice already.");
    }
}

void RSRenderPipeline::InitMainThread(const std::shared_ptr<AppExecFwk::EventHandler>& handler,
    const std::shared_ptr<VSyncReceiver>& receiver,
    const sptr<RSIRenderToServiceConnection>& renderToServiceConnection,
    const sptr<RSVsyncManagerAgent>& rsVsyncManagerAgent)
{
    mainThread_ = RSMainThread::Instance();
    mainThread_->Init(handler, receiver, renderToServiceConnection, rsVsyncManagerAgent);
}

void RSRenderPipeline::InitUniRenderThread()
{
    uniRenderThread_ = &(RSUniRenderThread::Instance());
    uniRenderThread_->Start();

    uniBufferThread_ = &(RSBufferThread::Instance());
    uniBufferThread_->Start();
}

void RSRenderPipeline::InitDumper()
{
    auto rpDumper_ = std::make_shared<RSProcessDumper>();
    rpDumper_->RpDumpInit();
}

} // namespace Rosen
} // namespace OHOS