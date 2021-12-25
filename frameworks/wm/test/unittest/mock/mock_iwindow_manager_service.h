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
    MOCK_METHOD1(GetDisplays, GSError(std::vector<struct WMDisplayInfo> &displays));
    MOCK_METHOD1(GetDisplayPower, sptr<PromisePowerStatus>(int32_t did));
    MOCK_METHOD2(SetDisplayPower, sptr<PromiseGSError>(int32_t did, DispPowerStatus status));
    MOCK_METHOD1(GetDisplayBacklight, sptr<PromiseBacklight>(int32_t did));
    MOCK_METHOD2(SetDisplayBacklight, sptr<PromiseGSError>(int32_t did, uint32_t level));
    MOCK_METHOD1(GetDisplayModes, GSError(uint32_t &displayModes));
    MOCK_METHOD1(SetDisplayMode, sptr<PromiseGSError>(WMSDisplayMode modes));
    MOCK_METHOD1(AddDisplayChangeListener, GSError(IWindowManagerDisplayListenerClazz *listener));
    MOCK_METHOD1(OnWindowListChange, sptr<PromiseGSError>(IWindowChangeListenerClazz *listener));
    MOCK_METHOD1(SetDisplayDirection, GSError(WMSDisplayDirection direction));
    MOCK_METHOD1(OnDisplayDirectionChange, GSError(DisplayDirectionChangeFunc func));
    MOCK_METHOD1(ShotScreen, sptr<PromiseWMSImageInfo>(int32_t did));
    MOCK_METHOD1(ShotWindow, sptr<PromiseWMSImageInfo>(int32_t wid));
    MOCK_METHOD1(SetStatusBarVisibility, sptr<PromiseGSError>(bool visibility));
    MOCK_METHOD1(SetNavigationBarVisibility, sptr<PromiseGSError>(bool visibility));
    MOCK_METHOD1(DestroyWindow, sptr<PromiseGSError>(int32_t wid));
    MOCK_METHOD1(SwitchTop, sptr<PromiseGSError>(int32_t wid));
    MOCK_METHOD1(Show, sptr<PromiseGSError>(int32_t wid));
    MOCK_METHOD1(Hide, sptr<PromiseGSError>(int32_t wid));
    MOCK_METHOD3(Move, sptr<PromiseGSError>(int32_t wid, int32_t x, int32_t y));
    MOCK_METHOD3(Resize, sptr<PromiseGSError>(int32_t wid, uint32_t width, uint32_t height));
    MOCK_METHOD3(ScaleTo, sptr<PromiseGSError>(int32_t wid, uint32_t width, uint32_t height));
    MOCK_METHOD2(SetWindowType, sptr<PromiseGSError>(int32_t wid, WindowType type));
    MOCK_METHOD2(SetWindowMode, sptr<PromiseGSError>(int32_t wid, WindowMode mode));
    MOCK_METHOD4(CreateVirtualDisplay, sptr<PromiseGSError>(int32_t x, int32_t y, int32_t width, int32_t height));
    MOCK_METHOD1(DestroyVirtualDisplay, sptr<PromiseGSError>(uint32_t did));
    MOCK_METHOD2(StartRotationAnimation, GSError(uint32_t did, int32_t degree));
    MOCK_METHOD3(SetSplitMode, sptr<PromiseGSError>(SplitMode mode, int32_t x, int32_t y));
};
} // namespace OHOS

#endif // FRAMEWORKS_WM_TEST_UNITTEST_MOCK_MOCK_IWINDOW_MANAGER_SERVICE_H
