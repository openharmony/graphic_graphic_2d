/*
 * Copyright (c) 2023 Huawei Device Co., Ltd.
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

#include <gtest/gtest.h>
#include "memory/rs_memory_graphic.h"

namespace OHOS::Rosen {
class RSMemoryGraphicTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void RSMemoryGraphicTest::SetUpTestCase() {}
void RSMemoryGraphicTest::TearDownTestCase() {}
void RSMemoryGraphicTest::SetUp() {}
void RSMemoryGraphicTest::TearDown() {}


/**
 * @tc.name: UnmarshallingTest
 * @tc.desc: test
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSMemoryGraphicTest, UnmarshallingTest, testing::ext::TestSize.Level1)
{
    Parcel parcel;
    MemoryGraphic memoryGraphic;
    auto mem = memoryGraphic.Unmarshalling(parcel);
    ASSERT_EQ(mem, nullptr);
}


/**
 * @tc.name: MarshallingTest
 * @tc.desc: test
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSMemoryGraphicTest, MarshallingTest, testing::ext::TestSize.Level1)
{
    Parcel parcel;
    MemoryGraphic memoryGraphic;
    auto mem = memoryGraphic.Marshalling(parcel);
    ASSERT_EQ(mem, true);
}

/**
 * @tc.name: IncreaseCpuMemoryTest
 * @tc.desc: test
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSMemoryGraphicTest, IncreaseCpuMemoryTest, testing::ext::TestSize.Level1)
{
    // for test
    float glSize = 1.0;
    // for test
    float graphicSize = 1.0;
    MemoryGraphic memoryGraphic;
    memoryGraphic.IncreaseCpuMemory(glSize);
    memoryGraphic.IncreaseGpuMemory(graphicSize);
    int ret = 0;
    ASSERT_EQ(ret, 0);
}

/**
 * @tc.name: GetPidTest
 * @tc.desc: test
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSMemoryGraphicTest, GetPidTest, testing::ext::TestSize.Level1)
{
    MemoryGraphic memoryGraphic;
    memoryGraphic.SetPid(1.0);
    auto ret = memoryGraphic.GetPid();
    ASSERT_EQ(ret, 1.0);
}

/**
 * @tc.name: GetGpuMemorySizeTest
 * @tc.desc: test
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSMemoryGraphicTest, GetGpuMemorySizeTest01, testing::ext::TestSize.Level1)
{
    MemoryGraphic memoryGraphic;
    memoryGraphic.SetGpuMemorySize(1.0);
    memoryGraphic.SetCpuMemorySize(1.0);
    auto ret = memoryGraphic.GetGpuMemorySize();
    ASSERT_EQ(ret, 1.0);
    ret = memoryGraphic.GetCpuMemorySize();
    ASSERT_EQ(ret, 1.0);
    ret = memoryGraphic.GetTotalMemorySize();
    ASSERT_EQ(ret, 2.0);
}

/**
 * @tc.name: GetGpuMemorySizeTest
 * @tc.desc: test
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSMemoryGraphicTest, GetGpuMemorySizeTest02, testing::ext::TestSize.Level1)
{
    MemoryGraphic memoryGraphic;
    memoryGraphic.SetGpuMemorySize(2.0);
    memoryGraphic.SetCpuMemorySize(2.0);
    auto ret = memoryGraphic.GetGpuMemorySize();
    ASSERT_EQ(ret, 2.0);
    ret = memoryGraphic.GetCpuMemorySize();
    ASSERT_EQ(ret, 2.0);
    ret = memoryGraphic.GetTotalMemorySize();
    ASSERT_EQ(ret, 4.0);
}

/**
 * @tc.name: GetGpuMemorySizeTest
 * @tc.desc: test
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSMemoryGraphicTest, GetGpuMemorySizeTest03, testing::ext::TestSize.Level1)
{
    MemoryGraphic memoryGraphic;
    memoryGraphic.SetGpuMemorySize(3.0);
    memoryGraphic.SetCpuMemorySize(3.0);
    auto ret = memoryGraphic.GetGpuMemorySize();
    ASSERT_EQ(ret, 3.0);
    ret = memoryGraphic.GetCpuMemorySize();
    ASSERT_EQ(ret, 3.0);
    ret = memoryGraphic.GetTotalMemorySize();
    ASSERT_EQ(ret, 6.0);
}

/**
 * @tc.name: GetGpuMemorySizeTest
 * @tc.desc: test
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSMemoryGraphicTest, GetGpuMemorySizeTest04, testing::ext::TestSize.Level1)
{
    MemoryGraphic memoryGraphic;
    memoryGraphic.SetGpuMemorySize(4.0);
    memoryGraphic.SetCpuMemorySize(4.0);
    auto ret = memoryGraphic.GetGpuMemorySize();
    ASSERT_EQ(ret, 4.0);
    ret = memoryGraphic.GetCpuMemorySize();
    ASSERT_EQ(ret, 4.0);
    ret = memoryGraphic.GetTotalMemorySize();
    ASSERT_EQ(ret, 8.0);
}

/**
 * @tc.name: GetGpuMemorySizeTest
 * @tc.desc: test
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSMemoryGraphicTest, GetGpuMemorySizeTest05, testing::ext::TestSize.Level1)
{
    MemoryGraphic memoryGraphic;
    memoryGraphic.SetGpuMemorySize(5.0);
    memoryGraphic.SetCpuMemorySize(5.0);
    auto ret = memoryGraphic.GetGpuMemorySize();
    ASSERT_EQ(ret, 5.0);
    ret = memoryGraphic.GetCpuMemorySize();
    ASSERT_EQ(ret, 5.0);
    ret = memoryGraphic.GetTotalMemorySize();
    ASSERT_EQ(ret, 10.0);
}

/**
 * @tc.name: GetGpuMemorySizeTest
 * @tc.desc: test
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSMemoryGraphicTest, GetGpuMemorySizeTest06, testing::ext::TestSize.Level1)
{
    MemoryGraphic memoryGraphic;
    memoryGraphic.SetGpuMemorySize(6.0);
    memoryGraphic.SetCpuMemorySize(6.0);
    auto ret = memoryGraphic.GetGpuMemorySize();
    ASSERT_EQ(ret, 6.0);
    ret = memoryGraphic.GetCpuMemorySize();
    ASSERT_EQ(ret, 6.0);
    ret = memoryGraphic.GetTotalMemorySize();
    ASSERT_EQ(ret, 12.0);
}

/**
 * @tc.name: GetGpuMemorySizeTest
 * @tc.desc: test
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSMemoryGraphicTest, GetGpuMemorySizeTest07, testing::ext::TestSize.Level1)
{
    MemoryGraphic memoryGraphic;
    memoryGraphic.SetGpuMemorySize(7.0);
    memoryGraphic.SetCpuMemorySize(7.0);
    auto ret = memoryGraphic.GetGpuMemorySize();
    ASSERT_EQ(ret, 7.0);
    ret = memoryGraphic.GetCpuMemorySize();
    ASSERT_EQ(ret, 7.0);
    ret = memoryGraphic.GetTotalMemorySize();
    ASSERT_EQ(ret, 14.0);
}

/**
 * @tc.name: GetGpuMemorySizeTest
 * @tc.desc: test
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSMemoryGraphicTest, GetGpuMemorySizeTest08, testing::ext::TestSize.Level1)
{
    MemoryGraphic memoryGraphic;
    memoryGraphic.SetGpuMemorySize(8.0);
    memoryGraphic.SetCpuMemorySize(8.0);
    auto ret = memoryGraphic.GetGpuMemorySize();
    ASSERT_EQ(ret, 8.0);
    ret = memoryGraphic.GetCpuMemorySize();
    ASSERT_EQ(ret, 8.0);
    ret = memoryGraphic.GetTotalMemorySize();
    ASSERT_EQ(ret, 16.0);
}


/**
 * @tc.name: GetGpuMemorySizeTest
 * @tc.desc: test
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSMemoryGraphicTest, GetGpuMemorySizeTest09, testing::ext::TestSize.Level1)
{
    MemoryGraphic memoryGraphic;
    memoryGraphic.SetGpuMemorySize(9.0);
    memoryGraphic.SetCpuMemorySize(9.0);
    auto ret = memoryGraphic.GetGpuMemorySize();
    ASSERT_EQ(ret, 9.0);
    ret = memoryGraphic.GetCpuMemorySize();
    ASSERT_EQ(ret, 9.0);
    ret = memoryGraphic.GetTotalMemorySize();
    ASSERT_EQ(ret, 18.0);
}

/**
 * @tc.name: GetGpuMemorySizeTest
 * @tc.desc: test
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSMemoryGraphicTest, GetGpuMemorySizeTest10, testing::ext::TestSize.Level1)
{
    MemoryGraphic memoryGraphic;
    memoryGraphic.SetGpuMemorySize(10.0);
    memoryGraphic.SetCpuMemorySize(10.0);
    auto ret = memoryGraphic.GetGpuMemorySize();
    ASSERT_EQ(ret, 10.0);
    ret = memoryGraphic.GetCpuMemorySize();
    ASSERT_EQ(ret, 10.0);
    ret = memoryGraphic.GetTotalMemorySize();
    ASSERT_EQ(ret, 20.0);
}


/**
 * @tc.name: GetGpuMemorySizeTest
 * @tc.desc: test
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSMemoryGraphicTest, GetGpuMemorySizeTest11, testing::ext::TestSize.Level1)
{
    MemoryGraphic memoryGraphic;
    memoryGraphic.SetGpuMemorySize(11.0);
    memoryGraphic.SetCpuMemorySize(11.0);
    auto ret = memoryGraphic.GetGpuMemorySize();
    ASSERT_EQ(ret, 11.0);
    ret = memoryGraphic.GetCpuMemorySize();
    ASSERT_EQ(ret, 11.0);
    ret = memoryGraphic.GetTotalMemorySize();
    ASSERT_EQ(ret, 22.0);
}
} // namespace OHOS::Rosen