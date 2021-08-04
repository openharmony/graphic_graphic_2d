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

#include <mutex>
#include <poll.h>
#include <sys/eventfd.h>
#include <unistd.h>

#include "window_manager_hilog.h"

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

bool WlDisplay::Connect(const char *name)
{
    // retry to connect
    constexpr int32_t retryTimes = 60;
    for (int32_t i = 0; i < retryTimes; i++) {
        display = wl_display_connect(name);
        if (display != nullptr) {
            WMLOGFI("connect");
            break;
        } else {
            WMLOGFW("create display failed! (%{public}d/%{public}d)", i + 1, retryTimes);
        }

        constexpr int32_t sleepTimeFactor = 50 * 1000;
        int32_t sleepTime = i * sleepTimeFactor;
        usleep(sleepTime);
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

    WMLOGFI("dispatch loop start");
    interruptFd = eventfd(0, EFD_CLOEXEC | EFD_NONBLOCK);
    int32_t displayFd = GetFd();
    while (true) {
        if (startOnceFlag != nullptr) {
            static const auto onceFunc = [this]() {
                if (startPromise != nullptr) {
                    startPromise->Resolve(true);
                }
            };
            std::call_once(*startOnceFlag, onceFunc);
        }

        while (PrepareRead() != 0) {
            DispatchPending();
        }

        if (Flush() == -1) {
            WMLOGFE("Flush return -1");
            break;
        }

        struct pollfd pfd[] = {
            { .fd = displayFd,   .events = POLLIN, },
            { .fd = interruptFd, .events = POLLIN, },
        };

        int32_t ret = poll(pfd, sizeof(pfd) / sizeof(*pfd), -1);
        if (ret == -1) {
            WMLOGFE("poll return -1");
            CancelRead();
            break;
        }

        if (pfd[1].revents & POLLIN) {
            WMLOGFI("return by interrupt");
            CancelRead();
            break;
        }

        if (pfd[0].revents & POLLIN) {
            ReadEvents();
            if (DispatchPending() == -1) {
                WMLOGFE("DispatchPending return -1");
                break;
            }
        }
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
