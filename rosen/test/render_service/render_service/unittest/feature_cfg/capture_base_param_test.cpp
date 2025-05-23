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

#include <gtest/gtest.h>
#include <test_header.h>

#include "capture_base_param.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Rosen {
class CaptureBaseParamTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp();
    void TearDown();
};

void CaptureBaseParamTest::SetUpTestCase() {}
void CaptureBaseParamTest::TearDownTestCase() {}
void CaptureBaseParamTest::SetUp() {}
void CaptureBaseParamTest::TearDown() {}

/*
 * @tc.name: IsSnapshotWithDMAEnabled
 * @tc.desc: Test IsSnapshotWithDMAEnabled
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(CaptureBaseParamTest, IsSnapshotWithDMAEnabled, Function | SmallTest | Level1)
{
    std::shared_ptr<CaptureBaseParam> captureBaseParam = std::make_shared<CaptureBaseParam>();
    captureBaseParam->SetSnapshotWithDMAEnabled(true);
    EXPECT_EQ(captureBaseParam->IsSnapshotWithDMAEnabled(), true);
    captureBaseParam->SetSnapshotWithDMAEnabled(false);
    EXPECT_EQ(captureBaseParam->IsSnapshotWithDMAEnabled(), false);
}
} // namespace OHOS::Rosen