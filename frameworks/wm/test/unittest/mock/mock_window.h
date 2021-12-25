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

#ifndef FRAMEWORKS_WM_TEST_UNITTEST_MOCK_MOCK_WINDOW_H
#define FRAMEWORKS_WM_TEST_UNITTEST_MOCK_MOCK_WINDOW_H

#include <gmock/gmock.h>

#include <window.h>

namespace OHOS {
class MockWindow : public Window {
public:
    MOCK_CONST_METHOD0(GetSurface, sptr<Surface>());
    MOCK_CONST_METHOD0(GetProducer, sptr<IBufferProducer>());
    MOCK_CONST_METHOD0(GetID, int32_t());
    MOCK_CONST_METHOD0(GetX, int32_t());
    MOCK_CONST_METHOD0(GetY, int32_t());
    MOCK_CONST_METHOD0(GetWidth, uint32_t());
    MOCK_CONST_METHOD0(GetHeight, uint32_t());
    MOCK_CONST_METHOD0(GetDestWidth, uint32_t());
    MOCK_CONST_METHOD0(GetDestHeight, uint32_t());
    MOCK_CONST_METHOD0(GetVisibility, bool());
    MOCK_CONST_METHOD0(GetType, WindowType());
    MOCK_CONST_METHOD0(GetMode, WindowMode());
    MOCK_METHOD0(Show, sptr<Promise<GSError>>());
    MOCK_METHOD0(Hide, sptr<Promise<GSError>>());
    MOCK_METHOD2(Move, sptr<Promise<GSError>>(int32_t x, int32_t y));
    MOCK_METHOD0(SwitchTop, sptr<Promise<GSError>>());
    MOCK_METHOD1(SetWindowType, sptr<Promise<GSError>>(WindowType type));
    MOCK_METHOD1(SetWindowMode, sptr<Promise<GSError>>(WindowMode mode));
    MOCK_METHOD2(Resize, sptr<Promise<GSError>>(uint32_t width, uint32_t height));
    MOCK_METHOD2(ScaleTo, sptr<Promise<GSError>>(uint32_t width, uint32_t height));
    MOCK_METHOD1(Rotate, GSError(WindowRotateType type));
    MOCK_METHOD0(Destroy, GSError());
    MOCK_METHOD1(OnPositionChange, void(WindowPositionChangeFunc func));
    MOCK_METHOD1(OnSizeChange, void(WindowSizeChangeFunc func));
    MOCK_METHOD1(OnVisibilityChange, void(WindowVisibilityChangeFunc func));
    MOCK_METHOD1(OnTypeChange, void(WindowTypeChangeFunc func));
    MOCK_METHOD1(OnModeChange, void(WindowModeChangeFunc func));
    MOCK_METHOD1(OnSplitStatusChange, void(SplitStatusChangeFunc func));
    MOCK_METHOD1(OnKey, GSError(OnKeyFunc cb));
    MOCK_METHOD1(OnTouch, GSError(OnTouchFunc cb));
};
} // namespace OHOS

#endif // FRAMEWORKS_WM_TEST_UNITTEST_MOCK_MOCK_WINDOW_H
