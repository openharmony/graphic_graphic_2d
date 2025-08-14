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

#include "rs_profiler_archive.h"
#include "rs_profiler_cache.h"
#include "rs_profiler_capturedata.h"
#include "rs_profiler_file.h"
#include "rs_profiler_log.h"
#include "rs_profiler_packet.h"
#include "rs_profiler_socket.h"
#include "rs_profiler_utils.h"

#include "pipeline/main_thread/rs_main_thread.h"

namespace OHOS::Rosen {

std::atomic<bool> Network::isRunning_ = false;
std::atomic<bool> Network::forceShutdown_ = false;
std::atomic<bool> Network::blockBinary_ = false;
std::chrono::steady_clock::time_point Network::ping_;

std::mutex Network::incomingMutex_ {};
std::queue<std::vector<std::string>> Network::incoming_ {};

std::mutex Network::outgoingMutex_ {};
std::queue<std::vector<char>> Network::outgoing_ {};

static void AwakeRenderServiceThread()
{
    RSMainThread::Instance()->SetAccessibilityConfigChanged();
    RSMainThread::Instance()->RequestNextVSync();
    RSMainThread::Instance()->PostTask([]() {
        RSMainThread::Instance()->SetAccessibilityConfigChanged();
        RSMainThread::Instance()->RequestNextVSync();
    });
}

bool Network::IsRunning()
{
    return isRunning_;
}

void Network::ResetPing()
{
    ping_ = std::chrono::steady_clock::now();
}

void Network::Ping(const Socket& socket)
{
    if (!socket.Connected()) {
        return;
    }

    using namespace std::chrono_literals;
    const std::chrono::milliseconds period = 1s;
    const auto now = std::chrono::steady_clock::now();
    if (now <= ping_ + period) {
        return;
    }

    Packet packet(Packet::COMMAND);
    packet.Write("ping");
    SendPacket(packet);
}

void Network::Run()
{
    constexpr uint16_t port = 5050;

    Socket* socket = nullptr;

    isRunning_ = true;
    forceShutdown_ = false;

    while (isRunning_) {
        if (!socket) {
            socket = new Socket();
        }

        const SocketState state = socket->GetState();
        if (forceShutdown_) {
            HRPW("Network: Run: Force shutdown");
            Shutdown(socket);
            forceShutdown_ = false;
        } else if (state == SocketState::INITIAL) {
            socket->Open(port);
        } else if (state == SocketState::CREATE) {
            socket->AcceptClient();
            if (!socket->Connected()) {
                usleep(20000); // 20000: sleep 20ms to reduce power consumption on standing by
            }
        } else if (state == SocketState::CONNECTED) {
            Ping(*socket); // add ping packet to a queue
            Send(*socket);
            Receive(*socket);
        } else if (state == SocketState::SHUTDOWN) {
            HRPW("Network: Run: Socket state got SHUTDOWN");
            Shutdown(socket);
        }
    }

    delete socket;
}

void Network::Stop()
{
    isRunning_ = false;
}

void Network::SendPacket(Packet& packet)
{
    if (isRunning_) {
        const std::lock_guard<std::mutex> guard(outgoingMutex_);
        outgoing_.emplace(packet.Release());
    }
}

void Network::SendPath(const std::string& path, PackageID id)
{
    if (!path.empty()) {
        std::string out;
        out += static_cast<char>(id);
        out += path;
        SendBinary(out);
    }
}

void Network::SendRdcPath(const std::string& path)
{
    SendPath(path, PackageID::RS_PROFILER_RDC_BINARY);
}

void Network::SendDclPath(const std::string& path)
{
    SendPath(path, PackageID::RS_PROFILER_DCL_BINARY);
}

void Network::SendMskpPath(const std::string& path)
{
    SendPath(path, PackageID::RS_PROFILER_MSKP_FILEPATH);
}

void Network::SendBetaRecordPath(const std::string& path)
{
    SendPath(path, PackageID::RS_PROFILER_BETAREC_FILEPATH);
}

void Network::SendSkp(const void* data, size_t size)
{
    if (data && (size > 0)) {
        std::vector<char> buffer;
        buffer.reserve(size + 1);
        buffer.push_back(static_cast<char>(PackageID::RS_PROFILER_SKP_BINARY));
        buffer.insert(buffer.end(), static_cast<const char*>(data), static_cast<const char*>(data) + size);
        SendBinary(buffer);
    }
}

void Network::SendCaptureData(const RSCaptureData& data)
{
    std::vector<char> out;
    DataWriter archive(out);
    char headerType = static_cast<char>(PackageID::RS_PROFILER_GFX_METRICS);
    archive.Serialize(headerType);

    const_cast<RSCaptureData&>(data).Serialize(archive);

    // if no data is serialized, we end up with just 1 char header
    if (out.size() > 1) {
        SendBinary(out);
    }
}

void Network::SendRSTreeDumpJSON(const std::string& jsonstr)
{
    Packet packet { Packet::BINARY };
    packet.Write(static_cast<char>(PackageID::RS_PROFILER_RSTREE_DUMP_JSON));
    packet.Write(jsonstr);
    SendPacket(packet);
}

void Network::SendRSTreePerfNodeList(const std::unordered_set<uint64_t>& perfNodesList)
{
    Packet packet { Packet::BINARY };
    packet.Write(static_cast<char>(PackageID::RS_PROFILER_RSTREE_PERF_NODE_LIST));
    packet.Write(perfNodesList);
    SendPacket(packet);
}

void Network::SendRSTreeSingleNodePerf(uint64_t id, uint64_t nanosec)
{
    Packet packet { Packet::BINARY };
    packet.Write(static_cast<char>(PackageID::RS_PROFILER_RSTREE_SINGLE_NODE_PERF));
    packet.Write(id);
    packet.Write(nanosec);
    SendPacket(packet);
}

void Network::SetBlockBinary(bool blockFlag)
{
    blockBinary_ = blockFlag;
}

void Network::SendBinary(const void* data, size_t size)
{
    if (blockBinary_) {
        return;
    }
    if (data && (size > 0)) {
        Packet packet { Packet::BINARY };
        packet.Write(data, size);
        SendPacket(packet);
    }
}

void Network::SendBinary(const std::vector<char>& data)
{
    SendBinary(data.data(), data.size());
}

void Network::SendBinary(const std::string& data)
{
    SendBinary(data.data(), data.size());
}

void Network::SendMessage(const std::string& message)
{
    if (!message.empty()) {
        Packet packet { Packet::LOG };
        packet.Write(message);
        SendPacket(packet);
    }
}

void Network::ResetSendQueue()
{
    const std::lock_guard<std::mutex> guard(outgoingMutex_);
    outgoing_ = {};
}

void Network::PushCommand(const std::vector<std::string>& args)
{
    if (!args.empty()) {
        const std::lock_guard<std::mutex> guard(incomingMutex_);
        incoming_.emplace(args);
    }
}

void Network::ResetCommandQueue()
{
    const std::lock_guard<std::mutex> guard(incomingMutex_);
    incoming_ = {};
}

bool Network::PopCommand(std::vector<std::string>& args)
{
    args.clear();

    incomingMutex_.lock();
    if (!incoming_.empty()) {
        args.swap(incoming_.front());
        incoming_.pop();
    }
    incomingMutex_.unlock();

    return !args.empty();
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

void Network::Send(Socket& socket)
{
    std::vector<char> data;

    while (socket.Connected()) {
        data.clear();

        outgoingMutex_.lock();
        if (!outgoing_.empty()) {
            data.swap(outgoing_.front());
            outgoing_.pop();
        }
        outgoingMutex_.unlock();

        if (data.empty()) {
            break;
        }
        
        if (socket.SendWhenReady(data.data(), data.size())) {
            ResetPing();
        }
    }
}

void Network::ProcessBinary(const std::vector<char>& data)
{
    if (data.empty()) {
        return;
    }

    std::istringstream stream(data.data(), data.size());
    std::string path;
    stream >> path;

    const auto offset = std::min(path.size() + 1, data.size());
    const auto size = data.size() - offset;
    if (size == 0) {
        HRPE("Network: Receive file: Invalid file size");
        return;
    }

    HRPI("Receive file: %s %.2fMB (%zu)", path.data(), Utils::Megabytes(size), size);
    if (auto file = Utils::FileOpen(path, "wb")) {
        Utils::FileWrite(file, data.data() + offset, size);
        Utils::FileClose(file);
        HRPI("Network: Receive file: Done");
    } else {
        HRPE("Network: Receive file: Cannot open file: %s", path.data());
    }
}

void Network::ForceShutdown()
{
    forceShutdown_ = true;
}

void Network::Shutdown(Socket*& socket)
{
    delete socket;
    socket = nullptr;

    ResetSendQueue();
    ResetCommandQueue();

    RSSystemProperties::SetProfilerDisabled();
    HRPE("Network: Shutdown: persist.graphic.profiler.enabled 0");
}

void Network::Receive(Socket& socket)
{
    if (!socket.Connected()) {
        return;
    }
    constexpr int timeWait = 10;
    if (socket.PollReceive(timeWait) == 0) {
        // no data for 10 ms
        return;
    }

    Packet packet { Packet::UNKNOWN };
    auto wannaReceive = Packet::HEADER_SIZE;
    socket.Receive(packet.Begin(), wannaReceive);

    if (wannaReceive == 0) {
        HRPW("Network: Receive: Invalid header");
        return;
    }

    const size_t size = packet.GetPayloadLength();
    if (size == 0) {
        return;
    }

    std::vector<char> data;
    data.resize(size);
    socket.ReceiveWhenReady(data.data(), data.size());

    if (packet.IsBinary()) {
        ProcessBinary(data);
    } else if (packet.IsCommand()) {
        ProcessCommand(data.data(), data.size());
    }

    ResetPing();
}

} // namespace OHOS::Rosen