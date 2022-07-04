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
#include "ui/rs_ui_share_context.h"
using namespace testing;
using namespace testing::ext;

namespace OHOS::Rosen {
class RSUIShareContextTest : public testing::Test {
public:
    static void SetUpTestCase() {}
    static void TearDownTestCase() {}
    void SetUp() override {}
    void TearDown() override {}
};

/**
 * @tc.name: GetInstance001
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSUIShareContextTest, GetInstance001, TestSize.Level1)
{
    auto& rs = RSUIShareContext::GetInstance();
    (void)(rs);
}

/**
 * @tc.name: GetRsRenderContext001
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSUIShareContextTest, GetRsRenderContext001, TestSize.Level1)
{
    auto context = RSUIShareContext::GetInstance().GetRsRenderContext();
    (void)(context);
}
} // namespace OHOS::Rosen
