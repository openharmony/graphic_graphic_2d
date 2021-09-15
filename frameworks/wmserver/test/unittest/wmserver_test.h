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

#ifndef FRAMEWORKS_WMSERVER_TEST_UNITTEST_WMSERVER_TEST_H
#define FRAMEWORKS_WMSERVER_TEST_UNITTEST_WMSERVER_TEST_H

#include <list>

#include <gtest/gtest.h>
#include <wms-client-protocol.h>

#include "wmserver.h"

struct WMSScreenShotInfo {
    uint32_t status;
    uint32_t id;
    int32_t fd;
    int32_t width;
    int32_t height;
    int32_t stride;
    uint32_t format;
    uint32_t seconds;
    uint32_t nanoseconds;
};

struct WMSDisplayInfo {
    uint32_t id;
    std::string name;
    uint32_t height;
    uint32_t width;
};

struct WMSContext {
    struct wl_display *display;
    struct wl_compositor *compositor;
    struct wms *wms;
    struct wl_shm *wl_shm;
};

struct WMSWindowStatus {
    uint32_t status;
    int32_t wid;
    int32_t x;
    int32_t y;
    uint32_t width;
    uint32_t height;
};

struct WMSGlobalWindowStatus {
    uint32_t pid;
    int32_t wid;
    uint32_t status;
};

namespace OHOS {
class WMServerTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    virtual void SetUp() override;
    virtual void TearDown() override;

private:
    static inline struct WMSContext ctx;
    static inline pthread_t tid;

    // for wms request/reply
    static inline std::mutex syncMutex;
    static inline bool replyFlag = false;
    static inline std::condition_variable syncVariable;

    // for wms relply info check.
    static inline uint32_t replyStatus;
    static inline WMSWindowStatus windowStatus;
    static inline std::list<WMSDisplayInfo> displayInfo;
    static inline WMSScreenShotInfo shotInfo;

    // for wms global window status check.
    static inline std::mutex syncMutexGlobalWinInfoCb;
    static inline bool replyGlobalWinInfoCbFlag = false;
    static inline std::condition_variable globalWinInfoCbVariable;
    static inline WMSGlobalWindowStatus gloadWindowStatus;

    // for passive destroy window.
    static inline std::mutex destroyMutex;
    static inline bool destroyReplyFlag = false;
    static inline std::condition_variable destroyVariable;
};
}

#endif // FRAMEWORKS_WMSERVER_TEST_UNITTEST_WMSERVER_TEST_H
