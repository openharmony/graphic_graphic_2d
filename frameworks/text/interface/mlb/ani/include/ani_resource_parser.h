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

#ifndef OHOS_TEXT_ANI_RESOURCE_H
#define OHOS_TEXT_ANI_RESOURCE_H

#include <ani.h>
#include <memory>
#include <string>
#include <vector>

namespace OHOS::Text::ANI {
struct AniResource {
    std::string bundleName;
    std::string moduleName;
    int32_t id{0};
    std::vector<std::string> params;
    int32_t type{0};
};
class AniResourceParser final {
public:
    static AniResource ParseResource(ani_env* env, ani_object obj);
    static bool ResolveResource(const AniResource& resource, size_t& dataLen, std::unique_ptr<uint8_t[]>& data);
};
} // namespace OHOS::Text::ANI
#endif // OHOS_TEXT_ANI_RESOURCE_H