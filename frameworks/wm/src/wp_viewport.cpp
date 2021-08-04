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

#include "wp_viewport.h"

namespace OHOS {
WpViewport::WpViewport(struct wp_viewport *viewport)
{
    this->viewport = viewport;
}

WpViewport::~WpViewport()
{
    wp_viewport_destroy(viewport);
}

struct wp_viewport *WpViewport::GetRawPtr() const
{
    return viewport;
}

void WpViewport::SetSource(double dx, double dy, double dw, double dh)
{
    if (viewport) {
        wp_viewport_set_source(viewport,
            wl_fixed_from_double(dx), wl_fixed_from_double(dy),
            wl_fixed_from_double(dw), wl_fixed_from_double(dh));
    }
}

void WpViewport::SetDestination(uint32_t w, uint32_t h)
{
    if (viewport) {
        wp_viewport_set_destination(viewport, w, h);
    }
}
} // namespace OHOS
