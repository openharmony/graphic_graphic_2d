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
#include "common/rs_xcollie.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Rosen {
class RSXCollieTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void RSXCollieTest::SetUpTestCase() {}
void RSXCollieTest::TearDownTestCase() {}
void RSXCollieTest::SetUp() {}
void RSXCollieTest::TearDown() {}

/**
 * @tc.name: RSXCollieTest001
 * @tc.desc: test of RSXCollie
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSXCollieTest, RSXCollieTest001, TestSize.Level1)
{
    // no except
    RSXCollie test("RSXCollieTest");
    ASSERT_EQ(test.tag_, "RSXCollieTest");
}
} // namespace OHOS::Rosen