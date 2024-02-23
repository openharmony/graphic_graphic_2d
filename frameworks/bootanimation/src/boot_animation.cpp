/*
 * Copyright (c) 2023 Huawei Device Co., Ltd.
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
#include "boot_animation.h"
#include "event_handler.h"
#include "rs_trace.h"
#include "transaction/rs_render_service_client.h"
#include "transaction/rs_interfaces.h"
#include "transaction/rs_transaction.h"
#ifdef NEW_RENDER_CONTEXT
#include "render_context_factory.h"
#include "render_backend/rs_surface_factory.h"
#endif
#include <display_manager.h>

using namespace OHOS;
using namespace Rosen;

constexpr float MAX_ZORDER = 100000.0f;

void BootAnimation::OnDraw(SkCanvas* canvas, int32_t curNo)
{
    if (canvas == nullptr) {
        LOGE("OnDraw canvas is nullptr");
        return;
    }
    if (curNo > (imgVecSize_ - 1) || curNo < 0) {
        return;
    }
    std::shared_ptr<ImageStruct> imgstruct = imageVector_[curNo];
    sk_sp<SkImage> image = imgstruct->imageData;

    ROSEN_TRACE_BEGIN(HITRACE_TAG_GRAPHIC_AGP, "BootAnimation::OnDraw in drawRect");
    SkPaint backPaint;
    backPaint.setColor(SK_ColorBLACK);
    canvas->drawRect(SkRect::MakeXYWH(0.0, 0.0, windowWidth_, windowHeight_), backPaint);
    ROSEN_TRACE_END(HITRACE_TAG_GRAPHIC_AGP);
    ROSEN_TRACE_BEGIN(HITRACE_TAG_GRAPHIC_AGP, "BootAnimation::OnDraw in drawImageRect");
    SkPaint paint;
    SkRect rect;
    rect.setXYWH(pointX_, pointY_, realWidth_, realHeight_);
#ifdef NEW_SKIA
    canvas->drawImageRect(image.get(), rect, SkSamplingOptions(), &paint);
#else
    canvas->drawImageRect(image.get(), rect, &paint);
#endif
    ROSEN_TRACE_END(HITRACE_TAG_GRAPHIC_AGP);
}

void BootAnimation::Draw()
{
    if (picCurNo_ < (imgVecSize_ - 1)) {
        picCurNo_ = picCurNo_ + 1;
    } else {
        CheckExitAnimation();
        return;
    }
    ROSEN_TRACE_BEGIN(HITRACE_TAG_GRAPHIC_AGP, "BootAnimation::Draw RequestFrame");
    auto frame = rsSurface_->RequestFrame(windowWidth_, windowHeight_);
    if (frame == nullptr) {
        LOGE("Draw frame is nullptr");
        return;
    }
    ROSEN_TRACE_END(HITRACE_TAG_GRAPHIC_AGP);
#ifdef NEW_RENDER_CONTEXT
    if (rsSurface_ == nullptr) {
        LOGE("rsSurface is nullptr");
        return;
    }
    auto canvas = rsSurface_->GetCanvas();
    OnDraw(canvas, picCurNo_);
    ROSEN_TRACE_BEGIN(HITRACE_TAG_GRAPHIC_AGP, "BootAnimation::Draw FlushFrame");
    rsSurface_->FlushFrame();
    ROSEN_TRACE_END(HITRACE_TAG_GRAPHIC_AGP);
#else
    framePtr_ = std::move(frame);
    auto canvas = framePtr_->GetCanvas();
    OnDraw(canvas, picCurNo_);
    ROSEN_TRACE_BEGIN(HITRACE_TAG_GRAPHIC_AGP, "BootAnimation::Draw FlushFrame");
    rsSurface_->FlushFrame(framePtr_);
    ROSEN_TRACE_END(HITRACE_TAG_GRAPHIC_AGP);
#endif
}

void BootAnimation::Init(Rosen::ScreenId defaultId, int32_t width, int32_t height)
{
    defaultId_ = defaultId;
    windowWidth_ = width;
    windowHeight_ = height;
    LOGI("Init enter, width: %{public}d, height: %{public}d", width, height);

    InitPicCoordinates();
    InitRsSurfaceNode();
    if (animationConfig_.IsBootVideoEnabled()) {
        LOGI("Init end");
        return;
    }

    auto& rsClient = OHOS::Rosen::RSInterfaces::GetInstance();
    while (receiver_ == nullptr) {
        receiver_ = rsClient.CreateVSyncReceiver("BootAnimation", mainHandler_);
    }
    VsyncError ret = receiver_->Init();
    if (ret) {
        LOGE("vsync receiver init failed: %{public}d", ret);
        PostTask(std::bind(&AppExecFwk::EventRunner::Stop, runner_));
        return;
    }
    InitRsSurface();
    ROSEN_TRACE_BEGIN(HITRACE_TAG_GRAPHIC_AGP, "BootAnimation::preload");
    if (animationConfig_.ReadPicZipFile(imageVector_, freq_)) {
        imgVecSize_ = imageVector_.size();
    } else {
        PostTask(std::bind(&AppExecFwk::EventRunner::Stop, runner_));
        return;
    }
    ROSEN_TRACE_END(HITRACE_TAG_GRAPHIC_AGP);

    OHOS::Rosen::VSyncReceiver::FrameCallback fcb = {
        .userData_ = this,
        .callback_ = std::bind(&BootAnimation::OnVsync, this),
    };
    int32_t changefreq = static_cast<int32_t>((1000.0 / freq_) / 16);
    ret = receiver_->SetVSyncRate(fcb, changefreq);
    if (ret) {
        PostTask(std::bind(&AppExecFwk::EventRunner::Stop, runner_));
        LOGE("SetVSyncRate failed: %{public}d %{public}d %{public}d", ret, freq_, changefreq);
        return;
    } else {
        LOGI("SetVSyncRate success: %{public}d %{public}d", freq_, changefreq);
    }
}

void BootAnimation::Run(Rosen::ScreenId id, int screenWidth, int screenHeight)
{
    LOGI("Run enter");
    animationConfig_.ParserCustomCfgFile();

    runner_ = AppExecFwk::EventRunner::Create(false);
    mainHandler_ = std::make_shared<AppExecFwk::EventHandler>(runner_);
    mainHandler_->PostTask(std::bind(&BootAnimation::Init, this, id, screenWidth, screenHeight));
    if (animationConfig_.IsBootVideoEnabled()) {
        mainHandler_->PostTask(std::bind(&BootAnimation::PlayVideo, this));
    } else {
        mainHandler_->PostTask(std::bind(&BootAnimation::PlaySound, this));
    }
    runner_->Run();
}

void BootAnimation::InitRsSurfaceNode()
{
    LOGI("Init RsSurfaceNode enter");
    struct Rosen::RSSurfaceNodeConfig rsSurfaceNodeConfig;
    Rosen::RSSurfaceNodeType rsSurfaceNodeType = Rosen::RSSurfaceNodeType::SELF_DRAWING_NODE;
    rsSurfaceNode_ = Rosen::RSSurfaceNode::Create(rsSurfaceNodeConfig, rsSurfaceNodeType);
    if (!rsSurfaceNode_) {
        LOGE("rsSurfaceNode_ is nullptr");
        return;
    }
    rsSurfaceNode_->SetPositionZ(MAX_ZORDER);
    rsSurfaceNode_->SetBounds({0, 0, windowWidth_, windowHeight_});
    rsSurfaceNode_->SetBackgroundColor(0xFF000000);
    rsSurfaceNode_->SetFrameGravity(Rosen::Gravity::RESIZE_ASPECT);
    rsSurfaceNode_->AttachToDisplay(defaultId_);
    OHOS::Rosen::RSTransaction::FlushImplicitTransaction();
    system::SetParameter("bootevent.bootanimation.started", "true");
    LOGI("Set bootevent.bootanimation.started true");
}

void BootAnimation::InitRsSurface()
{
#if defined(NEW_RENDER_CONTEXT)
    renderContext_ = Rosen::RenderContextBaseFactory::CreateRenderContext();
    if (renderContext_ == nullptr) {
        LOGE("Create Render Context failed");
        return;
    }
    renderContext_->Init();
    std::shared_ptr<Rosen::DrawingContext> drawingContext = std::make_shared<Rosen::DrawingContext>(
        renderContext_->GetRenderType());
    sptr<Surface> surface = rsSurfaceNode_->GetSurface();
    drawingContext->SetUpDrawingContext();
    rsSurface_ = Rosen::RSSurfaceFactory::CreateRSSurface(Rosen::PlatformName::OHOS, surface, drawingContext);
    rsSurface_->SetRenderContext(renderContext_);
#else
    rsSurface_ = OHOS::Rosen::RSSurfaceExtractor::ExtractRSSurface(rsSurfaceNode_);
    if (rsSurface_ == nullptr) {
        LOGE("rsSurface is nullptr");
        return;
    }
#ifdef ACE_ENABLE_GL
    rc_ = OHOS::Rosen::RenderContextFactory::GetInstance().CreateEngine();
    if (rc_ == nullptr) {
        LOGE("InitilizeEglContext failed");
        return;
    } else {
        LOGI("init egl context");
        rc_->InitializeEglContext();
        rsSurface_->SetRenderContext(rc_);
    }
#endif
    if (rc_ == nullptr) {
        LOGI("rc is nullptr, use cpu");
    }
#endif
}

BootAnimation::~BootAnimation()
{
    rsSurfaceNode_->DetachToDisplay(defaultId_);
    OHOS::Rosen::RSTransaction::FlushImplicitTransaction();
}

void BootAnimation::InitPicCoordinates()
{
    if (windowWidth_ >= windowHeight_) {
        realHeight_ = windowHeight_;
        realWidth_ = realHeight_;
        pointX_ = (windowWidth_ - realWidth_) / 2;
    } else {
        realWidth_ = windowWidth_;
        realHeight_ = realWidth_;
        pointY_ = (windowHeight_ - realHeight_) / 2;
    }
}

void BootAnimation::OnVsync()
{
    PostTask(std::bind(&BootAnimation::Draw, this));
}

bool BootAnimation::CheckExitAnimation()
{
    if (!isAnimationEnd_) {
        LOGI("Boot animation is end");
        isAnimationEnd_ = true;
    }
    std::string bootEventCompleted = system::GetParameter("bootevent.boot.completed", "false");
    if (bootEventCompleted == "true") {
        mainHandler_->PostTask(std::bind(&AppExecFwk::EventRunner::Stop, runner_));
        LOGI("Read bootevent.boot.completed is true");
        return true;
    }
    return false;
}

void BootAnimation::PlaySound()
{
    LOGI("PlaySound start");
    bool bootSoundEnabled = system::GetBoolParameter("persist.graphic.bootsound.enabled", true);
    if (bootSoundEnabled == true) {
        LOGI("PlaySound read bootSoundEnabled is true");
        if (soundPlayer_ == nullptr) {
            soundPlayer_ = Media::PlayerFactory::CreatePlayer();
        }
        std::string uri = animationConfig_.GetSoundUrl();
        soundPlayer_->SetSource(uri);
        soundPlayer_->SetLooping(false);
        soundPlayer_->PrepareAsync();
        soundPlayer_->Play();
    }
    LOGI("PlaySound end");
}

void BootAnimation::PlayVideo()
{
    LOGI("PlayVideo start w:%{public}d h:%{public}d", windowWidth_, windowHeight_);
    if (!animationConfig_.IsBootVideoEnabled()) {
        return;
    }
    fcb_ = {
        .userData_ = this,
        .callback_ = std::bind(&BootAnimation::CloseVideoPlayer, this),
    };
    bootVideoPlayer_ = std::make_shared<BootVideoPlayer>();
    bootVideoPlayer_->SetVideoPath(animationConfig_.GetBootVideoPath());
    bootVideoPlayer_->SetPlayerSurface(rsSurfaceNode_ ? rsSurfaceNode_->GetSurface() : nullptr);
    bootVideoPlayer_->SetCallback(&fcb_);
    if (!bootVideoPlayer_->PlayVideo()) {
        LOGE("Play video failed.");
        CloseVideoPlayer();
    }
}

void BootAnimation::CloseVideoPlayer()
{
    LOGI("Close video player.");
    while (!CheckExitAnimation()) {
        usleep(SLEEP_TIME_US);
    }
    LOGI("Check Exit Animation end.");
}
