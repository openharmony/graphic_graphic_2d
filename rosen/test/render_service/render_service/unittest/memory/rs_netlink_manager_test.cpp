/*
 * Copyright (c) 2026 Huawei Device Co., Ltd.
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

#include <string>
#include <fstream>
#include <securec.h>
#include "gtest/gtest.h"
#include "pipeline/rs_test_util.h"
#include "memory/rs_netlink_manager.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Rosen {
std::string g_logNMMsg;
void NetlinkManagerLogCallback(
    const LogType type, const LogLevel level, const unsigned int domain, const char* tag, const char* msg)
{
    g_logNMMsg = msg;
}

class RSNetlinkManagerTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void RSNetlinkManagerTest::SetUpTestCase() {}
void RSNetlinkManagerTest::TearDownTestCase() {}
void RSNetlinkManagerTest::SetUp() {}
void RSNetlinkManagerTest::TearDown() {}

/**
 * @tc.name: RSNetlinkManagerInstance001
 * @tc.desc: Test RSNetlinkManager singleton instance
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSNetlinkManagerTest, RSNetlinkManagerInstance001, TestSize.Level1)
{
    RSNetlinkManager& instance1 = RSNetlinkManager::Instance();
    RSNetlinkManager& instance2 = RSNetlinkManager::Instance();
    ASSERT_EQ(&instance1, &instance2) << "Multiple singleton instances detected";
}

/**
 * @tc.name: RSNetlinkListenerParseMsgAndEvent001
 * @tc.desc: Test ParseMsgAndEvent with valid memory over limit event
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSNetlinkManagerTest, RSNetlinkListenerParseMsgAndEvent001, TestSize.Level1)
{
    g_logNMMsg.clear();
    LOG_SetCallback(NetlinkManagerLogCallback);
    int32_t socket = -1;
    RSNetlinkListener listener(socket);
    std::string recvInfo = "ACTION=MEMORY_OVER_LIMIT-pid=" + std::to_string(getpid());
    char buf[50];
    strcpy_s(buf, 50, recvInfo.c_str());
    bool res = listener.ParseMsgAndEvent(buf, 50);
    EXPECT_TRUE(res);
    std::string recvInfo1 = "ACTION=MEMORY_OVER_LIMIT-pid=00";
    strcpy_s(buf, 50, recvInfo1.c_str());
    bool res1 = listener.ParseMsgAndEvent(buf, 50);
    EXPECT_FALSE(res1);
    char buf1[1030];
    strcpy_s(buf1, 1030, recvInfo1.c_str());
    bool res2 = listener.ParseMsgAndEvent(buf1, 1030);
    EXPECT_FALSE(res2);
}

/**
 * @tc.name: RSNetlinkListenerStartListener001
 * @tc.desc: Test StartListener with invalid socket
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSNetlinkManagerTest, RSNetlinkListenerStartListener001, TestSize.Level1)
{
    int32_t socket = -1;
    RSNetlinkListener listener(socket);
    bool result = listener.StartListener();
    EXPECT_FALSE(result);
}

/**
 * @tc.name: RSNetlinkManagerStart001
 * @tc.desc: Test RSNetlinkManager Start
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSNetlinkManagerTest, RSNetlinkManagerStart001, TestSize.Level1)
{
    g_logNMMsg.clear();
    LOG_SetCallback(NetlinkManagerLogCallback);
    RSNetlinkManager& manager = RSNetlinkManager::Instance();
    bool result = manager.Start();
    EXPECT_TRUE(result);
    manager.Stop();
    EXPECT_TRUE(g_logNMMsg.find("netlink socket failed") == std::string::npos);
}

} // namespace OHOS::Rosen