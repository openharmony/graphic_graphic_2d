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

#ifndef FRAMEWORKS_WM_SRC_TEST_DRAW_NATIVE_TEST_H
#define FRAMEWORKS_WM_SRC_TEST_DRAW_NATIVE_TEST_H

#include <cstdint>
#include <functional>
#include <optional>

#include <refbase.h>
#include <window_manager.h>

namespace OHOS {
class NativeTestFactory {
public:
    static sptr<Window> CreateWindow(WindowType type,
                                     sptr<Surface> csurf = nullptr,
                                     std::optional<uint32_t> did = std::nullopt);
    static inline int32_t defaultDisplayID = 0;
};

using DrawFunc = std::function<void(uint32_t *, uint32_t, uint32_t, uint32_t)>;
class NativeTestSync : public RefBase {
public:
    static sptr<NativeTestSync> CreateSync(DrawFunc drawFunc, sptr<Surface> &psurf, void *data = nullptr);

private:
    void Sync(int64_t, void *data);

    sptr<Surface> surf = nullptr;
    DrawFunc draw = nullptr;
    uint32_t count = 0;
};

class NativeTestDrawer : public RefBase {
public:
    static sptr<NativeTestDrawer> CreateDrawer(DrawFunc drawFunc, sptr<Surface> &psurf, void *data = nullptr);

    void SetDrawFunc(DrawFunc draw);

    void DrawOnce();

private:
    void Sync(int64_t, void *);

    sptr<Surface> surf = nullptr;
    DrawFunc draw = nullptr;
    uint32_t count = 0;
    bool isDrawing = false;
    void *data = nullptr;
};

class NativeTestDraw {
public:
    static void FlushDraw(uint32_t *addr, uint32_t width, uint32_t height, uint32_t count);
    static void ColorDraw(uint32_t *addr, uint32_t width, uint32_t height, uint32_t count);
    static void BlackDraw(uint32_t *addr, uint32_t width, uint32_t height, uint32_t count);
    static void RainbowDraw(uint32_t *addr, uint32_t width, uint32_t height, uint32_t count);
    static inline constexpr int32_t RainbowDrawFramerate = 100;
    static void BoxDraw(uint32_t *addr, uint32_t width, uint32_t height, uint32_t count);
    static void PureColorDraw(uint32_t *addr, uint32_t width, uint32_t height, uint32_t count, uint32_t *color);
};
} // namespace OHOS

#endif // FRAMEWORKS_WM_SRC_TEST_DRAW_NATIVE_TEST_H
