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

#include <cstring>
#include <functional>
#include <iostream>
#include <securec.h>
#include <thread>
#include <unistd.h>
#include <vector>

#include <display_gralloc.h>
#include <hilog/log.h>
#include <iservice_registry.h>
#include <surface.h>
#include <vsync_helper.h>
#include <window_manager.h>
#include <zlib.h>

using namespace OHOS;

#define TESTLOG(func, fmt, ...) (void)func(LOG_LABEL, "%{public}s: " fmt, __func__, ##__VA_ARGS__)
#define TESTLOG_F(fmt, ...)  TESTLOG(HiviewDFX::HiLog::Fatal, fmt, ##__VA_ARGS__)
#define TESTLOG_E(fmt, ...)  TESTLOG(HiviewDFX::HiLog::Error, fmt, ##__VA_ARGS__)
#define TESTLOG_W(fmt, ...)  TESTLOG(HiviewDFX::HiLog::Warn, fmt, ##__VA_ARGS__)
#define TESTLOG_I(fmt, ...)  TESTLOG(HiviewDFX::HiLog::Info, fmt, ##__VA_ARGS__)
#define TESTLOG_D(fmt, ...)  TESTLOG(HiviewDFX::HiLog::Debug, fmt, ##__VA_ARGS__)

namespace {
constexpr OHOS::HiviewDFX::HiLogLabel LOG_LABEL = {LOG_CORE, 0, "WMS_TEST"};
constexpr int32_t BUFFER_QUEUE_SIZE = 5;
std::mutex g_subwindowMutex;
std::unique_ptr<SubWindow> g_subWindow;
std::mutex g_windowMutex;
std::unique_ptr<Window> g_window;
BufferRequestConfig g_subWindowConfig;
BufferRequestConfig g_windowConfig;
BufferRequestConfig g_ipcConfig;
sptr<Surface> g_ipcSurface;
char g_argv2[32] = {};
using SyncFunc_t = void(*)(int64_t, void*);

int64_t GetNowTime()
{
    struct timeval start = {};
    gettimeofday(&start, nullptr);
    constexpr uint32_t secToUsec = 1000 * 1000;
    return static_cast<int64_t>(start.tv_sec) * secToUsec + start.tv_usec;
}

void WindowInfoCb(WindowInfo &info)
{
    TESTLOG_I("%{public}s width(%{public}d) height(%{public}d)"
        "pos_x(%{public}d) pos_y(%{public}d)", __func__, info.width, info.height, info.pos_x, info.pos_y);
    g_windowConfig.height = info.height;
    g_windowConfig.width = info.width;
}

void WindowInfoCbSub(WindowInfo &info)
{
    TESTLOG_I("%{public}s width(%{public}d) height(%{public}d)"
        "pos_x(%{public}d) pos_y(%{public}d)", __func__, info.width, info.height, info.pos_x, info.pos_y);
    g_subWindowConfig.height = info.height;
    g_subWindowConfig.width = info.width;
}

void PointerButtonCb(int32_t x, int32_t y, int32_t state, int32_t time)
{
    constexpr int32_t startX = 100;
    constexpr int32_t startY = 100;
    constexpr int32_t diff = 10;
    constexpr int32_t boardX = 700;
    constexpr int32_t boardY = 500;
    static int32_t testx = startX;
    static int32_t testy = startY;

    if (state == 0) {
        testx = (testx + diff) % boardX;
        testy = (testy + diff) % boardY;
        g_window->Move(testx, testy);
    }

    TESTLOG_I("%{public}s state(%{public}d) x(%{public}d)"
        "y(%{public}d) time(%{public}d)", __func__, state, x, y, time);
}

void Draw(uint8_t * const addr, uint32_t width, uint32_t height, uint32_t count)
{
    if (addr == nullptr) {
        return;
    }

    constexpr uint32_t bpp = 4;
    constexpr uint32_t color1 = 0xff / 3 * 0;
    constexpr uint32_t color2 = 0xff / 3 * 1;
    constexpr uint32_t color3 = 0xff / 3 * 2;
    constexpr uint32_t color4 = 0xff / 3 * 3;
    constexpr uint32_t bigDiv = 7;
    constexpr uint32_t smallDiv = 10;
    uint32_t c = count % (bigDiv * smallDiv);
    uint32_t stride = width * bpp;
    uint32_t beforeCount = height * c / bigDiv / smallDiv;
    uint32_t afterCount = height - beforeCount - 1;

    auto ret = memset_s(addr, stride * height, color3, beforeCount * stride);
    if (ret) {
        printf("memset_s: %s\n", strerror(ret));
    }

    ret = memset_s(addr + (beforeCount + 1) * stride, stride * height, color1, afterCount * stride);
    if (ret) {
        printf("memset_s: %s\n", strerror(ret));
    }

    for (uint32_t i = 0; i < bigDiv; i++) {
        ret = memset_s(addr + (i * height / bigDiv) * stride, stride * height, color4, stride);
        if (ret) {
            printf("memset_s: %s\n", strerror(ret));
        }
    }

    ret = memset_s(addr + beforeCount * stride, stride * height, color2, stride);
    if (ret) {
        printf("memset_s: %s\n", strerror(ret));
    }
}

void Sync(int64_t time, int32_t count, sptr<Surface> const &surface, BufferRequestConfig &config)
{
    sptr<SurfaceBuffer> buffer;
    int32_t releaseFence;

    if (surface == nullptr) {
        return;
    }

    SurfaceError ret = surface->RequestBuffer(buffer, releaseFence, config);
    TESTLOG_I("wms-test 2");
    if (ret != SURFACE_ERROR_OK) {
        printf("wms-test start buffer NULL\n");
        return;
    }

    TESTLOG_I("wms-test start 4 width %{public}d, height %{public}d", config.width, config.height);

    Draw(static_cast<uint8_t*>(buffer->GetVirAddr()), config.width, config.height, count);
    TESTLOG_I("wms-test start 5");

    BufferFlushConfig flushConfig = {
        .damage = {
            .x = 0,
            .y = 0,
            .w = config.width,
            .h = config.height,
        },
        .timestamp = 0,
    };
    ret = surface->FlushBuffer(buffer, -1, flushConfig);
    TESTLOG_I("Flush: %{public}d", ret);
}

void RequestSync(const SyncFunc_t syncFunc)
{
    struct FrameCallback cb = {
        .timestamp_ = 0,
        .userdata_ = nullptr,
        .callback_ = syncFunc,
    };
    VsyncError ret = VsyncHelper::Current()->RequestFrameCallback(cb);
    TESTLOG_I("RequestFrameCallback inner %{public}d", ret);
    if (ret) {
        printf("RequestFrameCallback inner %d\n", ret);
    }
}

void SyncL(int64_t time, void*)
{
    TESTLOG_I("start");
    std::lock_guard<std::mutex> lock(g_windowMutex);

    if (g_window == nullptr) {
        return;
    }

    sptr<Surface> surface = g_window->GetSurface();
    if (surface == nullptr) {
        printf("wms-test NULL == surface\n");
        return;
    }

    TESTLOG_I("wms-test 1");

    static uint32_t count = 0;
    g_window->GetRequestConfig(g_windowConfig);
    Sync(time, count++, surface, g_windowConfig);
    RequestSync(SyncL);
    TESTLOG_I("end");
}

void SyncLSub(int64_t time, void*)
{
    TESTLOG_I("start");
    std::lock_guard<std::mutex> lock(g_subwindowMutex);
    if (g_subWindow == nullptr) {
        return;
    }
    sptr<Surface> surface = g_subWindow->GetSurface();
    if (NULL == surface) {
        printf("wms-test NULL == surface\n");
        return;
    }
    TESTLOG_I("wms-test 1");

    static uint32_t count = 0;
    g_subWindow->GetRequestConfig(g_subWindowConfig);
    Sync(time, count++, surface, g_subWindowConfig);
    RequestSync(SyncLSub);
    TESTLOG_I("end");
}

void SyncIPC(int64_t time, void*)
{
    static uint32_t count = 0;
    Sync(time, count++, g_ipcSurface, g_ipcConfig);
    RequestSync(SyncIPC);
}

void FlushSub(int32_t pos, int32_t width, int32_t height, int32_t type)
{
    TESTLOG_I("wms-test start 1 pos=%{public}d", pos);
    WindowConfig config = {
        .width = width,
        .height = height,
        .pos_x = pos,
        .pos_y = pos,
        .format = PIXEL_FMT_RGBA_8888,
        .type = type
    };

    TESTLOG_I("wms-test start 2");
    g_subWindow = WindowManager::GetInstance()->CreateSubWindow(g_window->GetWindowID(), &config);
    g_subWindow->GetSurface()->SetQueueSize(BUFFER_QUEUE_SIZE);
    g_subWindow->GetRequestConfig(g_subWindowConfig);
    g_subWindow->RegistWindowInfoChangeCb(WindowInfoCbSub);
    TESTLOG_I("wms-test start 3");

    RequestSync(SyncLSub);

    TESTLOG_I("wms-test start 5");
}

void FlushSubVideo(int32_t pos, int32_t width, int32_t height, int32_t type)
{
    TESTLOG_I("wms-test start 1 pos=%{public}d", pos);
    WindowConfig config = {
        .width = width,
        .height = height,
        .pos_x = pos,
        .pos_y = pos,
        .format = PIXEL_FMT_RGBA_8888,
        .type = type
    };

    TESTLOG_I("wms-test start 2");
    g_subWindow = WindowManager::GetInstance()->CreateSubWindow(g_window->GetWindowID(), &config);
    g_subWindow->GetRequestConfig(g_subWindowConfig);
    g_subWindow->RegistWindowInfoChangeCb(WindowInfoCbSub);
    TESTLOG_I("wms-test start 5");
}

void Flush(int32_t pos, int32_t width, int32_t height, int32_t type)
{
    TESTLOG_I("wms-test start 1 pos=%{public}d", pos);
    WindowConfig config = {
        .width = width,
        .height = height,
        .pos_x = pos,
        .pos_y = pos,
        .format = PIXEL_FMT_RGBA_8888,
        .type = type
    };

    TESTLOG_I("wms-test start 2");
    g_window = WindowManager::GetInstance()->CreateWindow(&config);
    g_window->GetSurface()->SetQueueSize(BUFFER_QUEUE_SIZE);
    TESTLOG_I("wms-test start 3");
    g_window->GetRequestConfig(g_windowConfig);
    g_window->RegistPointerButtonCb(PointerButtonCb);
    g_window->RegistWindowInfoChangeCb(WindowInfoCb);
    g_window->SwitchTop();
    RequestSync(SyncL);

    TESTLOG_I("wms-test start 5");
}

void ColorDraw(uint32_t * const addr, int32_t width, int32_t height, int32_t count)
{
    if (addr == nullptr || width <= 0 || height <= 0) {
        return;
    }

    constexpr int32_t wdiv = 2;
    constexpr int32_t colorTable[][wdiv] = {
        {0xffff0000, 0xffff00ff},
        {0xffff0000, 0xffffff00},
        {0xff00ff00, 0xffffff00},
        {0xff00ff00, 0xff00ffff},
        {0xff0000ff, 0xff00ffff},
        {0xff0000ff, 0xffff00ff},
        {0xff777777, 0xff777777},
        {0xff777777, 0xff777777},
    };
    const int32_t hdiv = sizeof(colorTable) / sizeof(*colorTable);

    for (int32_t i = 0; i < height; i++) {
        auto table = colorTable[i / (height / hdiv)];

        for (int32_t j = 0; j < wdiv; j++) {
            auto color = table[j];

            for (int32_t k = j * width / wdiv; k < (j + 1) * width / wdiv; k++) {
                addr[i * width + k] = color;
            }
        }
    }
}

void RequestDataSync(const SyncFunc_t syncFunc, void* data)
{
    struct FrameCallback cb = {
        .timestamp_ = 0,
        .userdata_ = data,
        .callback_ = syncFunc,
    };
    VsyncError ret = VsyncHelper::Current()->RequestFrameCallback(cb);
    TESTLOG_I("RequestFrameCallback inner %{public}d", ret);
    if (ret) {
        printf("RequestFrameCallback inner %d\n", ret);
    }
}

void ColorSync(int64_t time, void *data)
{
    Window *window = reinterpret_cast<Window *>(data);
    sptr<Surface> surface = window->GetSurface();
    if (surface == nullptr) {
        printf("ColorSync FFFFFFFFFailed, surface is nullptr!!\n");
        return;
    }

    do {
        sptr<SurfaceBuffer> buffer;
        int32_t releaseFence;
        BufferRequestConfig config;
        window->GetRequestConfig(config);

        SurfaceError ret = surface->RequestBuffer(buffer, releaseFence, config);
        if (ret != SURFACE_ERROR_OK) {
            printf("ColorSync RequestBuffer FFFFFFFFFfailed\n");
            break;
        }
        if (buffer == nullptr) {
            break;
        }

        static uint32_t count = 0;
        auto addr = static_cast<uint32_t*>(buffer->GetVirAddr());
        ColorDraw(addr, buffer->GetWidth(), buffer->GetHeight(), count++);

        BufferFlushConfig flushConfig = {
            .damage = {
                .x = 0,
                .y = 0,
                .w = buffer->GetWidth(),
                .h = buffer->GetHeight(),
            },
            .timestamp = 0,
        };
        ret = surface->FlushBuffer(buffer, -1, flushConfig);
    } while (false);

    RequestDataSync(ColorSync, data);
}

void ColorFlush()
{
    WindowConfig config = {
        .width = WindowManager::GetInstance()->GetMaxWidth(),
        .height = WindowManager::GetInstance()->GetMaxHeight(),
        .pos_x = 0,
        .pos_y = 0,
        .format = PIXEL_FMT_RGBA_8888,
        .type = WINDOW_TYPE_NORMAL
    };

    static auto window = WindowManager::GetInstance()->CreateWindow(&config);
    if (window == nullptr) {
        printf("CreateWindow FFFFFfailed, ColorSync failed\n");
        return;
    }

    void *windowPtr = window.get();
    RequestDataSync(ColorSync, windowPtr);
}

void ShotDone(ImageInfo &info)
{
    printf("---------------shot_done\n");
    FILE *fp = fopen("test1111", "w+");
    if (fp == nullptr) {
        printf("---------------shot_done open file failed\n");
        return;
    }

    fwrite(info.data, sizeof(uint8_t), info.dataSize, fp);
    fclose(fp);
}

void PostTask(std::function<void()> func, uint32_t delayTime = 0)
{
    auto handler = AppExecFwk::EventHandler::Current();
    if (handler) {
        handler->PostTask(func, delayTime);
    }
}

void Test1()
{
    Flush(0, WindowManager::GetInstance()->GetMaxWidth(),
        WindowManager::GetInstance()->GetMaxHeight(), WINDOW_TYPE_NORMAL);
}

void Test2()
{
    constexpr int32_t barWidth = 200;
    constexpr int32_t barHeight = 100;
    Flush(0, barWidth, barHeight, WINDOW_TYPE_STATUS_BAR);
}

void Test3()
{
    constexpr int32_t barWidth = 200;
    constexpr int32_t barHeight = 100;
    Flush(0, barWidth, barHeight, WINDOW_TYPE_NAVI_BAR);
}

void Test4()
{
    constexpr int32_t windowId = 5000;
    WindowManager::GetInstance()->StartShotWindow(windowId, ShotDone);
}

void Test5()
{
    WindowManager::GetInstance()->StartShotScreen(ShotDone);
}

void Test6()
{
    constexpr int32_t subPosition = 200;
    constexpr int32_t subWidth = 50;
    constexpr int32_t subHeight = 50;

    Flush(0, 0, 0, WINDOW_TYPE_NORMAL);
    FlushSub(subPosition, subWidth, subHeight, WINDOW_TYPE_NORMAL);
}

void Test7()
{
    constexpr int32_t videoPosition = 100;
    constexpr int32_t videoWidth = 100;
    constexpr int32_t videoHeight = 100;

    Flush(0, 0, 0, WINDOW_TYPE_NORMAL);
    FlushSubVideo(videoPosition, videoWidth, videoHeight, WINDOW_TYPE_VIDEO);
}

void Test8()
{
    ColorFlush();
}

void Test9()
{
    static int32_t maxWidth = WindowManager::GetInstance()->GetMaxWidth();
    static int32_t maxHeight = WindowManager::GetInstance()->GetMaxHeight();
    Flush(0, maxWidth, maxHeight, WINDOW_TYPE_NORMAL);

    static std::function<void()> func = [&]() {
        constexpr int32_t step = 37;
        constexpr int32_t minWidthHeight = 100;
        static int32_t width = maxWidth;
        static int32_t height = maxHeight;

        width -= step;
        height -= step;

        if (width < minWidthHeight) {
            width += maxWidth;
        }

        if (height < minWidthHeight) {
            height += maxHeight;
        }

        g_window->ReSize(width, height);

        constexpr uint32_t delayTime = 300;
        PostTask(func, delayTime);
    };

    PostTask(func);
}

void Test10()
{
    constexpr int32_t wh = 100;
    Flush(0, wh, wh, WINDOW_TYPE_NORMAL);
    g_window->ReSize(wh, wh);

    auto handler = AppExecFwk::EventHandler::Current();
    static std::function<void()> func = [&]() {
        constexpr int32_t step = 37;
        constexpr int32_t minWidthHeight = 100;
        static int32_t maxWidth = WindowManager::GetInstance()->GetMaxWidth();
        static int32_t maxHeight = WindowManager::GetInstance()->GetMaxHeight();
        static int32_t width = maxWidth;
        static int32_t height = maxHeight;

        width -= step;
        height -= step;

        if (width < minWidthHeight) {
            width += maxWidth;
        }

        if (height < minWidthHeight) {
            height += maxHeight;
        }

        g_window->Move(width, height);

        constexpr uint32_t delayTime = 300;
        PostTask(func, delayTime);
    };

    PostTask(func);
}

void Test11()
{
    Test1();
    constexpr uint32_t delayTime = 2000;
    PostTask([]() { exit(0); }, delayTime);
}

void Test12()
{
    class Object : public RefBase {
    public:
        Object()
        {
            TESTLOG_D("+ Object");
        }

        ~Object()
        {
            TESTLOG_D("- Object");
        }
    };
    wptr<Object> wptr;
    sptr<Object> sptr;

    std::thread thread([&]() {
        while (true) {
            TESTLOG_D("wptr.promote");
            auto sp = wptr.promote();
            if (sp == nullptr) {
                TESTLOG_D("sp is nullptr");
            } else {
                TESTLOG_D("sp isn't nullptr");
            }
            TESTLOG_D("sleep(0)");
            sleep(0);
        }
    });

    while (true) {
        TESTLOG_D("sptr = new Object()");
        sptr = new Object();
        TESTLOG_D("wptr = sptr");
        wptr = sptr;
        TESTLOG_D("sptr = nullptr");
        sptr = nullptr;
    }
}

void Test13()
{
    constexpr int32_t subPosition = 200;
    constexpr int32_t subWidth = 50;
    constexpr int32_t subHeight = 50;

    Flush(0, 0, 0, WINDOW_TYPE_NORMAL);
    FlushSub(subPosition, subWidth, subHeight, WINDOW_TYPE_NORMAL);

    auto destorySubWindowFunc = []() {
        std::lock_guard<std::mutex> lock(g_subwindowMutex);
        WindowManager::GetInstance()->DestroyWindow(g_subWindow->GetWindowID());
        g_subWindow = nullptr;
    };
    constexpr uint32_t delayTime1 = 2000;
    PostTask(destorySubWindowFunc, delayTime1);

    auto destoryWindowFunc = []() {
        std::lock_guard<std::mutex> lock(g_windowMutex);
        WindowManager::GetInstance()->DestroyWindow(g_window->GetWindowID());
        g_window = nullptr;
    };
    constexpr uint32_t delayTime2 = delayTime1 * 2;
    PostTask(destoryWindowFunc, delayTime2);

    constexpr uint32_t delayTime3 = delayTime1 * 3;
    PostTask([]() { exit(0); }, delayTime3);
}

void Test14()
{
    constexpr int32_t videoPosition = 100;
    constexpr int32_t videoWidth = 100;
    constexpr int32_t videoHeight = 100;

    Flush(0, 0, 0, WINDOW_TYPE_NORMAL);
    FlushSubVideo(videoPosition, videoWidth, videoHeight, WINDOW_TYPE_VIDEO);

    auto destorySubWindowFunc = []() {
        std::lock_guard<std::mutex> lock(g_subwindowMutex);
        WindowManager::GetInstance()->DestroyWindow(g_subWindow->GetWindowID());
        g_subWindow = nullptr;
    };
    constexpr uint32_t delayTime1 = 3000;
    PostTask(destorySubWindowFunc, delayTime1);

    auto destoryWindowFunc = []() {
        std::lock_guard<std::mutex> lock(g_windowMutex);
        WindowManager::GetInstance()->DestroyWindow(g_window->GetWindowID());
        g_window = nullptr;
    };
    constexpr uint32_t delayTime2 = delayTime1 * 2;
    PostTask(destoryWindowFunc, delayTime2);

    constexpr uint32_t delayTime3 = delayTime1 * 3;
    PostTask([]() { exit(0); }, delayTime3);
}

int g_pipeFd[2];
constexpr int32_t SAID = 4699;
void Test15ChildProcess()
{
    TESTLOG_I("wms-test child_process start");
    WindowConfig config = {
        .width = 200,
        .height = 200,
        .pos_x = 0,
        .pos_y = 0,
        .format = PIXEL_FMT_RGBA_8888,
        .type = WINDOW_TYPE_NORMAL
    };

    g_window = WindowManager::GetInstance()->CreateWindow(&config);
    if (g_window == nullptr) {
        printf("%s g_window == nullptr\n", __func__);
        exit(0);
    }

    g_window->SwitchTop();
    auto surface = g_window->GetSurface();
    if (surface == nullptr) {
        printf("%s surface == nullptr\n", __func__);
        exit(0);
    }

    auto producer = surface->GetProducer();
    if (producer == nullptr) {
        printf("%s producer == nullptr\n", __func__);
        exit(0);
    }

    auto producerObject = producer->AsObject();
    if (producerObject == nullptr) {
        printf("%s producerObject == nullptr\n", __func__);
        exit(0);
    }

    auto sam = SystemAbilityManagerClient::GetInstance().GetSystemAbilityManager();
    if (sam == nullptr) {
        printf("%s sam == nullptr\n", __func__);
        exit(0);
    }

    sam->AddSystemAbility(SAID, producerObject);

    g_window->GetRequestConfig(g_ipcConfig);
    write(g_pipeFd[1], &g_ipcConfig, sizeof(g_ipcConfig));

    sleep(0);

    char buf[10];
    read(g_pipeFd[0], buf, sizeof(buf));
    sam->RemoveSystemAbility(SAID);
    exit(0);
}

void Test15()
{
    pipe(g_pipeFd);
    pid_t pid = fork();
    if (pid < 0) {
        printf("%s fork failed", __func__);
        exit(1);
    }

    if (pid == 0) {
        Test15ChildProcess();
    } else {
        // pid > 0
        read(g_pipeFd[0], &g_ipcConfig, sizeof(g_ipcConfig));

        auto sam = SystemAbilityManagerClient::GetInstance().GetSystemAbilityManager();
        if (sam == nullptr) {
            printf("%s child_process sam == nullptr\n", __func__);
            exit(0);
        }

        auto robj = sam->GetSystemAbility(SAID);
        if (robj == nullptr) {
            printf("%s child_process robj == nullptr\n", __func__);
            exit(0);
        }

        auto bp = iface_cast<IBufferProducer>(robj);
        if (bp == nullptr) {
            printf("%s child_process bp == nullptr\n", __func__);
            exit(0);
        }

        g_ipcSurface = Surface::CreateSurfaceAsProducer(bp);
        if (g_ipcSurface == nullptr) {
            printf("%s child_process g_ipcSurface == nullptr\n", __func__);
            exit(0);
        }

        g_ipcSurface->SetQueueSize(BUFFER_QUEUE_SIZE);
        RequestSync(SyncIPC);

        auto task = []() {
            char buf[10] = "end";
            write(g_pipeFd[1], buf, sizeof(buf));
            exit(0);
        };
        constexpr uint32_t delayTime = 3000;
        PostTask(task, delayTime);
    }
}

int32_t Test16Compress(const std::unique_ptr<uint8_t[]> &c,
    unsigned long &ul, const std::unique_ptr<uint8_t[]> &p, uint32_t size)
{
    auto ret = compress(c.get(), &ul, p.get(), size);
    if (ret) {
        printf("compress failed, %d\n", ret);
    }
    return ret;
}

int32_t Test16Uncompress(const std::unique_ptr<uint8_t[]> &c,
    unsigned long &ul, const std::unique_ptr<uint8_t[]> &p, uint32_t size)
{
    auto ret = uncompress(c.get(), &ul, p.get(), size);
    if (ret) {
        printf("uncompress failed, %d\n", ret);
    }
    return ret;
}

void Test16()
{
    if (g_argv2[0] == '\0') {
        return;
    }

    uint32_t size = -1;
    int ret = sscanf_s(g_argv2, "%u", &size);
    if (ret == 0) {
        printf("%s parse argv[2] failed\n", __func__);
        return;
    }

    auto ptr = std::make_unique<uint8_t[]>(size);
    auto ptr32 = reinterpret_cast<uint32_t *>(ptr.get());
    for (uint32_t i = 0; i < size / (sizeof(uint32_t) / sizeof(uint8_t)); i++) {
        ptr32[i] = 0xff000000;
    }

    auto clength = compressBound(size);
    auto compressed = std::make_unique<uint8_t[]>(clength);

    unsigned long ulength = clength;
    if (Test16Compress(compressed, ulength, ptr, size) != Z_OK) {
        return;
    }
    printf("compress length: %lu\n", ulength);

    int64_t start = GetNowTime();
    unsigned long uulength = size;
    if (Test16Uncompress(ptr, uulength, compressed, ulength) != Z_OK) {
        return;
    }
    printf("uncompress time: %lld\n", GetNowTime() - start);
}

struct WindowManagerTest {
    int32_t id;
    const char *desc;
    void(*func)();
};

#define ADD_TEST(tests, id_, desc_) \
    tests.push_back({ .id = id_, .desc = desc_, .func = Test##id_ })

void InitTest(std::vector<struct WindowManagerTest>& tests)
{
    ADD_TEST(tests, 1, "normal_window");
    ADD_TEST(tests, 2, "status_bar");
    ADD_TEST(tests, 3, "navi_bar");
    ADD_TEST(tests, 4, "shot_window");
    ADD_TEST(tests, 5, "shot_screen");
    ADD_TEST(tests, 6, "subwindow");
    ADD_TEST(tests, 7, "subvideo");
    ADD_TEST(tests, 8, "colorbar");
    ADD_TEST(tests, 9, "resize");
    ADD_TEST(tests, 10, "move");
    ADD_TEST(tests, 11, "exit after sleep 2 seconds");
    ADD_TEST(tests, 12, "raise wptr double free");
    ADD_TEST(tests, 13, "subwindow destory");
    ADD_TEST(tests, 14, "subvideo destory");
    ADD_TEST(tests, 15, "ipc draw");
    ADD_TEST(tests, 16, "uncompress perf");
}

void usage(const char* argv0, std::vector<struct WindowManagerTest>& tests)
{
    printf("usage: %s test_id\n", argv0);
    for (auto it = tests.begin(); it != tests.end(); it++) {
        printf("test %d: %s\n", it->id, it->desc);
    }
}
} // anonymous namespace

int32_t main(int32_t argc, const char* const argv[])
{
    TESTLOG_I("wms-test start");

    std::vector<struct WindowManagerTest> tests;
    InitTest(tests);

    if (argc <= 1) {
        usage(argv[0], tests);
        return 0;
    }

    int32_t testcase = -1;
    int ret = sscanf_s(argv[1], "%d", &testcase);
    if (ret == 0) {
        usage(argv[0], tests);
        return 1;
    }

    constexpr int32_t c = 2;
    if (argc > c) {
        memcpy_s(g_argv2, sizeof(g_argv2) - 1, argv[c], strlen(argv[c]));
    }

    auto runner = AppExecFwk::EventRunner::Create(false);
    auto handler = std::make_shared<AppExecFwk::EventHandler>(runner);
    auto condition = [testcase](auto& item) { return item.id == testcase; };
    auto test = std::find_if(tests.begin(), tests.end(), condition);
    if (test != tests.end()) {
        handler->PostTask(test->func);
        printf("%d %s run!\n", test->id, test->desc);
    } else {
        printf("not found test %d\n", testcase);
        return 0;
    }

    runner->Run();
    return 0;
}
