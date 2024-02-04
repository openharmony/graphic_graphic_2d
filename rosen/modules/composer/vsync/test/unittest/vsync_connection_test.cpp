/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
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
#include "vsync_distributor.h"
#include "vsync_generator.h"
#include "vsync_controller.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
class VSyncConnectionTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();

    static inline sptr<VSyncController> vsyncController = nullptr;
    static inline sptr<VSyncDistributor> vsyncDistributor = nullptr;
    static inline sptr<VSyncGenerator> vsyncGenerator = nullptr;
    static inline sptr<VSyncConnection> vsyncConnection = nullptr;
};

void VSyncConnectionTest::SetUpTestCase()
{
    vsyncGenerator = CreateVSyncGenerator();
    vsyncController = new VSyncController(vsyncGenerator, 0);
    vsyncDistributor = new VSyncDistributor(vsyncController, "VSyncConnection");
    vsyncConnection = new VSyncConnection(vsyncDistributor, "VSyncConnection");
}

void VSyncConnectionTest::TearDownTestCase()
{
    DestroyVSyncGenerator();
    vsyncController = nullptr;
    vsyncGenerator = nullptr;
    vsyncDistributor = nullptr;
    vsyncConnection = nullptr;
}

namespace {
/*
* Function: RequestNextVSync001
* Type: Function
* Rank: Important(2)
* EnvConditions: N/A
* CaseDescription: 1. call RequestNextVSync
 */
HWTEST_F(VSyncConnectionTest, RequestNextVSync001, Function | MediumTest| Level3)
{
    ASSERT_EQ(VSyncConnectionTest::vsyncConnection->RequestNextVSync(), VSYNC_ERROR_INVALID_ARGUMENTS);
}

/*
* Function: RequestNextVSync002
* Type: Function
* Rank: Important(2)
* EnvConditions: N/A
* CaseDescription: 1. call RequestNextVSync
 */
HWTEST_F(VSyncConnectionTest, RequestNextVSync002, Function | MediumTest| Level3)
{
    VSyncConnectionTest::vsyncDistributor->AddConnection(VSyncConnectionTest::vsyncConnection);
    ASSERT_EQ(VSyncConnectionTest::vsyncConnection->RequestNextVSync(), VSYNC_ERROR_OK);
    VSyncConnectionTest::vsyncDistributor->RemoveConnection(VSyncConnectionTest::vsyncConnection);
}

/*
* Function: RequestNextVSync003
* Type: Function
* Rank: Important(2)
* EnvConditions: N/A
* CaseDescription: 1. call RequestNextVSync
 */
HWTEST_F(VSyncConnectionTest, RequestNextVSync003, Function | MediumTest| Level3)
{
    ASSERT_EQ(VSyncConnectionTest::vsyncConnection->RequestNextVSync("unknown", 0), VSYNC_ERROR_INVALID_ARGUMENTS);
}

/*
* Function: SetVSyncRate001
* Type: Function
* Rank: Important(2)
* EnvConditions: N/A
* CaseDescription: 1. call SetVSyncRate
 */
HWTEST_F(VSyncConnectionTest, SetVSyncRate001, Function | MediumTest| Level3)
{
    ASSERT_EQ(VSyncConnectionTest::vsyncConnection->SetVSyncRate(-2), VSYNC_ERROR_INVALID_ARGUMENTS);
}

/*
* Function: SetVSyncRate002
* Type: Function
* Rank: Important(2)
* EnvConditions: N/A
* CaseDescription: 1. call SetVSyncRate
 */
HWTEST_F(VSyncConnectionTest, SetVSyncRate002, Function | MediumTest| Level3)
{
    VSyncConnectionTest::vsyncDistributor->AddConnection(VSyncConnectionTest::vsyncConnection);
    ASSERT_EQ(VSyncConnectionTest::vsyncConnection->SetVSyncRate(1), VSYNC_ERROR_OK);
    VSyncConnectionTest::vsyncDistributor->RemoveConnection(VSyncConnectionTest::vsyncConnection);
}

/*
* Function: GetReceiveFd001
* Type: Function
* Rank: Important(2)
* EnvConditions: N/A
* CaseDescription: 1. call GetReceiveFd
 */
HWTEST_F(VSyncConnectionTest, GetReceiveFd001, Function | MediumTest| Level3)
{
    int32_t fd = -1;
    ASSERT_EQ(VSyncConnectionTest::vsyncConnection->GetReceiveFd(fd), VSYNC_ERROR_OK);
    ASSERT_NE(fd, -1);
}
} // namespace
} // namespace Rosen
} // namespace OHOS
