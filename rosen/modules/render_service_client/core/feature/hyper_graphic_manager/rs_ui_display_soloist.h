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
#ifndef RENDER_SERVICE_CLIENT_CORE_UI_RS_UI_DISPLAY_SOLOIST_H
#define RENDER_SERVICE_CLIENT_CORE_UI_RS_UI_DISPLAY_SOLOIST_H

#include <atomic>
#include <functional>
#include <mutex>

#include "command/rs_animation_command.h"
#include "common/rs_common_def.h"
#include "feature/hyper_graphic_manager/rs_frame_rate_linker.h"
#include "transaction/rs_interfaces.h"
#include "vsync_receiver.h"

namespace OHOS {
namespace Rosen {

using SoloistIdType = uint32_t;
using TimestampType = int64_t;
using DisplaySoloistOnFrameCallback = std::function<void(long long, long long, void*)>;

const std::string TIME_OUT_TASK = "vsync_time_out_task_";
constexpr int64_t TIME_OUT_MILLISECONDS = 600;
static const std::vector<int32_t> REFRESH_RATE_LIST{ 90, 120, 144 };
static std::vector<int32_t> REFRESH_RATE_FACTORS;
static std::unordered_map<int32_t, std::vector<int32_t>> RATE_TO_FACTORS;
static std::once_flag COMPUTE_FACTORS_FLAG;
constexpr float SECOND_IN_NANO = 1000000000.0f;
constexpr int32_t FRAME_RATE_0 = 0;
constexpr int32_t SOLOIST_ERROR = -1;
constexpr int32_t EXEC_SUCCESS = 0;

enum class ActiveStatus : int32_t {
    INACTIVE = 0,
    ACTIVE = 1,
    NEED_REMOVE = 2,
};

class RSC_EXPORT SoloistId {
public:
    SoloistId();
    ~SoloistId();

    SoloistIdType GetId() const;
    static std::shared_ptr<SoloistId> Create();
private:
    static SoloistIdType GenerateId();
    const SoloistIdType id_;
};

class RSC_EXPORT RSDisplaySoloist {
public:
    RSDisplaySoloist() = default;
    RSDisplaySoloist(SoloistIdType instanceId);
    ~RSDisplaySoloist()
    {
        std::lock_guard<std::mutex> lock(mtx_);
        destroyed_ = true;
    }

    bool JudgeWhetherSkip(TimestampType timestamp);
    void RequestNextVSync();
    void SetSubFrameRateLinkerEnable(bool enabled);
    void TriggerCallback();
    void SetCallback(DisplaySoloistOnFrameCallback cb, void* params);
    static void OnVsync(TimestampType timestamp, void* client);
    void VsyncCallbackInner(TimestampType timestamp);

    enum ActiveStatus subStatus_ = ActiveStatus::INACTIVE;
    bool useExclusiveThread_ = false;
    FrameRateRange frameRateRange_;
private:
    void Init();
    bool IsCommonDivisor(int32_t expectedRate, int32_t vsyncRate);
    std::vector<int32_t> FindRefreshRateFactors(int32_t refreshRate);
    void FindAllRefreshRateFactors();
    int32_t FindMatchedRefreshRate(int32_t vsyncRate, int32_t targetRate);
    int32_t FindAccurateRefreshRate(int32_t approximateRate);
    int32_t SearchMatchedRate(const FrameRateRange& frameRateRange, int32_t vsyncRate,
                              int32_t iterCount = 1);

    void FlushFrameRate(int32_t rate);
    int64_t GetVSyncPeriod();
    bool SetVSyncRate(int32_t vsyncRate);
    int32_t GetVSyncRate();

    std::shared_ptr<AppExecFwk::EventHandler> subVsyncHandler_ = nullptr;
    std::shared_ptr<OHOS::Rosen::VSyncReceiver> subReceiver_ = nullptr;
    VSyncReceiver::FrameCallback subFrameCallback_ = {
        .userData_ = this,
        .callback_ = OnVsync,
    };
#ifdef RS_ENABLE_GPU
    bool hasInitVsyncReceiver_ = false;
#endif

    int32_t sourceVsyncRate_ = 0;
    int32_t drawFPS_ = 0;
    int32_t currRate_ = 0;
    int32_t currCnt_ = 0;
    TimestampType timestamp_ = 0;
    TimestampType targetTimestamp_ = 0;

    SoloistIdType instanceId_ = 0;
    std::shared_ptr<RSFrameRateLinker> frameRateLinker_;

    void OnVsyncTimeOut();
    std::mutex mtx_;
    bool hasRequestedVsync_ = false;
    bool destroyed_ = false;
    std::string vsyncTimeoutTaskName_ = "";
#ifdef RS_ENABLE_GPU
    AppExecFwk::EventHandler::Callback vsyncTimeoutCallback_ =
        [this] { this->OnVsyncTimeOut(); };
#endif

    std::mutex callbackMutex_;
    std::pair<DisplaySoloistOnFrameCallback, void*> callback_;
};

using IdToSoloistMapType = std::unordered_map<SoloistIdType, std::shared_ptr<RSDisplaySoloist>>;

class RSC_EXPORT RSDisplaySoloistManager {
public:
    static RSDisplaySoloistManager& GetInstance() noexcept;
    void RequestNextVSync();

    void Start(SoloistIdType id);
    void Stop(SoloistIdType id);

    void InsertOnVsyncCallback(SoloistIdType id, DisplaySoloistOnFrameCallback cb, void* data);
    void InsertFrameRateRange(SoloistIdType id, FrameRateRange frameRateRange);
    void InsertUseExclusiveThreadFlag(SoloistIdType id, bool useExclusiveThread);
    void RemoveSoloist(SoloistIdType id);

    void FlushFrameRate(int32_t rate);
    void SetMainFrameRateLinkerEnable(bool enabled);

    FrameRateRange GetFrameRateRange();
    IdToSoloistMapType GetIdToSoloistMap();
    std::shared_ptr<RSFrameRateLinker> GetFrameRateLinker();
    enum ActiveStatus GetManagerStatus();

    int64_t GetVSyncPeriod() const;
    bool SetVSyncRate(int32_t vsyncRate);
    int32_t GetVSyncRate() const;

private:
    RSDisplaySoloistManager() = default;
    ~RSDisplaySoloistManager() noexcept;
    RSDisplaySoloistManager(const RSDisplaySoloistManager&) = delete;
    RSDisplaySoloistManager(const RSDisplaySoloistManager&&) = delete;
    RSDisplaySoloistManager& operator=(const RSDisplaySoloistManager&) = delete;
    RSDisplaySoloistManager& operator=(const RSDisplaySoloistManager&&) = delete;
    void InitVsyncReceiver();

    std::shared_ptr<AppExecFwk::EventHandler> vsyncHandler_ = nullptr;
    std::shared_ptr<OHOS::Rosen::VSyncReceiver> receiver_ = nullptr;
    static void OnVsync(TimestampType timestamp, void* client);
    void VsyncCallbackInner(TimestampType timestamp);
    void DispatchSoloistCallback(TimestampType timestamp);
    VSyncReceiver::FrameCallback managerFrameCallback_ = {
        .userData_ = this,
        .callback_ = OnVsync,
    };

    enum ActiveStatus managerStatus_ = ActiveStatus::INACTIVE;
    int32_t sourceVsyncRate_ = 0;

    FrameRateRange frameRateRange_;
    std::shared_ptr<RSFrameRateLinker> frameRateLinker_;
    IdToSoloistMapType idToSoloistMap_;

    void OnVsyncTimeOut();
    std::mutex mtx_;
    std::mutex dataUpdateMtx_;
    std::string vsyncTimeoutTaskName_ = "soloist_manager_time_out_task";
#ifdef RS_ENABLE_GPU
    AppExecFwk::EventHandler::Callback vsyncTimeoutCallback_ =
        [this] { this->OnVsyncTimeOut(); };
#endif
};

} // namespace Rosen
} // namespace OHOS

#endif // RENDER_SERVICE_CLIENT_CORE_UI_RS_UI_DISPLAY_SOLOIST_H
