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

#include "rs_profiler_settings.h"

namespace OHOS::Rosen {

static const StringParameter TERMAL("paths.termal");
static const StringParameter CURRENT("paths.current");
static const StringParameter VOLTAGE("paths.voltage");
static const StringParameter MEMORY("paths.memory");
// cpu
static const StringParameter CPU_USAGE("paths.cpu.time");
static const StringParameter CPU_CORES("paths.cpu.cores");
static const StringParameter CPU_FREQUENCY("paths.cpu.frequency");
static const StringParameter CPU_FREQUENCY_POLICY("paths.cpu.frequency.policy");
// gpu
static const StringParameter GPU_FREQUENCY("paths.gpu.frequency");
static const StringParameter GPU_FREQUENCY_MIN("paths.gpu.frequency.min");
static const StringParameter GPU_FREQUENCY_MAX("paths.gpu.frequency.max");
static const StringParameter GPU_LOAD("paths.gpu.load");

struct CpuTime final {
    static const uint32_t COUNT = 7;
    double times[COUNT] = { 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0 };
    double total = 0;
};

static std::string GetTemperaturePath()
{
    static std::string path;
    if (!path.empty()) {
        return path;
    }

    std::vector<std::string> directories;
    Utils::IterateDirectory(*TERMAL, directories);

    std::string type;
    for (const std::string& directory : directories) {
        Utils::LoadContent(Utils::MakePath(directory, "type"), type);
        if (type.find("soc_thermal") != std::string::npos) {
            path = Utils::MakePath(directory, "temp");
            break;
        }
    }

    return path;
}

// cpufreq
static std::string GetCpuFrequencyPath(uint32_t cpu)
{
    return Utils::MakePath(*CPU_FREQUENCY + std::to_string(cpu), "cpufreq");
}

static std::string GetCpuCurrentFrequencyPath(uint32_t cpu)
{
    return Utils::MakePath(GetCpuFrequencyPath(cpu), "scaling_cur_freq");
}

// cpupolicy
static std::string GetCpuFrequencyPolicyPath(uint32_t cpu)
{
    return *CPU_FREQUENCY_POLICY + std::to_string(cpu);
}

static std::string GetCpuCurrentFrequencyPolicyPath(uint32_t cpu)
{
    return Utils::MakePath(GetCpuFrequencyPolicyPath(cpu), "scaling_cur_freq");
}

static std::string GetCpuMinFrequencyPolicyPath(uint32_t cpu)
{
    return Utils::MakePath(GetCpuFrequencyPolicyPath(cpu), "scaling_min_freq");
}

static std::string GetCpuMaxFrequencyPolicyPath(uint32_t cpu)
{
    return Utils::MakePath(GetCpuFrequencyPolicyPath(cpu), "scaling_max_freq");
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

static std::string FrequencyToString(float frequency)
{
    return std::to_string(frequency) + " GHz";
}

static std::string LoadToString(float load)
{
    return std::to_string(load) + " %";
}

static std::string FrequencyLoadToString(const FrequencyLoadInfo& info)
{
    return FrequencyToString(info.current) + "(min: " + FrequencyToString(info.min) +
           " max: " + FrequencyToString(info.max) + " load: " + LoadToString(info.load) + ")";
}

static std::string GetMetric(const std::string& name)
{
    std::string metric("0");
    Utils::LoadContent(name, metric);
    return metric;
}

static float GetMetricFloat(const std::string& name)
{
    return Utils::ToFp32(GetMetric(name));
}

static void GetCPUTemperature(CPUInfo& cpu)
{
    cpu.temperature = GetMetricFloat(GetTemperaturePath()) * Utils::MILLI;
}

static void GetBattery(CPUInfo& cpu)
{
    cpu.current = GetMetricFloat(*CURRENT) * Utils::MICRO;
    cpu.voltage = GetMetricFloat(*VOLTAGE) * Utils::MICRO;
}

static void GetValue(const std::string& name, const std::vector<std::string>& lines, uint64_t& value)
{
    for (const std::string& line : lines) {
        if (line.find(name) != std::string::npos) {
            value = Utils::ToUint64(Utils::ExtractNumber(line));
            return;
        }
    }
}

static void GetCPUMemory(CPUInfo& cpu)
{
    std::vector<std::string> lines;
    Utils::LoadLines(*MEMORY, lines);

    if (!lines.empty()) {
        GetValue("MemTotal", lines, cpu.ramTotal);
        GetValue("MemFree", lines, cpu.ramFree);
    }
}

static uint32_t GetCpuTime(const std::vector<std::string>& args, CpuTime& info)
{
    constexpr size_t cpuWordSize = 3;
    if (args[0].size() == cpuWordSize) {
        // this is cpu total line
        return UINT_MAX;
    }

    info.total = 0;
    for (uint32_t i = 0; i < CpuTime::COUNT; i++) {
        info.times[i] = Utils::ToUint64(args[i + 1]);
        info.total += info.times[i];
    }

    return Utils::ToUint64(args[0].data() + cpuWordSize);
}

static bool IsCpuLine(const std::vector<std::string>& args)
{
    constexpr size_t required = 11;
    return (args.size() == required) && (args[0].find("cpu") == 0);
}

static void GetCpuTime(std::vector<CpuTime>& infos)
{
    std::vector<std::string> lines;
    Utils::LoadLines(*CPU_USAGE, lines);
    if (lines.empty()) {
        return;
    }

    size_t processed = 0u;
    for (const auto& line : lines) {
        const std::vector<std::string> args = Utils::Split(line);
        if (!IsCpuLine(args) || (processed == infos.size())) {
            // either there are no more cpu lines in the file
            // or all cpus were processed
            return;
        }

        CpuTime info;
        const size_t id = GetCpuTime(args, info);
        if (id < infos.size()) {
            infos[id] = info;
            processed++;
        }
    }
}

static double GetCpuTotalUsage(const CpuTime& info, const CpuTime& lastInfo)
{
    const double deltaTotal = info.total - lastInfo.total;
    if (deltaTotal <= 0.0) {
        return 0.0;
    }

    double usage = 0.0;
    for (uint32_t i = 0; i < CpuTime::COUNT; i++) {
        usage += std::max(info.times[i] - lastInfo.times[i], 0.0);
    }

    constexpr double ratioToPercent = 100.0;
    return usage * ratioToPercent / deltaTotal;
}

static uint32_t GetCoreCount()
{
    std::string line;
    Utils::LoadLine(*CPU_CORES, line);

    if (line.empty()) {
        return 0;
    }

    if (line.size() == 1) {
        return 1;
    }

    constexpr size_t multiCoreLineLength = 3;
    if (line.size() >= multiCoreLineLength) {
        constexpr uint32_t maxCoreIndex = 2;
        return Utils::ToUint32(line.substr(maxCoreIndex)) + 1;
    }

    return 0;
}

static void GetCPUCores(CPUInfo& cpu)
{
    static const uint32_t CORE_COUNT = GetCoreCount();
    cpu.cores = std::min(CPUInfo::MAX_CORES, CORE_COUNT);

    for (uint32_t i = 0; i < cpu.cores; i++) {
        cpu.coreFrequencyLoad[i].current = GetMetricFloat(GetCpuCurrentFrequencyPath(i)) * Utils::MICRO;
    }

    static std::vector<CpuTime> cpuTimeInfos;
    if (cpuTimeInfos.empty()) {
        cpuTimeInfos.resize(cpu.cores);
    }

    static std::vector<CpuTime> lastCpuTimeInfos;
    if (lastCpuTimeInfos.empty()) {
        lastCpuTimeInfos.resize(cpu.cores);
    }

    GetCpuTime(cpuTimeInfos);
    for (size_t i = 0; i < cpu.cores; i++) {
        cpu.coreFrequencyLoad[i].load = GetCpuTotalUsage(cpuTimeInfos[i], lastCpuTimeInfos[i]);
    }
    lastCpuTimeInfos = cpuTimeInfos;
}

static void GetGPUFrequencyLoad(GPUInfo& gpu)
{
    gpu.frequencyLoad.current = GetMetricFloat(*GPU_FREQUENCY) * Utils::NANO;
    gpu.frequencyLoad.min = GetMetricFloat(*GPU_FREQUENCY_MIN) * Utils::NANO;
    gpu.frequencyLoad.max = GetMetricFloat(*GPU_FREQUENCY_MAX) * Utils::NANO;
    gpu.frequencyLoad.load = GetMetricFloat(*GPU_LOAD);
}

const DeviceInfo& RSTelemetry::GetDeviceInfo()
{
    static DeviceInfo info;
    GetBattery(info.cpu);
    GetCPUTemperature(info.cpu);
    GetCPUMemory(info.cpu);
    GetCPUCores(info.cpu);
    GetGPUFrequencyLoad(info.gpu);
    return info;
}

std::string RSTelemetry::GetDeviceInfoString()
{
    const DeviceInfo info = GetDeviceInfo();

    std::string out;
    for (size_t i = 0; i < info.cpu.cores; i++) {
        out += +"\nCPU" + std::to_string(i) + ": " + FrequencyLoadToString(info.cpu.coreFrequencyLoad[i]);
    }

    out += "\nTemperature: " + TemperatureToString(info.cpu.temperature) +
           "\nCurrent: " + CurrentToString(info.cpu.current) + "\nVoltage: " + VoltageToString(info.cpu.voltage) +
           "\nRAM Total: " + MemoryToString(info.cpu.ramTotal) + "\nRAM Free: " + MemoryToString(info.cpu.ramFree) +
           "\nGPU: " + FrequencyLoadToString(info.gpu.frequencyLoad);

    return out;
}

std::string RSTelemetry::GetDeviceFrequencyString()
{
    std::string out;

    constexpr int32_t count = 3;
    for (int32_t i = 0; i < count; i++) {
        const auto current = GetMetricFloat(GetCpuCurrentFrequencyPolicyPath(i)) * Utils::MICRO;
        const auto max = GetMetricFloat(GetCpuMaxFrequencyPolicyPath(i)) * Utils::MICRO;
        const auto min = GetMetricFloat(GetCpuMinFrequencyPolicyPath(i)) * Utils::MICRO;
        out += "CPU" + std::to_string(i) + ": " + FrequencyToString(current) + "(min=" + FrequencyToString(min) +
               " max=" + FrequencyToString(max) + ")\n";
    }

    const DeviceInfo info = GetDeviceInfo();
    out += "GPU: " + FrequencyLoadToString(info.gpu.frequencyLoad);

    return out;
}

std::string RSTelemetry::GetCpuAffinityString()
{
    std::string out = "Cpu affinity mask: ";
    const uint32_t cores = GetCoreCount();
    for (uint32_t i = 0; i < cores; i++) {
        out += Utils::GetCpuAffinity(i) ? "1" : "0";
    }

    return out;
}
} // namespace OHOS::Rosen