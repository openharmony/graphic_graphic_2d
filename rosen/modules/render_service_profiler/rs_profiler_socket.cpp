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

#include "rs_profiler_socket.h"

#include <fcntl.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <poll.h>
#include <securec.h>
#include <sys/ioctl.h>
#include <sys/select.h>
#include <sys/un.h>
#include <unistd.h>

#include "rs_profiler_log.h"
#include "rs_profiler_utils.h"

#define SOCKET_ERROR(message) Error(__func__, message)

namespace OHOS::Rosen {

static void Error(const char* function, const char* message)
{
    if (!function || !message) {
        return;
    }

    const auto code = errno;
    if (code == 0) {
        HRPE("Socket: %{public}s: %s", function, message);
        return;
    }

    constexpr size_t size = 2048u;
    char error[size] = { 0 };
    strerror_r(code, error, size);
    HRPE("Socket: %{public}s: %s: %s", function, message, error);
}

static timeval Timeout(uint32_t milliseconds)
{
    constexpr uint32_t factor = 1000u;
    return {
        .tv_sec = milliseconds / factor,
        .tv_usec = (milliseconds % factor) * factor,
    };
}

template<typename T>
static void SetOption(int32_t socket, int32_t level, int32_t option, const T& value)
{
    setsockopt(socket, level, option, reinterpret_cast<const char*>(&value), sizeof(value));
}

static void SetReuseAddress(int32_t socket, bool enable)
{
    SetOption(socket, SOL_SOCKET, SO_REUSEADDR, static_cast<int32_t>(enable));
}

static void SetSendTimeout(int32_t socket, uint32_t milliseconds)
{
    SetOption(socket, SOL_SOCKET, SO_SNDTIMEO, Timeout(milliseconds));
}

static void SetReceiveTimeout(int32_t socket, uint32_t milliseconds)
{
    SetOption(socket, SOL_SOCKET, SO_RCVTIMEO, Timeout(milliseconds));
}

static void SetNoDelay(int32_t socket, bool enable)
{
    SetOption(socket, IPPROTO_TCP, TCP_NODELAY, static_cast<int32_t>(enable));
}

static int32_t ToggleFlag(uint32_t flags, uint32_t flag, bool enable)
{
    return enable ? static_cast<int32_t>(flags | flag) : static_cast<int32_t>(flags & ~flag);
}

static void SetBlocking(int32_t socket, bool enable)
{
    fcntl(socket, F_SETFL, ToggleFlag(fcntl(socket, F_GETFL, 0), O_NONBLOCK, !enable));
}

static void SetCloseOnExec(int32_t socket, bool enable)
{
    fcntl(socket, F_SETFD, ToggleFlag(fcntl(socket, F_GETFD, 0), FD_CLOEXEC, enable));
}

Socket::~Socket()
{
    Shutdown();
}

bool Socket::Connected() const
{
    return (socket_ != -1) && (client_ != -1) && (state_ == SocketState::CONNECTED);
}

SocketState Socket::GetState() const
{
    return state_;
}

void Socket::Shutdown()
{
    if (socket_ != -1) {
        shutdown(socket_, SHUT_RDWR);
        fdsan_close_with_tag(socket_, LOG_DOMAIN);
        socket_ = -1;
    }

    if (client_ != -1) {
        shutdown(client_, SHUT_RDWR);
        fdsan_close_with_tag(client_, LOG_DOMAIN);
        client_ = -1;
    }

    state_ = SocketState::SHUTDOWN;
}

void Socket::Open(const std::string& name)
{
    sockaddr_un address {};
    address.sun_family = AF_UNIX;
    address.sun_path[0] = 0;
    if (name.empty() ||
        (::memmove_s(address.sun_path + 1, sizeof(address.sun_path) - 1, name.data(), name.size()) != EOK)) {
        SOCKET_ERROR("Cannot init address");
        return;
    }

    socket_ = socket(AF_UNIX, SOCK_STREAM | SOCK_CLOEXEC, 0);
    if (socket_ == -1) {
        SOCKET_ERROR("Cannot create socket");
        Shutdown();
        return;
    }
    fdsan_exchange_owner_tag(socket_, 0, LOG_DOMAIN);

    SetReuseAddress(socket_, true);
    const size_t addressSize = offsetof(sockaddr_un, sun_path) + name.size() + 1;
    if (bind(socket_, reinterpret_cast<sockaddr*>(&address), addressSize) == -1) {
        SOCKET_ERROR("Bind failed");
        Shutdown();
        return;
    }

    const int32_t maxConnections = 1;
    if (listen(socket_, maxConnections) != 0) {
        SOCKET_ERROR("Listen failed");
        Shutdown();
        return;
    }

    SetBlocking(socket_, false);
    SetCloseOnExec(socket_, true);

    state_ = SocketState::CREATE;
}

void Socket::AcceptClient()
{
    client_ = accept4(socket_, nullptr, nullptr, SOCK_CLOEXEC);
    if (client_ == -1) {
        if ((errno != EWOULDBLOCK) && (errno != EAGAIN) && (errno != EINTR)) {
            Shutdown();
        }
        return;
    }
    fdsan_exchange_owner_tag(client_, 0, LOG_DOMAIN);

    SetBlocking(client_, false);
    SetCloseOnExec(client_, true);
    SetNoDelay(client_, true);

    state_ = SocketState::CONNECTED;
}

size_t Socket::Available()
{
    int32_t size = 0;
    const auto result = ioctl(client_, FIONREAD, &size);
    if (result == -1) {
        SOCKET_ERROR("Failed");
        return 0u;
    }
    return static_cast<size_t>(size);
}

bool Socket::Send(const void* data, size_t size)
{
    if (!data || (size == 0)) {
        return true;
    }

    SetBlocking(client_, true);

    constexpr uint32_t timeout = 40;
    SetSendTimeout(client_, timeout);

    const char* bytes = reinterpret_cast<const char*>(data);
    size_t sent = 0;
    while (sent < size) {
        if (PollSend(1) == 0) {
            // wait for 1ms in worst case to have socket ready for sending
            continue;
        }
        const ssize_t sentBytes = send(client_, bytes, size - sent, 0);
        if ((sentBytes == -1) && (errno != EINTR)) {
            SOCKET_ERROR("Invoke shutdown");
            Shutdown();
            return false;
        }
        if (sentBytes > 0) {
            sent += static_cast<size_t>(sentBytes);
            bytes += static_cast<size_t>(sentBytes);
        }
    }

    SetBlocking(client_, false);
    return true;
}

bool Socket::Receive(void* data, size_t size)
{
    if (!data || (size == 0)) {
        return true;
    }

    const uint32_t bandwitdth = 10000; // KB/ms
    const uint32_t timeoutPad = 100;
    const uint32_t timeout = size / bandwitdth + timeoutPad;

    SetBlocking(client_, true);
    SetReceiveTimeout(client_, timeout);

    size_t received = 0;
    char* bytes = static_cast<char*>(data);
    while (received < size) {
        // receivedBytes can only be -1 or 0, or in [1, size - received]
        const ssize_t receivedBytes = recv(client_, bytes, size - received, 0);
        if ((receivedBytes == 0) || ((receivedBytes == -1) && (errno != EINTR))) {
            SOCKET_ERROR("Invoke shutdown");
            Shutdown();
            return false;
        }
        if (receivedBytes > 0) {
            received += static_cast<size_t>(receivedBytes);
            bytes += static_cast<size_t>(receivedBytes);
        }
    }

    SetBlocking(client_, false);
    return true;
}

int Socket::PollReceive(int timeout)
{
    struct pollfd pollFd = { 0 };
    pollFd.fd = client_;
    pollFd.events = POLLIN;
    return poll(&pollFd, 1, timeout);
}

int Socket::PollSend(int timeout)
{
    struct pollfd pollFd = { 0 };
    pollFd.fd = client_;
    pollFd.events = POLLOUT;
    return poll(&pollFd, 1, timeout);
}

} // namespace OHOS::Rosen