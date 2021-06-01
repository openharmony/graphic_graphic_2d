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

void ShotDoneCallback(ImageInfo& imageInfo){}
HWTEST_F(WindowManagerTest, StartShotScreen, testing::ext::TestSize.Level0)
{
    WindowManager::GetInstance()->StartShotScreen(ShotDoneCallback); 
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

void PointerButtonCB(int serial, int button, int state, int time){}
HWTEST_F(WindowTest, RegistPointerButtonCb, testing::ext::TestSize.Level0)
{
    g_window->RegistPointerButtonCb(PointerButtonCB);
}

HWTEST_F(WindowTest, RegistPointerButtonCbNULL, testing::ext::TestSize.Level0)
{
    g_window->RegistPointerButtonCb(nullptr);
}

void PointerEnterCB(int32_t x, int32_t y, int32_t serila){}
HWTEST_F(WindowTest, RegistPointerEnterCb, testing::ext::TestSize.Level0)
{
    g_window->RegistPointerEnterCb(PointerEnterCB);
}
HWTEST_F(WindowTest, RegistPointerEnterCbNULL, testing::ext::TestSize.Level0)
{
    g_window->RegistPointerEnterCb(nullptr);
}

void PointerLeaveCB(int32_t serial){}
HWTEST_F(WindowTest, RegistPointerLeaveCb, testing::ext::TestSize.Level0)
{
    g_window->RegistPointerLeaveCb(PointerLeaveCB);
}
HWTEST_F(WindowTest, RegistPointerLeaveCbNULL, testing::ext::TestSize.Level0)
{
    g_window->RegistPointerLeaveCb(nullptr);
}

void PointerMotionCB(int32_t x, int32_t y, int32_t time){}
HWTEST_F(WindowTest, RegistPointerMotionCb, testing::ext::TestSize.Level0)
{
    g_window->RegistPointerMotionCb(PointerMotionCB);
}
HWTEST_F(WindowTest, RegistPointerMotionCbNULL, testing::ext::TestSize.Level0)
{
    g_window->RegistPointerMotionCb(nullptr);
}

void PointerAxisDiscreteCB(int32_t axis, int32_t discrete){}
HWTEST_F(WindowTest, RegistPointerAxisDiscreteCb, testing::ext::TestSize.Level0)
{
    g_window->RegistPointerAxisDiscreteCb(PointerAxisDiscreteCB);
}
HWTEST_F(WindowTest, RegistPointerAxisDiscreteCbNULL, testing::ext::TestSize.Level0)
{
    g_window->RegistPointerAxisDiscreteCb(nullptr);
}

void PointerAxisSourceCB(int32_t axisSource){}
HWTEST_F(WindowTest, RegistPointerAxisSourceCb, testing::ext::TestSize.Level0)
{
    g_window->RegistPointerAxisSourceCb(PointerAxisSourceCB);
}
HWTEST_F(WindowTest, RegistPointerAxisSourceCbNULL, testing::ext::TestSize.Level0)
{
    g_window->RegistPointerAxisSourceCb(nullptr);
}

void PointerAxisStopCB(int32_t time, int32_t axis){}
HWTEST_F(WindowTest, RegistPointerAxisStopCb, testing::ext::TestSize.Level0)
{
    g_window->RegistPointerAxisStopCb(PointerAxisStopCB);
}
HWTEST_F(WindowTest, RegistPointerAxisStopCbNULL, testing::ext::TestSize.Level0)
{
    g_window->RegistPointerAxisStopCb(nullptr);
}

void PointerAxisCB(int32_t time, int32_t axis, int32_t value){};
HWTEST_F(WindowTest, RegistPointerAxisCb, testing::ext::TestSize.Level0)
{
    g_window->RegistPointerAxisCb(PointerAxisCB);
}
HWTEST_F(WindowTest, RegistPointerAxisCbNULL, testing::ext::TestSize.Level0)
{
    g_window->RegistPointerAxisCb(nullptr);
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

void TouchUpCB(int32_t serial, int32_t time, int32_t id){}
HWTEST_F(WindowTest, RegistTouchUpCb, testing::ext::TestSize.Level0)
{
    g_window->RegistTouchUpCb(TouchUpCB);
}
HWTEST_F(WindowTest, RegistTouchUpCbNULL, testing::ext::TestSize.Level0)
{
    g_window->RegistTouchUpCb(nullptr);
}

void TouchDownCB(int32_t x, int32_t y, int32_t serial, int32_t time, int32_t id){}
HWTEST_F(WindowTest, RegistTouchDownCb, testing::ext::TestSize.Level0)
{
    g_window->RegistTouchDownCb(TouchDownCB);
}
HWTEST_F(WindowTest, RegistTouchDownCbNULL, testing::ext::TestSize.Level0)
{
    g_window->RegistTouchDownCb(nullptr);
}

void TouchEmotionCB(int32_t x, int32_t y, int32_t time, int32_t id){}
HWTEST_F(WindowTest, RegistTouchEmotionCb, testing::ext::TestSize.Level0)
{
    g_window->RegistTouchEmotionCb(TouchEmotionCB);
}
HWTEST_F(WindowTest, RegistTouchEmotionCbNULL, testing::ext::TestSize.Level0)
{
    g_window->RegistTouchEmotionCb(nullptr);
}

void TouchFrameCB(){}
HWTEST_F(WindowTest, RegistTouchFrameCb, testing::ext::TestSize.Level0)
{
    g_window->RegistTouchFrameCb(TouchFrameCB);
}
HWTEST_F(WindowTest, RegistTouchFrameCbNULL, testing::ext::TestSize.Level0)
{
    g_window->RegistTouchFrameCb(nullptr);
}

void TouchCancelCB(){}
HWTEST_F(WindowTest, RegistTouchCancelCb, testing::ext::TestSize.Level0)
{
    g_window->RegistTouchCancelCb(TouchCancelCB);
}
HWTEST_F(WindowTest, RegistTouchCancelCbNULL, testing::ext::TestSize.Level0)
{
    g_window->RegistTouchCancelCb(nullptr);
}

void TouchShapeCB(int32_t major, int32_t minor){}
HWTEST_F(WindowTest, RegistTouchShapeCb, testing::ext::TestSize.Level0)
{
    g_window->RegistTouchShapeCb(TouchShapeCB);
}
HWTEST_F(WindowTest, RegistTouchShapeCbNULL, testing::ext::TestSize.Level0)
{
    g_window->RegistTouchShapeCb(nullptr);
}

void TouchOrientationCallback(int32_t id, int32_t Orientation){}
HWTEST_F(WindowTest, RegistTouchOrientationCb, testing::ext::TestSize.Level0)
{
    g_window->RegistTouchOrientationCb(TouchOrientationCallback);
}
HWTEST_F(WindowTest, RegistTouchOrientationCbNULL, testing::ext::TestSize.Level0)
{
    g_window->RegistTouchOrientationCb(nullptr);
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
