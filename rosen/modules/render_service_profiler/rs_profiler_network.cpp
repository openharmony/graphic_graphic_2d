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

#include "rs_profiler_network.h"

#include <fstream>
#include <memory>
#include <thread>

#include "rs_profiler.h"
#include "rs_profiler_file.h"
#include "rs_profiler_packet.h"
#include "rs_profiler_socket.h"
#include "rs_profiler_telemetry.h"
#include "rs_profiler_utils.h"

#include "pipeline/rs_main_thread.h"

namespace OHOS::Rosen {

bool Network::isRunning_ = false;

std::mutex Network::incomingMutex_ {};
std::vector<std::string> Network::incoming_ {};

std::mutex Network::outgoingMutex_ {};
std::queue<std::vector<char>> Network::outgoing_ {};

static void AwakeRenderServiceThread()
{
    RSMainThread::Instance()->PostTask([]() {
        RSMainThread::Instance()->SetAccessibilityConfigChanged();
        RSMainThread::Instance()->RequestNextVSync();
    });
}

static uint32_t OnBinaryPrepare(RSFile& file, const char* data, size_t size)
{
    file.Create(RSFile::GetDefaultPath());
    return BinaryHelper::BinaryCount(data);
}

static void OnBinaryHeader(RSFile& file, const char* data, size_t size)
{
    if (!file.IsOpen()) {
        return;
    }

    std::stringstream stream(std::ios::in | std::ios::out | std::ios::binary);
    stream.write(reinterpret_cast<const char*>(data + 1), size - 1);
    stream.seekg(0);

    double writeStartTime = 0.0;
    stream.read(reinterpret_cast<char*>(&writeStartTime), sizeof(writeStartTime));
    file.SetWriteTime(writeStartTime);

    uint32_t pidCount = 0u;
    stream.read(reinterpret_cast<char*>(&pidCount), sizeof(pidCount));
    for (uint32_t i = 0; i < pidCount; i++) {
        pid_t pid = 0u;
        stream.read(reinterpret_cast<char*>(&pid), sizeof(pid));
        file.AddHeaderPid(pid);
    }
    file.AddLayer();

    std::string dataFirstFrame;
    uint32_t sizeDataFirstFrame = 0;
    stream.read(reinterpret_cast<char*>(&sizeDataFirstFrame), sizeof(sizeDataFirstFrame));
    dataFirstFrame.resize(sizeDataFirstFrame);
    stream.read(reinterpret_cast<char*>(&dataFirstFrame[0]), sizeDataFirstFrame);
    file.AddHeaderFirstFrame(dataFirstFrame);

    ImageCache& cache = RSProfiler::GetImageCache();
    RSProfiler::ClearImageCache();

    uint32_t imageCount = 0u;
    stream.read(reinterpret_cast<char*>(&imageCount), sizeof(imageCount));
    for (uint32_t i = 0; i < imageCount; i++) {
        uint64_t key = 0u;
        stream.read(reinterpret_cast<char*>(&key), sizeof(key));

        ImageCacheRecord record;
        stream.read(reinterpret_cast<char*>(&record.skipBytes), sizeof(record.skipBytes));
        stream.read(reinterpret_cast<char*>(&record.imageSize), sizeof(record.imageSize));

        std::shared_ptr<uint8_t[]> image = std::make_unique<uint8_t[]>(record.imageSize);
        stream.read(reinterpret_cast<char*>(image.get()), record.imageSize);
        record.image = image;
        cache.insert({ key, record });
    }
}

static void OnBinaryChunk(RSFile& file, const char* data, size_t size)
{
    constexpr size_t timeOffset = 8 + 1;
    if (file.IsOpen() && (size >= timeOffset)) {
        const double time = *(reinterpret_cast<const double*>(data + 1));
        file.WriteRSData(time, const_cast<char*>(data) + timeOffset, size - timeOffset);
    }
}

static void OnBinaryFinish(RSFile& file, const char* data, size_t size)
{
    file.SetImageCache(reinterpret_cast<FileImageCache*>(&RSProfiler::GetImageCache()));
    file.Close();
}

void Network::Run()
{
    const uint16_t port = 5050;
    const uint32_t sleepTimeout = 500000u;

    Socket* socket = nullptr;

    isRunning_ = true;

    while (isRunning_) {
        if (!socket) {
            socket = new Socket();
        }

        const SocketState state = socket->GetState();
        if (state == SocketState::INITIAL) {
            socket->Open(port);
        } else if (state == SocketState::CREATE) {
            socket->AcceptClient();
            usleep(sleepTimeout);
        } else if (state == SocketState::ACCEPT) {
            bool readyToReceive = false;
            bool readyToSend = false;
            socket->GetStatus(readyToReceive, readyToSend);

            if (readyToReceive) {
                ProcessIncoming(*socket);
            }
            if (readyToSend) {
                ProcessOutgoing(*socket);
            }
        } else if (state == SocketState::SHUTDOWN) {
            delete socket;
            socket = nullptr;
        }
    }

    delete socket;
}

void Network::Stop()
{
    isRunning_ = false;
}

std::vector<NetworkStats> Network::GetStats(const std::string& interface)
{
    static const uint32_t INTERFACE_COLUMN = 0u;
    static const uint32_t RECV_BYTES_COLUMN = 1u;
    static const uint32_t SENT_BYTES_COLUMN = 9u;

    std::ifstream netdev("/proc/net/dev");
    if (!netdev.good()) {
        return {};
    }

    std::vector<NetworkStats> results;

    std::string data;
    // skip the first two lines (headers)
    std::getline(netdev, data);
    std::getline(netdev, data);

    while (netdev.good()) {
        std::getline(netdev, data);
        std::vector<std::string> parts = Utils::Split(data);
        if (parts.empty()) {
            continue;
        }

        std::string candidate = parts[INTERFACE_COLUMN];
        // remove the trailing ':' so we can compare against the provided interface
        candidate.pop_back();

        if (("*" == interface) || (candidate == interface)) {
            const uint64_t recvBytes = std::stoull(parts[RECV_BYTES_COLUMN]);
            const uint64_t sentBytes = std::stoull(parts[SENT_BYTES_COLUMN]);

            results.push_back({ .interface = candidate, .receivedBytes = recvBytes, .transmittedBytes = sentBytes });
        }
    };

    return results;
}

void Network::SendRdcPath(const std::string& path)
{
    if (!path.empty()) {
        std::string out;
        out += static_cast<char>(PackageID::RS_PROFILER_RDC_BINARY);
        out += path;
        SendBinary(out.data(), out.size());
    }
}

void Network::SendDclPath(const std::string& path)
{
    if (!path.empty()) {
        std::string out;
        out += static_cast<char>(PackageID::RS_PROFILER_DCL_BINARY);
        out += path;
        SendBinary(out.data(), out.size());
    }
}

void Network::SendSkp(const void* data, size_t size)
{
    if (data && (size > 0)) {
        std::vector<char> buffer;
        buffer.reserve(size + 1);
        buffer.push_back(static_cast<char>(PackageID::RS_PROFILER_SKP_BINARY));
        buffer.insert(buffer.end(), static_cast<const char*>(data), static_cast<const char*>(data) + size);
        SendBinary(buffer.data(), buffer.size());
    }
}

void Network::SendTelemetry(double startTime)
{
    if (startTime < 0.0) {
        return;
    }

    const DeviceInfo deviceInfo = RSTelemetry::GetDeviceInfo();

    std::stringstream load;
    std::stringstream frequency;
    for (uint32_t i = 0; i < deviceInfo.cpu.cores; i++) {
        load << deviceInfo.cpu.coreLoad[i];
        if (i + 1 < deviceInfo.cpu.cores) {
            load << ";";
        }
        frequency << deviceInfo.cpu.coreFrequency[i];
        if (i + 1 < deviceInfo.cpu.cores) {
            frequency << ";";
        }
    }

    RSCaptureData captureData;
    captureData.SetTime(Utils::Now() - startTime);
    captureData.SetProperty(RSCaptureData::KEY_CPU_TEMP, deviceInfo.cpu.temperature);
    captureData.SetProperty(RSCaptureData::KEY_CPU_CURRENT, deviceInfo.cpu.current);
    captureData.SetProperty(RSCaptureData::KEY_CPU_LOAD, load.str());
    captureData.SetProperty(RSCaptureData::KEY_CPU_FREQ, frequency.str());
    captureData.SetProperty(RSCaptureData::KEY_GPU_LOAD, deviceInfo.gpu.load);
    captureData.SetProperty(RSCaptureData::KEY_GPU_FREQ, deviceInfo.gpu.frequency);

    std::vector<char> out;
    captureData.Serialize(out);
    const char headerType = 3; // TYPE: GFX METRICS
    out.insert(out.begin(), headerType);
    SendBinary(out.data(), out.size());
}

void Network::SendRSTreePerfNodeList(const std::unordered_set<uint64_t>& perfNodesList)
{
    Packet packet { Packet::BINARY };
    packet.Write(static_cast<char>(PackageID::RS_PROFILER_RSTREE_PERF_NODE_LIST));
    packet.Write(perfNodesList);
    const std::lock_guard<std::mutex> guard(outgoingMutex_);
    outgoing_.emplace(packet.Release());
}

void Network::SendRSTreeSingleNodePerf(uint64_t id, uint64_t nanosec)
{
    Packet packet { Packet::BINARY };
    packet.Write(static_cast<char>(PackageID::RS_PROFILER_RSTREE_SINGLE_NODE_PERF));
    packet.Write(id);
    packet.Write(nanosec);
    const std::lock_guard<std::mutex> guard(outgoingMutex_);
    outgoing_.emplace(packet.Release());
}

void Network::SendBinary(const void* data, size_t size)
{
    if (data && (size > 0)) {
        Packet packet { Packet::BINARY };
        packet.Write(data, size);
        const std::lock_guard<std::mutex> guard(outgoingMutex_);
        outgoing_.emplace(packet.Release());
    }
}

void Network::SendMessage(const std::string& message)
{
    if (!message.empty()) {
        Packet packet { Packet::LOG };
        packet.Write(message);
        const std::lock_guard<std::mutex> guard(outgoingMutex_);
        outgoing_.emplace(packet.Release());
    }
}

void Network::PushCommand(const std::vector<std::string>& args)
{
    const std::lock_guard<std::mutex> guard(incomingMutex_);
    incoming_ = args;
}

void Network::PopCommand(std::string& command, std::vector<std::string>& args)
{
    command.clear();
    args.clear();

    const std::lock_guard<std::mutex> guard(incomingMutex_);
    if (!incoming_.empty()) {
        command = incoming_[0];
        if (incoming_.size() > 1) {
            args = std::vector<std::string>(incoming_.begin() + 1, incoming_.end());
        }
        incoming_.clear();
    }
}

void Network::ProcessCommand(const char* data, size_t size)
{
    const std::vector<std::string> args = Utils::Split({ data, size });
    if (args.empty()) {
        return;
    }

    PushCommand(args);
    AwakeRenderServiceThread();
}

void Network::ProcessOutgoing(Socket& socket)
{
    std::vector<char> data;

    bool nothingToSend = false;
    while (!nothingToSend) {
        {
            const std::lock_guard<std::mutex> guard(outgoingMutex_);
            nothingToSend = outgoing_.empty();
            if (!nothingToSend) {
                data.swap(outgoing_.front());
                outgoing_.pop();
            }
        }

        if (!nothingToSend) {
            socket.SendWhenReady(data.data(), data.size());
        }
    }
}

void Network::ProcessBinary(const char* data, size_t size)
{
    static uint32_t chunks = 0u;
    static RSFile file;

    const PackageID id = BinaryHelper::Type(data);
    if (id == PackageID::RS_PROFILER_PREPARE) {
        // ping/pong for connection speed measurement
        const char type = static_cast<char>(PackageID::RS_PROFILER_PREPARE);
        SendBinary(&type, sizeof(type));
        // amount of binary packages will be sent
        chunks = OnBinaryPrepare(file, data, size);
    } else if (id == PackageID::RS_PROFILER_HEADER) {
        OnBinaryHeader(file, data, size);
    } else if (id == PackageID::RS_PROFILER_BINARY) {
        OnBinaryChunk(file, data, size);

        chunks--;
        if (chunks == 0) {
            OnBinaryFinish(file, data, size);
            const char type = static_cast<char>(PackageID::RS_PROFILER_PREPARE_DONE);
            SendBinary(&type, sizeof(type));
        }
    }
}

void Network::ProcessIncoming(Socket& socket)
{
    const uint32_t sleepTimeout = 500000u;

    Packet packetIncoming { Packet::UNKNOWN };
    auto wannaReceive = Packet::HEADER_SIZE;
    socket.Receive(packetIncoming.Begin(), wannaReceive);

    if (wannaReceive == 0) {
        socket.SetState(SocketState::SHUTDOWN);
        usleep(sleepTimeout);
        return;
    }

    const size_t size = packetIncoming.GetPayloadLength();
    if (size == 0) {
        return;
    }

    std::vector<char> data;
    data.resize(size);
    socket.ReceiveWhenReady(data.data(), data.size());

    if (packetIncoming.IsBinary()) {
        ProcessBinary(data.data(), data.size());
    } else if (packetIncoming.IsCommand()) {
        ProcessCommand(data.data(), data.size());
    }
}

void Network::ReportStats()
{
    constexpr uint32_t bytesToBits = 8u;
    const std::string interface("wlan0");
    const std::vector<NetworkStats> stats = Network::GetStats(interface);

    std::string out = "Interface: " + interface;
    for (const NetworkStats& stat : stats) {
        out += "Transmitted: " + std::to_string(stat.transmittedBytes * bytesToBits);
        out += "Received: " + std::to_string(stat.transmittedBytes * bytesToBits);
    }

    SendMessage(out);
}

} // namespace OHOS::Rosen