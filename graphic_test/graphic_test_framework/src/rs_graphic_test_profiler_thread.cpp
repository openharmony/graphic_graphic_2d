/*
 * Copyright (c) 2025 Huawei Device Co., Ltd.
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

#include "rs_graphic_test_profiler_thread.h"

#include <securec.h>
#include <sys/un.h>
#include <unistd.h>

#include "rs_graphic_test_utils.h"
#ifdef RS_PROFILER_ENABLED
#include "rs_profiler_packet.h"
#endif

namespace OHOS {
namespace Rosen {
#ifdef RS_PROFILER_ENABLED
constexpr int64_t INIT_WAIT_TIME = 50;
constexpr int64_t SOCKET_REFRESH_TIME = 20;
constexpr int SOCKET_CONNECT_MAX_NUM = 10000;

RSGraphicTestProfilerThread::~RSGraphicTestProfilerThread()
{
    Stop();
}

void RSGraphicTestProfilerThread::Start()
{
    system("param set persist.graphic.profiler.enabled 1");
    WaitTimeout(INIT_WAIT_TIME);
    runnig_ = true;
    thread_ = std::thread(&RSGraphicTestProfilerThread::MainLoop, this);
}

void RSGraphicTestProfilerThread::Stop()
{
    system("param set persist.graphic.profiler.enabled 0");
    if (runnig_) {
        runnig_ = false;
        cv_.notify_all();
    }
    if (socket_ != -1) {
        close(socket_);
        socket_ = -1;
    }
    if (thread_.joinable()) {
        thread_.join();
    }
}

void RSGraphicTestProfilerThread::SendCommand(const std::string command)
{
    std::unique_lock lock(queue_mutex_);
    message_queue_.push(command);
}

void RSGraphicTestProfilerThread::MainLoop()
{
    socket_ = socket(AF_UNIX, SOCK_STREAM, 0);
    if (socket_ == -1) {
        std::cout << "profiler socket create failed" << std::endl;
        return;
    }

    const std::string socketName = "render_service_5050";
    sockaddr_un address {};
    address.sun_family = AF_UNIX;
    address.sun_path[0] = 0;
    ::memmove_s(address.sun_path + 1, sizeof(address.sun_path) - 1, socketName.data(), socketName.size());
    const size_t addressSize = offsetof(sockaddr_un, sun_path) + socketName.size() + 1;

    int tryNum = 1;
    while (connect(socket_, reinterpret_cast<struct sockaddr*>(&address), addressSize) < 0) {
        std::cout << "profiler socket connect failed, connect:" + std::to_string(tryNum) << std::endl;
        tryNum++;
        if (tryNum > SOCKET_CONNECT_MAX_NUM) {
            std::cout << "profiler socket connect failed" << std::endl;
            close(socket_);
            socket_ = -1;
            return;
        }
    }
    std::cout << "profiler socket connect success" << std::endl;
    runnig_ = true;
    while (runnig_) {
        {
            std::unique_lock lock(queue_mutex_);
            cv_.wait_for(lock, std::chrono::milliseconds(SOCKET_REFRESH_TIME), [this] { return !runnig_; });
        }
        SendMessage();
        RecieveMessage();
    }
    close(socket_);
}

void RSGraphicTestProfilerThread::SendMessage()
{
    std::string message;
    {
        std::lock_guard<std::mutex> lock(queue_mutex_);
        if (!message_queue_.empty()) {
            message = message_queue_.front();
            message_queue_.pop();
        }
    }
    if (message.empty()) {
        return;
    }
    Packet packet(Packet::COMMAND);
    packet.Write(message);
    auto data = packet.Release();
    const char* bytes = reinterpret_cast<const char*>(data.data());
    size_t size = data.size();
    size_t sent = 0;
    while (sent < size) {
        const ssize_t sentBytes = send(socket_, bytes, size - sent, 0);
        if ((sentBytes <= 0) && (errno != EINTR)) {
            std::cout << "profiler socket send failed:" + std::to_string(sentBytes) << std::endl;
            return;
        }
        auto actualSentBytes = static_cast<size_t>(sentBytes);
        sent += actualSentBytes;
        bytes += actualSentBytes;
    }
}

void RSGraphicTestProfilerThread::RecieveMessage()
{
    Packet packet { Packet::UNKNOWN };
    auto wannaReceive = Packet::HEADER_SIZE;
    RecieveHeader(packet.Begin(), wannaReceive);
    if (wannaReceive == 0) {
        std::cout << "profiler socket recieve header failed" << std::endl;
        return;
    }
    const size_t size = packet.GetPayloadLength();
    if (size == 0) {
        return;
    }

    std::vector<char> data;
    data.resize(size);
    size_t received = 0;
    char* bytes = static_cast<char*>(data.data());
    while (received < size) {
        const ssize_t receivedBytes = recv(socket_, bytes, size - received, 0);
        if ((receivedBytes == -1) && (errno != EINTR)) {
            std::cout << "profiler socket recieve failed" + std::to_string(receivedBytes) << std::endl;
            return;
        }
        auto actualReceivedBytes = static_cast<size_t>(receivedBytes);
        received += actualReceivedBytes;
        bytes += actualReceivedBytes;
    }

    if (packet.GetType() == Packet::LOG) {
        std::string out(data.begin(), data.end());
        std::cout << out << std::endl;
    }
}

bool RSGraphicTestProfilerThread::RecieveHeader(void* data, size_t& size)
{
    if (!data || (size == 0)) {
        return true;
    }

    const ssize_t receivedBytes = recv(socket_, static_cast<char*>(data), size, 0);
    if (receivedBytes > 0) {
        size = static_cast<size_t>(receivedBytes);
    } else {
        size = 0;
        if ((errno == EWOULDBLOCK) || (errno == EAGAIN) || (errno == EINTR)) {
            return true;
        }
        std::cout << "profiler socket recieve header failed" << std::endl;
        return false;
    }
    return true;
}
#endif
} // namespace Rosen
} // namespace OHOS
