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

#include "rs_profiler_telemetry.h"

#include <format>
#include <fstream>

#include "developtools/profiler/host/smartperf/client/client_command/include/CPU.h"
#include "developtools/profiler/host/smartperf/client/client_command/include/sp_utils.h"
#include "rs_profiler_utils.h"

namespace OHOS::Rosen {

static std::string GetMetric(const std::string& name)
{
    std::string metric("0");
    SmartPerf::SPUtils::LoadFile(name, metric);
    return metric;
}

static float GetMetricFloat(const std::string& name)
{
    return std::stof(GetMetric(name));
}

static std::string GetCPUTemperatureMetric()
{
    std::vector<std::string> directories;
    SmartPerf::SPUtils::ForDirFiles("/sys/class/thermal", directories);

    std::string type;
    for (const std::string& directory : directories) {
        SmartPerf::SPUtils::LoadFile(directory + "/type", type);
        if (type.find("soc_thermal") != std::string::npos) {
            return directory + "/temp";
        }
    }

    return "";
}

static void GetCPUTemperature(CPUInfo& cpu)
{
    static const std::string TEMPERATURE_METRIC = GetCPUTemperatureMetric();
    cpu.temperature = GetMetricFloat(TEMPERATURE_METRIC) * 1e-3f;
}

static void GetCPUCurrent(CPUInfo& cpu)
{
    cpu.current = GetMetricFloat("/sys/class/power_supply/Battery/current_now") * 1e-6f;
}

static void GetCPUVoltage(CPUInfo& cpu)
{
    cpu.voltage = GetMetricFloat("/sys/class/power_supply/Battery/voltage_now") * 1e-6f;
}

static void GetCPUMemory(CPUInfo& cpu)
{
    std::ifstream memoryFile("/proc/meminfo", std::ios::in);
    if (!memoryFile) {
        return;
    }

    struct {
        const char* name;
        uint64_t& value;
    } properties[] = {
        { "MemTotal", cpu.ramTotal },
        { "MemFree", cpu.ramFree },
    };

    const size_t propertyCount = sizeof(properties) / sizeof(properties[0]);

    size_t found = 0U;
    std::string line;
    while (getline(memoryFile, line, '\n') && (found < propertyCount)) {
        if (line.find(properties[found].name) != std::string::npos) {
            properties[found].value = std::stoull(SmartPerf::SPUtils::ExtractNumber(line));
            found++;
        }
    }
}

static void GetCPUCores(CPUInfo& cpu)
{
    SmartPerf::CPU& cpuPerf = SmartPerf::CPU::GetInstance();

    const std::vector<SmartPerf::CpuFreqs> frequency = cpuPerf.GetCpuFreq();
    const std::vector<SmartPerf::CpuUsageInfos> usage = cpuPerf.GetCpuUsage();

    cpu.cores = std::min(CPUInfo::MAX_CORE_COUNT, static_cast<uint32_t>(frequency.size()));

    for (uint32_t i = 0; i < cpu.cores; i++) {
        // Frequency
        const uint32_t frequencyCpuId = std::min(static_cast<uint32_t>(frequency[i].cpuId), cpu.cores - 1u);
        cpu.coreFrequency[frequencyCpuId] = frequency[i].curFreq * 1e-6f;

        // Load
        const uint32_t loadCpuId = std::min(static_cast<uint32_t>(std::atoi(usage[i].cpuId.data())), cpu.cores - 1u);
        cpu.coreLoad[loadCpuId] = usage[i].userUsage + usage[i].niceUsage + usage[i].systemUsage + usage[i].idleUsage +
                                  usage[i].ioWaitUsage + usage[i].irqUsage + usage[i].softIrqUsage;
    }
}

static void GetGPUFrequency(GPUInfo& gpu)
{
    static const std::string PATHS[] = {
        "/sys/class/devfreq/fde60000.gpu/cur_freq",
        "/sys/class/devfreq/gpufreq/cur_freq",
    };

    std::string frequency;
    for (const auto& path : PATHS) {
        if (SmartPerf::SPUtils::LoadFile(path, frequency)) {
            break;
        }
    }

    gpu.frequency = std::stof(frequency) * 1e-9f;
}

static void GetGPULoad(GPUInfo& gpu)
{
    static const std::string PATHS[] = {
        "/sys/class/devfreq/gpufreq/gpu_scene_aware/utilisation",
        "/sys/class/devfreq/fde60000.gpu/load",
    };

    std::string load;
    for (const auto& path : PATHS) {
        if (SmartPerf::SPUtils::LoadFile(path, load)) {
            break;
        }
    }

    std::vector<std::string> splits;
    SmartPerf::SPUtils::StrSplit(load, "@", splits);

    gpu.load = std::stof(splits.empty() ? load : splits[0]);
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
    GetGPUFrequency(info.gpu);
    GetGPULoad(info.gpu);

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
    return std::to_string(memory * 1e-6) + " GB";
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

    string +=
        "\nTemperature: " + TemperatureToString(info.cpu.temperature) +
        "\nCurrent: " + CurrentToString(info.cpu.current) + "\nVoltage: " + VoltageToString(info.cpu.voltage) +
        "\nRAM Total: " + MemoryToString(info.cpu.ramTotal) + "\nRAM Free: " + MemoryToString(info.cpu.ramFree) +
        "\nGPU: " + FrequencyLoadToString(info.gpu.frequency, info.gpu.load);

    return string;
}

} // namespace OHOS::Rosen