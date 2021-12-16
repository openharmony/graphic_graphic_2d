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

#include "wmclient_native_test_32.h"

#include <cstdio>
#include <functional>

#include <display_type.h>
#include <ipc_object_stub.h>
#include <multimodal_event_handler.h>
#include <window_manager.h>
#include "key_event_handler.h"
#include "touch_event_handler.h"

#include "inative_test.h"
#include "native_test_class.h"
#include "util.h"

using namespace OHOS;

namespace {
class WMClientNativeTest32Ability : public INativeTest {
public:
    virtual void Draw(void *vaddr, uint32_t width, uint32_t height, uint32_t count) = 0;

    void Run(int32_t argc, const char **argv) override
    {
        handler = AppExecFwk::EventHandler::Current();
        auto initRet = WindowManager::GetInstance()->Init();
        if (initRet) {
            printf("init failed with %s\n", WMErrorStr(initRet).c_str());
            ExitTest();
            return;
        }

        mainWindow = NativeTestFactory::CreateWindow(WINDOW_TYPE_NORMAL);
        if (mainWindow == nullptr) {
            ExitTest();
            return;
        }

        mainWindow->SwitchTop();
        auto surface = mainWindow->GetSurface();
        auto draw = std::bind(&WMClientNativeTest32Ability::Draw, this,
            ::std::placeholders::_1, ::std::placeholders::_2,
            ::std::placeholders::_3, ::std::placeholders::_4);
        mainSync = NativeTestSync::CreateSync(draw, surface);

        ListenWindowKeyEvent(mainWindow->GetID());
    }

    void EnterPIPMode()
    {
        handler->PostTask(std::bind(&WMClientNativeTest32Ability::EnterPictureInPictureMode, this));
    }

    void EnterPictureInPictureMode()
    {
        if (pipMode) {
            return;
        }

        auto wm = WindowManager::GetInstance();
        if (wm == nullptr) {
            return;
        }

        auto option = WindowOption::Get();
        if (option == nullptr) {
            return;
        }

        option->SetWindowType(WINDOW_TYPE_NORMAL);
        option->SetWindowMode(WINDOW_MODE_FREE);
        if (pipWindow == nullptr) {
            wm->CreateWindow(pipWindow, option);
            if (pipWindow == nullptr) {
                printf("wm->CreateWindow return nullptr\n");
                return;
            }

            auto pipSurface = pipWindow->GetSurface();
            auto draw = std::bind(&WMClientNativeTest32Ability::Draw, this,
                ::std::placeholders::_1, ::std::placeholders::_2,
                ::std::placeholders::_3, ::std::placeholders::_4);

            pipSync = NativeTestSync::CreateSync(draw, pipSurface);
            ListenWindowTouchEvent(pipWindow->GetID());

            int x = 100, y = 100, w = 300, h = 200;
            pipWindow->Move(x, y);
            pipWindow->Resize(w, h);
        }

        pipWindow->Show();
        pipWindow->SwitchTop();
        mainWindow->Hide();

        pipMode = true;
        onPIPModeChange(pipMode);
    }

    void ExitPIPMode()
    {
        if (!pipMode) {
            return;
        }

        pipMode = false;
        onPIPModeChange(pipMode);

        pipWindow->Hide();
        mainWindow->Show();
        mainWindow->SwitchTop();
    }

    void OnPIPModeChange(std::function<void(bool)> func)
    {
        onPIPModeChange = func;
    }

    virtual bool OnKey(const KeyEvent &event) override
    {
        if (event.IsKeyDown() ==  true && event.GetKeyCode() == OHOS::KeyEventEnum::KEY_BACK) {
            return OnKeyPublic(event);
        }
        return false;
    }

    virtual bool OnTouch(const TouchEvent &event) override
    {
        int index = event.GetIndex();
        if (event.GetAction() == OHOS::TouchEnum::POINT_MOVE) {
            pipWindow->Move(pipWindow->GetX() + event.GetPointerPosition(index).GetX() - DownX,
                            pipWindow->GetY() + event.GetPointerPosition(index).GetY() - DownY)
                ->Then(std::bind(&WMClientNativeTest32Ability::OnMoveReturn, this, std::placeholders::_1));
            DownX = event.GetPointerPosition(index).GetX();
            DownY = event.GetPointerPosition(index).GetY();
            touchUpFlag = false;
            return true;
        } else if (event.GetAction() == OHOS::TouchEnum::PRIMARY_POINT_UP && touchUpFlag == true) {
            return OnTouchPublic(event);
        } else if (event.GetAction() == OHOS::TouchEnum::PRIMARY_POINT_UP && touchUpFlag == false) {
            DownX = 0;
            DownY = 0;
        } else if (event.GetAction() == OHOS::TouchEnum::PRIMARY_POINT_DOWN) {
            DownX = event.GetPointerPosition(index).GetX();
            DownY = event.GetPointerPosition(index).GetY();
        }
        touchUpFlag = true;
        return false;
    }

    void OnMoveReturn(const WMError &err)
    {
        if (err != WM_OK) {
            printf("Move failed %d, means %s\n", err, WMErrorStr(err).c_str());
        }
    }

    virtual bool OnKeyPublic(const KeyEvent &event) = 0;
    virtual bool OnTouchPublic(const TouchEvent &event) = 0;

private:
    sptr<Window> mainWindow = nullptr;
    sptr<Window> pipWindow = nullptr;
    sptr<NativeTestSync> mainSync = nullptr;
    sptr<NativeTestSync> pipSync = nullptr;
 
    bool pipMode = false;
    bool touchUpFlag = true;
    std::function<void(bool)> onPIPModeChange = nullptr;
    std::shared_ptr<AppExecFwk::EventHandler> handler = nullptr;

    double DownX = 0;
    double DownY = 0;
};

class WMClientNativeTest32 : public WMClientNativeTest32Ability {
public:
    std::string GetDescription() const override
    {
        constexpr const char *desc = "PIP mode";
        return desc;
    }

    std::string GetDomain() const override
    {
        constexpr const char *domain = "wmclient";
        return domain;
    }

    int32_t GetID() const override
    {
        constexpr int32_t id = 32;
        return id;
    }

    uint32_t GetLastTime() const override
    {
        constexpr uint32_t lastTime = LAST_TIME_FOREVER;
        return lastTime;
    }

    void Run(int32_t argc, const char **argv) override
    {
        WMClientNativeTest32Ability::Run(argc, argv);
        OnPIPModeChange(std::bind(&WMClientNativeTest32::OnPipModeChange, this, std::placeholders::_1));
    }

    void Draw(void *vaddr, uint32_t width, uint32_t height, uint32_t count) override
    {
        drawptr(vaddr, width, height, count);
    }

    void OnPipModeChange(bool isOnPIPMode)
    {
        if (isOnPIPMode) {
            drawptr = NativeTestDraw::ColorDraw;
        } else {
            drawptr = NativeTestDraw::FlushDraw;
        }
    }

    virtual bool OnTouchPublic(const TouchEvent &event) override
    {
        ExitPIPMode();
        return true;
    }

    virtual bool OnKeyPublic(const KeyEvent &event) override
    {
        drawptr = NativeTestDraw::ColorDraw;
        EnterPIPMode();

        return true;
    }

private:
    DrawFunc drawptr = NativeTestDraw::FlushDraw;
} g_autoload;
} // namespace
