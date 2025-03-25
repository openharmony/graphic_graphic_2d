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

#ifndef SAFUZZ_MESSAGE_PARCEL_BASIC_TYPE_UTILS_H
#define SAFUZZ_MESSAGE_PARCEL_BASIC_TYPE_UTILS_H

#include "tools/ipc_utils.h"

namespace OHOS {
namespace Rosen {
class MessageParcelBasicTypeUtils {
public:
    static std::function<bool(MessageParcel&)> GetWriteFunctionByTypeName(const std::string& typeName);

private:
    static bool WriteRandomBool(MessageParcel& messageParcel);
    static bool WriteRandomInt8(MessageParcel& messageParcel);
    static bool WriteRandomInt16(MessageParcel& messageParcel);
    static bool WriteRandomInt32(MessageParcel& messageParcel);
    static bool WriteRandomInt64(MessageParcel& messageParcel);
    static bool WriteRandomUint8(MessageParcel& messageParcel);
    static bool WriteRandomUint16(MessageParcel& messageParcel);
    static bool WriteRandomUint32(MessageParcel& messageParcel);
    static bool WriteRandomUint64(MessageParcel& messageParcel);
    static bool WriteRandomDouble(MessageParcel& messageParcel);
    static bool WriteRandomFloat(MessageParcel& messageParcel);
    static bool WriteRandomString(MessageParcel& messageParcel);
    static bool WriteRandomCString(MessageParcel& messageParcel);
    static bool WriteRandomString16(MessageParcel& messageParcel);
    static bool WriteRandomString8WithLength(MessageParcel& messageParcel);
    static bool WriteRandomString16WithLength(MessageParcel& messageParcel);
    static bool WriteRandomPointer(MessageParcel& messageParcel);
    static bool WriteRandomBuffer(MessageParcel& messageParcel);
    static bool WriteRandomBoolVector(MessageParcel& messageParcel);
    static bool WriteRandomInt8Vector(MessageParcel& messageParcel);
    static bool WriteRandomInt16Vector(MessageParcel& messageParcel);
    static bool WriteRandomInt32Vector(MessageParcel& messageParcel);
    static bool WriteRandomInt64Vector(MessageParcel& messageParcel);
    static bool WriteRandomUInt8Vector(MessageParcel& messageParcel);
    static bool WriteRandomUInt16Vector(MessageParcel& messageParcel);
    static bool WriteRandomUInt32Vector(MessageParcel& messageParcel);
    static bool WriteRandomUInt64Vector(MessageParcel& messageParcel);
    static bool WriteRandomFloatVector(MessageParcel& messageParcel);
    static bool WriteRandomDoubleVector(MessageParcel& messageParcel);
    static bool WriteRandomStringVector(MessageParcel& messageParcel);
    static bool WriteRandomString16Vector(MessageParcel& messageParcel);
};
} // namespace Rosen
} // namespace OHOS

#endif // SAFUZZ_MESSAGE_PARCEL_BASIC_TYPE_UTILS_H
