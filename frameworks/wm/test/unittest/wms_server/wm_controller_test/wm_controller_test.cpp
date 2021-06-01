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

#include "wm_controller_test.h"
#include <display_type.h>
#include <gtest/gtest.h>
#include <layer_controller.h>
#include <iremote_stub.h>

namespace OHOS {
using namespace OHOS;
static uint32_t g_WinId = 0;
static int g_WinWidth   = 600;
static int g_SubWidth   = 500;
static int g_WinHeight  = 600;
static int g_SubHeight  = 500;
static int g_PosX       = 200;
static int g_PosY       = 100;
class WmControllerTest : public testing::Test {
public:
    static void SetUpTestCase(void)
    {
    }
};

HWTEST_F(WmControllerTest, GetInstance, testing::ext::TestSize.Level0)
{
    LayerController *playerController = LayerController::GetInstance();
    ASSERT_NE(playerController, nullptr);
}

HWTEST_F(WmControllerTest, CreateWindow, testing::ext::TestSize.Level0)
{
    WindowConfig Config = {
        .width = g_WinWidth,
        .height = g_WinHeight,
        .format = PIXEL_FMT_RGBA_8888,
        .pos_x = g_PosX,
        .pos_y = g_PosY,
        .type = WINDOW_TYPE_NORMAL
    };
    g_WinId = LayerController::GetInstance()->CreateWindow(getpid(), Config);
    ASSERT_GE(g_WinId, 0);
}

HWTEST_F(WmControllerTest, DestroyWindow, testing::ext::TestSize.Level0)
{
    WindowConfig Config = {
        .width = g_SubWidth,
        .height = g_SubHeight,
        .format = PIXEL_FMT_RGBA_8888,
        .pos_x = g_PosX,
        .pos_y = g_PosY,
        .type = WINDOW_TYPE_NORMAL
    };
    int WinId1 = LayerController::GetInstance()->CreateWindow(getpid(), Config);
    ASSERT_GE(WinId1, 0);
    LayerController::GetInstance()->DestroyWindow(WinId1);
}

HWTEST_F(WmControllerTest, ChangeWindowTop, testing::ext::TestSize.Level0)
{
    LayerController::GetInstance()->ChangeWindowTop(g_WinId);
}
}
