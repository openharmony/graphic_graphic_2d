/*
 * Copyright (c) 2023 Huawei Device Co., Ltd.
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

#include <iostream>
#include <fstream>
#include <unistd.h>
#include <cstdio>
#include <iostream>
#include <thread>
#include <mutex>
#include <queue>
#include <condition_variable>
#include <cstring>
#include <sys/socket.h>
#include <sys/un.h>
#include <securec.h>

#include "rs_graphic_test_profiler_thread.h"
#include "transaction/rs_interfaces.h"
#include "transaction/rs_transaction.h"
#include "ui/rs_display_node.h"
#include "ui/rs_root_node.h"
#include "ui/rs_surface_node.h"
#include "ui/rs_ui_context.h"
#include "ui/rs_ui_director.h"
#include "rs_graphic_test_utils.h"
#include "rs_profiler_packet.h"

using namespace OHOS;
using namespace OHOS::Rosen;
using namespace std;

constexpr uint32_t MAX_WAITING_TIMES = 1000;
constexpr int64_t INIT_WAIT_TIME = 50;
constexpr int64_t SOCKET_REFRESH_TIME = 20;
constexpr int SOCKET_CONNECT_MAX_NUM = 10000;

class RenderServiceClientRenderProfilerDemo {
public:
    ~RenderServiceClientRenderProfilerDemo()
    {
        Stop();
    }

    void Start()
    {
        system("param set persist.graphic.profiler.enabled 1");
        auto time = std::chrono::milliseconds(INIT_WAIT_TIME);
        std::this_thread::sleep_for(time);
        runnig_ = true;
        thread_ = std::thread(&RenderServiceClientRenderProfilerDemo::MainLoop, this);
    }

    void Stop()
    {
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
        system("param set persist.graphic.profiler.enabled 0");
        while (!messageQueue_.empty()) {
            messageQueue_.pop();
        }
    }

    void SendCommand(const std::string command)
    {
        std::unique_lock lock(queueMutex_);
        messageQueue_.push(command);
    }

    bool IsSocketActive() const
    {
        return socket_ != -1;
    }

    void ForceCloseSocket()
    {
        if (socket_ != -1) {
            close(socket_);
            socket_ = -1;
        }
    }
private:
    void MainLoop()
    {
        ForceCloseSocket();
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
                std::unique_lock lock(queueMutex_);
                cv_.wait_for(lock, std::chrono::milliseconds(SOCKET_REFRESH_TIME), [this] { return !runnig_; });
            }
            SendMessage();
            RecieveMessage();
        }
        close(socket_);
        socket_ = -1
    }

    void SendMessage()
    {
        std::string message;
        {
            std::lock_guard<std::mutex> lock(queueMutex_);
            if (!messageQueue_.empty()) {
                message = messageQueue_.front();
                messageQueue_.pop();
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

    void RecieveMessage()
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

    bool RecieveHeader(void* data, size_t& size)
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
private:
    int32_t socket_ = -1;
    std::thread thread_;
    std::queue<std::string> messageQueue_;
    bool runnig_ = false;
    std::mutex queueMutex_;
    std::condition_variable cv_;
};

void RecordStart(std::shared_ptr<RenderServiceClientRenderProfilerDemo> profilerThread)
{
    // 命令示例 ./demo record_start
    if (access("/data/service/el0/render_service/stop_recording", F_OK) == 0) {
        remove("/data/service/el0/render_service/stop_recording");
    }
    profilerThread->Start();
    sleep(1);
    std::string cmd = "rsrecord_start -f /data/service/el0/render_service/file.ohr";
    profilerThread->SendCommand(cmd);
    std::cout << "Record started!" << std::endl;
    for (int i = 0; i < MAX_WAITING_TIMES; ++i) {
        sleep(1);
        if (access("/data/service/el0/render_service/stop_recording", F_OK) == 0) {
            remove("/data/service/el0/render_service/stop_recording"); // 清理标志文件
            profilerThread->SendCommand("rsrecord_stop");
            std::cout << "Record stopped!" << std::endl;
            break;
        }
    }
    sleep(1);
    profilerThread->Stop();
}

void RecordStop()
{
    // 命令示例 ./demo record_stop
    std::ofstream stopFlag("/data/service/el0/render_service/stop_recording");
    stop_flag.close();
    std::cout << "Stop recording flag created." << std::endl;
}

void ReplayStart(std::shared_ptr<RenderServiceClientRenderProfilerDemo> profilerThread,
    std::string ohrFile)
{
    // 命令示例 ./demo replay_start -f filePath
    system("param set persist.graphic.profiler.enabled 1");
    // 检查socket是否彻底关闭
    if (profilerThread->IsSocketActive()) {
        profilerThread->ForceCloseSocket();
    }
    if (access("/data/service/el0/render_service/stop_replaying", F_OK) == 0) {
        remove("/data/service/el0/render_service/stop_replaying");
    }
    profilerThread->Start();
    sleep(1);
    profilerThread->SendCommand("rsrecord_replay_prepare a -f " + ohrFile);
    std::cout << "rsrecord_replay_prepare in progress" << std::endl;
    sleep(1);

    profilerThread->SendCommand("rsrecord_replay");
    std::cout << "Replay started!" << std::endl;
    for (int i = 0; i < MAX_WAITING_TIMES; ++i) {
        sleep(1);
        // 检查停止录制标志文件是否存在
        if (access("/data/service/el0/render_service/stop_replaying", F_OK) == 0) {
            remove("/data/service/el0/render_service/stop_replaying"); // 清理标志文件
            profilerThread->SendCommand("rsrecord_replay_stop");
            std::cout << "Replay stopped!" << std::endl;
            break;
        }
    }
    sleep(1);
    profilerThread->Stop();
}

void ReplayStop()
{
    // 命令示例 ./demo replay_stop
    std::ofstream stopFlag("/data/service/el0/render_service/stop_replaying");
    stop_flag.close();
    std::cout << "Stop replaying flag created." << std::endl;
}


int main(int argc, char * argv[])
{
    const int minArgNum = 2;
    if (argc < minArgNum) {
        std::cout << "缺少命令参数, 请指定操作类型:\n"
            "1. record_start 开始录制\n"
            "2. record_stop 结束录制\n"
            "3. replay_start -f filePath 开始回放\n"
            "4. replay_stop 结束回放" << std::endl;
    }

    std::cout << "render service client render profiler demo start!" << std::endl;
    std::string cmdType = argv[1];
    std::string cmd = "";
    std::string message = "";

    if (cmdType == "record_start") {
        std::shared_ptr<RenderServiceClientRenderProfilerDemo> profilerThread =
            std::make_shared<RenderServiceClientRenderProfilerDemo>();
        RecordStart(profilerThread);
    } else if (cmdType == "record_stop") {
        RecordStop();
    } else if (cmdType == "replay_start") {
        std::shared_ptr<RenderServiceClientRenderProfilerDemo> profilerThread =
            std::make_shared<RenderServiceClientRenderProfilerDemo>();
        std::string ohrFile = argv[3];
        ReplayStart(profilerThread, ohrFile);
    } else if (cmdType == "replay_stop") {
        ReplayStop();
    }
    return 0;
}
