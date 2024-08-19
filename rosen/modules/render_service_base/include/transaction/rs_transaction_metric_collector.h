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

#ifndef ROSEN_RENDER_SERVICE_BASE_RS_TRANSACTION_METRIC_COLLECTOR_H
#define ROSEN_RENDER_SERVICE_BASE_RS_TRANSACTION_METRIC_COLLECTOR_H

#include <cstdint>
#include <ctime>
#include <iomanip>
#include <string>
#include <unordered_map>

#include "rs_transaction_data.h"
#include "common/rs_macros.h"

namespace OHOS {
namespace Rosen {

class RSB_EXPORT RSTransactionMetricPid {
public:
    RSTransactionMetricPid();

    void Update(uint64_t rsTxSize, uint64_t currentTimestamp);

    std::string ToString() const;

private:
    uint64_t prevTxTimestamp_;
    uint64_t rsTxCount_;
    uint64_t rsTxIntervalSum_;
    uint64_t minRsTxInterval_;
    uint64_t maxRsTxInterval_;
    uint64_t rsTxSizeSum_;
    uint64_t minRsTxSize_;
    uint64_t maxRsTxSize_;

    void Initiate(uint64_t rsTxSize, uint64_t currentTimestamp);
};

class RSB_EXPORT RSTransactionMetricCollector {
public:
    static RSTransactionMetricCollector &GetInstance();

    RSTransactionMetricCollector(const RSTransactionMetricCollector &other) = delete;

    void Collect(std::unique_ptr<RSTransactionData> &rsTransactionData);

    void Reset(uint64_t ms);

    void Log();

private:
    uint64_t collectStartTimestamp_;
    uint64_t collectEndTimestamp_;
    std::unordered_map<pid_t, RSTransactionMetricPid> pidMetrics_;

    RSTransactionMetricCollector();

    const RSTransactionMetricCollector &operator=(const RSTransactionMetricCollector &other) = delete;
};

} // namespace Rosen
} // namespace OHOS

#endif // ROSEN_RENDER_SERVICE_BASE_RS_TRANSACTION_METRIC_COLLECTOR_H