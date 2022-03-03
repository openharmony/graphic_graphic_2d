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

#include "wmclient_native_test_21.h"

#include <GLES2/gl2.h>
#include <gslogger.h>
#include <iservice_registry.h>
#include <shader.h>
#include <texture.h>

#include "inative_test.h"
#include "native_test_class.h"
#include "util.h"

using namespace OHOS;

namespace {
class WMClientNativeTest21 : public INativeTest {
public:
    std::string GetDescription() const override
    {
        constexpr const char *desc = "EglImageSurface + Effect + Remote Producer";
        return desc;
    }

    std::string GetDomain() const override
    {
        constexpr const char *domain = "wmclient";
        return domain;
    }

    int32_t GetID() const override
    {
        constexpr int32_t id = 21;
        return id;
    }

    AutoLoadService GetAutoLoadService() const override
    {
        return AutoLoadService::WindowManager;
    }

    int32_t GetProcessNumber() const override
    {
        return 2;
    }

    void Run(int32_t argc, const char **argv) override
    {
        auto ret = IPCServerStart();
        if (ret) {
            GSLOG7SE(ERROR) << ret;
            ExitTest();
        }

        GSLOG7SO(INFO) << "fork return: " << StartSubprocess(0);
        sleep(1);
        GSLOG7SO(INFO) << "fork return: " << StartSubprocess(1);
        WaitSubprocessAllQuit();
    }
} g_autoload;

class WMClientNativeTest21Sub0 : public WMClientNativeTest21 {
public:
    std::string GetDescription() const override
    {
        constexpr const char *desc = "Remote Producer";
        return desc;
    }

    int32_t GetProcessSequence() const override
    {
        return 0;
    }

    void Run(int32_t argc, const char **argv) override
    {
    }

    void Run2()
    {
        sptr<IBufferProducer> producer = iface_cast<IBufferProducer>(remoteObject);
        psurf = Surface::CreateSurfaceAsProducer(producer);
        SetVsyncRate(1);
        subwindowSync = NativeTestSync::CreateSync(NativeTestDraw::RainbowDraw, psurf);
    }

    void IPCClientOnMessage(int32_t sequence, const std::string &message, const sptr<IRemoteObject> &robj) override
    {
        if (message == "producer") {
            remoteObject = robj;
            if (remoteObject == nullptr) {
                PostTask(std::bind(&INativeTest::IPCClientSendMessage, this, 1, "quit", nullptr));
                ExitTest();
            } else {
                PostTask(std::bind(&WMClientNativeTest21Sub0::Run2, this));
            }
            return;
        }

        if (message == "quit") {
            ExitTest();
            return;
        }
    }

private:
    sptr<IRemoteObject> remoteObject = nullptr;
    sptr<Surface> psurf = nullptr;
    sptr<NativeTestSync> subwindowSync = nullptr;
} g_autoload0;

class WMClientNativeTest21Sub1 : public WMClientNativeTest21 {
public:
    std::string GetDescription() const override
    {
        constexpr const char *desc = "EglImageSurface + Effect";
        return desc;
    }

    int32_t GetProcessSequence() const override
    {
        return 1;
    }

    void Run(int32_t argc, const char **argv) override
    {
        window = NativeTestFactory::CreateWindow(WINDOW_TYPE_NORMAL);
        if (window == nullptr) {
            ExitTest();
            return;
        }

        auto surf = window->GetSurface();
        window->SwitchTop();
        windowSync = NativeTestSync::CreateSync(NativeTestDraw::FlushDraw, surf);
        SetVsyncRate(1);

        auto so = SubwindowOption::Get();
        so->SetWidth(window->GetWidth() / 0x2);
        so->SetHeight(window->GetHeight() / 0x2);
        so->SetX(window->GetWidth() / 0x4);
        so->SetY(window->GetHeight() / 0x4);
        so->SetWindowType(SUBWINDOW_TYPE_OFFSCREEN);
        windowManager->CreateSubwindow(subwindow, window, so);
        auto onFrameAvailable = std::bind(&WMClientNativeTest21Sub1::OnFrameAvailable,
            this, std::placeholders::_1);
        subwindow->OnFrameAvailable(onFrameAvailable);

        ListenWindowInputEvent(window->GetID());

        IPCClientSendMessage(0, "producer", subwindow->GetSurface()->GetProducer()->AsObject());
    }

    void IPCClientOnMessage(int32_t sequence, const std::string &message, const sptr<IRemoteObject> &robj) override
    {
        if (message == "quit") {
            ExitTest();
        }
    }

    int32_t OnFrameAvailable(sptr<SurfaceBuffer> &buffer)
    {
        auto eglData = buffer->GetEglData();
        if (eglData == nullptr) {
            return -1;
        }

        if (shader == nullptr) {
            shader = std::make_unique<Shader>(vertexShaderSource, fragmentShaderSource);
            if (!shader->Available()) {
                shader = nullptr;
                return -1;
            }
        }

        glBindFramebuffer(GL_FRAMEBUFFER, eglData->GetFrameBufferObj());
        shader->Bind();
        glViewport(0, 0, buffer->GetWidth(), buffer->GetHeight());
        static const GLfloat verts[][0x2] = { {-1, -1}, {-1, 1}, {1, -1}, {1, 1} };
        static const GLfloat texCoords[][0x2] = { {0, 0}, {0, 1}, {1, 0}, {1, 1} };
        auto position = shader->GetAttribLocation("a_position");
        glEnableVertexAttribArray(position);
        glVertexAttribPointer(position, 0x2, GL_FLOAT, GL_FALSE, 0, verts);
        auto texCoord = shader->GetAttribLocation("a_texCoord");
        glEnableVertexAttribArray(texCoord);
        glVertexAttribPointer(texCoord, 0x2, GL_FLOAT, GL_FALSE, 0, texCoords);

        glActiveTexture(GL_TEXTURE0 + 0);
        glBindTexture(GL_TEXTURE_2D, eglData->GetTexture());
        shader->SetUniform1i("u_texture", 0);
        glDrawArrays(GL_TRIANGLE_STRIP, 0, 0x4);
        glFinish();
        return -1;
    }

private:
    sptr<Window> window = nullptr;
    sptr<Subwindow> subwindow = nullptr;
    std::unique_ptr<Shader> shader = nullptr;
    static constexpr const char *vertexShaderSource =
        "attribute vec2 a_position;\n"
        "attribute vec2 a_texCoord;\n"
        "varying vec2 v_texCoord;\n"
        "void main() {\n"
        "    gl_Position = vec4(a_position, 1.0, 1.0);\n"
        "    v_texCoord = a_texCoord;\n"
        "}\n";
    static constexpr const char *fragmentShaderSource =
        "precision mediump float;\n"
        "varying vec2 v_texCoord;\n"
        "uniform sampler2D u_texture;\n"
        "void main() {\n"
        "    gl_FragColor = texture2D(u_texture, v_texCoord);\n"
        "    float avg = 0.2126 * gl_FragColor.r + 0.7152 * gl_FragColor.g + 0.0722 * gl_FragColor.b;\n"
        "    gl_FragColor = vec4(avg, avg, avg, 1.0);\n"
        "}\n";
    sptr<NativeTestSync> windowSync = nullptr;
} g_autoload1;
} // namespace
