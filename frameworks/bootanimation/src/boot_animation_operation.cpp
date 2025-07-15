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
#include "ui/rs_ui_context.h"

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
    LOGI("Release RsNode");
}

void BootAnimationOperation::Init(const BootAnimationConfig& config, int32_t width, int32_t height, int32_t duration)
{
    LOGI("Init enter, width: %{public}d, height: %{public}d, screenId : " BPUBU64 "", width, height, config.screenId);
    currentScreenId_ = config.screenId;
    windowWidth_ = width;
    windowHeight_ = height;
    duration_ = duration * DELAY_TIME_MS;

    eventThread_ = std::thread([this, &config] { this->StartEventHandler(config); });
}

void BootAnimationOperation::StartEventHandler(const BootAnimationConfig& config)
{
    LOGI("StartEventHandler");
    runner_ = AppExecFwk::EventRunner::Create(false);
    mainHandler_ = std::make_shared<AppExecFwk::EventHandler>(runner_);
    mainHandler_->PostTask([this] { this->InitRsDisplayNode(); });
    mainHandler_->PostTask([this, &config] { this->InitRsSurfaceNode(config.rotateDegree); });
    mainHandler_->PostTask([this] { this->StopBootAnimation(); }, duration_);
#ifdef PLAYER_FRAMEWORK_ENABLE
    if (IsBootVideoEnabled(config)) {
        mainHandler_->PostTask([this, &config] { this->PlayVideo(config.videoDefaultPath); });
        runner_->Run();
        LOGI("runner run has ended.");
        return;
    } else {
        mainHandler_->PostTask([this, &config] { this->PlaySound(config.soundPath); });
    }
#else
    LOGI("player framework is disabled");
#endif
    mainHandler_->PostTask([this, &config] { this->PlayPicture(config.picZipPath); });
    runner_->Run();
    LOGI("runner run has ended.");
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
    OHOS::Rosen::RSDisplayNodeConfig config = { currentScreenId_ };
    rsUIDirector_ = OHOS::Rosen::RSUIDirector::Create();
    rsUIDirector_->Init(false, false);
    auto rsUIContext = rsUIDirector_->GetRSUIContext();
    rsDisplayNode_ = OHOS::Rosen::RSDisplayNode::Create(config, rsUIContext);
    if (rsDisplayNode_ == nullptr) {
        LOGE("init display node failed");
        return false;
    }
    rsDisplayNode_->SetFrame(0, 0, windowWidth_, windowHeight_);
    rsDisplayNode_->SetBounds(0, 0, windowWidth_, windowHeight_);
    rsDisplayNode_->SetBootAnimation(true);
    rsDisplayNode_->SetBackgroundColor(0xFF000000);
    if (rsUIContext != nullptr) {
        auto transaction = rsUIContext->GetRSTransaction();
        if (transaction == nullptr) {
            LOGE("transaction is nullptr");
            return false;
        }
        transaction->FlushImplicitTransaction();
    } else {
        auto transactionProxy = OHOS::Rosen::RSTransactionProxy::GetInstance();
        if (transactionProxy == nullptr) {
            LOGE("transactionProxy is nullptr");
            return false;
        }
        transactionProxy->FlushImplicitTransaction();
    }
    return true;
}

bool BootAnimationOperation::InitRsSurfaceNode(int32_t degree)
{
    LOGI("InitRsSurfaceNode start");
    struct Rosen::RSSurfaceNodeConfig rsSurfaceNodeConfig;
    rsSurfaceNodeConfig.SurfaceNodeName =
        currentScreenId_ == 0 ? "BootAnimationNode" : "BootAnimationNodeExtra";
    rsSurfaceNodeConfig.isSync = false;
    Rosen::RSSurfaceNodeType rsSurfaceNodeType = Rosen::RSSurfaceNodeType::SELF_DRAWING_WINDOW_NODE;
    auto rsUIContext = rsUIDirector_->GetRSUIContext();
    rsSurfaceNode_ = Rosen::RSSurfaceNode::Create(rsSurfaceNodeConfig, rsSurfaceNodeType, true, false, rsUIContext);
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
        .callback = [this](void*) { this->StopBootAnimation(); },
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

    if (!InitRsSurface()) {
        runner_->Stop();
        return;
    }
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
    return true;
}

bool BootAnimationOperation::IsBootVideoEnabled(const BootAnimationConfig& config)
{
    if (config.videoDefaultPath.empty() && !config.picZipPath.empty()) {
        LOGI("video path is empty and picture path is not empty");
        return false;
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
    runner_->Stop();
    LOGI("runner has called stop.");
    mainHandler_ = nullptr;
}
