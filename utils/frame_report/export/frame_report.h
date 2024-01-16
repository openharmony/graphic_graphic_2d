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

#ifndef UTILS_INCLUDE_FRAME_REPORT_H
#define UTILS_INCLUDE_FRAME_REPORT_H

#include <cstdint>
#include <string>
#include "ring_array.h"

namespace OHOS {
namespace Rosen {

struct FpsCalculator {
    int64_t duration;
    int frameNum;
    float realFps;
    int fps;
};

class FrameReport {
public:
    static FrameReport& GetInstance();

    bool IsGameScene() const;
    void SetGameScene(bool gameScene);
    void SetLastSwapBufferTime(int64_t lastSwapBufferTime);
    void SetDequeueBufferTime(std::string& name, int64_t dequeueBufferTime);
    void SetQueueBufferTime(std::string& name, int64_t queueBufferTime);
    void SetPendingBufferNum(std::string& name, int32_t pendingBufferNum);
    void Report(std::string& name);

private:
    FrameReport();
    ~FrameReport();

    bool IsReportBySurfaceName(std::string& name);
    void CalculateGameFps(int64_t timestamp);
    void DeleteInvalidTimes(int64_t timestamp);
    void GetTargetGameFps();
    void SendGameTargetFps(int32_t fps);
    int FindStage(const int fps) const;

    bool LoadLibrary();
    void CloseLibrary();
    void* LoadSymbol(const char* symName);
    int CurTime(int type, const std::string& message, int length);
    int SchedMsg(int type, const std::string& message, int length);

    static void SwitchFunction(const char *key, const char *value, void *context);

    int64_t lastSwapBufferTime_ = 0;
    int64_t dequeueBufferTime_ = 0;
    int64_t queueBufferTime_ = 0;
    int32_t pendingBufferNum_ = 0;
    int32_t skipHintStatus_ = 0;

    std::string name_ = "";
    int frameNumCnt_ = 0;
    int targetFps_ = 0;
    int lastTargetFps_ = 0;
    RingArray gameTimeStamps_;
    int64_t gameLastTimeStamp_ = 0;

    bool gameScene_ = false;

    void* curTimeFunc_ = nullptr;
    void* schedMsgFunc_ = nullptr;
    void* schedHandle_ = nullptr;
    bool schedSoLoaded_ = false;
};

} // namespace Rosen
} // namespace OHOS

#endif // UTILS_INCLUDE_FRAME_REPORT_H