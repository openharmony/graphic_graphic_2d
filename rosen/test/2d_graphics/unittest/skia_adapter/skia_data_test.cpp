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

#include <cstddef>
#include "gtest/gtest.h"
#include "skia_adapter/skia_data.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
namespace Drawing {
class SkiaDataTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void SkiaDataTest::SetUpTestCase() {}
void SkiaDataTest::TearDownTestCase() {}
void SkiaDataTest::SetUp() {}
void SkiaDataTest::TearDown() {}

/**
 * @tc.name: SkiaData001
 * @tc.desc: Test functions for SkiaData
 * @tc.type: FUNC
 * @tc.require: I91F9L
 */
HWTEST_F(SkiaDataTest, SkiaData001, TestSize.Level1)
{
    SkiaData skiaData;
    ASSERT_TRUE(skiaData.WritableData() == nullptr);
    ASSERT_TRUE(skiaData.GetSize() >= 0);
    ASSERT_TRUE(skiaData.GetData() == nullptr);
    ASSERT_TRUE(skiaData.GetSkData() == nullptr);
}

/**
 * @tc.name: BuildFromMalloc001
 * @tc.desc: Test BuildFromMalloc
 * @tc.type: FUNC
 * @tc.require: I91F9L
 */
HWTEST_F(SkiaDataTest, BuildFromMalloc001, TestSize.Level1)
{
    SkiaData skiaData;
    auto intData = new int();
    EXPECT_TRUE(skiaData.BuildFromMalloc(intData, sizeof(intData)));
}

/**
 * @tc.name: BuildWithCopy001
 * @tc.desc: Test BuildWithCopy
 * @tc.type: FUNC
 * @tc.require: I91F9L
 */
HWTEST_F(SkiaDataTest, BuildWithCopy001, TestSize.Level1)
{
    SkiaData skiaData;
    auto intData = std::make_unique<int>();
    skiaData.BuildWithCopy(intData.get(), sizeof(*intData.get()));
}

/**
 * @tc.name: BuildWithoutCopy001
 * @tc.desc: Test BuildWithoutCopy
 * @tc.type: FUNC
 * @tc.require: I91F9L
 */
HWTEST_F(SkiaDataTest, BuildWithoutCopy001, TestSize.Level1)
{
    SkiaData skiaData;
    auto intData = std::make_unique<int>();
    skiaData.BuildWithoutCopy(intData.get(), sizeof(*intData.get()));
}

/**
 * @tc.name: BuildUninitialized001
 * @tc.desc: Test BuildUninitialized
 * @tc.type: FUNC
 * @tc.require: I91F9L
 */
HWTEST_F(SkiaDataTest, BuildUninitialized001, TestSize.Level1)
{
    SkiaData skiaData;
    auto intData = std::make_unique<int>();
    skiaData.BuildUninitialized(sizeof(*intData.get()));
}

/**
 * @tc.name: BuildEmpty001
 * @tc.desc: Test BuildEmpty
 * @tc.type: FUNC
 * @tc.require: I91F9L
 */
HWTEST_F(SkiaDataTest, BuildEmpty001, TestSize.Level1)
{
    SkiaData skiaData;
    skiaData.BuildEmpty();
}
} // namespace Drawing
} // namespace Rosen
} // namespace OHOS