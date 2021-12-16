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

#include "wmclient_native_test_20.h"

#include <chrono>
#include <cstdio>
#include <sstream>
#include <thread>
#include <unistd.h>

#include <cpudraw.h>
#include <graphic_bytrace.h>
#include <gslogger.h>
#include <ipc_object_stub.h>
#include <option_parser.h>
#include <window_manager.h>
#include <window_manager_service_client.h>

#include "inative_test.h"
#include "native_test_class.h"
#include "util.h"

using namespace OHOS;
using namespace std::chrono_literals;

namespace {
class WMClientNativeTest20 : public INativeTest {
public:
    virtual std::string GetDescription() const override
    {
        constexpr const char *desc = "split mode";
        return desc;
    }

    virtual std::string GetDomain() const override
    {
        constexpr const char *domain = "wmclient";
        return domain;
    }

    virtual int32_t GetID() const override
    {
        constexpr int32_t id = 20;
        return id;
    }

    virtual enum AutoLoadService GetAutoLoadService() const override
    {
        return AutoLoadService::WindowManager;
    }

    virtual int32_t GetProcessNumber() const override
    {
        return 3;
    }

    virtual void Run(int32_t argc, const char **argv) override
    {
        GSLOG7SO(INFO) << "fork return: " << StartSubprocess(0);
        GSLOG7SO(INFO) << "fork return: " << StartSubprocess(1);
        ExitTest();
    }
} g_autoload;

class WMClientNativeTest20Sub0 : public WMClientNativeTest20 {
public:
    virtual std::string GetDescription() const override
    {
        constexpr const char *desc = "splited application mocker";
        return desc;
    }

    virtual int32_t GetProcessSequence() const override
    {
        return 0;
    }

    virtual void Run(int32_t argc, const char **argv) override
    {
        ScopedBytrace trace(__func__);
        GSLOG7SO(INFO) << getpid() << " run0";
        window = NativeTestFactory::CreateWindow(WINDOW_TYPE_NORMAL);
        if (window == nullptr) {
            GSLOG7SO(ERROR) << "NativeTestFactory::CreateWindow return nullptr";
            ExitTest();
            return;
        }

        auto onSizeChange = [this](uint32_t w, uint32_t h) {
            GSLOG7SO(INFO) << "onSizeChange " << w << "x" << h;
            config.width = w;
            config.height = h;
            window->Resize(w, h);
            PostTask(std::bind(&NativeTestDrawer::DrawOnce, windowDrawer));
        };
        window->OnSizeChange(onSizeChange);
        auto func = std::bind(&WMClientNativeTest20Sub0::OnSplitStatusChange, this, std::placeholders::_1);
        window->OnSplitStatusChange(func);
        ListenWindowInputEvent(window->GetID());

        auto surface = window->GetSurface();
        config.width = surface->GetDefaultWidth();
        config.height = surface->GetDefaultHeight();
        config.strideAlignment = 0x8;
        config.format = PIXEL_FMT_RGBA_8888;
        config.usage = surface->GetDefaultUsage();

        window->SwitchTop();
        auto draw = std::bind(&WMClientNativeTest20Sub0::Draw, this,
            std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4);
        windowDrawer = NativeTestDrawer::CreateDrawer(draw, surface, &config);
        windowDrawer->DrawOnce();
    }

    void Draw(void *vaddr, uint32_t width, uint32_t height, uint32_t count)
    {
        ScopedBytrace trace(__func__);
        count = NativeTestDraw::RainbowDrawFramerate / 0x2;
        if (adjStatus == SPLIT_STATUS_CLEAR) {
            count = 0;
        }

        NativeTestDraw::RainbowDraw(vaddr, width, height, count);
    }

    virtual bool OnKey(const KeyEvent &event) override
    {
        GSLOG7SO(DEBUG) << "[" << event.GetKeyCode() << "]";
        if (event.IsKeyDown() ==  true && event.GetKeyCode() == KeyEventEnum::KEY_BACK) {
            ExitTest();
            return false;
        }
        return true;
    }

    virtual bool OnTouch(const TouchEvent &event) override
    {
        if (event.GetAction() == TouchEnum::PRIMARY_POINT_DOWN) {
            window->SwitchTop();
        }

        GSLOG7SO(DEBUG) << "[" << event.GetAction() << "]"
            << "(" << event.GetPointerPosition(event.GetIndex()).GetX()
            << ", " << event.GetPointerPosition(event.GetIndex()).GetY()
            << ")";
        return false;
    }

    void OnSplitStatusChange(SplitStatus status)
    {
        ScopedBytrace trace(__func__);
        if (status == SPLIT_STATUS_DESTROY) {
            GSLOG7SO(INFO) << "SPLIT_STATUS_DESTROY";
            ExitTest();
        } else if (status == SPLIT_STATUS_RETAIN) {
            GSLOG7SO(INFO) << "SPLIT_STATUS_RETAIN";
            adjStatus = SPLIT_STATUS_CLEAR;
            PostTask(std::bind(&NativeTestDrawer::DrawOnce, windowDrawer));
        } else {
            if (adjStatus != status) {
                GSLOG7SO(INFO) << "SPLIT_STATUS_" << status;
            }
            adjStatus = status;
            PostTask(std::bind(&NativeTestDrawer::DrawOnce, windowDrawer));
        }
    }

protected:
    sptr<NativeTestDrawer> windowDrawer = nullptr;
    SplitStatus adjStatus = SPLIT_STATUS_CLEAR;

private:
    sptr<Window> window = nullptr;
    BufferRequestConfig config = {};
} g_autoload0;

class WMClientNativeTest20Sub1 : public WMClientNativeTest20 {
public:
    virtual std::string GetDescription() const override
    {
        constexpr const char *desc = "systemui mocker";
        return desc;
    }

    virtual int32_t GetProcessSequence() const override
    {
        return 1;
    }

    virtual enum AutoLoadService GetAutoLoadService() const override
    {
        return AutoLoadService::WindowManager | AutoLoadService::WindowManagerService;
    }

    virtual void Run(int32_t argc, const char **argv) override
    {
        ScopedBytrace trace(__func__);
        window = NativeTestFactory::CreateWindow(WINDOW_TYPE_SYSTEM_UI);
        if (window == nullptr) {
            GSLOG7SO(ERROR) << "NativeTestFactory::CreateWindow return nullptr";
            ExitTest();
            return;
        }

        auto surface = window->GetSurface();
        window->SwitchTop();
        auto draw = std::bind(&WMClientNativeTest20Sub1::Draw, this,
            std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4);
        windowDrawer = NativeTestDrawer::CreateDrawer(draw, surface);
        windowDrawer->DrawOnce();
        for (auto &icon : icons) {
            icon.rect.x *= window->GetWidth();
            icon.rect.y *= window->GetHeight();
            icon.rect.w *= window->GetWidth();
            icon.rect.h *= window->GetHeight();
        }
        total.x *= window->GetWidth();
        total.y *= window->GetHeight();
        total.w *= window->GetWidth();
        total.h *= window->GetHeight();

        ListenWindowInputEvent(window->GetID());
    }

    void Draw(void *vaddr, uint32_t width, uint32_t height, uint32_t count)
    {
        ScopedBytrace trace(__func__);
        GSLOG7SO(INFO) << "currentIcon: " << currentIcon;
        CPUDraw draw(vaddr, width, height);

        draw.SetColor(0x00000000);
        draw.DrawRect(0, 0, width, height);

        draw.SetColor(0xffffffff);
        draw.DrawRect(total);

        draw.SetColor(0xffaaaaaa);
        draw.SetBorder(0x2);
        draw.DrawBorder(total);

        for (auto &icon : icons) {
            draw.SetColor(icon.c);
            draw.DrawRect(icon.rect);
        }
    }

    virtual bool OnKey(const KeyEvent &event) override
    {
        GSLOG7SO(DEBUG) << "[" << event.GetKeyCode() << "]";
        if (event.IsKeyDown() ==  true && event.GetKeyCode() == KeyEventEnum::KEY_BACK) {
            ExitTest();
            return false;
        }
        return true;
    }

    virtual bool OnTouch(const TouchEvent &event) override
    {
        ScopedBytrace trace(__func__);
        if (event.GetAction() == TouchEnum::PRIMARY_POINT_DOWN) {
            window->SwitchTop();
        }

        int32_t index = event.GetIndex();
        int32_t x = event.GetPointerPosition(index).GetX();
        int32_t y = event.GetPointerPosition(index).GetY();
        if (event.GetAction() == TouchEnum::PRIMARY_POINT_DOWN) {
            downX = x;
            downY = y;
            currentIcon = nullptr;
            for (auto &icon : icons) {
                if (icon.rect.Contain(x, y)) {
                    currentIcon = &icon;
                    backupIcon = icon;
                    GSLOG7SO(INFO) << "selected: " << currentIcon
                        << " " << currentIcon->rect.x << ", " << currentIcon->rect.y;
                    if (currentIcon != &icons[0]) {
                        SetSplitMode(SPLIT_MODE_SINGLE);
                    } else {
                        SetSplitMode(SPLIT_MODE_UNENABLE);
                    }
                    break;
                }
            }
            return false;
        }

        if (event.GetAction() == TouchEnum::POINT_MOVE && currentIcon) {
            currentIcon->rect.x = backupIcon.rect.x + x - downX;
            currentIcon->rect.y = backupIcon.rect.y + y - downY;
            PostTask(std::bind(&NativeTestDrawer::DrawOnce, windowDrawer));

            if (currentIcon != &icons[0]) {
                if (total.Contain(x, y)) {
                    SetSplitMode(SPLIT_MODE_SINGLE);
                } else {
                    static int32_t lastX = -1;
                    static int32_t lastY = -1;
                    if (lastX != x || lastY != y) {
                        SetSplitMode(SPLIT_MODE_SELECT, x, y);
                    }
                    lastX = x;
                    lastY = y;
                }
            }
            return false;
        }

        if (event.GetAction() == TouchEnum::PRIMARY_POINT_UP && currentIcon) {
            if (currentIcon != &icons[0]) {
                // center point (x, y)
                auto x = currentIcon->rect.x + currentIcon->rect.w / 0x2;
                auto y = currentIcon->rect.y + currentIcon->rect.y / 0x2;
                if (total.Contain(x, y)) {
                    SetSplitMode(SPLIT_MODE_NULL);
                } else {
                    StartProcess2();
                    return false;
                }
            } else {
                SetSplitMode(SPLIT_MODE_NULL);
            }
            *currentIcon = backupIcon;
            PostTask(std::bind(&NativeTestDrawer::DrawOnce, windowDrawer));
            return false;
        }
        return true;
    }

    SplitMode lastMode = SPLIT_MODE_NULL;
    void SetSplitMode(SplitMode mode, int32_t x = 0, int32_t y = 0)
    {
        ScopedBytrace trace(__func__);
        if (lastMode != mode) {
            GSLOG7SO(INFO) << static_cast<int32_t>(mode);
            lastMode = mode;
        }
        windowManagerService->SetSplitMode(mode, x, y);
    }

    void StartProcess2()
    {
        extraArgs.clear();
        std::stringstream ss;
        ss << "--color=" << std::hex << std::showbase << currentIcon->c;
        auto sss = ss.str();
        extraArgs.push_back(sss.c_str());
        GSLOG7SO(INFO) << "fork return: " << StartSubprocess(2);

        std::this_thread::sleep_for(1s);
        SetSplitMode(SPLIT_MODE_CONFIRM);
        ExitTest();
        return;
    }

private:
    sptr<Window> window = nullptr;
    sptr<NativeTestDrawer> windowDrawer = nullptr;

    static constexpr double xx = 0.7;
    static constexpr double ww = 0.3;
    static constexpr double yy = 0.1;
    static constexpr double hh = 0.8;

    struct Position {
        struct CPUDrawRect rect;
        uint32_t c;
    };
    struct Position icons[0x4] = {
        { { xx + 0.15 * ww, yy + 1 * hh / 9, 0.7 * ww, hh / 9 }, 0xffff0000 },
        { { xx + 0.15 * ww, yy + 3 * hh / 9, 0.7 * ww, hh / 9 }, 0xff00ff00 },
        { { xx + 0.15 * ww, yy + 5 * hh / 9, 0.7 * ww, hh / 9 }, 0xff0000ff },
        { { xx + 0.15 * ww, yy + 7 * hh / 9, 0.7 * ww, hh / 9 }, 0xffff00ff },
    };
    struct Position *currentIcon = nullptr;
    struct Position backupIcon;
    struct CPUDrawRect total = { xx, yy, ww, hh };
    int32_t downX = 0;
    int32_t downY = 0;
} g_autoload1;

class WMClientNativeTest20Sub2 : public WMClientNativeTest20Sub0 {
public:
    virtual std::string GetDescription() const override
    {
        constexpr const char *desc = "spliting application mocker";
        return desc;
    }

    virtual int32_t GetProcessSequence() const override
    {
        return 2;
    }

    virtual void Run(int32_t argc, const char **argv) override
    {
        OptionParser parser;
        parser.AddOption("c", "color", color);
        if (parser.Parse(argc, argv)) {
            GSLOG7SE(ERROR) << parser.GetErrorString();
            ExitTest();
            return;
        }

        GSLOG7SO(INFO) << "color: " << std::hex << std::showbase << color;
        WMClientNativeTest20Sub0::Run(argc, argv);
        auto draw = std::bind(&WMClientNativeTest20Sub2::Draw, this,
            std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4);
        windowDrawer->SetDrawFunc(draw);
        windowDrawer->DrawOnce();
    }

    void Draw(void *vaddr, uint32_t width, uint32_t height, uint32_t count)
    {
        ScopedBytrace trace(__func__);
        CPUDraw draw(vaddr, width, height);
        draw.SetColor(color);
        if (adjStatus == SPLIT_STATUS_VAGUE) {
            draw.SetColor((0xffffffff - color) | 0xff000000);
        }

        draw.DrawRect(0, 0, width, height);
    }

private:
    uint32_t color = 0xffff0000;
} g_autoload2;
} // namespace
