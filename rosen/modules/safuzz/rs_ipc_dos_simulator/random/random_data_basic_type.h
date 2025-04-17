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

#ifndef SAFUZZ_RANDOM_DATA_BASIC_TYPE_H
#define SAFUZZ_RANDOM_DATA_BASIC_TYPE_H

#include <cstdint>
#include <string>
#include <unistd.h>
#include <vector>

namespace OHOS {
namespace Rosen {
class RandomDataBasicType {
public:
    static bool GetRandomBool();
    static int GetRandomInt();
    static int8_t GetRandomInt8();
    static int16_t GetRandomInt16();
    static int32_t GetRandomInt32();
    static int64_t GetRandomInt64();
    static uint8_t GetRandomUint8();
    static uint16_t GetRandomUint16();
    static uint32_t GetRandomUint32();
    static uint64_t GetRandomUint64();
    static size_t GetRandomSize();
    static pid_t GetRandomPid();
    static double GetRandomDouble();
    static float GetRandomFloat();
    static std::string GetRandomString();
    static std::u16string GetRandomString16();
    static uintptr_t GetRandomPointer();
    static std::string GetRandomBuffer();
    static std::vector<bool> GetRandomBoolVector();
    static std::vector<int8_t> GetRandomInt8Vector();
    static std::vector<int16_t> GetRandomInt16Vector();
    static std::vector<int32_t> GetRandomInt32Vector();
    static std::vector<int64_t> GetRandomInt64Vector();
    static std::vector<uint8_t> GetRandomUInt8Vector();
    static std::vector<uint16_t> GetRandomUInt16Vector();
    static std::vector<uint32_t> GetRandomUInt32Vector();
    static std::vector<uint64_t> GetRandomUInt64Vector();
    static std::vector<float> GetRandomFloatVector();
    static std::vector<double> GetRandomDoubleVector();
    static std::vector<std::string> GetRandomStringVector();
    static std::vector<std::u16string> GetRandomString16Vector();
};
} // namespace Rosen
} // namespace OHOS

#endif // SAFUZZ_RANDOM_DATA_BASIC_TYPE_H
