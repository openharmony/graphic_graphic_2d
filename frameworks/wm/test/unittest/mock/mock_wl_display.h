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

#ifndef FRAMEWORKS_WM_TEST_UNITTEST_MOCK_MOCK_WL_DISPLAY_H
#define FRAMEWORKS_WM_TEST_UNITTEST_MOCK_MOCK_WL_DISPLAY_H

#include <gmock/gmock.h>

#include "wl_display.h"

namespace OHOS {
class MockWlDisplay : public WlDisplay {
public:
    MOCK_METHOD1(Connect, bool(const char *name));
    MOCK_METHOD0(Disconnect, void());
    MOCK_CONST_METHOD0(GetRawPtr, struct wl_display *());
    MOCK_CONST_METHOD0(GetFd, int32_t());
    MOCK_CONST_METHOD0(GetError, int32_t());
    MOCK_METHOD0(Flush, int32_t());
    MOCK_METHOD0(Dispatch, int32_t());
    MOCK_METHOD0(Roundtrip, int32_t());
    MOCK_METHOD0(Sync, void());
    MOCK_METHOD0(StartDispatchThread, void());
    MOCK_METHOD0(StopDispatchThread, void());
    MOCK_METHOD1(AddDispatchDeathListener, int32_t(DispatchDeathFunc func));
    MOCK_METHOD1(RemoveDispatchDeathListener, void(int32_t deathListener));
};
} // namespace OHOS

#endif // FRAMEWORKS_WM_TEST_UNITTEST_MOCK_MOCK_WL_DISPLAY_H
