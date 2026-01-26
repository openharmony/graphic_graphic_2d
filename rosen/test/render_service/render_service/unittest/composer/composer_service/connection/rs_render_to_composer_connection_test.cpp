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
#include <memory>
#include <set>

#ifdef ENABLE_SERVER_CONN_UT
#include "rs_render_to_composer_connection.h"
#include "rs_render_composer_agent.h"
#include "rs_composer_to_render_connection.h"
#include "rs_layer_transaction_data.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Rosen {
class RSRenderToComposerConnectionTest : public Test {};

/**
 * Function: Methods_Call_WithNullAgent
 * Type: Function
 * Rank: Important(2)
 * EnvConditions: N/A
 * CaseDescription: 1. create RSRenderToComposerConnection with agent holding null composer
 *                  2. call CommitLayers with nullptr transaction
 *                  3. call ClearFrameBuffers
 *                  4. call CleanLayerBufferBySurfaceId with 0 and non-zero ids
 *                  5. call OnScreenVBlankIdleCallback
 *                  6. call ClearRedrawGPUCompositionCache
 *                  7. call SetScreenBacklight; ensure no crash
 */
HWTEST_F(RSRenderToComposerConnectionTest, Methods_Call_WithNullAgent, TestSize.Level1)
{
    std::shared_ptr<RSRenderComposerAgent> agent =
        std::make_shared<RSRenderComposerAgent>(
            std::shared_ptr<RSRenderComposer>(nullptr));
    RSRenderToComposerConnection conn("conn", 1u, agent);

    std::unique_ptr<RSLayerTransactionData> tx(nullptr);
    conn.CommitLayers(tx);

    conn.ClearFrameBuffers();

    conn.CleanLayerBufferBySurfaceId(0u);
    conn.CleanLayerBufferBySurfaceId(123u);

    conn.OnScreenVBlankIdleCallback(1u, 0u);

    std::unordered_set<uint64_t> ids { 1u };
    conn.ClearRedrawGPUCompositionCache(ids);

    conn.SetScreenBacklight(10u);

    SUCCEED();
}

/**
 * Function: Methods_Call_WithNullAgentPtr
 * Type: Function
 * Rank: Important(2)
 * EnvConditions: N/A
 * CaseDescription: 1. create RSRenderToComposerConnection with nullptr agent
 *                  2. call all methods to cover null-agent guard branches
 */
HWTEST_F(RSRenderToComposerConnectionTest, Methods_Call_WithNullAgentPtr, TestSize.Level1)
{
    std::shared_ptr<RSRenderComposerAgent> agent = nullptr;
    RSRenderToComposerConnection conn("conn", 2u, agent);

    std::unique_ptr<RSLayerTransactionData> tx(new RSLayerTransactionData());
    conn.CommitLayers(tx);
    conn.ClearFrameBuffers();
    conn.CleanLayerBufferBySurfaceId(1u);
    conn.OnScreenVBlankIdleCallback(2u, 0u);
    std::unordered_set<uint64_t> ids { 2u };
    conn.ClearRedrawGPUCompositionCache(ids);
    conn.SetScreenBacklight(20u);
    SUCCEED();
}

/**
 * Function: Connection_SetComposerToRenderConnection_NullAgent
 * Type: Function
 * Rank: Important(2)
 * CaseDescription: 1. create connection with nullptr agent
 *                  2. call SetComposerToRenderConnection to hit illegal-param branch
 */
HWTEST_F(RSRenderToComposerConnectionTest, Connection_SetComposerToRenderConnection_NullAgent, TestSize.Level1)
{
    std::shared_ptr<RSRenderComposerAgent> agent = nullptr;
    RSRenderToComposerConnection conn("conn", 3u, agent);
    sptr<RSComposerToRenderConnection> ctr = sptr<RSComposerToRenderConnection>::MakeSptr();
    conn.SetComposerToRenderConnection(ctr);
    SUCCEED();
}

/**
 * Function: Connection_PreAllocProtectedFrameBuffers_NullAgent
 * Type: Function
 * Rank: Important(2)
 * CaseDescription: 1. create connection with nullptr agent
 *                  2. call PreAllocProtectedFrameBuffers to hit illegal-param branch
 */
HWTEST_F(RSRenderToComposerConnectionTest, Connection_PreAllocProtectedFrameBuffers_NullAgent, TestSize.Level1)
{
    std::shared_ptr<RSRenderComposerAgent> agent = nullptr;
    RSRenderToComposerConnection conn("conn", 4u, agent);
    sptr<SurfaceBuffer> sb = SurfaceBuffer::Create();
    conn.PreAllocProtectedFrameBuffers(sb);
    SUCCEED();
}
} // namespace OHOS::Rosen
#endif // ENABLE_SERVER_CONN_UT 
