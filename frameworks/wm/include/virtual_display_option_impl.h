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

#ifndef FRAMEWORKS_WM_INCLUDE_VIRTUAL_DISPLAY_OPTION_IMPL_H
#define FRAMEWORKS_WM_INCLUDE_VIRTUAL_DISPLAY_OPTION_IMPL_H

#include <virtual_display_option.h>

namespace OHOS {
class VirtualDisplayOptionImpl : public VirtualDisplayOption {
public:
    VirtualDisplayOptionImpl() = default;
    virtual ~VirtualDisplayOptionImpl() = default;

    virtual WMError SetX(int32_t x) override;
    virtual WMError SetY(int32_t y) override;
    virtual WMError SetWidth(uint32_t width) override;
    virtual WMError SetHeight(uint32_t height) override;

    virtual int32_t GetX() const override;
    virtual int32_t GetY() const override;
    virtual uint32_t GetWidth() const override;
    virtual uint32_t GetHeight() const override;

    virtual bool IsSettingX() const override;
    virtual bool IsSettingY() const override;
    virtual bool IsSettingWidth() const override;
    virtual bool IsSettingHeight() const override;

private:
    bool settingX = false;
    bool settingY = false;
    bool settingWidth = false;
    bool settingHeight = false;

    int32_t x = 0;
    int32_t y = 0;
    uint32_t width = 1;
    uint32_t height = 1;
};
} // namespace OHOS

#endif // FRAMEWORKS_WM_INCLUDE_VIRTUAL_DISPLAY_OPTION_IMPL_H
