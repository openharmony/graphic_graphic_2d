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
#include "rs_profiler.h"
#include "rs_profiler_network.h"
#include "platform/common/rs_system_properties.h"
 
using namespace testing;
using namespace testing::ext;
 
namespace OHOS::Rosen {
class RSProfilerTest : public testing::Test {
public:
    static void SetUpTestCase() {};
    static void TearDownTestCase() {};
    void SetUp() override
    {
        Network::outgoing_ = {};
        Network::incoming_ = {};
    };
    void TearDown() override {};
};
 
/*
 * @tc.name: Interface Test
 * @tc.desc: RSProfiler Interface Test
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSProfilerTest, InterfaceTest, testing::ext::TestSize.Level1)
{
    RSProfiler::testing_ = true;
    EXPECT_NO_THROW({
        RSProfiler::Init(nullptr);
        RSProfiler::OnFrameBegin();
        RSProfiler::OnRenderBegin();
        RSProfiler::OnRenderEnd();
        RSProfiler::OnFrameEnd();
        RSProfiler::OnProcessCommand();
        Network::Stop();
    });
    RSProfiler::testing_ = false;
    EXPECT_NO_THROW({
        RSProfiler::Init(nullptr);
        RSProfiler::OnFrameBegin();
        RSProfiler::OnRenderBegin();
        RSProfiler::OnRenderEnd();
        RSProfiler::OnFrameEnd();
        RSProfiler::OnProcessCommand();
        Network::Stop();
    });
}
 
/*
 * @tc.name: Info Test
 * @tc.desc: RSProfiler Info Test
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSProfilerTest, InfoTest, testing::ext::TestSize.Level1)
{
    RSProfiler::testing_ = true;
    std::vector<std::string> cmds {"info", "freq", "fixenv",
        "params", "rscon_print", "rstree_contains", "rstree_pid"};
    EXPECT_NO_THROW({
        Network::PushCommand(cmds);
        for (auto cmd : cmds) {
            RSProfiler::ProcessCommands();
        }
    });
}
 
/*
 * @tc.name: RSTreeTest
 * @tc.desc: RSProfiler RSTree Test
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSProfilerTest, RSTreeTest, testing::ext::TestSize.Level1)
{
    RSProfiler::testing_ = true;
    std::vector<std::string> cmds {
        "rstree_save_frame 1", "rstree_load_frame 1",
    };
    EXPECT_NO_THROW({
        RSProfiler::Init(nullptr);
        Network::PushCommand(cmds);
        for (auto cmd : cmds) {
            RSProfiler::ProcessCommands();
        }
        Network::Stop();
    });
}
} // namespace OHOS::Rosen