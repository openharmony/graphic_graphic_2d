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

#include "wl_subsurface.h"

namespace OHOS {
WlSubsurface::WlSubsurface(struct wl_subsurface *subsurf)
{
    this->subsurf = subsurf;
}

WlSubsurface::~WlSubsurface()
{
    wl_subsurface_destroy(subsurf);
}

struct wl_subsurface *WlSubsurface::GetRawPtr() const
{
    return subsurf;
}

void WlSubsurface::SetPosition(int32_t x, int32_t y)
{
    wl_subsurface_set_position(subsurf, x, y);
}

void WlSubsurface::PlaceAbove(const sptr<WlSurface> &sibling)
{
    wl_subsurface_place_above(subsurf, sibling->GetRawPtr());
}

void WlSubsurface::PlaceBelow(const sptr<WlSurface> &sibling)
{
    wl_subsurface_place_below(subsurf, sibling->GetRawPtr());
}

void WlSubsurface::SetSync()
{
    wl_subsurface_set_sync(subsurf);
}

void WlSubsurface::SetDesync()
{
    wl_subsurface_set_desync(subsurf);
}
} // namespace OHOS
