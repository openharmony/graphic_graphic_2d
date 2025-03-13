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

#include <chrono>
#include <queue>
#include <mutex>
#include <string>
#include <vector>
#include <unordered_set>

namespace OHOS::Rosen {

class Packet;
class Socket;
class RSCaptureData;
enum class PackageID;

class Network {
public:
    static void Run();
    static void Stop();
    static void ForceShutdown();
    static bool IsRunning();

    static void SendRdcPath(const std::string& path);
    static void SendDclPath(const std::string& path);
    static void SendMskpPath(const std::string& path);
    static void SendBetaRecordPath(const std::string& path);

    static void SendSkp(const void* data, size_t size);
    static void SendCaptureData(const RSCaptureData& data);
    static void SendRSTreeDumpJSON(const std::string& jsonstr);
    static void SendRSTreePerfNodeList(const std::unordered_set<uint64_t>& perfNodesList);
    static void SendRSTreeSingleNodePerf(uint64_t id, uint64_t nanosec);

    static void SendBinary(const void* data, size_t size);
    static void SendBinary(const std::vector<char>& data);
    static void SendBinary(const std::string& data);
    static void SendMessage(const std::string& message);

    static bool PopCommand(std::vector<std::string>& args);

    static void SetBlockBinary(bool blockFlag);

private:
    static void PushCommand(const std::vector<std::string>& args);
    static void ResetCommandQueue();
    static void ProcessCommand(const char* data, size_t size);
    static void ProcessBinary(const std::vector<char>& data);
    static void Receive(Socket& socket);
    static void Send(Socket& socket);
    static void SendPath(const std::string& path, PackageID id);
    static void SendPacket(Packet& packet);
    static void ResetSendQueue();
    static void Shutdown(Socket*& socket);
    static void Ping(const Socket& socket);
    static void ResetPing();

private:
    static std::atomic<bool> isRunning_;
    static std::atomic<bool> forceShutdown_;
    static std::atomic<bool> blockBinary_;
    static std::chrono::steady_clock::time_point ping_;

    static std::mutex incomingMutex_;
    static std::queue<std::vector<std::string>> incoming_;

    static std::mutex outgoingMutex_;
    static std::queue<std::vector<char>> outgoing_;
};

} // namespace OHOS::Rosen

#endif // RS_PROFILER_NETWORK_H