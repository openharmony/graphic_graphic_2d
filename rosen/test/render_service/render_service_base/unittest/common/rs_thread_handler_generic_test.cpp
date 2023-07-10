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

#include "common/rs_thread_handler.h"

#include "common/rs_thread_handler_generic.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Rosen {
class RSThreadHandlerGenericTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void RSThreadHandlerGenericTest::SetUpTestCase() {}
void RSThreadHandlerGenericTest::TearDownTestCase() {}
void RSThreadHandlerGenericTest::SetUp() {}
void RSThreadHandlerGenericTest::TearDown() {}

/**
 * @tc.name: Create001
 * @tc.desc: test results of Create
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSThreadHandlerGenericTest, Create001, TestSize.Level1)
{
    /**
     * @tc.steps: step1. Create
     */
    auto rsThreadHandlerGeneric = RSThreadHandler::Create();
    ASSERT_NE(rsThreadHandlerGeneric, nullptr);
}
} // namespace OHOS::Rosen