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

#ifndef FRAMEWORKS_WM_INCLUDE_WP_VIEWPORT_H
#define FRAMEWORKS_WM_INCLUDE_WP_VIEWPORT_H

#include <refbase.h>
#include <viewporter-client-protocol.h>

namespace OHOS {
class WpViewport : public RefBase {
public:
    WpViewport(struct wp_viewport *viewport);
    virtual ~WpViewport() override;

    struct wp_viewport *GetRawPtr() const;

    void SetSource(double x, double y, double w, double h);
    void SetDestination(uint32_t w, uint32_t h);

private:
    struct wp_viewport *viewport;
};
} // namespace OHOS

#endif // FRAMEWORKS_WM_INCLUDE_WP_VIEWPORT_H
