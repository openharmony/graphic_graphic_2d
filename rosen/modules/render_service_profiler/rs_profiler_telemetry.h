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

#ifndef RS_PROFILER_TELEMETRY_H
#define RS_PROFILER_TELEMETRY_H

#include <string>

namespace OHOS::Rosen {

struct FrequencyLoadInfo final {
    float current = 0.0f;
    float min = 0.0f;
    float max = 0.0f;
    float load = 0.0f;
};

struct GPUInfo final {
    FrequencyLoadInfo frequencyLoad;
};

struct CPUInfo final {
    static constexpr uint32_t MAX_CORES = 16u;
    FrequencyLoadInfo coreFrequencyLoad[MAX_CORES];
    uint32_t cores = 0u;
    float temperature = 0.0f;
    float current = 0.0f;
    float voltage = 0.0f;
    uint64_t ramTotal = 0u;
    uint64_t ramFree = 0u;
};

struct DeviceInfo final {
    CPUInfo cpu;
    GPUInfo gpu;
};

class RSTelemetry final {
public:
    static const DeviceInfo& GetDeviceInfo();
    static std::string GetDeviceInfoString();
    static std::string GetDeviceFrequencyString();
    static std::string GetCpuAffinityString();
};

} // namespace OHOS::Rosen

#endif // RS_PROFILER_TELEMETRY_H