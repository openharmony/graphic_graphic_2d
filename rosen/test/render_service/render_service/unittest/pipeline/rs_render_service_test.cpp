/*
 * Copyright (c) 2023 Huawei Device Co., Ltd.
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
#include "limit_number.h"
#include "pipeline/rs_render_service.h"

using namespace testing;
using namespace testing::ext;


namespace OHOS::Rosen {
class RSRenderServiceUnitTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void RSRenderServiceUnitTest::SetUpTestCase() {}
void RSRenderServiceUnitTest::TearDownTestCase() {}
void RSRenderServiceUnitTest::SetUp() {}
void RSRenderServiceUnitTest::TearDown() {}

/**
 * @tc.name: TestRemoveConnection001
 * @tc.desc: RemoveConnection test.
 * @tc.type: FUNC
 * @tc.require: issueI7G75T
 */
HWTEST_F(RSRenderServiceUnitTest, TestRemoveConnection001, TestSize.Level1)
{
    sptr<RSRenderService> renderService(new RSRenderService());
    ASSERT_NE(renderService, nullptr);
    renderService->RemoveConnection(nullptr);
}
} // namespace OHOS::Rosen
