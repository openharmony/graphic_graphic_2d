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

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <wayland-client.h>
#include <wms-client-protocol.h>

#define COMMAND_ARG_COUNT 2

enum ScreenInfoTestId {
    ID_GET_SCREENS_INFO = 1,
    ID_GET_SEATS_INFO = 2,
    ID_SET_LISTENER = 3,
    ID_UNSET_LISTENER = 4,
};

struct MyContext {
    struct wl_display *display;
    struct wl_registry *registry;
    struct screen_info *screenInfo;
};

static bool g_running = true;

void Reply(void *data, struct screen_info *screenInfo, uint32_t status)
{
    printf("Reply: %d\n", status);
    g_running = false;
}

struct screen_info_listener g_listener = {Reply};

static void RegistryHandleGlobal(void *data, struct wl_registry *registry,
    uint32_t id, const char *interface, uint32_t version)
{
    struct MyContext *ctx = data;

    if (strcmp(interface, "screen_info") == 0) {
        ctx->screenInfo = wl_registry_bind(registry, id, &screen_info_interface, 1);
        if (ctx->screenInfo != NULL) {
            screen_info_add_listener(ctx->screenInfo, &g_listener, ctx);
        }
    }
}

static void RegistryHandleGlobalRemove(void *a, struct wl_registry *b, uint32_t c)
{
}

static const struct wl_registry_listener g_registryListener = {
    RegistryHandleGlobal,
    RegistryHandleGlobalRemove
};

void Usage()
{
    printf("Usage: screen-info-test test id\n");
    printf("test 1: GetScreensInfo test.\n");
    printf("test 2: GetSeatsInfo test.\n");
    printf("test 3: Set Listener test.\n");
    printf("test 4: UnSet Listener test.\n");
}

int main(int argc, char **argv)
{
    int ret = 0;
    struct MyContext ctx;

    if (argc != COMMAND_ARG_COUNT) {
        Usage();
        return 1;
    }

    int testId = atoi(argv[1]);
    if (testId < ID_GET_SCREENS_INFO || testId > ID_UNSET_LISTENER) {
        printf("invalid testId.\n");
        Usage();
        return 1;
    }

    ctx.display = wl_display_connect(NULL);
    if (!ctx.display) {
        printf("wl_display_connect failed\n");
        return 1;
    }

    ctx.registry = wl_display_get_registry(ctx.display);
    wl_registry_add_listener(ctx.registry, &g_registryListener, &ctx);
    wl_display_roundtrip(ctx.display);
    wl_display_roundtrip(ctx.display);

    if (testId == ID_GET_SCREENS_INFO || testId == ID_GET_SEATS_INFO) {
        screen_info_get(ctx.screenInfo, testId);
    } else if (testId == ID_SET_LISTENER) {
        screen_info_listener(ctx.screenInfo, SCREEN_INFO_LISTENER_SET_ENABLE);
    } else {
        screen_info_listener(ctx.screenInfo, SCREEN_INFO_LISTENER_SET_DISABLE);
    }

    while (g_running && ret != -1) {
        ret = wl_display_dispatch(ctx.display);
    }

    if (ctx.screenInfo) {
        screen_info_destroy(ctx.screenInfo);
    }

    wl_registry_destroy(ctx.registry);
    wl_display_flush(ctx.display);
    wl_display_disconnect(ctx.display);
    return 0;
}
