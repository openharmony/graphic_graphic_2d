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

#ifndef FRAMEWORKS_WM_TEST_UNITTEST_LOG_LISTENER_TEST_H
#define FRAMEWORKS_WM_TEST_UNITTEST_LOG_LISTENER_TEST_H

#include <gtest/gtest.h>
#include <refbase.h>
#include <window_manager.h>

#include "input_listener_manager.h"
#include "log_listener.h"

namespace OHOS {
class LogListenerTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    virtual void SetUp() override;
    virtual void TearDown() override;

private:
    static inline WMError initRet = WM_ERROR_NOT_INIT;
    static inline sptr<InputListener> listener[4] = {};
    static inline int32_t addressA = 0;
};
}

#endif // FRAMEWORKS_WM_TEST_UNITTEST_LOG_LISTENER_TEST_H
