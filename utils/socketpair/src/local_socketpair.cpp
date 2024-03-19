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
#include <scoped_bytrace.h>

namespace OHOS {
using namespace OHOS::HiviewDFX;
#undef LOG_DOMAIN
#define LOG_DOMAIN 1400

#undef LOG_TAG
#define LOG_TAG "LocalSocketPair"
#define LOGD(fmt, ...) HILOG_DEBUG(LOG_CORE, fmt, ##__VA_ARGS__)
#define LOGW(fmt, ...) HILOG_WARN(LOG_CORE, fmt, ##__VA_ARGS__)
#define LOGE(fmt, ...) HILOG_ERROR(LOG_CORE, fmt, ##__VA_ARGS__)
namespace {
constexpr int32_t SOCKET_PAIR_SIZE = 2;
constexpr int32_t INVALID_FD = -1;
constexpr int32_t ERRNO_EAGAIN = -1;
constexpr int32_t ERRNO_OTHER = -2;
constexpr int32_t LEAK_FD_CNT = 200;
static int32_t g_fdCnt = 0;
}  // namespace

LocalSocketPair::LocalSocketPair()
    : sendFd_(INVALID_FD), receiveFd_(INVALID_FD)
{
}

LocalSocketPair::~LocalSocketPair()
{
    LOGD("%{public}s close socketpair, sendFd : %{public}d, receiveFd : %{public}d", __func__, sendFd_, receiveFd_);
    if ((sendFd_ != INVALID_FD) || (receiveFd_ != INVALID_FD)) {
        g_fdCnt--;
    }
    CloseFd(sendFd_);
    CloseFd(receiveFd_);
}

int32_t LocalSocketPair::SetSockopt(size_t sendSize, size_t receiveSize, int32_t* socketPair, int32_t socketPairSize)
{
    for (int i = 0; i < socketPairSize; ++i) {
        int32_t ret = setsockopt(socketPair[i], SOL_SOCKET, SO_SNDBUF, &sendSize, sizeof(sendSize));
        if (ret != 0) {
            CloseFd(socketPair[0]);
            CloseFd(socketPair[1]);
            LOGE("%{public}s setsockopt socketpair %{public}d sendbuffer size failed", __func__, i);
            return -1;
        }
        ret = setsockopt(socketPair[i], SOL_SOCKET, SO_RCVBUF, &receiveSize, sizeof(receiveSize));
        if (ret != 0) {
            CloseFd(socketPair[0]);
            CloseFd(socketPair[1]);
            LOGE("%{public}s setsockopt socketpair %{public}d receivebuffer size failed", __func__, i);
            return -1;
        }
        ret = fcntl(socketPair[i], F_SETFL, O_NONBLOCK);
        if (ret != 0) {
            CloseFd(socketPair[0]);
            CloseFd(socketPair[1]);
            LOGE("%{public}s fcntl socketpair %{public}d nonblock failed", __func__, i);
            return -1;
        }
    }
    return 0;
}

int32_t LocalSocketPair::CreateChannel(size_t sendSize, size_t receiveSize)
{
    if ((sendFd_ != INVALID_FD) || (receiveFd_ != INVALID_FD)) {
        LOGD("%{public}s already create socketpair", __func__);
        return 0;
    }

    int32_t socketPair[SOCKET_PAIR_SIZE] = { 0 };
    if (socketpair(AF_UNIX, SOCK_SEQPACKET, 0, socketPair) != 0) {
        ScopedBytrace func("Create socketpair failed, errno = " + std::to_string(errno));
        LOGE("%{public}s create socketpair failed", __func__);
        return -1;
    }
    if (socketPair[0] == 0 || socketPair[1] == 0) {
        int32_t unusedFds[SOCKET_PAIR_SIZE] = {socketPair[0], socketPair[1]};
        int32_t err = socketpair(AF_UNIX, SOCK_SEQPACKET, 0, socketPair);
        CloseFd(unusedFds[0]);
        CloseFd(unusedFds[1]);
        if (err != 0) {
            ScopedBytrace func2("Create socketpair failed for the second time, errno = " + std::to_string(errno));
            LOGE("%{public}s create socketpair failed", __func__);
            return -1;
        }
    }

    // set socket attr
    int32_t ret = SetSockopt(sendSize, receiveSize, socketPair, SOCKET_PAIR_SIZE);
    if (ret != 0) {
        return ret;
    }
    sendFd_ = socketPair[0];
    receiveFd_ = socketPair[1];
    LOGD("%{public}s create socketpair success, receiveFd_ : %{public}d, sendFd_ : %{public}d", __func__, receiveFd_,
        sendFd_);
    g_fdCnt++;
    if (g_fdCnt > LEAK_FD_CNT) {
        LOGW("%{public}s fdCnt: %{public}d", __func__, g_fdCnt);
    }

    return 0;
}

int32_t LocalSocketPair::SendData(const void *vaddr, size_t size)
{
    if (vaddr == nullptr || sendFd_ < 0) {
        LOGE("%{public}s failed, param is invalid", __func__);
        return -1;
    }
    ssize_t length = TEMP_FAILURE_RETRY(send(sendFd_, vaddr, size, MSG_DONTWAIT | MSG_NOSIGNAL));
    if (length < 0) {
        int errnoRecord = errno;
        ScopedBytrace func("SocketPair SendData failed, errno = " + std::to_string(errnoRecord) +
                            ", sendFd_ = " + std::to_string(sendFd_) + ", receiveFd_ = " + std::to_string(receiveFd_) +
                            ", length = " + std::to_string(length));
        LOGD("%{public}s send failed:%{public}d, length = %{public}d", __func__, errnoRecord,
            static_cast<int32_t>(length));
        if (errnoRecord == EAGAIN) {
            return ERRNO_EAGAIN;
        } else {
            return ERRNO_OTHER;
        }
    }
    return length;
}

int32_t LocalSocketPair::ReceiveData(void *vaddr, size_t size)
{
    if (vaddr == nullptr || (receiveFd_ < 0)) {
        LOGE("%{public}s failed, vaddr is null or receiveFd_ invalid", __func__);
        return -1;
    }
    ssize_t length;
    do {
        length = recv(receiveFd_, vaddr, size, MSG_DONTWAIT);
    } while (errno == EINTR);
    if (length < 0) {
        ScopedBytrace func("SocketPair ReceiveData failed errno = " + std::to_string(errno) +
                            ", sendFd_ = " + std::to_string(sendFd_) + ", receiveFd_ = " + std::to_string(receiveFd_) +
                            ", length = " + std::to_string(length));
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

