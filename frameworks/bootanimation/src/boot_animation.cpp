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
#include "util.h"
#include "rs_trace.h"
#include "transaction/rs_render_service_client.h"
#include "transaction/rs_interfaces.h"

using namespace OHOS;
static const std::string BOOT_PIC_ZIP = "/system/etc/init/bootpic.zip";
static const std::string DST_FILE_PATH = "/data/media/bootpic";
static const std::string BOOT_PIC_DIR = "/data/media/bootpic/OpenHarmony_";
static const int32_t EXIT_TIME = 10 * 1000;

void BootAnimation::OnDraw(SkCanvas* canvas)
{
    ROSEN_TRACE_BEGIN(BYTRACE_TAG_GRAPHIC_AGP, "BootAnimation::OnDraw before codec");
    std::string imgPath = BOOT_PIC_DIR + std::to_string(bootPicCurNo_) + ".jpg";
    // pic is named from 0
    if (bootPicCurNo_ != (maxPicNum_ - 1)) {
        bootPicCurNo_ = bootPicCurNo_ + 1;
    }
    char newpath[PATH_MAX + 1] = { 0x00 };
    if (strlen(imgPath.c_str()) > PATH_MAX || realpath(imgPath.c_str(), newpath) == NULL) {
        LOG("OnDraw imgPath is invalid");
        return;
    }
    FILE *fp = fopen(newpath, "rb");
    if (fp == nullptr) {
        LOG("OnDraw fopen image file is nullptr");
        return;
    }
    std::unique_ptr<FILE, decltype(&fclose)> file(fp, fclose);
    if (file == nullptr) {
        LOG("OnDraw file is nullptr");
        return;
    }
    ROSEN_TRACE_END(BYTRACE_TAG_GRAPHIC_AGP);
    ROSEN_TRACE_BEGIN(BYTRACE_TAG_GRAPHIC_AGP, "BootAnimation::OnDraw in codec");
    auto skData = SkData::MakeFromFILE(file.get());
    if (!skData) {
        LOG("skdata memory data is null. update data failed");
        return;
    }
    auto codec = SkCodec::MakeFromData(skData);
    sk_sp<SkImage> image = SkImage::MakeFromEncoded(skData);
    ROSEN_TRACE_END(BYTRACE_TAG_GRAPHIC_AGP);
    ROSEN_TRACE_BEGIN(BYTRACE_TAG_GRAPHIC_AGP, "BootAnimation::OnDraw in drawimage");
    SkPaint backPaint;
    backPaint.setColor(SK_ColorBLACK);
    canvas->drawRect(SkRect::MakeXYWH(0.0, 0.0, windowWidth_, windowHeight_), backPaint);
    SkPaint paint;
    SkRect rect;
    rect.setXYWH(pointX_, pointY_, realWidth_, realHeight_);
    canvas->drawImageRect(image.get(), rect, &paint);
    ROSEN_TRACE_END(BYTRACE_TAG_GRAPHIC_AGP);

    ROSEN_TRACE_BEGIN(BYTRACE_TAG_GRAPHIC_AGP, "BootAnimation::OnDraw in FlushFrame");
    rsSurface_->FlushFrame(framePtr_);
    ROSEN_TRACE_END(BYTRACE_TAG_GRAPHIC_AGP);
}

void BootAnimation::Draw()
{
    ROSEN_TRACE_BEGIN(BYTRACE_TAG_GRAPHIC_AGP, "BootAnimation::Draw");
    auto frame = rsSurface_->RequestFrame(windowWidth_, windowHeight_);
    if (frame == nullptr) {
        LOG("OnDraw frame is nullptr");
        RequestNextVsync();
        return;
    }
    framePtr_ = std::move(frame);
    auto canvas = framePtr_->GetCanvas();
    ROSEN_TRACE_END(BYTRACE_TAG_GRAPHIC_AGP);
    OnDraw(canvas);
    RequestNextVsync();
}

void BootAnimation::Init(int32_t width, int32_t height, const std::shared_ptr<AppExecFwk::EventHandler>& handler)
{
    ROSEN_TRACE_BEGIN(BYTRACE_TAG_GRAPHIC_AGP, "BootAnimation::Init");
    windowWidth_ = width;
    windowHeight_ = height;
    LOG("Init enter, width: %{public}d, height: %{public}d", width, height);

    auto& rsClient = OHOS::Rosen::RSInterfaces::GetInstance();
    while (receiver_ == nullptr) {
        receiver_ = rsClient.CreateVSyncReceiver("BootAnimation", handler);
    }
    receiver_->Init();

    InitBootWindow();
    InitRsSurface();
    InitPicCoordinates();

    std::vector<int32_t> freqs = {60, 30};
    if (freqs.size() >= 0x2) {
        freq_ = freqs[0];
    }

    LOG("ready to unzip pics, freq is %{public}d", freq_);
    UnzipFile(BOOT_PIC_ZIP, DST_FILE_PATH);
    CountPicNum(DST_FILE_PATH.c_str(), maxPicNum_);
    LOG("unzip pics finish, maxPicNum: %{public}d", maxPicNum_);
    ROSEN_TRACE_END(BYTRACE_TAG_GRAPHIC_AGP);

    Draw();
    PostTask(std::bind(&BootAnimation::CheckExitAnimation, this), EXIT_TIME);
}

void BootAnimation::InitBootWindow()
{
    sptr<OHOS::Rosen::WindowOption> option = new OHOS::Rosen::WindowOption();
    option->SetWindowType(OHOS::Rosen::WindowType::WINDOW_TYPE_BOOT_ANIMATION);
    option->RemoveWindowFlag(OHOS::Rosen::WindowFlag::WINDOW_FLAG_NEED_AVOID);
    option->SetWindowRect({0, 0, windowWidth_, windowHeight_} );
    int displayId = 0;
    sptr<OHOS::Rosen::IWindowLifeCycle> listener = nullptr;
    scene_ = new OHOS::Rosen::WindowScene();
    scene_->Init(displayId, nullptr, listener, option);
    window_ = scene_->GetMainWindow();
    while (window_ == nullptr) {
        LOG("window is nullptr, continue to init window");
        scene_->Init(displayId, nullptr, listener, option);
        window_ = scene_->GetMainWindow();
        sleep(1);
    }
    scene_->GoForeground();
}

void BootAnimation::InitRsSurface()
{
    rsSurface_ = OHOS::Rosen::RSSurfaceExtractor::ExtractRSSurface(window_->GetSurfaceNode());
    if (rsSurface_ == nullptr) {
        LOG("rsSurface is nullptr");
        return;
    }
#ifdef ACE_ENABLE_GL
    rc_ = OHOS::Rosen::RenderContextFactory::GetInstance().CreateEngine();
    if (rc_ == nullptr) {
        LOG("InitilizeEglContext failed");
        return;
    } else {
        LOG("init egl context");
        rc_->InitializeEglContext();
        rsSurface_->SetRenderContext(rc_);
    }
#endif
    if (rc_ == nullptr) {
        LOG("rc is nullptr, use cpu");
    }
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

void BootAnimation::RequestNextVsync()
{
    if (needCheckExit) {
        CheckExitAnimation();
    }

    OHOS::Rosen::VSyncReceiver::FrameCallback fcb = {
        .userData_ = this,
        .callback_ = std::bind(&BootAnimation::Draw, this),
    };
    receiver_->RequestNextVSync(fcb);
}

void BootAnimation::CheckExitAnimation()
{
    LOG("CheckExitAnimation enter");
    std::string windowInit = system::GetParameter("persist.window.boot.inited", "0");
    if (windowInit == "1") {
        LOG("CheckExitAnimation read windowInit is 1");
        window_->Destroy();
        int delRet = RemoveDir(DST_FILE_PATH.c_str());
        LOG("clean resources and exit animation, delRet: %{public}d", delRet);
        exit(0);
    }
    needCheckExit = true;
}

