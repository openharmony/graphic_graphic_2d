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

#ifndef FRAMEWORKS_WM_TEST_UNITTEST_MOCK_MOCK_IWINDOW_MANAGER_SERVICE_H
#define FRAMEWORKS_WM_TEST_UNITTEST_MOCK_MOCK_IWINDOW_MANAGER_SERVICE_H

#include <gmock/gmock.h>

#include <iwindow_manager_service.h>

namespace OHOS {
class MockIWindowManagerService : public IWindowManagerService {
public:
    MOCK_METHOD1(GetDisplays, WMError(std::vector<struct WMDisplayInfo> &displays));
    MOCK_METHOD1(GetDisplayModes, WMError(uint32_t &displayModes));
    MOCK_METHOD1(SetDisplayMode, sptr<Promise<WMError>>(WMSDisplayMode modes));
    MOCK_METHOD1(AddDisplayChangeListener, WMError(IWindowManagerDisplayListenerClazz *listener));
    MOCK_METHOD1(OnWindowListChange, sptr<Promise<WMError>>(IWindowChangeListenerClazz *listener));
    MOCK_METHOD1(SetDisplayDirection, WMError(WMSDisplayDirection direction));
    MOCK_METHOD1(OnDisplayDirectionChange, WMError(DisplayDirectionChangeFunc func));
    MOCK_METHOD1(ShotScreen, sptr<Promise<WMSImageInfo>>(int32_t did));
    MOCK_METHOD1(ShotWindow, sptr<Promise<WMSImageInfo>>(int32_t wid));
    MOCK_METHOD1(DestroyWindow, sptr<Promise<WMError>>(int32_t wid));
    MOCK_METHOD1(SwitchTop, sptr<Promise<WMError>>(int32_t wid));
    MOCK_METHOD1(Show, sptr<Promise<WMError>>(int32_t wid));
    MOCK_METHOD1(Hide, sptr<Promise<WMError>>(int32_t wid));
    MOCK_METHOD3(Move, sptr<Promise<WMError>>(int32_t wid, int32_t x, int32_t y));
    MOCK_METHOD3(Resize, sptr<Promise<WMError>>(int32_t wid, uint32_t width, uint32_t height));
    MOCK_METHOD3(ScaleTo, sptr<Promise<WMError>>(int32_t wid, uint32_t width, uint32_t height));
    MOCK_METHOD2(SetWindowType, sptr<Promise<WMError>>(int32_t wid, WindowType type));
};
} // namespace OHOS

#endif // FRAMEWORKS_WM_TEST_UNITTEST_MOCK_MOCK_IWINDOW_MANAGER_SERVICE_H
