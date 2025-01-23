/*
 * Copyright (c) 2025 Huawei Device Co., Ltd.
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

#include "hdiframebuffersurface_fuzzer.h"

#include <securec.h>

#include "hdi_framebuffer_surface.h"

using namespace OHOS::Rosen;

namespace OHOS {
    namespace {
        const uint8_t* data_ = nullptr;
        size_t size_ = 0;
        size_t pos;
    }

    /*
    * describe: get data from outside untrusted data(data_) which size is according to sizeof(T)
    * tips: only support basic type
    */
    template<class T>
    T GetData()
    {
        T object {};
        size_t objectSize = sizeof(object);
        if (data_ == nullptr || objectSize > size_ - pos) {
            return object;
        }
        errno_t ret = memcpy_s(&object, objectSize, data_ + pos, objectSize);
        if (ret != EOK) {
            return {};
        }
        pos += objectSize;
        return object;
    }
    
    bool DoSomethingInterestingWithMyAPI(const uint8_t* data, size_t size)
    {
        if (data == nullptr) {
            return false;
        }

        // initialize
        data_ = data;
        size_ = size;
        pos = 0;
        sptr<HdiFramebufferSurface> fbSurface = HdiFramebufferSurface::CreateFramebufferSurface();
        if (fbSurface == nullptr) {
            return false;
        }

        //get data
        uint32_t bufferSize = GetData<uint32_t>() % 10 + 1; // 1-10
        uint8_t operation = GetData<uint8_t>(); //operation choice

        int operationCount = 4;
        int tryReleaseFramebuffer = 0;
        int clearFrameBuffer = 1;
        int dump = 2;
        int onBufferAvailable = 3;
        // test
        fbSurface->SetBufferQueueSize(bufferSize);
        fbSurface->GetBufferQueueSize();
        fbSurface->OnBufferAvailable();
        auto fbEntry = fbSurface->GetFramebuffer();
        switch (operation % operationCount) {
            case tryReleaseFramebuffer: {
                // try ReleaseFramebuffer
                if (fbEntry) {
                    auto buffer = fbEntry->buffer;
                    auto fence = fbEntry->acquireFence;
                    fbSurface->ReleaseFramebuffer(buffer, fence);
                }
                break;
            }
            case clearFrameBuffer: {
                fbSurface->ClearFrameBuffer();
                break;
            }
            case dump: {
                std::string result;
                fbSurface->Dump(result);
                break;
            }
            case onBufferAvailable: {
                // multiple OnBufferAvailable, see if it can trigger queue full, Acquire failure, etc.
                fbSurface->OnBufferAvailable();
                fbSurface->OnBufferAvailable();
                break;
            }
            default:
                break;
        }
        return true;
    }
}

/* Fuzzer entry point */
extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size)
{
    /* Run your code on data */
    OHOS::DoSomethingInterestingWithMyAPI(data, size);
    return 0;
}

