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
#include "transaction/rs_transaction.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Rosen {
class RSTransactionTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void RSTransactionTest::SetUpTestCase() {}
void RSTransactionTest::TearDownTestCase() {}
void RSTransactionTest::SetUp() {}
void RSTransactionTest::TearDown() {}

/**
 * @tc.name: FlushImplicitTransaction001
 * @tc.desc:
 * @tc.type:
 * @tc.require:
 * @tc.author:
 */
HWTEST_F(RSTransactionTest, FlushImplicitTransaction001, TestSize.Level1)
{
    // RSRenderThreadClient cannot be created,
    //      which constructor is privated.
    //      Only use its static function.
    RSTransaction::FlushImplicitTransaction();
}
} // namespace OHOS::Rosen
