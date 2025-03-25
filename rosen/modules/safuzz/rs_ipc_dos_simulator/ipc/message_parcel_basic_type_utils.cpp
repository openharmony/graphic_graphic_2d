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

#include "ipc/message_parcel_basic_type_utils.h"

#include <cstdlib>
#include <cstring>
#include <functional>
#include <string>
#include <unordered_map>
#include <vector>

#include "string_ex.h"

#include "common/safuzz_log.h"
#include "random/random_data.h"
#include "tools/common_utils.h"

namespace OHOS {
namespace Rosen {
namespace {
#define DECLARE_WRITE_RANDOM(type) { "Write" #type, MessageParcelBasicTypeUtils::WriteRandom##type }

const std::unordered_map<std::string, std::function<bool(MessageParcel&)>> TYPE_NAME_TO_WRITE_FUNCTION_MAPPING = {
    DECLARE_WRITE_RANDOM(Bool),
    DECLARE_WRITE_RANDOM(Int8),
    DECLARE_WRITE_RANDOM(Int16),
    DECLARE_WRITE_RANDOM(Int32),
    DECLARE_WRITE_RANDOM(Int64),
    DECLARE_WRITE_RANDOM(Uint8),
    DECLARE_WRITE_RANDOM(Uint16),
    DECLARE_WRITE_RANDOM(Uint32),
    DECLARE_WRITE_RANDOM(Uint64),
    DECLARE_WRITE_RANDOM(Double),
    DECLARE_WRITE_RANDOM(Float),
    DECLARE_WRITE_RANDOM(String),
    DECLARE_WRITE_RANDOM(CString),
    DECLARE_WRITE_RANDOM(String16),
    DECLARE_WRITE_RANDOM(String8WithLength),
    DECLARE_WRITE_RANDOM(String16WithLength),
    DECLARE_WRITE_RANDOM(Pointer),
    DECLARE_WRITE_RANDOM(Buffer),
    DECLARE_WRITE_RANDOM(BoolVector),
    DECLARE_WRITE_RANDOM(Int8Vector),
    DECLARE_WRITE_RANDOM(Int16Vector),
    DECLARE_WRITE_RANDOM(Int32Vector),
    DECLARE_WRITE_RANDOM(Int64Vector),
    DECLARE_WRITE_RANDOM(UInt8Vector),
    DECLARE_WRITE_RANDOM(UInt16Vector),
    DECLARE_WRITE_RANDOM(UInt32Vector),
    DECLARE_WRITE_RANDOM(UInt64Vector),
    DECLARE_WRITE_RANDOM(FloatVector),
    DECLARE_WRITE_RANDOM(DoubleVector),
    DECLARE_WRITE_RANDOM(StringVector),
    DECLARE_WRITE_RANDOM(String16Vector),
};
}

std::function<bool(MessageParcel&)> MessageParcelBasicTypeUtils::GetWriteFunctionByTypeName(const std::string& typeName)
{
    auto it = TYPE_NAME_TO_WRITE_FUNCTION_MAPPING.find(typeName);
    if (it == TYPE_NAME_TO_WRITE_FUNCTION_MAPPING.end()) {
        return nullptr;
    }
    return it->second;
}

bool MessageParcelBasicTypeUtils::WriteRandomBool(MessageParcel& messageParcel)
{
    bool data = RandomData::GetRandomBool();
    SAFUZZ_LOGI("MessageParcelBasicTypeUtils::WriteRandomBool %d", static_cast<int>(data));
    return messageParcel.WriteBool(data);
}

bool MessageParcelBasicTypeUtils::WriteRandomInt8(MessageParcel& messageParcel)
{
    int8_t data = RandomData::GetRandomInt8();
    SAFUZZ_LOGI("MessageParcelBasicTypeUtils::WriteRandomInt8 0x%x", data);
    return messageParcel.WriteInt8(data);
}

bool MessageParcelBasicTypeUtils::WriteRandomInt16(MessageParcel& messageParcel)
{
    int16_t data = RandomData::GetRandomInt16();
    SAFUZZ_LOGI("MessageParcelBasicTypeUtils::WriteRandomInt16 0x%x", data);
    return messageParcel.WriteInt16(data);
}

bool MessageParcelBasicTypeUtils::WriteRandomInt32(MessageParcel& messageParcel)
{
    int32_t data = RandomData::GetRandomInt32();
    SAFUZZ_LOGI("MessageParcelBasicTypeUtils::WriteRandomInt32 %" PRId32, data);
    return messageParcel.WriteInt32(data);
}

bool MessageParcelBasicTypeUtils::WriteRandomInt64(MessageParcel& messageParcel)
{
    int64_t data = RandomData::GetRandomInt64();
    SAFUZZ_LOGI("MessageParcelBasicTypeUtils::WriteRandomInt64 %" PRId64, data);
    return messageParcel.WriteInt64(data);
}

bool MessageParcelBasicTypeUtils::WriteRandomUint8(MessageParcel& messageParcel)
{
    uint8_t data = RandomData::GetRandomUint8();
    SAFUZZ_LOGI("MessageParcelBasicTypeUtils::WriteRandomUint8 0x%x", data);
    return messageParcel.WriteUint8(data);
}

bool MessageParcelBasicTypeUtils::WriteRandomUint16(MessageParcel& messageParcel)
{
    uint16_t data = RandomData::GetRandomUint16();
    SAFUZZ_LOGI("MessageParcelBasicTypeUtils::WriteRandomUint16 0x%x", data);
    return messageParcel.WriteUint16(data);
}

bool MessageParcelBasicTypeUtils::WriteRandomUint32(MessageParcel& messageParcel)
{
    uint32_t data = RandomData::GetRandomUint32();
    SAFUZZ_LOGI("MessageParcelBasicTypeUtils::WriteRandomUint32 %" PRIu32, data);
    return messageParcel.WriteUint32(data);
}

bool MessageParcelBasicTypeUtils::WriteRandomUint64(MessageParcel& messageParcel)
{
    uint64_t data = RandomData::GetRandomUint64();
    SAFUZZ_LOGI("MessageParcelBasicTypeUtils::WriteRandomUint64 %" PRIu64, data);
    return messageParcel.WriteUint64(data);
}

bool MessageParcelBasicTypeUtils::WriteRandomDouble(MessageParcel& messageParcel)
{
    double data = RandomData::GetRandomDouble();
    SAFUZZ_LOGI("MessageParcelBasicTypeUtils::WriteRandomDouble %f", data);
    return messageParcel.WriteDouble(data);
}

bool MessageParcelBasicTypeUtils::WriteRandomFloat(MessageParcel& messageParcel)
{
    float data = RandomData::GetRandomFloat();
    SAFUZZ_LOGI("MessageParcelBasicTypeUtils::WriteRandomFloat %f", data);
    return messageParcel.WriteFloat(data);
}

bool MessageParcelBasicTypeUtils::WriteRandomString(MessageParcel& messageParcel)
{
    std::string data = RandomData::GetRandomString();
    SAFUZZ_LOGI("MessageParcelBasicTypeUtils::WriteRandomString %s", data.c_str());
    return messageParcel.WriteString(data);
}

bool MessageParcelBasicTypeUtils::WriteRandomCString(MessageParcel& messageParcel)
{
    std::string data = RandomData::GetRandomString();
    SAFUZZ_LOGI("MessageParcelBasicTypeUtils::WriteRandomCString %s", data.c_str());
    return messageParcel.WriteCString(data.c_str());
}

bool MessageParcelBasicTypeUtils::WriteRandomString16(MessageParcel& messageParcel)
{
    std::u16string data = RandomData::GetRandomString16();
    std::string dataStr8 = Str16ToStr8(data);
    SAFUZZ_LOGI("MessageParcelBasicTypeUtils::WriteRandomString16 %s", dataStr8.c_str());
    return messageParcel.WriteString16(data);
}

bool MessageParcelBasicTypeUtils::WriteRandomString8WithLength(MessageParcel& messageParcel)
{
    std::string data = RandomData::GetRandomString();
    SAFUZZ_LOGI("MessageParcelBasicTypeUtils::WriteRandomString8WithLength %s", data.c_str());
    return messageParcel.WriteString8WithLength(data.c_str(), data.length());
}

bool MessageParcelBasicTypeUtils::WriteRandomString16WithLength(MessageParcel& messageParcel)
{
    std::u16string data = RandomData::GetRandomString16();
    std::string dataStr8 = Str16ToStr8(data);
    SAFUZZ_LOGI("MessageParcelBasicTypeUtils::WriteRandomString16WithLength %s", dataStr8.c_str());
    return messageParcel.WriteString16WithLength(data.data(), data.length());
}

bool MessageParcelBasicTypeUtils::WriteRandomPointer(MessageParcel& messageParcel)
{
    uintptr_t data = RandomData::GetRandomPointer();
    SAFUZZ_LOGI("MessageParcelBasicTypeUtils::WriteRandomPointer %" PRIu64, static_cast<uint64_t>(data));
    return messageParcel.WritePointer(data);
}

bool MessageParcelBasicTypeUtils::WriteRandomBuffer(MessageParcel& messageParcel)
{
    std::string data = RandomData::GetRandomBuffer();
    SAFUZZ_LOGI("MessageParcelBasicTypeUtils::WriteRandomBuffer %s", data.c_str());
    return messageParcel.WriteBuffer(data.c_str(), sizeof(data));
}

bool MessageParcelBasicTypeUtils::WriteRandomBoolVector(MessageParcel& messageParcel)
{
    std::vector<bool> data = RandomData::GetRandomBoolVector();
    std::string dataDesc = CommonUtils::PrintVector<bool>(data, CommonUtils::BASIC_PRINTER<bool>);
    SAFUZZ_LOGI("MessageParcelBasicTypeUtils::WriteRandomBoolVector %s", dataDesc.c_str());
    return messageParcel.WriteBoolVector(data);
}

bool MessageParcelBasicTypeUtils::WriteRandomInt8Vector(MessageParcel& messageParcel)
{
    std::vector<int8_t> data = RandomData::GetRandomInt8Vector();
    std::string dataDesc = CommonUtils::PrintVector<int8_t>(data, CommonUtils::BASIC_PRINTER<int8_t>);
    SAFUZZ_LOGI("MessageParcelBasicTypeUtils::WriteRandomInt8Vector %s", dataDesc.c_str());
    return messageParcel.WriteInt8Vector(data);
}

bool MessageParcelBasicTypeUtils::WriteRandomInt16Vector(MessageParcel& messageParcel)
{
    std::vector<int16_t> data = RandomData::GetRandomInt16Vector();
    std::string dataDesc = CommonUtils::PrintVector<int16_t>(data, CommonUtils::BASIC_PRINTER<int16_t>);
    SAFUZZ_LOGI("MessageParcelBasicTypeUtils::WriteRandomInt16Vector %s", dataDesc.c_str());
    return messageParcel.WriteInt16Vector(data);
}

bool MessageParcelBasicTypeUtils::WriteRandomInt32Vector(MessageParcel& messageParcel)
{
    std::vector<int32_t> data = RandomData::GetRandomInt32Vector();
    std::string dataDesc = CommonUtils::PrintVector<int32_t>(data, CommonUtils::BASIC_PRINTER<int32_t>);
    SAFUZZ_LOGI("MessageParcelBasicTypeUtils::WriteRandomInt32Vector %s", dataDesc.c_str());
    return messageParcel.WriteInt32Vector(data);
}

bool MessageParcelBasicTypeUtils::WriteRandomInt64Vector(MessageParcel& messageParcel)
{
    std::vector<int64_t> data = RandomData::GetRandomInt64Vector();
    std::string dataDesc = CommonUtils::PrintVector<int64_t>(data, CommonUtils::BASIC_PRINTER<int64_t>);
    SAFUZZ_LOGI("MessageParcelBasicTypeUtils::WriteRandomInt64Vector %s", dataDesc.c_str());
    return messageParcel.WriteInt64Vector(data);
}

bool MessageParcelBasicTypeUtils::WriteRandomUInt8Vector(MessageParcel& messageParcel)
{
    std::vector<uint8_t> data = RandomData::GetRandomUInt8Vector();
    std::string dataDesc = CommonUtils::PrintVector<uint8_t>(data, CommonUtils::BASIC_PRINTER<uint8_t>);
    SAFUZZ_LOGI("MessageParcelBasicTypeUtils::WriteRandomUInt8Vector %s", dataDesc.c_str());
    return messageParcel.WriteUInt8Vector(data);
}

bool MessageParcelBasicTypeUtils::WriteRandomUInt16Vector(MessageParcel& messageParcel)
{
    std::vector<uint16_t> data = RandomData::GetRandomUInt16Vector();
    std::string dataDesc = CommonUtils::PrintVector<uint16_t>(data, CommonUtils::BASIC_PRINTER<uint16_t>);
    SAFUZZ_LOGI("MessageParcelBasicTypeUtils::WriteRandomUInt16Vector %s", dataDesc.c_str());
    return messageParcel.WriteUInt16Vector(data);
}

bool MessageParcelBasicTypeUtils::WriteRandomUInt32Vector(MessageParcel& messageParcel)
{
    std::vector<int32_t> data = RandomData::GetRandomInt32Vector();
    std::string dataDesc = CommonUtils::PrintVector<int32_t>(data, CommonUtils::BASIC_PRINTER<int32_t>);
    SAFUZZ_LOGI("MessageParcelBasicTypeUtils::WriteRandomInt32Vector %s", dataDesc.c_str());
    return messageParcel.WriteInt32Vector(data);
}

bool MessageParcelBasicTypeUtils::WriteRandomUInt64Vector(MessageParcel& messageParcel)
{
    std::vector<uint64_t> data = RandomData::GetRandomUInt64Vector();
    std::string dataDesc = CommonUtils::PrintVector<uint64_t>(data, CommonUtils::BASIC_PRINTER<uint64_t>);
    SAFUZZ_LOGI("MessageParcelBasicTypeUtils::WriteRandomUInt64Vector %s", dataDesc.c_str());
    return messageParcel.WriteUInt64Vector(data);
}

bool MessageParcelBasicTypeUtils::WriteRandomFloatVector(MessageParcel& messageParcel)
{
    std::vector<float> data = RandomData::GetRandomFloatVector();
    std::string dataDesc = CommonUtils::PrintVector<float>(data, CommonUtils::BASIC_PRINTER<float>);
    SAFUZZ_LOGI("MessageParcelBasicTypeUtils::WriteRandomFloatVector %s", dataDesc.c_str());
    return messageParcel.WriteFloatVector(data);
}

bool MessageParcelBasicTypeUtils::WriteRandomDoubleVector(MessageParcel& messageParcel)
{
    std::vector<double> data = RandomData::GetRandomDoubleVector();
    std::string dataDesc = CommonUtils::PrintVector<double>(data, CommonUtils::BASIC_PRINTER<double>);
    SAFUZZ_LOGI("MessageParcelBasicTypeUtils::WriteRandomDoubleVector %s", dataDesc.c_str());
    return messageParcel.WriteDoubleVector(data);
}

bool MessageParcelBasicTypeUtils::WriteRandomStringVector(MessageParcel& messageParcel)
{
    std::vector<std::string> data = RandomData::GetRandomStringVector();
    std::string dataDesc = CommonUtils::PrintVector<std::string>(data, CommonUtils::STRING_PRINTER);
    SAFUZZ_LOGI("MessageParcelBasicTypeUtils::WriteRandomStringVector %s", dataDesc.c_str());
    return messageParcel.WriteStringVector(data);
}

bool MessageParcelBasicTypeUtils::WriteRandomString16Vector(MessageParcel& messageParcel)
{
    std::vector<std::u16string> data = RandomData::GetRandomString16Vector();
    std::string dataDesc = CommonUtils::PrintVector<std::u16string>(data, CommonUtils::U16STRING_PRINTER);
    SAFUZZ_LOGI("MessageParcelBasicTypeUtils::WriteRandomString16Vector %s", dataDesc.c_str());
    return messageParcel.WriteString16Vector(data);
}
} // namespace Rosen
} // namespace OHOS
