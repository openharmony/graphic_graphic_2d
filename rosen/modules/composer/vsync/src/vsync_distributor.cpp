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
#include <sched.h>
#include <sys/resource.h>
#include <scoped_bytrace.h>
#include "vsync_log.h"

namespace OHOS {
namespace Rosen {
namespace {
constexpr HiviewDFX::HiLogLabel LABEL = { LOG_CORE, 0, "VsyncDistributor" };
constexpr int32_t SOFT_VSYNC_PERIOD = 16;
constexpr int32_t ERRNO_EAGAIN = -1;
constexpr int32_t ERRNO_OTHER = -2;
constexpr int32_t THREAD_PRIORTY = -6;
constexpr int32_t SCHED_PRIORITY = 2;
}
VSyncConnection::VSyncConnection(const sptr<VSyncDistributor>& distributor, std::string name)
    : rate_(-1), distributor_(distributor), name_(name)
{
    socketPair_ = new LocalSocketPair();
    socketPair_->CreateChannel(sizeof(int64_t), sizeof(int64_t));
}

VSyncConnection::~VSyncConnection()
{
}

VsyncError VSyncConnection::RequestNextVSync()
{
    if (distributor_ == nullptr) {
        return VSYNC_ERROR_NULLPTR;
    }
    const sptr<VSyncDistributor> distributor = distributor_.promote();
    if (distributor == nullptr) {
        return VSYNC_ERROR_NULLPTR;
    }
    ScopedBytrace func(name_ + "RequestNextVSync");
    return distributor->RequestNextVSync(this);
}

VsyncError VSyncConnection::GetReceiveFd(int32_t &fd)
{
    fd = socketPair_->GetReceiveDataFd();
    return VSYNC_ERROR_OK;
}

int32_t VSyncConnection::PostEvent(int64_t now)
{
    return socketPair_->SendData(&now, sizeof(int64_t));
}

VsyncError VSyncConnection::SetVSyncRate(int32_t rate)
{
    if (distributor_ == nullptr) {
        return VSYNC_ERROR_NULLPTR;
    }
    const sptr<VSyncDistributor> distributor = distributor_.promote();
    if (distributor == nullptr) {
        return VSYNC_ERROR_NULLPTR;
    }
    return distributor->SetVSyncRate(rate, this);
}

VSyncDistributor::VSyncDistributor(sptr<VSyncController> controller, std::string name)
    : controller_(controller), mutex_(), con_(), connections_(),
    vsyncEnabled_(false), name_(name)
{
    event_.timestamp = 0;
    event_.vsyncCount = 0;
    vsyncThreadRunning_ = true;
    threadLoop_ = std::thread(std::bind(&VSyncDistributor::ThreadMain, this));
}

VSyncDistributor::~VSyncDistributor()
{
    vsyncThreadRunning_ = false;
    if (threadLoop_.joinable()) {
        con_.notify_all();
        threadLoop_.join();
    }
}

VsyncError VSyncDistributor::AddConnection(const sptr<VSyncConnection>& connection)
{
    if (connection == nullptr) {
        return VSYNC_ERROR_NULLPTR;
    }
    std::lock_guard<std::mutex> locker(mutex_);
    auto it = find(connections_.begin(), connections_.end(), connection);
    if (it != connections_.end()) {
        return VSYNC_ERROR_INVALID_ARGUMENTS;
    }
    connections_.push_back(connection);
    return VSYNC_ERROR_OK;
}

VsyncError VSyncDistributor::RemoveConnection(const sptr<VSyncConnection>& connection)
{
    if (connection == nullptr) {
        return VSYNC_ERROR_NULLPTR;
    }
    std::lock_guard<std::mutex> locker(mutex_);
    auto it = find(connections_.begin(), connections_.end(), connection);
    if (it == connections_.end()) {
        return VSYNC_ERROR_INVALID_ARGUMENTS;
    }
    connections_.erase(it);
    return VSYNC_ERROR_OK;
}

void VSyncDistributor::ThreadMain()
{
    // set thread priorty
    setpriority(PRIO_PROCESS, 0, THREAD_PRIORTY);
    struct sched_param param = {0};
    param.sched_priority = SCHED_PRIORITY;
    sched_setscheduler(0, SCHED_FIFO, &param);

    int64_t timestamp;
    int64_t vsyncCount;
    while (vsyncThreadRunning_ == true) {
        std::vector<sptr<VSyncConnection> > conns;
        bool waitForVSync = false;
        {
            std::unique_lock<std::mutex> locker(mutex_);
            timestamp = event_.timestamp;
            event_.timestamp = 0;
            vsyncCount = event_.vsyncCount;
            for (uint32_t i = 0; i <connections_.size(); i++) {
                if (connections_[i]->rate_ == 0) {
                    VLOGI("conn name:%{public}s, rate:%{public}d",
                        connections_[i]->GetName().c_str(), connections_[i]->rate_);
                    waitForVSync = true;
                    if (timestamp > 0) {
                        connections_[i]->rate_ = -1;
                        conns.push_back(connections_[i]);
                    }
                } else if ((connections_[i]->rate_) > 0 && (vsyncCount % connections_[i]->rate_ == 0)) {
                    conns.push_back(connections_[i]);
                    waitForVSync = true;
                }
            }
            VLOGI("Distributor name:%{public}s, WaitforVSync:%{public}d, timestamp:" VPUBI64 "",
                name_.c_str(), waitForVSync, timestamp);
            // no vsync signal
            if (timestamp == 0) {
                // there is some connections request next vsync, enable vsync if vsync disable and
                // and start the software vsync with wait_for function
                if (waitForVSync == true && vsyncEnabled_ == false) {
                    EnableVSync();
                    if (con_.wait_for(locker, std::chrono::milliseconds(SOFT_VSYNC_PERIOD)) ==
                        std::cv_status::timeout) {
                        const auto &now = std::chrono::steady_clock::now().time_since_epoch();
                        timestamp = std::chrono::duration_cast<std::chrono::nanoseconds>(now).count();
                        event_.timestamp = timestamp;
                        event_.vsyncCount++;
                    }
                } else {
                    // just wait request or vsync signal
                    if (vsyncThreadRunning_ == true) {
                        con_.wait(locker);
                    }
                }
                continue;
            } else if ((timestamp > 0) && (waitForVSync == false)) {
                // if there is a vsync signal but no vaild connections, we should disable vsync,
                // but the system was unstable, so we didn't choose to disable it
                // DisableVSync()
                continue;
            }
        }
        ScopedBytrace func(name_ + "_SendVsync");
        for (uint32_t i = 0; i < conns.size(); i++) {
            int32_t ret = conns[i]->PostEvent(timestamp);
            VLOGI("Distributor name:%{public}s, connection name:%{public}s, ret:%{public}d",
                name_.c_str(), conns[i]->GetName().c_str(), ret);
            if (ret == 0 || ret == ERRNO_OTHER) {
                RemoveConnection(conns[i]);
            } else if (ret == ERRNO_EAGAIN) {
                std::unique_lock<std::mutex> locker(mutex_);
                conns[i]->rate_ = 0;
            }
        }
    }
}

void VSyncDistributor::EnableVSync()
{
    if (controller_ != nullptr && vsyncEnabled_ == false) {
        vsyncEnabled_ = true;
        controller_->SetCallback(this);
        controller_->SetEnable(true);
    }
}

void VSyncDistributor::DisableVSync()
{
    if (controller_ != nullptr && vsyncEnabled_ == true) {
        vsyncEnabled_ = false;
        controller_->SetEnable(false);
    }
}

void VSyncDistributor::OnVSyncEvent(int64_t now)
{
    std::lock_guard<std::mutex> locker(mutex_);
    event_.timestamp = now;
    event_.vsyncCount++;
    con_.notify_all();
}

VsyncError VSyncDistributor::RequestNextVSync(const sptr<VSyncConnection>& connection)
{
    if (connection == nullptr) {
        VLOGE("connection is nullptr");
        return VSYNC_ERROR_NULLPTR;
    }
    ScopedBytrace func(connection->GetName() + "_RequestNextVSync");
    std::lock_guard<std::mutex> locker(mutex_);
    auto it = find(connections_.begin(), connections_.end(), connection);
    if (it == connections_.end()) {
        VLOGE("connection is invalid arguments");
        return VSYNC_ERROR_INVALID_ARGUMENTS;
    }
    if (connection->rate_ < 0) {
        connection->rate_ = 0;
        con_.notify_all();
    }
    VLOGI("conn name:%{public}s, rate:%{public}d", connection->GetName().c_str(), connection->rate_);
    return VSYNC_ERROR_OK;
}

VsyncError VSyncDistributor::SetVSyncRate(int32_t rate, const sptr<VSyncConnection>& connection)
{
    if (rate <= 0 || connection == nullptr) {
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
    con_.notify_all();
    return VSYNC_ERROR_OK;
}
}
}
