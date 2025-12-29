/**
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
#include "rs_layer_transaction_handler.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Rosen{
class RSLayerTransactionHandlerTest : public Test {};

/**
 * Function: Commit_WithNull_Connection_NoCrash
 * Type: Function
 * Rank: Important(2)
 * EnvConditions: N/A
 * CaseDescription: 1. construct handler with null connection
 *                  2. call CommitRSLayerTransaction
 *                  3. expect no crash and IsEmpty callable
 */
HWTEST(RSLayerTransactionHandlerTest, Commit_WithNull_Connection_NoCrash, TestSize.Level1)
{
    RSLayerTransactionHandler handler;
    ComposerInfo info;
    handler.CommitRSLayerTransaction(info, 0, "");
    EXPECT_TRUE(handler.IsEmpty());
}
}
