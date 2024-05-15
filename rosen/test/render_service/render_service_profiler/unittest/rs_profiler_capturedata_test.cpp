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
#include "rs_profiler_capturedata.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Rosen {
class RSProfilerCaptureDataTest : public testing::Test {
public:
    static void SetUpTestCase() {};
    static void TearDownTestCase() {};
    void SetUp() override {};
    void TearDown() override {};
};

/*
 * @tc.name: TimeTest
 * @tc.desc: Test CaptureData time read/write
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSProfilerCaptureDataTest, TimeTest, testing::ext::TestSize.Level1)
{
    const float initial = 32.45f;
    RSCaptureData data;
    data.SetTime(initial);
    EXPECT_EQ(initial, data.GetTime());
}

/*
 * @tc.name: StringPropertyTest
 * @tc.desc: Test CaptureData string property read/write
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSProfilerCaptureDataTest, StringPropertyTest, testing::ext::TestSize.Level1)
{
    const std::string initial = "hello";
    const std::string name = "String";
    RSCaptureData data;
    data.SetProperty(name, initial);
    EXPECT_EQ(initial, data.GetProperty(name));
}

/*
 * @tc.name: FloatPropertyTest
 * @tc.desc: Test CaptureData float property read/write
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSProfilerCaptureDataTest, FloatPropertyTest, testing::ext::TestSize.Level1)
{
    const float initial = 435.542f;
    const std::string name = "Float";
    RSCaptureData data;
    data.SetProperty(name, initial);
    EXPECT_EQ(initial, data.GetPropertyFloat(name));
}

/*
 * @tc.name: DoublePropertyTest
 * @tc.desc: Test CaptureData float property read/write
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSProfilerCaptureDataTest, DoublePropertyTest, testing::ext::TestSize.Level1)
{
    const double initial = 98.327;
    const std::string name = "Double";
    RSCaptureData data;
    data.SetProperty(name, initial);
    EXPECT_EQ(initial, data.GetPropertyDouble(name));
}

/*
 * @tc.name: Int8PropertyTest
 * @tc.desc: Test CaptureData float property read/write
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSProfilerCaptureDataTest, Int8PropertyTest, testing::ext::TestSize.Level1)
{
    const int8_t initial = -7;
    const std::string name = "Int8";
    RSCaptureData data;
    data.SetProperty(name, initial);
    EXPECT_EQ(initial, data.GetPropertyInt8(name));
}

/*
 * @tc.name: Uint8PropertyTest
 * @tc.desc: Test CaptureData float property read/write
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSProfilerCaptureDataTest, Uint8PropertyTest, testing::ext::TestSize.Level1)
{
    const uint8_t initial = 86;
    const std::string name = "Uint8";
    RSCaptureData data;
    data.SetProperty(name, initial);
    EXPECT_EQ(initial, data.GetPropertyUint8(name));
}

/*
 * @tc.name: Int16PropertyTest
 * @tc.desc: Test CaptureData float property read/write
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSProfilerCaptureDataTest, Int16PropertyTest, testing::ext::TestSize.Level1)
{
    const int16_t initial = 2789;
    const std::string name = "Int16";
    RSCaptureData data;
    data.SetProperty(name, initial);
    EXPECT_EQ(initial, data.GetPropertyInt16(name));
}

/*
 * @tc.name: Uint16PropertyTest
 * @tc.desc: Test CaptureData float property read/write
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSProfilerCaptureDataTest, Uint16PropertyTest, testing::ext::TestSize.Level1)
{
    const uint16_t initial = 5342;
    const std::string name = "Uint16";
    RSCaptureData data;
    data.SetProperty(name, initial);
    EXPECT_EQ(initial, data.GetPropertyUint16(name));
}

/*
 * @tc.name: Int32PropertyTest
 * @tc.desc: Test CaptureData float property read/write
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSProfilerCaptureDataTest, Int32PropertyTest, testing::ext::TestSize.Level1)
{
    const int32_t initial = 235832;
    const std::string name = "Int32";
    RSCaptureData data;
    data.SetProperty(name, initial);
    EXPECT_EQ(initial, data.GetPropertyInt32(name));
}

/*
 * @tc.name: Uint32PropertyTest
 * @tc.desc: Test CaptureData float property read/write
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSProfilerCaptureDataTest, Uint32PropertyTest, testing::ext::TestSize.Level1)
{
    const uint32_t initial = 3572989;
    const std::string name = "Uint32";
    RSCaptureData data;
    data.SetProperty(name, initial);
    EXPECT_EQ(initial, data.GetPropertyUint32(name));
}

/*
 * @tc.name: Int64PropertyTest
 * @tc.desc: Test CaptureData float property read/write
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSProfilerCaptureDataTest, Int64PropertyTest, testing::ext::TestSize.Level1)
{
    const int64_t initial = -2315235234;
    const std::string name = "Int64";
    RSCaptureData data;
    data.SetProperty(name, initial);
    EXPECT_EQ(initial, data.GetPropertyInt64(name));
}

/*
 * @tc.name: Uint64PropertyTest
 * @tc.desc: Test CaptureData float property read/write
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSProfilerCaptureDataTest, Uint64PropertyTest, testing::ext::TestSize.Level1)
{
    const uint64_t initial = 37258975989;
    const std::string name = "Uint64";
    RSCaptureData data;
    data.SetProperty(name, initial);
    EXPECT_EQ(initial, data.GetPropertyUint64(name));
}

/*
 * @tc.name: SerializationTest
 * @tc.desc: Test CaptureData float property read/write
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSProfilerCaptureDataTest, SerializationTest, testing::ext::TestSize.Level1)
{
    std::vector<char> buf;
    RSCaptureData initialData;
    RSCaptureData recoveredData;
    initialData.SetTime(78.354f);
    initialData.SetProperty("qwe", 4353);
    initialData.SetProperty("asd", 675.234);
    initialData.SetProperty("zxc", std::string { "important data" });
    initialData.Serialize(buf);

    recoveredData.Deserialize(buf);
    EXPECT_EQ(initialData.time_, recoveredData.time_);
    EXPECT_EQ(initialData.properties_, recoveredData.properties_);
}

} // namespace OHOS::Rosen