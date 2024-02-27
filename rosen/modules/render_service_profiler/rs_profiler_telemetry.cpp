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

#include "platform/common/rs_log.h"

namespace OHOS::Rosen {

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
    if (!collector) {
        return;
    }

    auto freqResult = collector->CollectSysMemory();
    if (freqResult.retCode == HiviewDFX::UCollect::SUCCESS) {
        cpu.ramTotal = freqResult.data.memTotal;
        cpu.ramFree = freqResult.data.memFree;
    } else {
        cpu.ramTotal = 0;
        cpu.ramFree = 0;
    }
}

static void GetCPUCores(CPUInfo& cpu)
{
    auto collector = HiviewDFX::UCollectUtil::CpuCollector::Create();
    if (!collector) {
        return;
    }

    auto freqResult = collector->CollectCpuFrequency();
    if (freqResult.retCode == HiviewDFX::UCollect::SUCCESS) {
        auto& cpuFreq = freqResult.data;
        cpu.cores = std::min(CPUInfo::MAX_CORE_COUNT, static_cast<uint32_t>(cpuFreq.size()));
        for (const auto& freqInfo : cpuFreq) {
            if (freqInfo.cpuId < cpu.cores) {
                cpu.coreFrequency[freqInfo.cpuId] = freqInfo.curFreq * Utils::MICRO;
            }
        }
    }

    auto usageResult = collector->CollectSysCpuUsage(true);
    if (usageResult.retCode != HiviewDFX::UCollect::SUCCESS) {
        return;
    }

    auto& cpuUsage = usageResult.data.cpuInfos;
    for (const auto& cpuInfo : cpuUsage) {
        constexpr int cpuWordLen = 3;
        const auto& cpuId = cpuInfo.cpuId;
        // cpu IDs are: cpu, cpu0, cpu1, ..., cpu7  but nobody knows when would it change
        if (cpuId.rfind("cpu", 0) == 0 && cpuId.size() > cpuWordLen) {
            const uint32_t loadCpuId = std::atoi(cpuId.data() + cpuWordLen);
            if (loadCpuId < cpu.cores) {
                constexpr float maxPercents = 100.0f;
                cpu.coreLoad[loadCpuId] =
                    maxPercents * (cpuInfo.userUsage + cpuInfo.niceUsage + cpuInfo.systemUsage + cpuInfo.idleUsage +
                                      cpuInfo.ioWaitUsage + cpuInfo.irqUsage + cpuInfo.softIrqUsage);
            }
        }
    }
}

static void GetGPUFreqAndLoad(GPUInfo& gpu)
{
    auto collector = HiviewDFX::UCollectUtil::GpuCollector::Create();
    if (!collector) {
        return;
    }

    auto gpuFreq = collector->CollectGpuFrequency();
    if (gpuFreq.retCode == HiviewDFX::UCollect::SUCCESS) {
        gpu.frequency = gpuFreq.data.curFeq * Utils::NANO;
    } else {
        gpu.frequency = 0;
    }

    auto gpuLoad = collector->CollectSysGpuLoad();
    if (gpuLoad.retCode == HiviewDFX::UCollect::SUCCESS) {
        gpu.load = gpuLoad.data.gpuLoad;
    } else {
        gpu.load = 0;
    }
}

const DeviceInfo& RSTelemetry::GetDeviceInfo()
{
    static DeviceInfo info;
    GetCPUTemperature(info.cpu);
    GetCPUCurrent(info.cpu);
    GetCPUVoltage(info.cpu);
    GetCPUMemory(info.cpu);
    GetCPUCores(info.cpu);
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