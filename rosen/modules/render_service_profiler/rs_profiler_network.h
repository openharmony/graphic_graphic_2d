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

#ifndef RS_PROFILER_NETWORK_H
#define RS_PROFILER_NETWORK_H

#include <list>
#include <mutex>
#include <string>
#include <vector>

namespace OHOS::Rosen {

class Socket;

struct NetworkStats {
    std::string interface;
    uint64_t receivedBytes = 0u;
    uint64_t transmittedBytes = 0u;
};

class Network {
public:
    static void Run();
    static void Stop();

    static std::vector<NetworkStats> GetStats(const std::string& interface);
    static void SendRdc(const std::string& path);
    static void SendBinary(const void* data, size_t size);
    static void SendMessage(const std::string& message);

    static void PopCommand(std::string& command, std::vector<std::string>& args);

private:
    static void ReportStats();
    static void PushCommand(const std::vector<std::string>& args);
    static void ProcessCommand(const char* data, size_t size);
    static void ProcessBinary(const char* data, size_t size);
    static void ProcessIncoming(Socket& socket);
    static void ProcessOutgoing(Socket& socket);

public:
    static std::mutex commandMutex_;
    static std::vector<std::string> commandData_;

private:
    static bool isRunning_;

    static std::mutex outgoingMutex_;
    static std::list<std::vector<char>> outgoing_;
};

} // namespace OHOS::Rosen

#endif // RS_PROFILER_NETWORK_H