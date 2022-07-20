/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
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

#include "surface_fuzzer.h"

#include <securec.h>

#include "surface.h"
#include "surface_buffer.h"
#include "surface_buffer_impl.h"
#include "sync_fence.h"

namespace OHOS {
    namespace {
        const uint8_t* data_ = nullptr;
        size_t size_ = 0;
        size_t pos;
        constexpr size_t STR_LEN = 10;
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

    /*
    * get a string from data_
    */
    std::string GetStringFromData(int strlen)
    {
        char cstr[strlen];
        cstr[strlen - 1] = '\0';
        for (int i = 0; i < strlen - 1; i++) {
            cstr[i] = GetData<char>();
        }
        std::string str(cstr);
        return str;
    }

    bool DoSomethingInterestingWithMyAPI(const uint8_t* data, size_t size)
    {
        if (data == nullptr || size <= 0) {
            return false;
        }

        // initialize
        data_ = data;
        size_ = size;
        pos = 0;

        // get data
        bool isShared = GetData<bool>();
        std::string name = GetStringFromData(STR_LEN);
        BufferRequestConfig requestConfig = GetData<BufferRequestConfig>();
        BufferFlushConfig flushConfig = GetData<BufferFlushConfig>();
        int64_t timestamp = GetData<int64_t>();
        Rect damage = GetData<Rect>();

        // test
        sptr<OHOS::Surface> cSurface = OHOS::Surface::CreateSurfaceAsConsumer(name, isShared);
        auto producer = cSurface->GetProducer();
        sptr<OHOS::Surface> pSurface = OHOS::Surface::CreateSurfaceAsProducer(producer);
        sptr<OHOS::SurfaceBuffer> buffer = nullptr;
        sptr<SyncFence> syncFence = SyncFence::INVALID_FENCE;
        auto sRet = pSurface->RequestBuffer(buffer, syncFence, requestConfig);
        sRet = pSurface->FlushBuffer(buffer, syncFence, flushConfig);
        sRet = cSurface->AcquireBuffer(buffer, syncFence, timestamp, damage);
        sRet = cSurface->ReleaseBuffer(buffer, syncFence);

        return true;
    }
} // namespace OHOS

/* Fuzzer entry point */
extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size)
{
    /* Run your code on data */
    OHOS::DoSomethingInterestingWithMyAPI(data, size);
    return 0;
}

