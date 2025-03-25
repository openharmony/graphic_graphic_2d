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

#include "random/random_data_basic_type.h"

#include <cstdlib>
#include <cstring>
#include <sys/types.h>

#include "string_ex.h"

#include "common/rs_common_def.h"
#include "random/random_engine.h"
#include "sandbox_utils.h"

namespace OHOS {
namespace Rosen {
namespace {
NodeId GenerateRandomNodeId()
{
    static pid_t pid = GetRealPid();
    uint32_t randomId = RandomDataBasicType::GetRandomUint32();

    // concat two 32-bit numbers to one 64-bit number
    return ((NodeId)pid << 32) | randomId;
}
}

bool RandomDataBasicType::GetRandomBool()
{
    bool data = 0;
    std::string s_str = RandomEngine::GetRandomBoolean();
    data = atoi(s_str.c_str());
    return data;
}

int RandomDataBasicType::GetRandomInt()
{
    int data = 0;
    std::string s_str = RandomEngine::GetRandomInt();
    if (strncmp(s_str.c_str(), "0x", 2) == 0 || strncmp(s_str.c_str(), "0X", 2) == 0) {
        data = strtol(s_str.c_str(), nullptr, 16);
    } else {
        data = strtol(s_str.c_str(), nullptr, 10);
    }
    return data;
}

int8_t RandomDataBasicType::GetRandomInt8()
{
    int8_t data = 0;
    std::string s_str = RandomEngine::GetRandomByte();
    if (strncmp(s_str.c_str(), "0x", 2) == 0 || strncmp(s_str.c_str(), "0X", 2) == 0) {
        data = strtol(s_str.c_str(), nullptr, 16);
    } else {
        data = strtol(s_str.c_str(), nullptr, 10);
    }
    return data;
}

int16_t RandomDataBasicType::GetRandomInt16()
{
    int16_t data = 0;
    std::string s_str = RandomEngine::GetRandomShort();
    if (strncmp(s_str.c_str(), "0x", 2) == 0 || strncmp(s_str.c_str(), "0X", 2) == 0) {
        data = strtol(s_str.c_str(), nullptr, 16);
    } else {
        data = strtol(s_str.c_str(), nullptr, 10);
    }
    return data;
}

int32_t RandomDataBasicType::GetRandomInt32()
{
    int32_t data = 0;
    std::string s_str = RandomEngine::GetRandomInt();
    if (strncmp(s_str.c_str(), "0x", 2) == 0 || strncmp(s_str.c_str(), "0X", 2) == 0) {
        data = strtol(s_str.c_str(), nullptr, 16);
    } else {
        data = strtol(s_str.c_str(), nullptr, 10);
    }
    return data;
}

int64_t RandomDataBasicType::GetRandomInt64()
{
    int64_t data = 0;
    std::string s_str = RandomEngine::GetRandomLong();
    data = atoll(s_str.c_str());
    return data;
}

uint8_t RandomDataBasicType::GetRandomUint8()
{
    uint8_t data = 0;
    std::string s_str = RandomEngine::GetRandomByte();
    if (strncmp(s_str.c_str(), "0x", 2) == 0 || strncmp(s_str.c_str(), "0X", 2) == 0) {
        data = static_cast<uint8_t>(strtol(s_str.c_str(), nullptr, 16));
    } else {
        data = static_cast<uint8_t>(strtol(s_str.c_str(), nullptr, 10));
    }
    return data;
}

uint16_t RandomDataBasicType::GetRandomUint16()
{
    uint16_t data = 0;
    std::string s_str = RandomEngine::GetRandomShort();
    if (strncmp(s_str.c_str(), "0x", 2) == 0 || strncmp(s_str.c_str(), "0X", 2) == 0) {
        data = static_cast<uint16_t>(strtol(s_str.c_str(), nullptr, 16));
    } else {
        data = static_cast<uint16_t>(strtol(s_str.c_str(), nullptr, 10));
    }
    return data;
}

uint32_t RandomDataBasicType::GetRandomUint32()
{
    uint32_t data = 0;
    std::string s_str = RandomEngine::GetRandomInt();
    if (strncmp(s_str.c_str(), "0x", 2) == 0 || strncmp(s_str.c_str(), "0X", 2) == 0) {
        data = static_cast<uint32_t>(strtol(s_str.c_str(), nullptr, 16));
    } else {
        data = static_cast<uint32_t>(strtol(s_str.c_str(), nullptr, 10));
    }
    return data;
}

uint64_t RandomDataBasicType::GetRandomUint64()
{
    static constexpr float GENERATE_NODEID_PROBABILITY = 0.25;
    if (RandomEngine::ChooseByProbability(GENERATE_NODEID_PROBABILITY)) {
        return GenerateRandomNodeId();
    }
    uint64_t data = 0;
    std::string s_str = RandomEngine::GetRandomLong();
    data = static_cast<uint64_t>(atoll(s_str.c_str()));
    return data;
}

size_t RandomDataBasicType::GetRandomSize()
{
    size_t data = 0;
    std::string s_str = RandomEngine::GetRandomLong();
    data = static_cast<size_t>(atoll(s_str.c_str()));
    return data;
}

pid_t RandomDataBasicType::GetRandomPid()
{
    pid_t data = 0;
    std::string s_str = RandomEngine::GetRandomInt();
    if (strncmp(s_str.c_str(), "0x", 2) == 0 || strncmp(s_str.c_str(), "0X", 2) == 0) {
        data = strtol(s_str.c_str(), nullptr, 16);
    } else {
        data = strtol(s_str.c_str(), nullptr, 10);
    }
    return data;
}

double RandomDataBasicType::GetRandomDouble()
{
    double data = 0;
    std::string s_str = RandomEngine::GetRandomDouble();
    data = std::stod(s_str.c_str());
    return data;
}

float RandomDataBasicType::GetRandomFloat()
{
    float data = 0;
    std::string s_str = RandomEngine::GetRandomFloat();
    data = std::stof(s_str.c_str());
    return data;
}

std::string RandomDataBasicType::GetRandomString()
{
    std::string data = RandomEngine::GetRandomString();
    return data;
}

std::u16string RandomDataBasicType::GetRandomString16()
{
    std::string s_str = RandomEngine::GetRandomString16();
    std::u16string data = Str8ToStr16(s_str);
    return data;
}

uintptr_t RandomDataBasicType::GetRandomPointer()
{
    std::string s_str = RandomEngine::GetRandomPointer();
    uintptr_t data = (uintptr_t)(s_str.c_str());
    return data;
}

std::string RandomDataBasicType::GetRandomBuffer()
{
    std::string data = RandomEngine::GetRandomBuffer();
    return data;
}

std::vector<bool> RandomDataBasicType::GetRandomBoolVector()
{
    std::vector<bool> data;
    std::vector<std::string> recvlist = RandomEngine::GetRandomBoolVector();
    for (size_t i = 0; i < recvlist.size(); i++) {
        data.push_back(atoi(recvlist[i].c_str()));
    }
    return data;
}

std::vector<int8_t> RandomDataBasicType::GetRandomInt8Vector()
{
    std::vector<int8_t> data;
    std::vector<std::string> recvlist = RandomEngine::GetRandomByteVector();
    for (size_t i = 0; i < recvlist.size(); i++) {
        data.push_back(atoi(recvlist[i].c_str()));
    }
    return data;
}

std::vector<int16_t> RandomDataBasicType::GetRandomInt16Vector()
{
    std::vector<int16_t> data;
    std::vector<std::string> recvlist = RandomEngine::GetRandomShortVector();
    for (size_t i = 0; i < recvlist.size(); i++) {
        data.push_back(atoi(recvlist[i].c_str()));
    }
    return data;
}

std::vector<int32_t> RandomDataBasicType::GetRandomInt32Vector()
{
    std::vector<int32_t> data;
    std::vector<std::string> recvlist = RandomEngine::GetRandomIntVector();
    for (size_t i = 0; i < recvlist.size(); i++) {
        data.push_back(atoi(recvlist[i].c_str()));
    }
    return data;
}

std::vector<int64_t> RandomDataBasicType::GetRandomInt64Vector()
{
    std::vector<int64_t> data;
    std::vector<std::string> recvlist = RandomEngine::GetRandomLongVector();
    for (size_t i = 0; i < recvlist.size(); i++) {
        data.push_back(atoll(recvlist[i].c_str()));
    }
    return data;
}

std::vector<uint8_t> RandomDataBasicType::GetRandomUInt8Vector()
{
    std::vector<uint8_t> data;
    std::vector<std::string> recvlist = RandomEngine::GetRandomByteVector();
    for (size_t i = 0; i < recvlist.size(); i++) {
        data.push_back(static_cast<uint8_t>(atoi(recvlist[i].c_str())));
    }
    return data;
}

std::vector<uint16_t> RandomDataBasicType::GetRandomUInt16Vector()
{
    std::vector<uint16_t> data;
    std::vector<std::string> recvlist = RandomEngine::GetRandomShortVector();
    for (size_t i = 0; i < recvlist.size(); i++) {
        data.push_back(static_cast<uint16_t>(atoi(recvlist[i].c_str())));
    }
    return data;
}

std::vector<uint32_t> RandomDataBasicType::GetRandomUInt32Vector()
{
    std::vector<uint32_t> data;
    std::vector<std::string> recvlist = RandomEngine::GetRandomIntVector();
    for (size_t i = 0; i < recvlist.size(); i++) {
        data.push_back(static_cast<uint32_t>(atoi(recvlist[i].c_str())));
    }
    return data;
}

std::vector<uint64_t> RandomDataBasicType::GetRandomUInt64Vector()
{
    std::vector<uint64_t> data;
    std::vector<std::string> recvlist = RandomEngine::GetRandomLongVector();
    for (size_t i = 0; i < recvlist.size(); i++) {
        data.push_back(static_cast<uint64_t>(atoll(recvlist[i].c_str())));
    }
    return data;
}

std::vector<float> RandomDataBasicType::GetRandomFloatVector()
{
    std::vector<float> data;
    std::vector<std::string> recvlist = RandomEngine::GetRandomFloatVector();
    for (size_t i = 0; i < recvlist.size(); i++) {
        data.push_back(std::stof(recvlist[i].c_str()));
    }
    return data;
}

std::vector<double> RandomDataBasicType::GetRandomDoubleVector()
{
    std::vector<double> data;
    std::vector<std::string> recvlist = RandomEngine::GetRandomDoubleVector();
    for (size_t i = 0; i < recvlist.size(); i++) {
        data.push_back(std::stod(recvlist[i].c_str()));
    }
    return data;
}

std::vector<std::string> RandomDataBasicType::GetRandomStringVector()
{
    std::vector<std::string> data = RandomEngine::GetRandomStringVector();
    return data;
}

std::vector<std::u16string> RandomDataBasicType::GetRandomString16Vector()
{
    std::vector<std::u16string> data;
    std::vector<std::string> recvlist = RandomEngine::GetRandomString16Vector();
    for (size_t i = 0; i < recvlist.size(); i++) {
        data.push_back(Str8ToStr16(recvlist[i].c_str()));
    }
    return data;
}
} // namespace Rosen
} // namespace OHOS
