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

#ifndef RENDER_SERVICE_MOCK_ACCESSTOKEN_KIT_H
#define RENDER_SERVICE_MOCK_ACCESSTOKEN_KIT_H

#include <cstdint>
#include <string>

namespace OHOS::Rosen {
class MockAccessTokenKit {
public:
    static void MockProcessName(const std::string &processName);
    static void MockAccessTokenKitRet(int accessTokenKitRet);
    static void MockTokenType(bool isNative);
};

} // namespace OHOS::Rosen
#endif  // RENDER_SERVICE_MOCK_ACCESSTOKEN_KIT_H
 