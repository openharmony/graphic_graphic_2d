/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
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

#include "utils/data.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
namespace Drawing {
class DataTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void DataTest::SetUpTestCase() {}
void DataTest::TearDownTestCase() {}
void DataTest::SetUp() {}
void DataTest::TearDown() {}

/*
 * @tc.name: CreateAndDestory001
 * @tc.desc:Verify the CreateAndDestory001 sub function.
 * @tc.type: FUNC
 * @tc.require: I71KWQ
 */
HWTEST_F(DataTest, CreateAndDestory001, TestSize.Level1)
{
    auto data = std::make_unique<Data>();
    ASSERT_TRUE(data != nullptr);
}

/*
 * @tc.name: BuildFromMallocTest001
 * @tc.desc:Verify the BuildFromMallocTest001 sub function.
 * @tc.type: FUNC
 * @tc.require: I71KWQ
 */
HWTEST_F(DataTest, BuildFromMallocTest001, TestSize.Level1)
{
    auto data = std::make_unique<Data>();
    ASSERT_TRUE(data != nullptr);
    auto intData = new int();
    EXPECT_TRUE(data->BuildFromMalloc(intData, sizeof(intData)));
}

/*
 * @tc.name: BuildWithCopyTest001
 * @tc.desc:Verify the BuildWithCopyTest001 sub function.
 * @tc.type: FUNC
 * @tc.require: I71KWQ
 */
HWTEST_F(DataTest, BuildWithCopyTest001, TestSize.Level1)
{
    auto data = std::make_unique<Data>();
    ASSERT_TRUE(data != nullptr);
    auto intData = std::make_unique<int>();
    EXPECT_TRUE(data->BuildWithCopy(intData.get(), sizeof(*intData.get())));
}

/*
 * @tc.name: BuildWithoutCopyTest001
 * @tc.desc:Verify the BuildWithoutCopyTest001 sub function.
 * @tc.type: FUNC
 * @tc.require: I71KWQ
 */
HWTEST_F(DataTest, BuildWithoutCopyTest001, TestSize.Level1)
{
    auto data = std::make_unique<Data>();
    ASSERT_TRUE(data != nullptr);
    auto intData = std::make_unique<int>();
    EXPECT_TRUE(data->BuildWithoutCopy(intData.get(), sizeof(*intData.get())));
}

/*
 * @tc.name: BuildUninitializedTest001
 * @tc.desc:Verify the BuildUninitializedTest001 sub function.
 * @tc.type: FUNC
 * @tc.require: I71KWQ
 */
HWTEST_F(DataTest, BuildUninitializedTest001, TestSize.Level1)
{
    auto data = std::make_unique<Data>();
    ASSERT_TRUE(data != nullptr);
    auto intData = std::make_unique<int>();
    EXPECT_TRUE(data->BuildUninitialized(sizeof(*intData.get())));
}

/*
 * @tc.name: WritableDataTest001
 * @tc.desc:Verify the WritableDataTest001 sub function.
 * @tc.type: FUNC
 * @tc.require: I71KWQ
 */
HWTEST_F(DataTest, WritableDataTest001, TestSize.Level1)
{
    auto data = std::make_unique<Data>();
    ASSERT_TRUE(data != nullptr);
    EXPECT_FALSE(data->WritableData());
}

/*
 * @tc.name: DataGetSizeTest001
 * @tc.desc:Verify the DataGetSizeTest001 sub function.
 * @tc.type: FUNC
 * @tc.require: I71KWQ
 */
HWTEST_F(DataTest, DataGetSizeTest001, TestSize.Level1)
{
    auto data = std::make_unique<Data>();
    ASSERT_TRUE(data != nullptr);
    auto intData = std::make_unique<int>();
    data->BuildWithCopy(intData.get(), sizeof(*intData.get()));
    EXPECT_TRUE(sizeof(*intData.get()) == data->GetSize());
}

/*
 * @tc.name: DataGetDataTest001
 * @tc.desc:Verify the DataGetDataTest001 sub function.
 * @tc.type: FUNC
 * @tc.require: I71KWQ
 */
HWTEST_F(DataTest, DataGetDataTest001, TestSize.Level1)
{
    auto data = std::make_unique<Data>();
    ASSERT_TRUE(data != nullptr);
    auto intData = std::make_unique<int>(10);
    data->BuildWithCopy(intData.get(), sizeof(*intData.get()));
    EXPECT_TRUE(*intData == *((int*)data->GetData()));
}
} // namespace Drawing
} // namespace Rosen
} // namespace OHOS
