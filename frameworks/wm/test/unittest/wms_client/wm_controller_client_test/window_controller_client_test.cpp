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

#include "window_controller_client_test.h"
#include <iostream>
#include <memory.h>
#include <display_type.h>
#include <gtest/gtest.h>
#include <iservice_registry.h>
#include <system_ability_definition.h>
#include <surface.h>
#include <window_manager.h>
#include <window_manager_proxy.h>
#include <window_manager_controller_client.h>

namespace OHOS {
LayerControllerClient* g_playerControllerClient = nullptr;
static int32_t g_testWindowIdBaseId = 0;
static uint32_t g_testU32WindowIdBaseId = 0;
static uint32_t g_testSubWindowIdBaseId = 0;
static int g_width = 200;
static int g_height = 100;
WindowConfig g_config = {
    .width = g_width,
    .height = g_height,
    .format = PIXEL_FMT_RGBA_8888,
    .pos_x = 0,
    .pos_y = 0,
    .type = WINDOW_TYPE_NORMAL
};

class WindowControllerClientTest : public testing::Test {
public:
    static void SetUpTestCase(void)
    {
        auto sam = SystemAbilityManagerClient::GetInstance().GetSystemAbilityManager();
        if (sam == nullptr) {
            printf("GetSystemAbilityManager failed\n");
            return;
        }

        auto object = sam->GetSystemAbility(WINDOW_MANAGER_ID);
        if (object == nullptr) {
            printf("GetSystemAbilityAbility failed\n");
            return;
        }

        sptr<IWindowManagerService> windowManagerService = iface_cast<IWindowManagerService>(object);
        g_testWindowIdBaseId = windowManagerService->CreateWindow(g_config);
        g_testSubWindowIdBaseId = windowManagerService->CreateWindow(g_config);
        g_playerControllerClient = LayerControllerClient::GetInstance();
        if (g_playerControllerClient == nullptr) {
            printf("SetUpTestCase-----nullptr\n");
        }
    };
};

HWTEST_F(WindowControllerClientTest, GetInstance, testing::ext::TestSize.Level0)
{
    LayerControllerClient* ptest = LayerControllerClient::GetInstance();
    ASSERT_NE(ptest, nullptr);
}

HWTEST_F(WindowControllerClientTest, CreateWindow, testing::ext::TestSize.Level0)
{
    InnerWindowInfo* pWinInfo = g_playerControllerClient->CreateWindow(g_testWindowIdBaseId, g_config);
    ASSERT_NE(pWinInfo, nullptr);
}

HWTEST_F(WindowControllerClientTest, CreateSubWindow, testing::ext::TestSize.Level0)
{
    InnerWindowInfo* pSubWinInfo = g_playerControllerClient->CreateSubWindow(g_testSubWindowIdBaseId,
                                                         g_testWindowIdBaseId, g_config);
    ASSERT_NE(pSubWinInfo, nullptr);
}

HWTEST_F(WindowControllerClientTest, CreateWlBuffer, testing::ext::TestSize.Level0)
{
    sptr<Surface> surface = Surface::CreateSurfaceAsConsumer();
    g_playerControllerClient->CreateWlBuffer(surface, g_testU32WindowIdBaseId);
    ASSERT_NE(surface, nullptr);
}

HWTEST_F(WindowControllerClientTest, Move, testing::ext::TestSize.Level0)
{
    g_playerControllerClient->Move(g_testWindowIdBaseId, 50, 50);
}

HWTEST_F(WindowControllerClientTest, Show, testing::ext::TestSize.Level0)
{
    g_playerControllerClient->Show(g_testWindowIdBaseId);
}

HWTEST_F(WindowControllerClientTest, Hide, testing::ext::TestSize.Level0)
{
    g_playerControllerClient->Hide(g_testWindowIdBaseId);
}

HWTEST_F(WindowControllerClientTest, ReSize, testing::ext::TestSize.Level0)
{
    g_playerControllerClient->ReSize(g_testWindowIdBaseId, 500, 500);
}

HWTEST_F(WindowControllerClientTest, Rotate, testing::ext::TestSize.Level0)
{
    g_playerControllerClient->Rotate(g_testWindowIdBaseId, WM_ROTATE_TYPE_NORMAL);
    g_playerControllerClient->Rotate(g_testWindowIdBaseId, WM_ROTATE_TYPE_90);
    g_playerControllerClient->Rotate(g_testWindowIdBaseId, WM_ROTATE_TYPE_180);
    g_playerControllerClient->Rotate(g_testWindowIdBaseId, WM_ROTATE_TYPE_270);
    g_playerControllerClient->Rotate(g_testWindowIdBaseId, WM_ROTATE_TYPE_FLIPPED);
    g_playerControllerClient->Rotate(g_testWindowIdBaseId, WM_ROTATE_TYPE_FLIPPED_90);
    g_playerControllerClient->Rotate(g_testWindowIdBaseId, WM_ROTATE_TYPE_FLIPPED_180);
    g_playerControllerClient->Rotate(g_testWindowIdBaseId, WM_ROTATE_TYPE_FLIPPED_270);
}

HWTEST_F(WindowControllerClientTest, GetMaxWidth, testing::ext::TestSize.Level0)
{
    int iWidth = g_playerControllerClient->GetMaxWidth();
    ASSERT_GE(iWidth, 0);
}

HWTEST_F(WindowControllerClientTest, GetMaxHeight, testing::ext::TestSize.Level0)
{
    int iheight = g_playerControllerClient->GetMaxHeight();
    ASSERT_GE(iheight, 0);
}

HWTEST_F(WindowControllerClientTest, ChangeWindowType, testing::ext::TestSize.Level0)
{
    g_playerControllerClient->ChangeWindowType(g_testWindowIdBaseId, WINDOW_TYPE_NORMAL);
    g_playerControllerClient->ChangeWindowType(g_testWindowIdBaseId, WINDOW_TYPE_STATUS_BAR);
    g_playerControllerClient->ChangeWindowType(g_testWindowIdBaseId, WINDOW_TYPE_NAVI_BAR);
    g_playerControllerClient->ChangeWindowType(g_testWindowIdBaseId, WINDOW_TYPE_ALARM_SCREEN);
    g_playerControllerClient->ChangeWindowType(g_testWindowIdBaseId, WINDOW_TYPE_SYSTEM_UI);
    g_playerControllerClient->ChangeWindowType(g_testWindowIdBaseId, WINDOW_TYPE_LAUNCHER);
    g_playerControllerClient->ChangeWindowType(g_testWindowIdBaseId, WINDOW_TYPE_VIDEO);
}

void ShotDone(ImageInfo& imageInfo){};
HWTEST_F(WindowControllerClientTest, StartShotScreen, testing::ext::TestSize.Level0)
{
    g_playerControllerClient->StartShotScreen(ShotDone);
}
HWTEST_F(WindowControllerClientTest, StartShotScreenNULL, testing::ext::TestSize.Level0)
{
    g_playerControllerClient->StartShotScreen(nullptr);
}

HWTEST_F(WindowControllerClientTest, StartShotWindow, testing::ext::TestSize.Level0)
{
    g_playerControllerClient->StartShotWindow(g_testWindowIdBaseId, ShotDone);
}
HWTEST_F(WindowControllerClientTest, StartShotWindowNULL, testing::ext::TestSize.Level0)
{
    g_playerControllerClient->StartShotWindow(g_testWindowIdBaseId, nullptr);
}

void PointerButton(int serial, int button, int state, int time){};
HWTEST_F(WindowControllerClientTest, RegistPointerButtonCb, testing::ext::TestSize.Level0)
{
    g_playerControllerClient->RegistPointerButtonCb(g_testWindowIdBaseId, PointerButton);
}
HWTEST_F(WindowControllerClientTest, RegistPointerButtonCbNULL, testing::ext::TestSize.Level0)
{
    g_playerControllerClient->RegistPointerButtonCb(g_testWindowIdBaseId, nullptr);
}

void PointerEnter(int x, int y, int serila){};
HWTEST_F(WindowControllerClientTest, RegistPointerEnterCb, testing::ext::TestSize.Level0)
{
    g_playerControllerClient->RegistPointerEnterCb(g_testWindowIdBaseId, PointerEnter);
}
HWTEST_F(WindowControllerClientTest, RegistPointerEnterCbNULL, testing::ext::TestSize.Level0)
{
    g_playerControllerClient->RegistPointerEnterCb(g_testWindowIdBaseId, nullptr);
}

void PointerLeave(int serial){};
HWTEST_F(WindowControllerClientTest, RegistPointerLeaveCb, testing::ext::TestSize.Level0)
{
    g_playerControllerClient->RegistPointerLeaveCb(g_testWindowIdBaseId, PointerLeave);
}
HWTEST_F(WindowControllerClientTest, RegistPointerLeaveCbNULL, testing::ext::TestSize.Level0)
{
    g_playerControllerClient->RegistPointerLeaveCb(g_testWindowIdBaseId, nullptr);
}

void PointerMotion(int x, int y, int time){};
HWTEST_F(WindowControllerClientTest, RegistPointerMotionCb, testing::ext::TestSize.Level0)
{
    g_playerControllerClient->RegistPointerMotionCb(g_testWindowIdBaseId, PointerMotion);
}
HWTEST_F(WindowControllerClientTest, RegistPointerMotionCbNULL,
         testing::ext::TestSize.Level0)
{
    g_playerControllerClient->RegistPointerMotionCb(g_testWindowIdBaseId, nullptr);
}

void PointerAxisDiscrete(int axis, int discrete){};
HWTEST_F(WindowControllerClientTest, RegistPointerAxisDiscreteCb, testing::ext::TestSize.Level0)
{
    g_playerControllerClient->RegistPointerAxisDiscreteCb(g_testWindowIdBaseId, PointerAxisDiscrete);
}
HWTEST_F(WindowControllerClientTest, RegistPointerAxisDiscreteCbNULL,
                                         testing::ext::TestSize.Level0)
{
    g_playerControllerClient->RegistPointerAxisDiscreteCb(g_testWindowIdBaseId, nullptr);
}

void PointerAxisSource(int axisSource){};
HWTEST_F(WindowControllerClientTest, RegistPointerAxisSourceCb, testing::ext::TestSize.Level0)
{
    g_playerControllerClient->RegistPointerAxisSourceCb(g_testWindowIdBaseId, PointerAxisSource);
}
HWTEST_F(WindowControllerClientTest, RegistPointerAxisSourceCbNULL,
         testing::ext::TestSize.Level0)
{
    g_playerControllerClient->RegistPointerAxisSourceCb(g_testWindowIdBaseId, nullptr);
}

void PointerAxisStop(int time, int axis){};
HWTEST_F(WindowControllerClientTest, RegistPointerAxisStopCb, testing::ext::TestSize.Level0)
{
    g_playerControllerClient->RegistPointerAxisStopCb(g_testWindowIdBaseId, PointerAxisStop);
}
HWTEST_F(WindowControllerClientTest, RegistPointerAxisStopCbNULL, testing::ext::TestSize.Level0)
{
    g_playerControllerClient->RegistPointerAxisStopCb(g_testWindowIdBaseId, nullptr);
}

void PointerAxis(int time, int axis, int value){};
HWTEST_F(WindowControllerClientTest, RegistPointerAxisCb, testing::ext::TestSize.Level0)
{
    g_playerControllerClient->RegistPointerAxisCb(g_testWindowIdBaseId, PointerAxis);
}
HWTEST_F(WindowControllerClientTest, RegistPointerAxisCbNULL, testing::ext::TestSize.Level0)
{
    g_playerControllerClient->RegistPointerAxisCb(g_testWindowIdBaseId, nullptr);
}

void TouchUp(int serial, int time, int id){};
HWTEST_F(WindowControllerClientTest, RegistTouchUpCb, testing::ext::TestSize.Level0)
{
    g_playerControllerClient->RegistTouchUpCb(g_testWindowIdBaseId, TouchUp);
}
HWTEST_F(WindowControllerClientTest, RegistTouchUpCbNULL, testing::ext::TestSize.Level0)
{
    g_playerControllerClient->RegistTouchUpCb(g_testWindowIdBaseId, nullptr);
}

void TouchDown(int x, int y, int serial, int time, int id){}
HWTEST_F(WindowControllerClientTest, RegistTouchDownCb, testing::ext::TestSize.Level0)
{
    g_playerControllerClient->RegistTouchDownCb(g_testWindowIdBaseId, TouchDown);
}
HWTEST_F(WindowControllerClientTest, RegistTouchDownCbNULL, testing::ext::TestSize.Level0)
{
    g_playerControllerClient->RegistTouchDownCb(g_testWindowIdBaseId, nullptr);
}

void TouchEmotion(int x, int y, int time, int id){}
HWTEST_F(WindowControllerClientTest, RegistTouchEmotionCb, testing::ext::TestSize.Level0)
{
    g_playerControllerClient->RegistTouchEmotionCb(g_testWindowIdBaseId, TouchEmotion);
}
HWTEST_F(WindowControllerClientTest, RegistTouchEmotionCbNULL, testing::ext::TestSize.Level0)
{
    g_playerControllerClient->RegistTouchEmotionCb(g_testWindowIdBaseId, nullptr);
}

void TouchFrame(){}
HWTEST_F(WindowControllerClientTest, RegistTouchFrameCb, testing::ext::TestSize.Level0)
{
    g_playerControllerClient->RegistTouchFrameCb(g_testWindowIdBaseId, TouchFrame);
}
HWTEST_F(WindowControllerClientTest, RegistTouchFrameCbNULL, testing::ext::TestSize.Level0)
{
    g_playerControllerClient->RegistTouchFrameCb(g_testWindowIdBaseId, nullptr);
}

void TouchCancel(){}
HWTEST_F(WindowControllerClientTest, RegistTouchCancelCb, testing::ext::TestSize.Level0)
{
    g_playerControllerClient->RegistTouchCancelCb(g_testWindowIdBaseId, TouchCancel);
}
HWTEST_F(WindowControllerClientTest, RegistTouchCancelCbNULL, testing::ext::TestSize.Level0)
{
    g_playerControllerClient->RegistTouchCancelCb(g_testWindowIdBaseId, nullptr);
}

void TouchShape(int major, int minor){}
HWTEST_F(WindowControllerClientTest, RegistTouchShapeCb, testing::ext::TestSize.Level0)
{
    g_playerControllerClient->RegistTouchShapeCb(g_testWindowIdBaseId, TouchShape);
}
HWTEST_F(WindowControllerClientTest, RegistTouchShapeCbNULL, testing::ext::TestSize.Level0)
{
    g_playerControllerClient->RegistTouchShapeCb(g_testWindowIdBaseId, nullptr);
}

void TouchOrientation(int id, int Orientation){}
HWTEST_F(WindowControllerClientTest, RegistTouchOrientationCb, testing::ext::TestSize.Level0)
{
    g_playerControllerClient->RegistTouchOrientationCb(g_testWindowIdBaseId, TouchOrientation);
}
HWTEST_F(WindowControllerClientTest, RegistTouchOrientationCbNULL, testing::ext::TestSize.Level0)
{
    g_playerControllerClient->RegistTouchOrientationCb(g_testWindowIdBaseId, nullptr);
}

void WindowInfoChangeCb(WindowInfo &info){}
HWTEST_F(WindowControllerClientTest, RegistWindowInfoChangeCb, testing::ext::TestSize.Level0)
{
    g_playerControllerClient->RegistWindowInfoChangeCb(g_testWindowIdBaseId, WindowInfoChangeCb);
}
HWTEST_F(WindowControllerClientTest, RegistWindowInfoChangeCbNULL, testing::ext::TestSize.Level0)
{
    g_playerControllerClient->RegistWindowInfoChangeCb(g_testWindowIdBaseId, nullptr);
}

HWTEST_F(WindowControllerClientTest, InnerWindowInfoFromId, testing::ext::TestSize.Level0)
{
    InnerWindowInfo* pWinInfo = g_playerControllerClient->GetInnerWindowInfoFromId((uint32_t)g_testWindowIdBaseId);
    ASSERT_NE(pWinInfo, nullptr);
}

HWTEST_F(WindowControllerClientTest, RemoveInnerWindowInfo, testing::ext::TestSize.Level0)
{
    g_playerControllerClient->RemoveInnerWindowInfo((uint32_t)g_testWindowIdBaseId);
}

HWTEST_F(WindowControllerClientTest, thread_display_dispatch, testing::ext::TestSize.Level0)
{
    void* retptr = LayerControllerClient::thread_display_dispatch(g_playerControllerClient);
    ASSERT_EQ(retptr, nullptr);
}
}
