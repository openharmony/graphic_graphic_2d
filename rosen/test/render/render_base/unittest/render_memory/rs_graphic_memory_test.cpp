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

#include <gtest/gtest.h>
#include "memory/rs_memory_graphic.h"

namespace OHOS::Rosen {
class RSGraphicMemoryTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void RSGraphicMemoryTest::SetUpTestCase() {}
void RSGraphicMemoryTest::TearDownTestCase() {}
void RSGraphicMemoryTest::SetUp() {}
void RSGraphicMemoryTest::TearDown() {}


/**
 * @tc.name: IncreaseCpuMemoryTest
 * @tc.desc: test
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSGraphicMemoryTest, IncreaseCpuMemoryTest, testing::ext::TestSize.Level1)
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
HWTEST_F(RSGraphicMemoryTest, GetPidTest, testing::ext::TestSize.Level1)
{
    MemoryGraphic memoryGraphic;
    memoryGraphic.SetPid(1.0);
    auto ret = memoryGraphic.GetPid();
    ASSERT_EQ(ret, 1.0);
}

/**
 * @tc.name: UnmarshallingTest
 * @tc.desc: test
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSGraphicMemoryTest, UnmarshallingTest, testing::ext::TestSize.Level1)
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
HWTEST_F(RSGraphicMemoryTest, MarshallingTest, testing::ext::TestSize.Level1)
{
    Parcel parcel;
    MemoryGraphic memoryGraphic;
    auto mem = memoryGraphic.Marshalling(parcel);
    ASSERT_EQ(mem, true);
}

/**
 * @tc.name: GetGpuMemorySizeTest
 * @tc.desc: test
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSGraphicMemoryTest, GetGpuMemorySizeTest, testing::ext::TestSize.Level1)
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
} // namespace OHOS::Rosen