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
#ifndef RS_ANCO_TYPE_H
#define RS_ANCO_TYPE_H

namespace OHOS::Rosen {
enum class AncoFlags : uint32_t {
    IS_ANCO_NODE = 0x0001,
    ANCO_SFV_NODE = 0x0011,
    ANCO_NATIVE_NODE = 0x0111,
    FORCE_REFRESH = 0x1000
};

enum class AncoHebcStatus : int32_t {
    INITIAL,
    NOT_USE_HEBC,
    USE_HEBC
};

} // namespace OHOS::Rosen
#endif // RS_ANCO_TYPE_H