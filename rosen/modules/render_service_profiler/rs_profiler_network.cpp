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
#include <thread>

#include "rs_profiler_base.h"
#include "rs_profiler_file.h"
#include "rs_profiler_packet.h"
#include "rs_profiler_socket.h"
#include "rs_profiler_utils.h"

#include "pipeline/rs_main_thread.h"

namespace OHOS::Rosen {

std::mutex Network::commandMutex_ {};
std::vector<std::string> Network::commandData_ {};

std::mutex Network::outgoingMutex_ {};
std::list<std::vector<char>> Network::outgoing_ {};

static void AwakeRenderServiceThread()
{
    RSMainThread::Instance()->PostTask([]() {
        RSMainThread::Instance()->SetAccessibilityConfigChanged();
        RSMainThread::Instance()->RequestNextVSync();
    });
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

void Network::SendBinary(void* data, int size)
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

void Network::PushCommand(const std::vector<std::string>& command)
{
    const std::lock_guard<std::mutex> guard(commandMutex_);
    commandData_ = command;
}

void Network::PopCommand(std::string& command, std::vector<std::string>& args)
{
    command.clear();
    args.clear();

    const std::lock_guard<std::mutex> guard(commandMutex_);
    if (!commandData_.empty()) {
        command = commandData_[0];
        if (commandData_.size() > 1) {
            args = std::vector<std::string>(commandData_.begin() + 1, commandData_.end());
        }
        commandData_.clear();
    }
}

void Network::ProcessCommand(const char* data, size_t size)
{
    // TODO(user): Refactor
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

    std::reverse(args.begin(), args.end());
    // END TODO

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
        // pop data
        {
            const std::lock_guard<std::mutex> guard(outgoingMutex_);
            nothingToSend = outgoing_.empty();
            if (!nothingToSend) {
                data.swap(outgoing_.front());
                outgoing_.pop_front();
            }
        }

        // send data
        if (!nothingToSend) {
            socket.SendWhenReady(data.data(), data.size());
        }
    }
}

void Network::ProcessBinary(const char* data, size_t size)
{
    static int32_t pendingBinary = 0;
    static RSFile file;

    const PackageID id = BinaryHelper::Type(data);
    if (id == PackageID::RS_PROFILER_PREPARE) {
        // ping/pong for connection speed measurement
        char type = static_cast<char>(PackageID::RS_PROFILER_PREPARE);
        SendBinary((void*)&type, 1);
        // amount of binary packages will be sent
        pendingBinary = BinaryHelper::BinaryCount(data);
        file.Create(RSFile::GetDefaultPath());
    } else if (id == PackageID::RS_PROFILER_HEADER) {
        if (file.IsOpen()) {
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

                std::shared_ptr<uint8_t> image(new uint8_t[record.imageSize]);
                stream.read(reinterpret_cast<char*>(image.get()), record.imageSize);
                record.image = image;
                imageMap.insert({ key, record });
            }
        }
    } else if (id == PackageID::RS_PROFILER_BINARY) {
        if (file.IsOpen() && (size > 0)) {
            const double time = *(reinterpret_cast<const double*>(data + 1));
            file.WriteRSData(time, const_cast<char*>(data) + (8 + 1), size - (8 + 1));
        }

        pendingBinary--;
        if (pendingBinary == 0) {
            file.SetImageMapPtr(
                reinterpret_cast<std::map<uint64_t, ReplayImageCacheRecordFile>*>(&RSProfilerBase::ImageMapGet()));
            char type = static_cast<char>(PackageID::RS_PROFILER_PREPARE_DONE);
            SendBinary((void*)&type, 1);
            file.Close();
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
    data.resize(size + 2); // WTF?
    socket.ReceiveWhenReady(data.data(), size);
    data[size] = 0;

    if (packetIncoming.IsBinary()) {
        ProcessBinary(data.data(), size);
    } else if (packetIncoming.IsCommand()) {
        ProcessCommand(data.data(), size + 1); // WTF?

        // DEPRECATED: REMOVE THIS!
        errno = EWOULDBLOCK;
        Packet packetReply { Packet::COMMAND_ACKNOWLEDGED };
        packetReply.Write(std::string(" "));
        const std::lock_guard<std::mutex> guard(outgoingMutex_);
        outgoing_.emplace_back(packetReply.Release());
        // DEPRECATED
    }
}

void Network::Run()
{
    const uint16_t port = 5050;
    const uint32_t sleepTimeout = 500000u;

    while (true) {
        static Socket* socket = nullptr;
        if (socket == nullptr) {
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
            socket->SetState(SocketState::BEFORE_START);
            delete socket;
            socket = nullptr;
        }
    }
}

} // namespace OHOS::Rosen