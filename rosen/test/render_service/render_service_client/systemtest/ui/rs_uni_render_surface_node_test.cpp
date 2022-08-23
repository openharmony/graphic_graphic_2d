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

#include "platform/common/rs_system_properties.h"
#include "ui/rs_surface_node.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Rosen {
class RSUniRenderSurfaceNodeTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
};

void RSUniRenderSurfaceNodeTest::SetUpTestCase() {}
void RSUniRenderSurfaceNodeTest::TearDownTestCase() {}

namespace {
/**
 * @tc.name: TestSurfaceNodeSurface001
 * @tc.desc: Verify the non nullptr surface of (window) surface node for non-uni application in uni render case
 * @tc.type: FUNC
 * @tc.require: issueI54AGD
 */
HWTEST_F(RSUniRenderSurfaceNodeTest, TestSurfaceNodeSurface001, Function | MediumTest | Level1)
{
    RSSurfaceNodeConfig config;
    auto surfaceNode = RSSurfaceNode::Create(config);
    ASSERT_TRUE(surfaceNode != nullptr);
    ASSERT_TRUE(surfaceNode->GetSurface() != nullptr);
}

/**
 * @tc.name: TestSurfaceNodeSurface002
 * @tc.desc: Verify the nullptr surface of (window) surface node for uni application in uni render case
 * @tc.type: FUNC
 * @tc.require: issueI54AGD
 */
HWTEST_F(RSUniRenderSurfaceNodeTest, TestSurfaceNodeSurface002, Function | MediumTest | Level1)
{
    RSSystemProperties::GetUniRenderEnabled();
    RSSurfaceNodeConfig config;
    auto surfaceNode = RSSurfaceNode::Create(config);
    ASSERT_TRUE(surfaceNode != nullptr);
    ASSERT_TRUE(surfaceNode->GetSurface() != nullptr || RSSystemProperties::GetUniRenderEnabled());
}

/**
 * @tc.name: TestSurfaceNodeSurface003
 * @tc.desc: Verify the non nullptr surface of (surfaceView) surface node in uni render case
 * @tc.type: FUNC
 * @tc.require: issueI54AGD
 */
HWTEST_F(RSUniRenderSurfaceNodeTest, TestSurfaceNodeSurface003, Function | MediumTest | Level1)
{
    RSSurfaceNodeConfig config;
    auto surfaceNode = RSSurfaceNode::Create(config, false);
    ASSERT_TRUE(surfaceNode != nullptr);
    ASSERT_TRUE(surfaceNode->GetSurface() != nullptr);
}
} // namespace
} // namespace OHOS::Rosen
