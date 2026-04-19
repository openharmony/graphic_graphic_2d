/*
 * Copyright (c) 2026 Huawei Device Co., Ltd.
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
class RenderProfilerArchiveTest : public testing::Test {
public:
    static void SetUpTestCase() {};
    static void TearDownTestCase() {};
    void SetUp() override {};
    void TearDown() override {};
};

/*
 * @tc.name: SerializeCharTest01
 * @tc.desc: Test Archive Char serialization
 * @tc.type: FUNC
 * @tc.require: 22738
 */
HWTEST_F(RenderProfilerArchiveTest, SerializeCharTest01, TestSize.Level1)
{
    std::vector<char> buffer;
    DataWriter writer(buffer);
    DataReader reader(buffer);
    char initial = 'A';
    char result = 'a';
    writer.Serialize(initial);
    reader.Serialize(result);
    EXPECT_EQ(initial, result);
}

/*
 * @tc.name: SerializeDoubleTest01
 * @tc.desc: Test Archive double serialization
 * @tc.type: FUNC
 * @tc.require: 22738
 */
HWTEST_F(RenderProfilerArchiveTest, SerializeDoubleTest01, TestSize.Level1)
{
    std::vector<char> buffer;
    DataWriter writer(buffer);
    DataReader reader(buffer);
    double initial = 87.0;
    double result = 0.0;
    writer.Serialize(initial);
    reader.Serialize(result);
    EXPECT_EQ(initial, result);
}

/*
 * @tc.name: SerializeInt8Test01
 * @tc.desc: Test Archive int8_t serialization
 * @tc.type: FUNC
 * @tc.require: 22738
 */
HWTEST_F(RenderProfilerArchiveTest, SerializeInt8Test01, TestSize.Level1)
{
    std::vector<char> buffer;
    DataWriter writer(buffer);
    DataReader reader(buffer);
    int8_t initial = -87;
    int8_t result = 0;
    writer.Serialize(initial);
    reader.Serialize(result);
    EXPECT_EQ(initial, result);
}

/*
 * @tc.name: SerializeFloatTest01
 * @tc.desc: Test Archive float serialization
 * @tc.type: FUNC
 * @tc.require: 22738
 */
HWTEST_F(RenderProfilerArchiveTest, SerializeFloatTest01, TestSize.Level1)
{
    std::vector<char> buffer;
    DataWriter writer(buffer);
    DataReader reader(buffer);
    float initial = 27.0f;
    float result = 0.0f;
    writer.Serialize(initial);
    reader.Serialize(result);
    EXPECT_EQ(initial, result);
}

/*
 * @tc.name: SerializeInt32Test01
 * @tc.desc: Test Archive int32_t serialization
 * @tc.type: FUNC
 * @tc.require: 22738
 */
HWTEST_F(RenderProfilerArchiveTest, SerializeInt32Test01, TestSize.Level1)
{
    std::vector<char> buffer;
    DataWriter writer(buffer);
    DataReader reader(buffer);
    int32_t initial = 564;
    int32_t result = 0;
    writer.Serialize(initial);
    reader.Serialize(result);
    EXPECT_EQ(initial, result);
}

/*
 * @tc.name: SerializeSizeTTest01
 * @tc.desc: Test Archive size_t serialization
 * @tc.type: FUNC
 * @tc.require: 22738
 */
HWTEST_F(RenderProfilerArchiveTest, SerializeSizeTTest01, TestSize.Level1)
{
    std::vector<char> buffer;
    DataWriter writer(buffer);
    DataReader reader(buffer);
    size_t initial = 1056;
    size_t result = 0;
    writer.Serialize(initial);
    reader.Serialize(result);
    EXPECT_EQ(initial, result);
}

/*
 * @tc.name: SerializeStringTest01
 * @tc.desc: Test Archive std::string serialization
 * @tc.type: FUNC
 * @tc.require: 22738
 */
HWTEST_F(RenderProfilerArchiveTest, SerializeStringTest01, TestSize.Level1)
{
    std::vector<char> buffer;
    DataWriter writer(buffer);
    DataReader reader(buffer);
    std::string initial = "sample text";
    std::string result = "";
    writer.Serialize(initial);
    reader.Serialize(result);
    EXPECT_EQ(initial, result);
}

/*
 * @tc.name: SerializeUInt8Test01
 * @tc.desc: Test Archive uint8_t serialization
 * @tc.type: FUNC
 * @tc.require: 22738
 */
HWTEST_F(RenderProfilerArchiveTest, SerializeUInt8Test01, TestSize.Level1)
{
    std::vector<char> buffer;
    DataWriter writer(buffer);
    DataReader reader(buffer);
    uint8_t initial = 87;
    uint8_t result = 0;
    writer.Serialize(initial);
    reader.Serialize(result);
    EXPECT_EQ(initial, result);
}

/*
 * @tc.name: SerializeUint32Test01
 * @tc.desc: Test Archive uint32_t serialization
 * @tc.type: FUNC
 * @tc.require: 22738
 */
HWTEST_F(RenderProfilerArchiveTest, SerializeUint32Test01, TestSize.Level1)
{
    std::vector<char> buffer;
    DataWriter writer(buffer);
    DataReader reader(buffer);
    uint32_t initial = 3644;
    uint32_t result = 0;
    writer.Serialize(initial);
    reader.Serialize(result);
    EXPECT_EQ(initial, result);
}

struct RenderRenderSomeStruct {
    static void Serialize(Archive& archive, RenderSomeStruct& value)
    {
        value.Serialize(archive);
    }

    void Serialize(Archive& archive)
    {
        archive.Serialize(intData);
        archive.Serialize(stringData);
    }

    bool operator==(const RenderSomeStruct& other) const
    {
        return intData == other.intData && stringData == other.stringData;
    }

    int intData = 0;
    std::string stringData = "";
};

/*
 * @tc.name: SerializeInt1601
 * @tc.desc: Test SerializeInt
 * @tc.type: FUNC
 * @tc.require: 22738
 */
HWTEST_F(RenderProfilerArchiveTest, SerializeInt1601, TestSize.Level1)
{
    std::vector<char> buffer;
    DataWriter writer(buffer);
    DataReader reader(buffer);
    int16_t v1 = 0xb00d;
    uint16_t v2 = 0xdeec;

    writer.Serialize(v1);
    writer.Serialize(v2);
    int16_t res1 { 0 };
    uint16_t res2 { 0 };
    reader.Serialize(res1);
    reader.Serialize(res2);
    EXPECT_EQ(v1, res1);
    EXPECT_EQ(v2, res2);
}

/*
 * @tc.name: SerializeVoid01
 * @tc.desc: Test SerializeVoid
 * @tc.type: FUNC
 * @tc.require: 22738
 */
HWTEST_F(RenderProfilerArchiveTest, SerializeVoid01, TestSize.Level1)
{
    std::vector<char> buffer;
    DataWriter writer(buffer);
    EXPECT_TRUE(buffer.empty());
    writer.Serialize(nullptr, 100);
    EXPECT_TRUE(buffer.empty());

    int placeholder;
    void* addr = &placeholder;
    writer.Serialize(addr, 0);
    EXPECT_TRUE(buffer.empty());

    writer.Serialize(nullptr, 0); // both
    EXPECT_TRUE(buffer.empty());
}

/*
 * @tc.name: SerializeVectorNonFlatTest01
 * @tc.desc: Test Archive std::vector of user types serialization
 * @tc.type: FUNC
 * @tc.require: 22738
 */
HWTEST_F(RenderProfilerArchiveTest, SerializeVectorNonFlatTest01, TestSize.Level1)
{
    std::vector<char> buffer;
    DataWriter writer(buffer);
    DataReader reader(buffer);
    std::vector<RenderSomeStruct> initial = { { 892, "qwe" }, { 457, "asd" }, { 2065, "zxc" } };
    std::vector<RenderSomeStruct> result = {};
    writer.SerializeNonFlat(initial);
    reader.SerializeNonFlat(result);
    EXPECT_EQ(initial, result);
}

/*
 * @tc.name: SerializeVectorCustomSerializerTest01
 * @tc.desc: Test Archive std::vector with custom serializer serialization
 * @tc.type: FUNC
 * @tc.require: 22738
 */
HWTEST_F(RenderProfilerArchiveTest, SerializeVectorCustomSerializerTest01, TestSize.Level1)
{
    std::vector<char> buffer;
    DataWriter writer(buffer);
    DataReader reader(buffer);
    std::vector<RenderSomeStruct> initial = { { -546, "ioreh" }, { -135, "gbseg" }, { 968, "gvpwg" } };
    std::vector<RenderSomeStruct> result = {};
    writer.Serialize(initial, RenderSomeStruct::Serialize);
    reader.Serialize(result, RenderSomeStruct::Serialize);
    EXPECT_EQ(initial, result);
}
} // namespace OHOS::Rosen