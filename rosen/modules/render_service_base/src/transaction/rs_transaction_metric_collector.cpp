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
#include <cstdint>
#include <ctime>
#include <iomanip>
#include <iostream>
#include <map>
#include <string>

#include "transaction/rs_transaction_metric_collector.h"
#include "hilog/log.h"
#include "platform/common/rs_log.h"
#include "platform/common/rs_system_properties.h"

namespace OHOS {
namespace Rosen {
namespace {
    const uint64_t PRECISION = 2;

    const uint64_t COLLECT_INTERVAL = 5000;

    const uint64_t MS_UNIT = 1000;

    uint64_t NowInMs()
    {
        auto now = std::chrono::system_clock::now();
        auto nowCastMs = std::chrono::time_point_cast<std::chrono::milliseconds>(now);
        uint64_t msNow = (uint64_t) (nowCastMs.time_since_epoch().count());
        return msNow;
    }

    std::string MsToString(uint64_t ms)
    {
        std::chrono::duration<uint64_t, std::milli> dur(ms);
        auto tp = std::chrono::system_clock::time_point(
            std::chrono::duration_cast<std::chrono::system_clock::duration>(dur));
        std::time_t t = std::chrono::system_clock::to_time_t(tp);
        std::stringstream ss;
        ss << std::put_time(std::localtime(&t), "%Y-%m-%d %H.%M.%S") << "." << ms % MS_UNIT;
        return ss.str();
    }
}

RSTransactionMetricPid::RSTransactionMetricPid() : prevTxTimestamp_(0)
{
}

void RSTransactionMetricPid::Initiate(uint64_t rsTxSize, uint64_t currentTimestamp)
{
    prevTxTimestamp_ = currentTimestamp;
    rsTxCount_ = 1;
    rsTxIntervalSum_ = 0;
    minRsTxInterval_ = std::numeric_limits<uint64_t>::max();
    maxRsTxInterval_ = std::numeric_limits<uint64_t>::min();
    rsTxSizeSum_ = rsTxSize;
    minRsTxSize_ = rsTxSize;
    maxRsTxSize_ = rsTxSize;
}

void RSTransactionMetricPid::Update(uint64_t rsTxSize, uint64_t currentTimestamp)
{
    if (prevTxTimestamp_ == 0) {
        Initiate(rsTxSize, currentTimestamp);
        return;
    }
    uint64_t currentInterval = currentTimestamp - prevTxTimestamp_;
    rsTxCount_++;
    rsTxIntervalSum_ += currentInterval;
    minRsTxInterval_ = minRsTxInterval_ < currentInterval ? minRsTxInterval_ : currentInterval;
    maxRsTxInterval_ = maxRsTxInterval_ < currentInterval ? currentInterval : maxRsTxInterval_;

    rsTxSizeSum_ += rsTxSize;
    minRsTxSize_ = minRsTxSize_ < rsTxSize ? minRsTxSize_ : rsTxSize;
    maxRsTxSize_ = maxRsTxSize_ < rsTxSize ? rsTxSize : maxRsTxSize_;
    prevTxTimestamp_ = currentTimestamp;
}

std::string RSTransactionMetricPid::ToString() const
{
    double avgCmd = 0;
    if (rsTxCount_ > 0) {
        avgCmd = static_cast<double>(rsTxSizeSum_) / static_cast<double>(rsTxCount_);
    }
    uint64_t minRsTxInterval = 0;
    uint64_t maxRsTxInterval = 0;
    if (rsTxCount_ > 1) {
        minRsTxInterval = minRsTxInterval_;
        maxRsTxInterval = maxRsTxInterval_;
    }

    std::stringstream stream;
    stream << "count:" << rsTxCount_ << ", ";
    stream << "minInterval:" << minRsTxInterval << ", ";
    stream << "maxInterval:" << maxRsTxInterval << ", ";
    stream << "minCmd:" << minRsTxSize_ << ", ";
    stream << "maxCmd:" << maxRsTxSize_ << ", ";
    stream << "totalCmd:" << rsTxSizeSum_ << ", ";
    stream << "avgCmd:" << std::fixed << std::setprecision(PRECISION) << avgCmd;
    return stream.str();
}

RSTransactionMetricCollector &RSTransactionMetricCollector::GetInstance()
{
    static RSTransactionMetricCollector instance;
    return instance;
}

RSTransactionMetricCollector::RSTransactionMetricCollector() : collectStartTimestamp_(0)
{
}

void RSTransactionMetricCollector::Reset(uint64_t ms)
{
    collectStartTimestamp_ = ms;
    collectEndTimestamp_ = ms;
    pidMetrics_.clear();
}

void RSTransactionMetricCollector::Collect(std::unique_ptr<RSTransactionData> &rsTransactionData)
{
    if (!RSSystemProperties::IsBetaRelease()) {
        return;
    }
    uint64_t currentTimestamp = NowInMs();
    if (collectStartTimestamp_ == 0) {
        Reset(currentTimestamp);
    }

    uint64_t sinceStart = currentTimestamp - collectStartTimestamp_;
    pid_t sendingPid = rsTransactionData->GetSendingPid();
    uint64_t rsTxSize = rsTransactionData->GetCommandCount();
    pid_t fakePidAsAll = -1;
    for (const pid_t &pid: {fakePidAsAll, sendingPid}) {
        pidMetrics_[pid].Update(rsTxSize, currentTimestamp);
    }

    if (sinceStart > COLLECT_INTERVAL) {
        collectEndTimestamp_ = currentTimestamp;
        Log();
        Reset(currentTimestamp);
        return;
    }
}

void RSTransactionMetricCollector::Log()
{
    std::string collectStartTimestamp = MsToString(collectStartTimestamp_);
    std::string collectEndTimestamp = MsToString(collectEndTimestamp_);

    std::map<pid_t, RSTransactionMetricPid> ordered(pidMetrics_.begin(), pidMetrics_.end());
    for (auto it = ordered.begin(); it != ordered.end(); ++it) {
        RS_LOGI("[%{public}s - %{public}s], %{public}d %{public}s",
            collectStartTimestamp.c_str(), collectEndTimestamp.c_str(),
                (std::int32_t)(it->first), it->second.ToString().c_str());
    }
}

} // namespace Rosen
} // namespace OHOS