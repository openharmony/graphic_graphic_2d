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

#include <dirent.h>
#include <format>
#include <fstream>

#include "battery_srv_client.h"
#include "cpu_collector.h"
#include "gpu_collector.h"
#include "memory_collector.h"
#include "rs_profiler_telemetry.h"
#include "rs_profiler_utils.h"

namespace OHOS::Rosen {

static inline std::string IncludePathDelimiter(const std::string& path)
{
    if (path.rfind('/') != path.size() - 1) {
        return path + "/";
    }

    return path;
}

static void IterateDirFiles(const std::string& path, std::vector<std::string>& files)
{
    std::string pathStringWithDelimiter;
    DIR* dir = opendir(path.c_str());
    if (!dir) {
        return;
    }

    while (true) {
        struct dirent* ptr = readdir(dir);
        if (!ptr) {
            break;
        }

        // current dir OR parent dir
        if ((strcmp(ptr->d_name, ".") == 0) || (strcmp(ptr->d_name, "..") == 0)) {
            continue;
        }
        if (ptr->d_type == DT_DIR) {
            pathStringWithDelimiter = IncludePathDelimiter(path) + std::string(ptr->d_name);
            IterateDirFiles(pathStringWithDelimiter, files);
        } else {
            files.push_back(IncludePathDelimiter(path) + std::string(ptr->d_name));
        }
    }
    closedir(dir);
}

static void GetCPUTemperature(CPUInfo& cpu)
{
    // temporary disabled
    cpu.temperature = 0;
}

static void GetCPUCurrent(CPUInfo& cpu)
{
    cpu.current = OHOS::PowerMgr::BatterySrvClient::GetInstance().GetNowCurrent();
}

static void GetCPUVoltage(CPUInfo& cpu)
{
    cpu.voltage = OHOS::PowerMgr::BatterySrvClient::GetInstance().GetVoltage();
}

static void GetCPUMemory(CPUInfo& cpu)
{
    auto collector = HiviewDFX::UCollectUtil::MemoryCollector::Create();
    auto freqResult = collector->CollectSysMemory();
    cpu.ramTotal = freqResult.data.memTotal;
    cpu.ramFree = freqResult.data.memFree;
}

static void GetCPUCores(CPUInfo& cpu)
{
    auto collector = HiviewDFX::UCollectUtil::CpuCollector::Create();
    auto freqResult = collector->CollectCpuFrequency();
    auto& cpuFreq = freqResult.data;
    auto usageResult = collector->CollectSysCpuUsage(true);
    auto& cpuUsage = usageResult.data;

    cpu.cores = std::min(CPUInfo::MAX_CORE_COUNT, static_cast<uint32_t>(cpuFreq.size()));

    for (uint32_t i = 0; i < cpu.cores; i++) { // Frequency
        const uint32_t frequencyCpuId = std::min(static_cast<uint32_t>(cpuFreq[i].cpuId), cpu.cores - 1u);
        cpu.coreFrequency[frequencyCpuId] = cpuFreq[i].curFreq * Utils::MICRO;
    }

    for (auto& cpuInfo : cpuUsage.cpuInfos) { // Load
        const uint32_t loadCpuId = std::min(static_cast<uint32_t>(std::atoi(cpuInfo.cpuId.data())), cpu.cores - 1u);
        cpu.coreLoad[loadCpuId] = cpuInfo.userUsage + cpuInfo.niceUsage + cpuInfo.systemUsage + cpuInfo.idleUsage +
                                  cpuInfo.ioWaitUsage + cpuInfo.irqUsage + cpuInfo.softIrqUsage;
    }
}

static void GetGPUFreqAndLoad(GPUInfo& gpu)
{
    auto collector = HiviewDFX::UCollectUtil::GpuCollector::Create();
    gpu.frequency = collector->CollectGpuFrequency().data.curFeq * Utils::NANO;
    gpu.load = collector->CollectSysGpuLoad().data.gpuLoad;
}

const DeviceInfo& RSTelemetry::GetDeviceInfo()
{
    static DeviceInfo info;

    // cpu
    GetCPUTemperature(info.cpu);
    GetCPUCurrent(info.cpu);
    GetCPUVoltage(info.cpu);
    GetCPUMemory(info.cpu);
    GetCPUCores(info.cpu);

    // gpu
    GetGPUFreqAndLoad(info.gpu);

    return info;
}

static std::string TemperatureToString(float temperature)
{
    return std::to_string(temperature) + " °C";
}

static std::string CurrentToString(float current)
{
    return std::to_string(current) + " mA";
}

static std::string VoltageToString(float voltage)
{
    return std::to_string(voltage) + " V";
}

static std::string MemoryToString(uint64_t memory)
{
    return std::to_string(memory * Utils::MICRO) + " GB";
}

static std::string FrequencyLoadToString(float frequency, float load)
{
    return std::to_string(frequency) + " GHz (" + std::to_string(load) + " %)";
}

std::string RSTelemetry::GetDeviceInfoString()
{
    const DeviceInfo info = GetDeviceInfo();

    std::string string;
    for (size_t i = 0; i < info.cpu.cores; i++) {
        string += +"\nCPU" + std::to_string(i) + ": " +
                  FrequencyLoadToString(info.cpu.coreFrequency[i], info.cpu.coreLoad[i]);
    }

    string += "\nTemperature: " + TemperatureToString(info.cpu.temperature) +
              "\nCurrent: " + CurrentToString(info.cpu.current) + "\nVoltage: " + VoltageToString(info.cpu.voltage) +
              "\nRAM Total: " + MemoryToString(info.cpu.ramTotal) + "\nRAM Free: " + MemoryToString(info.cpu.ramFree) +
              "\nGPU: " + FrequencyLoadToString(info.gpu.frequency, info.gpu.load);

    return string;
}
} // namespace OHOS::Rosen