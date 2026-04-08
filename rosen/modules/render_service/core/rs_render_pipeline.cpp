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
#include "engine/rs_base_render_engine.h"
#include "engine/rs_uni_render_engine.h"
#include "feature/hwc/rs_uni_hwc_prevalidate_util.h"
#ifdef RS_ENABLE_GPU
#include "feature/uifirst/rs_sub_thread_manager.h"
#include "feature/uifirst/rs_uifirst_frame_rate_control.h"
#include "feature/uifirst/rs_uifirst_manager.h"
#include "feature/capture/rs_ui_capture.h"
#endif
#ifdef RS_ENABLE_OVERLAY_DISPLAY
#include "feature/overlay_display/rs_overlay_display_manager.h"
#endif
#ifdef RS_ENABLE_TV_PQ_METADATA
#include "feature/tv_metadata/rs_tv_metadata_manager.h"
#endif

#include "dfx/rs_pipeline_dumper.h"
#include "ge_mesa_blur_shader_filter.h"
#include "ge_render.h"
#include "graphic_feature_param_manager.h"

#include "parameter.h"
#include "pipeline/main_thread/rs_main_thread.h"
#include "pipeline/main_thread/rs_render_service_listener.h"
#include "pipeline/render_thread/rs_uni_render_thread.h"
#include "pipeline/render_thread/rs_uni_render_util.h"
#include "pipeline/rs_uni_render_judgement.h"
#include <platform/common/rs_log.h>
#include "platform/common/rs_system_properties.h"
#ifdef HETERO_HDR_ENABLE
#include "rs_hetero_hdr_manager.h"
#endif
#include "rs_profiler.h"
#include "rs_trace.h"
#include "screen_manager/rs_screen_property.h"
#include "system/rs_system_parameters.h"
#include "text/font_mgr.h"
#include "pipeline/hwc/rs_hwc_context.h"

#undef LOG_TAG
#define LOG_TAG "RSRenderPipeline"

namespace OHOS {
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

    InitMainThread(handler, receiver, renderToServiceConnection, rsVsyncManagerAgent);

    // Gfx init
    InitDumper(handler);

    RS_LOGD("%{public}s: render pipeline int success.", __func__);
}

std::shared_ptr<RSComposerClientManager> RSRenderPipeline::GetComposerClientManager() const
{
    return composerClientManager_;
}

void RSRenderPipeline::PostMainThreadTask(RSTaskMessage::RSTask task)
{
    if (mainThread_ == nullptr) {
        return;
    }
    mainThread_->PostTask(task);
}

void RSRenderPipeline::PostMainThreadTask(RSTaskMessage::RSTask task, const std::string& name, int64_t delayTime,
    AppExecFwk::EventQueue::Priority priority)
{
    if (mainThread_ == nullptr) {
        return;
    }
    mainThread_->PostTask(task, name, delayTime, priority);
}

void RSRenderPipeline::PostUniRenderThreadTask(RSTaskMessage::RSTask task)
{
    if (uniRenderThread_ == nullptr) {
        return;
    }
    uniRenderThread_->PostTask(task);
}

bool RSRenderPipeline::PostMainThreadSyncTask(RSTaskMessage::RSTask task)
{
    if (mainThread_ == nullptr) {
        return false;
    }
    return mainThread_->PostSyncTask(task);
}

void RSRenderPipeline::PostUniRenderThreadSyncTask(RSTaskMessage::RSTask task)
{
    if (uniRenderThread_ == nullptr) {
        return;
    }
    uniRenderThread_->PostSyncTask(task);
}

void RSRenderPipeline::AddTransactionDataPidInfo(pid_t remotePid)
{
    if (mainThread_ == nullptr) {
        return;
    }
    mainThread_->AddTransactionDataPidInfo(remotePid);
}

void RSRenderPipeline::OnScreenConnected(const sptr<RSScreenProperty>& rsScreenProperty,
    const sptr<IRSRenderToComposerConnection>& renderToComposerConn,
    const sptr<IRSComposerToRenderConnection>& composerToRenderConn,
    const std::shared_ptr<HdiOutput>& output)
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
    std::shared_ptr<RSComposerClient> composerClient = nullptr;
    if (!rsScreenProperty->IsVirtual()) {
        composerToRenderConn->RegisterReleaseLayerBuffersCB(
            std::bind(&RSUniRenderThread::ReleaseLayerBuffers, uniRenderThread_, std::placeholders::_1));
        RegisterJudgeLppLayerCB(composerToRenderConn);
        composerClient = RSComposerClient::Create(renderToComposerConn, composerToRenderConn);
        composerClient->RegisterOnReleaseLayerBuffersCB(std::bind(&RSUniRenderThread::OnReleaseLayerBuffers,
            uniRenderThread_, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));
        composerClient->SetRmvSurfaceFpsOpCallback([](const std::vector<SurfaceFpsOp>& rmvList) {
            RSMainThread::Instance()->RmvSurfaceFpsOp(rmvList);
        });
        if (RSUniRenderJudgement::GetUniRenderEnabledType() != UniRenderEnabledType::UNI_RENDER_ENABLED_FOR_ALL) {
            composerClient->SetOutput(output);
        }
    }
    composerClientManager_->AddComposerClient(rsScreenProperty->GetScreenId(), composerClient);
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
    composerClientManager_->DeleteComposerClient(screenId);
}

void RSRenderPipeline::OnScreenPropertyChanged(
    ScreenId id, ScreenPropertyType type, const sptr<ScreenPropertyBase>& property)
{
    if (!mainThread_) {
        RS_LOGE("%{public}s mainThread_ is nullptr, return", __func__);
        return;
    }
    mainThread_->OnScreenPropertyChanged(id, type, property);
    RS_LOGD("RSRenderPipeline %{public}s, screen id: %{public}" PRIu64, __func__, id);
}

void RSRenderPipeline::OnScreenRefresh(ScreenId screenId)
{
    if (!mainThread_) {
        RS_LOGE("%{public}s mainThread_ is nullptr, return", __func__);
        return;
    }
    mainThread_->SetForceUpdateUniRenderFlag(true);
    mainThread_->RequestNextVSync();
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
    GraphicsEffectEngine::GERender::SetMesablurAllEnabledByCCM(FilterParam::IsMesablurAllEnable());
    GEMESABlurShaderFilter::SetMesaModeByCCM(FilterParam::GetSimplifiedMesaMode());
}

void RSRenderPipeline::InitMainThread(const std::shared_ptr<AppExecFwk::EventHandler>& handler,
    const std::shared_ptr<VSyncReceiver>& receiver,
    const sptr<RSIRenderToServiceConnection>& renderToServiceConnection,
    const sptr<RSVsyncManagerAgent>& rsVsyncManagerAgent)
{
    mainThread_ = RSMainThread::Instance();
    mainThread_->Init(handler, receiver, renderToServiceConnection, rsVsyncManagerAgent, composerClientManager_);
}

void RSRenderPipeline::InitUniRenderThread()
{
    RSBaseRenderEngine::RegisterUniRenderUtilCallback(RSUniRenderUtil::CreateLayerBufferDrawParam,
        RSUniRenderUtil::DrawRectForDfx);
#ifdef HETERO_HDR_ENABLE
    RSBaseRenderEngine::RegisterHeteroHDRCallback(
        [](BufferDrawParam& params, std::shared_ptr<Drawing::ShaderEffect>& imageShader) {
            RSHeteroHDRManager::Instance().GenerateHDRHeteroShader(params, imageShader);
        },
        [](RSPaintFilterCanvas& canvas, const DrawableV2::RSSurfaceRenderNodeDrawable& surfaceDrawable,
            BufferDrawParam& params) -> bool {
            return RSHeteroHDRManager::Instance().UpdateHDRHeteroParams(canvas, surfaceDrawable, params);
        },
        []() -> std::shared_ptr<RSSurfaceHandler> { return RSHeteroHDRManager::Instance().GetHDRSurfaceHandler(); });
#endif
#ifdef RS_ENABLE_TV_PQ_METADATA
    RSUniRenderEngine::RegisterTvMetadataCallback(
        [](const RSSurfaceRenderParams& params, const sptr<SurfaceBuffer>& buffer) {
            RSTvMetadataManager::Instance().RecordTvMetadata(params, buffer);
        });
#endif
    uniRenderThread_ = &(RSUniRenderThread::Instance());
    composerClientManager_ = std::make_shared<RSComposerClientManager>();
    uniRenderThread_->Start(composerClientManager_);
}

void RSRenderPipeline::InitDumper(const std::shared_ptr<AppExecFwk::EventHandler>& handler)
{
    rpDumpManager_ = std::make_shared<RSPipelineDumpManager>();
    rpDumper_ = std::make_shared<RSPipelineDumper>(handler);
    rpDumper_->RenderPipelineDumpInit(rpDumpManager_);
}

bool RSRenderPipeline::RemoveConnection(const sptr<RSIConnectionToken>& token)
{
    if (token == nullptr) {
        RS_LOGE("RemoveConnection: token is nullptr");
        return false;
    }
    // temporarily extending the life cycle
    auto tokenObj = token->AsObject();
    std::unique_lock<std::mutex> lock(renderConnectionMutex_);
    auto iter = renderConnections_.find(tokenObj);
    if (iter == renderConnections_.end()) {
        return false;
    }
    renderConnections_.erase(tokenObj);
    lock.unlock();
    return true;
}

void RSRenderPipeline::AddConnection(
    sptr<IRemoteObject>& token, sptr<RSIClientToRenderConnection> connectToRenderConnection)
{
    std::unique_lock<std::mutex> lock(renderConnectionMutex_);
    if (renderConnections_.find(token) != renderConnections_.end()) {
        return;
    }
    renderConnections_[token] = connectToRenderConnection;
}

sptr<RSIClientToRenderConnection> RSRenderPipeline::FindClientToRenderConnection(const sptr<IRemoteObject>& token)
{
    std::unique_lock<std::mutex> lock(renderConnectionMutex_);
    auto it = renderConnections_.find(token);
    if (it != renderConnections_.end()) {
        auto clientToRenderConnection = it->second;
        return clientToRenderConnection;
    }
    return nullptr;
}

void RSRenderPipeline::RegisterJudgeLppLayerCB(const sptr<IRSComposerToRenderConnection>& composerToRenderConn)
{
    composerToRenderConn->RegisterJudgeLppLayerCB(
        [mainThread = mainThread_](uint64_t vsyncId, const std::unordered_set<uint64_t>& lppNodeIds) {
            mainThread->GetLppVideoHander().JudgeLppLayer(vsyncId, lppNodeIds);
        });
}
} // namespace Rosen
} // namespace OHOS