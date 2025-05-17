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
#include <cinttypes>
#include <cstdint>
#include <mutex>
#include <sched.h>
#include <sys/resource.h>
#include <scoped_bytrace.h>
#include <hitrace_meter.h>
#include "vsync_log.h"
#include "vsync_type.h"
#include "vsync_generator.h"
#include <rs_trace.h>
#include "scoped_trace_fmt.h"

#ifdef COMPOSER_SCHED_ENABLE
#include "if_system_ability_manager.h"
#include <iservice_registry.h>
#include "system_ability_definition.h"
#endif

#if defined(RS_ENABLE_DVSYNC_2)
#include "dvsync.h"
#include "dvsync_controller.h"
#endif

namespace OHOS {
namespace Rosen {
namespace {
constexpr int32_t SOFT_VSYNC_PERIOD = 16;
constexpr int32_t ERRNO_EAGAIN = -1;
constexpr int32_t ERRNO_OTHER = -2;
constexpr int32_t THREAD_PRIORTY = -6;
constexpr int32_t SCHED_PRIORITY = 2;
constexpr int32_t DEFAULT_VSYNC_RATE = 1;
constexpr uint32_t SOCKET_CHANNEL_SIZE = 1024;
constexpr int32_t VSYNC_CONNECTION_MAX_SIZE = 256;
constexpr std::string_view URGENT_SELF_DRAWING = "UrgentSelfdrawing";
constexpr int32_t MAX_VSYNC_QUEUE_SIZE = 100;
constexpr int64_t VSYNC_TIME_TOLERANCE_THRESHOLD = 2000000;
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
        RS_TRACE_NAME_FMT("Create socket channel failed, errno = %d", errno);
    }
    proxyPid_ = GetCallingPid();
    isDead_ = false;
}

void VSyncConnection::RegisterDeathRecipient()
{
    if (token_ != nullptr) {
        token_->AddDeathRecipient(vsyncConnDeathRecipient_);
    }
}

VSyncConnection::~VSyncConnection()
{
    if ((token_ != nullptr) && (vsyncConnDeathRecipient_ != nullptr)) {
        token_->RemoveDeathRecipient(vsyncConnDeathRecipient_);
    }
}

VsyncError VSyncConnection::RequestNextVSync()
{
    static const std::string DEFAULT_REQUEST = "unknown";
    return RequestNextVSync(DEFAULT_REQUEST, 0);
}

VsyncError VSyncConnection::RequestNextVSync(
    const std::string &fromWhom, int64_t lastVSyncTS, const int64_t& requestVsyncTime)
{
    sptr<VSyncDistributor> distributor;
    {
        std::unique_lock<std::mutex> locker(mutex_);
        if (isDead_) {
            VLOGE("%{public}s VSync Client Connection is dead, name:%{public}s.", __func__, info_.name_.c_str());
            return VSYNC_ERROR_API_FAILED;
        }
        if (distributor_ == nullptr) {
            VLOGE("%{public}s distributor_ is null, name:%{public}s.", __func__, info_.name_.c_str());
            return VSYNC_ERROR_NULLPTR;
        }
        distributor = distributor_.promote();
        if (distributor == nullptr) {
            VLOGE("%{public}s distributor is null, name:%{public}s.", __func__, info_.name_.c_str());
            return VSYNC_ERROR_NULLPTR;
        }
        if (isFirstRequestVsync_) {
            isFirstRequestVsync_ = false;
            distributor->FirstRequestVsync();
            VLOGI("First vsync is requested, name: %{public}s", info_.name_.c_str());
        }
        if (requestNativeVSyncCallback_ != nullptr) {
            requestNativeVSyncCallback_();
        }
    }
    return distributor->RequestNextVSync(this, fromWhom, lastVSyncTS, requestVsyncTime);
}

VsyncError VSyncConnection::GetReceiveFd(int32_t &fd)
{
    std::unique_lock<std::mutex> locker(mutex_);
    if (isDead_ || socketPair_ == nullptr) {
        VLOGE("%{public}s VSync Client Connection is dead, name:%{public}s.", __func__, info_.name_.c_str());
        return VSYNC_ERROR_API_FAILED;
    }
    fd = socketPair_->GetReceiveDataFd();
    if (fd < 0) {
        VLOGE("%{public}s socketPair invalid fd:%{public}d.", __func__, fd);
        return VSYNC_ERROR_API_FAILED;
    }
    return VSYNC_ERROR_OK;
}

int32_t VSyncConnection::PostEvent(int64_t now, int64_t period, int64_t vsyncCount)
{
    sptr<LocalSocketPair> socketPair;
    {
        std::unique_lock<std::mutex> locker(mutex_);
        if (isDead_) {
            RS_TRACE_NAME_FMT("Vsync Client Connection is dead, conn: %s", info_.name_.c_str());
            VLOGE("%{public}s VSync Client Connection is dead, name:%{public}s.", __func__, info_.name_.c_str());
            return ERRNO_OTHER;
        }
        socketPair = socketPair_;
    }
    if (socketPair == nullptr) {
        RS_TRACE_NAME_FMT("socketPair is null, conn: %s", info_.name_.c_str());
        return ERRNO_OTHER;
    }

    std::unique_lock<std::mutex> lockerPostEvent(postEventMutex_);
    RS_TRACE_NAME_FMT("SendVsyncTo conn: %s, now:%ld, refreshRate:%d", info_.name_.c_str(), now, refreshRate_);
    // 3 is array size.
    int64_t data[3];
    data[0] = now;
    // 1, 2: index of array data.
    data[1] = period;
    data[2] = vsyncCount;
    if (isFirstSendVsync_) {
        isFirstSendVsync_ = false;
        VLOGI("First vsync has send to : %{public}s", info_.name_.c_str());
    }
    int32_t ret = socketPair->SendData(data, sizeof(data));
    if (ret == ERRNO_EAGAIN) {
        RS_TRACE_NAME("remove the earlies data and SendData again.");
        int64_t receiveData[3];
        socketPair->ReceiveData(receiveData, sizeof(receiveData));
        ret = socketPair->SendData(data, sizeof(data));
        VLOGW("vsync signal is not processed in time, please check pid:%{public}d, ret:%{public}d", proxyPid_, ret);
    }
    if (ret > -1) {
        ScopedDebugTrace successful("successful");
        info_.postVSyncCount_++;
        if (gcNotifyTask_ != nullptr) {
            gcNotifyTask_(false);
        }
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

void VSyncConnection::AddRequestVsyncTimestamp(const int64_t& timestamp)
{
    std::lock_guard<std::recursive_mutex> lock(vsyncTimeMutex_);
    if (requestVsyncTimestamp_.size() > MAX_VSYNC_QUEUE_SIZE || timestamp < 0) {
        requestVsyncTimestamp_.insert(0);
        return;
    }
    requestVsyncTimestamp_.insert(timestamp);
}

void VSyncConnection::RemoveTriggeredVsync(const int64_t& currentTime)
{
    std::lock_guard<std::recursive_mutex> lock(vsyncTimeMutex_);
    for (auto iter = requestVsyncTimestamp_.begin(); iter != requestVsyncTimestamp_.end();) {
        if (*iter - VSYNC_TIME_TOLERANCE_THRESHOLD <= currentTime) {
            iter = requestVsyncTimestamp_.erase(iter);
        } else {
            return;
        }
    }
}

bool VSyncConnection::NeedTriggeredVsync(const int64_t& currentTime)
{
    std::lock_guard<std::recursive_mutex> lock(vsyncTimeMutex_);
    if (requestVsyncTimestamp_.size() == 0) {
        return false;
    }
    int64_t timesDiff = *(requestVsyncTimestamp_.begin()) - currentTime;
    auto isNeedTriggered = timesDiff <= VSYNC_TIME_TOLERANCE_THRESHOLD;
    if (!isNeedTriggered) {
        RS_TRACE_NAME_FMT("Skip Vsync, name: %s, trigger time: %lld", info_.name_.c_str(), timesDiff);
    }
    return isNeedTriggered;
}

VsyncError VSyncConnection::SetUiDvsyncSwitch(bool dvsyncSwitch)
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
    return distributor->SetUiDvsyncSwitch(dvsyncSwitch, this);
}

VsyncError VSyncConnection::SetNativeDVSyncSwitch(bool dvsyncSwitch)
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
    return distributor->SetNativeDVSyncSwitch(dvsyncSwitch, this);
}

VsyncError VSyncConnection::SetUiDvsyncConfig(int32_t bufferCount)
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
    return distributor->SetUiDvsyncConfig(bufferCount);
}

void VSyncConnection::RegisterRequestNativeVSyncCallback(const RequestNativeVSyncCallback &callback)
{
    std::unique_lock<std::mutex> locker(mutex_);
    requestNativeVSyncCallback_ = callback;
}

VSyncDistributor::VSyncDistributor(sptr<VSyncController> controller, std::string name, DVSyncFeatureParam dvsyncParam)
    : controller_(controller), mutex_(), con_(), connections_(),
    event_(), vsyncEnabled_(false), name_(name)
{
    static const std::string DEFAULT_RS_NAME = "rs";
    if (name == DEFAULT_RS_NAME) {
        isRs_ = true;
    }
#if defined(RS_ENABLE_DVSYNC)
    dvsync_ = new DVsync(isRs_);
    if (dvsync_->IsFeatureEnabled()) {
        vsyncThreadRunning_ = true;
        threadLoop_ = std::thread([this] { this->ThreadMain(); });
        std::string threadName = "DVSync-" + name;
        pthread_setname_np(threadLoop_.native_handle(), threadName.c_str());
    }
#endif
    // Start of DVSync
    InitDVSync(dvsyncParam);
    // End of DVSync
}

VSyncDistributor::~VSyncDistributor()
{
#if defined(RS_ENABLE_DVSYNC)
    if (dvsync_->IsFeatureEnabled()) {
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
    std::lock_guard<std::mutex> locker(mutex_);
    int32_t proxyPid = connection->proxyPid_;
    if (connectionCounter_[proxyPid] > VSYNC_CONNECTION_MAX_SIZE) {
        VLOGE("You [%{public}d] have created too many vsync connection, please check!!!", proxyPid);
        return VSYNC_ERROR_API_FAILED;
    }
    auto it = std::find(connections_.begin(), connections_.end(), connection);
    if (it != connections_.end()) {
        return VSYNC_ERROR_INVALID_ARGUMENTS;
    }
    RS_TRACE_NAME_FMT("Add VSyncConnection: %s", connection->info_.name_.c_str());
    connections_.push_back(connection);
    connMap_[connection->id_] = connection;
    // Start of DVSync
    DVSyncAddConnection(connection);
    // End of DVSync
    connectionCounter_[proxyPid]++;
    if (windowNodeId != 0) {
        connectionsMap_[windowNodeId].push_back(connection);
        uint32_t tmpPid;
        if (QosGetPidByName(connection->info_.name_, tmpPid) == VSYNC_ERROR_OK) {
            std::vector<uint64_t> tmpVec = pidWindowIdMap_[tmpPid];
            if (std::find(tmpVec.begin(), tmpVec.end(), windowNodeId) == tmpVec.end()) {
                pidWindowIdMap_[tmpPid].push_back(windowNodeId);
            }
        }
    } else {
        pid_t extractPid = ExtractPid(connection->id_);
        if (extractPid != 0) {
            unalliedWindowConnectionsMap_[extractPid].push_back(connection);
        }

        uint32_t tmpPid;
        if (QosGetPidByName(connection->info_.name_, tmpPid) == VSYNC_ERROR_OK) {
            connectionsMap_[tmpPid].push_back(connection);
        }
    }
    connection->RegisterDeathRecipient();
    return VSYNC_ERROR_OK;
}

sptr<VSyncConnection> VSyncDistributor::GetVSyncConnection(uint64_t id)
{
    std::lock_guard<std::mutex> locker(mutex_);
    for (const auto& conn : connections_) {
        if (conn != nullptr && conn->id_ == id) {
            return conn;
        }
    }
    return nullptr;
}

VsyncError VSyncDistributor::RemoveConnection(const sptr<VSyncConnection>& connection)
{
    if (connection == nullptr) {
        return VSYNC_ERROR_NULLPTR;
    }
    std::lock_guard<std::mutex> locker(mutex_);
    int32_t proxyPid = connection->proxyPid_;
    auto it = std::find(connections_.begin(), connections_.end(), connection);
    if (it == connections_.end()) {
        return VSYNC_ERROR_INVALID_ARGUMENTS;
    }
    RS_TRACE_NAME_FMT("Remove VSyncConnection: %s", connection->info_.name_.c_str());
    connections_.erase(it);
    connMap_.erase(connection->id_);
    connectionCounter_[proxyPid]--;
    if (connectionCounter_[proxyPid] == 0) {
        connectionCounter_.erase(proxyPid);
    }
    connectionsMap_.erase(connection->windowNodeId_);
    auto unalliedWindowConns = unalliedWindowConnectionsMap_.find(ExtractPid(connection->id_));
    if (unalliedWindowConns != unalliedWindowConnectionsMap_.end()) {
        auto connIter = find(unalliedWindowConns->second.begin(), unalliedWindowConns->second.end(), connection);
        if (connIter != unalliedWindowConns->second.end()) {
            unalliedWindowConns->second.erase(connIter);
        }
        if (unalliedWindowConns->second.empty()) {
            unalliedWindowConnectionsMap_.erase(unalliedWindowConns);
        }
    }
    uint32_t tmpPid;
    if (QosGetPidByName(connection->info_.name_, tmpPid) == VSYNC_ERROR_OK) {
        pidWindowIdMap_.erase(tmpPid);
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
    beforeWaitRnvTime_ = Now();
    dvsync_->RNVNotify();
    if (!isRs_ && IsDVsyncOn()) {
        con_.wait_for(locker, std::chrono::nanoseconds(dvsync_->WaitTime()), [this] { return dvsync_->WaitCond(); });
    } else {
        if (!(hasVsync_.load() && isRs_)) {
            con_.wait(locker);
        }
        hasVsync_.store(false);
    }
    afterWaitRnvTime_ = Now();
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
#else
    con_.wait(locker);
#endif
}

int64_t Now()
{
    const auto &now = std::chrono::steady_clock::now().time_since_epoch();
    return std::chrono::duration_cast<std::chrono::nanoseconds>(now).count();
}

void VSyncDistributor::WaitForVsyncOrTimeOut(std::unique_lock<std::mutex> &locker)
{
#if defined(RS_ENABLE_DVSYNC)
    dvsync_->RNVNotify();
#endif
    beforeWaitRnvTime_ = Now();
    if (con_.wait_for(locker, std::chrono::milliseconds(SOFT_VSYNC_PERIOD)) ==
        std::cv_status::timeout) {
        event_.timestamp = Now();
        event_.vsyncCount++;
    }
    afterWaitRnvTime_ = Now();
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
            CollectConns(waitForVSync, timestamp, conns, true);
            // no vsync signal
            if (timestamp == 0) {
                // there is some connections request next vsync, enable vsync if vsync disable and
                // and start the software vsync with wait_for function
                if (waitForVSync == true && vsyncEnabled_ == false) {
                    EnableVSync();
                    WaitForVsyncOrTimeOut(locker);
                } else {
                    // just wait request or vsync signal
                    WaitForVsyncOrRequest(locker);
                }
                RS_TRACE_NAME_FMT("%s_continue: waitForVSync %d, vsyncEnabled %d, dvsyncOn %d",
                    name_.c_str(), waitForVSync, vsyncEnabled_, IsDVsyncOn());
                if ((isRs_ && event_.timestamp == 0) || !IsDVsyncOn()) {
                    continue;
                } else {
                    timestamp = event_.timestamp;
                }
            } else if ((timestamp > 0) && (waitForVSync == false) && (isRs_ || !IsDVsyncOn())) {
                // if there is a vsync signal but no vaild connections, we should disable vsync
                RS_TRACE_NAME_FMT("%s_DisableVSync, there is no valid connections", name_.c_str());
                VLOGI("%s_DisableVSync, there is no valid connections", name_.c_str());
                DisableVSync();
                continue;
            }
        }
        if (!PostVSyncEventPreProcess(timestamp, conns)) {
            continue;
        } else {
            // IMPORTANT: ScopedDebugTrace here will cause frame loss.
            RS_TRACE_NAME_FMT("%s_SendVsync", name_.c_str());
        }
        PostVSyncEvent(conns, timestamp, true);
    }
}

void VSyncDistributor::CollectConns(bool &waitForVSync, int64_t &timestamp,
    std::vector<sptr<VSyncConnection>> &conns, bool isDvsyncThread)
{
    if (isDvsyncThread) {
        timestamp = event_.timestamp;
        event_.timestamp = 0;
    }
    if (vsyncMode_ == VSYNC_MODE_LTPO) {
        CollectConnectionsLTPO(waitForVSync, timestamp, conns, event_.vsyncPulseCount, isDvsyncThread);
    } else {
        CollectConnections(waitForVSync, timestamp, conns, event_.vsyncCount, isDvsyncThread);
    }
}

bool VSyncDistributor::PostVSyncEventPreProcess(int64_t &timestamp, std::vector<sptr<VSyncConnection>> &conns)
{
#if defined(RS_ENABLE_DVSYNC)
    bool waitForVSync = false;
    // ensure the preexecution only gets ahead for at most one period(i.e., 3 buffer rotation)
    if (IsDVsyncOn()) {
        {
            std::unique_lock<std::mutex> locker(mutex_);
            dvsync_->MarkDistributorSleep(true);
            dvsync_->RNVNotify();
            dvsync_->DelayBeforePostEvent(event_.timestamp, locker);
            dvsync_->MarkDistributorSleep(false);
            CollectConns(waitForVSync, timestamp, conns, true);
            hasVsync_.store(false);
        }
        // if getting switched into vsync mode after sleep
        if (!IsDVsyncOn()) {
            ScopedBytrace func("NoAccumulateInVsync");
            lastDVsyncTS_.store(0);  // ensure further OnVSyncEvent do not skip
            for (auto conn : conns) {
                RequestNextVSync(conn);
            }  // resend RNV for vsync
            if (isRs_) {
                VLOGW("DVSync is close");
            }
            return false;  // do not accumulate frame;
        }
    } else {
        std::unique_lock<std::mutex> locker(mutex_);
        hasVsync_.store(false);
    }
    {
        std::unique_lock<std::mutex> locker(mutex_);
        pendingRNVInVsync_ = false;
        if (IsUiDvsyncOn()) {
            event_.period = dvsync_->GetPeriod();
        }
    }
#endif
    return true;
}

void VSyncDistributor::EnableVSync()
{
    if (controller_ != nullptr && vsyncEnabled_ == false) {
        controller_->SetCallback(this);
        controller_->SetEnable(true, vsyncEnabled_);
        // Start of DVSync
        RecordEnableVsync();
    }
    if (dvsyncController_!= nullptr && dvsyncControllerEnabled_ == false) {
        ScopedBytrace func("dvsyncController enable");
        dvsyncController_->SetCallback(this);
        dvsyncController_->SetEnable(true, dvsyncControllerEnabled_);
    }
    // End of DVSync
#if defined(RS_ENABLE_DVSYNC)
    dvsync_->RecordEnableVsync();
#endif
}

void VSyncDistributor::DisableVSync()
{
    if (controller_ != nullptr && vsyncEnabled_ == true) {
        controller_->SetEnable(false, vsyncEnabled_);
    }
    // Start of DVSync
    DVSyncDisableVSync();
    // End of DVSync
}

#if defined(RS_ENABLE_DVSYNC)
void VSyncDistributor::OnDVSyncTrigger(int64_t now, int64_t period,
    uint32_t refreshRate, VSyncMode vsyncMode, uint32_t vsyncMaxRefreshRate)
{
    std::unique_lock<std::mutex> locker(mutex_);
    if (isFirstSend_) {
        isFirstSend_ = false;
        VLOGI("First vsync OnDVSyncTrigger");
    }
    vsyncMode_ = vsyncMode;
    dvsync_->RuntimeSwitch();
    if (IsDVsyncOn()) {
        RS_TRACE_NAME_FMT("VSyncD onVSyncEvent, now:%ld", now);
    } else {
        RS_TRACE_NAME_FMT("VSync onVSyncEvent, now:%ld", now);
    }
    event_.period = period;

    dvsync_->RecordVSync(now, period, refreshRate);
    dvsync_->NotifyPreexecuteWait();

    SendConnectionsToVSyncWindow(now, period, refreshRate, vsyncMode, locker);
    // when dvsync switch to vsync, skip all vsync events within one period from the pre-rendered timestamp
    if (dvsync_->NeedSkipDVsyncPrerenderedFrame()) {
        return;
    }

    // When vsync switches to dvsync, need to handle pending RNVs during vsync
    if (!IsDVsyncOn() || pendingRNVInVsync_) {
        event_.timestamp = now;
        event_.vsyncCount++;
    }

    if (refreshRate > 0) {
        event_.vsyncPulseCount += static_cast<int64_t>(vsyncMaxRefreshRate / refreshRate);
        generatorRefreshRate_ = refreshRate;
    }

    ChangeConnsRateLocked(vsyncMaxRefreshRate);
    RS_TRACE_NAME_FMT("pendingRNVInVsync: %d DVSyncOn: %d isRS:%d", pendingRNVInVsync_, IsDVsyncOn(), isRs_);
    if (dvsync_->WaitCond() || pendingRNVInVsync_) {
        con_.notify_all();
    } else {
        CheckNeedDisableDvsync(now, period);
    }
}
#endif

void VSyncDistributor::OnVSyncTrigger(int64_t now, int64_t period,
    uint32_t refreshRate, VSyncMode vsyncMode, uint32_t vsyncMaxRefreshRate)
{
    std::vector<sptr<VSyncConnection>> conns;
    uint32_t generatorRefreshRate;
    int64_t vsyncCount;

    {
        bool waitForVSync = false;
        std::lock_guard<std::mutex> locker(mutex_);
        if (isFirstSend_) {
            isFirstSend_ = false;
            VLOGI("First vsync OnVSyncTrigger");
        }
        // Start of DVSync
        DVSyncRecordVSync(now, period, refreshRate, false);
        // End of DVSync
        event_.vsyncCount++;
        vsyncCount = event_.vsyncCount;
#if defined(RS_ENABLE_DVSYNC)
        if (dvsync_->IsFeatureEnabled()) {
            dvsync_->RecordVSync(now, period, refreshRate);
        }
#endif
        if (refreshRate > 0) {
            event_.vsyncPulseCount += static_cast<int64_t>(vsyncMaxRefreshRate / refreshRate);
            generatorRefreshRate_ = refreshRate;
        }
        vsyncMode_ = vsyncMode;
        ChangeConnsRateLocked(vsyncMaxRefreshRate);

        if (vsyncMode_ == VSYNC_MODE_LTPO) {
            CollectConnectionsLTPO(waitForVSync, now, conns, event_.vsyncPulseCount);
        } else {
            CollectConnections(waitForVSync, now, conns, event_.vsyncCount);
        }
#if defined(RS_ENABLE_DVSYNC_2)
        bool canDisableVsync = isRs_ || !DVSync::Instance().IsAppDVSyncOn();
#else
        bool canDisableVsync = true;
#endif
        if (!waitForVSync && canDisableVsync) {
            DisableVSync();
            return;
        }

        countTraceValue_ = (countTraceValue_ + 1) % 2;  // 2 : change num
        CountTrace(HITRACE_TAG_GRAPHIC_AGP, "VSync-" + name_, countTraceValue_);

        generatorRefreshRate = generatorRefreshRate_;
#if defined(RS_ENABLE_DVSYNC)
        if (dvsync_->IsFeatureEnabled()) {
            dvsync_->RecordPostEvent(conns, now);
        }
#endif
    }
    ConnectionsPostEvent(conns, now, period, generatorRefreshRate, vsyncCount, false);
}

void VSyncDistributor::TriggerNext(sptr<VSyncConnection> con)
{
    std::lock_guard<std::mutex> locker(mutex_);
    // Trigger VSync Again for LTPO
    con->AddRequestVsyncTimestamp(0);
    // Exclude SetVSyncRate for LTPS
    if (con->rate_ < 0) {
        con->rate_ = 0;
    }
}

void VSyncDistributor::ConnPostEvent(sptr<VSyncConnection> con, int64_t now, int64_t period, int64_t vsyncCount)
{
    int32_t ret = con->PostEvent(now, period, vsyncCount);
    VLOGD("Distributor name: %{public}s, Conn name: %{public}s, ret: %{public}d",
        name_.c_str(), con->info_.name.c_str(), ret);
    if (ret == 0 || ret == ERRNO_OTHER) {
        RemoveConnection(con);
    } else if (ret == ERRNO_EAGAIN) {
        TriggerNext(con);
    }
}

void VSyncDistributor::ConnectionsPostEvent(std::vector<sptr<VSyncConnection>> &conns, int64_t now, int64_t period,
    uint32_t generatorRefreshRate, int64_t vsyncCount, bool isDvsyncController)
{
    for (uint32_t i = 0; i < conns.size(); i++) {
        int64_t actualPeriod = period;
        int64_t timestamp = now;
        if ((generatorRefreshRate > 0) && (conns[i]->refreshRate_ > 0) &&
            (generatorRefreshRate % conns[i]->refreshRate_ == 0)
            && !isDvsyncController) {
            actualPeriod = period * static_cast<int64_t>(generatorRefreshRate / conns[i]->refreshRate_);
        }
        // Start of DVSync
        if (DVSyncCheckSkipAndUpdateTs(conns[i], timestamp)) {
            TriggerNext(conns[i]);
            continue;
        }
        // End of DVSync
        ConnPostEvent(conns[i], timestamp, actualPeriod, vsyncCount);
    }
}

void VSyncDistributor::OnVSyncEvent(int64_t now, int64_t period,
    uint32_t refreshRate, VSyncMode vsyncMode, uint32_t vsyncMaxRefreshRate)
{
#if defined(RS_ENABLE_DVSYNC)
    bool needDVSyncTrigger = true;
    if (dvsync_->IsFeatureEnabled()) {
        std::unique_lock<std::mutex> locker(mutex_);
        dvsync_->ChangeState(now);
        needDVSyncTrigger = dvsync_->NeedDVSyncTrigger();
    }
    if (dvsync_->IsFeatureEnabled() && needDVSyncTrigger) {
        OnDVSyncTrigger(now, period, refreshRate, vsyncMode, vsyncMaxRefreshRate);
    } else
#endif
    {
        OnVSyncTrigger(now, period, refreshRate, vsyncMode, vsyncMaxRefreshRate);
    }
}

#if defined(RS_ENABLE_DVSYNC)
void VSyncDistributor::SendConnectionsToVSyncWindow(int64_t now, int64_t period, uint32_t refreshRate,
                                                    VSyncMode vsyncMode, std::unique_lock<std::mutex> &locker)
{
    std::vector<sptr<VSyncConnection>> conns;
    bool waitForVSync = false;
    if (isRs_ || GetUIDVsyncPid() == 0) {
        return;
    }
    CollectConns(waitForVSync, now, conns, false);
    locker.unlock();
    PostVSyncEvent(conns, now, false);
    locker.lock();
}

int32_t VSyncDistributor::GetUIDVsyncPid()
{
    int32_t pid = 0;
    if (!isRs_) {
        pid = dvsync_->GetProxyPid();
    }
    return pid;
}
#endif

void VSyncDistributor::CheckNeedDisableDvsync(int64_t now, int64_t period)
{
#if defined(RS_ENABLE_DVSYNC)
    if (!isRs_ && IsDVsyncOn()) {
        return;
    }
    // When Dvsync on, if the RequestNextVsync is not invoked within three period and SetVSyncRate
    // is not invoked either, execute DisableVSync.
    for (uint32_t i = 0; i < connections_.size(); i++) {
        if (connections_[i]->NeedTriggeredVsync(now) || connections_[i]->rate_ >= 0) {
            return;
        }
    }
    if (now - dvsync_->GetLastRnvTS() > 3 * period) {  // 3 period
        ScopedBytrace func(name_ + "_DisableVsync");
        DisableVSync();
    }
#endif
}

/* std::pair<id, refresh rate> */
void VSyncDistributor::OnConnsRefreshRateChanged(const std::vector<std::pair<uint64_t, uint32_t>> &refreshRates)
{
    std::lock_guard<std::mutex> locker(changingConnsRefreshRatesMtx_);
    for (auto refreshRate : refreshRates) {
        changingConnsRefreshRates_[refreshRate.first] = refreshRate.second;
    }
}

void VSyncDistributor::SubScribeSystemAbility(const std::string& threadName)
{
    VLOGI("%{public}s", __func__);
    sptr<ISystemAbilityManager> systemAbilityManager =
        SystemAbilityManagerClient::GetInstance().GetSystemAbilityManager();
    if (!systemAbilityManager) {
        VLOGE("%{public}s failed to get system ability manager client", __func__);
        return;
    }
    std::string strUid = std::to_string(getuid());
    std::string strPid = std::to_string(getpid());
    std::string strTid = std::to_string(gettid());

    saStatusChangeListener_ = new VSyncSystemAbilityListener(threadName, strUid, strPid, strTid);
    int32_t ret = systemAbilityManager->SubscribeSystemAbility(RES_SCHED_SYS_ABILITY_ID, saStatusChangeListener_);
    if (ret != ERR_OK) {
        VLOGE("%{public}s subscribe system ability %{public}d failed.", __func__, RES_SCHED_SYS_ABILITY_ID);
        saStatusChangeListener_ = nullptr;
    }
}

void VSyncDistributor::CollectConnections(bool &waitForVSync, int64_t timestamp,
    std::vector<sptr<VSyncConnection>> &conns, int64_t vsyncCount, bool isDvsyncThread)
{
#if defined(RS_ENABLE_DVSYNC)
    auto uiDVsyncPid = GetUIDVsyncPid();
#endif
    for (uint32_t i = 0; i < connections_.size(); i++) {
#if defined(RS_ENABLE_DVSYNC)
    if (uiDVsyncPid != 0 && ((!isDvsyncThread && connections_[i]->proxyPid_ == uiDVsyncPid) ||
        (isDvsyncThread && connections_[i]->proxyPid_ != uiDVsyncPid))) {
            continue;
        }
#endif
        int32_t rate = connections_[i]->highPriorityState_ ? connections_[i]->highPriorityRate_ :
                                                             connections_[i]->rate_;

        if (rate < 0) {
            continue;
        }

        if (rate == 0) {  // for RequestNextVSync
            waitForVSync = true;
            if (timestamp > 0) {
                connections_[i]->rate_ = -1;
                connections_[i]->NeedTriggeredVsync(timestamp);
                conns.push_back(connections_[i]);
            }
            continue;
        }
        
        RS_TRACE_NAME_FMT("CollectConnections name:%s, proxyPid:%d, highPriorityState_:%d, highPriorityRate_:%d"
            ", rate_:%d, timestamp:%ld, vsyncCount:%ld", connections_[i]->info_.name_.c_str(),
            connections_[i]->proxyPid_, connections_[i]->highPriorityState_,
            connections_[i]->highPriorityRate_, connections_[i]->rate_, timestamp, vsyncCount);

        if (connections_[i]->rate_ == 0) {  // for SetHighPriorityVSyncRate with RequestNextVSync
            waitForVSync = true;
            if (timestamp > 0 && (vsyncCount % rate == 0)) {
                connections_[i]->rate_ = -1;
                connections_[i]->RemoveTriggeredVsync(timestamp);
                conns.push_back(connections_[i]);
            }
        } else if (connections_[i]->rate_ > 0) {  // for SetVSyncRate
            waitForVSync = true;
            if (timestamp > 0 && (vsyncCount % rate == 0)) {
                conns.push_back(connections_[i]);
            }
        }
    }
}

void VSyncDistributor::CollectConnectionsLTPO(bool &waitForVSync, int64_t timestamp,
    std::vector<sptr<VSyncConnection>> &conns, int64_t vsyncCount, bool isDvsyncThread)
{
#if defined(RS_ENABLE_DVSYNC)
    auto uiDVsyncPid = GetUIDVsyncPid();
#endif
    for (uint32_t i = 0; i < connections_.size(); i++) {
#if defined(RS_ENABLE_DVSYNC)
    if (uiDVsyncPid != 0 && ((!isDvsyncThread && connections_[i]->proxyPid_ == uiDVsyncPid) ||
        (isDvsyncThread && connections_[i]->proxyPid_ != uiDVsyncPid))) {
            continue;
        }
#endif
        SCOPED_DEBUG_TRACE_FMT("CollectConnectionsLTPO, i:%d, name:%s, rate:%d, vsyncPulseFreq:%u"
            ", referencePulseCount:%ld, vsyncCount:%d, highPriorityRate_:%d", i, connections_[i]->info_.name_.c_str(),
            connections_[i]->rate_, connections_[i]->vsyncPulseFreq_, connections_[i]->referencePulseCount_,
            vsyncCount, connections_[i]->highPriorityRate_);
        // Start of DVSync
        if (DVSyncNeedSkipUi(connections_[i])) {
            waitForVSync = true;
            continue;
        }
        // End of DVSync
        if (!connections_[i]->NeedTriggeredVsync(timestamp) && connections_[i]->rate_ <= 0) {
            continue;
        }
        waitForVSync = true;
        if (timestamp <= 0) {
            break;
        }
        int64_t vsyncPulseFreq = static_cast<int64_t>(connections_[i]->vsyncPulseFreq_);
        if (vsyncPulseFreq == 0) {
            continue;
        }
        if ((vsyncCount - connections_[i]->referencePulseCount_) % vsyncPulseFreq == 0) {
            connections_[i]->RemoveTriggeredVsync(timestamp);
            if (connections_[i]->rate_ == 0) {
                connections_[i]->rate_ = -1;
            }
            conns.push_back(connections_[i]);
        }
    }
}

void VSyncDistributor::PostVSyncEvent(const std::vector<sptr<VSyncConnection>> &conns,
                                      int64_t timestamp, bool isDvsyncThread)
{
    beforePostEvent_.store(Now());
#if defined(RS_ENABLE_DVSYNC)
    if (isDvsyncThread) {
        std::unique_lock<std::mutex> locker(mutex_);
        dvsync_->RecordPostEvent(conns, timestamp);
    }
#endif
    uint32_t generatorRefreshRate = 0;
    int64_t eventPeriod = 0;
    int64_t vsyncCount = 0;
    {
        std::unique_lock<std::mutex> locker(mutex_);
        generatorRefreshRate = generatorRefreshRate_;
        eventPeriod = event_.period;
        vsyncCount = event_.vsyncCount;
    }
    for (uint32_t i = 0; i < conns.size(); i++) {
        int64_t period = eventPeriod;
        if ((generatorRefreshRate > 0) && (conns[i]->refreshRate_ > 0) &&
            (generatorRefreshRate % conns[i]->refreshRate_ == 0)) {
            period = eventPeriod * static_cast<int64_t>(generatorRefreshRate / conns[i]->refreshRate_);
        }
        startPostEvent_.store(Now());
        int32_t ret = conns[i]->PostEvent(timestamp, period, vsyncCount);
        VLOGD("Distributor name:%{public}s, connection name:%{public}s, ret:%{public}d",
            name_.c_str(), conns[i]->info_.name_.c_str(), ret);
        if (ret == 0 || ret == ERRNO_OTHER) {
            RemoveConnection(conns[i]);
        } else if (ret == ERRNO_EAGAIN) {
            std::unique_lock<std::mutex> locker(mutex_);
            // Trigger VSync Again for LTPO
            conns[i]->AddRequestVsyncTimestamp(0);
#if defined(RS_ENABLE_DVSYNC)
            if (isDvsyncThread) {
                hasVsync_.store(true);
            }
#endif
            // Exclude SetVSyncRate for LTPS
            if (conns[i]->rate_ < 0) {
                conns[i]->rate_ = 0;
            }
        }
    }
}

VsyncError VSyncDistributor::RequestNextVSync(const sptr<VSyncConnection> &connection, const std::string &fromWhom,
                                              int64_t lastVSyncTS, const int64_t& requestVsyncTime)
{
    if (connection == nullptr) {
        VLOGE("connection is nullptr");
        return VSYNC_ERROR_NULLPTR;
    }

    RS_TRACE_NAME_FMT("%s_RequestNextVSync", connection->info_.name_.c_str());
    bool NeedPreexecute = false;
    bool isUrgent = fromWhom == URGENT_SELF_DRAWING;
    int64_t timestamp = 0;
    int64_t period = 0;
    int64_t vsyncCount = 0;
    {
        std::unique_lock<std::mutex> locker(mutex_);
        auto it = find(connections_.begin(), connections_.end(), connection);
        if (it == connections_.end()) {
            VLOGE("connection is invalid arguments");
            return VSYNC_ERROR_INVALID_ARGUMENTS;
        }
        if (connection->rate_ < 0) {
            connection->rate_ = 0;
        }
        connection->AddRequestVsyncTimestamp(requestVsyncTime);
        if (isUrgent) {
            NeedPreexecute = VSyncCheckPreexecuteAndUpdateTs(connection, timestamp, period, vsyncCount);
        }
        EnableVSync();
        // Start of DVSync
        DVSyncRecordRNV(connection, fromWhom, lastVSyncTS);
        // adaptive sync game mode, urgent scenario don't need to preexecute
        if (!isUrgent) {
            NeedPreexecute = DVSyncCheckPreexecuteAndUpdateTs(connection, timestamp, period, vsyncCount);
        }
        lastNotifyTime_ = Now();
    }
    if (NeedPreexecute) {
        ConnPostEvent(connection, timestamp, period, vsyncCount);
    }
    // End of DVSync
    if (isFirstRequest_) {
        isFirstRequest_ = false;
        isFirstSend_ = true;
        VLOGI("First vsync RequestNextVSync conn:%{public}s, rate:%{public}d, highPriorityRate:%{public}d",
            connection->info_.name_.c_str(), connection->rate_, connection->highPriorityRate_);
    }
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
    if (dvsync_->IsFeatureEnabled()) {
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
    if (dvsync_->IsFeatureEnabled()) {
        con_.notify_all();
    } else
#endif
    {
        EnableVSync();
    }
    return VSYNC_ERROR_OK;
}

VsyncError VSyncDistributor::QosGetPidByName(const std::string& name, uint32_t& pid)
{
    if (name.find("WM") == std::string::npos) {
        return VSYNC_ERROR_INVALID_ARGUMENTS;
    }
    // exclude names like NWeb_WM or ArkWebCore_WM
    if ((name.find("NWeb") != std::string::npos) || (name.find("ArkWebCore") != std::string::npos)) {
        return VSYNC_ERROR_INVALID_ARGUMENTS;
    }
    std::string::size_type pos = name.find("_");
    if (pos == std::string::npos || (pos + 1) >= name.size()) {
        return VSYNC_ERROR_INVALID_ARGUMENTS;
    }
    pid = static_cast<uint32_t>(stoi(name.substr(pos + 1)));
    return VSYNC_ERROR_OK;
}

VsyncError VSyncDistributor::SetQosVSyncRateByPid(uint32_t pid, int32_t rate, bool isSystemAnimateScene)
{
    // only set vsync rate by pid in SystemAnimateSecne
    if (!isSystemAnimateScene && rate != DEFAULT_VSYNC_RATE) {
        return VSYNC_ERROR_OK;
    }
    auto iter = connectionsMap_.find(pid);
    if (iter == connectionsMap_.end()) {
        VLOGD("%{public}s:%{public}d pid[%{public}u] can not found", __func__, __LINE__, pid);
        return VSYNC_ERROR_INVALID_ARGUMENTS;
    }
    bool isNeedNotify = false;
    for (auto connection : iter->second) {
        uint32_t tmpPid;
        if (QosGetPidByName(connection->info_.name_, tmpPid) != VSYNC_ERROR_OK || tmpPid != pid) {
            continue;
        }
        if (connection->highPriorityRate_ != rate) {
            connection->highPriorityRate_ = rate;
            connection->highPriorityState_ = true;
            RS_TRACE_NAME_FMT("VSyncDistributor::SetQosVSyncRateByPid pid:%u, rate:%d",
                pid, rate);
            VLOGD("in, conn name:%{public}s, highPriorityRate:%{public}d", connection->info_.name_.c_str(),
                connection->highPriorityRate_);
            isNeedNotify = true;
        }
    }

    if (isNeedNotify) {
#if defined(RS_ENABLE_DVSYNC)
        if (dvsync_->IsFeatureEnabled()) {
            con_.notify_all();
        } else
#endif
        {
            EnableVSync();
        }
    }

    return VSYNC_ERROR_OK;
}

VsyncError VSyncDistributor::SetQosVSyncRateByPidPublic(uint32_t pid, uint32_t rate, bool isSystemAnimateScene)
{
    std::vector<uint64_t> tmpVec = pidWindowIdMap_[pid];
    for (const auto& windowId : tmpVec) {
        VsyncError ret = SetQosVSyncRate(windowId, rate, isSystemAnimateScene);
        if (ret != VSYNC_ERROR_OK) {
            VLOGD("windowId:%{public}" PRUint " is not exit", windowId);
            return VSYNC_ERROR_INVALID_ARGUMENTS;
        }
    }
    return VSYNC_ERROR_OK;
}

std::vector<uint64_t> VSyncDistributor::GetVsyncNameLinkerIds(uint32_t pid, const std::string &name)
{
    std::lock_guard<std::mutex> locker(mutex_);
    auto iter = unalliedWindowConnectionsMap_.find(pid);
    if (iter == unalliedWindowConnectionsMap_.end()) {
        return {};
    }

    std::vector<uint64_t> connLinkerIds {};
    for (auto& connection : iter->second) {
        if (connection != nullptr) {
            if (connection->info_.name_.compare(name) == 0) {
                connLinkerIds.push_back(connection->id_);
            }
        }
    }
    return connLinkerIds;
}

constexpr pid_t VSyncDistributor::ExtractPid(uint64_t id)
{
    constexpr uint32_t bits = 32u;
    return static_cast<pid_t>(id >> bits);
}

std::vector<uint64_t> VSyncDistributor::GetSurfaceNodeLinkerIds(uint64_t windowNodeId)
{
    std::lock_guard<std::mutex> locker(mutex_);
    auto iter = connectionsMap_.find(windowNodeId);
    if (iter == connectionsMap_.end()) {
        return {};
    }

    std::vector<uint64_t> connectionLinkerIds {};
    for (auto& connection : iter->second) {
        if (connection != nullptr) {
            connectionLinkerIds.push_back(connection->id_);
        }
    }

    return connectionLinkerIds;
}

VsyncError VSyncDistributor::SetVsyncRateDiscountLTPS(uint32_t pid, const std::string &name, uint32_t rateDiscount)
{
    std::lock_guard<std::mutex> locker(mutex_);
    auto iter = unalliedWindowConnectionsMap_.find(pid);
    if (iter == unalliedWindowConnectionsMap_.end()) {
        return VSYNC_ERROR_INVALID_ARGUMENTS;
    }

    bool isNeedNotify = false;
    for (auto& connection : iter->second) {
        if (connection != nullptr && connection->highPriorityRate_ != static_cast<int32_t>(rateDiscount) &&
            connection->info_.name_.compare(name) == 0) {
            connection->highPriorityRate_ = static_cast<int32_t>(rateDiscount);
            connection->highPriorityState_ = true;
            VLOGD("SetVsyncRateDiscountLTPS conn name:%{public}s, highPriorityRate:%{public}d",
                connection->info_.name_.c_str(), connection->highPriorityRate_);
            isNeedNotify = true;
        }
    }
    if (isNeedNotify) {
#if defined(RS_ENABLE_DVSYNC)
        if (dvsync_->IsFeatureEnabled()) {
            con_.notify_all();
        } else
#endif
        {
            EnableVSync();
        }
    }
    return VSYNC_ERROR_OK;
}

VsyncError VSyncDistributor::SetQosVSyncRateByConnId(uint64_t connId, int32_t rate)
{
    std::lock_guard<std::mutex> locker(mutex_);
    auto conn = connMap_.find(connId);
    bool isNeedNotify = false;
    if (conn != connMap_.end()) {
        auto connection = conn->second;

        if (connection != nullptr && connection->highPriorityRate_ != rate) {
            connection->highPriorityRate_ = rate;
            connection->highPriorityState_ = rate != 1;
            RS_TRACE_NAME_FMT("VSyncDistributor::SetQosVSyncRateByConnId connId:%lu, rate:%d", connId, rate);
            VLOGD("in, conn name:%{public}s, highPriorityRate:%{public}d", connection->info_.name_.c_str(),
                connection->highPriorityRate_);
            isNeedNotify = true;
        }
    }
    if (isNeedNotify) {
#if defined(RS_ENABLE_DVSYNC)
        if (dvsync_->IsFeatureEnabled()) {
            con_.notify_all();
        } else
#endif
        {
            EnableVSync();
        }
    }
    return VSYNC_ERROR_OK;
}

VsyncError VSyncDistributor::SetQosVSyncRate(uint64_t windowNodeId, int32_t rate, bool isSystemAnimateScene)
{
    std::lock_guard<std::mutex> locker(mutex_);
    VsyncError resCode = SetQosVSyncRateByPid(ExtractPid(windowNodeId), rate, isSystemAnimateScene);
    auto iter = connectionsMap_.find(windowNodeId);
    if (iter == connectionsMap_.end()) {
        return resCode;
    }
    bool isNeedNotify = false;
    for (auto& connection : iter->second) {
        if (connection != nullptr && connection->highPriorityRate_ != rate) {
            connection->highPriorityRate_ = rate;
            connection->highPriorityState_ = true;
            RS_TRACE_NAME_FMT("VSyncDistributor::SetQosVSyncRateByWindowId windowNodeId:%lu, rate:%d",
                windowNodeId, rate);
            VLOGD("in, conn name:%{public}s, highPriorityRate:%{public}d", connection->info_.name_.c_str(),
                connection->highPriorityRate_);
            isNeedNotify = true;
        }
    }
    if (isNeedNotify) {
#if defined(RS_ENABLE_DVSYNC)
        if (dvsync_->IsFeatureEnabled()) {
            con_.notify_all();
        } else
#endif
        {
            EnableVSync();
        }
    }
    return VSYNC_ERROR_OK;
}

void VSyncDistributor::ChangeConnsRateLocked(uint32_t vsyncMaxRefreshRate)
{
    std::lock_guard<std::mutex> locker(changingConnsRefreshRatesMtx_);
    for (auto conn : connections_) {
        auto it = changingConnsRefreshRates_.find(conn->id_);
        if (it == changingConnsRefreshRates_.end()) {
            continue;
        }
        uint32_t refreshRate = it->second;
        if ((generatorRefreshRate_ == 0) || (refreshRate == 0) ||
            (vsyncMaxRefreshRate % refreshRate != 0) || (generatorRefreshRate_ % refreshRate != 0)) {
            conn->refreshRate_ = 0;
            conn->vsyncPulseFreq_ = 1;
            continue;
        }
        conn->refreshRate_ = refreshRate;
        conn->vsyncPulseFreq_ = vsyncMaxRefreshRate / refreshRate;
        conn->referencePulseCount_ = event_.vsyncPulseCount;
    }
    changingConnsRefreshRates_.clear();
}

// Start of DVSync
void VSyncDistributor::DisableDVSyncController()
{
    if (dvsyncController_ != nullptr && dvsyncControllerEnabled_ == true) {
        dvsyncController_->SetEnable(false, dvsyncControllerEnabled_);
    }
}

void VSyncDistributor::OnDVSyncEvent(int64_t now, int64_t period,
    uint32_t refreshRate, VSyncMode vsyncMode, uint32_t vsyncMaxRefreshRate)
{
#if defined(RS_ENABLE_DVSYNC_2)
    int64_t dvsyncPeriod = DVSync::Instance().GetOccurPeriod();
    uint32_t dvsyncRefreshRate = DVSync::Instance().GetOccurRefreshRate();
    if (dvsyncPeriod != 0 && dvsyncRefreshRate != 0) {
        period = dvsyncPeriod;
        refreshRate = dvsyncRefreshRate;
    }

    std::vector<sptr<VSyncConnection>> conns;
    int64_t vsyncCount;
    {
        bool waitForVsync = false;
        std::lock_guard<std::mutex> locker(mutex_);
        DVSync::Instance().RecordVSync(this, now, period, refreshRate, true);
        vsyncCount = DVSync::Instance().GetVsyncCount(event_.vsyncCount);
        vsyncMode_ = vsyncMode;
        ChangeConnsRateLocked(vsyncMaxRefreshRate);
        // must ltpo mode
        for (uint32_t i = 0; i < connections_.size(); i++) {
            if (connections_[i] != DVSync::Instance().GetConnectionApp()) {
                continue;
            }
            if (!connections_[i]->NeedTriggeredVsync(now) && connections_[i]->rate_ <= 0) {
                continue;
            }
            waitForVsync = true;
            if (now <= 0) {
                break;
            }
            connections_[i]->RemoveTriggeredVsync(now);
            if (connections_[i]->rate_ == 0) {
                connections_[i]->rate_ = -1;
            }
            conns.push_back(connections_[i]);
        }

        if (!waitForVsync && !IsUiDvsyncOn()) {
            DisableDVSyncController();
            return;
        }

        countTraceValue_ = (countTraceValue_ + 1) % 2; // 2 : change num
        CountTrace(HITRACE_TAG_GRAPHIC_AGP, "vsync-" + name_, countTraceValue_);
    }
    ConnectionsPostEvent(conns, now, period, refreshRate, vsyncCount, true);
#endif
}
// End of DVSync

bool VSyncDistributor::IsDVsyncOn()
{
#if defined(RS_ENABLE_DVSYNC)
    return dvsync_->IsEnabledAndOn();
#else
    return false;
#endif
}

void VSyncDistributor::SetFrameIsRender(bool isRender)
{
#if defined(RS_ENABLE_DVSYNC)
    std::unique_lock<std::mutex> locker(mutex_);
    ScopedBytrace trace("SetFrameIsRender:" + std::to_string(isRender));
    if (isRender) {
        dvsync_->UnMarkRSNotRendering();
    } else {
        dvsync_->MarkRSNotRendering();
    }
#endif
#if defined(RS_ENABLE_DVSYNC_2)
    ScopedBytrace trace("SetFrameIsRender:" + std::to_string(isRender));
    DVSync::Instance().MarkRSRendering(isRender);
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

VsyncError VSyncDistributor::SetUiDvsyncSwitch(bool dvsyncSwitch, const sptr<VSyncConnection> &connection)
{
#if defined(RS_ENABLE_DVSYNC)
    std::lock_guard<std::mutex> locker(mutex_);
    dvsync_->RuntimeMark(connection, dvsyncSwitch);
#endif
#if defined(RS_ENABLE_DVSYNC_2)
    DVSync::Instance().SetAppDVSyncSwitch(connection, dvsyncSwitch, false);
#endif
    return VSYNC_ERROR_OK;
}

VsyncError VSyncDistributor::SetUiDvsyncConfig(int32_t bufferCount)
{
#if defined(RS_ENABLE_DVSYNC)
    std::lock_guard<std::mutex> locker(mutex_);
    dvsync_->SetUiDvsyncConfig(bufferCount);
#endif
#if defined(RS_ENABLE_DVSYNC_2)
    DVSync::Instance().SetUiDVSyncConfig(bufferCount);
#endif
    return VSYNC_ERROR_OK;
}

VsyncError VSyncDistributor::SetNativeDVSyncSwitch(bool dvsyncSwitch, const sptr<VSyncConnection> &connection)
{
#if defined(RS_ENABLE_DVSYNC)
    std::lock_guard<std::mutex> locker(mutex_);
    dvsync_->RuntimeMark(connection, dvsyncSwitch, true);
#endif
#if defined(RS_ENABLE_DVSYNC_2)
    DVSync::Instance().SetAppDVSyncSwitch(connection, dvsyncSwitch, true);
#endif
    return VSYNC_ERROR_OK;
}

uint32_t VSyncDistributor::GetRefreshRate()
{
#if defined(RS_ENABLE_DVSYNC)
    std::lock_guard<std::mutex> locker(mutex_);
    return dvsync_->GetRefreshRate();
#else
    return generatorRefreshRate_;
#endif
}

void VSyncDistributor::RecordVsyncModeChange(uint32_t refreshRate, int64_t period)
{
#if defined(RS_ENABLE_DVSYNC)
    std::lock_guard<std::mutex> locker(mutex_);
    dvsync_->RecordVsyncModeChange(refreshRate, period);
#endif
}

bool  VSyncDistributor::IsUiDvsyncOn()
{
#if defined(RS_ENABLE_DVSYNC)
    return dvsync_->IsUiDvsyncOn();
#elif defined(RS_ENABLE_DVSYNC_2)
    return DVSync::Instance().IsAppDVSyncOn();
#else
    return false;
#endif
}
int64_t VSyncDistributor::GetUiCommandDelayTime()
{
#if defined(RS_ENABLE_DVSYNC)
    return dvsync_->GetUiCommandDelayTime();
#elif defined(RS_ENABLE_DVSYNC_2)
    return DVSync::Instance().GetUiCommandDelayTime();
#else
    return 0;
#endif
}

void VSyncDistributor::UpdatePendingReferenceTime(int64_t &timeStamp)
{
#if defined(RS_ENABLE_DVSYNC)
    if (IsDVsyncOn()) {
        dvsync_->UpdatePendingReferenceTime(timeStamp);
    }
#endif
#if defined(RS_ENABLE_DVSYNC_2)
    DVSync::Instance().UpdatePendingReferenceTime(timeStamp);
#endif
}

uint64_t VSyncDistributor::GetRealTimeOffsetOfDvsync(int64_t time)
{
#if defined(RS_ENABLE_DVSYNC)
    if (IsDVsyncOn()) {
        return dvsync_->GetRealTimeOffsetOfDvsync(time);
    }
#elif defined(RS_ENABLE_DVSYNC_2)
    return DVSync::Instance().GetRealTimeOffsetOfDvsync(time);
#else
    return 0;
#endif
}

void VSyncDistributor::SetHardwareTaskNum(uint32_t num)
{
#if defined(RS_ENABLE_DVSYNC)
    if (IsDVsyncOn()) {
        dvsync_->SetHardwareTaskNum(num);
    }
#endif
#if defined(RS_ENABLE_DVSYNC_2)
    DVSync::Instance().SetHardwareTaskNum(num);
#endif
}

int64_t VSyncDistributor::GetVsyncCount()
{
    std::unique_lock<std::mutex> locker(mutex_);
    return event_.vsyncCount;
}

void VSyncDistributor::SetHasNativeBuffer()
{
#if defined(RS_ENABLE_DVSYNC)
    std::unique_lock<std::mutex> locker(mutex_);
    dvsync_->SetHasNativeBuffer();
#endif
}

void VSyncDistributor::InitDVSync(DVSyncFeatureParam dvsyncParam)
{
#if defined(RS_ENABLE_DVSYNC_2)
    DVSync::Instance().InitWithParam(dvsyncParam);
    bool IsEnable = DVSync::Instance().SetDistributor(isRs_, this);
    if (IsEnable && isRs_ == false) {
        auto generator = CreateVSyncGenerator();
        dvsyncController_ = new DVSyncController(generator, 0);
    }
#endif
}

void VSyncDistributor::DVSyncAddConnection(const sptr<VSyncConnection> &connection)
{
#if defined(RS_ENABLE_DVSYNC_2)
    DVSync::Instance().SetConnection(isRs_, connection);
#endif
}

void VSyncDistributor::DVSyncDisableVSync()
{
#if defined(RS_ENABLE_DVSYNC_2)
    DVSync::Instance().DisableVSync(this);
#endif
}

void VSyncDistributor::DVSyncRecordVSync(int64_t now, int64_t period, uint32_t refreshRate, bool isDvsyncController)
{
#if defined(RS_ENABLE_DVSYNC_2)
    DVSync::Instance().RecordVSync(this, now, period, refreshRate, false);
#endif
}

bool VSyncDistributor::DVSyncCheckSkipAndUpdateTs(const sptr<VSyncConnection> &connection, int64_t &timeStamp)
{
#if defined(RS_ENABLE_DVSYNC_2)
    return DVSync::Instance().NeedSkipAndUpdateTs(connection, timeStamp);
#else
    return false;
#endif
}

bool VSyncDistributor::DVSyncNeedSkipUi(const sptr<VSyncConnection> &connection)
{
#if defined(RS_ENABLE_DVSYNC_2)
    return DVSync::Instance().NeedSkipUi(connection);
#else
    return false;
#endif
}

void VSyncDistributor::RecordEnableVsync()
{
#if defined(RS_ENABLE_DVSYNC_2)
    DVSync::Instance().RecordEnableVsync(this);
#endif
}

void VSyncDistributor::DVSyncRecordRNV(const sptr<VSyncConnection> &connection, const std::string &fromWhom,
    int64_t lastVSyncTS)
{
#if defined(RS_ENABLE_DVSYNC_2)
    DVSync::Instance().RecordRNV(connection, fromWhom, vsyncMode_, lastVSyncTS);
#endif
}

bool VSyncDistributor::VSyncCheckPreexecuteAndUpdateTs(const sptr<VSyncConnection> &connection, int64_t &timestamp,
    int64_t &period, int64_t &vsyncCount)
{
    if (controller_ == nullptr || vsyncEnabled_) {
        return false;
    }
    bool NeedPreexecute = controller_->NeedPreexecuteAndUpdateTs(timestamp, period);
    bool needTriggeredVsync = connection->NeedTriggeredVsync(timestamp);
    if (NeedPreexecute && needTriggeredVsync) {
        RS_TRACE_NAME_FMT("VSyncDistributor::VSyncCheckPreexecuteAndUpdateTs timestamp:%ld, period:%ld",
            timestamp, period);
        event_.vsyncCount++;
        vsyncCount = event_.vsyncCount;
        if (connection->rate_ == 0) {
            connection->rate_ = -1;
        }
        connection->RemoveTriggeredVsync(timestamp);
    }
    return NeedPreexecute;
}

bool VSyncDistributor::DVSyncCheckPreexecuteAndUpdateTs(const sptr<VSyncConnection> &connection, int64_t &timestamp,
    int64_t &period, int64_t &vsyncCount)
{
#if defined(RS_ENABLE_DVSYNC_2)
    bool NeedPreexecute = DVSync::Instance().NeedPreexecuteAndUpdateTs(connection, timestamp, period);
    bool needTriggeredVsync = connection->NeedTriggeredVsync(timestamp);
    if (NeedPreexecute && needTriggeredVsync) {
        RS_TRACE_NAME_FMT("DVSync::DVSyncCheckPreexecuteAndUpdateTs timestamp:%ld, period:%ld", timestamp, period);
        event_.vsyncCount++;
        vsyncCount = event_.vsyncCount;
        if (connection->rate_ == 0) {
            connection->rate_ = -1;
        }
        connection->RemoveTriggeredVsync(timestamp);
    }
    return NeedPreexecute;
#else
    return false;
#endif
}

void VSyncDistributor::NotifyPackageEvent(const std::vector<std::string>& packageList)
{
#if defined(RS_ENABLE_DVSYNC_2)
    DVSync::Instance().NotifyPackageEvent(packageList);
#endif
}

void VSyncDistributor::NotifyTouchEvent(int32_t touchStatus, int32_t touchCnt)
{
#if defined(RS_ENABLE_DVSYNC)
    if (IsDVsyncOn()) {
        dvsync_->NotifyTouchEvent(touchStatus, touchCnt);
    }
#endif
#if defined(RS_ENABLE_DVSYNC_2)
    DVSync::Instance().NotifyTouchEvent(touchStatus, touchCnt);
#endif
}

bool VSyncDistributor::AdaptiveDVSyncEnable(const std::string &nodeName, int64_t timeStamp, int32_t bufferCount,
    bool &needConsume)
{
#if defined(RS_ENABLE_DVSYNC_2)
    return DVSync::Instance().AdaptiveDVSyncEnable(nodeName, timeStamp, bufferCount, needConsume);
#else
    return false;
#endif
}

void VSyncDistributor::SetBufferInfo(uint64_t id, const std::string &name, uint32_t queueSize,
    int32_t bufferCount, int64_t lastConsumeTime)
{
#if defined(RS_ENABLE_DVSYNC_2)
    DVSync::Instance().SetBufferInfo(id, name, queueSize, bufferCount, lastConsumeTime);
#endif
}

void VSyncDistributor::PrintConnectionsStatus()
{
    std::unique_lock<std::mutex> locker(mutex_);
    for (uint32_t i = 0; i < connections_.size(); i++) {
        VLOGW("[Info]PrintConnectionsStatus, i:%{public}d, name:%{public}s, proxyPid:%{public}d"
            ", highPriorityRate:%{public}d, rate:%{public}d, vsyncPulseFreq:%{public}u",
            i, connections_[i]->info_.name_.c_str(), connections_[i]->proxyPid_, connections_[i]->highPriorityRate_,
            connections_[i]->rate_, connections_[i]->vsyncPulseFreq_);
    }
    VLOGW("[Info]PrintVSyncInfo, beforeWaitRnvTime %{public}" PRId64 " afterWaitRnvTime %{public}" PRId64
        " lastNotifyTime %{public}" PRId64 " beforePostEvent %{public}" PRId64 " startPostEvent %{public}" PRId64,
        beforeWaitRnvTime_, afterWaitRnvTime_, lastNotifyTime_, beforePostEvent_.load(), startPostEvent_.load());
#if defined(RS_ENABLE_DVSYNC)
    VLOGW("[Info]DVSync featureEnable %{public}d on %{public}d needDVSyncRnv %{public}d, needDVSyncTrigger %{public}d",
        dvsync_->IsFeatureEnabled(), IsDVsyncOn(), dvsync_->NeedDVSyncRNV(), dvsync_->NeedDVSyncTrigger());
#endif
}

void VSyncDistributor::FirstRequestVsync()
{
    std::unique_lock<std::mutex> locker(mutex_);
    isFirstRequest_ = true;
}
}
}
