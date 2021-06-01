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

#include "window_manager_proxy_test.h"
#include <iostream>
#include <display_type.h>
#include <gtest/gtest.h>
#include <iservice_registry.h>
#include <system_ability_definition.h>
#include <window_manager.h>
#include <window_manager_common.h>
#include <window_manager_proxy.h>
#include <window_manager_controller_client.h>

namespace OHOS {
class WindowManagerProxyTest : public testing::Test {
public:
    static void SetUpTestCase(void){};
    static void TearDownTestCase(void){};
};

using namespace OHOS;
static sptr<IWindowManagerService> g_proxy;
static WindowConfig g_config = {
    .format = PIXEL_FMT_RGBA_8888,
};
static int WindowId = 0;
static int g_pos = 0;

void Init(void)
{
    g_config.height = WindowManager::GetInstance()->GetMaxHeight();
    g_config.width = WindowManager::GetInstance()->GetMaxWidth();
    g_config.pos_x = g_pos;
    g_config.pos_y = g_pos;

    auto sm = SystemAbilityManagerClient::GetInstance().GetSystemAbilityManager();
    sptr<IRemoteObject> object = sm->GetSystemAbility(WINDOW_MANAGER_ID);
    g_proxy = iface_cast<IWindowManagerService>(object);
    WindowId = g_proxy->CreateWindow(g_config);
}

HWTEST_F(WindowManagerProxyTest, CreateWindow, testing::ext::TestSize.Level0)
{
    Init();
    ASSERT_GE(WindowId, 5000);
}
HWTEST_F(WindowManagerProxyTest, SwitchTop, testing::ext::TestSize.Level0)
{
    g_proxy->SwitchTop(WindowId);
}
HWTEST_F(WindowManagerProxyTest, DestroyWindow, testing::ext::TestSize.Level0)
{
    g_proxy->DestroyWindow(WindowId);
}
}
