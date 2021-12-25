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

#ifndef FRAMEWORKS_WM_INCLUDE_WL_SUBSURFACE_H
#define FRAMEWORKS_WM_INCLUDE_WL_SUBSURFACE_H

#include <refbase.h>
#include <wayland-client-protocol.h>

#include "wl_surface.h"

namespace OHOS {
class WlSubsurface : public RefBase {
public:
    WlSubsurface(struct wl_subsurface *subsurf);
    virtual ~WlSubsurface() override;

    struct wl_subsurface *GetRawPtr() const;

    void SetPosition(int32_t x, int32_t y);
    void PlaceAbove(const sptr<WlSurface> &sibling);
    void PlaceBelow(const sptr<WlSurface> &sibling);
    void SetSync();
    void SetDesync();

private:
    struct wl_subsurface *subsurf;
};
} // namespace OHOS

#endif // FRAMEWORKS_WM_INCLUDE_WL_SUBSURFACE_H
