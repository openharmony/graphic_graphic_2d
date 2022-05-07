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

#include <memory>
#include "gtest/gtest.h"
#include "limit_number.h"
#include "pipeline/rs_render_service_listener.h"
#include "pipeline/rs_surface_render_node.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Rosen {
class RSRenderServiceListenerTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void RSRenderServiceListenerTest::SetUpTestCase() {}
void RSRenderServiceListenerTest::TearDownTestCase() {}
void RSRenderServiceListenerTest::SetUp() {}
void RSRenderServiceListenerTest::TearDown() {}

/**
 * @tc.name: CreateAndDestory001
 * @tc.desc:
 * @tc.type:
 * @tc.require:
 * @tc.author:
 */
HWTEST_F(RSRenderServiceListenerTest, CreateAndDestory001, TestSize.Level1)
{
    // nullptr test
    std::weak_ptr<RSSurfaceRenderNode> wp;
    RSRenderServiceListener rsRenderServiceListener(wp);
}

/**
 * @tc.name: OnBufferAvailable009
 * @tc.desc:
 * @tc.type:
 * @tc.require:
 * @tc.author:
 */
HWTEST_F(RSRenderServiceListenerTest, OnBufferAvailable009, TestSize.Level1)
{
    // nullptr test
    std::weak_ptr<RSSurfaceRenderNode> wp;
    RSRenderServiceListener rsRenderServiceListener(wp);
    rsRenderServiceListener.OnBufferAvailable();
}
} // namespace OHOS::Rosen
