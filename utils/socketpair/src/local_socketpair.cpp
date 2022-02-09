/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
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

#include "local_socketpair.h"
#include "hilog/log.h"

#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <fcntl.h>
#include <cerrno>

namespace OHOS {
using namespace OHOS::HiviewDFX;

namespace {
constexpr HiLogLabel LABEL = { LOG_CORE, 0xD001400, "LocalSocketPair" };
constexpr int32_t DEFAULT_CHANNEL_SIZE = 2 * 1024;
constexpr int32_t SOCKET_PAIR_SIZE = 2;
constexpr int32_t INVALID_FD = -1;
}  // namespace

LocalSocketPair::LocalSocketPair()
    : sendFd_(INVALID_FD), receiveFd_(INVALID_FD)
{
    HiLog::Debug(LABEL, "%{public}s sendFd: %{public}d", __func__, sendFd_);
}

LocalSocketPair::~LocalSocketPair()
{
    CloseFd(sendFd_);
    CloseFd(receiveFd_);
}

int32_t LocalSocketPair::CreateChannel(size_t sendSize, size_t receiveSize)
{
    if ((sendFd_ != INVALID_FD) || (receiveFd_ != INVALID_FD)) {
        HiLog::Debug(LABEL, "%{public}s already create socketpair", __func__);
        return 0;
    }

    int32_t socketPair[SOCKET_PAIR_SIZE] = { 0 };
    if (socketpair(AF_UNIX, SOCK_SEQPACKET, 0, socketPair) != 0) {
        HiLog::Error(LABEL, "%{public}s create socketpair failed", __func__);
        sendFd_ = INVALID_FD;
        receiveFd_ = INVALID_FD;
        return -1;
    }
    // set socket attr
    setsockopt(socketPair[0], SOL_SOCKET, SO_SNDBUF, &sendSize, sizeof(sendSize));
    setsockopt(socketPair[1], SOL_SOCKET, SO_RCVBUF, &receiveSize, sizeof(receiveSize));
    int32_t bufferSize = DEFAULT_CHANNEL_SIZE;
    int32_t ret = setsockopt(socketPair[0], SOL_SOCKET, SO_RCVBUF, &bufferSize, sizeof(bufferSize));
    if (ret != 0) {
        HiLog::Error(LABEL, "%{public}s setsockopt socketpair 0 failed", __func__);
        return -1;
    }
    ret = setsockopt(socketPair[1], SOL_SOCKET, SO_SNDBUF, &bufferSize, sizeof(bufferSize));
    if (ret != 0) {
        HiLog::Error(LABEL, "%{public}s setsockopt socketpair 1 failed", __func__);
        return -1;
    }
    ret = fcntl(socketPair[0], F_SETFL, O_NONBLOCK);
    if (ret != 0) {
        HiLog::Error(LABEL, "%{public}s fcntl socketpair 0 failed", __func__);
        return -1;
    }
    ret = fcntl(socketPair[1], F_SETFL, O_NONBLOCK);
    if (ret != 0) {
        HiLog::Error(LABEL, "%{public}s fcntl socketpair 1 failed", __func__);
        return -1;
    }
    sendFd_ = socketPair[0];
    receiveFd_ = socketPair[1];
    HiLog::Debug(LABEL, "%{public}s create socketpair success, receiveFd_ : %{public}d, sendFd_ : %{public}d", __func__,
                 receiveFd_, sendFd_);

    return 0;
}

int32_t LocalSocketPair::SendData(const void *vaddr, size_t size)
{
    if (vaddr == nullptr || sendFd_ < 0) {
        HiLog::Error(LABEL, "%{public}s failed, param is invalid", __func__);
        return -1;
    }
    ssize_t length;
    do {
        length = send(sendFd_, vaddr, size, MSG_DONTWAIT | MSG_NOSIGNAL);
    } while (errno == EINTR);
    if (length < 0) {
        HiLog::Error(LABEL, "%{public}s send fail : %{public}d, length = %{public}d", __func__, errno, (int32_t)length);
        return -1;
    }
    return length;
}

int32_t LocalSocketPair::ReceiveData(void *vaddr, size_t size)
{
    if (vaddr == nullptr || (receiveFd_ < 0)) {
        HiLog::Error(LABEL, "%{public}s failed, vaddr is null or receiveFd_ invalid", __func__);
        return -1;
    }
    ssize_t length;
    do {
        length = recv(receiveFd_, vaddr, size, MSG_DONTWAIT);
    } while (errno == EINTR);
    if (length < 0) {
        return -1;
    }
    return length;
}

// internal interface
int32_t LocalSocketPair::SendFdToBinder(MessageParcel &data, int32_t &fd)
{
    if (fd < 0) {
        return -1;
    }
    // need dup???
    bool result = data.WriteFileDescriptor(fd);
    if (!result) {
        return -1;
    }
    return 0;
}

int32_t LocalSocketPair::SendToBinder(MessageParcel &data)
{
    return SendFdToBinder(data, sendFd_);
}

int32_t LocalSocketPair::ReceiveToBinder(MessageParcel &data)
{
    return SendFdToBinder(data, receiveFd_);
}

// internal interface
void LocalSocketPair::CloseFd(int32_t &fd)
{
    if (fd != INVALID_FD) {
        close(fd);
        fd = INVALID_FD;
    }
}
}

