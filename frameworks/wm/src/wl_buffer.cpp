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

void WlBuffer::Release(void *, struct wl_buffer *buffer)
{
    if (onReleaseFuncs.find(buffer) != onReleaseFuncs.end()) {
        onReleaseFuncs[buffer](buffer);
    }
}

WlBuffer::WlBuffer(struct wl_buffer *buffer)
{
    this->buffer = buffer;
    if (buffer != nullptr) {
        const struct wl_buffer_listener listener = { WlBuffer::Release };
        if (wl_buffer_add_listener(buffer, &listener, nullptr) == -1) {
            WMLOGFW("wl_buffer_add_listener failed");
        }
    }
}

WlBuffer::~WlBuffer()
{
    if (buffer != nullptr) {
        wl_buffer_destroy(buffer);
        onReleaseFuncs.erase(buffer);
    }
}

struct wl_buffer *WlBuffer::GetRawPtr() const
{
    return buffer;
}

void WlBuffer::OnRelease(WlBufferReleaseFunc func)
{
    onRelease = func;
    onReleaseFuncs[buffer] = onRelease;
}
} // namespace OHOS
