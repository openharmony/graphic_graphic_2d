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

#include "gtest/gtest.h"
#include "rs_profiler_network.h"
#include "rs_profiler_packet.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Rosen {
class RSProfilerNetworkUnitTest : public testing::Test {
public:
    static void SetUpTestCase() {};
    static void TearDownTestCase() {};
    void SetUp() override;
    void TearDown() override {};
};

void RSProfilerNetworkUnitTest::SetUp()
{
    Network::outgoing_ = {};
    Network::incoming_ = {};
}

/*
 * @tc.name: RSProfilerNetworkPushPopCommandTest
 * @tc.desc: Test public method PushPopCommand
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSProfilerNetworkUnitTest, RSProfilerNetworkPushPopCommandTest, testing::ext::TestSize.Level1)
{
    std::vector<std::string> command { "qwe", "123", "asd", "456" };
    Network::PushCommand(command);
    EXPECT_EQ(Network::incoming_.front(), command);

    std::vector<std::string> args;
    Network::PopCommand(args);
    EXPECT_EQ(args, command);
    EXPECT_TRUE(Network::incoming_.empty());
}

/*
 * @tc.name: RSProfilerNetworkProcessCommandTest
 * @tc.desc: Test public method ProcessCommand
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSProfilerNetworkUnitTest, RSProfilerNetworkProcessCommandTest, testing::ext::TestSize.Level1)
{
    std::string command = "";
    Network::ProcessCommand(command.data(), command.size());
    EXPECT_TRUE(Network::incoming_.empty());

    command = "qwe 123";
    Network::ProcessCommand(command.data(), command.size());
    std::vector<std::string> expected { "qwe", "123" };
    EXPECT_EQ(Network::incoming_.front(), expected);
}

/*
 * @tc.name: RSProfilerNetworkSendBinaryTest
 * @tc.desc: Test public method SendBinary
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSProfilerNetworkUnitTest, RSProfilerNetworkSendBinaryTest, testing::ext::TestSize.Level1)
{
    std::vector<char> data { 0, 1, 2, 3, 4, 5 };
    Network::SendBinary(nullptr, data.size());
    EXPECT_TRUE(Network::outgoing_.empty());
    Network::SendBinary(data.data(), 0);
    EXPECT_TRUE(Network::outgoing_.empty());
    Network::SendBinary(data.data(), data.size());

    auto type = static_cast<char>(Packet::BINARY);
    std::vector<char> sendData { type, 11, 0, 0, 0, 0, 1, 2, 3, 4, 5 };
    EXPECT_EQ(Network::outgoing_.back(), sendData);
}

/*
 * @tc.name: RSProfilerNetworkSendMessageTest
 * @tc.desc: Test public method SendMessage
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSProfilerNetworkUnitTest, RSProfilerNetworkSendMessageTest, testing::ext::TestSize.Level1)
{
    Network::SendMessage("");
    EXPECT_TRUE(Network::outgoing_.empty());
    Network::SendMessage("hello");
    auto type = static_cast<char>(Packet::LOG);
    std::vector<char> sendData { type, 10, 0, 0, 0, 'h', 'e', 'l', 'l', 'o' };
    EXPECT_EQ(Network::outgoing_.back(), sendData);
}

/*
 * @tc.name: RSProfilerNetworkSendRdcPathTest
 * @tc.desc: Test public method SendRdcPath
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSProfilerNetworkUnitTest, RSProfilerNetworkSendRdcPathTest, testing::ext::TestSize.Level1)
{
    Network::SendRdcPath("");
    EXPECT_TRUE(Network::outgoing_.empty());
    Network::SendRdcPath("path");
    auto type = static_cast<char>(Packet::BINARY);
    auto subtype = static_cast<char>(PackageID::RS_PROFILER_RDC_BINARY);
    std::vector<char> sendData { type, 10, 0, 0, 0, subtype, 'p', 'a', 't', 'h' };
    EXPECT_EQ(Network::outgoing_.back(), sendData);
}

/*
 * @tc.name: RSProfilerNetworkSendDclPathTest
 * @tc.desc: Test public method SendDclPath
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSProfilerNetworkUnitTest, RSProfilerNetworkSendDclPathTest, testing::ext::TestSize.Level1)
{
    Network::SendDclPath("");
    EXPECT_TRUE(Network::outgoing_.empty());
    Network::SendDclPath("path");
    auto type = static_cast<char>(Packet::BINARY);
    auto subtype = static_cast<char>(PackageID::RS_PROFILER_DCL_BINARY);
    std::vector<char> sendData { type, 10, 0, 0, 0, subtype, 'p', 'a', 't', 'h' };
    EXPECT_EQ(Network::outgoing_.back(), sendData);
}

/*
 * @tc.name: RSProfilerNetworkSendSkpTest
 * @tc.desc: Test public method SendSkp
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSProfilerNetworkUnitTest, RSProfilerNetworkSendSkpTest, testing::ext::TestSize.Level1)
{
    std::vector<char> data { 0, 1, 2, 3, 4, 5 };
    Network::SendSkp(nullptr, data.size());
    EXPECT_TRUE(Network::outgoing_.empty());
    Network::SendSkp(data.data(), 0);
    EXPECT_TRUE(Network::outgoing_.empty());
    Network::SendSkp(data.data(), data.size());
    auto type = static_cast<char>(Packet::BINARY);
    auto subtype = static_cast<char>(PackageID::RS_PROFILER_SKP_BINARY);
    std::vector<char> sendData { type, 12, 0, 0, 0, subtype, 0, 1, 2, 3, 4, 5 };
    EXPECT_EQ(Network::outgoing_.back(), sendData);
}

/*
 * @tc.name: RSProfilerNetworkSendPerfNodeListTest
 * @tc.desc: Test public method SendRSTreePerfNodeList
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSProfilerNetworkUnitTest, RSProfilerNetworkSendPerfNodeListTest, testing::ext::TestSize.Level1)
{
    std::unordered_set<uint64_t> nodelist;
    auto type = static_cast<char>(Packet::BINARY);
    auto subtype = static_cast<char>(PackageID::RS_PROFILER_RSTREE_PERF_NODE_LIST);
    std::vector<char> sendData { type, 6, 0, 0, 0, subtype };
    Network::SendRSTreePerfNodeList(nodelist);
    EXPECT_EQ(Network::outgoing_.back(), sendData);

    nodelist = { 1, 2 };
    sendData = { type, 22, 0, 0, 0, subtype, 2, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0 };
    Network::SendRSTreePerfNodeList(nodelist);
    EXPECT_EQ(Network::outgoing_.back(), sendData);
}

/*
 * @tc.name: RSProfilerNetworkSendSingleNodePerfTest
 * @tc.desc: Test public method SendRSTreeSingleNodePerf
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSProfilerNetworkUnitTest, RSProfilerNetworkSendSingleNodePerfTest, testing::ext::TestSize.Level1)
{
    auto type = static_cast<char>(Packet::BINARY);
    auto subtype = static_cast<char>(PackageID::RS_PROFILER_RSTREE_SINGLE_NODE_PERF);
    std::vector<char> sendData { type, 22, 0, 0, 0, subtype, 10, 0, 0, 0, 0, 0, 0, 0, 80, 0, 0, 0, 0, 0, 0, 0 };
    Network::SendRSTreeSingleNodePerf(10, 80);
    EXPECT_EQ(Network::outgoing_.back(), sendData);
}

} // namespace OHOS::Rosen