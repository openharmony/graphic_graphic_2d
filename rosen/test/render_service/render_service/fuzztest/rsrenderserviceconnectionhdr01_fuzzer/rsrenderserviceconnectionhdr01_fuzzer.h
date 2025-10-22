/*
 * Copyright (c) 2025 Huawei Device Co., Ltd.
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
#ifndef RSRENDERSERVICECONNECTIONHDR_FUZZER_H
#define RSRENDERSERVICECONNECTIONHDR_FUZZER_H

#include <ipc_callbacks/brightness_info_change_callback.h>
#include <iremote_proxy.h>

#define FUZZ_PROJECT_NAME "rsrenderserviceconnectionhdr01_fuzzer"
namespace OHOS {
namespace Rosen {
class MockBrightnessInfoChangeCallback : public IRemoteProxy<RSIBrightnessInfoChangeCallback> {
public:
    explicit MockBrightnessInfoChangeCallback() : IRemoteProxy<RSIBrightnessInfoChangeCallback>(nullptr) {};
    virtual ~MockBrightnessInfoChangeCallback() noexcept = default;

    void OnBrightnessInfoChange(ScreenId screenId, const BrightnessInfo& brightnessInfo) override {}
};
} // namespace Rosen
} // namespace OHOS
#endif // RSRENDERSERVICECONNECTIONHDR_FUZZER_H