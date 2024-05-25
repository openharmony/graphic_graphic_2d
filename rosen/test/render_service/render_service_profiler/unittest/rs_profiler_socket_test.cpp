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
#include "rs_profiler_socket.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Rosen {
class RSProfilerSocketTest : public testing::Test {
public:
    static void SetUpTestCase() {};
    static void TearDownTestCase() {};
    void SetUp() override {};
    void TearDown() override {};
};

/*
 * @tc.name: RSProfilerSocketStateTest
 * @tc.desc: Test state setter/getter
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSProfilerSocketTest, RSProfilerSocketStateTest, testing::ext::TestSize.Level1)
{
    Socket s;
    EXPECT_EQ(s.GetState(), SocketState::INITIAL);

    s.SetState(SocketState::CREATE);
    EXPECT_EQ(s.GetState(), SocketState::CREATE);
}

/*
 * @tc.name: RSProfilerSocketOpenTest
 * @tc.desc: Test public method Open
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSProfilerSocketTest, RSProfilerSocketOpenTest, testing::ext::TestSize.Level1)
{
    Socket s;
    s.Open(5050);
    EXPECT_EQ(s.GetState(), SocketState::CREATE);
    EXPECT_NE(s.socket_, -1);
    EXPECT_EQ(s.client_, -1);
}

/*
 * @tc.name: RSProfilerSocketShutdownTest
 * @tc.desc: Test public method Shutdown
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSProfilerSocketTest, RSProfilerSocketShutdownTest, testing::ext::TestSize.Level1)
{
    Socket s;
    s.Open(5050);
    s.Shutdown();
    EXPECT_EQ(s.GetState(), SocketState::SHUTDOWN);
    EXPECT_EQ(s.socket_, -1);
    EXPECT_EQ(s.client_, -1);
}

/*
 * @tc.name: RSProfilerSocketAcceptClientTest
 * @tc.desc: Test public method AcceptClient
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSProfilerSocketTest, RSProfilerSocketAcceptClientTest, testing::ext::TestSize.Level1)
{
    Socket s;
    s.AcceptClient();
    EXPECT_EQ(s.GetState(), SocketState::SHUTDOWN);
    EXPECT_EQ(s.socket_, -1);
    EXPECT_EQ(s.client_, -1);
}

/*
 * @tc.name: RSProfilerSocketGetStatusTest
 * @tc.desc: Test public method GetStatus
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSProfilerSocketTest, RSProfilerSocketGetStatusTest, testing::ext::TestSize.Level1)
{
    Socket s;
    bool readyToReceive;
    bool readyToSend;
    s.GetStatus(readyToReceive, readyToSend);
    EXPECT_FALSE(readyToReceive);
    EXPECT_FALSE(readyToSend);
}

/*
 * @tc.name: RSProfilerSocketSendTest
 * @tc.desc: Test public method socket send
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSProfilerSocketTest, RSProfilerSocketSendTest, testing::ext::TestSize.Level1)
{
    Socket s;
    std::vector<uint8_t> sendData {0x04, 0x03, 0x02, 0x08, 0x1a};
    std::vector<uint8_t> recvData(5);
    EXPECT_NO_THROW({
        s.SendWhenReady(sendData.data(), sendData.size() * sizeof(uint8_t));
    });
    EXPECT_FALSE(s.ReceiveWhenReady(recvData.data(), recvData.size() * sizeof(uint8_t)));
    size_t size = recvData.size() * sizeof(uint8_t);
    EXPECT_TRUE(s.Receive(recvData.data(), size));
}

} // namespace OHOS::Rosen