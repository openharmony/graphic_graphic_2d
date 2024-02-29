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

#include "frame_report.h"

#include <dlfcn.h>
#include <cstdio>
#include <securec.h>
#include <unistd.h>
#include <hilog/log.h>

#include "parameter.h"
#include "parameters.h"

namespace OHOS {
namespace Rosen {

static constexpr OHOS::HiviewDFX::HiLogLabel LOG_LABEL = { LOG_CORE, 0xD001404, "FrameReport" };
#define LOGF(...) (void)OHOS::HiviewDFX::HiLog::Fatal(LOG_LABEL, __VA_ARGS__)
#define LOGE(...) (void)OHOS::HiviewDFX::HiLog::Error(LOG_LABEL, __VA_ARGS__)
#define LOGW(...) (void)OHOS::HiviewDFX::HiLog::Warn(LOG_LABEL, __VA_ARGS__)
#define LOGI(...) (void)OHOS::HiviewDFX::HiLog::Info(LOG_LABEL, __VA_ARGS__)
#define LOGD(...) (void)OHOS::HiviewDFX::HiLog::Debug(LOG_LABEL, __VA_ARGS__)

#if (defined(__aarch64__) || defined(__x86_64__))
    const std::string FRAME_AWARE_SO_PATH = "/system/lib64/platformsdk/libframe_ui_intf.z.so";
#else
    const std::string FRAME_AWARE_SO_PATH = "/system/lib/platformsdk/libframe_ui_intf.z.so";
#endif

constexpr int SCHEDULE_MSG_BUFFER_SIZE = 48;
constexpr int REPORT_BUFFER_SIZE = 256;

constexpr int THOUSAND_COUNT = 1000;
constexpr int MIN_GAME_FPS = 10;
constexpr int SEND_FPS_FRAME_NUM = 100;
constexpr int64_t VALID_TIME_INTERVAL = 800000000;
constexpr int MIN_FRAME_NUM = 40;
constexpr int64_t ONE_SECOND = 1000000000;
const std::vector<int> LEVEL_TO_TARGET_FPS({ 10, 15, 20, 25, 30, 35, 40, 45, 50, 55, 60, 90, 120, 144 });
const int MAX_FRAME_LEVEL(LEVEL_TO_TARGET_FPS.size() - 1);
const std::vector<int> REAL_FPS_TO_LEVEL({ 0, 10, 15, 20, 25, 30, 35, 40, 45, 50, 55, 60, 90, 120 });
constexpr const char *SWITCH_TEXT = "debug.graphic.framereport";

FrameReport& FrameReport::GetInstance()
{
    static FrameReport instance;
    return instance;
}

FrameReport::FrameReport()
{
    int ret = WatchParameter(SWITCH_TEXT, SwitchFunction, this);
    if (ret) {
        LOGW("WatchParameter failed");
    }
    ret = LoadLibrary();
    if (!ret) {
        LOGE("dlopen libframe_ui_intf.so failed");
        return;
    }
}

FrameReport::~FrameReport()
{
    CloseLibrary();
}

void FrameReport::SwitchFunction(const char *key, const char *value, void *context)
{
    auto &that = *reinterpret_cast<FrameReport *>(context);
    that.gameScene_ = std::atoi(value) != 0;
    LOGI("SwitchFunction value %{public}s  that.gameScene_ %{public}d ",
        value, that.gameScene_);
}

bool FrameReport::IsReportBySurfaceName(std::string& name)
{
    bool isSurface = name.find("Surface") != std::string::npos;
    return isSurface;
}

bool FrameReport::IsGameScene() const
{
    return gameScene_;
}

void FrameReport::SetGameScene(bool gameScene)
{
    gameScene_ = gameScene;
    LOGI("SetGameScene gameScene_ %{public}d ", gameScene_);
}

void FrameReport::CalculateGameFps(int64_t timestamp)
{
    DeleteInvalidTimes(timestamp);
    gameTimeStamps_.PushElement(timestamp);
    gameLastTimeStamp_ = timestamp;
    GetTargetGameFps();

    if ((targetFps_ > 0 && targetFps_ != lastTargetFps_) || (frameNumCnt_ > SEND_FPS_FRAME_NUM)) {
        SendGameTargetFps(targetFps_);
        lastTargetFps_ = targetFps_;
        frameNumCnt_ = 0;
    }
    frameNumCnt_++;
}

void FrameReport::DeleteInvalidTimes(int64_t timestamp)
{
    int64_t headTimestamp = std::get<int64_t>(gameTimeStamps_.GetTailElement());
    if ((timestamp - headTimestamp) > VALID_TIME_INTERVAL) {
        gameTimeStamps_.ClearArray();
    }
    return;
}

void FrameReport::GetTargetGameFps()
{
    targetFps_ = 0;
    FpsCalculator curFps;
    curFps.frameNum = gameTimeStamps_.ArraySize();

    if (curFps.frameNum < MIN_FRAME_NUM) {
        return;
    }
    curFps.duration =
        std::get<int64_t>(gameTimeStamps_.GetTailElement()) - std::get<int64_t>(gameTimeStamps_.GetHeadElement());
    curFps.realFps =
        static_cast<float>(ONE_SECOND) * static_cast<float>(curFps.frameNum - 1) / static_cast<float>(curFps.duration);
    curFps.fps = static_cast<int>(curFps.realFps) + 1;

    targetFps_ = FindStage(curFps.fps);

    LOGD("GetTargetGameFps targetFps_ %{public}d  curFps.fps %{public}d  curFps.realFps %{public}f", targetFps_,
        curFps.fps, curFps.realFps);
}

int FrameReport::FindStage(const int fps) const
{
    int validFps = (fps <= MIN_GAME_FPS) ? MIN_GAME_FPS : fps;
    auto it = lower_bound(REAL_FPS_TO_LEVEL.begin(), REAL_FPS_TO_LEVEL.end(), validFps);
    int level = it - REAL_FPS_TO_LEVEL.begin() - 1;
    level = (level < 0) ? 0 : level;
    level = (level > MAX_FRAME_LEVEL) ? MAX_FRAME_LEVEL : level;
    return LEVEL_TO_TARGET_FPS[level];
}

void FrameReport::SetLastSwapBufferTime(int64_t lastSwapBufferTime)
{
    if (!IsReportBySurfaceName(name_)) {
        return;
    }
    lastSwapBufferTime_ = lastSwapBufferTime;

    int64_t time = std::chrono::duration_cast<std::chrono::nanoseconds>(
        std::chrono::steady_clock::now().time_since_epoch()).count();
    CalculateGameFps(time);
}

void FrameReport::SetDequeueBufferTime(std::string& name, int64_t dequeueBufferTime)
{
    if (!IsReportBySurfaceName(name)) {
        return;
    }
    name_ = name;
    dequeueBufferTime_ = dequeueBufferTime;
}

void FrameReport::SetQueueBufferTime(std::string& name, int64_t queueBufferTime)
{
    if (!IsReportBySurfaceName(name)) {
        return;
    }
    queueBufferTime_ = queueBufferTime;
}

void FrameReport::SetPendingBufferNum(std::string& name, int32_t pendingBufferNum)
{
    if (!IsReportBySurfaceName(name)) {
        return;
    }
    pendingBufferNum_ = pendingBufferNum;
}

bool FrameReport::LoadLibrary()
{
    if (!schedSoLoaded_) {
        schedHandle_ = dlopen(FRAME_AWARE_SO_PATH.c_str(), RTLD_LAZY);
        if (schedHandle_ == nullptr) {
            LOGE("dlopen libframe_ui_intf.so failed! error = %{public}s", dlerror());
            return false;
        }
        schedSoLoaded_ = true;
    }
    LOGI("load library success!");
    return true;
}

void FrameReport::CloseLibrary()
{
    if (schedHandle_ != nullptr) {
        if (dlclose(schedHandle_) != 0) {
            LOGE("libframe_ui_intf.so close failed!\n");
            return;
        }
    }
    schedHandle_ = nullptr;
    schedSoLoaded_ = false;
    LOGI("libframe_ui_intf.so close success!\n");
}

void* FrameReport::LoadSymbol(const char* symName)
{
    if (!schedSoLoaded_) {
        LOGE("libframe_ui_intf.so not loaded.\n");
        return nullptr;
    }

    void *funcSym = dlsym(schedHandle_, symName);
    if (funcSym == nullptr) {
        LOGE("Get %{public}s symbol failed: %{public}s\n", symName, dlerror());
        return nullptr;
    }
    return funcSym;
}

int FrameReport::CurTime(int type, const std::string& message, int length)
{
    int ret = -1;
    if (curTimeFunc_ == nullptr) {
        curTimeFunc_ = LoadSymbol("CurTime");
    }
    if (curTimeFunc_ != nullptr) {
        auto curTimeFunc = reinterpret_cast<int (*)(int, const std::string&, int)>(curTimeFunc_);
        ret = curTimeFunc(type, message, length);
    } else {
        LOGE("load CurTime function failed!");
    }
    return ret;
}

int FrameReport::SchedMsg(int type, const std::string& message, int length)
{
    int ret = -1;
    if (schedMsgFunc_ == nullptr) {
        schedMsgFunc_ = LoadSymbol("SchedMsg");
    }
    if (schedMsgFunc_ != nullptr) {
        auto schedMsgFunc = reinterpret_cast<int (*)(int, const std::string&, int)>(schedMsgFunc_);
        ret = schedMsgFunc(type, message, length);
    } else {
        LOGE("load SchedMsg function failed!");
    }
    return ret;
}

void FrameReport::Report(std::string& name)
{
    if (!IsReportBySurfaceName(name)) {
        return;
    }

    char msg[REPORT_BUFFER_SIZE] = { 0 };

    int ret = sprintf_s(msg, sizeof(msg),
        "{\"swapBufferTime\":\"%d\",\"pendingBufferNum\":\"%d\",\"dequeueBufferTime\":\"%d\","
        "\"queueBufferTime\":\"%d\", \"skipHint\":\"%d\"}",
        static_cast<int>(lastSwapBufferTime_ / THOUSAND_COUNT), pendingBufferNum_,
        static_cast<int>(dequeueBufferTime_ / THOUSAND_COUNT), static_cast<int>(queueBufferTime_ / THOUSAND_COUNT),
        skipHintStatus_);
    if (ret == -1) {
        return;
    }
    std::string bfMsg(msg);
    LOGD("Report bfMsg %{public}s ", bfMsg.c_str());
#ifdef AI_SCHED_ENABLE
    ret = CurTime(1, bfMsg, bfMsg.size());
    if (ret) {
        LOGW("hwsched sf time failed");
    } else {
        LOGD("hwsched sf time succ");
    }
#endif
}

void FrameReport::SendGameTargetFps(int32_t fps)
{
    char msg[SCHEDULE_MSG_BUFFER_SIZE] = { 0 };
    int ret = sprintf_s(msg, sizeof(msg), "{\"agpFPS\":\"%d\"}", fps);
    if (ret == -1) {
        return;
    }

    std::string bfMsg(msg);
    LOGD("SendGameTargetFps bfMsg %{public}s ", bfMsg.c_str());
#ifdef AI_SCHED_ENABLE
    ret = SchedMsg(1, bfMsg, bfMsg.size());
    if (ret) {
        LOGW("hwsched game fps failed");
    } else {
        LOGD("hwsched game fps succ");
    }
#endif
}

} // namespace Rosen
} // namespace OHOS
