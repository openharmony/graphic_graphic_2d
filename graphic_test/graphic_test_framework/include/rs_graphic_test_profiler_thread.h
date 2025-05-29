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

#ifndef RS_GRAPHIC_TEST_PROFILER_THREAD_H
#define RS_GRAPHIC_TEST_PROFILER_THREAD_H

#include <iostream>
#include <thread>
#include <mutex>
#include <queue>
#include <condition_variable>
#include <cstring>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>

namespace OHOS {
namespace Rosen {
class RSGraphicTestProfilerThread {
public:
#ifdef RS_PROFILER_ENABLED
    ~RSGraphicTestProfilerThread();

    void Start();
    void Stop();
    void SendCommand(const std::string command, int outTime);
private:
    void MainLoop();
    void SendMessage();
    void RecieveMessage();
    bool RecieveHeader(void* data, size_t& size);
    bool IsReceiveWaitMessage(const std::string& message);
private:
    int32_t socket_ = -1;
    std::thread thread_;
    std::queue<std::string> message_queue_;
    bool runnig_ = false;
    bool waitReceive_ = false;
    std::mutex queue_mutex_;
    std::mutex wait_mutex_;
    std::condition_variable cv_;
#else
    void Start() {}
    void Stop() {}
    void SendCommand(const std::string command, int outTime) {}
#endif
};

} // namespace Rosen
} // namespace OHOS
#endif