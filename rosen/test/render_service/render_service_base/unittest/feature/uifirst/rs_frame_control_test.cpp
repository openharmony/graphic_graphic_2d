/*
* Copyright (c) 2026-2026 Huawei Device Co., Ltd.
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

#include "feature/uifirst/rs_frame_control.h"
#include "platform/common/rs_log.h"
#include "platform/common/rs_system_properties.h"

using namespace testing;
using namespace testing::ext;
namespace OHOS {
namespace Rosen {

class RSFrameControlToolTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void RSFrameControlToolTest::SetUpTestCase() {}
void RSFrameControlToolTest::TearDownTestCase() {}
void RSFrameControlToolTest::SetUp() {}
void RSFrameControlToolTest::TearDown() {}

/**
* @tc.name: SerchNodeIdFromAppWindowsSet001
* @tc.desc: test single frame modifier add to list
* @tc.type: FUNC
* @tc.require:
*/
HWTEST_F(RSFrameControlToolTest, SerchNodeIdFromAppWindowsSet001, TestSize.Level1)
    NodeId id1 = 100;
    NodeId id2 = 200;
    // case1
    RSFrameControlTool::Instance().InsertNodeIdToAppWindowSet(id1);
    EXPECT_EQ(RSFrameControlTool::Instance().SerchNodeIdFromAppWindowsSet(id1), true);
    // case2
    EXPECT_EQ(RSFrameControlTool::Instance().SerchNodeIdFromAppWindowsSet(id2), false);
}
}