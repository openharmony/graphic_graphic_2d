/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
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

#include "gtest/gtest.h"
#include "rs_profiler_archive.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Rosen {
class RSProfilerArchiveTest : public testing::Test {
public:
    static void SetUpTestCase() {};
    static void TearDownTestCase() {};
    void SetUp() override {};
    void TearDown() override {};
};

/*
 * @tc.name: SerializeFloatTest
 * @tc.desc: Test Archive float serialization
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSProfilerArchiveTest, SerializeFloatTest, testing::ext::TestSize.Level1)
{
    std::vector<char> buf;
    DataWriter writer(buf);
    DataReader reader(buf);
    float initial = 27.0f;
    float result = 0.0f;
    writer.Serialize(initial);
    reader.Serialize(result);
    EXPECT_EQ(initial, result);
}

/*
 * @tc.name: SerializeDoubleTest
 * @tc.desc: Test Archive double serialization
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSProfilerArchiveTest, SerializeDoubleTest, testing::ext::TestSize.Level1)
{
    std::vector<char> buf;
    DataWriter writer(buf);
    DataReader reader(buf);
    double initial = 87.0;
    double result = 0.0;
    writer.Serialize(initial);
    reader.Serialize(result);
    EXPECT_EQ(initial, result);
}

/*
 * @tc.name: SerializeInt32Test
 * @tc.desc: Test Archive int32_t serialization
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSProfilerArchiveTest, SerializeInt32Test, testing::ext::TestSize.Level1)
{
    std::vector<char> buf;
    DataWriter writer(buf);
    DataReader reader(buf);
    int32_t initial = 564;
    int32_t result = 0;
    writer.Serialize(initial);
    reader.Serialize(result);
    EXPECT_EQ(initial, result);
}

/*
 * @tc.name: SerializeUint32Test
 * @tc.desc: Test Archive uint32_t serialization
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSProfilerArchiveTest, SerializeUint32Test, testing::ext::TestSize.Level1)
{
    std::vector<char> buf;
    DataWriter writer(buf);
    DataReader reader(buf);
    uint32_t initial = 3644;
    uint32_t result = 0;
    writer.Serialize(initial);
    reader.Serialize(result);
    EXPECT_EQ(initial, result);
}

/*
 * @tc.name: SerializeSizeTTest
 * @tc.desc: Test Archive size_t serialization
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSProfilerArchiveTest, SerializeSizeTTest, testing::ext::TestSize.Level1)
{
    std::vector<char> buf;
    DataWriter writer(buf);
    DataReader reader(buf);
    size_t initial = 1056;
    size_t result = 0;
    writer.Serialize(initial);
    reader.Serialize(result);
    EXPECT_EQ(initial, result);
}

/*
 * @tc.name: SerializeStringTest
 * @tc.desc: Test Archive std::string serialization
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSProfilerArchiveTest, SerializeStringTest, testing::ext::TestSize.Level1)
{
    std::vector<char> buf;
    DataWriter writer(buf);
    DataReader reader(buf);
    std::string initial = "sample text";
    std::string result = "";
    writer.Serialize(initial);
    reader.Serialize(result);
    EXPECT_EQ(initial, result);
}

struct SomeStruct {
    static void Serialize(Archive& archive, SomeStruct& value)
    {
        value.Serialize(archive);
    }

    void Serialize(Archive& archive)
    {
        archive.Serialize(intData);
        archive.Serialize(stringData);
    }

    bool operator==(const SomeStruct& other) const
    {
        return intData == other.intData && stringData == other.stringData;
    }

    int intData = 0;
    std::string stringData = "";
};

/*
 * @tc.name: SerializeVectorNonFlatTest
 * @tc.desc: Test Archive std::vector of user types serialization
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSProfilerArchiveTest, SerializeVectorNonFlatTest, testing::ext::TestSize.Level1)
{
    std::vector<char> buf;
    DataWriter writer(buf);
    DataReader reader(buf);
    std::vector<SomeStruct> initial = { { 892, "qwe" }, { 457, "asd" }, { 2065, "zxc" } };
    std::vector<SomeStruct> result = {};
    writer.SerializeNonFlat(initial);
    reader.SerializeNonFlat(result);
    EXPECT_EQ(initial, result);
}

/*
 * @tc.name: SerializeVectorCustomSerializerTest
 * @tc.desc: Test Archive std::vector with custom serializer serialization
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSProfilerArchiveTest, SerializeVectorCustomSerializerTest, testing::ext::TestSize.Level1)
{
    std::vector<char> buf;
    DataWriter writer(buf);
    DataReader reader(buf);
    std::vector<SomeStruct> initial = { { -546, "ioreh" }, { -135, "gbseg" }, { 968, "gvpwg" } };
    std::vector<SomeStruct> result = {};
    writer.Serialize(initial, SomeStruct::Serialize);
    reader.Serialize(result, SomeStruct::Serialize);
    EXPECT_EQ(initial, result);
}

} // namespace OHOS::Rosen