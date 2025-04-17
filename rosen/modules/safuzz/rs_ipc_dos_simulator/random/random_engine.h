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

#ifndef SAFUZZ_RANDOM_ENGINE_H
#define SAFUZZ_RANDOM_ENGINE_H

#include <string>
#include <vector>

namespace OHOS {
namespace Rosen {
class RandomEngine {
public:
    static std::string GetRandomInt();
    static std::string GetRandomString();
    static std::string GetRandomLong();
    static std::string GetRandomFloat();
    static std::string GetRandomDouble();
    static std::string GetRandomBoolean();
    static std::string GetRandomBuffer();
    static std::string GetRandomByte();
    static std::string GetRandomShort();
    static std::string GetRandomPointer();
    static std::string GetRandomCString();
    static std::string GetRandomString16();
    static std::vector<std::string> GetRandomBoolVector();
    static std::vector<std::string> GetRandomByteVector();
    static std::vector<std::string> GetRandomShortVector();
    static std::vector<std::string> GetRandomIntVector();
    static std::vector<std::string> GetRandomLongVector();
    static std::vector<std::string> GetRandomFloatVector();
    static std::vector<std::string> GetRandomDoubleVector();
    static std::vector<std::string> GetRandomStringVector();
    static std::vector<std::string> GetRandomString16Vector();

    static int GetRandomVectorLength();
    static int GetRandomSmallVectorLength();
    static int GetRandomMiddleVectorLength();
    static int GetRandomIndex(int indexMax, int indexMin = 0);
    static bool ChooseByProbability(float probability);

private:
    static constexpr int SMALL_VECTOR_LENGTH_MAX = 32;
    static constexpr int MIDDLE_VECTOR_LENGTH_MAX = 1000;
    static constexpr int VECTOR_LENGTH_MAX = 1000000;
    static constexpr int VECTOR_LENGTH_POISSON_DISTRIB_PARAM = 100;
    static constexpr float VECTOR_LENGTH_USE_POISSON_DISTRIB_PROBABILITY = 0.9;
};
} // namespace Rosen
} // namespace OHOS

#endif // SAFUZZ_RANDOM_ENGINE_H
