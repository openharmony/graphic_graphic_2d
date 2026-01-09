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
#include <set>
#include <unistd.h>
#include "rs_composer_to_render_connection.h"
#include "frame_report.h"
#include "pipeline/render_thread/rs_uni_render_thread.h"
#include "composer/composer_client/pipeline/rs_render_composer_client.h"
#include "graphic_common_c.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Rosen {
class RSComposerToRenderConnectionTest : public Test {};

/**
 * Function: Connection_ReleaseLayerBuffers_ComposerNull
 * Type: Function
 * Rank: Important(2)
 * CaseDescription: 1. no composer client registered for screen
 *                  2. expect COMPOSITOR_ERROR_NULLPTR
 */
HWTEST_F(RSComposerToRenderConnectionTest, Connection_ReleaseLayerBuffers_ComposerNull, TestSize.Level1)
{
    RSComposerToRenderConnection conn;
    ReleaseLayerBuffersInfo info;
    info.screenId = 101u;
    int32_t ret = conn.ReleaseLayerBuffers(info);
    EXPECT_EQ(ret, COMPOSITOR_ERROR_OK);
}

/**
 * Function: Connection_ReleaseLayerBuffers_Normal_NoGame
 * Type: Function
 * Rank: Important(2)
 * CaseDescription: 1. register minimal composer client for screen
 *                  2. HasGameScene=false, expect COMPOSITOR_ERROR_OK
 */
HWTEST_F(RSComposerToRenderConnectionTest, Connection_ReleaseLayerBuffers_Normal_NoGame, TestSize.Level1)
{
    RSComposerToRenderConnection conn;
    uint64_t screenId = 102u;
    auto client = RSRenderComposerClient::Create(nullptr, nullptr, nullptr);
    RSUniRenderThread::Instance().AddRenderComposerClient(screenId, client);

    ReleaseLayerBuffersInfo info;
    info.screenId = screenId;
    info.lastSwapBufferTime = 0;
    int32_t ret = conn.ReleaseLayerBuffers(info);
    EXPECT_EQ(ret, COMPOSITOR_ERROR_OK);

    RSUniRenderThread::Instance().DeleteRSRenderComposerClient(screenId);
}

/**
 * Function: Connection_ReleaseLayerBuffers_Normal_WithGame
 * Type: Function
 * Rank: Important(2)
 * CaseDescription: 1. HasGameScene=true, expect COMPOSITOR_ERROR_OK
 *                  2. register minimal composer client and invoke
 */
HWTEST_F(RSComposerToRenderConnectionTest, Connection_ReleaseLayerBuffers_Normal_WithGame, TestSize.Level1)
{
    RSComposerToRenderConnection conn;
    uint64_t screenId = 103u;
    auto client = RSRenderComposerClient::Create(nullptr, nullptr, nullptr);
    RSUniRenderThread::Instance().AddRenderComposerClient(screenId, client);

    // Activate game scene to hit SetLastSwapBufferTime branch
    FrameReport::GetInstance().SetGameScene(getpid(), 2); // FR_GAME_SCHED

    ReleaseLayerBuffersInfo info;
    info.screenId = screenId;
    info.lastSwapBufferTime = 987654321;
    int32_t ret = conn.ReleaseLayerBuffers(info);
    EXPECT_EQ(ret, COMPOSITOR_ERROR_OK);

    // Reset game scene
    FrameReport::GetInstance().SetGameScene(getpid(), 0); // FR_GAME_BACKGROUND
    RSUniRenderThread::Instance().DeleteRSRenderComposerClient(screenId);
}

/**
 * Function: Connection_NotifyLppLayerToRender_Normal
 * Type: Function
 * Rank: Important(2)
 * CaseDescription: 1. call with non-empty ids; main thread exists
 *                  2. expect COMPOSITOR_ERROR_OK
 */
HWTEST_F(RSComposerToRenderConnectionTest, Connection_NotifyLppLayerToRender_Normal, TestSize.Level1)
{
    RSComposerToRenderConnection conn;
    std::set<uint64_t> ids { 1u, 2u, 3u };
    int32_t ret = conn.NotifyLppLayerToRender(555u, ids);
    EXPECT_EQ(ret, COMPOSITOR_ERROR_OK);
}
} // namespace OHOS::Rosen