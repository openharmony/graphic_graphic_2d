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

#include "wl_surface.h"

namespace OHOS {
WlSurface::WlSurface(struct wl_surface *surface)
{
    this->surface = surface;
}

WlSurface::~WlSurface()
{
    wl_surface_destroy(surface);
}

struct wl_surface *WlSurface::GetRawPtr() const
{
    return surface;
}

void *WlSurface::GetUserData() const
{
    return wl_surface_get_user_data(surface);
}

void WlSurface::SetUserData(void *data)
{
    wl_surface_set_user_data(surface, data);
}

void WlSurface::Attach(sptr<WlBuffer> &buffer, int32_t x, int32_t y)
{
    wl_surface_attach(surface, buffer->GetRawPtr(), x, y);
}

void WlSurface::Damage(int32_t x, int32_t y, uint32_t w, uint32_t h)
{
    wl_surface_damage(surface, x, y, w, h);
}

void WlSurface::Commit()
{
    wl_surface_commit(surface);
}

void WlSurface::SetSurfaceType(wl_surface_type type)
{
    wl_surface_set_surface_type(surface, type);
}

void WlSurface::SetBufferTransform(wl_output_transform type)
{
    wl_surface_set_buffer_transform(surface, type);
}
} // namespace OHOS
