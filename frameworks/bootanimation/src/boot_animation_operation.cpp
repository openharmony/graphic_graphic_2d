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

#include "boot_animation_operation.h"

#include "boot_picture_player.h"
#include "boot_sound_player.h"
#include "boot_video_player.h"
#include "log.h"
#include <parameters.h>
#include "platform/common/rs_system_properties.h"
#include "transaction/rs_transaction.h"
#include "transaction/rs_interfaces.h"

using namespace OHOS;
static const int DELAY_TIME_MS = 1000;

BootAnimationOperation::~BootAnimationOperation()
{
    if (rsSurfaceNode_) {
        rsSurfaceNode_->DetachToDisplay(currentScreenId_);
    }
    if (rsDisplayNode_) {
        rsDisplayNode_->RemoveFromTree();
    }
    OHOS::Rosen::RSTransaction::FlushImplicitTransaction();
}

void BootAnimationOperation::Init(const BootAnimationConfig& config, const int32_t width,
    const int32_t height, const int32_t duration)
{
    LOGI("Init enter, width: %{public}d, height: %{public}d, screenId : " BPUBU64 "", width, height, config.screenId);
    currentScreenId_ = config.screenId;
    windowWidth_ = width;
    windowHeight_ = height;
    duration_ = duration * DELAY_TIME_MS;

    eventThread_ = std::thread(&BootAnimationOperation::StartEventHandler, this, config);
    std::unique_lock<std::mutex> lock(eventMutex_);
    eventCon_.wait(lock, [this] {
        return mainHandler_ != nullptr;
    });
}

void BootAnimationOperation::StartEventHandler(const BootAnimationConfig& config)
{
    LOGI("StartEventHandler");
    runner_ = AppExecFwk::EventRunner::Create(false);
    mainHandler_ = std::make_shared<AppExecFwk::EventHandler>(runner_);
    mainHandler_->PostTask(std::bind(&BootAnimationOperation::InitRsDisplayNode, this));
    mainHandler_->PostTask(std::bind(&BootAnimationOperation::InitRsSurfaceNode, this, config.rotateDegree));
    mainHandler_->PostTask(std::bind(&BootAnimationOperation::StopBootAnimation, this), duration_);
    eventCon_.notify_all();
#ifdef PLAYER_FRAMEWORK_ENABLE
    if (IsBootVideoEnabled(config)) {
        mainHandler_->PostTask(std::bind(&BootAnimationOperation::PlayVideo, this, config.videoDefaultPath));
        runner_->Run();
        return;
    } else {
        mainHandler_->PostTask(std::bind(
            &BootAnimationOperation::PlaySound, this, config.soundPath));
    }
#else
    LOGI("player framework is disabled");
#endif
    mainHandler_->PostTask(
        std::bind(&BootAnimationOperation::PlayPicture, this, config.picZipPath));
    runner_->Run();
}

void BootAnimationOperation::SetSoundEnable(bool isEnabled)
{
    isSoundEnabled_ = isEnabled;
}

std::thread& BootAnimationOperation::GetThread()
{
    return eventThread_;
}

bool BootAnimationOperation::InitRsDisplayNode()
{
    LOGI("InitRsDisplayNode start");
    OHOS::Rosen::RSDisplayNodeConfig config = {currentScreenId_, false, 0};

    rsDisplayNode_ = OHOS::Rosen::RSDisplayNode::Create(config);
    if (rsDisplayNode_ == nullptr) {
        LOGE("init display node failed");
        return false;
    }
    rsDisplayNode_->SetDisplayOffset(0, 0);
    rsDisplayNode_->SetFrame(0, 0, windowWidth_, windowHeight_);
    rsDisplayNode_->SetBounds(0, 0, windowWidth_, windowHeight_);
    rsDisplayNode_->SetBootAnimation(true);
    // flush transaction
    auto transactionProxy = OHOS::Rosen::RSTransactionProxy::GetInstance();
    if (transactionProxy == nullptr) {
        LOGE("transactionProxy is nullptr");
        return false;
    }
    transactionProxy->FlushImplicitTransaction();
    return true;
}

bool BootAnimationOperation::InitRsSurfaceNode(const int32_t degree)
{
    LOGI("InitRsSurfaceNode start");
    struct Rosen::RSSurfaceNodeConfig rsSurfaceNodeConfig;
    rsSurfaceNodeConfig.SurfaceNodeName =
        currentScreenId_ == 0 ? "BootAnimationNode" : "BootAnimationNodeExtra";
    rsSurfaceNodeConfig.isSync = true;
    Rosen::RSSurfaceNodeType rsSurfaceNodeType = Rosen::RSSurfaceNodeType::SELF_DRAWING_WINDOW_NODE;
    rsSurfaceNode_ = Rosen::RSSurfaceNode::Create(rsSurfaceNodeConfig, rsSurfaceNodeType);
    if (!rsSurfaceNode_) {
        LOGE("create rsSurfaceNode failed");
        return false;
    }
    LOGI("rotation degree: %{public}d", degree);
    rsSurfaceNode_->SetRotation(degree);
    rsSurfaceNode_->SetPositionZ(MAX_ZORDER);
    rsSurfaceNode_->SetBounds({0, 0, windowWidth_, windowHeight_});
    rsSurfaceNode_->SetBackgroundColor(0xFF000000);
    rsSurfaceNode_->SetFrameGravity(Rosen::Gravity::RESIZE_ASPECT);
    rsSurfaceNode_->SetBootAnimation(true);
    OHOS::Rosen::RSTransaction::FlushImplicitTransaction();
    rsSurfaceNode_->AttachToDisplay(currentScreenId_);
    OHOS::Rosen::RSTransaction::FlushImplicitTransaction();
    if (!system::GetBoolParameter(BOOT_ANIMATION_READY, false)) {
        system::SetParameter(BOOT_ANIMATION_READY, "true");
        LOGI("set boot animation ready true");
    }
    return true;
}

void BootAnimationOperation::PlayVideo(const std::string& path)
{
    LOGI("boot animation play video");
    PlayerParams params;
#ifdef PLAYER_FRAMEWORK_ENABLE
    params.surface = rsSurfaceNode_ ? rsSurfaceNode_->GetSurface() : nullptr;
#endif
    params.resPath = path;
    callback_ = {
        .userData = this,
        .callback = std::bind(&BootAnimationOperation::StopBootAnimation, this),
    };
    params.callback = &callback_;
    params.screenId = currentScreenId_;
    params.soundEnabled = isSoundEnabled_;
    videoPlayer_ = std::make_shared<BootVideoPlayer>(params);
    videoPlayer_->Play();
}

void BootAnimationOperation::PlayPicture(const std::string& path)
{
    LOGI("boot animation play sequence frames");
    if (!system::GetBoolParameter(BOOT_ANIMATION_STARTED, false)) {
        system::SetParameter(BOOT_ANIMATION_STARTED, "true");
        LOGI("set boot animation started true");
    }

    InitRsSurface();
    PlayerParams params;
    params.screenId = currentScreenId_;
    params.rsSurface = rsSurface_;
    params.resPath = path;
    picPlayer_ = std::make_shared<BootPicturePlayer>(params);
    picPlayer_->Play();
}

void BootAnimationOperation::PlaySound(const std::string& path)
{
    LOGI("boot animation play sound");
    PlayerParams params;
    params.resPath = path;
    params.screenId = currentScreenId_;
    params.soundEnabled = isSoundEnabled_;
    soundPlayer_ = std::make_shared<BootSoundPlayer>(params);
    soundPlayer_->Play();
}

bool BootAnimationOperation::InitRsSurface()
{
    LOGI("InitRsSurface start");
#ifdef NEW_RENDER_CONTEXT
    std::shared_ptr<OHOS::Rosen::RenderContextBase> renderContext =
        Rosen::RenderContextBaseFactory::CreateRenderContext();
    if (renderContext == nullptr) {
        LOGE("create render context failed");
        return false;
    }
    renderContext->Init();
    std::shared_ptr<Rosen::DrawingContext> drawingContext = std::make_shared<Rosen::DrawingContext>(
        renderContext->GetRenderType());
    sptr<Surface> surface = rsSurfaceNode_->GetSurface();
    drawingContext->SetUpDrawingContext();
    rsSurface_ = Rosen::RSSurfaceFactory::CreateRSSurface(Rosen::PlatformName::OHOS, surface, drawingContext);
    rsSurface_->SetRenderContext(renderContext);
#else
    rsSurface_ = OHOS::Rosen::RSSurfaceExtractor::ExtractRSSurface(rsSurfaceNode_);
    if (rsSurface_ == nullptr) {
        LOGE("rsSurface is nullptr");
        return false;
    }
#ifdef ACE_ENABLE_GL
    LOGI("init egl context start");
    if (Rosen::RSSystemProperties::GetGpuApiType() == OHOS::Rosen::GpuApiType::OPENGL) {
        OHOS::Rosen::RenderContext* rc = OHOS::Rosen::RenderContextFactory::GetInstance().CreateEngine();
        if (rc == nullptr) {
            LOGE("init egl context failed");
            return false;
        } else {
            LOGI("init egl context success");
            rc->InitializeEglContext();
            rsSurface_->SetRenderContext(rc);
        }
    }
#endif // ACE_ENABLE_GL
#endif // NEW_RENDER_CONTEXT
    return true;
}

bool BootAnimationOperation::IsBootVideoEnabled(const BootAnimationConfig& config)
{
    if (config.videoDefaultPath.empty() && !config.picZipPath.empty()) {
        LOGI("video path is empty and picture path is not empty");
        return false;
    }

    if (!IsFileExisted(config.videoDefaultPath)) {
        LOGE("video file not found");
    }
    return true;
}

void BootAnimationOperation::StopBootAnimation()
{
    LOGI("StopBootAnimation");
    if (!system::GetBoolParameter(BOOT_ANIMATION_STARTED, false)) {
        system::SetParameter(BOOT_ANIMATION_STARTED, "true");
        LOGI("set boot animation started true");
    }
    mainHandler_->PostTask(std::bind(&AppExecFwk::EventRunner::Stop, runner_));
}
