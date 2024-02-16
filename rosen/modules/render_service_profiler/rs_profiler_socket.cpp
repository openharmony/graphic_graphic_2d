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
#include <securec.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>

#include "rs_profiler_utils.h"

namespace OHOS::Rosen {

static timeval GetTimeoutDesc(uint32_t milliseconds)
{
    const uint32_t millisecondsInSecond = 1000u;

    timeval timeout = {};
    timeout.tv_sec = milliseconds / millisecondsInSecond;
    timeout.tv_usec = (milliseconds % millisecondsInSecond) * millisecondsInSecond;
    return timeout;
}

static timeval GetTimeout(int32_t socket)
{
    timeval timeout = {};
    socklen_t size = sizeof(timeout);
    getsockopt(socket, SOL_SOCKET, SO_SNDTIMEO, reinterpret_cast<char*>(&timeout), &size);
    return timeout;
}

static void SetTimeout(int32_t socket, const timeval& timeout)
{
    setsockopt(socket, SOL_SOCKET, SO_SNDTIMEO, reinterpret_cast<const char*>(&timeout), sizeof(timeout));
}

static void SetTimeout(int32_t socket, uint32_t milliseconds)
{
    SetTimeout(socket, GetTimeoutDesc(milliseconds));
}

static int32_t ToggleFlag(uint32_t flags, uint32_t flag, bool enable)
{
    return enable ? (flags | flag) : (flags & ~flag);
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

SocketState Socket::GetState() const
{
    return state_;
}

void Socket::SetState(SocketState state)
{
    state_ = state;
}

void Socket::Shutdown()
{
    shutdown(socket_, SHUT_RDWR);
    close(socket_);
    socket_ = -1;

    shutdown(clientSocket_, SHUT_RDWR);
    close(clientSocket_);
    clientSocket_ = -1;

    state_ = SocketState::SHUTDOWN;
}

void Socket::Open(uint16_t port)
{
    socket_ = socket(AF_UNIX, SOCK_STREAM | SOCK_CLOEXEC, 0);
    if (socket_ == -1) {
        Shutdown();
        return;
    }

    const std::string socketName = "render_service_" + std::to_string(port);
    sockaddr_un address {};
    address.sun_family = AF_UNIX;
    address.sun_path[0] = 0;
    ::memmove_s(address.sun_path + 1, sizeof(address.sun_path) - 1, socketName.data(), socketName.size());
    
    const size_t addressSize = offsetof(sockaddr_un, sun_path) + socketName.size() + 1;

    if (bind(socket_, reinterpret_cast<sockaddr*>(&address), addressSize) == -1) {
        Shutdown();
        return;
    }

    const int32_t maxConnections = 5;
    if (listen(socket_, maxConnections) != 0) {
        Shutdown();
        return;
    }

    SetBlocking(socket_, false);
    SetCloseOnExec(socket_, true);

    state_ = SocketState::CREATE_SOCKET;
}

void Socket::AcceptClient()
{
    clientSocket_ = accept4(socket_, nullptr, nullptr, SOCK_CLOEXEC);

    if (clientSocket_ == -1) {
        const int err = errno;
        if ((err != EWOULDBLOCK) && (err != EAGAIN) && (err != EINTR)) {
            Shutdown();
        }

    } else {
        SetBlocking(clientSocket_, false);
        SetCloseOnExec(clientSocket_, true);

        int nodelay = 1;
        setsockopt(clientSocket_, IPPROTO_TCP, TCP_NODELAY, reinterpret_cast<char*>(&nodelay), sizeof(nodelay));

        state_ = SocketState::ACCEPT_STATE;
    }
}

void Socket::SendWhenReady(const void* data, size_t size)
{
    if (size == 0) {
        return;
    }

    SetBlocking(clientSocket_, true);

    const timeval previousTimeout = GetTimeout(clientSocket_);

    const uint32_t timeoutMilliseconds = 40;
    SetTimeout(clientSocket_, timeoutMilliseconds);

    const char* bytes = reinterpret_cast<const char*>(data);
    uint32_t sent = 0;
    while (sent < size) {
        const ssize_t sentBytes = send(clientSocket_, bytes, size - sent, 0);
        if ((sentBytes <= 0) && (errno != EINTR)) {
            Shutdown();
            return;
        }
        sent += sentBytes;
        bytes += sentBytes;
    }

    SetTimeout(clientSocket_, previousTimeout);
    SetBlocking(clientSocket_, false);
}

bool Socket::Receive(void* data, size_t& size)
{
    if (!data || (size == 0)) {
        return true;
    }

    SetBlocking(clientSocket_, false);

    const ssize_t receivedBytes = recv(clientSocket_, static_cast<char*>(data), size, 0);
    if (receivedBytes > 0) {
        size = static_cast<uint32_t>(receivedBytes);
    } else {
        size = 0;
        const int err = errno;
        if (err == EWOULDBLOCK || err == EAGAIN || err == EINTR) {
            return true;
        }
        Shutdown();
        return false;
    }
    return true;
}

bool Socket::ReceiveWhenReady(void* data, size_t size)
{
    if (size == 0) {
        return true;
    }

    const timeval previousTimeout = GetTimeout(clientSocket_);
    const uint32_t bandwitdth = 10000; // KB/ms
    const uint32_t timeoutPad = 100;
    const uint32_t timeout = size / bandwitdth + timeoutPad;

    SetBlocking(clientSocket_, true);
    SetTimeout(clientSocket_, timeout);

    uint32_t received = 0;
    char* bytes = static_cast<char*>(data);
    while (received < size) {
        const ssize_t receivedBytes = recv(clientSocket_, bytes, size - received, 0);
        if ((receivedBytes == -1) && (errno != EINTR)) {
            Shutdown();
            return false;
        }

        received += receivedBytes;
        bytes += receivedBytes;
    }

    SetTimeout(clientSocket_, previousTimeout);
    SetBlocking(clientSocket_, false);
    return true;
}

int Socket::Select()
{
    int32_t status = 0;

    if (clientSocket_ == -1) {
        return status;
    }

    fd_set write;
    FD_ZERO(&write);
    FD_SET(clientSocket_, &write);

    fd_set read;
    FD_ZERO(&read);
    FD_SET(clientSocket_, &read);

    const uint32_t timeoutMilliseconds = 10;
    timeval timeout = GetTimeoutDesc(timeoutMilliseconds);

    if (select(clientSocket_ + 1, &read, &write, nullptr, &timeout) == 0) {
        status |= static_cast<int>(SocketState::TIMEOUT);
    }

    if (FD_ISSET(clientSocket_, &read)) {
        status |= static_cast<int>(SocketState::READ_ENABLE);
    }

    if (FD_ISSET(clientSocket_, &write)) {
        status |= static_cast<int>(SocketState::WRITE_ENABLE);
    }

    return status;
}

bool Socket::IsReceiveEnabled(int status)
{
    return ((status & static_cast<int>(SocketState::READ_ENABLE)) == static_cast<int>(SocketState::READ_ENABLE));
}

bool Socket::IsSendEnabled(int status)
{
    return ((status & static_cast<int>(SocketState::WRITE_ENABLE)) == static_cast<int>(SocketState::WRITE_ENABLE));
}

} // namespace OHOS::Rosen