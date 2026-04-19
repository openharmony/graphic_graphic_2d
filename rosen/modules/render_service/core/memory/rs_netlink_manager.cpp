/*
 * Copyright (c) 2026 Huawei Device Co., Ltd.
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

#include "memory/rs_netlink_manager.h"

#include <cerrno>
#include <fcntl.h>
#include <iostream>
#include <sys/socket.h>
#include <linux/netlink.h>
#include <unistd.h>
#include <securec.h>

#include "rs_trace.h"
#include "platform/common/rs_log.h"
#include "memory/rs_memory_manager.h"
#include "feature_cfg/feature_param/extend_feature/mem_param.h"

namespace OHOS::Rosen {
constexpr uint32_t UEVENT_MSG_LEN = 1024;
const std::string UEVENT_MSG_TAG = "ACTION=MEMORY_OVER_LIMIT";

void RSNetlinkListener::RunListener()
{
    char buf[UEVENT_MSG_LEN];
    while (1) {
        long msgSize = recv(socketFd_, buf, sizeof(buf), 0);
        if (msgSize < 0 && errno == EBADF) {
            RS_LOGE("RSNetlinkListener::RunListener sockFd_ is closed!");
            break;
        }
        if (msgSize > 0) {
            ParseMsgAndEvent(buf, msgSize);
        }
    }
}

bool RSNetlinkListener::ParseMsgAndEvent(char *buf, long msgSize)
{
    for (int i = 0; i < msgSize; i++) {
        if (buf[i] == '\0') {
            buf[i] = '-';
        }
    }
    buf[msgSize - 1] = '\0';
    std::string recvInfo(buf);
    if (recvInfo.find(UEVENT_MSG_TAG) != std::string::npos &&
        recvInfo.find(std::to_string(getpid())) != std::string::npos) {
        MemoryManager::GpuReportFromKernel(recvInfo);
        return true;
    }
    return false;
}

bool RSNetlinkListener::StartListener()
{
    if (socketFd_ < 0) {
        RS_LOGE("RSNetlinkListener::StartListener socketFd < 0");
        return false;
    }
    socketThread_ = std::make_unique<std::thread>([this]() {
        pthread_setname_np(pthread_self(), "RSNetlinkThread");
        this->RunListener();
    });
    if (socketThread_ == nullptr) {
        RS_LOGE("RSNetlinkListener::StartListener start thread fail");
        return false;
    }
    return true;
}

bool RSNetlinkListener::StopListener()
{
    if (socketThread_ != nullptr && socketThread_->joinable()) {
        socketThread_->join();
        return true;
    }
    return false;
}

RSNetlinkListener::RSNetlinkListener(int32_t socket)
{
    socketFd_ = socket;
}

RSNetlinkManager &RSNetlinkManager::Instance()
{
    static RSNetlinkManager instance_;
    return instance_;
}

bool RSNetlinkManager::Start()
{
    if (!MEMParam::IsKernelReportEnabled()) {
        RS_LOGE("RSNetlinkManager::Start kernel report enable is false");
        return false;
    }
    struct sockaddr_nl addr;
    memset_s(&addr, sizeof(addr), 0, sizeof(addr));
    addr.nl_family = AF_NETLINK;
    addr.nl_pid = static_cast<uint32_t>(getpid());
    addr.nl_groups = 0xffffffff;
    socketFd_ = socket(PF_NETLINK, SOCK_DGRAM | SOCK_CLOEXEC, NETLINK_KOBJECT_UEVENT);
    if (socketFd_ < 0) {
        RS_LOGE("RSNetlinkManager::Start create netlink socket failed, errno %{public}d", errno);
        return false;
    }
    if (bind(socketFd_, reinterpret_cast<struct sockaddr *>(&addr), sizeof(addr)) != 0) {
        RS_LOGE("RSNetlinkManager::Start socket bind failed, errno %{public}d", errno);
        close(socketFd_);
        return false;
    }

    rsNetlinkListener_ = new RSNetlinkListener(socketFd_);
    if (!rsNetlinkListener_->StartListener()) {
        close(socketFd_);
        delete rsNetlinkListener_;
        return false;
    }
    return true;
}

void RSNetlinkManager::Stop()
{
    close(socketFd_);
    if (rsNetlinkListener_ != nullptr && rsNetlinkListener_->StopListener()) {
        delete rsNetlinkListener_;
    }
    rsNetlinkListener_ = nullptr;
    socketFd_ = -1;
}
}