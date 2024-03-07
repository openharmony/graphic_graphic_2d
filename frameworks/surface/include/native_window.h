/*
 * Copyright (c) 2021-2022 Huawei Device Co., Ltd.
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

#ifndef BS_NATIVE_WINDOW_H
#define BS_NATIVE_WINDOW_H

#include "window.h"

#include <refbase.h>
#include <surface.h>
#include <surface_buffer.h>
#include <unordered_map>

struct NativeWindowMagic : public OHOS::RefBase {
    NativeWindowMagic(NativeObjectMagic m) : magic(m) {}
    virtual ~NativeWindowMagic() {}
    NativeObjectMagic magic;
};

struct NativeWindow : public NativeWindowMagic {
    NativeWindow();
    ~NativeWindow();
    OHOS::BufferRequestConfig config = {0};
    OHOS::sptr<OHOS::Surface> surface;
    int64_t uiTimestamp = 0;
    uint32_t lastBufferSeqNum = 0;
    std::unordered_map<uint32_t, NativeWindowBuffer*> bufferCache_;
};

struct NativeWindowBuffer : public NativeWindowMagic {
    NativeWindowBuffer();
    ~NativeWindowBuffer();
    OHOS::sptr<OHOS::SurfaceBuffer> sfbuffer;
    int64_t uiTimestamp = 0;
};


#endif
