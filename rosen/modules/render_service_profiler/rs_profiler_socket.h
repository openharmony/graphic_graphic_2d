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

#ifndef RS_PROFILER_SOCKET_H
#define RS_PROFILER_SOCKET_H

#include <cstddef>
#include <cstdint>

namespace OHOS::Rosen {

enum class SocketState {
    INITIAL,
    CREATE,
    ACCEPT,
    SHUTDOWN
};

class Socket final {
public:
    Socket() = default;
    Socket(const Socket& other) = delete;
    Socket(Socket&& other) = delete;
    Socket& operator=(const Socket& other) = delete;
    Socket& operator=(Socket&& other) = delete;
    ~Socket();

    SocketState GetState() const;
    void SetState(SocketState state);
    void Shutdown();
    void Open(uint16_t port);
    void AcceptClient();

    void GetStatus(bool& readyToReceive, bool& readyToSend) const;

    void SendWhenReady(const void* data, size_t size);
    bool Receive(void* data, size_t& size);
    bool ReceiveWhenReady(void* data, size_t size);

private:
    int32_t socket_ = -1;
    int32_t client_ = -1;
    SocketState state_ = SocketState::INITIAL;
};

} // namespace OHOS::Rosen

#endif // RS_PROFILER_SOCKET_H