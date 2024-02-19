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

#include "rs_profiler_base.h"
#include "rs_profiler_file.h"
#include "rs_profiler_packet.h"
#include "rs_profiler_socket.h"
#include "rs_profiler_utils.h"

#include "pipeline/rs_main_thread.h"

namespace OHOS::Rosen {

bool Network::isRunning_ = false;

std::mutex Network::incomingMutex_ {};
std::vector<std::string> Network::incoming_ {};

std::mutex Network::outgoingMutex_ {};
std::list<std::vector<char>> Network::outgoing_ {};

static void AwakeRenderServiceThread()
{
    RSMainThread::Instance()->PostTask([]() {
        RSMainThread::Instance()->SetAccessibilityConfigChanged();
        RSMainThread::Instance()->RequestNextVSync();
    });
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
        if (state == SocketState::BEFORE_START) {
            socket->Open(port);
        } else if (state == SocketState::CREATE_SOCKET) {
            socket->AcceptClient();
            usleep(sleepTimeout);
        } else if (state == SocketState::ACCEPT_STATE) {
            const int32_t status = socket->Select();
            if (Socket::IsReceiveEnabled(status)) {
                ProcessIncoming(*socket);
            }
            if (Socket::IsSendEnabled(status)) {
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

void Network::SendRdc(const std::string& path)
{
    if (!path.empty()) {
        std::string out;
        out += static_cast<char>(PackageID::RS_PROFILER_RDC_BINARY);
        out += path;
        SendBinary(out.data(), out.size());
    }
}

void Network::SendBinary(const void* data, size_t size)
{
    if (data && (size > 0)) {
        Packet packet { Packet::BINARY };
        packet.Write(data, size);
        const std::lock_guard<std::mutex> guard(outgoingMutex_);
        outgoing_.emplace_back(packet.Release());
    }
}

void Network::SendMessage(const std::string& message)
{
    if (!message.empty()) {
        Packet packet { Packet::LOG };
        packet.Write(message);
        const std::lock_guard<std::mutex> guard(outgoingMutex_);
        outgoing_.emplace_back(packet.Release());
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
    std::vector<std::string> args;
    int32_t end = size;
    bool quote = false;
    int32_t index = size - 1;
    do {
        if (data[index] == '\"') {
            quote = !quote;
        } else if (!quote && (data[index] == ' ')) {
            if (end != (index + 1)) {
                args.emplace_back(&data[index + 1], &data[end]);
            }
            end = index;
        }
        index--;
    } while (index != -1);

    if (index == -1) {
        args.emplace_back(&data[index + 1], &data[end]);
    }

    if (args.empty()) {
        return;
    }

    std::reverse(args.begin(), args.end());

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
                outgoing_.pop_front();
            }
        }

        if (!nothingToSend) {
            socket.SendWhenReady(data.data(), data.size());
        }
    }
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
        file.AddHeaderPID(pid);
    }
    file.AddLayer();

    std::map<uint64_t, ReplayImageCacheRecord>& imageMap = RSProfilerBase::ImageMapGet();
    imageMap.clear();

    uint32_t imageCount = 0u;
    stream.read(reinterpret_cast<char*>(&imageCount), sizeof(imageCount));
    for (uint32_t i = 0; i < imageCount; i++) {
        ReplayImageCacheRecord record;
        uint64_t key = 0u;

        stream.read(reinterpret_cast<char*>(&key), sizeof(key));
        stream.read(reinterpret_cast<char*>(&record.skipBytes), sizeof(record.skipBytes));
        stream.read(reinterpret_cast<char*>(&record.imageSize), sizeof(record.imageSize));

        std::shared_ptr<uint8_t[]> image = std::make_unique<uint8_t[]>(record.imageSize);
        stream.read(reinterpret_cast<char*>(image.get()), record.imageSize);
        record.image = image;
        imageMap.insert({ key, record });
    }
}

static void OnBinaryChunk(RSFile& file, const char* data, size_t size)
{
    if (file.IsOpen() && (size > 0)) {
        constexpr size_t timeOffset = 8 + 1;
        const double time = *(reinterpret_cast<const double*>(data + 1));
        file.WriteRSData(time, const_cast<char*>(data) + timeOffset, size - timeOffset);
    }
}

static void OnBinaryFinish(RSFile& file, const char* data, size_t size)
{
    auto imageMap = reinterpret_cast<std::map<uint64_t, ReplayImageCacheRecordFile>*>(&RSProfilerBase::ImageMapGet());
    file.SetImageMapPtr(imageMap);
    file.Close();
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
    auto wannaReceive = Packet::headerSize;
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

    std::string data;
    constexpr size_t extraSpace = 2;
    data.resize(size + extraSpace);
    socket.ReceiveWhenReady(data.data(), size);
    data[size] = 0;

    if (packetIncoming.IsBinary()) {
        ProcessBinary(data.data(), size);
    } else if (packetIncoming.IsCommand()) {
        ProcessCommand(data.data(), size + 1);

        errno = EWOULDBLOCK;
        Packet acknowledgement { Packet::COMMAND_ACKNOWLEDGED };
        acknowledgement.Write(std::string(" "));
        const std::lock_guard<std::mutex> guard(outgoingMutex_);
        outgoing_.emplace_back(acknowledgement.Release());
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