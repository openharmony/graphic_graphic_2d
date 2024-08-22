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

#include <thread>

#include "rs_profiler.h"
#include "rs_profiler_archive.h"
#include "rs_profiler_file.h"
#include "rs_profiler_network.h"
#include "rs_profiler_packet.h"
#include "rs_profiler_telemetry.h"
#include "rs_profiler_utils.h"

namespace OHOS::Rosen {

static constexpr uint32_t INACTIVITY_THRESHOLD_SECONDS = 5u;
static DeviceInfo g_deviceInfo;
static std::mutex g_deviceInfoMutex;
static std::mutex g_fileSavingMutex;
static bool g_started = false;
static double g_inactiveTimestamp = 0.0;
static std::vector<std::string> g_records;
static double g_recordsTimestamp = 0.0;
static double g_currentFrameDirtyRegion = 0.0;

// implemented in rs_profiler.cpp
void DeviceInfoToCaptureData(double time, const DeviceInfo& in, RSCaptureData& out);

static bool HasInitializationFinished()
{
    constexpr uint32_t maxAttempts = 600u;
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

void RSProfiler::WriteBetaRecordFileThread(RSFile& file, const std::string& path)
{
    std::vector<uint8_t> fileData;
    if (!file.GetDataCopy(fileData)) {
        return;
    }
    
    std::thread thread([fileDataCopy{std::move(fileData)}, path]() {
        const std::lock_guard<std::mutex> fileSavingMutex(g_fileSavingMutex);

        FILE* fileCopy = Utils::FileOpen(path, "wbe");
        if (!Utils::IsFileValid(fileCopy)) {
            return;
        }
        Utils::FileWrite(fileCopy, fileDataCopy.data(), fileDataCopy.size());
        Utils::FileClose(fileCopy);
    });
    thread.detach();
}

void RSProfiler::RenameAndSendFilenameThread()
{
    std::thread thread([]() {
        const std::lock_guard<std::mutex> fileSavingMutex(g_fileSavingMutex);
        SendBetaRecordPath();
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
    g_records.clear();

    Network::SendBetaRecordPath(path);
}

bool RSProfiler::SaveBetaRecord()
{
    if (!IsBetaRecordSavingTriggered()) {
        return false;
    }

    RecordStop(ArgList());
    RenameAndSendFilenameThread();
    EnableBetaRecord();
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
        constexpr uint32_t recordMaxLengthSeconds = 30u; // 30sec length of each recording
        const double recordLength = Now() - g_recordsTimestamp;
        if (recordLength > recordMaxLengthSeconds) {
            RecordStop(ArgList());
            RecordStart(ArgList());
        }
    }

    // the last time any rendering is done
    if (g_currentFrameDirtyRegion > 0) {
        g_inactiveTimestamp = Now();
    }
}

bool RSProfiler::OpenBetaRecordFile(RSFile& file)
{
    if (!IsBetaRecordStarted()) {
        return false;
    }

    const auto path = "RECORD_IN_MEMORY";
    file.SetVersion(RSFILE_VERSION_LATEST);
    file.Create(path);

    g_recordsTimestamp = Now();
    return true;
}

bool RSProfiler::SaveBetaRecordFile(RSFile& file)
{
    if (!IsBetaRecordStarted()) {
        return false;
    }

    const std::string cacheFile("data/service/el0/render_service/file0");
    constexpr uint32_t maxCacheFiles = 5u; // 5 recordings in a "ring buffer" way

    static uint32_t index = 0u;
    const auto path = cacheFile + std::to_string(index++) + ".ohr";
    if (index >= maxCacheFiles) {
        index = 0u;
    }

    WriteBetaRecordFileThread(file, path);

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
    if (!IsBetaRecordStarted() || (time < 0.0) || !IsBetaRecordEnabledWithMetrics()) {
        return;
    }

    g_deviceInfoMutex.lock();
    const DeviceInfo deviceInfo = g_deviceInfo;
    g_deviceInfoMutex.unlock();

    RSCaptureData captureData;
    DeviceInfoToCaptureData(time, deviceInfo, captureData);

    std::vector<char> out;
    DataWriter archive(out);
    char headerType = static_cast<char>(PackageID::RS_PROFILER_GFX_METRICS);
    archive.Serialize(headerType);
    captureData.Serialize(archive);

    file.WriteGFXMetrics(0, time, 0, out.data(), out.size());
}

void RSProfiler::UpdateDirtyRegionBetaRecord(double currentFrameDirtyRegion)
{
    g_currentFrameDirtyRegion = currentFrameDirtyRegion;
}

} // namespace OHOS::Rosen