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

#ifndef FRAMEWORKS_WM_INCLUDE_WL_BUFFER_H
#define FRAMEWORKS_WM_INCLUDE_WL_BUFFER_H

#include <map>

#include <linux-explicit-synchronization-unstable-v1-client-protocol.h>
#include <refbase.h>
#include <wayland-client-protocol.h>

namespace OHOS {
using WlBufferReleaseFunc = std::function<void(struct wl_buffer *buffer, int32_t fence)>;

class WlBuffer : public RefBase {
public:
    WlBuffer(struct wl_buffer *wb);
    virtual ~WlBuffer() override;

    struct wl_buffer *GetRawPtr() const;

    void SetBufferRelease(struct zwp_linux_buffer_release_v1 *br);
    void OnRelease(WlBufferReleaseFunc func);

protected:
    struct wl_buffer *buffer = nullptr;
    struct zwp_linux_buffer_release_v1 *release = nullptr;
    WlBufferReleaseFunc onRelease = nullptr;

private:
    static void Release(void *, struct wl_buffer *buffer);
    static void FencedRelease(void *, struct zwp_linux_buffer_release_v1 *release, int32_t fence);
    static void ImmediateRelease(void *, struct zwp_linux_buffer_release_v1 *release);
};
} // namespace OHOS

#endif // FRAMEWORKS_WM_INCLUDE_WL_BUFFER_H
