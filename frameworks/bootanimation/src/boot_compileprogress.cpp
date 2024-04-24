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

#include <chrono>

#include "boot_compileprogress.h"
#include "event_handler.h"
#include "parameter.h"
#include "parameters.h"
#include "recording/recording_canvas.h"
#include "platform/common/rs_system_properties.h"
#include "transaction/rs_interfaces.h"
#include "transaction/rs_render_service_client.h"
#include "transaction/rs_transaction.h"
#ifdef NEW_RENDER_CONTEXT
#include "render_context_factory.h"
#include "rs_surface_factory.h"
#endif

constexpr const char* DUE_UPDATE_TYPE_PARAM = "persist.dupdate_engine.update_type";
const std::string DUE_UPDATE_TYPE_MANUAL = "manual";
const std::string DUE_UPDATE_TYPE_NIGHT = "night";

constexpr const char* OTA_BMS_COMPILE_SWITCH = "const.bms.optimizing_apps.switch";
const std::string OTA_BMS_COMPILE_SWITCH_OFF = "off";
const std::string OTA_BMS_COMPILE_SWITCH_ON = "on";

constexpr const char* OTA_COMPILE_TIME_LIMIT = "persist.bms.optimizing_apps.timing";
constexpr int32_t OTA_COMPILE_TIME_LIMIT_DEFAULT = 5 * 60;

constexpr const char* BMS_COMPILE_STATUS = "bms.optimizing_apps.status";
const std::string BMS_COMPILE_STATUS_BEGIN = "0";
const std::string BMS_COMPILE_STATUS_END = "1";

constexpr const char* OTA_COMPILE_DISPLAY_INFO = "const.bms.optimizing_apps.display_info";
const std::string OTA_COMPILE_DISPLAY_INFO_DEFAULT = "正在优化应用";

const std::string FONTS_PATH = "/system/fonts/HarmonyOS_Sans_SC_Black.ttf";
constexpr const int32_t WAITING_SECONDS = 30;
constexpr const int32_t ONE_HUNDRED_PERCENT = 100;
constexpr const int32_t SEC_MS = 1000;
constexpr const int32_t CIRCLE_NUM = 3;
constexpr const float RADIUS = 10.0f;
constexpr const float OFFSET_Y_PERCENT = 0.85;
constexpr const float HEIGT_PERCENT = 0.05;
constexpr const int TEXT_BLOB_OFFSET = 5;
constexpr const int FONT_SIZE = 50;
constexpr const int32_t MAX_RETRY = 5;
constexpr const int32_t WAIT_MS = 500;

using namespace OHOS;

bool BootCompileProgress::Init(Rosen::ScreenId screenId, int32_t height, int width)
{
    screenId_ = screenId;
    windowHeight_ = height;
    windowWidth_= width;

    timeLimitSec_ = system::GetIntParameter<int32_t>(OTA_COMPILE_TIME_LIMIT, OTA_COMPILE_TIME_LIMIT_DEFAULT);
    tf_ = Rosen::Drawing::Typeface::MakeFromFile(FONTS_PATH.c_str());
    if (!tf_.get()) {
        LOGE("can not read font: %{public}s", FONTS_PATH.c_str());
        return false;
    }

    displayInfo_ = system::GetParameter(OTA_COMPILE_DISPLAY_INFO, OTA_COMPILE_DISPLAY_INFO_DEFAULT);
    LOGI("BootCompileProgress Init Done.");
    return true;
}

bool BootCompileProgress::CreateCanvasNode(int32_t rotateDegree, float positionZ)
{
    LOGI("BootCompileProgress Create Canvas Node Enter.");

    struct Rosen::RSSurfaceNodeConfig surfaceNodeConfig;
    surfaceNodeConfig.SurfaceNodeName = "BootCompileProgressNode";
    surfaceNodeConfig.isSync = true;
    Rosen::RSSurfaceNodeType surfaceNodeType = Rosen::RSSurfaceNodeType::SELF_DRAWING_WINDOW_NODE;
    rsSurfaceNode_ = Rosen::RSSurfaceNode::Create(surfaceNodeConfig, surfaceNodeType);
    if (!rsSurfaceNode_) {
        LOGE("Boot Compile Progress SFNode Create failed.");
        return false;
    }

    rsSurfaceNode_->SetRotation(rotateDegree);
    rsSurfaceNode_->SetPositionZ(positionZ);
    rsSurfaceNode_->SetBounds(0, 0, windowWidth_, windowHeight_);
    rsSurfaceNode_->SetBackgroundColor(SK_ColorTRANSPARENT);
    rsSurfaceNode_->SetFrameGravity(Rosen::Gravity::RESIZE_ASPECT);
    rsSurfaceNode_->SetBootAnimation(true);
    Rosen::RSTransaction::FlushImplicitTransaction();
    rsSurfaceNode_->AttachToDisplay(screenId_);
    Rosen::RSTransaction::FlushImplicitTransaction();

    rsCanvasNode_ = Rosen::RSCanvasNode::Create();
    rsCanvasNode_->SetBounds(0, 0, windowWidth_, windowHeight_);
    rsCanvasNode_->SetFrame(0, windowHeight_ * OFFSET_Y_PERCENT, windowWidth_, windowHeight_ * HEIGT_PERCENT);
    rsCanvasNode_->SetBackgroundColor(Rosen::Drawing::Color::COLOR_TRANSPARENT);
    rsCanvasNode_->SetPositionZ(positionZ);

    rsSurfaceNode_->AddChild(rsCanvasNode_, 0);

    LOGI("BootCompileProgress Create Canvas Node Done.");
    return true;
}

bool BootCompileProgress::RegisterVsyncAndStart()
{
    LOGI("Register Vsync Callback Enter");

    if (!WaitBmsStartIfNeeded()) {
        return false;
    }

    auto& rsClient = Rosen::RSInterfaces::GetInstance();
    int32_t retry = 0;
    while (receiver_ == nullptr) {
        if (retry++ > MAX_RETRY) {
            LOGE("get vsync receiver failed.");
            return false;
        }
        if (retry > 1) {
            std::this_thread::sleep_for(std::chrono::milliseconds(WAIT_MS));
        }
        receiver_ = rsClient.CreateVSyncReceiver("BootCompileProgress");
    }
    VsyncError ret = receiver_->Init();
    if (ret) {
        LOGE("init vsync receiver failed.");
        return false;
    }

    Rosen::VSyncReceiver::FrameCallback fcb = {
        .userData_ = this,
        .callback_ = std::bind(&BootCompileProgress::OnVsync, this),
    };
    int32_t changeFreq = static_cast<int32_t>((1000.0/freq_)/16);
    ret = receiver_->SetVSyncRate(fcb, changeFreq);
    if (ret) {
        LOGE("set vsync rate failed.");
    }

    startTimeMs_ = std::chrono::duration_cast<std::chrono::milliseconds>(
        std::chrono::system_clock::now().time_since_epoch()).count();
    
    endTimePredictMs_ = startTimeMs_ + timeLimitSec_ * SEC_MS;

    LOGI("Register Vsync Callback Done");
    return true;
}

bool BootCompileProgress::WaitBmsStartIfNeeded()
{
    int waitSeconds = 0;
    while (system::GetParameter(BMS_COMPILE_STATUS, "-1") != BMS_COMPILE_STATUS_BEGIN) {
        if (++waitSeconds > WAITING_SECONDS) {
            break;
        }
        LOGE("waiting Bms start oat compile...");
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }

    if (system::GetParameter(BMS_COMPILE_STATUS, "-1") != BMS_COMPILE_STATUS_BEGIN) {
        LOGE("Bms did not start oat compile.");
        return false;
    }
    return true;
}

void BootCompileProgress::OnVsync()
{
    LOGI("On Vsync Enter.");
    if (!isUpdateOptEnd_) {
        PostTask(std::bind(&BootCompileProgress::DrawCompileProgress, this));
    }
}

void BootCompileProgress::DrawCompileProgress()
{
    UpdateCompileProgress();

    auto canvas = static_cast<Rosen::Drawing::RecordingCanvas*>(
        rsCanvasNode_->BeginRecording(windowWidth_, windowHeight_ * HEIGT_PERCENT));

    Rosen::Drawing::Font font;
    font.SetTypeface(tf_);
    font.SetSize(FONT_SIZE);
    char info[64] = {0};
    int ret = sprintf_s(info, sizeof(info), "%s %d%%", displayInfo_.c_str(), progress_);
    if (ret == -1) {
        LOGE("set info failed.");
        return;
    }
    std::shared_ptr<Rosen::Drawing::TextBlob> textBlob = Rosen::Drawing::TextBlob::MakeFromString(info, font);

    Rosen::Drawing::Brush whiteBrush;
    whiteBrush.SetColor(0xFFFFFFFF);
    whiteBrush.SetAntiAlias(true);
    canvas->AttachBrush(whiteBrush);

    double scale = windowWidth_ >= windowHeight_ ? 0.55: 0.6;
    float scalarX = windowWidth_ * scale - textBlob->Bounds()->GetWidth() / 2;
    float scalarY = TEXT_BLOB_OFFSET + textBlob->Bounds()->GetHeight() / 2;
    canvas->DrawTextBlob(textBlob.get(), scalarX, scalarY);
    canvas->DetachBrush();

    Rosen::Drawing::Brush grayBrush;
    grayBrush.SetColor(0x40FFFFFF);
    grayBrush.SetAntiAlias(true);

    int whitePos = isBmsCompileDone_ ? ++times % CIRCLE_NUM : ++times/freq_ % CIRCLE_NUM;
    for (int i = 0; i < CIRCLE_NUM; i++) {
        if (i == whitePos) {
            canvas->AttachBrush(whiteBrush);
        } else {
            canvas->AttachBrush(grayBrush);
        }
        int pointX = windowWidth_/2 + 4 * RADIUS * (i - 1);
        int pointY = rsCanvasNode_->GetPaintHeight() - 2 * RADIUS;
        canvas->DrawCircle({pointX, pointY}, RADIUS);
        canvas->DetachBrush();
    }

    if (progress_ >= ONE_HUNDRED_PERCENT) {
        isUpdateOptEnd_ = true;
    }

    rsCanvasNode_->FinishRecording();
    Rosen::RSTransaction::FlushImplicitTransaction();
}

void BootCompileProgress::CheckNeedOatCompile()
{
    std::string otaCompileSwitch = system::GetParameter(OTA_BMS_COMPILE_SWITCH, OTA_BMS_COMPILE_SWITCH_OFF);
    if (otaCompileSwitch != OTA_BMS_COMPILE_SWITCH_ON) {
        LOGI("ota compile switch is: %{public}s", otaCompileSwitch.c_str());
        return;
    }

    std::string dueUpdateType = system::GetParameter(DUE_UPDATE_TYPE_PARAM, "");
    LOGI("dueUpdateType is: %{public}s", dueUpdateType.c_str());
    bool isOtaUpdate = dueUpdateType == DUE_UPDATE_TYPE_MANUAL || dueUpdateType == DUE_UPDATE_TYPE_NIGHT;

    std::string bmsCompileStatus  = system::GetParameter(BMS_COMPILE_STATUS, "-1");
    LOGI("bmsCompileStatus is: %{public}s", bmsCompileStatus.c_str());
    bool isCompileDone = bmsCompileStatus == BMS_COMPILE_STATUS_END;

    if (isOtaUpdate && !isCompileDone) {
        needUpdateOpt_ = true;
    }
}

void BootCompileProgress::UpdateCompileProgress()
{
    if (!isBmsCompileDone_) {
        isBmsCompileDone_ = system::GetParameter(BMS_COMPILE_STATUS, "-1") == BMS_COMPILE_STATUS_END;
        int64_t now =
            std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch())
            .count();
        if (endTimePredictMs_ < now) {
            progress_ = ONE_HUNDRED_PERCENT;
            return;
        }
        progress_ = (int32_t)((now - startTimeMs_) * ONE_HUNDRED_PERCENT / (timeLimitSec_ * SEC_MS));
        progress_ = progress_ < 0 ? 0 : progress_ > ONE_HUNDRED_PERCENT ? ONE_HUNDRED_PERCENT: progress_;
    } else {
        progress_++;
    }

    LOGI("update progress: %{public}d", progress_);
}

bool BootCompileProgress::NeedUpdate()
{
    return needUpdateOpt_;
}

bool BootCompileProgress::IsUpdateDone()
{
    return isUpdateOptEnd_;
}

bool BootCompileProgress::IsStarted()
{
    return started_;
}

void BootCompileProgress::SetStart(bool start)
{
    started_ = start;
}
