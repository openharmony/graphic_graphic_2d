/*
 * Copyright (c) 2025 Huawei Device Co., Ltd.
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

#include "feature/hwc/rs_uni_hwc_prevalidate_util.h"
#include "foundation/graphic/graphic_2d/rosen/test/render_service/render_service/unittest/pipeline/rs_test_util.h"
#include "gtest/gtest.h"
#include "feature/mock/mock_dlsym.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Rosen {
class RSUniHwcPrevalidateUtilTest1 : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void RSUniHwcPrevalidateUtilTest1::SetUpTestCase()
{
    RSTestUtil::InitRenderNodeGC();
}
void RSUniHwcPrevalidateUtilTest1::TearDownTestCase() {}
void RSUniHwcPrevalidateUtilTest1::SetUp() {}
void RSUniHwcPrevalidateUtilTest1::TearDown() {}

/**
 * @tc.name: Init001
 * @tc.desc: test for Init when handleEventFunc_ is nullptr and handleEventFunc_ isn't nullptr
 * @tc.type: FUNC
 * @tc.require: issueIBQDHZ
 */
HWTEST_F(RSUniHwcPrevalidateUtilTest1, Init001, TestSize.Level1)
{
    g_handleEventFuncNull = true;
    RSUniHwcPrevalidateUtil util1;
    ASSERT_EQ(util1.handleEventFunc_, nullptr);
    util1.Init();
    g_handleEventFuncNull = false;

    RSUniHwcPrevalidateUtil util2;
    ASSERT_NE(util2.handleEventFunc_, nullptr);
    util2.Init();
}

/**
 * @tc.name: OnHwcEvent001
 * @tc.desc: test for OnHwcEvent when handleEventFunc_ isn't nullptr
 * @tc.type: FUNC
 * @tc.require: issueIBQDHZ
 */
HWTEST_F(RSUniHwcPrevalidateUtilTest1, OnHwcEvent001, TestSize.Level1)
{
    RSUniHwcPrevalidateUtil util;
    vector<int32_t> eventData;
    ASSERT_NE(util.handleEventFunc_, nullptr);
    util.OnHwcEvent(0, 0, eventData, nullptr);
}
}