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
#include <memory>
#include <unistd.h>

#include "pipeline/rs_qos_thread.h"
#include "platform/common/rs_innovation.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Rosen {
class RSQosThreadTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void RSQosThreadTest::SetUpTestCase() {}
void RSQosThreadTest::TearDownTestCase() {
    RSQosThread::GetInstance()->SetQosCal(false);
}
void RSQosThreadTest::SetUp() {}
void RSQosThreadTest::TearDown() {}

/*
 * @tc.name: QosThreadStartAndStop
 * @tc.desc: 
 * @tc.type:
 * @tc.require:
 * @tc.author:
 */
HWTEST_F(RSQosThreadTest, QosThreadStartAndStop, TestSize.Level1)
{
    // so not loaded 
    RSQosThread::ThreadStart();
    RSQosThread::ThreadStop();
    RSQosThread::ResetQosPid();
}

/*
 * @tc.name: QosOnRSVisibilityChangeCB
 * @tc.desc:
 * @tc.type:
 * @tc.require:
 * @tc.author:
 */
HWTEST_F(RSQosThreadTest,  QosOnRSVisibilityChangeCB, TestSize.Level1)
{
    std::map<uint32_t, bool> pidVisMap;

    // qosCal not setted
    RSQosThread::GetInstance()->OnRSVisibilityChangeCB(pidVisMap);

    // qosCal not is setted
    RSQosThread::GetInstance()->SetQosCal(true);
    bool qosCal = RSQosThread::GetInstance()->GetQosCal();
    ASSERT_EQ(qosCal, true);
    RSQosThread::GetInstance()->OnRSVisibilityChangeCB(pidVisMap);
}

} // OHOS::Rosen
