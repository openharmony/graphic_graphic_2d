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

#include "wmserver_test.h"
#include <pthread.h>
#include <sys/mman.h>
#include <wayland-client.h>
#include <stdlib.h>

// for debug.
#define LOG(fmt, ...)  //printf("wmserver_test " fmt "\n", ##__VA_ARGS__)

namespace OHOS {
namespace {
static void WindowShotError(void *data, struct wms *wms, uint32_t error, uint32_t window_id)
{
    LOG("WindowShotError error: %d", error);
    LOG("WindowShotError window_id: %d", window_id);
    WMServerTest::shotInfo.status = error;
    WMServerTest::shotInfo.id = window_id;
    {
        std::unique_lock<std::mutex> lck(WMServerTest::syncMutex);
        WMServerTest::replyFlag = true;
        WMServerTest::syncVariable.notify_all();
    }
}

static void WindowShotDone(void *data,
                struct wms *wms,
                uint32_t window_id, int32_t fd,
                int32_t width, int32_t height, int32_t stride,
                uint32_t format, uint32_t seconds, uint32_t nanoseconds)
{
    size_t size = stride * height;

    LOG("WindowShotDone window_id: %d", window_id);
    LOG("WindowShotDone fd: %d", fd);
    LOG("WindowShotDone width: %d", width);
    LOG("WindowShotDone height: %d", height);
    LOG("WindowShotDone stride: %d", stride);
    LOG("WindowShotDone format: %d", format);
    LOG("WindowShotDone seconds: %d", seconds);
    LOG("WindowShotDone nanoseconds: %d", nanoseconds);
    
    LOG("windowshot OK!!!");

    close(fd);

    WMServerTest::shotInfo.status = WMS_ERROR_OK;
    WMServerTest::shotInfo.id = window_id;
    WMServerTest::shotInfo.fd = fd;
    WMServerTest::shotInfo.width = width;
    WMServerTest::shotInfo.height = height;
    WMServerTest::shotInfo.stride = stride;
    WMServerTest::shotInfo.format = format;
    WMServerTest::shotInfo.seconds = seconds;
    WMServerTest::shotInfo.nanoseconds = nanoseconds;
    {
        std::unique_lock<std::mutex> lck(WMServerTest::syncMutex);
        WMServerTest::replyFlag = true;
        WMServerTest::syncVariable.notify_all();
    }
}

static void ScreenShotError(void *data, struct wms *wms, uint32_t error, uint32_t screen_id)
{
    LOG("ScreenShotError error: %d", error);
    LOG("ScreenShotError screen_id: %d", screen_id);
    WMServerTest::shotInfo.status = error;
    WMServerTest::shotInfo.id = screen_id;
    {
        std::unique_lock<std::mutex> lck(WMServerTest::syncMutex);
        WMServerTest::replyFlag = true;
        WMServerTest::syncVariable.notify_all();
    }
}

static void ScreenShotDone(void *data,
                struct wms *wms,
                uint32_t screen_id, int32_t fd,
                int32_t width, int32_t height, int32_t stride,
                uint32_t format, uint32_t seconds, uint32_t nanoseconds)
{
    size_t size = stride * height;

    LOG("ScreenShotDone screen_id: %d", screen_id);
    LOG("ScreenShotDone fd: %d", fd);
    LOG("ScreenShotDone width: %d", width);
    LOG("ScreenShotDone height: %d", height);
    LOG("ScreenShotDone stride: %d", stride);
    LOG("ScreenShotDone format: %d", format);
    LOG("ScreenShotDone seconds: %d", seconds);
    LOG("ScreenShotDone nanoseconds: %d", nanoseconds);

    LOG("screenshot OK!!!");
    close(fd);

    WMServerTest::shotInfo.status = WMS_ERROR_OK;
    WMServerTest::shotInfo.id = screen_id;
    WMServerTest::shotInfo.fd = fd;
    WMServerTest::shotInfo.width = width;
    WMServerTest::shotInfo.height = height;
    WMServerTest::shotInfo.stride = stride;
    WMServerTest::shotInfo.format = format;
    WMServerTest::shotInfo.seconds = seconds;
    WMServerTest::shotInfo.nanoseconds = nanoseconds;
    {
        std::unique_lock<std::mutex> lck(WMServerTest::syncMutex);
        WMServerTest::replyFlag = true;
        WMServerTest::syncVariable.notify_all();
    }
}

static void ReplyStatus(void *data, struct wms *wms, uint32_t status)
{
    LOG("ReplyStatus status: %d", status);
    WMServerTest::replyStatus = status;
    {
        std::unique_lock<std::mutex> lck(WMServerTest::syncMutex);
        WMServerTest::replyFlag = true;
        WMServerTest::syncVariable.notify_all();
    }
}

static void DisplayMode(void *data, struct wms *wms, uint32_t flag)
{
    LOG("DisplayMode flag: %d", flag);
}

void ScreenUpdate(void *data,
                struct wms *wms,
                uint32_t screen_id,
                const char *name,
                uint32_t update_state,
                int width, int height)
{
    LOG("screenUpdate screen_id: %d", screen_id);
    LOG("screenUpdate name: %s", name);
    LOG("screenUpdate update_state: %d", update_state);
    LOG("screenUpdate width: %d", width);
    LOG("screenUpdate height: %d", height);

    if (update_state == WMS_SCREEN_STATUS_ADD) {
        LOG("screen add. ");
        WMSDisplayInfo info;
        info.id = screen_id;
        info.name = name;
        info.width = width;
        info.height = height;
        WMServerTest::displayInfo.push_back(info);
    }
    else {
        LOG("screen destroy.");
        for (auto it = WMServerTest::displayInfo.begin(); it != WMServerTest::displayInfo.end(); it++) {
            if (it->id == screen_id) {
                WMServerTest::displayInfo.erase(it);
                break;
            }
        }
    }
}

void WindowUpdate(void *data, struct wms *wms, uint32_t update_state, uint32_t window_id,
                    int32_t x, int32_t y, int32_t width, int32_t height)
{
    LOG("WindowUpdate window_id: %d", window_id);
    LOG("WindowUpdate update_state: %d", update_state);
    LOG("WindowUpdate x:%d, y:%d", x, y);
    LOG("WindowUpdate width:%d, height:%d", width, height);

    WMServerTest::windowStatus.status = update_state;
    WMServerTest::windowStatus.wid = window_id;
    WMServerTest::windowStatus.x = x;
    WMServerTest::windowStatus.y = y;
    WMServerTest::windowStatus.width = width;
    WMServerTest::windowStatus.height = height;
    
    if (update_state == WMS_WINDOW_STATUS_CREATED) {
        LOG("window %d create. ", window_id);
        {
            std::unique_lock<std::mutex> lck(WMServerTest::syncMutex);
            WMServerTest::replyFlag = true;
            WMServerTest::syncVariable.notify_all();
        }
    }
    else if (update_state == WMS_WINDOW_STATUS_FAILED) {
        LOG("window create failed. ");
        {
            std::unique_lock<std::mutex> lck(WMServerTest::syncMutex);
            WMServerTest::replyFlag = true;
            WMServerTest::syncVariable.notify_all();
        }
    }
    else {
        LOG("window %d destroy. ", window_id);
        {
            std::unique_lock<std::mutex> lck(WMServerTest::destroyMutex);
            WMServerTest::destroyReplyFlag = true;
            WMServerTest::destroyVariable.notify_all();
        }
    }
}

void RegistryGlobal(void *data, struct wl_registry *registry,
               uint32_t id, const char *interface, uint32_t version)
{
    struct WMSContext *ctx = (struct WMSContext *)(data);

    if (strcmp(interface, "wl_compositor") == 0) {
        ctx->compositor = (struct wl_compositor*)wl_registry_bind(registry, id, &wl_compositor_interface, 1);
        return;
    }

    if (strcmp(interface, "wl_shm") == 0) {
        ctx->wl_shm = (struct wl_shm*)wl_registry_bind(registry, id, &wl_shm_interface, 1);
        return;
    }

    if (strcmp(interface, "wms") == 0) {
        ctx->wms = (struct wms *)wl_registry_bind(registry, id, &wms_interface, 1);

        static struct wms_listener wmsListener = {
            WindowUpdate,
            ScreenUpdate,
            DisplayMode,
            ReplyStatus,
            ScreenShotDone,
            ScreenShotError,
            WindowShotDone,
            WindowShotError
        };
        wms_add_listener(ctx->wms, &wmsListener, ctx);
        wl_display_flush(ctx->display);
        wl_display_roundtrip(ctx->display);
        return;
    }
}

static void *thread_display_dispatch(void* data)
{
    struct WMSContext* ctx = (struct WMSContext*)(data);
    while (true) {
        if (wl_display_dispatch(ctx->display) == -1) {
            LOG("wl_display_connect failed errno: ", errno);
            break;
        }
    }
    return nullptr;
}

int32_t CreateShmFile(int32_t size)
{
    int fd;
    char tempPath[] = "/data/tempfile-XXXXXX";

    fd = mkstemp(tempPath);

    if (fd < 0) {
        return -1;
    }

    unlink(tempPath);

    if (ftruncate(fd, size) < 0) {
        close(fd);
        return -1;
    }

    return fd;
}

struct wl_buffer* CreateShmBuffer(struct wl_shm *wlShm, int32_t width, int32_t height)
{
    int32_t stride = width * 4;
    int32_t size = stride * height;
    int32_t fd = CreateShmFile(size);
    if (fd < 0) {
        LOG("CreateShmFile failed(fd)");
        return nullptr;
    }

    void *shm_data = mmap(nullptr, size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    if (shm_data == nullptr) {
        LOG("mmap failed(mmap)");
        close(fd);
        return nullptr;
    }

    auto pool = wl_shm_create_pool(wlShm, fd, size);
    wl_buffer *buffer = nullptr;
    if (pool) {
        buffer = wl_shm_pool_create_buffer(pool, 0, width, height, stride, WL_SHM_FORMAT_XRGB8888);
        wl_shm_pool_destroy(pool);
    }

    if (buffer == nullptr) {
        LOG("wl_shm_pool_create_buffer failed(wl_buffer)");
    }

    close(fd);
    return buffer;
}

} // namespace

void WMServerTest::SetUp()
{
}

void WMServerTest::TearDown()
{
}

void WMServerTest::SetUpTestCase()
{
    ctx.display = wl_display_connect(NULL);
    ASSERT_NE(ctx.display, nullptr);

    struct wl_registry *registry = wl_display_get_registry(ctx.display);
    ASSERT_NE(registry, nullptr);

    static struct wl_registry_listener registryListener = {
        RegistryGlobal
    };

    wl_registry_add_listener(registry, &registryListener, &ctx);

    wl_display_roundtrip(ctx.display);
    wl_display_roundtrip(ctx.display);

    pthread_create(&tid, NULL, thread_display_dispatch, &ctx);

}

void WMServerTest::TearDownTestCase()
{
}

namespace {

/*
 * Feature: wms_create_window by normal arguments
 * Function: WMSServer
 * SubFunction: wms_create_window
 * FunctionPoints: wms_create_window(WMS_WINDOW_TYPE_NORMAL)
 * EnvConditions: display, compositor, wms init success.
 * CaseDescription: 1. Create WlSurface
 *                  2. Create Window by normal arguments
 *                  3. check it
 */
HWTEST_F(WMServerTest, CreateWindow001, testing::ext::TestSize.Level0)
{
    // WMSServer init success.
    ASSERT_NE(ctx.display, nullptr) << "EnvConditions: wl_display init success..";
    ASSERT_NE(ctx.compositor, nullptr) << "EnvConditions: wl_compositor init success..";
    ASSERT_NE(ctx.wms, nullptr) << "EnvConditions: wms init success..";

    // 1. create WlSurface
    struct wl_surface* wlSurface = wl_compositor_create_surface(ctx.compositor);
    ASSERT_NE(wlSurface, nullptr) << "CaseDescription: 1. create WlSurface (wlSurface != nullptr)";

    // 2. Create a Window (WMS_WINDOW_TYPE_NORMAL)
    {
        std::unique_lock<std::mutex> lck(syncMutex);
        replyFlag = false;
        wms_create_window(ctx.wms, wlSurface, 0, WMS_WINDOW_TYPE_NORMAL);
        wl_display_flush(ctx.display);
        syncVariable.wait(lck, [&](){ return replyFlag; });
    }

    // 3. check it
    ASSERT_EQ(windowStatus.status, WMS_WINDOW_STATUS_CREATED) 
        << "CaseDescription: 3. check it (status == WMS_WINDOW_STATUS_CREATED)";
    ASSERT_GT(windowStatus.wid, 0) << "CaseDescription: 3. check it (wid > 0)";
    ASSERT_GE(windowStatus.x, 0) << "CaseDescription: 3. check it (x >= 0)";
    ASSERT_GE(windowStatus.y, 0) << "CaseDescription: 3. check it (y >= 0)";
    ASSERT_GT(windowStatus.width, 0u) << "CaseDescription: 3. check it (width > 0)";
    ASSERT_GT(windowStatus.height, 0u) << "CaseDescription: 3. check it (height > 0)";

}

/*
 * Feature: wms_create_window by normal arguments
 * Function: WMSServer
 * SubFunction: wms_create_window
 * FunctionPoints: wms_create_window(WMS_WINDOW_TYPE_STATUS_BAR)
 * EnvConditions: display, compositor, wms init success.
 * CaseDescription: 1. Create WlSurface
 *                  2. Create Window by normal arguments
 *                  3. check it
 */
HWTEST_F(WMServerTest, CreateWindow002, testing::ext::TestSize.Level0)
{
    // WMSServer init success.
    ASSERT_NE(ctx.display, nullptr) << "EnvConditions: wl_display init success..";
    ASSERT_NE(ctx.compositor, nullptr) << "EnvConditions: wl_compositor init success..";
    ASSERT_NE(ctx.wms, nullptr) << "EnvConditions: wms init success..";

    // 1. create WlSurface
    struct wl_surface* wlSurface = wl_compositor_create_surface(ctx.compositor);
    ASSERT_NE(wlSurface, nullptr) << "CaseDescription: 1. create WlSurface (wlSurface != nullptr)";

    // 2. Create a Window (WMS_WINDOW_TYPE_STATUS_BAR)
    {
        std::unique_lock<std::mutex> lck(syncMutex);
        replyFlag = false;
        wms_create_window(ctx.wms, wlSurface, 0, WMS_WINDOW_TYPE_STATUS_BAR);
        wl_display_flush(ctx.display);
        syncVariable.wait(lck, [&](){ return replyFlag; });
    }

    // 3. check it
    ASSERT_EQ(windowStatus.status, WMS_WINDOW_STATUS_CREATED) 
        << "CaseDescription: 3. check it (status == WMS_WINDOW_STATUS_CREATED)";
    ASSERT_GT(windowStatus.wid, 0) << "CaseDescription: 3. check it (wid > 0)";
    ASSERT_GE(windowStatus.x, 0) << "CaseDescription: 3. check it (x >= 0)";
    ASSERT_GE(windowStatus.y, 0) << "CaseDescription: 3. check it (y >= 0)";
    ASSERT_GT(windowStatus.width, 0u) << "CaseDescription: 3. check it (width > 0)";
    ASSERT_GT(windowStatus.height, 0u) << "CaseDescription: 3. check it (height > 0)";

}

/*
 * Feature: wms_create_window by normal arguments
 * Function: WMSServer
 * SubFunction: wms_create_window
 * FunctionPoints: wms_create_window(WMS_WINDOW_TYPE_NAVI_BAR)
 * EnvConditions: display, compositor, wms init success.
 * CaseDescription: 1. Create WlSurface
 *                  2. Create Window by normal arguments
 *                  3. check it
 */
HWTEST_F(WMServerTest, CreateWindow003, testing::ext::TestSize.Level0)
{
    // WMSServer init success.
    ASSERT_NE(ctx.display, nullptr) << "EnvConditions: wl_display init success..";
    ASSERT_NE(ctx.compositor, nullptr) << "EnvConditions: wl_compositor init success..";
    ASSERT_NE(ctx.wms, nullptr) << "EnvConditions: wms init success..";

    // 1. create WlSurface
    struct wl_surface* wlSurface = wl_compositor_create_surface(ctx.compositor);
    ASSERT_NE(wlSurface, nullptr) << "CaseDescription: 1. create WlSurface (wlSurface != nullptr)";

    // 2. Create a Window (WMS_WINDOW_TYPE_NAVI_BAR)
    {
        std::unique_lock<std::mutex> lck(syncMutex);
        replyFlag = false;
        wms_create_window(ctx.wms, wlSurface, 0, WMS_WINDOW_TYPE_NAVI_BAR);
        wl_display_flush(ctx.display);
        syncVariable.wait(lck, [&](){ return replyFlag; });
    }

    // 3. check it
    ASSERT_EQ(windowStatus.status, WMS_WINDOW_STATUS_CREATED) 
        << "CaseDescription: 3. check it (status == WMS_WINDOW_STATUS_CREATED)";
    ASSERT_GT(windowStatus.wid, 0) << "CaseDescription: 3. check it (wid > 0)";
    ASSERT_GE(windowStatus.x, 0) << "CaseDescription: 3. check it (x >= 0)";
    ASSERT_GE(windowStatus.y, 0) << "CaseDescription: 3. check it (y >= 0)";
    ASSERT_GT(windowStatus.width, 0u) << "CaseDescription: 3. check it (width > 0)";
    ASSERT_GT(windowStatus.height, 0u) << "CaseDescription: 3. check it (height > 0)";

}

/*
 * Feature: wms_create_window by normal arguments
 * Function: WMSServer
 * SubFunction: wms_create_window
 * FunctionPoints: wms_create_window(WMS_WINDOW_TYPE_ALARM)
 * EnvConditions: display, compositor, wms init success.
 * CaseDescription: 1. Create WlSurface
 *                  2. Create Window by normal arguments
 *                  3. check it
 */
HWTEST_F(WMServerTest, CreateWindow004, testing::ext::TestSize.Level0)
{
    // WMSServer init success.
    ASSERT_NE(ctx.display, nullptr) << "EnvConditions: wl_display init success..";
    ASSERT_NE(ctx.compositor, nullptr) << "EnvConditions: wl_compositor init success..";
    ASSERT_NE(ctx.wms, nullptr) << "EnvConditions: wms init success..";

    // 1. create WlSurface
    struct wl_surface* wlSurface = wl_compositor_create_surface(ctx.compositor);
    ASSERT_NE(wlSurface, nullptr) << "CaseDescription: 1. create WlSurface (wlSurface != nullptr)";

    // 2. Create a Window (WMS_WINDOW_TYPE_ALARM)
    {
        std::unique_lock<std::mutex> lck(syncMutex);
        replyFlag = false;
        wms_create_window(ctx.wms, wlSurface, 0, WMS_WINDOW_TYPE_ALARM);
        wl_display_flush(ctx.display);
        syncVariable.wait(lck, [&](){ return replyFlag; });
    }

    // 3. check it
    ASSERT_EQ(windowStatus.status, WMS_WINDOW_STATUS_CREATED) 
        << "CaseDescription: 3. check it (status == WMS_WINDOW_STATUS_CREATED)";
    ASSERT_GT(windowStatus.wid, 0) << "CaseDescription: 3. check it (wid > 0)";
    ASSERT_GE(windowStatus.x, 0) << "CaseDescription: 3. check it (x >= 0)";
    ASSERT_GE(windowStatus.y, 0) << "CaseDescription: 3. check it (y >= 0)";
    ASSERT_GT(windowStatus.width, 0u) << "CaseDescription: 3. check it (width > 0)";
    ASSERT_GT(windowStatus.height, 0u) << "CaseDescription: 3. check it (height > 0)";

}

/*
 * Feature: wms_create_window by abnormal arguments
 * Function: WMSServer
 * SubFunction: wms_create_window
 * FunctionPoints: wms_create_window abnormal arguments
 * EnvConditions: display, compositor, wms init success.
 * CaseDescription: 1. Create WlSurface
 *                  2. Create Window by abnormal arguments
 *                  3. check it
 */
HWTEST_F(WMServerTest, CreateWindow005, testing::ext::TestSize.Level0)
{
    // WMSServer init success.
    ASSERT_NE(ctx.display, nullptr) << "EnvConditions: wl_display init success..";
    ASSERT_NE(ctx.compositor, nullptr) << "EnvConditions: wl_compositor init success..";
    ASSERT_NE(ctx.wms, nullptr) << "EnvConditions: wms init success..";

    // 1. create WlSurface
    struct wl_surface* wlSurface = wl_compositor_create_surface(ctx.compositor);
    ASSERT_NE(wlSurface, nullptr) << "CaseDescription: 1. create WlSurface (wlSurface != nullptr)";

    // 2. Create Window by abnormal arguments
    {
        std::unique_lock<std::mutex> lck(syncMutex);
        replyFlag = false;
        wms_create_window(ctx.wms, wlSurface, -1, WMS_WINDOW_TYPE_NORMAL);
        wl_display_flush(ctx.display);
        syncVariable.wait(lck, [&](){ return replyFlag; });
    }

    // 3. check it
    ASSERT_EQ(windowStatus.status, WMS_WINDOW_STATUS_FAILED) 
        << "CaseDescription: 3. check it (status == WMS_WINDOW_STATUS_FAILED)";
    ASSERT_EQ(windowStatus.wid, 0) << "CaseDescription: 3. check it (wid = 0)";
    ASSERT_EQ(windowStatus.x, 0) << "CaseDescription: 3. check it (x = 0)";
    ASSERT_EQ(windowStatus.y, 0) << "CaseDescription: 3. check it (y = 0)";
    ASSERT_EQ(windowStatus.width, 0u) << "CaseDescription: 3. check it (width = 0)";
    ASSERT_EQ(windowStatus.height, 0u) << "CaseDescription: 3. check it (height = 0)";
}

/*
 * Feature: wms_create_window by abnormal arguments
 * Function: WMSServer
 * SubFunction: wms_create_window
 * FunctionPoints: wms_create_window abnormal arguments
 * EnvConditions: display, compositor, wms init success.
 * CaseDescription: 1. Create WlSurface
 *                  2. Create Window by abnormal arguments
 *                  3. check it
 */
HWTEST_F(WMServerTest, CreateWindow006, testing::ext::TestSize.Level0)
{
    // WMSServer init success.
    ASSERT_NE(ctx.display, nullptr) << "EnvConditions: wl_display init success..";
    ASSERT_NE(ctx.compositor, nullptr) << "EnvConditions: wl_compositor init success..";
    ASSERT_NE(ctx.wms, nullptr) << "EnvConditions: wms init success..";

    // 1. create WlSurface
    struct wl_surface* wlSurface = wl_compositor_create_surface(ctx.compositor);
    ASSERT_NE(wlSurface, nullptr) << "CaseDescription: 1. create WlSurface (wlSurface != nullptr)";

    // 2. Create Window by abnormal arguments
    {
        std::unique_lock<std::mutex> lck(syncMutex);
        replyFlag = false;
        wms_create_window(ctx.wms, wlSurface, 0, WMS_WINDOW_TYPE_MAX_COUNT);
        wl_display_flush(ctx.display);
        syncVariable.wait(lck, [&](){ return replyFlag; });
    }

    // 3. check it
    ASSERT_EQ(windowStatus.status, WMS_WINDOW_STATUS_FAILED) 
        << "CaseDescription: 3. check it (status == WMS_WINDOW_STATUS_FAILED)";
    ASSERT_EQ(windowStatus.wid, 0) << "CaseDescription: 3. check it (wid = 0)";
    ASSERT_EQ(windowStatus.x, 0) << "CaseDescription: 3. check it (x = 0)";
    ASSERT_EQ(windowStatus.y, 0) << "CaseDescription: 3. check it (y = 0)";
    ASSERT_EQ(windowStatus.width, 0u) << "CaseDescription: 3. check it (width = 0)";
    ASSERT_EQ(windowStatus.height, 0u) << "CaseDescription: 3. check it (height = 0)";
}

/*
 * Feature: wms_create_window by abnormal arguments
 * Function: WMSServer
 * SubFunction: wms_create_window
 * FunctionPoints: wms_create_window abnormal arguments
 * EnvConditions: display, compositor, wms init success.
 * CaseDescription: 1. Create WlSurface
 *                  2. Create Window by normal arguments
 *                  3. check result
 *                  4. Create Window by abnormal arguments(use WlSurface again)
 *                  5. check result
 */
HWTEST_F(WMServerTest, CreateWindow007, testing::ext::TestSize.Level0)
{
    // WMSServer init success.
    ASSERT_NE(ctx.display, nullptr) << "EnvConditions: wl_display init success..";
    ASSERT_NE(ctx.compositor, nullptr) << "EnvConditions: wl_compositor init success..";
    ASSERT_NE(ctx.wms, nullptr) << "EnvConditions: wms init success..";

    // 1. create WlSurface
    struct wl_surface* wlSurface = wl_compositor_create_surface(ctx.compositor);
    ASSERT_NE(wlSurface, nullptr) << "CaseDescription: 1. create WlSurface (wlSurface != nullptr)";

    // 2. Create Window by normal arguments
    {
        std::unique_lock<std::mutex> lck(syncMutex);
        replyFlag = false;
        wms_create_window(ctx.wms, wlSurface, 0, WMS_WINDOW_TYPE_NORMAL);
        wl_display_flush(ctx.display);
        syncVariable.wait(lck, [&](){ return replyFlag; });
    }

    // 3. check it
    ASSERT_EQ(windowStatus.status, WMS_WINDOW_STATUS_CREATED) 
        << "CaseDescription: 3. check it (status == WMS_WINDOW_STATUS_CREATED)";
    ASSERT_GT(windowStatus.wid, 0) << "CaseDescription: 3. check it (wid > 0)";
    ASSERT_GE(windowStatus.x, 0) << "CaseDescription: 3. check it (x >= 0)";
    ASSERT_GE(windowStatus.y, 0) << "CaseDescription: 3. check it (y >= 0)";
    ASSERT_GT(windowStatus.width, 0u) << "CaseDescription: 3. check it (width > 0)";
    ASSERT_GT(windowStatus.height, 0u) << "CaseDescription: 3. check it (height > 0)";

    // 4. Create Window by abnormal arguments(wlSurface is already used.)
    {
        std::unique_lock<std::mutex> lck(syncMutex);
        replyFlag = false;
        wms_create_window(ctx.wms, wlSurface, 0, WMS_WINDOW_TYPE_NORMAL);
        wl_display_flush(ctx.display);
        syncVariable.wait(lck, [&](){ return replyFlag; });
    }

    // 5. check it
    ASSERT_EQ(windowStatus.status, WMS_WINDOW_STATUS_FAILED) 
        << "CaseDescription: 5. check it (status == WMS_WINDOW_STATUS_FAILED)";
    ASSERT_EQ(windowStatus.wid, 0) << "CaseDescription: 5. check it (wid = 0)";
    ASSERT_EQ(windowStatus.x, 0) << "CaseDescription: 5. check it (x = 0)";
    ASSERT_EQ(windowStatus.y, 0) << "CaseDescription: 5. check it (y = 0)";
    ASSERT_EQ(windowStatus.width, 0u) << "CaseDescription: 5. check it (width = 0)";
    ASSERT_EQ(windowStatus.height, 0u) << "CaseDescription: 5. check it (height = 0)";
}

/*
 * Feature: wms_destroy_window by normal arguments
 * Function: WMSServer
 * SubFunction: wms_destroy_window
 * FunctionPoints: wms_destroy_window normal arguments
 * EnvConditions: display, compositor, wms init success.
 * CaseDescription: 1. Create WlSurface
 *                  2. Create Window by normal arguments
 *                  3. check result
 *                  4. Destroy Window by normal arguments
 *                  5. check result
 */
HWTEST_F(WMServerTest, DestroyWindow001, testing::ext::TestSize.Level0)
{
    // WMSServer init success.
    ASSERT_NE(ctx.display, nullptr) << "EnvConditions: wl_display init success..";
    ASSERT_NE(ctx.compositor, nullptr) << "EnvConditions: wl_compositor init success..";
    ASSERT_NE(ctx.wms, nullptr) << "EnvConditions: wms init success..";

    // 1. create WlSurface
    struct wl_surface* wlSurface = wl_compositor_create_surface(ctx.compositor);
    ASSERT_NE(wlSurface, nullptr) << "CaseDescription: 1. create WlSurface (wlSurface != nullptr)";

    // 2. Create Window by normal arguments
    {
        std::unique_lock<std::mutex> lck(syncMutex);
        replyFlag = false;
        wms_create_window(ctx.wms, wlSurface, 0, WMS_WINDOW_TYPE_NORMAL);
        wl_display_flush(ctx.display);
        syncVariable.wait(lck, [&](){ return replyFlag; });
    }

    // 3. check it
    ASSERT_EQ(windowStatus.status, WMS_WINDOW_STATUS_CREATED) 
        << "CaseDescription: 3. check it (status == WMS_WINDOW_STATUS_CREATED)";
    ASSERT_GT(windowStatus.wid, 0) << "CaseDescription: 3. check it (wid > 0)";
    ASSERT_GE(windowStatus.x, 0) << "CaseDescription: 3. check it (x >= 0)";
    ASSERT_GE(windowStatus.y, 0) << "CaseDescription: 3. check it (y >= 0)";
    ASSERT_GT(windowStatus.width, 0u) << "CaseDescription: 3. check it (width > 0)";
    ASSERT_GT(windowStatus.height, 0u) << "CaseDescription: 3. check it (height > 0)";

    // 4. Destroy Window by normal arguments
    {
        std::unique_lock<std::mutex> lck(syncMutex);
        replyFlag = false;
        wms_destroy_window(ctx.wms, windowStatus.wid);
        wl_display_flush(ctx.display);
        syncVariable.wait(lck, [&](){ return replyFlag; });
    }

    // 5. check it
    ASSERT_EQ(replyStatus, WMS_ERROR_OK) 
        << "CaseDescription: 5. check it (replyStatus == WMS_ERROR_OK)";
    ASSERT_EQ(windowStatus.status, WMS_WINDOW_STATUS_DESTROYED) 
        << "CaseDescription: 5. check it (status == WMS_WINDOW_STATUS_DESTROYED)";
    ASSERT_GT(windowStatus.wid, 0) << "CaseDescription: 5. check it (wid > 0)";
}

/*
 * Feature: wms_destroy_window passive trigger
 * Function: WMSServer
 * SubFunction: wms_destroy_window
 * FunctionPoints: wms_destroy_window passive trigger
 * EnvConditions: display, compositor, wms init success.
 * CaseDescription: 1. Create WlSurface
 *                  2. Create Window by normal arguments
 *                  3. check result
 *                  4. Destroy WlSurface(passive trigger)
 *                  5. check result
 */
HWTEST_F(WMServerTest, DestroyWindow002, testing::ext::TestSize.Level0)
{
    // WMSServer init success.
    ASSERT_NE(ctx.display, nullptr) << "EnvConditions: wl_display init success..";
    ASSERT_NE(ctx.compositor, nullptr) << "EnvConditions: wl_compositor init success..";
    ASSERT_NE(ctx.wms, nullptr) << "EnvConditions: wms init success..";

    // 1. create WlSurface
    struct wl_surface* wlSurface = wl_compositor_create_surface(ctx.compositor);
    ASSERT_NE(wlSurface, nullptr) << "CaseDescription: 1. create WlSurface (wlSurface != nullptr)";

    // 2. Create Window by normal arguments
    {
        std::unique_lock<std::mutex> lck(syncMutex);
        replyFlag = false;
        wms_create_window(ctx.wms, wlSurface, 0, WMS_WINDOW_TYPE_NORMAL);
        wl_display_flush(ctx.display);
        syncVariable.wait(lck, [&](){ return replyFlag; });
    }

    // 3. check it
    ASSERT_EQ(windowStatus.status, WMS_WINDOW_STATUS_CREATED) 
        << "CaseDescription: 3. check it (status == WMS_WINDOW_STATUS_CREATED)";
    ASSERT_GT(windowStatus.wid, 0) << "CaseDescription: 3. check it (wid > 0)";
    ASSERT_GE(windowStatus.x, 0) << "CaseDescription: 3. check it (x >= 0)";
    ASSERT_GE(windowStatus.y, 0) << "CaseDescription: 3. check it (y >= 0)";
    ASSERT_GT(windowStatus.width, 0u) << "CaseDescription: 3. check it (width > 0)";
    ASSERT_GT(windowStatus.height, 0u) << "CaseDescription: 3. check it (height > 0)";

    // 4. Destroy wlSurface
    {
        std::unique_lock<std::mutex> lck(destroyMutex);
        destroyReplyFlag = false;
        wl_surface_destroy(wlSurface);
        wl_display_flush(ctx.display);
        destroyVariable.wait(lck, [&](){ return destroyReplyFlag; });
    }

    // 5. check it
    ASSERT_EQ(windowStatus.status, WMS_WINDOW_STATUS_DESTROYED) 
        << "CaseDescription: 5. check it (status == WMS_WINDOW_STATUS_DESTROYED)";
    ASSERT_GT(windowStatus.wid, 0) << "CaseDescription: 5. check it (wid > 0)";
}

/*
 * Feature: wms_destroy_window by abnormal arguments
 * Function: WMSServer
 * SubFunction: wms_destroy_window
 * FunctionPoints: wms_destroy_window abnormal arguments
 * EnvConditions: display, compositor, wms init success.
 * CaseDescription: 1. Destroy Window by abnormal arguments
 *                  2. check result
 */
HWTEST_F(WMServerTest, DestroyWindow003, testing::ext::TestSize.Level0)
{
    // WMSServer init success.
    ASSERT_NE(ctx.display, nullptr) << "EnvConditions: wl_display init success..";
    ASSERT_NE(ctx.compositor, nullptr) << "EnvConditions: wl_compositor init success..";
    ASSERT_NE(ctx.wms, nullptr) << "EnvConditions: wms init success..";

    // 1. Create Window by normal arguments
    {
        std::unique_lock<std::mutex> lck(syncMutex);
        replyFlag = false;
        wms_destroy_window(ctx.wms, 0);
        wl_display_flush(ctx.display);
        syncVariable.wait(lck, [&](){ return replyFlag; });
    }

    // 2. check it
    ASSERT_NE(replyStatus, WMS_ERROR_OK) 
        << "CaseDescription: 2. check it (replyStatus != WMS_ERROR_OK)";
}

/*
 * Feature: wms_set_window_top by normal arguments
 * Function: WMSServer
 * SubFunction: wms_set_window_top
 * FunctionPoints: wms_set_window_top normal arguments
 * EnvConditions: display, compositor, wms init success.
 * CaseDescription: 1. Create WlSurface
 *                  2. Create Window by normal arguments
 *                  3. check result
 *                  4. SetWindowTop by normal arguments
 *                  5. check result
 */ 
HWTEST_F(WMServerTest, SetWindowTop001, testing::ext::TestSize.Level0)
{
    // WMSServer init success.
    ASSERT_NE(ctx.display, nullptr) << "EnvConditions: wl_display init success..";
    ASSERT_NE(ctx.compositor, nullptr) << "EnvConditions: wl_compositor init success..";
    ASSERT_NE(ctx.wms, nullptr) << "EnvConditions: wms init success..";

    // 1. create WlSurface
    struct wl_surface* wlSurface = wl_compositor_create_surface(ctx.compositor);
    ASSERT_NE(wlSurface, nullptr) << "CaseDescription: 1. create WlSurface (wlSurface != nullptr)";

    // 2. Create Window by normal arguments
    {
        std::unique_lock<std::mutex> lck(syncMutex);
        replyFlag = false;
        wms_create_window(ctx.wms, wlSurface, 0, WMS_WINDOW_TYPE_NORMAL);
        wl_display_flush(ctx.display);
        syncVariable.wait(lck, [&](){ return replyFlag; });
    }

    // 3. check it
    ASSERT_EQ(windowStatus.status, WMS_WINDOW_STATUS_CREATED) 
        << "CaseDescription: 3. check it (status == WMS_WINDOW_STATUS_CREATED)";
    ASSERT_GT(windowStatus.wid, 0) << "CaseDescription: 3. check it (wid > 0)";
    ASSERT_GE(windowStatus.x, 0) << "CaseDescription: 3. check it (x >= 0)";
    ASSERT_GE(windowStatus.y, 0) << "CaseDescription: 3. check it (y >= 0)";
    ASSERT_GT(windowStatus.width, 0u) << "CaseDescription: 3. check it (width > 0)";
    ASSERT_GT(windowStatus.height, 0u) << "CaseDescription: 3. check it (height > 0)";

    // 4. SetWindowTop by normal arguments
    {
        std::unique_lock<std::mutex> lck(syncMutex);
        replyFlag = false;
        wms_set_window_top(ctx.wms, windowStatus.wid);
        wms_commit_changes(ctx.wms);
        wl_display_flush(ctx.display);
        syncVariable.wait(lck, [&](){ return replyFlag; });
    }

    // 5. check it
    ASSERT_EQ(replyStatus, WMS_ERROR_OK) 
        << "CaseDescription: 5. check it (replyStatus == WMS_ERROR_OK)";
}

/*
 * Feature: wms_set_window_top by abnormal arguments
 * Function: WMSServer
 * SubFunction: wms_set_window_top
 * FunctionPoints: wms_set_window_top abnormal arguments
 * EnvConditions: display, compositor, wms init success.
 * CaseDescription: 1. SetWindowTop by abnormal arguments
 *                  2. check result
 */ 
HWTEST_F(WMServerTest, SetWindowTop002, testing::ext::TestSize.Level0)
{
    // WMSServer init success.
    ASSERT_NE(ctx.display, nullptr) << "EnvConditions: wl_display init success..";
    ASSERT_NE(ctx.compositor, nullptr) << "EnvConditions: wl_compositor init success..";
    ASSERT_NE(ctx.wms, nullptr) << "EnvConditions: wms init success..";

    // 1. SetWindowTop by abnormal arguments
    {
        std::unique_lock<std::mutex> lck(syncMutex);
        replyFlag = false;
        wms_set_window_top(ctx.wms, 0);
        wms_commit_changes(ctx.wms);
        wl_display_flush(ctx.display);
        syncVariable.wait(lck, [&](){ return replyFlag; });
    }

    // 5. check it
    ASSERT_NE(replyStatus, WMS_ERROR_OK) 
        << "CaseDescription: 5. check it (replyStatus != WMS_ERROR_OK)";
}

/*
 * Feature: wms_set_window_size by normal arguments
 * Function: WMSServer
 * SubFunction: wms_set_window_size
 * FunctionPoints: wms_set_window_size normal arguments
 * EnvConditions: display, compositor, wms init success.
 * CaseDescription: 1. Create WlSurface
 *                  2. Create Window by normal arguments
 *                  3. check result
 *                  4. SetWindowSize by normal arguments
 *                  5. check result
 */ 
HWTEST_F(WMServerTest, SetWindowSize001, testing::ext::TestSize.Level0)
{
    // WMSServer init success.
    ASSERT_NE(ctx.display, nullptr) << "EnvConditions: wl_display init success..";
    ASSERT_NE(ctx.compositor, nullptr) << "EnvConditions: wl_compositor init success..";
    ASSERT_NE(ctx.wms, nullptr) << "EnvConditions: wms init success..";

    // 1. create WlSurface
    struct wl_surface* wlSurface = wl_compositor_create_surface(ctx.compositor);
    ASSERT_NE(wlSurface, nullptr) << "CaseDescription: 1. create WlSurface (wlSurface != nullptr)";

    // 2. Create Window by normal arguments
    {
        std::unique_lock<std::mutex> lck(syncMutex);
        replyFlag = false;
        wms_create_window(ctx.wms, wlSurface, 0, WMS_WINDOW_TYPE_NORMAL);
        wl_display_flush(ctx.display);
        syncVariable.wait(lck, [&](){ return replyFlag; });
    }

    // 3. check it
    ASSERT_EQ(windowStatus.status, WMS_WINDOW_STATUS_CREATED) 
        << "CaseDescription: 3. check it (status == WMS_WINDOW_STATUS_CREATED)";
    ASSERT_GT(windowStatus.wid, 0) << "CaseDescription: 3. check it (wid > 0)";
    ASSERT_GE(windowStatus.x, 0) << "CaseDescription: 3. check it (x >= 0)";
    ASSERT_GE(windowStatus.y, 0) << "CaseDescription: 3. check it (y >= 0)";
    ASSERT_GT(windowStatus.width, 0u) << "CaseDescription: 3. check it (width > 0)";
    ASSERT_GT(windowStatus.height, 0u) << "CaseDescription: 3. check it (height > 0)";

    // 4. SetWindowSize by normal arguments
    {
        std::unique_lock<std::mutex> lck(syncMutex);
        replyFlag = false;
        wms_set_window_size(ctx.wms, windowStatus.wid, 100, 100);
        wms_commit_changes(ctx.wms);
        wl_display_flush(ctx.display);
        syncVariable.wait(lck, [&](){ return replyFlag; });
    }

    // 5. check it
    ASSERT_EQ(replyStatus, WMS_ERROR_OK) 
        << "CaseDescription: 5. check it (replyStatus == WMS_ERROR_OK)";
}

/*
 * Feature: wms_set_window_size by abnormal arguments
 * Function: WMSServer
 * SubFunction: wms_set_window_size
 * FunctionPoints: wms_set_window_size abnormal arguments
 * EnvConditions: display, compositor, wms init success.
 * CaseDescription: 1. SetWindowSize by abnormal arguments
 *                  2. check result
 */ 
HWTEST_F(WMServerTest, SetWindowSize002, testing::ext::TestSize.Level0)
{
    // WMSServer init success.
    ASSERT_NE(ctx.display, nullptr) << "EnvConditions: wl_display init success..";
    ASSERT_NE(ctx.compositor, nullptr) << "EnvConditions: wl_compositor init success..";
    ASSERT_NE(ctx.wms, nullptr) << "EnvConditions: wms init success..";

    // 1. SetWindowSize by abnormal arguments
    {
        std::unique_lock<std::mutex> lck(syncMutex);
        replyFlag = false;
        wms_set_window_size(ctx.wms, 0, 100, 100);
        wms_commit_changes(ctx.wms);
        wl_display_flush(ctx.display);
        syncVariable.wait(lck, [&](){ return replyFlag; });
    }

    // 5. check it
    ASSERT_NE(replyStatus, WMS_ERROR_OK) 
        << "CaseDescription: 5. check it (replyStatus != WMS_ERROR_OK)";
}

/*
 * Feature: wms_set_window_scale by normal arguments
 * Function: WMSServer
 * SubFunction: wms_set_window_scale
 * FunctionPoints: wms_set_window_scale normal arguments
 * EnvConditions: display, compositor, wms init success.
 * CaseDescription: 1. Create WlSurface
 *                  2. Create Window by normal arguments
 *                  3. check result
 *                  4. SetWindowScale by normal arguments
 *                  5. check result
 */ 
HWTEST_F(WMServerTest, SetWindowScale001, testing::ext::TestSize.Level0)
{
    // WMSServer init success.
    ASSERT_NE(ctx.display, nullptr) << "EnvConditions: wl_display init success..";
    ASSERT_NE(ctx.compositor, nullptr) << "EnvConditions: wl_compositor init success..";
    ASSERT_NE(ctx.wms, nullptr) << "EnvConditions: wms init success..";

    // 1. create WlSurface
    struct wl_surface* wlSurface = wl_compositor_create_surface(ctx.compositor);
    ASSERT_NE(wlSurface, nullptr) << "CaseDescription: 1. create WlSurface (wlSurface != nullptr)";

    // 2. Create Window by normal arguments
    {
        std::unique_lock<std::mutex> lck(syncMutex);
        replyFlag = false;
        wms_create_window(ctx.wms, wlSurface, 0, WMS_WINDOW_TYPE_NORMAL);
        wl_display_flush(ctx.display);
        syncVariable.wait(lck, [&](){ return replyFlag; });
    }

    // 3. check it
    ASSERT_EQ(windowStatus.status, WMS_WINDOW_STATUS_CREATED) 
        << "CaseDescription: 3. check it (status == WMS_WINDOW_STATUS_CREATED)";
    ASSERT_GT(windowStatus.wid, 0) << "CaseDescription: 3. check it (wid > 0)";
    ASSERT_GE(windowStatus.x, 0) << "CaseDescription: 3. check it (x >= 0)";
    ASSERT_GE(windowStatus.y, 0) << "CaseDescription: 3. check it (y >= 0)";
    ASSERT_GT(windowStatus.width, 0u) << "CaseDescription: 3. check it (width > 0)";
    ASSERT_GT(windowStatus.height, 0u) << "CaseDescription: 3. check it (height > 0)";

    // 4. SetWindowScale by normal arguments
    {
        std::unique_lock<std::mutex> lck(syncMutex);
        replyFlag = false;
        wms_set_window_scale(ctx.wms, windowStatus.wid, 100, 100);
        wms_commit_changes(ctx.wms);
        wl_display_flush(ctx.display);
        syncVariable.wait(lck, [&](){ return replyFlag; });
    }

    // 5. check it
    ASSERT_EQ(replyStatus, WMS_ERROR_OK) 
        << "CaseDescription: 5. check it (replyStatus == WMS_ERROR_OK)";
}

/*
 * Feature: wms_set_window_scale by abnormal arguments
 * Function: WMSServer
 * SubFunction: wms_set_window_scale
 * FunctionPoints: wms_set_window_scale abnormal arguments
 * EnvConditions: display, compositor, wms init success.
 * CaseDescription: 1. SetWindowScale by abnormal arguments
 *                  2. check result
 */ 
HWTEST_F(WMServerTest, SetWindowScale002, testing::ext::TestSize.Level0)
{
    // WMSServer init success.
    ASSERT_NE(ctx.display, nullptr) << "EnvConditions: wl_display init success..";
    ASSERT_NE(ctx.compositor, nullptr) << "EnvConditions: wl_compositor init success..";
    ASSERT_NE(ctx.wms, nullptr) << "EnvConditions: wms init success..";

    // 1. SetWindowScale by abnormal arguments
    {
        std::unique_lock<std::mutex> lck(syncMutex);
        replyFlag = false;
        wms_set_window_scale(ctx.wms, 0, 100, 100);
        wms_commit_changes(ctx.wms);
        wl_display_flush(ctx.display);
        syncVariable.wait(lck, [&](){ return replyFlag; });
    }

    // 5. check it
    ASSERT_NE(replyStatus, WMS_ERROR_OK) 
        << "CaseDescription: 5. check it (replyStatus != WMS_ERROR_OK)";
}

/*
 * Feature: wms_set_window_position by normal arguments
 * Function: WMSServer
 * SubFunction: wms_set_window_position
 * FunctionPoints: wms_set_window_position normal arguments
 * EnvConditions: display, compositor, wms init success.
 * CaseDescription: 1. Create WlSurface
 *                  2. Create Window by normal arguments
 *                  3. check result
 *                  4. SetWindowPosition by normal arguments
 *                  5. check result
 */ 
HWTEST_F(WMServerTest, SetWindowPosition001, testing::ext::TestSize.Level0)
{
    // WMSServer init success.
    ASSERT_NE(ctx.display, nullptr) << "EnvConditions: wl_display init success..";
    ASSERT_NE(ctx.compositor, nullptr) << "EnvConditions: wl_compositor init success..";
    ASSERT_NE(ctx.wms, nullptr) << "EnvConditions: wms init success..";

    // 1. create WlSurface
    struct wl_surface* wlSurface = wl_compositor_create_surface(ctx.compositor);
    ASSERT_NE(wlSurface, nullptr) << "CaseDescription: 1. create WlSurface (wlSurface != nullptr)";

    // 2. Create Window by normal arguments
    {
        std::unique_lock<std::mutex> lck(syncMutex);
        replyFlag = false;
        wms_create_window(ctx.wms, wlSurface, 0, WMS_WINDOW_TYPE_NORMAL);
        wl_display_flush(ctx.display);
        syncVariable.wait(lck, [&](){ return replyFlag; });
    }

    // 3. check it
    ASSERT_EQ(windowStatus.status, WMS_WINDOW_STATUS_CREATED) 
        << "CaseDescription: 3. check it (status == WMS_WINDOW_STATUS_CREATED)";
    ASSERT_GT(windowStatus.wid, 0) << "CaseDescription: 3. check it (wid > 0)";
    ASSERT_GE(windowStatus.x, 0) << "CaseDescription: 3. check it (x >= 0)";
    ASSERT_GE(windowStatus.y, 0) << "CaseDescription: 3. check it (y >= 0)";
    ASSERT_GT(windowStatus.width, 0u) << "CaseDescription: 3. check it (width > 0)";
    ASSERT_GT(windowStatus.height, 0u) << "CaseDescription: 3. check it (height > 0)";

    // 4. SetWindowPosition by normal arguments
    {
        std::unique_lock<std::mutex> lck(syncMutex);
        replyFlag = false;
        wms_set_window_position(ctx.wms, windowStatus.wid, 100, 100);
        wms_commit_changes(ctx.wms);
        wl_display_flush(ctx.display);
        syncVariable.wait(lck, [&](){ return replyFlag; });
    }

    // 5. check it
    ASSERT_EQ(replyStatus, WMS_ERROR_OK) 
        << "CaseDescription: 5. check it (replyStatus == WMS_ERROR_OK)";
}

/*
 * Feature: wms_set_window_position by abnormal arguments
 * Function: WMSServer
 * SubFunction: wms_set_window_position
 * FunctionPoints: wms_set_window_position abnormal arguments
 * EnvConditions: display, compositor, wms init success.
 * CaseDescription: 1. SetWindowPosition by abnormal arguments
 *                  2. check result
 */ 
HWTEST_F(WMServerTest, SetWindowPosition002, testing::ext::TestSize.Level0)
{
    // WMSServer init success.
    ASSERT_NE(ctx.display, nullptr) << "EnvConditions: wl_display init success..";
    ASSERT_NE(ctx.compositor, nullptr) << "EnvConditions: wl_compositor init success..";
    ASSERT_NE(ctx.wms, nullptr) << "EnvConditions: wms init success..";

    // 1. SetWindowPosition by abnormal arguments
    {
        std::unique_lock<std::mutex> lck(syncMutex);
        replyFlag = false;
        wms_set_window_position(ctx.wms, 0, 100, 100);
        wms_commit_changes(ctx.wms);
        wl_display_flush(ctx.display);
        syncVariable.wait(lck, [&](){ return replyFlag; });
    }

    // 5. check it
    ASSERT_NE(replyStatus, WMS_ERROR_OK) 
        << "CaseDescription: 5. check it (replyStatus != WMS_ERROR_OK)";
}

/*
 * Feature: wms_set_window_visibility by normal arguments
 * Function: WMSServer
 * SubFunction: wms_set_window_visibility
 * FunctionPoints: wms_set_window_visibility normal arguments
 * EnvConditions: display, compositor, wms init success.
 * CaseDescription: 1. Create WlSurface
 *                  2. Create Window by normal arguments
 *                  3. check result
 *                  4. SetWindowVisibility by normal arguments
 *                  5. check result
 */ 
HWTEST_F(WMServerTest, SetWindowVisibility001, testing::ext::TestSize.Level0)
{
    // WMSServer init success.
    ASSERT_NE(ctx.display, nullptr) << "EnvConditions: wl_display init success..";
    ASSERT_NE(ctx.compositor, nullptr) << "EnvConditions: wl_compositor init success..";
    ASSERT_NE(ctx.wms, nullptr) << "EnvConditions: wms init success..";

    // 1. create WlSurface
    struct wl_surface* wlSurface = wl_compositor_create_surface(ctx.compositor);
    ASSERT_NE(wlSurface, nullptr) << "CaseDescription: 1. create WlSurface (wlSurface != nullptr)";

    // 2. Create Window by normal arguments
    {
        std::unique_lock<std::mutex> lck(syncMutex);
        replyFlag = false;
        wms_create_window(ctx.wms, wlSurface, 0, WMS_WINDOW_TYPE_NORMAL);
        wl_display_flush(ctx.display);
        syncVariable.wait(lck, [&](){ return replyFlag; });
    }

    // 3. check it
    ASSERT_EQ(windowStatus.status, WMS_WINDOW_STATUS_CREATED) 
        << "CaseDescription: 3. check it (status == WMS_WINDOW_STATUS_CREATED)";
    ASSERT_GT(windowStatus.wid, 0) << "CaseDescription: 3. check it (wid > 0)";
    ASSERT_GE(windowStatus.x, 0) << "CaseDescription: 3. check it (x >= 0)";
    ASSERT_GE(windowStatus.y, 0) << "CaseDescription: 3. check it (y >= 0)";
    ASSERT_GT(windowStatus.width, 0u) << "CaseDescription: 3. check it (width > 0)";
    ASSERT_GT(windowStatus.height, 0u) << "CaseDescription: 3. check it (height > 0)";

    // 4. SetWindowVisibility by normal arguments
    {
        std::unique_lock<std::mutex> lck(syncMutex);
        replyFlag = false;
        wms_set_window_visibility(ctx.wms, windowStatus.wid, false);
        wms_commit_changes(ctx.wms);
        wl_display_flush(ctx.display);
        syncVariable.wait(lck, [&](){ return replyFlag; });
    }

    // 5. check it
    ASSERT_EQ(replyStatus, WMS_ERROR_OK) 
        << "CaseDescription: 5. check it (replyStatus == WMS_ERROR_OK)";
}

/*
 * Feature: wms_set_window_visibility by abnormal arguments
 * Function: WMSServer
 * SubFunction: wms_set_window_visibility
 * FunctionPoints: wms_set_window_visibility abnormal arguments
 * EnvConditions: display, compositor, wms init success.
 * CaseDescription: 1. SetWindowVisibility by abnormal arguments
 *                  2. check result
 */ 
HWTEST_F(WMServerTest, SetWindowVisibility002, testing::ext::TestSize.Level0)
{
    // WMSServer init success.
    ASSERT_NE(ctx.display, nullptr) << "EnvConditions: wl_display init success..";
    ASSERT_NE(ctx.compositor, nullptr) << "EnvConditions: wl_compositor init success..";
    ASSERT_NE(ctx.wms, nullptr) << "EnvConditions: wms init success..";

    // 1. SetWindowVisibility by abnormal arguments
    {
        std::unique_lock<std::mutex> lck(syncMutex);
        replyFlag = false;
        wms_set_window_visibility(ctx.wms, 0, true);
        wms_commit_changes(ctx.wms);
        wl_display_flush(ctx.display);
        syncVariable.wait(lck, [&](){ return replyFlag; });
    }

    // 2. check it
    ASSERT_NE(replyStatus, WMS_ERROR_OK) 
        << "CaseDescription: 2. check it (replyStatus != WMS_ERROR_OK)";
}

/*
 * Feature: wms_set_window_type by normal arguments
 * Function: WMSServer
 * SubFunction: wms_set_window_type
 * FunctionPoints: wms_set_window_type normal arguments
 * EnvConditions: display, compositor, wms init success.
 * CaseDescription: 1. Create WlSurface
 *                  2. Create Window by normal arguments
 *                  3. check result
 *                  4. SetWindowType by normal arguments
 *                  5. check result
 *                  6. SetWindowType by normal arguments(type not change)
 *                  7. check result
 * 
 */ 
HWTEST_F(WMServerTest, SetWindowType001, testing::ext::TestSize.Level0)
{
    // WMSServer init success.
    ASSERT_NE(ctx.display, nullptr) << "EnvConditions: wl_display init success..";
    ASSERT_NE(ctx.compositor, nullptr) << "EnvConditions: wl_compositor init success..";
    ASSERT_NE(ctx.wms, nullptr) << "EnvConditions: wms init success..";

    // 1. create WlSurface
    struct wl_surface* wlSurface = wl_compositor_create_surface(ctx.compositor);
    ASSERT_NE(wlSurface, nullptr) << "CaseDescription: 1. create WlSurface (wlSurface != nullptr)";

    // 2. Create Window by normal arguments
    {
        std::unique_lock<std::mutex> lck(syncMutex);
        replyFlag = false;
        wms_create_window(ctx.wms, wlSurface, 0, WMS_WINDOW_TYPE_NORMAL);
        wl_display_flush(ctx.display);
        syncVariable.wait(lck, [&](){ return replyFlag; });
    }

    // 3. check it
    ASSERT_EQ(windowStatus.status, WMS_WINDOW_STATUS_CREATED) 
        << "CaseDescription: 3. check it (status == WMS_WINDOW_STATUS_CREATED)";
    ASSERT_GT(windowStatus.wid, 0) << "CaseDescription: 3. check it (wid > 0)";
    ASSERT_GE(windowStatus.x, 0) << "CaseDescription: 3. check it (x >= 0)";
    ASSERT_GE(windowStatus.y, 0) << "CaseDescription: 3. check it (y >= 0)";
    ASSERT_GT(windowStatus.width, 0u) << "CaseDescription: 3. check it (width > 0)";
    ASSERT_GT(windowStatus.height, 0u) << "CaseDescription: 3. check it (height > 0)";

    // 4. SetWindowType by normal arguments
    {
        std::unique_lock<std::mutex> lck(syncMutex);
        replyFlag = false;
        wms_set_window_type(ctx.wms, windowStatus.wid, WMS_WINDOW_TYPE_ALARM);
        wms_commit_changes(ctx.wms);
        wl_display_flush(ctx.display);
        syncVariable.wait(lck, [&](){ return replyFlag; });
    }

    // 5. check it
    ASSERT_EQ(replyStatus, WMS_ERROR_OK) 
        << "CaseDescription: 5. check it (replyStatus == WMS_ERROR_OK)";
    
    // 6. SetWindowType by normal arguments
    {
        std::unique_lock<std::mutex> lck(syncMutex);
        replyFlag = false;
        wms_set_window_type(ctx.wms, windowStatus.wid, WMS_WINDOW_TYPE_ALARM);
        wms_commit_changes(ctx.wms);
        wl_display_flush(ctx.display);
        syncVariable.wait(lck, [&](){ return replyFlag; });
    }

    // 7. check it
    ASSERT_NE(replyStatus, WMS_ERROR_OK) 
        << "CaseDescription: 7. check it (replyStatus != WMS_ERROR_OK)";
}

/*
 * Feature: wms_set_window_type by abnormal arguments
 * Function: WMSServer
 * SubFunction: wms_set_window_type
 * FunctionPoints: wms_set_window_type abnormal arguments
 * EnvConditions: display, compositor, wms init success.
 * CaseDescription: 1. Create WlSurface
 *                  2. Create Window by normal arguments
 *                  3. check result
 *                  4. SetWindowType by abnormal arguments
 *                  5. check result
 *                  6. SetWindowType by abnormal arguments
 *                  7. check result
 */ 
HWTEST_F(WMServerTest, SetWindowType002, testing::ext::TestSize.Level0)
{
    // WMSServer init success.
    ASSERT_NE(ctx.display, nullptr) << "EnvConditions: wl_display init success..";
    ASSERT_NE(ctx.compositor, nullptr) << "EnvConditions: wl_compositor init success..";
    ASSERT_NE(ctx.wms, nullptr) << "EnvConditions: wms init success..";

    // 1. create WlSurface
    struct wl_surface* wlSurface = wl_compositor_create_surface(ctx.compositor);
    ASSERT_NE(wlSurface, nullptr) << "CaseDescription: 1. create WlSurface (wlSurface != nullptr)";

    // 2. Create Window by normal arguments
    {
        std::unique_lock<std::mutex> lck(syncMutex);
        replyFlag = false;
        wms_create_window(ctx.wms, wlSurface, 0, WMS_WINDOW_TYPE_NORMAL);
        wl_display_flush(ctx.display);
        syncVariable.wait(lck, [&](){ return replyFlag; });
    }

    // 3. check it
    ASSERT_EQ(windowStatus.status, WMS_WINDOW_STATUS_CREATED) 
        << "CaseDescription: 3. check it (status == WMS_WINDOW_STATUS_CREATED)";
    ASSERT_GT(windowStatus.wid, 0) << "CaseDescription: 3. check it (wid > 0)";
    ASSERT_GE(windowStatus.x, 0) << "CaseDescription: 3. check it (x >= 0)";
    ASSERT_GE(windowStatus.y, 0) << "CaseDescription: 3. check it (y >= 0)";
    ASSERT_GT(windowStatus.width, 0u) << "CaseDescription: 3. check it (width > 0)";
    ASSERT_GT(windowStatus.height, 0u) << "CaseDescription: 3. check it (height > 0)";

    // 4. SetWindowType by abnormal arguments
    {
        std::unique_lock<std::mutex> lck(syncMutex);
        replyFlag = false;
        wms_set_window_type(ctx.wms, 0, WMS_WINDOW_TYPE_ALARM);
        wms_commit_changes(ctx.wms);
        wl_display_flush(ctx.display);
        syncVariable.wait(lck, [&](){ return replyFlag; });
    }

    // 5. check it
    ASSERT_NE(replyStatus, WMS_ERROR_OK) 
        << "CaseDescription: 5. check it (replyStatus != WMS_ERROR_OK)";
    
    // 6. SetWindowType by normal arguments
    {
        std::unique_lock<std::mutex> lck(syncMutex);
        replyFlag = false;
        wms_set_window_type(ctx.wms, windowStatus.wid, WMS_WINDOW_TYPE_MAX_COUNT);
        wms_commit_changes(ctx.wms);
        wl_display_flush(ctx.display);
        syncVariable.wait(lck, [&](){ return replyFlag; });
    }

    // 7. check it
    ASSERT_NE(replyStatus, WMS_ERROR_OK) 
        << "CaseDescription: 7. check it (replyStatus != WMS_ERROR_OK)";
}

/*
 * Feature: wms_set_display_mode by normal arguments
 * Function: WMSServer
 * SubFunction: wms_set_display_mode
 * FunctionPoints: wms_set_display_mode normal arguments
 * EnvConditions: display, compositor, wms init success.
 * CaseDescription: 1. SetDisplayMode by normal arguments
 *                  2. check result
 */ 
HWTEST_F(WMServerTest, SetDisplayMode001, testing::ext::TestSize.Level0)
{
    // WMSServer init success.
    ASSERT_NE(ctx.display, nullptr) << "EnvConditions: wl_display init success..";
    ASSERT_NE(ctx.compositor, nullptr) << "EnvConditions: wl_compositor init success..";
    ASSERT_NE(ctx.wms, nullptr) << "EnvConditions: wms init success..";

    // 1. SetDisplayMode by normal arguments
    {
        std::unique_lock<std::mutex> lck(syncMutex);
        replyFlag = false;
        wms_set_display_mode(ctx.wms, WMS_DISPLAY_MODE_SINGLE);
        wms_commit_changes(ctx.wms);
        wl_display_flush(ctx.display);
        syncVariable.wait(lck, [&](){ return replyFlag; });
    }

    // 2. check it
    ASSERT_NE(replyStatus, WMS_ERROR_OK) 
        << "CaseDescription: 2. check it (replyStatus != WMS_ERROR_OK)";
    
    // 1. SetDisplayMode by normal arguments
    {
        std::unique_lock<std::mutex> lck(syncMutex);
        replyFlag = false;
        wms_set_display_mode(ctx.wms, WMS_DISPLAY_MODE_CLONE);
        wms_commit_changes(ctx.wms);
        wl_display_flush(ctx.display);
        syncVariable.wait(lck, [&](){ return replyFlag; });
    }

    // 2. check it
    ASSERT_NE(replyStatus, WMS_ERROR_OK) 
        << "CaseDescription: 2. check it (replyStatus != WMS_ERROR_OK)";
    
    // 1. SetDisplayMode by normal arguments
    {
        std::unique_lock<std::mutex> lck(syncMutex);
        replyFlag = false;
        wms_set_display_mode(ctx.wms, WMS_DISPLAY_MODE_EXTEND);
        wms_commit_changes(ctx.wms);
        wl_display_flush(ctx.display);
        syncVariable.wait(lck, [&](){ return replyFlag; });
    }

    // 2. check it
    ASSERT_NE(replyStatus, WMS_ERROR_OK) 
        << "CaseDescription: 2. check it (replyStatus != WMS_ERROR_OK)";
    
    // 1. SetDisplayMode by normal arguments
    {
        std::unique_lock<std::mutex> lck(syncMutex);
        replyFlag = false;
        wms_set_display_mode(ctx.wms, WMS_DISPLAY_MODE_EXPAND);
        wms_commit_changes(ctx.wms);
        wl_display_flush(ctx.display);
        syncVariable.wait(lck, [&](){ return replyFlag; });
    }

    // 2. check it
    ASSERT_NE(replyStatus, WMS_ERROR_OK) 
        << "CaseDescription: 2. check it (replyStatus != WMS_ERROR_OK)";
}

/*
 * Feature: wms_set_display_mode by abnormal arguments
 * Function: WMSServer
 * SubFunction: wms_set_display_mode
 * FunctionPoints: wms_set_display_mode abnormal arguments
 * EnvConditions: display, compositor, wms init success.
 * CaseDescription: 1. SetDisplayMode by abnormal arguments
 *                  2. check result
 */ 
HWTEST_F(WMServerTest, SetDisplayMode002, testing::ext::TestSize.Level0)
{
    // WMSServer init success.
    ASSERT_NE(ctx.display, nullptr) << "EnvConditions: wl_display init success..";
    ASSERT_NE(ctx.compositor, nullptr) << "EnvConditions: wl_compositor init success..";
    ASSERT_NE(ctx.wms, nullptr) << "EnvConditions: wms init success..";

    // 1. SetDisplayMode by abnormal arguments
    {
        std::unique_lock<std::mutex> lck(syncMutex);
        replyFlag = false;
        wms_set_display_mode(ctx.wms, -1);
        wms_commit_changes(ctx.wms);
        wl_display_flush(ctx.display);
        syncVariable.wait(lck, [&](){ return replyFlag; });
    }

    // 2. check it
    ASSERT_NE(replyStatus, WMS_ERROR_OK) 
        << "CaseDescription: 2. check it (replyStatus != WMS_ERROR_OK)";
}

/*
 * Feature: wms_screenshot by normal arguments
 * Function: WMSServer
 * SubFunction: wms_screenshot
 * FunctionPoints: wms_screenshot normal arguments
 * EnvConditions: display, compositor, wms init success.
 * CaseDescription: 1. ScreenShot by normal arguments
 *                  2. check result
 * 
 */ 
HWTEST_F(WMServerTest, ScreenShot001, testing::ext::TestSize.Level0)
{
    // WMSServer init success.
    ASSERT_NE(ctx.display, nullptr) << "EnvConditions: wl_display init success..";
    ASSERT_NE(ctx.compositor, nullptr) << "EnvConditions: wl_compositor init success..";
    ASSERT_NE(ctx.wms, nullptr) << "EnvConditions: wms init success..";

    // 1. ScreenShot by normal arguments
    {
        std::unique_lock<std::mutex> lck(syncMutex);
        replyFlag = false;
        wms_screenshot(ctx.wms, 0);
        wms_commit_changes(ctx.wms);
        wl_display_flush(ctx.display);
        syncVariable.wait(lck, [&](){ return replyFlag; });
    }

    // 2. check it
    ASSERT_EQ(shotInfo.status, WMS_ERROR_OK) 
        << "CaseDescription: 2. check it (status == WMS_ERROR_OK)";
    ASSERT_EQ(shotInfo.id, 0u) << "CaseDescription: 2. check it (id == 0)";
    ASSERT_GT(shotInfo.fd, 0) << "CaseDescription: 2. check it (fd > 0)";
    ASSERT_GT(shotInfo.width, 0) << "CaseDescription: 2. check it (width > 0)";
    ASSERT_GT(shotInfo.height, 0) << "CaseDescription: 2. check it (height > 0)";
    ASSERT_GT(shotInfo.stride, 0) << "CaseDescription: 2. check it (stride > 0)";
    ASSERT_GT(shotInfo.format, 0u) << "CaseDescription: 2. check it (format > 0)";
    ASSERT_GT(shotInfo.seconds, 0u) << "CaseDescription: 2. check it (seconds > 0)";
    ASSERT_GT(shotInfo.nanoseconds, 0u) << "CaseDescription: 2. check it (nanoseconds > 0)";
}

/*
 * Feature: wms_screenshot by abnormal arguments
 * Function: WMSServer
 * SubFunction: wms_screenshot
 * FunctionPoints: wms_screenshot abnormal arguments
 * EnvConditions: display, compositor, wms init success.
 * CaseDescription: 1. ScreenShot by abnormal arguments
 *                  2. check result
 * 
 */ 
HWTEST_F(WMServerTest, ScreenShot002, testing::ext::TestSize.Level0)
{
    // WMSServer init success.
    ASSERT_NE(ctx.display, nullptr) << "EnvConditions: wl_display init success..";
    ASSERT_NE(ctx.compositor, nullptr) << "EnvConditions: wl_compositor init success..";
    ASSERT_NE(ctx.wms, nullptr) << "EnvConditions: wms init success..";

    // 1. ScreenShot by abnormal arguments
    {
        std::unique_lock<std::mutex> lck(syncMutex);
        replyFlag = false;
        wms_screenshot(ctx.wms, 2);
        wms_commit_changes(ctx.wms);
        wl_display_flush(ctx.display);
        syncVariable.wait(lck, [&](){ return replyFlag; });
    }

    // 2. check it
    ASSERT_NE(shotInfo.status, WMS_ERROR_OK) 
        << "CaseDescription: 2. check it (status != WMS_ERROR_OK)";
    ASSERT_EQ(shotInfo.id, 2) << "CaseDescription: 2. check it (id == 2)";
}

/*
 * Feature: wms_windowshot by normal arguments
 * Function: WMSServer
 * SubFunction: wms_windowshot
 * FunctionPoints: wms_windowshot normal arguments
 * EnvConditions: display, compositor, wms init success.
 * CaseDescription: 1. Create WlSurface
 *                  2. Create Window by normal arguments
 *                  3. check result
 *                  4. WindowShot by normal arguments
 *                  5. check result
 */ 
HWTEST_F(WMServerTest, WindowShot001, testing::ext::TestSize.Level0)
{
    // WMSServer init success.
    ASSERT_NE(ctx.display, nullptr) << "EnvConditions: wl_display init success..";
    ASSERT_NE(ctx.compositor, nullptr) << "EnvConditions: wl_compositor init success..";
    ASSERT_NE(ctx.wms, nullptr) << "EnvConditions: wms init success..";
    ASSERT_NE(ctx.wl_shm, nullptr) << "EnvConditions: wl_shm init success..";

    // 1. create WlSurface
    struct wl_surface* wlSurface = wl_compositor_create_surface(ctx.compositor);
    ASSERT_NE(wlSurface, nullptr) << "CaseDescription: 1. create WlSurface (wlSurface != nullptr)";

    // 2. Create Window by normal arguments
    {
        std::unique_lock<std::mutex> lck(syncMutex);
        replyFlag = false;
        wms_create_window(ctx.wms, wlSurface, 0, WMS_WINDOW_TYPE_NORMAL);
        wl_display_flush(ctx.display);
        syncVariable.wait(lck, [&](){ return replyFlag; });
    }

    // 3. check it
    ASSERT_EQ(windowStatus.status, WMS_WINDOW_STATUS_CREATED) 
        << "CaseDescription: 3. check it (status == WMS_WINDOW_STATUS_CREATED)";
    ASSERT_GT(windowStatus.wid, 0) << "CaseDescription: 3. check it (wid > 0)";
    ASSERT_GE(windowStatus.x, 0) << "CaseDescription: 3. check it (x >= 0)";
    ASSERT_GE(windowStatus.y, 0) << "CaseDescription: 3. check it (y >= 0)";
    ASSERT_GT(windowStatus.width, 0u) << "CaseDescription: 3. check it (width > 0)";
    ASSERT_GT(windowStatus.height, 0u) << "CaseDescription: 3. check it (height > 0)";

    struct wl_buffer* wl_buffer = CreateShmBuffer(ctx.wl_shm, windowStatus.width, windowStatus.height);
    ASSERT_NE(wl_buffer, nullptr);
    
    wl_surface_attach(wlSurface, wl_buffer, 0, 0);
    wl_surface_damage(wlSurface, 0, 0, windowStatus.width, windowStatus.height);
    wl_surface_commit(wlSurface);
    wl_display_flush(ctx.display);

    // 4. WindowShot by normal arguments
    {
        std::unique_lock<std::mutex> lck(syncMutex);
        replyFlag = false;
        wms_windowshot(ctx.wms, windowStatus.wid);
        wms_commit_changes(ctx.wms);
        wl_display_flush(ctx.display);
        syncVariable.wait(lck, [&](){ return replyFlag; });
    }

    // 5. check it
    ASSERT_EQ(shotInfo.status, WMS_ERROR_OK) 
        << "CaseDescription: 5. check it (status == WMS_ERROR_OK)";
    ASSERT_EQ(shotInfo.id, windowStatus.wid) << "CaseDescription: 5. check it (id == windowStatus.wid)";
    ASSERT_GT(shotInfo.fd, 0) << "CaseDescription: 5. check it (fd > 0)";
    ASSERT_EQ(shotInfo.width, windowStatus.width) 
        << "CaseDescription: 5. check it (width == windowStatus.width)";
    ASSERT_EQ(shotInfo.height, windowStatus.height) 
        << "CaseDescription: 5. check it (height == windowStatus.height)";
    ASSERT_EQ(shotInfo.stride, windowStatus.width * 4) 
        << "CaseDescription: 5. check it (stride ==  windowStatus.width * 4)";
    ASSERT_GT(shotInfo.format, 0u) << "CaseDescription: 5. check it (format > 0)";
    ASSERT_GT(shotInfo.seconds, 0u) << "CaseDescription: 5. check it (seconds > 0)";
    ASSERT_GT(shotInfo.nanoseconds, 0u) << "CaseDescription: 5. check it (nanoseconds > 0)";
}

/*
 * Feature: wms_windowshot by abnormal arguments
 * Function: WMSServer
 * SubFunction: wms_windowshot
 * FunctionPoints: wms_windowshot abnormal arguments
 * EnvConditions: display, compositor, wms init success.
 * CaseDescription: 1. WindowShot by abnormal arguments
 *                  2. check result
 * 
 */ 
HWTEST_F(WMServerTest, WindowShot002, testing::ext::TestSize.Level0)
{
    // WMSServer init success.
    ASSERT_NE(ctx.display, nullptr) << "EnvConditions: wl_display init success..";
    ASSERT_NE(ctx.compositor, nullptr) << "EnvConditions: wl_compositor init success..";
    ASSERT_NE(ctx.wms, nullptr) << "EnvConditions: wms init success..";

    // 1. WindowShot by abnormal arguments
    {
        std::unique_lock<std::mutex> lck(syncMutex);
        replyFlag = false;
        wms_windowshot(ctx.wms, 0);
        wms_commit_changes(ctx.wms);
        wl_display_flush(ctx.display);
        syncVariable.wait(lck, [&](){ return replyFlag; });
    }

    // 2. check it
    ASSERT_NE(shotInfo.status, WMS_ERROR_OK) 
        << "CaseDescription: 2. check it (status != WMS_ERROR_OK)";
    ASSERT_EQ(shotInfo.id, 0) << "CaseDescription: 2. check it (id == 0)";
}

} // namespace
} // namespace OHOS
