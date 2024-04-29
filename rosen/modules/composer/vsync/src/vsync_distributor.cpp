/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
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

#include "vsync_distributor.h"
#include <chrono>
#include <condition_variable>
#include <algorithm>
#include <cstdint>
#include <mutex>
#include <sched.h>
#include <sys/resource.h>
#include <scoped_bytrace.h>
#include <hitrace_meter.h>
#include "vsync_log.h"
#include "vsync_type.h"
#include "vsync_generator.h"

#ifdef COMPOSER_SCHED_ENABLE
#include "if_system_ability_manager.h"
#include <iservice_registry.h>
#include "system_ability_definition.h"
#endif

namespace OHOS {
namespace Rosen {
namespace {
constexpr int32_t SOFT_VSYNC_PERIOD = 16;
constexpr int32_t ERRNO_EAGAIN = -1;
constexpr int32_t ERRNO_OTHER = -2;
constexpr int32_t THREAD_PRIORTY = -6;
constexpr int32_t SCHED_PRIORITY = 2;
constexpr uint32_t SOCKET_CHANNEL_SIZE = 1024;
constexpr int32_t VSYNC_CONNECTION_MAX_SIZE = 128;
#if defined(RS_ENABLE_DVSYNC)
constexpr int64_t DVSYNC_ON_PERIOD = 8333333;
constexpr int64_t MAX_PERIOD_BIAS = 500000;
#endif
}

VSyncConnection::VSyncConnectionDeathRecipient::VSyncConnectionDeathRecipient(
    wptr<VSyncConnection> conn) : conn_(conn)
{
}

void VSyncConnection::VSyncConnectionDeathRecipient::OnRemoteDied(const wptr<IRemoteObject>& token)
{
    auto tokenSptr = token.promote();
    if (tokenSptr == nullptr) {
        VLOGW("%{public}s: can't promote remote object.", __func__);
        return;
    }
    auto vsyncConn = conn_.promote();
    if (vsyncConn == nullptr) {
        VLOGW("%{public}s: VSyncConnection was dead, do nothing.", __func__);
        return;
    }
    if (vsyncConn->token_ != tokenSptr) {
        VLOGI("%{public}s: token doesn't match, ignore it.", __func__);
        return;
    }
    VLOGD("%{public}s: clear socketPair, conn name:%{public}s.", __func__, vsyncConn->info_.name_.c_str());
    VsyncError ret = vsyncConn->Destroy();
    if (ret != VSYNC_ERROR_OK) {
        VLOGE("vsync connection clearAll failed!");
    }
}

VSyncConnection::VSyncConnection(
    const sptr<VSyncDistributor>& distributor,
    std::string name,
    const sptr<IRemoteObject>& token,
    uint64_t id,
    uint64_t windowNodeId)
    : rate_(-1),
      info_(name),
      id_(id),
      windowNodeId_(windowNodeId),
      vsyncConnDeathRecipient_(new VSyncConnectionDeathRecipient(this)),
      token_(token),
      distributor_(distributor)
{
    socketPair_ = new LocalSocketPair();
    int32_t err = socketPair_->CreateChannel(SOCKET_CHANNEL_SIZE, SOCKET_CHANNEL_SIZE);
    if (err != 0) {
        ScopedBytrace func("Create socket channel failed, errno = " + std::to_string(errno));
    }
    if (token_ != nullptr) {
        token_->AddDeathRecipient(vsyncConnDeathRecipient_);
    }
    proxyPid_ = GetCallingPid();
    isDead_ = false;
}

VSyncConnection::~VSyncConnection()
{
    if ((token_ != nullptr) && (vsyncConnDeathRecipient_ != nullptr)) {
        token_->RemoveDeathRecipient(vsyncConnDeathRecipient_);
    }
}

VsyncError VSyncConnection::RequestNextVSync()
{
    return RequestNextVSync("unknown", 0);
}

VsyncError VSyncConnection::RequestNextVSync(const std::string &fromWhom, int64_t lastVSyncTS)
{
    sptr<VSyncDistributor> distributor;
    {
        std::unique_lock<std::mutex> locker(mutex_);
        if (isDead_) {
            VLOGE("%{public}s VSync Client Connection is dead, name:%{public}s.", __func__, info_.name_.c_str());
            return VSYNC_ERROR_API_FAILED;
        }
        if (distributor_ == nullptr) {
            return VSYNC_ERROR_NULLPTR;
        }
        distributor = distributor_.promote();
        if (distributor == nullptr) {
            return VSYNC_ERROR_NULLPTR;
        }
    }
    return distributor->RequestNextVSync(this, fromWhom, lastVSyncTS);
}

VsyncError VSyncConnection::GetReceiveFd(int32_t &fd)
{
    std::unique_lock<std::mutex> locker(mutex_);
    if (isDead_) {
        VLOGE("%{public}s VSync Client Connection is dead, name:%{public}s.", __func__, info_.name_.c_str());
        return VSYNC_ERROR_API_FAILED;
    }
    fd = socketPair_->GetReceiveDataFd();
    return VSYNC_ERROR_OK;
}

int32_t VSyncConnection::PostEvent(int64_t now, int64_t period, int64_t vsyncCount)
{
    sptr<LocalSocketPair> socketPair;
    {
        std::unique_lock<std::mutex> locker(mutex_);
        if (isDead_) {
            ScopedBytrace func("Vsync Client Connection is dead, conn: " + info_.name_);
            VLOGE("%{public}s VSync Client Connection is dead, name:%{public}s.", __func__, info_.name_.c_str());
            return ERRNO_OTHER;
        }
        socketPair = socketPair_;
    }
    if (socketPair == nullptr) {
        ScopedBytrace func("socketPair is null, conn: " + info_.name_);
        return ERRNO_OTHER;
    }
    ScopedBytrace func("SendVsyncTo conn: " + info_.name_ + ", now:" + std::to_string(now));
    // 3 is array size.
    int64_t data[3];
    data[0] = now;
    // 1, 2: index of array data.
    data[1] = period;
    data[2] = vsyncCount;
    int32_t ret = socketPair->SendData(data, sizeof(data));
    if (ret > -1) {
        ScopedDebugTrace successful("successful");
        info_.postVSyncCount_++;
    } else {
        ScopedBytrace failed("failed");
    }
    return ret;
}

VsyncError VSyncConnection::SetVSyncRate(int32_t rate)
{
    std::unique_lock<std::mutex> locker(mutex_);
    if (isDead_) {
        VLOGE("%{public}s VSync Client Connection is dead, name:%{public}s.", __func__, info_.name_.c_str());
        return VSYNC_ERROR_API_FAILED;
    }
    if (distributor_ == nullptr) {
        return VSYNC_ERROR_NULLPTR;
    }
    const sptr<VSyncDistributor> distributor = distributor_.promote();
    if (distributor == nullptr) {
        return VSYNC_ERROR_NULLPTR;
    }
    return distributor->SetVSyncRate(rate, this);
}

VsyncError VSyncConnection::CleanAllLocked()
{
    socketPair_ = nullptr;
    const sptr<VSyncDistributor> distributor = distributor_.promote();
    if (distributor == nullptr) {
        return VSYNC_ERROR_OK;
    }
    VsyncError ret = distributor->RemoveConnection(this);
    isDead_ = true;
    return ret;
}

VsyncError VSyncConnection::Destroy()
{
    std::unique_lock<std::mutex> locker(mutex_);
    return CleanAllLocked();
}

VSyncDistributor::VSyncDistributor(sptr<VSyncController> controller, std::string name)
    : controller_(controller), mutex_(), con_(), connections_(),
    event_(), vsyncEnabled_(false), name_(name)
{
    if (name == "rs") {
        isRs_ = true;
    }
#if defined(RS_ENABLE_DVSYNC)
    dvsync_ = new DVsync();
    if (isRs_ && dvsync_->IsFeatureEnabled()) {
        vsyncThreadRunning_ = true;
        threadLoop_ = std::thread(std::bind(&VSyncDistributor::ThreadMain, this));
        std::string threadName = "DVSync-" + name;
        pthread_setname_np(threadLoop_.native_handle(), threadName.c_str());
    }
#endif
}

VSyncDistributor::~VSyncDistributor()
{
#if defined(RS_ENABLE_DVSYNC)
    if (isRs_ && dvsync_->IsFeatureEnabled()) {
        {
            std::unique_lock<std::mutex> locker(mutex_);
            vsyncThreadRunning_ = false;
        }
        if (threadLoop_.joinable()) {
            {
                std::unique_lock<std::mutex> locker(mutex_);
                dvsync_->RNVNotify();
            }
            con_.notify_all();
            threadLoop_.join();
        }
    }
#endif
}

VsyncError VSyncDistributor::AddConnection(const sptr<VSyncConnection>& connection, uint64_t windowNodeId)
{
    if (connection == nullptr) {
        return VSYNC_ERROR_NULLPTR;
    }

    int32_t proxyPid = connection->proxyPid_;
    std::lock_guard<std::mutex> locker(mutex_);
    if (connectionCounter_[proxyPid] > VSYNC_CONNECTION_MAX_SIZE) {
        VLOGE("You [%{public}d] have created too many vsync connection, please check!!!", proxyPid);
        return VSYNC_ERROR_API_FAILED;
    }

    auto it = std::find(connections_.begin(), connections_.end(), connection);
    if (it != connections_.end()) {
        return VSYNC_ERROR_INVALID_ARGUMENTS;
    }
    ScopedBytrace func("Add VSyncConnection: " + connection->info_.name_);
    connections_.push_back(connection);
    connectionCounter_[proxyPid]++;
    if (windowNodeId != 0) {
        connectionsMap_[windowNodeId].push_back(connection);
    } else {
        uint32_t tmpPid;
        if (QosGetPidByName(connection->info_.name_, tmpPid) == VSYNC_ERROR_OK) {
            connectionsMap_[tmpPid].push_back(connection);
        }
    }
    
    return VSYNC_ERROR_OK;
}

VsyncError VSyncDistributor::RemoveConnection(const sptr<VSyncConnection>& connection)
{
    if (connection == nullptr) {
        return VSYNC_ERROR_NULLPTR;
    }
    int32_t proxyPid = connection->proxyPid_;
    std::lock_guard<std::mutex> locker(mutex_);
    auto it = std::find(connections_.begin(), connections_.end(), connection);
    if (it == connections_.end()) {
        return VSYNC_ERROR_INVALID_ARGUMENTS;
    }
    ScopedBytrace func("Remove VSyncConnection: " + connection->info_.name_);
    connections_.erase(it);
    connectionCounter_[proxyPid]--;
    if (connectionCounter_[proxyPid] == 0) {
        connectionCounter_.erase(proxyPid);
    }
    connectionsMap_.erase(connection->windowNodeId_);
    uint32_t tmpPid;
    if (QosGetPidByName(connection->info_.name_, tmpPid) == VSYNC_ERROR_OK) {
        auto iter = connectionsMap_.find(tmpPid);
        if (iter == connectionsMap_.end()) {
            return VSYNC_ERROR_OK;
        }
        auto connIter = find(iter->second.begin(), iter->second.end(), connection);
        if (connIter != iter->second.end()) {
            iter->second.erase(connIter);
        }
        if (iter->second.empty()) {
            connectionsMap_.erase(iter);
        }
    }
    return VSYNC_ERROR_OK;
}

void VSyncDistributor::WaitForVsyncOrRequest(std::unique_lock<std::mutex> &locker)
{
    if (!vsyncThreadRunning_) {
        return;
    }

    // before con_ wait, notify the rnv_con.
#if defined(RS_ENABLE_DVSYNC)
        dvsync_->RNVNotify();
#endif
    con_.wait(locker);

#if defined(RS_ENABLE_DVSYNC)
    if (pendingRNVInVsync_) {
        return;
    }
    if (IsDVsyncOn()) {
        std::pair<bool, int64_t> result = dvsync_->DoPreExecute(locker, con_);
        if (result.first) {
            event_.timestamp = result.second;
            lastDVsyncTS_.store(result.second);
            event_.vsyncCount++;
            if (vsyncEnabled_ == false) {
                ScopedBytrace func(name_ + "_EnableVsync");
                EnableVSync();
            }
        }
    }
#endif
}

int64_t Now()
{
    const auto &now = std::chrono::steady_clock::now().time_since_epoch();
    return std::chrono::duration_cast<std::chrono::nanoseconds>(now).count();
}

void VSyncDistributor::ThreadMain()
{
    // set thread priorty
    setpriority(PRIO_PROCESS, 0, THREAD_PRIORTY);
    struct sched_param param = {0};
    param.sched_priority = SCHED_PRIORITY;
    sched_setscheduler(0, SCHED_FIFO, &param);

#ifdef COMPOSER_SCHED_ENABLE
    std::string threadName = "VSync-" + name_;
    SubScribeSystemAbility(threadName);
#endif

    int64_t timestamp;
    while (vsyncThreadRunning_ == true) {
        std::vector<sptr<VSyncConnection>> conns;
        {
            bool waitForVSync = false;
            std::unique_lock<std::mutex> locker(mutex_);
            CollectConns(waitForVSync, timestamp, conns);
            // no vsync signal
            if (timestamp == 0) {
                // there is some connections request next vsync, enable vsync if vsync disable and
                // and start the software vsync with wait_for function
                if (waitForVSync == true && vsyncEnabled_ == false) {
                    EnableVSync();
#if defined(RS_ENABLE_DVSYNC)
                    dvsync_->RNVNotify();
#endif
                    if (con_.wait_for(locker, std::chrono::milliseconds(SOFT_VSYNC_PERIOD)) ==
                        std::cv_status::timeout) {
                        event_.timestamp = Now();
                        event_.vsyncCount++;
                    }
                } else {
                    // just wait request or vsync signal
                    WaitForVsyncOrRequest(locker);
                }
                ScopedBytrace func(name_ + "_continue: waitForVSync " + std::to_string(waitForVSync) +
                    ", vsyncEnabled " + std::to_string(vsyncEnabled_) + ", dvsyncOn " + std::to_string(IsDVsyncOn()));
                continue;
            } else if ((timestamp > 0) && (waitForVSync == false)) {
                // if there is a vsync signal but no vaild connections, we should disable vsync
                ScopedBytrace func(name_ + "_DisableVSync, there is no valid connections");
                DisableVSync();
                continue;
            }
        }
        if (!PostVSyncEventPreProcess(timestamp, conns)) {
            continue;
        } else {
            // IMPORTANT: ScopedDebugTrace here will cause frame loss.
            ScopedBytrace func(name_ + "_SendVsync");
        }
        PostVSyncEvent(conns, timestamp);
    }
}

void VSyncDistributor::CollectConns(bool &waitForVSync, int64_t &timestamp,
    std::vector<sptr<VSyncConnection>> &conns)
{
    timestamp = event_.timestamp;
    event_.timestamp = 0;
    if (vsyncMode_ == VSYNC_MODE_LTPO) {
        CollectConnectionsLTPO(waitForVSync, timestamp, conns, event_.vsyncPulseCount);
    } else {
        CollectConnections(waitForVSync, timestamp, conns, event_.vsyncCount);
    }
}

bool VSyncDistributor::PostVSyncEventPreProcess(int64_t &timestamp, std::vector<sptr<VSyncConnection>> &conns)
{
#if defined(RS_ENABLE_DVSYNC)
    // ensure the preexecution only gets ahead for at most one period(i.e., 3 buffer rotation)
    if (IsDVsyncOn()) {
        int64_t periodBeforeDelay = 0L;
        int64_t periodAfterDelay = 0L;
        {
            std::unique_lock<std::mutex> locker(mutex_);
            periodBeforeDelay = event_.period;
            dvsync_->MarkDistributorSleep(true);
            dvsync_->RNVNotify();
            dvsync_->DelayBeforePostEvent(timestamp, locker);
            dvsync_->MarkDistributorSleep(false);
            periodAfterDelay = event_.period;
        }
        // if getting switched into vsync mode after sleep
        if (!IsDVsyncOn()) {
            ScopedBytrace func("NoAccumulateInVsync");
            lastDVsyncTS_.store(0);  // ensure further OnVSyncEvent do not skip
            for (auto conn : conns) {
                RequestNextVSync(conn);
            }  // resend RNV for vsync
            return false;  // do not accumulate frame;
        } else if (std::abs(periodAfterDelay - periodBeforeDelay) > MAX_PERIOD_BIAS) {
            timestamp = timestamp + periodAfterDelay - periodBeforeDelay;
            dvsync_->SetLastVirtualVSyncTS(timestamp);
        }
    }
    {
        std::unique_lock<std::mutex> locker(mutex_);
        pendingRNVInVsync_ = false;
    }
#endif
    return true;
}

void VSyncDistributor::EnableVSync()
{
    if (controller_ != nullptr && vsyncEnabled_ == false) {
        controller_->SetCallback(this);
        controller_->SetEnable(true, vsyncEnabled_);
    }
}

void VSyncDistributor::DisableVSync()
{
    if (controller_ != nullptr && vsyncEnabled_ == true) {
        controller_->SetEnable(false, vsyncEnabled_);
    }
}

#if defined(RS_ENABLE_DVSYNC)
void VSyncDistributor::OnDVSyncTrigger(int64_t now, int64_t period, uint32_t refreshRate, VSyncMode vsyncMode)
{
    std::lock_guard<std::mutex> locker(mutex_);
    vsyncMode_ = vsyncMode;
    event_.period = period;
    dvsync_->RuntimeSwitch();
    if (IsDVsyncOn()) {
        ScopedBytrace func("VSyncD onVSyncEvent, now:" + std::to_string(now));
    } else {
        ScopedBytrace func("VSync onVSyncEvent, now:" + std::to_string(now));
    }

    if (IsDVsyncOn()) {
        dvsync_->RecordVSync(now, period);
    }

    dvsync_->NotifyPreexecuteWait();

    int64_t lastDVsyncTS = lastDVsyncTS_.load();
    // when dvsync switch to vsync, skip all vsync events within one period from the pre-rendered timestamp
    if (!IsDVsyncOn() && now < (lastDVsyncTS + DVSYNC_ON_PERIOD - MAX_PERIOD_BIAS)) {
        ScopedBytrace func("skip DVSync prerendered frame, now: " + std::to_string(now) +
            ",lastDVsyncTS: " + std::to_string(lastDVsyncTS));
        return;
    }

    if (!IsDVsyncOn() || pendingRNVInVsync_) {
        event_.timestamp = now;
        event_.vsyncCount++;
    }

    if (refreshRate > 0) {
        event_.vsyncPulseCount += static_cast<int64_t>(VSYNC_MAX_REFRESHRATE / refreshRate);
        generatorRefreshRate_ = refreshRate;
    }

    ChangeConnsRateLocked();
    ScopedBytrace func("pendingRNVInVsync: " + std::to_string(pendingRNVInVsync_) + " DVSyncOn: " +
        std::to_string(IsDVsyncOn()));
    if (!IsDVsyncOn() || pendingRNVInVsync_) {
        con_.notify_all();
    } else {
        // When Dvsync on, if the RequestNextVsync is not invoked within three period and SetVSyncRate
        // is not invoked either, execute DisableVSync.
        for (uint32_t i = 0; i < connections_.size(); i++) {
            if (connections_[i]->triggerThisTime_ || connections_[i]->rate_ >= 0) {
                return;
            }
        }
        if (now - dvsync_->GetLastRnvTS() > 3 * period) {  // 3 period
            ScopedBytrace func(name_ + "_DisableVsync");
            DisableVSync();
        }
    }
}
#endif

void VSyncDistributor::OnVSyncTrigger(int64_t now, int64_t period, uint32_t refreshRate, VSyncMode vsyncMode)
{
    std::vector<sptr<VSyncConnection>> conns;
    bool waitForVSync = false;
    event_.vsyncCount++;

    if (refreshRate > 0) {
        event_.vsyncPulseCount += static_cast<int64_t>(VSYNC_MAX_REFRESHRATE / refreshRate);
        generatorRefreshRate_ = refreshRate;
    }
    vsyncMode_ = vsyncMode;
    ChangeConnsRateLocked();

    if (vsyncMode_ == VSYNC_MODE_LTPO) {
        CollectConnectionsLTPO(waitForVSync, now, conns, event_.vsyncPulseCount);
    } else {
        CollectConnections(waitForVSync, now, conns, event_.vsyncCount);
    }
    if (!waitForVSync) {
        DisableVSync();
        return;
    }

    countTraceValue_ = (countTraceValue_ + 1) % 2;  // 2 : change num
    CountTrace(HITRACE_TAG_GRAPHIC_AGP, "VSync-" + name_, countTraceValue_);

    for (uint32_t i = 0; i < conns.size(); i++) {
        int64_t actualPeriod = period;
        if ((generatorRefreshRate_ > 0) && (conns[i]->refreshRate_ > 0) &&
            (generatorRefreshRate_ % conns[i]->refreshRate_ == 0)) {
            actualPeriod = period * static_cast<int64_t>(generatorRefreshRate_ / conns[i]->refreshRate_);
        }
        int32_t ret = conns[i]->PostEvent(now, actualPeriod, event_.vsyncCount);
        VLOGD("Distributor name:%{public}s, connection name:%{public}s, ret:%{public}d",
            name_.c_str(), conns[i]->info_.name.c_str(), ret);
        if (ret == 0 || ret == ERRNO_OTHER) {
            RemoveConnection(conns[i]);
        } else if (ret == ERRNO_EAGAIN) {
            std::unique_lock<std::mutex> locker(mutex_);
            // Trigger VSync Again for LTPO
            conns[i]->triggerThisTime_ = true;
            // Exclude SetVSyncRate for LTPS
            if (conns[i]->rate_ < 0) {
                conns[i]->rate_ = 0;
            }
        }
    }
}

void VSyncDistributor::OnVSyncEvent(int64_t now, int64_t period, uint32_t refreshRate, VSyncMode vsyncMode)
{
#if defined(RS_ENABLE_DVSYNC)
    if (isRs_ && dvsync_->IsFeatureEnabled()) {
        OnDVSyncTrigger(now, period, refreshRate, vsyncMode);
    } else
#endif
    {
        OnVSyncTrigger(now, period, refreshRate, vsyncMode);
    }
}

/* std::pair<id, refresh rate> */
void VSyncDistributor::OnConnsRefreshRateChanged(const std::vector<std::pair<uint64_t, uint32_t>> &refreshRates)
{
    std::lock_guard<std::mutex> locker(changingConnsRefreshRatesMtx_);
    changingConnsRefreshRates_ = refreshRates;
}

void VSyncDistributor::SubScribeSystemAbility(const std::string& threadName)
{
    VLOGD("%{public}s", __func__);
    sptr<ISystemAbilityManager> systemAbilityManager =
        SystemAbilityManagerClient::GetInstance().GetSystemAbilityManager();
    if (!systemAbilityManager) {
        VLOGE("%{public}s failed to get system ability manager client", __func__);
        return;
    }
    std::string strUid = std::to_string(getuid());
    std::string strPid = std::to_string(getpid());
    std::string strTid = std::to_string(gettid());

    saStatusChangeListener_ = new (std::nothrow)VSyncSystemAbilityListener(threadName, strUid, strPid, strTid);
    int32_t ret = systemAbilityManager->SubscribeSystemAbility(RES_SCHED_SYS_ABILITY_ID, saStatusChangeListener_);
    if (ret != ERR_OK) {
        VLOGE("%{public}s subscribe system ability %{public}d failed.", __func__, RES_SCHED_SYS_ABILITY_ID);
        saStatusChangeListener_ = nullptr;
    }
}

void VSyncDistributor::CollectConnections(bool &waitForVSync, int64_t timestamp,
                                          std::vector<sptr<VSyncConnection>> &conns, int64_t vsyncCount)
{
    for (uint32_t i = 0; i < connections_.size(); i++) {
        int32_t rate = connections_[i]->highPriorityState_ ? connections_[i]->highPriorityRate_ :
                                                             connections_[i]->rate_;
        if (rate == 0) {  // for RequestNextVSync
            waitForVSync = true;
            if (timestamp > 0) {
                connections_[i]->rate_ = -1;
                conns.push_back(connections_[i]);
                connections_[i]->triggerThisTime_ = false;
            }
        } else if (rate > 0) {
            ScopedBytrace trace("CollectConnections name:" + connections_[i]->info_.name_ +
                                ", proxyPid:" + std::to_string(connections_[i]->proxyPid_) +
                                ", highPriorityState_:" + std::to_string(connections_[i]->highPriorityState_) +
                                ", highPriorityRate_:" + std::to_string(connections_[i]->highPriorityRate_) +
                                ", rate_:" + std::to_string(connections_[i]->rate_) +
                                ", timestamp:" + std::to_string(timestamp) +
                                ", vsyncCount:" + std::to_string(vsyncCount));
            if (connections_[i]->rate_ == 0) {  // for SetHighPriorityVSyncRate with RequestNextVSync
                waitForVSync = true;
                if (timestamp > 0 && (vsyncCount % rate == 0)) {
                    connections_[i]->rate_ = -1;
                    conns.push_back(connections_[i]);
                    connections_[i]->triggerThisTime_ = false;
                }
            } else if (connections_[i]->rate_ > 0) {  // for SetVSyncRate
                waitForVSync = true;
                if (timestamp > 0 && (vsyncCount % rate == 0)) {
                    conns.push_back(connections_[i]);
                }
            }
        }
    }
}

void VSyncDistributor::CollectConnectionsLTPO(bool &waitForVSync, int64_t timestamp,
                                              std::vector<sptr<VSyncConnection>> &conns, int64_t vsyncCount)
{
    for (uint32_t i = 0; i < connections_.size(); i++) {
        ScopedDebugTrace trace("CollectConnectionsLTPO, i:" + std::to_string(i) +
                               ", name:" + connections_[i]->info_.name_ +
                               ", rate:" + std::to_string(connections_[i]->rate_) +
                               ", vsyncPulseFreq:" + std::to_string(connections_[i]->vsyncPulseFreq_) +
                               ", referencePulseCount:" + std::to_string(connections_[i]->referencePulseCount_) +
                               ", vsyncCount:" + std::to_string(vsyncCount));
        if (!connections_[i]->triggerThisTime_ && connections_[i]->rate_ <= 0) {
            continue;
        }
        waitForVSync = true;
        int64_t vsyncPulseFreq = static_cast<int64_t>(connections_[i]->vsyncPulseFreq_);
        if (timestamp > 0 &&
            (vsyncCount - connections_[i]->referencePulseCount_) % vsyncPulseFreq == 0) {
            conns.push_back(connections_[i]);
            connections_[i]->triggerThisTime_ = false;
            if (connections_[i]->rate_ == 0) {
                connections_[i]->rate_ = -1;
            }
        }
    }
}

void VSyncDistributor::PostVSyncEvent(const std::vector<sptr<VSyncConnection>> &conns, int64_t timestamp)
{
    for (uint32_t i = 0; i < conns.size(); i++) {
        int64_t period = event_.period;
        if ((generatorRefreshRate_ > 0) && (conns[i]->refreshRate_ > 0) &&
            (generatorRefreshRate_ % conns[i]->refreshRate_ == 0)) {
            period = event_.period * static_cast<int64_t>(generatorRefreshRate_ / conns[i]->refreshRate_);
        }
        int32_t ret = conns[i]->PostEvent(timestamp, period, event_.vsyncCount);
        VLOGD("Distributor name:%{public}s, connection name:%{public}s, ret:%{public}d",
            name_.c_str(), conns[i]->info_.name_.c_str(), ret);
        if (ret == 0 || ret == ERRNO_OTHER) {
            RemoveConnection(conns[i]);
        } else if (ret == ERRNO_EAGAIN) {
            std::unique_lock<std::mutex> locker(mutex_);
            // Trigger VSync Again for LTPO
            conns[i]->triggerThisTime_ = true;
            // Exclude SetVSyncRate for LTPS
            if (conns[i]->rate_ < 0) {
                conns[i]->rate_ = 0;
            }
        }
    }
}

VsyncError VSyncDistributor::RequestNextVSync(const sptr<VSyncConnection> &connection, const std::string &fromWhom,
                                              int64_t lastVSyncTS)
{
    if (connection == nullptr) {
        VLOGE("connection is nullptr");
        return VSYNC_ERROR_NULLPTR;
    }

#if defined(RS_ENABLE_DVSYNC)
    if (IsDVsyncOn() && fromWhom == "ltpoForceUpdate") {
        return VSYNC_ERROR_OK;
    }
#endif

    ScopedBytrace func(connection->info_.name_ + "_RequestNextVSync");
    std::unique_lock<std::mutex> locker(mutex_);

#if defined(RS_ENABLE_DVSYNC)
    if (IsDVsyncOn()) {
        dvsync_->RNVWait(locker);
    }
#endif

    auto it = find(connections_.begin(), connections_.end(), connection);
    if (it == connections_.end()) {
        VLOGE("connection is invalid arguments");
#if defined(RS_ENABLE_DVSYNC)
        if (IsDVsyncOn()) {
            dvsync_->RNVNotify();
        }
#endif
        return VSYNC_ERROR_INVALID_ARGUMENTS;
    }
    // record RNV and lastVSyncTS for D-VSYNC
#if defined(RS_ENABLE_DVSYNC)
    if (IsDVsyncOn()) {
        dvsync_->RecordRNV(fromWhom, lastVSyncTS);
    } else {
        pendingRNVInVsync_ = true;
    }
#endif
    if (connection->rate_ < 0) {
        connection->rate_ = 0;
    }
    connection->triggerThisTime_ = true;
#if defined(RS_ENABLE_DVSYNC)
    if (isRs_ && dvsync_->IsFeatureEnabled()) {
        con_.notify_all();
    } else
#else
    {
        EnableVSync();
    }
#endif
    VLOGD("conn name:%{public}s, rate:%{public}d", connection->info_.name_.c_str(), connection->rate_);
    return VSYNC_ERROR_OK;
}

VsyncError VSyncDistributor::SetVSyncRate(int32_t rate, const sptr<VSyncConnection>& connection)
{
    if (rate < -1 || connection == nullptr) {
        return VSYNC_ERROR_INVALID_ARGUMENTS;
    }
    std::lock_guard<std::mutex> locker(mutex_);
    auto it = find(connections_.begin(), connections_.end(), connection);
    if (it == connections_.end()) {
        return VSYNC_ERROR_INVALID_ARGUMENTS;
    }
    if (connection->rate_ == rate) {
        return VSYNC_ERROR_INVALID_ARGUMENTS;
    }
    connection->rate_ = rate;
    VLOGD("conn name:%{public}s", connection->info_.name_.c_str());
#if defined(RS_ENABLE_DVSYNC)
    if (isRs_ && dvsync_->IsFeatureEnabled()) {
        con_.notify_all();
    } else
#endif
    {
        EnableVSync();
    }
    return VSYNC_ERROR_OK;
}

VsyncError VSyncDistributor::SetHighPriorityVSyncRate(int32_t highPriorityRate, const sptr<VSyncConnection>& connection)
{
    if (highPriorityRate <= 0 || connection == nullptr) {
        return VSYNC_ERROR_INVALID_ARGUMENTS;
    }

    std::lock_guard<std::mutex> locker(mutex_);
    auto it = find(connections_.begin(), connections_.end(), connection);
    if (it == connections_.end()) {
        return VSYNC_ERROR_INVALID_ARGUMENTS;
    }
    if (connection->highPriorityRate_ == highPriorityRate) {
        return VSYNC_ERROR_INVALID_ARGUMENTS;
    }
    connection->highPriorityRate_ = highPriorityRate;
    connection->highPriorityState_ = true;
    VLOGD("in, conn name:%{public}s, highPriorityRate:%{public}d", connection->info_.name_.c_str(),
          connection->highPriorityRate_);
#if defined(RS_ENABLE_DVSYNC)
    if (isRs_ && dvsync_->IsFeatureEnabled()) {
        con_.notify_all();
    } else
#endif
    {
        EnableVSync();
    }
    return VSYNC_ERROR_OK;
}

VsyncError VSyncDistributor::GetVSyncConnectionInfos(std::vector<ConnectionInfo>& infos)
{
    infos.clear();
    for (auto &connection : connections_) {
        infos.push_back(connection->info_);
    }
    return VSYNC_ERROR_OK;
}

VsyncError VSyncDistributor::QosGetPidByName(const std::string& name, uint32_t& pid)
{
    if (name.find("WM") == std::string::npos && name.find("NWeb") == std::string::npos) {
        return VSYNC_ERROR_INVALID_ARGUMENTS;
    }
    std::string::size_type pos = name.find("_");
    if (pos == std::string::npos) {
        return VSYNC_ERROR_INVALID_ARGUMENTS;
    }
    pid = (uint32_t)stoi(name.substr(pos + 1));
    return VSYNC_ERROR_OK;
}

VsyncError VSyncDistributor::SetQosVSyncRateByPid(uint32_t pid, int32_t rate)
{
    auto iter = connectionsMap_.find(pid);
    if (iter == connectionsMap_.end()) {
        VLOGD("%{public}s:%{public}d pid[%{public}u] can not found", __func__, __LINE__, pid);
        return VSYNC_ERROR_INVALID_ARGUMENTS;
    }
#if defined(RS_ENABLE_DVSYNC)
    bool isNeedNotify = false;
#endif
    for (auto connection : iter->second) {
        uint32_t tmpPid;
        if (QosGetPidByName(connection->info_.name_, tmpPid) != VSYNC_ERROR_OK || tmpPid != pid) {
            continue;
        }
        if (connection->highPriorityRate_ != rate) {
            connection->highPriorityRate_ = rate;
            connection->highPriorityState_ = true;
            VLOGD("in, conn name:%{public}s, highPriorityRate:%{public}d", connection->info_.name_.c_str(),
                connection->highPriorityRate_);
#if defined(RS_ENABLE_DVSYNC)
            isNeedNotify = true;
#endif
        }
    }
#if defined(RS_ENABLE_DVSYNC)
    if (isNeedNotify && isRs_ && dvsync_->IsFeatureEnabled()) {
        con_.notify_all();
    } else
#endif
    {
        EnableVSync();
    }
    return VSYNC_ERROR_OK;
}

constexpr pid_t VSyncDistributor::ExtractPid(uint64_t id)
{
    constexpr uint32_t bits = 32u;
    return static_cast<pid_t>(id >> bits);
}

VsyncError VSyncDistributor::SetQosVSyncRate(uint64_t windowNodeId, int32_t rate)
{
    std::lock_guard<std::mutex> locker(mutex_);
    VsyncError resCode = SetQosVSyncRateByPid(ExtractPid(windowNodeId), rate);
    auto iter = connectionsMap_.find(windowNodeId);
    if (iter == connectionsMap_.end()) {
        return resCode;
    }
#if defined(RS_ENABLE_DVSYNC)
    bool isNeedNotify = false;
#endif
    for (auto& connection : iter->second) {
        if (connection && connection->highPriorityRate_ != rate) {
            connection->highPriorityRate_ = rate;
            connection->highPriorityState_ = true;
            VLOGD("in, conn name:%{public}s, highPriorityRate:%{public}d", connection->info_.name_.c_str(),
                connection->highPriorityRate_);
#if defined(RS_ENABLE_DVSYNC)
            isNeedNotify = true;
#endif
        }
    }
#if defined(RS_ENABLE_DVSYNC)
    if (isNeedNotify && isRs_ && dvsync_->IsFeatureEnabled()) {
        con_.notify_all();
    } else
#endif
    {
        EnableVSync();
    }
    return VSYNC_ERROR_OK;
}

VsyncError VSyncDistributor::GetQosVSyncRateInfos(std::vector<std::pair<uint32_t, int32_t>>& vsyncRateInfos)
{
    vsyncRateInfos.clear();

    std::lock_guard<std::mutex> locker(mutex_);
    for (auto &connection : connections_) {
        uint32_t tmpPid;
        if (QosGetPidByName(connection->info_.name_, tmpPid) != VSYNC_ERROR_OK) {
            continue;
        }
        int32_t tmpRate = connection->highPriorityState_ ? connection->highPriorityRate_ : connection->rate_;
        vsyncRateInfos.push_back(std::make_pair(tmpPid, tmpRate));
    }
    return VSYNC_ERROR_OK;
}

void VSyncDistributor::ChangeConnsRateLocked()
{
    std::lock_guard<std::mutex> locker(changingConnsRefreshRatesMtx_);
    for (auto connRefreshRate : changingConnsRefreshRates_) {
        for (auto conn : connections_) {
            if (conn->id_ != connRefreshRate.first) {
                continue;
            }
            uint32_t refreshRate = connRefreshRate.second;
            if ((generatorRefreshRate_ == 0) || (refreshRate == 0) ||
                (VSYNC_MAX_REFRESHRATE % refreshRate != 0) || (generatorRefreshRate_ % refreshRate != 0)) {
                conn->refreshRate_ = 0;
                conn->vsyncPulseFreq_ = 1;
                continue;
            }
            conn->refreshRate_ = refreshRate;
            conn->vsyncPulseFreq_ = VSYNC_MAX_REFRESHRATE / refreshRate;
            conn->referencePulseCount_ = event_.vsyncPulseCount;
        }
    }
    changingConnsRefreshRates_.clear();
}

bool VSyncDistributor::IsDVsyncOn()
{
#if defined(RS_ENABLE_DVSYNC)
    return isRs_ && dvsync_->IsEnabledAndOn();
#else
    return false;
#endif
}

void VSyncDistributor::SetFrameIsRender(bool isRender)
{
#if defined(RS_ENABLE_DVSYNC)
    std::unique_lock<std::mutex> locker(mutex_);
    ScopedBytrace trace("SetFrameIsRender");
    if (isRender) {
        dvsync_->UnMarkRSNotRendering();
    } else {
        dvsync_->MarkRSNotRendering();
    }
#endif
}

void VSyncDistributor::MarkRSAnimate()
{
#if defined(RS_ENABLE_DVSYNC)
    dvsync_->MarkRSAnimate();
#endif
}

void VSyncDistributor::UnmarkRSAnimate()
{
#if defined(RS_ENABLE_DVSYNC)
    dvsync_->UnmarkRSAnimate();
#endif
}

bool VSyncDistributor::HasPendingUIRNV()
{
#if defined(RS_ENABLE_DVSYNC)
    return dvsync_->HasPendingUIRNV();
#else
    return false;
#endif
}
}
}
