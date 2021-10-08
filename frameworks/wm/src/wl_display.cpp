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

#include "wl_display.h"

#include <cerrno>
#include <chrono>
#include <mutex>
#include <poll.h>
#include <sys/eventfd.h>
#include <thread>
#include <unistd.h>

#include "window_manager_hilog.h"

using namespace std::chrono_literals;

namespace OHOS {
namespace {
constexpr HiviewDFX::HiLogLabel LABEL = {LOG_CORE, 0, "WMWlDisplay"};
}

WlDisplay::WlDisplay()
{
    WMLOGFD("WlDisplay");
}

WlDisplay::~WlDisplay()
{
    WMLOGFD("~WlDisplay");
}

sptr<WlDisplay> WlDisplay::GetInstance()
{
    if (instance == nullptr) {
        static std::mutex mutex;
        std::lock_guard<std::mutex> lock(mutex);
        if (instance == nullptr) {
            instance = new WlDisplay();
        }
    }
    return instance;
}

void WlDisplayError(void *, struct wl_display *, void *, uint32_t code, const char *message)
{
    printf("wldisplay occur error: %d, %s\n", code, message);
    printf("wldisplay occur error: %d, %s\n", code, message);
    printf("wldisplay occur error: %d, %s\n", code, message);
    WMLOGFE("wldisplay occur error: %{public}d, %{public}s\n", code, message);
    WMLOGFE("wldisplay occur error: %{public}d, %{public}s\n", code, message);
    WMLOGFE("wldisplay occur error: %{public}d, %{public}s\n", code, message);
}

bool WlDisplay::Connect(const char *name)
{
    // retry to connect
    constexpr int32_t retryTimes = 60;
    for (int32_t i = 0; i < retryTimes; i++) {
        display = wl_display_connect(name);
        if (display != nullptr) {
            WMLOGFI("connect");
            struct wl_display_listener listener = {WlDisplayError};
            wl_display_add_listener(display, &listener, nullptr);
            break;
        } else {
            WMLOGFW("create display failed! (%{public}d/%{public}d)", i + 1, retryTimes);
        }
        std::this_thread::sleep_for(50ms * i);
    }
    return display != nullptr;
}

void WlDisplay::Disconnect()
{
    if (display != nullptr) {
        WMLOGFI("disconnect");
        wl_display_disconnect(display);
        display = nullptr;
    }
}

struct wl_display *WlDisplay::GetRawPtr() const
{
    return display;
}

int32_t WlDisplay::GetFd() const
{
    if (display) {
        return wl_display_get_fd(display);
    }
    return -1;
}

int32_t WlDisplay::GetError() const
{
    if (display) {
        return wl_display_get_error(display);
    }
    return -1;
}

int32_t WlDisplay::Flush()
{
    if (display) {
        return wl_display_flush(display);
    }
    return -1;
}

int32_t WlDisplay::Dispatch()
{
    if (display) {
        return wl_display_dispatch(display);
    }
    return -1;
}

int32_t WlDisplay::Roundtrip()
{
    if (display) {
        return wl_display_roundtrip(display);
    }
    return -1;
}

int32_t WlDisplay::PrepareRead()
{
    if (display) {
        return wl_display_prepare_read(display);
    }
    return -1;
}

int32_t WlDisplay::DispatchPending()
{
    if (display) {
        return wl_display_dispatch_pending(display);
    }
    return -1;
}

void WlDisplay::CancelRead()
{
    if (display) {
        wl_display_cancel_read(display);
    }
}

int32_t WlDisplay::ReadEvents()
{
    if (display) {
        return wl_display_read_events(display);
    }
    return -1;
}

void WlDisplay::SyncDone(void *done, struct wl_callback *cb, uint32_t data)
{
    Promise<uint32_t> *promise = reinterpret_cast<Promise<uint32_t> *>(done);
    if (promise != nullptr) {
        promise->Resolve(data);
    }
}

void WlDisplay::Sync()
{
    if (display == nullptr) {
        WMLOGFW("display is nullptr");
        return;
    }

    auto callback = wl_display_sync(display);
    if (callback == nullptr) {
        WMLOGFW("callback is nullptr");
        return;
    }

    Promise<uint32_t> done;
    const struct wl_callback_listener listener = { &WlDisplay::SyncDone };
    if (wl_callback_add_listener(callback, &listener, &done) == -1) {
        WMLOGFW("wl_callback_add_listener failed");
        wl_callback_destroy(callback);
        return;
    }

    auto deathFunc = [&done]() { done.Resolve(0); };
    auto dl = AddDispatchDeathListener(deathFunc);

    if (wl_display_flush(display) == -1) {
        WMLOGFW("wl_display_flush failed");
    }

    done.Await();
    RemoveDispatchDeathListener(dl);
    wl_callback_destroy(callback);
}

void WlDisplay::StartDispatchThread()
{
    if (dispatchThread == nullptr) {
        startOnceFlag = std::make_unique<std::once_flag>();
        startPromise = new Promise<bool>();

        dispatchThread = std::make_unique<std::thread>(std::bind(&WlDisplay::DispatchThreadMain, this));

        startPromise->Await();
    } else {
        WMLOGFW("dispatch loop already started");
    }
}

void WlDisplay::StopDispatchThread()
{
    if (dispatchThread != nullptr) {
        InterruptDispatchThread();
        dispatchThread->join();
        dispatchThread = nullptr;
    } else {
        WMLOGFW("dispatch loop is not start");
    }
}

void WlDisplay::DispatchThreadMain()
{
    if (display == nullptr) {
        return;
    }

    if (startOnceFlag != nullptr) {
        static const auto onceFunc = [this]() {
            if (startPromise != nullptr) {
                startPromise->Resolve(true);
            }
        };
        std::call_once(*startOnceFlag, onceFunc);
    }

    WMLOGFI("dispatch loop start");
    interruptFd = eventfd(0, EFD_CLOEXEC | EFD_NONBLOCK);
    while (DispatchThreadCoreProcess()) {
    }

    WMLOGFI("return %{public}d, errno: %{public}d", GetError(), errno);
    {
        std::lock_guard<std::mutex> lock(dispatchDeathFuncsMutex);
        for (const auto &[_, func] : dispatchDeathFuncs) {
            func();
        }
    }

    if (interruptFd != -1) {
        close(interruptFd);
        interruptFd = -1;
    }
}

bool WlDisplay::DispatchThreadCoreProcess()
{
    while (PrepareRead() != 0) {
        DispatchPending();
    }

    if (Flush() == -1) {
        WMLOGFE("Flush return -1");
        return false;
    }

    struct pollfd pfd[] = {
        { .fd = GetFd(),   .events = POLLIN, },
        { .fd = interruptFd, .events = POLLIN, },
    };

    int32_t ret = 0;
    do {
        ret = poll(pfd, sizeof(pfd) / sizeof(*pfd), -1);
    } while (ret == -1 && errno == EINTR);

    if (ret == -1) {
        WMLOGFE("poll return -1");
        CancelRead();
        return false;
    }

    if (pfd[1].revents & POLLIN) {
        WMLOGFI("return by interrupt");
        CancelRead();
        return false;
    }

    if (pfd[0].revents & POLLIN) {
        ReadEvents();
        if (DispatchPending() == -1) {
            WMLOGFE("DispatchPending return -1");
            return false;
        }
    }
    return true;
}

void WlDisplay::InterruptDispatchThread()
{
    if (interruptFd == -1) {
        WMLOGFW("interruptFd is invalid");
        return;
    }

    uint64_t buf = 1;
    int32_t ret = 0;
    WMLOGFD("send interrupt");

    do {
        ret = write(interruptFd, &buf, sizeof(buf));
    } while (ret == -1 && errno == EINTR);
}

int32_t WlDisplay::AddDispatchDeathListener(DispatchDeathFunc func)
{
    static int32_t next = 0;
    if (func != nullptr) {
        std::lock_guard<std::mutex> lock(dispatchDeathFuncsMutex);
        dispatchDeathFuncs[next] = func;
        return next++;
    } else {
        return -1;
    }
}

void WlDisplay::RemoveDispatchDeathListener(int32_t deathListener)
{
    if (deathListener < 0) {
        return;
    }
    {
        std::lock_guard<std::mutex> lock(dispatchDeathFuncsMutex);
        dispatchDeathFuncs.erase(deathListener);
    }
}
} // namespace OHOS
