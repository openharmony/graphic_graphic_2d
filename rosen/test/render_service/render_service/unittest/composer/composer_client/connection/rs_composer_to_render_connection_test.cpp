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
#include "rs_composer_to_render_connection.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Rosen {
class RSComposerToRenderConnectionTest : public Test {};

/**
 * Function: Impl_ReleaseLayerBuffers_And_Notify
 * Type: Function
 * Rank: Important(2)
 * EnvConditions: N/A
 * CaseDescription: 1. create RSComposerToRenderConnection implementation
 *                  2. call ReleaseLayerBuffers with non-existent screenId
 *                  3. verify returns error
 *                  4. call NotifyLppLayerToRender and expect OK
 */
HWTEST_F(RSComposerToRenderConnectionTest, Impl_ReleaseLayerBuffers_And_Notify, TestSize.Level1)
{
    RSComposerToRenderConnection impl;
    ReleaseLayerBuffersInfo info;
    info.screenId = 999u; // no client, hit error branch inside
    GraphicPresentTimestamp ts { GRAPHIC_DISPLAY_PTS_TIMESTAMP, 1 };
    info.timestampVec.push_back(std::tuple(static_cast<RSLayerId>(1u), false, ts));
    sptr<SyncFence> fence = sptr<SyncFence>::MakeSptr(-1);
    info.releaseBufferFenceVec.push_back(std::tuple(static_cast<RSLayerId>(1u), nullptr, fence));
    info.lastSwapBufferTime = 0;
    int32_t r = impl.ReleaseLayerBuffers(info);
    EXPECT_NE(r, COMPOSITOR_ERROR_OK);

    // Notify path should return OK
    std::set<uint64_t> ids { 1u };
    r = impl.NotifyLppLayerToRender(1u, ids);
    EXPECT_EQ(r, COMPOSITOR_ERROR_OK);
}
} // namespace OHOS::Rosen
