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

#include "random/random_engine.h"

#include <algorithm>
#include <cstdlib>
#include <cstring>
#include <random>

namespace OHOS {
namespace Rosen {
namespace {
constexpr int LIST_SIZE_MAX = 1000;

const std::vector<std::string> BOOLEAN_LIST = {
    "0", "1", "0", "1", "0", "1", "0", "1", "0", "1", "0", "1", "0", "1" };

const std::vector<std::string> INT_LIST = {
    "1",
    "0",
    "-1",
    "7",
    "-7",
    "8",
    "-8",
    "255",
    "-256",
    "-255",
    "256",
    "65535",
    "-65535",
    "65536",
    "-65536",
    "2147483647",
    "-2147483647",
    "2147483648",
    "-2147483648",
    "4294967295",
    "-4294967296",
    "9223372036854775807",
    "-9223372036854775807",
    "9223372036854775808",
    "-9223372036854775808" };

const std::vector<std::string> DOUBLE_LIST = {
    "0.0", "-0.0", "-1.0", "1.0", "1.7976931348623158E308", "-1.7976931348623158E308", "255.0", "256.0",
    "-255.0", "-256.0", "3.402823466E38", "-3.402823466E38" };

const std::vector<std::string> STRING_LIST = {
    ".", "'.'", "a", "null", "'..'",
    "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa"
    "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa"
    "aaaaaaaaaaaaaaaaaaaaaaaaaaaa",
    "aaaaaaaaaaaaaaaaaaaaa",
    "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa"
    "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa"
    "aaaaaaaaaaaaaaaaaaaaaaaaaaa",
    "0", "133254121245", "-1", "10/10/2017", "Hello", "</end>", "func()", "owner", "abcdefefdfdfd",
    "eeeeeeeeeeee,", "............................................................", "{aaa:bbb}", "{", "[]",
    "....................", "*", "********************" };

const std::vector<std::string> LONG_LIST = {
    "1",
    "0",
    "-1",
    "7",
    "-7",
    "8",
    "-8",
    "255",
    "-256",
    "-255",
    "256",
    "65535",
    "-65535",
    "65536",
    "-65536",
    "2147483647",
    "-2147483647",
    "2147483648",
    "-2147483648",
    "4294967295",
    "-4294967296",
    "9223372036854775807",
    "-9223372036854775807",
    "9223372036854775808",
    "-9223372036854775808" };

const std::vector<std::string> FLOAT_LIST = {
    "0.0", "-0.0", "-1.0", "1.0", "255.0", "256.0", "-255.0", "-256.0", "3.402823466E38",
    "-3.402823466E38", "3.402823467E38", "-3.402823467E38" };

const std::vector<std::string> BYTE_LIST = {
    "0", "1", "-1", "-128", "127", "-129", "128", "56", "2", "3", "4", "5", "10", "100", "255", "256" };

const std::vector<std::string> SHORT_LIST = {
    "0", "1", "-1", "-32768", "32767", "-32770", "32770", "56",
    "2", "3", "4", "5", "10", "100", "1000", "65535", "65536" };
}

std::string RandomEngine::GetRandomInt()
{
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> distrib(0, LIST_SIZE_MAX);

    int randomNumber = distrib(gen);
    return INT_LIST[randomNumber % INT_LIST.size()];
}

std::string RandomEngine::GetRandomString()
{
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> distrib(0, LIST_SIZE_MAX);

    int randomNumber = distrib(gen);
    return STRING_LIST[randomNumber % STRING_LIST.size()];
}

std::string RandomEngine::GetRandomLong()
{
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> distrib(0, LIST_SIZE_MAX);

    int randomNumber = distrib(gen);
    return LONG_LIST[randomNumber % LONG_LIST.size()];
}

std::string RandomEngine::GetRandomFloat()
{
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> distrib(0, LIST_SIZE_MAX);

    int randomNumber = distrib(gen);
    return FLOAT_LIST[randomNumber % FLOAT_LIST.size()];
}

std::string RandomEngine::GetRandomDouble()
{
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> distrib(0, LIST_SIZE_MAX);

    int randomNumber = distrib(gen);
    return DOUBLE_LIST[randomNumber % DOUBLE_LIST.size()];
}

std::string RandomEngine::GetRandomBoolean()
{
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> distrib(0, LIST_SIZE_MAX);

    int randomNumber = distrib(gen);
    return BOOLEAN_LIST[randomNumber % BOOLEAN_LIST.size()];
}

std::string RandomEngine::GetRandomBuffer()
{
    return "";
}

std::string RandomEngine::GetRandomByte()
{
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> distrib(0, LIST_SIZE_MAX);

    int randomNumber = distrib(gen);
    return BYTE_LIST[randomNumber % BYTE_LIST.size()];
}

std::string RandomEngine::GetRandomShort()
{
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> distrib(0, LIST_SIZE_MAX);

    int randomNumber = distrib(gen);
    return SHORT_LIST[randomNumber % SHORT_LIST.size()];
}

std::string RandomEngine::GetRandomPointer()
{
    return "";
}

std::string RandomEngine::GetRandomCString()
{
    return GetRandomString();
}

std::string RandomEngine::GetRandomString16()
{
    return GetRandomString();
}

std::vector<std::string> RandomEngine::GetRandomBoolVector()
{
    std::vector<std::string> listBool;
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> distrib(0, LIST_SIZE_MAX);

    int vectorLength = GetRandomVectorLength();
    for (int i = 0; i < vectorLength; i++) {
        int randomNumber = distrib(gen);
        listBool.push_back(BOOLEAN_LIST[randomNumber % BOOLEAN_LIST.size()]);
    }
    return listBool;
}

std::vector<std::string> RandomEngine::GetRandomByteVector()
{
    std::vector<std::string> listByte;
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> distrib(0, LIST_SIZE_MAX);

    int vectorLength = GetRandomVectorLength();
    for (int i = 0; i < vectorLength; i++) {
        int randomNumber = distrib(gen);
        listByte.push_back(BYTE_LIST[randomNumber % BYTE_LIST.size()]);
    }
    return listByte;
}

std::vector<std::string> RandomEngine::GetRandomShortVector()
{
    std::vector<std::string> listShort;
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> distrib(0, LIST_SIZE_MAX);

    int vectorLength = GetRandomVectorLength();
    for (int i = 0; i < vectorLength; i++) {
        int randomNumber = distrib(gen);
        listShort.push_back(SHORT_LIST[randomNumber % SHORT_LIST.size()]);
    }
    return listShort;
}

std::vector<std::string> RandomEngine::GetRandomIntVector()
{
    std::vector<std::string> listInt;
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> distrib(0, LIST_SIZE_MAX);

    int vectorLength = GetRandomVectorLength();
    for (int i = 0; i < vectorLength; i++) {
        int randomNumber = distrib(gen);
        listInt.push_back(INT_LIST[randomNumber % INT_LIST.size()]);
    }
    return listInt;
}

std::vector<std::string> RandomEngine::GetRandomLongVector()
{
    std::vector<std::string> listLong;
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> distrib(0, LIST_SIZE_MAX);

    int vectorLength = GetRandomVectorLength();
    for (int i = 0; i < vectorLength; i++) {
        int randomNumber = distrib(gen);
        listLong.push_back(LONG_LIST[randomNumber % LONG_LIST.size()]);
    }
    return listLong;
}

std::vector<std::string> RandomEngine::GetRandomFloatVector()
{
    std::vector<std::string> listFloat;
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> distrib(0, LIST_SIZE_MAX);

    int vectorLength = GetRandomVectorLength();
    for (int i = 0; i < vectorLength; i++) {
        int randomNumber = distrib(gen);
        listFloat.push_back(FLOAT_LIST[randomNumber % FLOAT_LIST.size()]);
    }
    return listFloat;
}

std::vector<std::string> RandomEngine::GetRandomDoubleVector()
{
    std::vector<std::string> listDouble;
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> distrib(0, LIST_SIZE_MAX);

    int vectorLength = GetRandomVectorLength();
    for (int i = 0; i < vectorLength; i++) {
        int randomNumber = distrib(gen);
        listDouble.push_back(DOUBLE_LIST[randomNumber % DOUBLE_LIST.size()]);
    }
    return listDouble;
}

std::vector<std::string> RandomEngine::GetRandomStringVector()
{
    std::vector<std::string> listString;
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> distrib(0, LIST_SIZE_MAX);

    int vectorLength = GetRandomVectorLength();
    for (int i = 0; i < vectorLength; i++) {
        int randomNumber = distrib(gen);
        listString.push_back(STRING_LIST[randomNumber % STRING_LIST.size()]);
    }
    return listString;
}

std::vector<std::string> RandomEngine::GetRandomString16Vector()
{
    return GetRandomStringVector();
}

int RandomEngine::GetRandomVectorLength()
{
    std::random_device rd;
    std::mt19937 gen(rd());

    std::uniform_int_distribution<> uniformIntDistrib(0, VECTOR_LENGTH_MAX);
    std::poisson_distribution<> poissonDistrib(VECTOR_LENGTH_POISSON_DISTRIB_PARAM);
    std::uniform_real_distribution<> uniformRealDistrib(0, 1);

    int vectorLength = 0;
    if (uniformRealDistrib(gen) < VECTOR_LENGTH_USE_POISSON_DISTRIB_PROBABILITY) {
        vectorLength = poissonDistrib(gen);
    } else {
        vectorLength = uniformIntDistrib(gen);
    }
    if (vectorLength > VECTOR_LENGTH_MAX) {
        vectorLength = VECTOR_LENGTH_POISSON_DISTRIB_PARAM;
    }
    return vectorLength;
}

int RandomEngine::GetRandomSmallVectorLength()
{
    return GetRandomIndex(SMALL_VECTOR_LENGTH_MAX);
}

int RandomEngine::GetRandomMiddleVectorLength()
{
    return GetRandomIndex(MIDDLE_VECTOR_LENGTH_MAX);
}

int RandomEngine::GetRandomIndex(int indexMax, int indexMin)
{
    if (indexMax <= indexMin) {
        return indexMax;
    }
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> distrib(indexMin, indexMax);
    return static_cast<int>(distrib(gen));
}

bool RandomEngine::ChooseByProbability(float probability)
{
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<> uniformRealDistrib(0, 1);
    probability = std::clamp<float>(probability, 0.f, 1.f);
    return uniformRealDistrib(gen) < probability;
}
} // namespace Rosen
} // namespace OHOS
