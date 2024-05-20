/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, Hardware
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <gtest/gtest.h>

#include "platform/ohos/rs_vsync_client_ohos.h"
#include "platform/drawing/rs_vsync_client.h"
#include "platform/ohos/rs_render_service_connect_hub.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
class RSVsyncClientTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;

    std::unique_ptr<RSVsyncClient> vsyncClient = nullptr;
    static inline RSVsyncClient::VsyncCallback cb = nullptr;
};

void RSVsyncClientTest::SetUpTestCase()
{
    cb = [](int64_t timestamp) {};
}
void RSVsyncClientTest::TearDownTestCase() {}
void RSVsyncClientTest::SetUp()
{
    vsyncClient = RSVsyncClient::Create();
}
void RSVsyncClientTest::TearDown() {}

/**
 * @tc.name: SetVsyncCallback Test
 * @tc.desc: SetVsyncCallback Test
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSVsyncClientTest, SetVsyncCallback_Test, TestSize.Level1)
{
    ASSERT_NE(vsyncClient, nullptr);
    vsyncClient->SetVsyncCallback(cb);
}
} // namespace Rosen
} // namespace OHOS