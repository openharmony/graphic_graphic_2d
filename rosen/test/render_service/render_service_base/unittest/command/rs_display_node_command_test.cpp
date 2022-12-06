/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
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
#include "include/command/rs_display_node_command.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Rosen {
class RSDisplayNodeCommandTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void RSDisplayNodeCommandTest::SetUpTestCase() {}
void RSDisplayNodeCommandTest::TearDownTestCase() {}
void RSDisplayNodeCommandTest::SetUp() {}
void RSDisplayNodeCommandTest::TearDown() {}

/**
 * @tc.name: TestRSBaseNodeCommand001
 * @tc.desc: SetScreenId test.
 * @tc.type: FUNC
 */
HWTEST_F(RSDisplayNodeCommandTest, TestRSDisplayNodeCommand001, TestSize.Level1)
{
    RSContext context;
    NodeId id = static_cast<NodeId>(-1);
    uint64_t screenId = static_cast<uint64_t>(0);
    DisplayNodeCommandHelper::SetScreenId(context, id, screenId);
}

/**
 * @tc.name: TestRSDisplayNodeCommand002
 * @tc.desc: SetDisplayOffset test.
 * @tc.type: FUNC
 */
HWTEST_F(RSDisplayNodeCommandTest, TestRSDisplayNodeCommand002, TestSize.Level1)
{
    RSContext context;
    NodeId id = static_cast<NodeId>(-1);
    int32_t offsetX = static_cast<int32_t>(1);
    int32_t offsetY = static_cast<int32_t>(1);
    DisplayNodeCommandHelper::SetDisplayOffset(context, id, offsetX, offsetY);
}

/**
 * @tc.name: TestRSDisplayNodeCommand003
 * @tc.desc: SetSecurityDisplay test.
 * @tc.type: FUNC
 */
HWTEST_F(RSDisplayNodeCommandTest, TestRSDisplayNodeCommand003, TestSize.Level1)
{
    RSContext context;
    NodeId id = static_cast<NodeId>(-1);
    bool isSecurityDisplay = false;
    DisplayNodeCommandHelper::SetSecurityDisplay(context, id, isSecurityDisplay);
}

/**
 * @tc.name: TestRSDisplayNodeCommand004
 * @tc.desc: SetDisplayMode test.
 * @tc.type: FUNC
 */
HWTEST_F(RSDisplayNodeCommandTest, TestRSDisplayNodeCommand004, TestSize.Level1)
{
    RSContext context;
    NodeId id = static_cast<NodeId>(1);
    RSDisplayNodeConfig config { 0, true, 0 };
    DisplayNodeCommandHelper::SetDisplayMode(context, id, config);
}

/**
 * @tc.name: Create001
 * @tc.desc: test.
 * @tc.type: FUNC
 */
HWTEST_F(RSDisplayNodeCommandTest, Create001, TestSize.Level1)
{
    RSContext context;
    NodeId id = static_cast<NodeId>(1);
    RSDisplayNodeConfig config { 0, true, 0 };
    DisplayNodeCommandHelper::Create(context, id, config);
}
} // namespace OHOS::Rosen
