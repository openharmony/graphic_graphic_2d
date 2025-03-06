/*
 * Copyright (c) 2025 Huawei Device Co., Ltd.
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

#include <vector>
#include <string>
#include <unordered_set>
#include "gtest/gtest.h"
#include "gfx/dump/rs_dump_manager.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Rosen {
class RSDumpManagerTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void RSDumpManagerTest::SetUpTestCase() {}
void RSDumpManagerTest::TearDownTestCase() {}
void RSDumpManagerTest::SetUp() {}
void RSDumpManagerTest::TearDown() {}

// Test case for Register method
HWTEST_F(RSDumpManagerTest, Dump_DumpIdExist, TestSize.Level1)
{
    RSDumpManager& rsDumpManager = RSDumpManager::GetInstance();
    RSDumpHander rsDumpHander = { RSDumpID::RENDER_NODE_INFO,
        [](const std::u16string &cmd, std::unordered_set<std::u16string> &argSets,
            std::string &dumpString) -> void {
            dumpString = "Test RSTree Information";
        } };
    rsDumpManager.Register(rsDumpHander);

    std::u16string cmd = u"RSTree";
    std::unordered_set<std::u16string> argSets = { cmd };
    std::string out;

    auto it = rsDumpManager.rsDumpHanderMap_.find(RSDumpID::RENDER_NODE_INFO);
    RSDumpHander hander = it->second;
    hander.func(cmd, argSets, out);
    EXPECT_EQ(out, "Test RSTree Information");
}

// Test case for Register method
HWTEST_F(RSDumpManagerTest, Register_WhenModuleIdExist, TestSize.Level1)
{
    RSDumpManager& rsDumpManager = RSDumpManager::GetInstance();
    RSDumpHander rsDumpHander = { RSDumpID::RENDER_NODE_INFO,
        [](const std::u16string &cmd, std::unordered_set<std::u16string> &argSets,
            std::string &dumpString) -> void {
            dumpString = "Test RSTree Information";
        } };
    rsDumpManager.Register(rsDumpHander);
    rsDumpManager.Register(rsDumpHander);
    rsDumpManager.Register(rsDumpHander);
    EXPECT_EQ(rsDumpManager.rsDumpHanderMap_.size(), 2);
}

// Test case for UnRegister method
HWTEST_F(RSDumpManagerTest, UnRegister_DumpIdExist, TestSize.Level1)
{
    RSDumpManager& rsDumpManager = RSDumpManager::GetInstance();
    RSDumpHander rsDumpHander = { RSDumpID::RENDER_NODE_INFO,
        [](const std::u16string &cmd, std::unordered_set<std::u16string> &argSets,
            std::string &dumpString) -> void {
            dumpString = "Test RSTree Information";
        } };
    rsDumpManager.Register(rsDumpHander);
    rsDumpManager.UnRegister(RSDumpID::RENDER_NODE_INFO);
    EXPECT_EQ(rsDumpManager.rsDumpHanderMap_.find(RSDumpID::RENDER_NODE_INFO), rsDumpManager.rsDumpHanderMap_.end());
}

// Test case for UnRegister method
HWTEST_F(RSDumpManagerTest, UnRegister_DumpIdNotExist, TestSize.Level1)
{
    RSDumpManager& rsDumpManager = RSDumpManager::GetInstance();
    rsDumpManager.UnRegister(RSDumpID::RENDER_NODE_INFO);
    EXPECT_EQ(rsDumpManager.rsDumpHanderMap_.size(), 1);
}

// Test case for CmdExec method
HWTEST_F(RSDumpManagerTest, CmdExec_WhenCommandExist, TestSize.Level1)
{
    RSDumpManager& rsDumpManager = RSDumpManager::GetInstance();
    RSDumpHander rsDumpHander = { RSDumpID::RENDER_NODE_INFO,
        [](const std::u16string &cmd, std::unordered_set<std::u16string> &argSets,
            std::string &dumpString) -> void {
            dumpString = "Test RSTree Information";
        } };
    rsDumpManager.Register(rsDumpHander);

    std::u16string cmd = u"RSTree";
    std::unordered_set<std::u16string> argSets = { cmd };
    std::string out;
    rsDumpManager.CmdExec(argSets, out);
    EXPECT_EQ(out, "Test RSTree Information");
}

// Test case for CmdExec method
HWTEST_F(RSDumpManagerTest, CmdExec_WhenCommandNotExist, TestSize.Level1)
{
    RSDumpManager& rsDumpManager = RSDumpManager::GetInstance();
    std::unordered_set<std::u16string> argSets = { u"invalid_command" };
    std::string out;
    rsDumpManager.CmdExec(argSets, out);
    ASSERT_TRUE(out.find("help text for the tool") != std::string::npos);
}
}