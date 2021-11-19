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

#ifndef INTERFACES_INNERKITS_WMCLIENT_SUBWINDOW_H
#define INTERFACES_INNERKITS_WMCLIENT_SUBWINDOW_H

#include <refbase.h>
#include <surface.h>

#include "window_manager_type.h"

namespace OHOS {
class Subwindow : public RefBase {
public:
    virtual sptr<Surface> GetSurface() const = 0;

    virtual WMError Move(int32_t x, int32_t y) = 0;
    virtual WMError Resize(uint32_t width, uint32_t height) = 0;
    virtual WMError Destroy() = 0;

    virtual void OnPositionChange(WindowPositionChangeFunc func) = 0;
    virtual void OnSizeChange(WindowSizeChangeFunc func) = 0;
    virtual void OnBeforeFrameSubmit(BeforeFrameSubmitFunc func) = 0;
};
} // namespace OHOS

#endif // INTERFACES_INNERKITS_WMCLIENT_SUBWINDOW_H
