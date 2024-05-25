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

#include "rs_profiler.h"
#include "rs_profiler_network.h"
#include "rs_profiler_telemetry.h"
#include "rs_profiler_utils.h"
#include "rs_profiler_file.h"

namespace OHOS::Rosen {

static constexpr uint32_t INACTIVITY_THRESHOLD_SECONDS = 5u;
static DeviceInfo g_deviceInfo;
static std::mutex g_deviceInfoMutex;
static bool g_started = false;
static double g_inactiveTimestamp = 0.0;
static std::vector<std::string> g_records;
static double g_recordsTimestamp = 0.0;

// implemented in rs_profiler.cpp
void DeviceInfoToCaptureData(double time, const DeviceInfo& in, RSCaptureData& out);

static bool HasInitializationFinished()
{
    constexpr uint32_t maxAttempts = 100u;
    static uint32_t attempt = 0u;
    if (attempt < maxAttempts) {
        attempt++;
    }
    return attempt == maxAttempts;
}

bool RSProfiler::IsBetaRecordInactive()
{
    return (Now() - g_inactiveTimestamp) > INACTIVITY_THRESHOLD_SECONDS;
}

void RSProfiler::RequestVSyncOnBetaRecordInactivity()
{
    if (IsBetaRecordInactive()) {
        RequestNextVSync();
    }
}

void RSProfiler::LaunchBetaRecordNotificationThread()
{
    std::thread thread([]() {
        while (IsBetaRecordStarted()) {
            RequestVSyncOnBetaRecordInactivity();
            std::this_thread::sleep_for(std::chrono::seconds(INACTIVITY_THRESHOLD_SECONDS));
        }
    });
    thread.detach();
}

void RSProfiler::LaunchBetaRecordMetricsUpdateThread()
{
    if (!IsBetaRecordEnabledWithMetrics()) {
        return;
    }
    std::thread thread([]() {
        while (IsBetaRecordStarted()) {
            const DeviceInfo deviceInfo = RSTelemetry::GetDeviceInfo();

            g_deviceInfoMutex.lock();
            g_deviceInfo = deviceInfo;
            g_deviceInfoMutex.unlock();

            constexpr int32_t sendInterval = 8;
            std::this_thread::sleep_for(std::chrono::milliseconds(sendInterval));
        }
    });
    thread.detach();
}

void RSProfiler::StartBetaRecord()
{
    if (HasInitializationFinished() && !IsBetaRecordStarted() && IsBetaRecordEnabled()) {
        g_started = true;
        g_inactiveTimestamp = Now();

        LaunchBetaRecordNotificationThread();
        LaunchBetaRecordMetricsUpdateThread();

        // Start recording for the first file
        RecordStart(ArgList());
    }
}

bool RSProfiler::IsBetaRecordStarted()
{
    return g_started;
}

void RSProfiler::SendBetaRecordPath()
{
    if (g_records.empty()) {
        return;
    }

    const std::string directory = Utils::GetDirectory(g_records[0]);

    std::string path;
    for (size_t i = 0; i < g_records.size(); i++) {
        const auto& original = g_records[i];
        const auto renamed = Utils::MakePath(directory, "rec_" + std::to_string(g_records.size() - i - 1) + ".ohr");
        const auto message = "BetaRecord: Rename: " + original + " -> " + renamed;
        if (std::rename(original.data(), renamed.data())) {
            Network::SendMessage(message + "failed");
            path.clear();
            break;
        }
         
        Network::SendMessage(message);
        path += renamed + ";";
    }
        
    Network::SendBetaRecordPath(path);
}

bool RSProfiler::SaveBetaRecord()
{
    if (!IsBetaRecordSavingTriggered()) {
        return false;
    }

    RecordStop(ArgList());
    SendBetaRecordPath();
    EnableBetaRecord();
    g_records.clear();
    RecordStart(ArgList());
    return true;
}

void RSProfiler::UpdateBetaRecord()
{
    if (!IsBetaRecordStarted()) {
        return;
    }

    if (!IsBetaRecordEnabled()) {
        RecordStop(ArgList());
        g_started = false;
    }

    if (!IsRecording()) {
        return;
    }

    if (!SaveBetaRecord()) {
        constexpr uint32_t recordMaxLengthSeconds = 90u;
        const double recordLength = Now() - g_recordsTimestamp;
        if (IsBetaRecordInactive() && (recordLength > recordMaxLengthSeconds)) {
            RecordStop(ArgList());
            RecordStart(ArgList());
        }
    }

    // the last time any rendering is done
    g_inactiveTimestamp = Now();
}

bool RSProfiler::OpenBetaRecordFile(RSFile& file)
{
    if (!IsBetaRecordStarted()) {
        return false;
    }

    const std::string cacheFile("data/service/el0/render_service/file0");
    constexpr uint32_t maxCacheFiles = 6u;

    static uint32_t index = 0u;
    const auto path = cacheFile + std::to_string(index++) + ".ohr";
    if (index >= maxCacheFiles) {
        index = 0u;
    }

    file.Create(path);

    g_recordsTimestamp = Now();

    constexpr uint32_t maxRecords = 4u;
    if (g_records.size() < maxRecords) {
        g_records.push_back(path);
    } else {
        std::rotate(g_records.begin(), g_records.begin() + 1, g_records.end());
        g_records[g_records.size() - 1] = path;
    }

    return true;
}

void RSProfiler::WriteBetaRecordMetrics(RSFile& file, double time)
{
    if (!IsBetaRecordStarted() || (time < 0.0)) {
        return;
    }

    g_deviceInfoMutex.lock();
    const DeviceInfo deviceInfo = g_deviceInfo;
    g_deviceInfoMutex.unlock();

    RSCaptureData captureData;
    DeviceInfoToCaptureData(time, deviceInfo, captureData);

    std::vector<char> out;
    const char headerType = 3; // TYPE: GFX METRICS
    captureData.Serialize(out);
    out.insert(out.begin(), headerType);

    file.WriteGFXMetrics(0, time, 0, out.data(), out.size());
}

} // namespace OHOS::Rosen