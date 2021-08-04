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

#include "windows_manager_test.h"
#include <display_type.h>
#include <gtest/gtest.h>
#include <iostream>
#include <memory.h>
#include <window_manager.h>

namespace OHOS {
using namespace OHOS;
static int g_pos = 0;
static int g_width = 200;
static int g_height = 100;
static int32_t g_WindowId = 0;
static std::unique_ptr<Window> g_window;
static WindowConfig Config = {
    .width = g_width,
    .height = g_height,
    .format = PIXEL_FMT_RGBA_8888,
    .pos_x = g_pos,
    .pos_y = g_pos,
    .type = WINDOW_TYPE_NORMAL
};

class WindowTest : public testing::Test {
public:
	static void SetUpTestCase(void)
    {
        g_window = WindowManager::GetInstance()->CreateWindow(&Config); 
    };
};

class WindowManagerTest : public testing::Test {
public:
	static void SetUpTestCase(void)
    {   
    };
};

HWTEST_F(WindowManagerTest, GetInstance, testing::ext::TestSize.Level0)
{
    WindowManager * InstancePtr = WindowManager::GetInstance();
    ASSERT_NE(InstancePtr, nullptr);
}

HWTEST_F(WindowManagerTest, CreateWindow, testing::ext::TestSize.Level0)
{
    std::unique_ptr<Window> WinIdTemp = WindowManager::GetInstance()->CreateWindow(&Config);
    ASSERT_NE(WinIdTemp, nullptr);
}

HWTEST_F(WindowManagerTest, GetMaxWidth, testing::ext::TestSize.Level0)
{
    int iWidth = WindowManager::GetInstance()->GetMaxWidth();
    ASSERT_GE(iWidth, 0);
}

HWTEST_F(WindowManagerTest, GetMaxHeight, testing::ext::TestSize.Level0)
{
    int iheight = WindowManager::GetInstance()->GetMaxHeight();
    ASSERT_GE(iheight, 0);
}

HWTEST_F(WindowManagerTest, SwitchTop, testing::ext::TestSize.Level0)
{
    WindowManager::GetInstance()->SwitchTop(g_WindowId);
}

HWTEST_F(WindowTest, Show, testing::ext::TestSize.Level0)
{
    g_window->Show();
}
HWTEST_F(WindowTest, Hide, testing::ext::TestSize.Level0)
{
    g_window->Hide();
}
HWTEST_F(WindowTest, Move, testing::ext::TestSize.Level0)
{
    g_window->Move(2, 2);
}
HWTEST_F(WindowTest, SwitchTop, testing::ext::TestSize.Level0)
{
    g_window->SwitchTop();
}

HWTEST_F(WindowTest, ChangeWindowType, testing::ext::TestSize.Level0)
{
    g_window->ChangeWindowType(WINDOW_TYPE_NORMAL);
    g_window->ChangeWindowType(WINDOW_TYPE_STATUS_BAR);
    g_window->ChangeWindowType(WINDOW_TYPE_NAVI_BAR);
    g_window->ChangeWindowType(WINDOW_TYPE_ALARM_SCREEN);
    g_window->ChangeWindowType(WINDOW_TYPE_SYSTEM_UI);
    g_window->ChangeWindowType(WINDOW_TYPE_LAUNCHER);
    g_window->ChangeWindowType(WINDOW_TYPE_VIDEO);
}

HWTEST_F(WindowTest, ReSize, testing::ext::TestSize.Level0)
{
    g_window->ReSize(100, 100);
}

HWTEST_F(WindowTest, Rotate, testing::ext::TestSize.Level0)
{
    g_window->Rotate(WM_ROTATE_TYPE_NORMAL);
    g_window->Rotate(WM_ROTATE_TYPE_90);
    g_window->Rotate(WM_ROTATE_TYPE_180);
    g_window->Rotate(WM_ROTATE_TYPE_270);
    g_window->Rotate(WM_ROTATE_TYPE_FLIPPED);
    g_window->Rotate(WM_ROTATE_TYPE_FLIPPED_90);
    g_window->Rotate(WM_ROTATE_TYPE_FLIPPED_180);
    g_window->Rotate(WM_ROTATE_TYPE_FLIPPED_270);
}

void WindowInfoChangeCb(WindowInfo &info){}
HWTEST_F(WindowTest, RegistWindowInfoChangeCb, testing::ext::TestSize.Level0)
{
    g_window->RegistWindowInfoChangeCb(WindowInfoChangeCb);
}
HWTEST_F(WindowTest, RegistWindowInfoChangeCbNULL, testing::ext::TestSize.Level0)
{
    g_window->RegistWindowInfoChangeCb(nullptr);
}
}
