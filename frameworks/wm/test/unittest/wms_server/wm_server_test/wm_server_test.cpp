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

#include "wm_server_test.h"
#include <display_type.h>
#include <gtest/gtest.h>
#include <refbase.h>
#include <window_manager_server.h>

namespace OHOS {
using namespace OHOS;
sptr<WindowManagerServer> g_server;
WindowConfig g_config = {0};
static int g_WinId = 0;
static int g_height = 1920;
static int g_width = 1080;
static int g_pos_x = 300;
static int g_pos_y = 100;
class WmServerTest : public testing::Test {
public:
    static void SetUpTestCase(void)
    {
        g_config.height = g_height;
        g_config.width = g_width;
        g_config.format = PIXEL_FMT_RGBA_8888;
        g_config.pos_x = g_pos_x;
        g_config.pos_y = g_pos_y;
        g_server = new WindowManagerServer();
    }
};

HWTEST_F(WmServerTest, CreateWindow, testing::ext::TestSize.Level0)
{
    g_WinId = g_server->CreateWindow(g_config);
    ASSERT_NE(g_WinId, 0);
}

HWTEST_F(WmServerTest, SwitchTop, testing::ext::TestSize.Level0)
{
    g_server->SwitchTop(g_WinId);
}

HWTEST_F(WmServerTest, DestroyWindow, testing::ext::TestSize.Level0){
    g_server->DestroyWindow(g_WinId);
}
}
