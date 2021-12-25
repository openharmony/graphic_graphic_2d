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

#ifndef INTERFACES_INNERKITS_WMCLIENT_WINDOW_OPTION_H
#define INTERFACES_INNERKITS_WMCLIENT_WINDOW_OPTION_H

#include <refbase.h>
#include <surface.h>

#include "window_manager_type.h"

namespace OHOS {
class WindowOption : public RefBase {
public:
    static sptr<WindowOption> Get();

    virtual GSError SetWindowType(WindowType type) = 0;
    virtual GSError SetWindowMode(WindowMode mode) = 0;
    virtual GSError SetDisplay(int32_t did) = 0;
    virtual GSError SetConsumerSurface(const sptr<Surface> &surf) = 0;
    virtual GSError SetX(int32_t x) = 0;
    virtual GSError SetY(int32_t y) = 0;
    virtual GSError SetWidth(uint32_t width) = 0;
    virtual GSError SetHeight(uint32_t height) = 0;

    virtual WindowType GetWindowType() const = 0;
    virtual WindowMode GetWindowMode() const = 0;
    virtual int32_t GetDisplay() const = 0;
    virtual sptr<Surface> GetConsumerSurface() const = 0;
    virtual int32_t GetX() const = 0;
    virtual int32_t GetY() const = 0;
    virtual uint32_t GetWidth() const = 0;
    virtual uint32_t GetHeight() const = 0;

    virtual bool IsSettingX() const = 0;
    virtual bool IsSettingY() const = 0;
    virtual bool IsSettingWidth() const = 0;
    virtual bool IsSettingHeight() const = 0;
};
} // namespace OHOS

#endif // INTERFACES_INNERKITS_WMCLIENT_WINDOW_OPTION_H
