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
    shutdown(clientSocket_, SHUT_RDWR);
    close(clientSocket_);
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
        usleep(1000000);
        return;
    }

    const int32_t maxConnections = 5;
    if (listen(socket_, maxConnections) != 0) {
        Shutdown();
        return;
    }

    int flags = fcntl(socket_, F_GETFL, 0);
    fcntl(socket_, F_SETFL, flags | O_NONBLOCK);
    flags = fcntl(socket_, F_GETFD, 0);
    fcntl(socket_, F_SETFD, flags | FD_CLOEXEC);
    state_ = SocketState::CREATE_SOCKET;
}

void Socket::AcceptClient()
{
    clientSocket_ = accept4(socket_, NULL, NULL, SOCK_CLOEXEC);

    if (clientSocket_ != -1) {
        int flags = fcntl(clientSocket_, F_GETFL, 0);
        fcntl(clientSocket_, F_SETFL, flags | O_NONBLOCK);
        flags = fcntl(clientSocket_, F_GETFD, 0);
        fcntl(clientSocket_, F_SETFD, flags | FD_CLOEXEC);
        int nodelay = 1;
        setsockopt(clientSocket_, IPPROTO_TCP, TCP_NODELAY, reinterpret_cast<char*>(&nodelay), sizeof(nodelay));
        state_ = SocketState::ACCEPT_STATE;
        return;
    }

    const int err = errno;
    if (err != EWOULDBLOCK && err != EAGAIN && err != EINTR) {
        Shutdown();
    }
}

void Socket::SendWhenReady(const void* data, uint32_t size)
{
    if (size == 0) {
        return;
    }

    int flags = fcntl(clientSocket_, F_GETFL, 0);
    fcntl(clientSocket_, F_SETFL, flags & ~O_NONBLOCK);
    timeval oldtimeout = { 0 };
    socklen_t len = sizeof(oldtimeout);
    getsockopt(clientSocket_, SOL_SOCKET, SO_SNDTIMEO, reinterpret_cast<char*>(&oldtimeout), &len);

    const int timeoutMS = 40;
    timeval timeout = { 0 };
    timeout.tv_sec = (timeoutMS / 1000);
    timeout.tv_usec = (timeoutMS % 1000) * 1000;
    setsockopt(clientSocket_, SOL_SOCKET, SO_SNDTIMEO, reinterpret_cast<const char*>(&timeout), sizeof(timeout));

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

    flags = fcntl(clientSocket_, F_GETFL, 0);
    fcntl(clientSocket_, F_SETFL, flags | O_NONBLOCK);

    setsockopt(clientSocket_, SOL_SOCKET, SO_SNDTIMEO, reinterpret_cast<const char*>(&oldtimeout), sizeof(oldtimeout));
}

bool Socket::Receive(void* data, uint32_t& size)
{
    if (size == 0) {
        return true;
    }

    if (fcntl(clientSocket_, F_GETFL) & O_NONBLOCK) {
        fcntl(clientSocket_, F_SETFL, fcntl(clientSocket_, F_GETFL) | O_NONBLOCK);
    }

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

bool Socket::ReceiveWhenReady(void* data, uint32_t size)
{
    if (size == 0) {
        return true;
    }

    uint32_t received = 0;
    char* dst = static_cast<char*>(data);
    int flags = fcntl(clientSocket_, F_GETFL, 0);
    fcntl(clientSocket_, F_SETFL, flags & ~O_NONBLOCK);
    timeval oldtimeout = { 0 };
    socklen_t len = sizeof(oldtimeout);
    getsockopt(clientSocket_, SOL_SOCKET, SO_RCVTIMEO, reinterpret_cast<char*>(&oldtimeout), &len);
    timeval timeout = { 0 };
    const int timeoutMS = size / 10000 /*  kb/ms */ + 100;
    timeout.tv_sec = (timeoutMS / 1000);
    timeout.tv_usec = (timeoutMS % 1000) * 1000;
    setsockopt(clientSocket_, SOL_SOCKET, SO_RCVTIMEO, reinterpret_cast<const char*>(&timeout), sizeof(timeout));

    while (received < size) {
        const ssize_t ret = recv(clientSocket_, dst, size - received, 0);
        if (ret == 0) {
            Shutdown();
            return false;
        }
        if (ret <= 0) {
            const int err = errno;
            if (err == EINTR) {
                continue;
            }
            if (err == EWOULDBLOCK || err == EAGAIN) {
                Shutdown();
                return false;
            } else {
                Shutdown();
                return false;
            }
        }

        received += ret;
        dst += ret;
    }

    flags = fcntl(clientSocket_, F_GETFL, 0);
    fcntl(clientSocket_, F_SETFL, flags | O_NONBLOCK);
    setsockopt(clientSocket_, SOL_SOCKET, SO_RCVTIMEO, reinterpret_cast<const char*>(&oldtimeout), sizeof(oldtimeout));
    return true;
}

int Socket::SocketSelect()
{
    int status = 0;

    if (clientSocket_ == -1 || socket_ == -1) {
        return status;
    }

    fd_set write;
    FD_ZERO(&write);
    FD_SET(clientSocket_, &write);

    fd_set read;
    FD_ZERO(&read);
    FD_SET(clientSocket_, &read);

    fd_set except;
    FD_ZERO(&except);
    FD_SET(clientSocket_, &except);

    const int32_t timeoutMS = 10;
    timeval timeout {};
    timeout.tv_sec = (timeoutMS / 1000);
    timeout.tv_usec = (timeoutMS % 1000) * 1000;

    if (select(clientSocket_ + 1, &read, &write, &except, &timeout) == 0) {
        status |= static_cast<int>(SocketState::TIMEOUT);
    }

    if (FD_ISSET(clientSocket_, &read)) {
        status |= static_cast<int>(SocketState::READ_ENABLE);
    }

    if (FD_ISSET(clientSocket_, &write)) {
        status |= static_cast<int>(SocketState::WRITE_ENABLE);
    }

    if (FD_ISSET(clientSocket_, &except)) {
        status |= static_cast<int>(SocketState::EXCEPT_ENABLE);
    }

    return status;
}

bool Socket::ReadEnable(int status)
{
    return ((status & static_cast<int>(SocketState::READ_ENABLE)) == static_cast<int>(SocketState::READ_ENABLE));
}

bool Socket::WriteEnable(int status)
{
    return ((status & static_cast<int>(SocketState::WRITE_ENABLE)) == static_cast<int>(SocketState::WRITE_ENABLE));
}

} // namespace OHOS::Rosen