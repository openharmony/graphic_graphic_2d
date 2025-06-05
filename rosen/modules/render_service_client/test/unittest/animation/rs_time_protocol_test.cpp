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

#include "rs_animation_base_test.h"
#include "rs_animation_test_utils.h"

#include "animation/rs_animation_timing_protocol.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
using namespace ANIMATIONTEST;

class RSAnimationProtocolTest : public RSAnimationBaseTest {
};

/**
 * @tc.name: SetInterfaceName
 * @tc.desc: Verify the SetInterfaceName of time protocol
 * @tc.type: FUNC
 */
HWTEST_F(RSAnimationProtocolTest, SetInterfaceName, TestSize.Level1)
{
    /**
     * @tc.steps: step1. init
     */
    std::string test = "test"; // test string
    RSAnimationTimingProtocol timingProtocol;
    timingProtocol.SetInterfaceName(test);
    EXPECT_EQ(test, timingProtocol.GetInterfaceName());
}

} // namespace Rosen
} // namespace OHOS