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
#include "boot_picture_player.h"

#include "event_handler.h"
#include "log.h"
#include "rs_trace.h"
#include "transaction/rs_interfaces.h"

namespace OHOS {
namespace {
    const std::string BOOT_PIC_ZIP = "/system/etc/graphic/bootpic.zip";
}

BootPicturePlayer::BootPicturePlayer(const PlayerParams& params)
{
    resPath_ = params.resPath;
    rsSurface_ = params.rsSurface;
    InitPicCoordinates(params.screenId);
}

void BootPicturePlayer::Play()
{
    auto& rsClient = OHOS::Rosen::RSInterfaces::GetInstance();
    while (receiver_ == nullptr) {
        LOGI("receiver is nullptr, try create again");
        receiver_ = rsClient.CreateVSyncReceiver("BootAnimation", AppExecFwk::EventHandler::Current());
    }

    VsyncError ret = receiver_->Init();
    if (ret) {
        LOGE("vsync receiver init failed: %{public}d", ret);
        PostTask(std::bind(&AppExecFwk::EventRunner::Stop, AppExecFwk::EventRunner::Current()));
        return;
    }

    ROSEN_TRACE_BEGIN(HITRACE_TAG_GRAPHIC_AGP, "BootAnimation::preload");
    if (ReadPicZipFile(imageVector_, freq_)) {
        imgVecSize_ = static_cast<int32_t> (imageVector_.size());
    } else {
        LOGE("read pic zip failed");
        PostTask(std::bind(&AppExecFwk::EventRunner::Stop, AppExecFwk::EventRunner::Current()));
        return;
    }
    ROSEN_TRACE_END(HITRACE_TAG_GRAPHIC_AGP);

    OHOS::Rosen::VSyncReceiver::FrameCallback fcb = {
        .userData_ = this,
        .callback_ = std::bind(&BootPicturePlayer::OnVsync, this),
    };
    int32_t changeFreq = static_cast<int32_t> ((1000.0 / freq_) / 16);
    ret = receiver_->SetVSyncRate(fcb, changeFreq);
    if (ret) {
        LOGE("SetVSyncRate failed: %{public}d %{public}d", ret, freq_);
        PostTask(std::bind(&AppExecFwk::EventRunner::Stop, AppExecFwk::EventRunner::Current()));
        return;
    } else {
        LOGI("SetVSyncRate success: %{public}d, %{public}d", freq_, changeFreq);
    }
}

void BootPicturePlayer::InitPicCoordinates(Rosen::ScreenId screenId)
{
    Rosen::RSInterfaces& interface = Rosen::RSInterfaces::GetInstance();
    Rosen::RSScreenModeInfo modeinfo = interface.GetScreenActiveMode(screenId);
    windowWidth_ = modeinfo.GetScreenWidth();
    windowHeight_ = modeinfo.GetScreenHeight();
    if (windowWidth_ >= windowHeight_) {
        realHeight_ = windowHeight_;
        realWidth_ = realHeight_;
        pointX_ = (windowWidth_ - realWidth_) / NUMBER_TWO;
    } else {
        realWidth_ = windowWidth_;
        realHeight_ = realWidth_;
        pointY_ = (windowHeight_ - realHeight_) / NUMBER_TWO;
    }
}

bool BootPicturePlayer::ReadPicZipFile(ImageStructVec& imgVec, int32_t& freq)
{
    FrameRateConfig frameConfig;
    ReadZipFile(GetPicZipPath(), imgVec, frameConfig);
    int32_t imageNum = static_cast<int32_t>(imgVec.size());
    if (imageNum < 0) {
        LOGE("zip pic num is invalid.");
        return false;
    }

    SortZipFile(imgVec);

    if (CheckFrameRateValid(frameConfig.frameRate)) {
        freq = frameConfig.frameRate;
    } else {
        LOGW("Only Support 30, 60 frame rate: %{public}d", frameConfig.frameRate);
    }
    LOGI("read freq: %{public}d, pic num: %{public}d", freq, imageNum);
    return true;
}

std::string BootPicturePlayer::GetPicZipPath()
{
    if (!IsFileExisted(resPath_)) {
        LOGI("pic zip path empty or not exist, use default path");
        return BOOT_PIC_ZIP;
    }
    return resPath_;
}

bool BootPicturePlayer::CheckFrameRateValid(int32_t frameRate)
{
    std::vector<int> freqs = {60, 30};
    int nCount = std::count(freqs.begin(), freqs.end(), frameRate);
    if (nCount <= 0) {
        return false;
    }
    return true;
}

void BootPicturePlayer::OnVsync()
{
    PostTask(std::bind(&BootPicturePlayer::Draw, this));
}

bool BootPicturePlayer::Draw()
{
    if (picCurNo_ < (imgVecSize_ - 1)) {
        picCurNo_ = picCurNo_ + 1;
    } else {
        LOGI("play sequence frames end");
        PostTask(std::bind(&AppExecFwk::EventRunner::Stop, AppExecFwk::EventRunner::Current()));
        return false;
    }
    ROSEN_TRACE_BEGIN(HITRACE_TAG_GRAPHIC_AGP, "BootAnimation::Draw RequestFrame");
    auto frame = rsSurface_->RequestFrame(windowWidth_, windowHeight_);
    ROSEN_TRACE_END(HITRACE_TAG_GRAPHIC_AGP);
    if (frame == nullptr) {
        LOGE("draw frame is nullptr");
        PostTask(std::bind(&AppExecFwk::EventRunner::Stop, AppExecFwk::EventRunner::Current()));
        return false;
    }
#ifdef NEW_RENDER_CONTEXT
    if (rsSurface_ == nullptr) {
        LOGE("rsSurface is nullptr");
        PostTask(std::bind(&AppExecFwk::EventRunner::Stop, AppExecFwk::EventRunner::Current()));
        return false;
    }
    auto canvas = rsSurface_->GetCanvas();
    OnDraw(canvas, picCurNo_);
    ROSEN_TRACE_BEGIN(HITRACE_TAG_GRAPHIC_AGP, "BootAnimation::Draw FlushFrame");
    rsSurface_->FlushFrame();
    ROSEN_TRACE_END(HITRACE_TAG_GRAPHIC_AGP);
#else
    rsSurfaceFrame_ = std::move(frame);
    auto canvas = rsSurfaceFrame_->GetCanvas();
    OnDraw(canvas, picCurNo_);
    ROSEN_TRACE_BEGIN(HITRACE_TAG_GRAPHIC_AGP, "BootAnimation::Draw FlushFrame");
    rsSurface_->FlushFrame(rsSurfaceFrame_);
    ROSEN_TRACE_END(HITRACE_TAG_GRAPHIC_AGP);
#endif
    return true;
}

bool BootPicturePlayer::OnDraw(Rosen::Drawing::CoreCanvas* canvas, int32_t curNo)
{
    if (canvas == nullptr) {
        LOGE("OnDraw canvas is nullptr");
        PostTask(std::bind(&AppExecFwk::EventRunner::Stop, AppExecFwk::EventRunner::Current()));
        return false;
    }
    if (curNo > (imgVecSize_ - 1) || curNo < 0) {
        PostTask(std::bind(&AppExecFwk::EventRunner::Stop, AppExecFwk::EventRunner::Current()));
        return false;
    }
    std::shared_ptr<ImageStruct> imgstruct = imageVector_[curNo];
    std::shared_ptr<Rosen::Drawing::Image> image = imgstruct->imageData;

    ROSEN_TRACE_BEGIN(HITRACE_TAG_GRAPHIC_AGP, "BootAnimation::OnDraw in drawRect");
    Rosen::Drawing::Brush brush;
    brush.SetColor(Rosen::Drawing::Color::COLOR_BLACK);
    Rosen::Drawing::Rect bgRect(0.0, 0.0, windowWidth_, windowHeight_);
    canvas->AttachBrush(brush);
    canvas->DrawRect(bgRect);
    canvas->DetachBrush();
    ROSEN_TRACE_END(HITRACE_TAG_GRAPHIC_AGP);
    ROSEN_TRACE_BEGIN(HITRACE_TAG_GRAPHIC_AGP, "BootAnimation::OnDraw in drawImageRect");
    Rosen::Drawing::Rect rect(pointX_, pointY_, pointX_ + realWidth_, pointY_ + realHeight_);
    Rosen::Drawing::SamplingOptions samplingOptions;
    canvas->DrawImageRect(*image, rect, samplingOptions);
    ROSEN_TRACE_END(HITRACE_TAG_GRAPHIC_AGP);
    return true;
}
} // namespace OHOS
