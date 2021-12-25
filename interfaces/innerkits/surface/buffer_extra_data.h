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

#ifndef INTERFACES_INNERKITS_SURFACE_BUFFER_EXTRA_DATA_H
#define INTERFACES_INNERKITS_SURFACE_BUFFER_EXTRA_DATA_H

#include <string>

#include "surface_type.h"

namespace OHOS {
class MessageParcel;
class BufferExtraData {
public:
    virtual GSError ReadFromParcel(MessageParcel &parcel) = 0;
    virtual GSError WriteToParcel(MessageParcel &parcel) = 0;
    virtual GSError ExtraGet(std::string &key, int32_t &value) const = 0;
    virtual GSError ExtraGet(std::string &key, int64_t &value) const = 0;
    virtual GSError ExtraGet(std::string &key, double &value) const = 0;
    virtual GSError ExtraGet(std::string &key, std::string &value) const = 0;
    virtual GSError ExtraSet(std::string &key, int32_t value) = 0;
    virtual GSError ExtraSet(std::string &key, int64_t value) = 0;
    virtual GSError ExtraSet(std::string &key, double value) = 0;
    virtual GSError ExtraSet(std::string &key, std::string value) = 0;
};
} // namespace OHOS

#endif // INTERFACES_INNERKITS_SURFACE_BUFFER_EXTRA_DATA_H
