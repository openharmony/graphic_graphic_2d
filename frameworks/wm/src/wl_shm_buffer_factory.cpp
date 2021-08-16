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

#include "wl_shm_buffer_factory.h"

#include <map>

#include <fcntl.h>
#include <securec.h>
#include <sys/mman.h>
#include <unistd.h>

#include "window_manager_hilog.h"
#include "wl_display.h"
#include "wl_shm_buffer.h"

namespace OHOS {
namespace {
constexpr HiviewDFX::HiLogLabel LABEL = { LOG_CORE, 0, "WMWlSHMBufferFactory" };
constexpr int32_t STRIDE_NUM = 4;
}

sptr<WlSHMBufferFactory> WlSHMBufferFactory::GetInstance()
{
    if (instance == nullptr) {
        static std::mutex mutex;
        std::lock_guard<std::mutex> lock(mutex);
        if (instance == nullptr) {
            instance = new WlSHMBufferFactory();
        }
    }
    return instance;
}

void WlSHMBufferFactory::Init()
{
    delegator.Dep<WaylandService>()->OnAppear(&WlSHMBufferFactory::OnAppear);
}

void WlSHMBufferFactory::Deinit()
{
    if (shmbuf != nullptr) {
        wl_shm_destroy(shmbuf);
        shmbuf = nullptr;
    }
}

void WlSHMBufferFactory::OnAppear(const GetServiceFunc get, const std::string &iname, uint32_t ver)
{
    if (iname == "wl_shm") {
        constexpr uint32_t wlShmVersion = 1;
        auto ret = get(&wl_shm_interface, wlShmVersion);
        shmbuf = static_cast<struct wl_shm *>(ret);
    }
}

namespace {
int32_t CreateShmFile(int32_t size)
{
    static const char tempPath[] = "/weston-shared-XXXXXX";
    static const char path[] = "/data/weston";
    size_t len = sizeof(path) + sizeof(tempPath) + 1;
    std::unique_ptr<char[]> name = std::make_unique<char[]>(len);
    auto ret = strcpy_s(name.get(), len, path);
    if (ret) {
        WMLOGFE("strcpy_s: %{public}s, name: %{public}s, len: %{public}zu, path: %{public}s",
            strerror(ret), name.get() ? "Yes" : "No", len, path);
        return -1;
    }

    ret = strcat_s(name.get(), len, tempPath);
    if (ret) {
        WMLOGFE("strcpy_s: %{public}s, name: %{public}s, len: %{public}zu, tempPath: %{public}s",
            strerror(ret), name.get() ? "Yes" : "No", len, tempPath);
        return -1;
    }

    int32_t fd = mkstemp(name.get());
    if (fd < 0) {
        WMLOGFE("mktemp failed with %{public}d, name: %{public}s", fd, name.get());
        return -1;
    }

    ret = fcntl(fd, F_GETFD);
    if (ret == -1) {
        WMLOGFE("fcntl F_GETFD return -1, errno: %{public}s", strerror(ret));
        close(fd);
        return -1;
    }

    uint32_t flags = ret;
    ret = fcntl(fd, F_SETFD, flags | FD_CLOEXEC);
    if (ret == -1) {
        WMLOGFE("fctn F_SETFD return -1, errno: %{public}s", strerror(ret));
        close(fd);
        return -1;
    }

    ret = unlink(name.get());
    if (ret == -1) {
        WMLOGFE("unlink return -1, errno: %{public}s", strerror(ret));
        close(fd);
        return -1;
    }

    ret = ftruncate(fd, size);
    if (ret < 0) {
        WMLOGFE("ftruncate: %{public}s", strerror(ret));
        close(fd);
        return -1;
    }

    return fd;
}
} // namespace

wl_shm_pool *WlSHMBufferFactory::CreatePool(uint32_t mmapSize, int32_t &fd, void *&mmapPtr)
{
    if (mmapSize == 0) {
        WMLOGFE("shared memory size is zero");
        return nullptr;
    }

    if (shmbuf == nullptr) {
        WMLOGFE("shmbuf is nullptr");
        return nullptr;
    }

    fd = CreateShmFile(mmapSize);
    if (fd < 0) {
        WMLOGFE("CreateShmFile failed, size: %{public}d", mmapSize);
        return nullptr;
    }

    mmapPtr = mmap(nullptr, mmapSize, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    if (mmapPtr == nullptr) {
        WMLOGFE("mmap failed, size: %{public}d, fd: %{public}d", mmapSize, fd);
        close(fd);
        return nullptr;
    }

    auto pool = wl_shm_create_pool(shmbuf, fd, mmapSize);
    if (pool == nullptr) {
        WMLOGFE("wl_shm_create_pool failed, size: %{public}d, fd: %{public}d", mmapSize, fd);
        munmap(mmapPtr, mmapSize);
        close(fd);
    }
    return pool;
}

sptr<WlBuffer> WlSHMBufferFactory::Create(uint32_t w, uint32_t h, int32_t format)
{
    uint32_t stride = w * STRIDE_NUM;
    uint32_t mmapSize = stride * h;
    int32_t fd = -1;
    void *mmapPtr = nullptr;
    auto pool = CreatePool(mmapSize, fd, mmapPtr);
    if (pool == nullptr) {
        return nullptr;
    }

    auto display = delegator.Dep<WlDisplay>();
    auto buffer = wl_shm_pool_create_buffer(pool, 0, w, h, stride, format);
    if (buffer == nullptr) {
        WMLOGFE("%{public}s failed, %{public}dx%{public}d, stride: %{public}d, format: %{public}d",
            "wl_shm_pool_create_buffer", w, h, stride, format);
        return nullptr;
    }

    display->Sync();
    if (display->GetError() != 0) {
        WMLOGFE("%{public}s failed with %{public}d, %{public}dx%{public}d, stride: %{public}d, format: %{public}d",
            "wl_shm_pool_create_buffer", display->GetError(), w, h, stride, format);
        close(fd);
        return nullptr;
    }

    sptr<WlSHMBuffer> ret = new WlSHMBuffer(buffer);
    if (ret == nullptr) {
        WMLOGFE("new WlSHMBuffer failed");
        munmap(mmapPtr, mmapSize);
        close(fd);
        return nullptr;
    }
    ret->SetMmap(mmapPtr, mmapSize);

    wl_shm_pool_destroy(pool);
    close(fd);
    return ret;
}
} // namespace OHOS
