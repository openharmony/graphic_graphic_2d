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

#ifndef FRAMEWORKS_WM_SRC_TEST_INATIVE_TEST_TOUCH_EVENT_HANDLER_H
#define FRAMEWORKS_WM_SRC_TEST_INATIVE_TEST_TOUCH_EVENT_HANDLER_H

#if 0
#include <scoped_bytrace.h>

#include "inative_test.h"

namespace OHOS {
class INativeTestTouchEventHandler : public MMI::TouchEventHandler {
public:
    explicit INativeTestTouchEventHandler(INativeTest *test) : test_(test)
    {
    }

    virtual bool OnTouch(const TouchEvent &event) override
    {
        ScopedBytrace trace(__func__);
        return test_->OnTouch(event);
    }

private:
    INativeTest *test_ = nullptr;
};
} // namespace OHOS
#endif

#endif // FRAMEWORKS_WM_SRC_TEST_INATIVE_TEST_TOUCH_EVENT_HANDLER_H
