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

#ifndef FRAMEWORKS_WM_TEST_UNITTEST_MOCK_MOCK_SUBWINDOW_OPTION_H
#define FRAMEWORKS_WM_TEST_UNITTEST_MOCK_MOCK_SUBWINDOW_OPTION_H

#include <gmock/gmock.h>

#include <subwindow_option.h>

namespace OHOS {
class MockSubwindowOption : public SubwindowOption {
public:
    MOCK_METHOD1(SetWindowType, GSError(SubwindowType type));
    MOCK_METHOD1(SetX, GSError(int32_t x));
    MOCK_METHOD1(SetY, GSError(int32_t y));
    MOCK_METHOD1(SetWidth, GSError(uint32_t w));
    MOCK_METHOD1(SetHeight, GSError(uint32_t h));
    MOCK_METHOD1(SetConsumerSurface, GSError(const sptr<Surface> &surf));
    MOCK_CONST_METHOD0(GetWindowType, SubwindowType());
    MOCK_CONST_METHOD0(GetX, int32_t());
    MOCK_CONST_METHOD0(GetY, int32_t());
    MOCK_CONST_METHOD0(GetWidth, uint32_t());
    MOCK_CONST_METHOD0(GetHeight, uint32_t());
    MOCK_CONST_METHOD0(GetConsumerSurface, sptr<Surface>());
};
} // namespace OHOS

#endif // FRAMEWORKS_WM_TEST_UNITTEST_MOCK_MOCK_SUBWINDOW_OPTION_H
