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

#include "wl_buffer.h"

#include "window_manager_hilog.h"

namespace OHOS {
namespace {
constexpr HiviewDFX::HiLogLabel LABEL = { LOG_CORE, 0, "WMWlBuffer" };
} // namespace

void WlBuffer::Release(void *data, struct wl_buffer *buffer)
{
    auto wb = reinterpret_cast<WlBuffer *>(data);
    if (wb->release == nullptr && wb->onRelease != nullptr) {
        wb->onRelease(wb->buffer, -1);
    }
}

void WlBuffer::FencedRelease(void *data, struct zwp_linux_buffer_release_v1 *release, int32_t fence)
{
    auto wb = reinterpret_cast<WlBuffer *>(data);
    if (wb->onRelease != nullptr) {
        wb->onRelease(wb->buffer, fence);
        zwp_linux_buffer_release_v1_destroy(release);
        wb->release = nullptr;
    }
}

void WlBuffer::ImmediateRelease(void *data, struct zwp_linux_buffer_release_v1 *release)
{
    auto wb = reinterpret_cast<WlBuffer *>(data);
    if (wb->onRelease != nullptr) {
        wb->onRelease(wb->buffer, -1);
        zwp_linux_buffer_release_v1_destroy(release);
        wb->release = nullptr;
    }
}

WlBuffer::WlBuffer(struct wl_buffer *wb)
{
    buffer = wb;
    if (buffer == nullptr) {
        return;
    }

    const struct wl_buffer_listener listener = { WlBuffer::Release };
    if (wl_buffer_add_listener(buffer, &listener, this) == -1) {
        WMLOGFW("wl_buffer_add_listener failed");
    }
}

WlBuffer::~WlBuffer()
{
    if (release != nullptr) {
        zwp_linux_buffer_release_v1_destroy(release);
        release = nullptr;
    }

    if (buffer != nullptr) {
        wl_buffer_destroy(buffer);
        buffer = nullptr;
    }
}

struct wl_buffer *WlBuffer::GetRawPtr() const
{
    return buffer;
}

void WlBuffer::SetBufferRelease(struct zwp_linux_buffer_release_v1 *br)
{
    release = br;
    if (release == nullptr) {
        return;
    }

    const struct zwp_linux_buffer_release_v1_listener listener = {
        WlBuffer::FencedRelease,
        WlBuffer::ImmediateRelease,
    };
    if (zwp_linux_buffer_release_v1_add_listener(release, &listener, this) == -1) {
        WMLOGFW("zwp_linux_buffer_release_v1_add_listener failed");
        zwp_linux_buffer_release_v1_destroy(release);
        release = nullptr;
    }
}

void WlBuffer::OnRelease(WlBufferReleaseFunc func)
{
    onRelease = func;
}
} // namespace OHOS
