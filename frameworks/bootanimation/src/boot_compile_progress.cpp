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

#include "boot_compile_progress.h"

#include <chrono>
#include "event_handler.h"
#include "parameter.h"
#include "parameters.h"
#include "platform/common/rs_system_properties.h"
#include "recording/recording_canvas.h"
#include "animation/rs_animation_common.h"
#include "animation/rs_cubic_bezier_interpolator.h"
#include "animation/rs_interpolator.h"
#include "transaction/rs_interfaces.h"
#include "transaction/rs_render_service_client.h"
#include "transaction/rs_transaction.h"
#include "ui/rs_ui_context_manager.h"
#include "util.h"

namespace OHOS {
namespace {
    constexpr const char* OTA_COMPILE_TIME_LIMIT = "persist.bms.optimizing_apps.timing";
    constexpr int32_t OTA_COMPILE_TIME_LIMIT_DEFAULT = 4 * 60;
    constexpr const char* OTA_COMPILE_DISPLAY_INFO = "const.bms.optimizing_apps.display_info";
    const std::string BOOTEVENT_BMS_MAIN_BUNDLES_READY = "bootevent.bms.main.bundles.ready";
    const std::string OTA_COMPILE_DISPLAY_INFO_DEFAULT = "正在优化应用";
    const std::string OTA_COMPILE_DISPLAY_INFO_OVERSEA = "Optimizing Apps";
    constexpr const char* REGION_PARA_STR_CUST = "const.cust.region";
    constexpr const char* CHINA_REGION = "cn";
    constexpr const int32_t ONE_HUNDRED_PERCENT = 100;
    constexpr const int32_t SEC_MS = 1000;
    constexpr const int32_t CIRCLE_NUM = 3;
    constexpr const float RADIUS = 3.0f;
    constexpr const float RADIUS_WEARABLE = 5.0f;
    constexpr const float OFFSET_Y_PERCENT = 0.2f;
    constexpr const float HEIGHT_PERCENT = 0.05f;
    constexpr const int TEXT_BLOB_OFFSET = 5;
    constexpr const int FONT_SIZE_PHONE = 12;
    constexpr const int FONT_SIZE_OTHER = 24;
    constexpr const int FONT_SIZE_WEARABLE = 24;
    constexpr const int OFFSET_Y_WEARABLE = 120;
    constexpr const int MAGIN_WEARABLE = 12;
    constexpr const int HEIGHT_WEARABLE = FONT_SIZE_WEARABLE + RADIUS_WEARABLE * 2 + MAGIN_WEARABLE;
    constexpr const int32_t MAX_RETRY_TIMES = 5;
    constexpr const int32_t WAIT_MS = 500;
    constexpr const int32_t LOADING_FPS = 30;
    constexpr const int32_t PERIOD_FPS = 10;
    constexpr const int32_t CHANGE_FREQ = 4;
    constexpr const float TEN_FRAME_TIMES = 10.0f;
    constexpr const float SHARP_CURVE_CTLX1 = 0.33f;
    constexpr const float SHARP_CURVE_CTLY1 = 0.0f;
    constexpr const float SHARP_CURVE_CTLX2 = 0.67f;
    constexpr const float SHARP_CURVE_CTLY2 = 1.0f;
    constexpr const float OPACITY_ARR[3][3][2] = {
        { { 0.2f, 1.0f }, { 1.0f, 0.5f }, { 0.5f, 0.2f } },
        { { 0.5f, 0.2f }, { 0.2f, 1.0f }, { 1.0f, 0.5f } },
        { { 1.0f, 0.5f }, { 0.5f, 0.2f }, { 0.2f, 1.0f } },
    };
    constexpr const char EXPAND = '0';
    constexpr const char FOLD_1 = '1';
    constexpr const char FOLD_2 = '3';
    constexpr const char MIDDLE = '2';
    constexpr const float DEFAULT_SIZE_RATIO = 1.0f;
    constexpr const float EXPAND_MIDDLE_SIZE_RATIO = 2.0/3.0f;
    constexpr const float FOLD_SIZE_RATIO = 1.0/3.0f;
    constexpr const float FOLD_X_OFFSET = 1.0/3.0f;
    constexpr const float MIDDLE_X_OFFSET = 1.0/6.0f; 
    constexpr const float DEGREE = 90.0f;
}

void BootCompileProgress::Init(const BootAnimationConfig& config)
{
    LOGI("ota compile, screenId: " BPUBU64 "", config.screenId);
    RecordDeviceType();
    screenId_ = config.screenId;
    rotateDegree_ = config.rotateDegree;
    if (!config.screenStatus.empty()) {
        screenStatus_ = config.screenStatus;
    }
    Rosen::RSInterfaces& interface = Rosen::RSInterfaces::GetInstance();
    Rosen::RSScreenModeInfo modeInfo = interface.GetScreenActiveMode(config.screenId);
    windowWidth_ = modeInfo.GetScreenWidth();
    windowHeight_ = modeInfo.GetScreenHeight();
    fontSize_ = TranslateVp2Pixel(std::min(windowWidth_, windowHeight_), isOther_ ? FONT_SIZE_OTHER : FONT_SIZE_PHONE);
    currentRadius_ = isWearable_ ? RADIUS_WEARABLE :
        TranslateVp2Pixel(std::min(windowWidth_, windowHeight_), isOther_ ? RADIUS * 2 : RADIUS);

    timeLimitSec_ = system::GetIntParameter<int32_t>(OTA_COMPILE_TIME_LIMIT, OTA_COMPILE_TIME_LIMIT_DEFAULT);
    tf_ = Rosen::Drawing::Typeface::MakeFromName("HarmonyOS Sans SC", Rosen::Drawing::FontStyle());

    std::string defaultDisplayInfo = OTA_COMPILE_DISPLAY_INFO_DEFAULT;
    if (system::GetParameter(REGION_PARA_STR_CUST, CHINA_REGION) != CHINA_REGION) {
        defaultDisplayInfo = OTA_COMPILE_DISPLAY_INFO_OVERSEA;
    }
    displayInfo_ = system::GetParameter(OTA_COMPILE_DISPLAY_INFO, defaultDisplayInfo);
    sharpCurve_ = std::make_shared<Rosen::RSCubicBezierInterpolator>(
        SHARP_CURVE_CTLX1, SHARP_CURVE_CTLY1, SHARP_CURVE_CTLX2, SHARP_CURVE_CTLY2);
    compileRunner_ = AppExecFwk::EventRunner::Create(false);
    compileHandler_ = std::make_shared<AppExecFwk::EventHandler>(compileRunner_);
    compileHandler_->PostTask([this] { this->CreateCanvasNode(); });
    compileHandler_->PostTask([this] { this->RegisterVsyncCallback(); });
    compileRunner_->Run();
}

bool BootCompileProgress::CreateCanvasNode()
{
    struct Rosen::RSSurfaceNodeConfig surfaceNodeConfig;
    surfaceNodeConfig.SurfaceNodeName = "BootCompileProgressNode";
    surfaceNodeConfig.isSync = true;
    Rosen::RSSurfaceNodeType surfaceNodeType = Rosen::RSSurfaceNodeType::SELF_DRAWING_WINDOW_NODE;
    rsUIDirector_ = OHOS::Rosen::RSUIDirector::Create();
    rsUIDirector_->Init(false, false);
    auto rsUIContext = rsUIDirector_->GetRSUIContext();
    rsSurfaceNode_ = Rosen::RSSurfaceNode::Create(surfaceNodeConfig, surfaceNodeType, true, false, rsUIContext);
    if (!rsSurfaceNode_) {
        LOGE("ota compile, SFNode create failed");
        compileRunner_->Stop();
        return false;
    }
    float positionZ = MAX_ZORDER + 1;
    rsSurfaceNode_->SetRotation(rotateDegree_);
    rsSurfaceNode_->SetPositionZ(positionZ);
    rsSurfaceNode_->SetBounds(0, 0, windowWidth_, windowHeight_);
    rsSurfaceNode_->SetBackgroundColor(SK_ColorTRANSPARENT);
    rsSurfaceNode_->SetFrameGravity(Rosen::Gravity::RESIZE_ASPECT);
    rsSurfaceNode_->SetBootAnimation(true);
    Rosen::RSTransaction::FlushImplicitTransaction();
    rsSurfaceNode_->AttachToDisplay(screenId_);
    Rosen::RSTransaction::FlushImplicitTransaction();

    rsCanvasNode_ = Rosen::RSCanvasNode::Create(true, false, rsUIContext);
    rsCanvasNode_->SetBounds(0, 0, windowWidth_, windowHeight_);
    SetFrame();
    rsCanvasNode_->SetBackgroundColor(Rosen::Drawing::Color::COLOR_TRANSPARENT);
    rsCanvasNode_->SetPositionZ(positionZ);
    rsSurfaceNode_->AddChild(rsCanvasNode_, 0);

    LOGI("CreateCanvasNode success");
    return true;
}

bool BootCompileProgress::RegisterVsyncCallback()
{
    if (IsBmsBundleReady()) {
        LOGI("bms bundle is already done.");
        compileRunner_->Stop();
        return false;
    }

    auto& rsClient = Rosen::RSInterfaces::GetInstance();
    int32_t retry = 0;
    while (receiver_ == nullptr) {
        if (retry++ > MAX_RETRY_TIMES) {
            LOGE("get vsync receiver failed");
            compileRunner_->Stop();
            return false;
        }
        if (retry > 1) {
            std::this_thread::sleep_for(std::chrono::milliseconds(WAIT_MS));
        }
        receiver_ = rsClient.CreateVSyncReceiver("BootCompileProgress", compileHandler_);
    }
    VsyncError ret = receiver_->Init();
    if (ret) {
        compileRunner_->Stop();
        LOGE("init vsync receiver failed");
        return false;
    }

    Rosen::VSyncReceiver::FrameCallback fcb = {
        .userData_ = this,
        .callback_ = [this](int64_t, void*) { this->OnVsync(); },
    };
    ret = receiver_->SetVSyncRate(fcb, CHANGE_FREQ);
    if (ret) {
        compileRunner_->Stop();
        LOGE("set vsync rate failed");
    }

    startTimeMs_ = GetSystemCurrentTime();
    endTimePredictMs_ = startTimeMs_ + timeLimitSec_ * SEC_MS;

    LOGI("RegisterVsyncCallback success");
    return true;
}

bool BootCompileProgress::IsBmsBundleReady()
{
    return system::GetBoolParameter(BOOTEVENT_BMS_MAIN_BUNDLES_READY, false);
}

void BootCompileProgress::OnVsync()
{
    if (!isUpdateOptEnd_) {
        compileHandler_->PostTask([this] { this->DrawCompileProgress(); });
    } else {
        LOGI("ota compile completed");
        compileRunner_->Stop();
    }
}

void BootCompileProgress::DrawCompileProgress()
{
    UpdateCompileProgress();

    auto canvas = static_cast<Rosen::Drawing::RecordingCanvas*>(rsCanvasNode_->BeginRecording(windowWidth_,
        isWearable_ ? HEIGHT_WEARABLE : std::max(windowWidth_, windowHeight_) * HEIGHT_PERCENT));
    if (canvas == nullptr) {
        LOGE("DrawCompileProgress canvas is null");
        return;
    }

    Rosen::Drawing::Font font;
    font.SetTypeface(tf_);
    font.SetSize(isWearable_ ? FONT_SIZE_WEARABLE : fontSize_);
    char info[64] = {0};
    int ret = sprintf_s(info, sizeof(info), "%s %d%%", displayInfo_.c_str(), progress_);
    if (ret == -1) {
        LOGE("set info failed");
        return;
    }
    std::shared_ptr<Rosen::Drawing::TextBlob> textBlob = Rosen::Drawing::TextBlob::MakeFromString(info, font);
    auto textBound = textBlob->Bounds();
    if (textBound == nullptr) {
        LOGE("textBound is null");
        return;
    }

    Rosen::Drawing::Brush whiteBrush;
    whiteBrush.SetColor(0xFFFFFFFF);
    whiteBrush.SetAntiAlias(true);
    canvas->AttachBrush(whiteBrush);

    auto textWidth = font.MeasureText(info, strlen(info), Rosen::Drawing::TextEncoding::UTF8, nullptr);
    float scalarX = windowWidth_ / NUMBER_TWO - textWidth / NUMBER_TWO;
    float scalarY = TEXT_BLOB_OFFSET + textBound->GetHeight() / NUMBER_TWO;
    canvas->DrawTextBlob(textBlob.get(), scalarX, scalarY);
    canvas->DetachBrush();

    DrawMarginBrush(canvas);

    int32_t freqNum = times_++;
    for (int i = 0; i < CIRCLE_NUM; i++) {
        canvas->AttachBrush(DrawProgressPoint(i, freqNum));
        int pointX = windowWidth_/2.0f + 4 * currentRadius_ * (i - 1);
        int pointY = rsCanvasNode_->GetPaintHeight() - currentRadius_;
        canvas->DrawCircle({pointX, pointY}, currentRadius_);
        canvas->DetachBrush();
    }

    rsCanvasNode_->FinishRecording();
    Rosen::RSTransaction::FlushImplicitTransaction();

    if (progress_ >= ONE_HUNDRED_PERCENT) {
        isUpdateOptEnd_ = true;
    }
}

void BootCompileProgress::DrawMarginBrush(Rosen::Drawing::RecordingCanvas* canvas)
{
    if (isWearable_) {
        Rosen::Drawing::Brush marginBrush;
        marginBrush.SetColor(0x00000000);
        marginBrush.SetAntiAlias(true);
        Rosen::Drawing::Rect rect(0, FONT_SIZE_WEARABLE, windowWidth_, FONT_SIZE_WEARABLE + MAGIN_WEARABLE);
        canvas->AttachBrush(marginBrush);
        canvas->DrawRect(rect);
        canvas->DetachBrush();
    }
}

void BootCompileProgress::UpdateCompileProgress()
{
    if (!IsBmsBundleReady()) {
        int64_t now = GetSystemCurrentTime();
        if (endTimePredictMs_ < now) {
            progress_ = ONE_HUNDRED_PERCENT;
            return;
        }
        if (!timeLimitSec_) {
            return;
        }
        progress_ = (int32_t)((now - startTimeMs_) * ONE_HUNDRED_PERCENT / (timeLimitSec_ * SEC_MS));
        progress_ = progress_ < 0 ? 0 : progress_ > ONE_HUNDRED_PERCENT ? ONE_HUNDRED_PERCENT: progress_;
    } else {
        progress_++;
    }

    LOGD("update progress: %{public}d", progress_);
}

Rosen::Drawing::Brush BootCompileProgress::DrawProgressPoint(int32_t idx, int32_t frameNum)
{
    int32_t fpsNo = frameNum % LOADING_FPS;
    int32_t fpsStage = fpsNo / PERIOD_FPS;
    float input  = fpsNo % PERIOD_FPS / TEN_FRAME_TIMES;

    float startOpaCity = OPACITY_ARR[idx][fpsStage][0];
    float endOpaCity = OPACITY_ARR[idx][fpsStage][1];

    float fraction = sharpCurve_->Interpolate(input);
    float opacity = startOpaCity + (endOpaCity - startOpaCity) * fraction;

    Rosen::Drawing::Brush brush;
    brush.SetColor(0xFFFFFFFF);
    brush.SetAntiAlias(true);
    brush.SetAlphaF(opacity);
    return brush;
}

void BootCompileProgress::RecordDeviceType()
{
    std::string deviceType = GetDeviceType();
    LOGI("deviceType: %{public}s", deviceType.c_str());
    if (deviceType == DEVICE_TYPE_WEARABLE) {
        isWearable_ = true;
    } else if (deviceType != DEVICE_TYPE_PHONE) {
        isOther_ = true;
    }
}


void BootCompileProgress::SetFrame()
{
    if (isWearable_) {
        rsCanvasNode_->SetFrame(0, windowHeight_ - OFFSET_Y_WEARABLE - HEIGHT_WEARABLE, windowWidth_, HEIGHT_WEARABLE);
    } else {
        LOGI("screenStatus: %{public}s", screenStatus_.c_str());
        float sizeRatio = DEFAULT_SIZE_RATIO;
        switch (screenStatus_.c_str()[0]) {
            case EXPAND:
                sizeRatio = EXPAND_MIDDLE_SIZE_RATIO;
                rsCanvasNode_->SetFrame(0, windowWidth_, windowHeight_, windowWidth_ * HEIGHT_PERCENT);
                break;
            case FOLD_1:
            case FOLD_2:
                sizeRatio = FOLD_SIZE_RATIO;
                rsCanvasNode_->SetFrame(0-windowHeight_ * FOLD_X_OFFSET, windowWidth_ , windowHeight_,
                    windowWidth_ * HEIGHT_PERCENT);
                break;
            case MIDDLE:
                sizeRatio = EXPAND_MIDDLE_SIZE_RATIO;
                rsCanvasNode_->SetFrame(windowHeight_ * MIDDLE_X_OFFSET, windowWidth_, windowHeight_,
                    windowWidth_ * HEIGHT_PERCENT);
              break;
            default:
                int32_t maxLength = std::max(windowWidth_, windowHeight_);
                rsCanvasNode_->SetFrame(0, windowHeight_ - maxLength * OFFSET_Y_PERCENT, windowWidth_,
                    maxLength * HEIGHT_PERCENT);
                return;
        }
        fontSize_ = TranslateVp2Pixel(std::min(windowWidth_, windowHeight_), FONT_SIZE_PHONE) * sizeRatio;
        currentRadius_ = TranslateVp2Pixel(std::min(windowWidth_, windowHeight_), RADIUS) * sizeRatio;
        rsCanvasNode_->SetRotation(DEGREE);
    }
}
} // namespace OHOS
