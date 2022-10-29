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

#include <gtest/gtest.h>
#include <hilog/log.h>
#include <cmath>
#include <thread>

#include "local_socketpair.h"

using namespace testing::ext;

namespace OHOS {

static bool g_flag = false;
static void SendDataThread(LocalSocketPair *socketPair)
{
    int32_t data = 10;
    socketPair->SendData((void *)&data, sizeof(int32_t));
    g_flag = true;
}

class LocalSocketPairTest : public testing::Test {
public:
    static constexpr HiviewDFX::HiLogLabel LOG_LABEL = {LOG_CORE, 0, "LocalSocketPairTest"};

    static void SetUpTestCase()
    {}

    static void TearDownTestCase()
    {}
};

/*
* Function: CreateChannel001
* Type: Function
* Rank: Important(2)
* EnvConditions: N/A
* CaseDescription: CreateChannel
*/
HWTEST_F(LocalSocketPairTest, CreateChannel001, Function | SmallTest | Level2)
{
    LocalSocketPair socketPair;
    socketPair.CreateChannel(8, 8);
    int32_t ret = socketPair.CreateChannel(8, 8);
    ASSERT_EQ(ret, 0);
}
/*
* Function: ReceiveData001
* Type: Function
* Rank: Important(2)
* EnvConditions: N/A
* CaseDescription: ReceiveData
*/
HWTEST_F(LocalSocketPairTest, ReceiveData001, Function | SmallTest | Level2)
{
    LocalSocketPair socketPair;
    int32_t ret = socketPair.ReceiveData(nullptr, 0);
    ASSERT_EQ(ret, -1);
    socketPair.CreateChannel(8, 8);
    std::thread th(SendDataThread, &socketPair);
    th.join();
    while (!g_flag) {}
    int32_t data = 0;
    ret = socketPair.ReceiveData(&data, sizeof(int32_t));
    ASSERT_EQ(data, 10);
}
}