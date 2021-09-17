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

#ifndef FRAMEWORKS_WMTEST_TEST_WMCLIENT_WMCLIENT_NATIVE_TEST_6_H
#define FRAMEWORKS_WMTEST_TEST_WMCLIENT_WMCLIENT_NATIVE_TEST_6_H

#include <cstdint>
#include <string>

#include <refbase.h>
#include <window_manager.h>

#include "native_test_class.h"
#include "wmclient_native_test_1.h"

namespace OHOS {
class WMClientNativeTest6 : public WMClientNativeTest1 {
public:
    virtual ~WMClientNativeTest6() = default;
    virtual std::string GetDescription() const override;
    virtual int32_t GetID() const override;
    virtual void Run(int32_t argc, const char **argv) override;

    virtual void AfterRun();

protected:
    sptr<Surface> subcsurface = nullptr;
    sptr<Subwindow> subwindow = nullptr;
    sptr<NativeTestSync> subwindowSync = nullptr;
};
}

#endif // FRAMEWORKS_WMTEST_TEST_WMCLIENT_WMCLIENT_NATIVE_TEST_6_H
