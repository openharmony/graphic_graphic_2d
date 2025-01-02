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
 * @tc.desc: Test Connected
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSProfilerSocketTest, RSProfilerSocketConnectedTest, testing::ext::TestSize.Level1)
{
    Socket s;
    s.Open(5050);
    s.Shutdown();
    EXPECT_FALSE(s.Connected());
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
 * @tc.name: RSProfilerSocketAvailableTest
 * @tc.desc: Test public method Available
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSProfilerSocketTest, RSProfilerSocketAvailableTest, testing::ext::TestSize.Level1)
{
    Socket s;
    const auto size = s.Available();
    EXPECT_EQ(size, 0);
}

} // namespace OHOS::Rosen