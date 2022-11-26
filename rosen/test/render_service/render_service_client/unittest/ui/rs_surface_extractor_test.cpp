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

#include "gtest/gtest.h"
#include "ui/rs_surface_extractor.h"
#include "ui/rs_surface_node.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Rosen {
class RSSurfaceExtractorTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void RSSurfaceExtractorTest::SetUpTestCase() {}
void RSSurfaceExtractorTest::TearDownTestCase() {}
void RSSurfaceExtractorTest::SetUp() {}
void RSSurfaceExtractorTest::TearDown() {}

/**
 * @tc.name: ExtractRSSurface001
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSSurfaceExtractorTest, ExtractRSSurface001, TestSize.Level1)
{
    // RSSurfaceExtractor cannot be created,
    //      which constructor is privated.
    //      Only use its static function.
    RSSurfaceNodeConfig c;

    RSSurfaceNode::SharedPtr surfaceNode = RSSurfaceNode::Create(c);
    ASSERT_TRUE(surfaceNode != nullptr);

    RSSurfaceExtractor::ExtractRSSurface(surfaceNode);
    auto node = RSSurfaceExtractor::ExtractRSSurface(nullptr);
    ASSERT_TRUE(node == nullptr);
}
} // namespace OHOS::Rosen