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

#include <fcntl.h>

#include "gtest/gtest.h"
#include "rs_profiler_command.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Rosen {
class RSProfilerCommandTest : public testing::Test {
public:
    static void SetUpTestCase() {};
    static void TearDownTestCase() {};
    void SetUp() override {};
    void TearDown() override {};
};

/*
 * @tc.name: CommandArgListTest
 * @tc.desc: RSProfiler Command ArgList public API Test
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSProfilerCommandTest, CommandArgListTest, testing::ext::TestSize.Level1)
{
    std::vector<std::string> args { "123", "-5", "45738957893", "837983", "1.5", "678.789", "qwe" };
    ArgList list { args };

    EXPECT_EQ(list.Uint32(0), 123);
    EXPECT_EQ(list.Int32(1), -5);
    EXPECT_EQ(list.Uint64(2), 45738957893);
    EXPECT_EQ(list.Int64(3), 837983);
    EXPECT_EQ(list.Fp32(4), 1.5f);
    EXPECT_EQ(list.Fp64(5), 678.789);
    EXPECT_EQ(list.String(6), "qwe");
    EXPECT_EQ(list.Count(), 7);
    EXPECT_TRUE(list.String(7).empty());
    EXPECT_TRUE(list.String(100).empty());

    EXPECT_FALSE(list.Empty());
    list.Clear();
    EXPECT_TRUE(list.Empty());
}
}