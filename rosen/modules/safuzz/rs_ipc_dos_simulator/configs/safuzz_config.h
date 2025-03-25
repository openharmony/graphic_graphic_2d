/*
 * Copyright (c) 2025 Huawei Device Co., Ltd.
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

#ifndef SAFUZZ_CONFIG_H
#define SAFUZZ_CONFIG_H

#include <cstddef>
#include <cstdint>
#include <string>

namespace OHOS {
namespace Rosen {
/* user configs */
const std::string USER_TYPE_DEFAULT = "shell";

/* test case configs */
const std::string TEST_CASE_CONFIG_PATH_DEFAULT = "/data/local/tmp/test_case_config.json";
constexpr int TEST_CASE_ITERATIONS_DEFAULT = 100;

/* process and thread configs */
const char* const SAFUZZ_PROCESS_NAME = "RSIpcDosSimulator";
constexpr size_t SAFUZZ_THREAD_COUNT = 16;
constexpr int SEND_REQUEST_TIMES_DEFAULT = 1;
constexpr int MAX_RETRY_COUNT = 20; // maximum counts of retrying SendRequest
constexpr int RETRY_WAIT_TIME_US = 1000; // wait 1ms before retrying SendRequest

/* render service common configs */
constexpr int RS_SAID = 10;

/* render service ipc configs */
constexpr size_t ASHMEM_SIZE_THRESHOLD = 100 * 1024; // cannot > 500K in TF_ASYNC mode
constexpr size_t PARCEL_MAX_CPACITY_VARIANT = UINT32_MAX; // upper bound of parcel capacity
constexpr size_t PARCEL_SPLIT_THRESHOLD_VARIANT = UINT32_MAX; // should be < PARCEL_MAX_CPACITY
} // namespace Rosen
} // namespace OHOS

#endif // SAFUZZ_CONFIG_H
