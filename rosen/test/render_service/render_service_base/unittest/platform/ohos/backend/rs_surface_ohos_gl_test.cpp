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

#include "platform/ohos/backend/rs_surface_ohos_gl.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
class RSSurfaceOhosGlTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void RSSurfaceOhosGlTest::SetUpTestCase() {}
void RSSurfaceOhosGlTest::TearDownTestCase() {}
void RSSurfaceOhosGlTest::SetUp() {}
void RSSurfaceOhosGlTest::TearDown() {}

/**
 * @tc.name: ClearBuffer001
 * @tc.desc: test
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSSurfaceOhosGlTest, ClearBuffer001, TestSize.Level1)
{
    RSSurfaceOhosGl gl(Surface::CreateSurfaceAsConsumer());
    gl.ClearBuffer();
}

/**
 * @tc.name: ResetBufferAge001
 * @tc.desc: test
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSSurfaceOhosGlTest, ResetBufferAge001, TestSize.Level1)
{
    RSSurfaceOhosGl gl(Surface::CreateSurfaceAsConsumer());
    gl.ResetBufferAge();
}
} // namespace Rosen
} // namespace OHOS