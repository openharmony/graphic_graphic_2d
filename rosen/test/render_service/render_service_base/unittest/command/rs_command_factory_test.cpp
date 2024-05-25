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
#include "command/rs_command_factory.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Rosen {
class RSCommandFactoryTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void RSCommandFactoryTest::SetUpTestCase() {}
void RSCommandFactoryTest::TearDownTestCase() {}
void RSCommandFactoryTest::SetUp() {}
void RSCommandFactoryTest::TearDown() {}

/**
 * @tc.name: Register001
 * @tc.desc: test results of Register
 * @tc.type: FUNC
 * @tc.require: issueI9P2KH
 */
HWTEST_F(RSCommandFactoryTest, Register001, TestSize.Level1)
{
    RSCommandFactory& factory = RSCommandFactory::Instance();
    UnmarshallingFunc func = nullptr;
    factory.Register(0, 0, func);
    EXPECT_TRUE(func == nullptr);
}

/**
 * @tc.name: GetUnmarshallingFunc001
 * @tc.desc: test results of GetUnmarshallingFunc
 * @tc.type: FUNC
 * @tc.require: issueI9P2KH
 */
HWTEST_F(RSCommandFactoryTest, GetUnmarshallingFunc001, TestSize.Level1)
{
    RSCommandFactory& factory = RSCommandFactory::Instance();
    UnmarshallingFunc func = factory.GetUnmarshallingFunc(0, 0);
    EXPECT_TRUE(func != nullptr);

    func = factory.GetUnmarshallingFunc(65535, 65535); // for test
    EXPECT_TRUE(func == nullptr);
}
} // namespace OHOS::Rosen
