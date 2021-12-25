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

#ifndef FRAMEWORKS_WM_INCLUDE_WL_SURFACE_H
#define FRAMEWORKS_WM_INCLUDE_WL_SURFACE_H

#include <linux-explicit-synchronization-unstable-v1-client-protocol.h>
#include <refbase.h>
#include <viewporter-client-protocol.h>
#include <wayland-client-protocol.h>

#include "wl_buffer.h"

namespace OHOS {
class WlSurface : public RefBase {
public:
    WlSurface(struct wl_surface *ws,
              struct zwp_linux_surface_synchronization_v1 *zlssv,
              struct wp_viewport *wv);
    virtual ~WlSurface() override;

    struct wl_surface *GetRawPtr() const;
    void *GetUserData() const;
    void SetUserData(void *data);

    void Attach(sptr<WlBuffer> &buffer, int32_t x, int32_t y);
    void Damage(int32_t x, int32_t y, uint32_t w, uint32_t h);
    void Commit();
    void SetSurfaceType(wl_surface_type type);
    void SetBufferTransform(wl_output_transform type);
    struct zwp_linux_buffer_release_v1 *GetBufferRelease();
    void SetAcquireFence(int32_t fence);
    void SetSource(double x, double y, double w, double h);
    void SetDestination(uint32_t w, uint32_t h);

private:
    struct wl_surface *surf = nullptr;
    struct zwp_linux_surface_synchronization_v1 *sync = nullptr;
    struct wp_viewport *viewport = nullptr;
    enum wl_output_transform bufferTransform = WL_OUTPUT_TRANSFORM_NORMAL;
};
} // namespace OHOS

#endif // FRAMEWORKS_WM_INCLUDE_WL_SURFACE_H
