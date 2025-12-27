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

#include "gtest/gtest.h"

#include "info_collection/rs_hdr_collection.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Rosen {
class RsHdrCollectionTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void RsHdrCollectionTest::SetUpTestCase() {}
void RsHdrCollectionTest::TearDownTestCase() {}
void RsHdrCollectionTest::SetUp() {}
void RsHdrCollectionTest::TearDown() {}

/**
 * @tc.name: HandleHdrState001
 * @tc.desc: HandleHdrState test
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RsHdrCollectionTest, HandleHdrState001, TestSize.Level1)
{
    std::shared_ptr<RsHdrCollection> rsHdrCollection = std::make_shared<RsHdrCollection>();
    ASSERT_NE(rsHdrCollection, nullptr);
    rsHdrCollection->HandleHdrState(true);
    EXPECT_TRUE(rsHdrCollection->isHdrOn_);
    rsHdrCollection->HandleHdrState(false);
    EXPECT_FALSE(rsHdrCollection->isHdrOn_);
}

/**
 * @tc.name: ResetHdrOnDuration001
 * @tc.desc: ResetHdrOnDuration test
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RsHdrCollectionTest, ResetHdrOnDuration001, TestSize.Level1)
{
    std::shared_ptr<RsHdrCollection> rsHdrCollection = std::make_shared<RsHdrCollection>();
    ASSERT_NE(rsHdrCollection, nullptr);
    rsHdrCollection->hdrOnDuration_ = 1;
    rsHdrCollection->ResetHdrOnDuration();
    EXPECT_EQ(rsHdrCollection->hdrOnDuration_, 0);
}

/**
 * @tc.name: GetHdrOnDuration001
 * @tc.desc: GetHdrOnDuration test
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RsHdrCollectionTest, GetHdrOnDuration001, TestSize.Level1)
{
    std::shared_ptr<RsHdrCollection> rsHdrCollection = std::make_shared<RsHdrCollection>();
    ASSERT_NE(rsHdrCollection, nullptr);
    rsHdrCollection->hdrOnDuration_ = 1;
    EXPECT_EQ(rsHdrCollection->GetHdrOnDuration(), 1);
}
} // namespace OHOS::Rosen