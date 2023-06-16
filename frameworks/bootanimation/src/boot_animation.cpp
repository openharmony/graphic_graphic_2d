/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
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
#include "rs_surface_factory.h"
#endif

using namespace OHOS;

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

void BootAnimation::Init(int32_t width, int32_t height)
{
    windowWidth_ = width;
    windowHeight_ = height;
    LOGI("Init enter, width: %{public}d, height: %{public}d", width, height);

    InitPicCoordinates();
    InitBootWindow();
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

void BootAnimation::Run(std::vector<sptr<OHOS::Rosen::Display>>& displays)
{
    LOGI("Run enter");
    animationConfig_.ParserCustomCfgFile();

    runner_ = AppExecFwk::EventRunner::Create(false);
    mainHandler_ = std::make_shared<AppExecFwk::EventHandler>(runner_);
    mainHandler_->PostTask(std::bind(&BootAnimation::Init, this, displays[0]->GetWidth(), displays[0]->GetHeight()));
    if (animationConfig_.IsBootVideoEnabled()) {
        mainHandler_->PostTask(std::bind(&BootAnimation::PlayVideo, this));
    } else {
        mainHandler_->PostTask(std::bind(&BootAnimation::PlaySound, this));
    }
    runner_->Run();
}

void BootAnimation::InitBootWindow()
{
    sptr<OHOS::Rosen::WindowOption> option = new OHOS::Rosen::WindowOption();
    option->SetWindowType(OHOS::Rosen::WindowType::WINDOW_TYPE_BOOT_ANIMATION);
    option->RemoveWindowFlag(OHOS::Rosen::WindowFlag::WINDOW_FLAG_NEED_AVOID);
    option->SetTouchable(false);
    if (animationConfig_.IsBootVideoEnabled()) {
        option->SetWindowMode(Rosen::WindowMode::WINDOW_MODE_FLOATING);
    }
    option->SetWindowRect({0, 0, windowWidth_, windowHeight_} );
    int displayId = 0;
    sptr<OHOS::Rosen::IWindowLifeCycle> listener = nullptr;
    LOGI("Create window scene.");
    scene_ = new OHOS::Rosen::WindowScene();
    LOGI("Init window scene.");
    scene_->Init(displayId, nullptr, listener, option);
    window_ = scene_->GetMainWindow();
    while (window_ == nullptr) {
        LOGI("window is nullptr, continue to init window");
        scene_->Init(displayId, nullptr, listener, option);
        window_ = scene_->GetMainWindow();
        usleep(SLEEP_TIME_US);
    }
    LOGI("Init window scene end.");
    if (animationConfig_.IsBootVideoEnabled()) {
        auto surfaceNode = window_->GetSurfaceNode();
        surfaceNode->SetBackgroundColor(0xFF000000);
        surfaceNode->SetFrameGravity(Rosen::Gravity::RESIZE_ASPECT);
        OHOS::Rosen::RSTransaction::FlushImplicitTransaction();
    }
    scene_->GoForeground();
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
    sptr<Surface> surface = window_->GetSurfaceNode()->GetSurface();
    rsSurface_ = Rosen::RSSurfaceFactory::CreateRSSurface(Rosen::PlatformName::OHOS, surface, drawingContext);
    rsSurface_->SetRenderContext(renderContext_);
#else
    rsSurface_ = OHOS::Rosen::RSSurfaceExtractor::ExtractRSSurface(window_->GetSurfaceNode());
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
    window_->Destroy();
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
    if (!setBootEvent_) {
        LOGI("CheckExitAnimation set bootevent parameter");
        system::SetParameter("bootevent.bootanimation.started", "true");
        setBootEvent_ = true;
    }
    std::string windowInit = system::GetParameter("bootevent.boot.completed", "false");
    if (windowInit == "true") {
        mainHandler_->PostTask(std::bind(&AppExecFwk::EventRunner::Stop, runner_));
        LOGI("CheckExitAnimation read windowInit is true");
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
    bootVideoPlayer_->SetPlayerWindow(window_);
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
