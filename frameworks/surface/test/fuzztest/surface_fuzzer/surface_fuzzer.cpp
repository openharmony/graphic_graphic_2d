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

#include "iconsumer_surface.h"
#include "surface.h"
#include "surface_buffer.h"
#include "surface_buffer_impl.h"
#include "sync_fence.h"
#include <iostream>

namespace OHOS {
    namespace {
        const uint8_t* g_data = nullptr;
        size_t g_size = 0;
        size_t g_pos;
        constexpr size_t STR_LEN = 10;
    }

    /*
    * describe: get data from outside untrusted data(g_data) which size is according to sizeof(T)
    * tips: only support basic type
    */
    template<class T>
    T GetData()
    {
        T object {};
        size_t objectSize = sizeof(object);
        if (g_data == nullptr || objectSize > g_size - g_pos) {
            return object;
        }
        errno_t ret = memcpy_s(&object, objectSize, g_data + g_pos, objectSize);
        if (ret != EOK) {
            return {};
        }
        g_pos += objectSize;
        return object;
    }

    /*
    * get a string from g_data
    */
    std::string GetStringFromData(int strlen)
    {
        char cstr[strlen];
        cstr[strlen - 1] = '\0';
        for (int i = 0; i < strlen - 1; i++) {
            char tmp = GetData<char>();
            if (tmp == '\0') {
                tmp = '1';
            }
            cstr[i] = tmp;
        }
        std::string str(cstr);
        return str;
    }

    void SurfaceFuzzTest2()
    {
        // get data
        std::string name = GetStringFromData(STR_LEN);
        bool isShared = GetData<bool>();
        std::string key = GetStringFromData(STR_LEN);
        std::string val = GetStringFromData(STR_LEN);
        BufferVerifyAllocInfo info = GetData<BufferVerifyAllocInfo>();
        bool supported = GetData<bool>();
        uint32_t sequence = GetData<uint32_t>();
        ScalingMode scalingMode = GetData<ScalingMode>();
        GraphicHDRMetaData metaData = GetData<GraphicHDRMetaData>();
        GraphicHDRMetadataKey metakey = GetData<GraphicHDRMetadataKey>();
        uint8_t metaData2 = GetData<uint8_t>();
        HDRMetaDataType metaType = GetData<HDRMetaDataType>();
        GraphicPresentTimestamp ptimestamp = GetData<GraphicPresentTimestamp>();
        GraphicPresentTimestampType type = GetData<GraphicPresentTimestampType>();
        int64_t time = GetData<int64_t>();
        std::string result = GetStringFromData(STR_LEN);

        std::vector<BufferVerifyAllocInfo> infos = {info};
        std::vector<bool> supporteds = {supported};
        std::vector<GraphicHDRMetaData> metaDatas = {metaData};
        std::vector<uint8_t> metaDatas2 = {metaData2};

        // test
        sptr<OHOS::IConsumerSurface> cSurface = OHOS::IConsumerSurface::Create(name, isShared);
        auto producer = cSurface->GetProducer();
        sptr<OHOS::Surface> pSurface = OHOS::Surface::CreateSurfaceAsProducer(producer);

        pSurface->SetUserData(key, val);
        pSurface->IsSupportedAlloc(infos, supporteds);
        pSurface->SetScalingMode(sequence, scalingMode);
        pSurface->GetPresentTimestamp(sequence, type, time);
        cSurface->SetUserData(key, val);
        
        cSurface->SetScalingMode(sequence, scalingMode);
        cSurface->GetScalingMode(sequence, scalingMode);
        cSurface->SetMetaData(sequence, metaDatas);
        cSurface->SetMetaDataSet(sequence, metakey, metaDatas2);
        cSurface->QueryMetaDataType(sequence, metaType);
        cSurface->GetMetaData(sequence, metaDatas);
        cSurface->GetMetaDataSet(sequence, metakey, metaDatas2);
        cSurface->SetPresentTimestamp(sequence, ptimestamp);
        cSurface->Dump(result);
    }

    bool DoSomethingInterestingWithMyAPI(const uint8_t* data, size_t size)
    {
        if (data == nullptr) {
            return false;
        }

        // initialize
        g_data = data;
        g_size = size;
        g_pos = 0;

        // get data
        std::string name = GetStringFromData(STR_LEN);
        bool isShared = GetData<bool>();
        BufferRequestConfig requestConfig = GetData<BufferRequestConfig>();
        BufferFlushConfig flushConfig = GetData<BufferFlushConfig>();
        int64_t timestamp = GetData<int64_t>();
        Rect damage = GetData<Rect>();
        uint32_t seqNum = GetData<uint32_t>();
        uint32_t queueSize = GetData<uint32_t>();
        int32_t width = GetData<int32_t>();
        int32_t height = GetData<int32_t>();
        uint64_t usage = GetData<uint64_t>();
        GraphicTransformType transform = GetData<GraphicTransformType>();

        // test
        sptr<OHOS::IConsumerSurface> cSurface = OHOS::IConsumerSurface::Create(name, isShared);
        auto producer = cSurface->GetProducer();
        sptr<OHOS::Surface> pSurface = OHOS::Surface::CreateSurfaceAsProducer(producer);
        sptr<OHOS::SurfaceBuffer> buffer = new SurfaceBufferImpl(seqNum);
        sptr<SyncFence> syncFence = SyncFence::INVALID_FENCE;
        int32_t fenceFd = syncFence->Get();

        pSurface->RequestBuffer(buffer, fenceFd, requestConfig);
        pSurface->CancelBuffer(buffer);
        pSurface->FlushBuffer(buffer, fenceFd, flushConfig);
        pSurface->AttachBuffer(buffer);
        pSurface->DetachBuffer(buffer);
        pSurface->SetQueueSize(queueSize);
        pSurface->SetTransform(transform);

        cSurface->AcquireBuffer(buffer, fenceFd, timestamp, damage);
        cSurface->ReleaseBuffer(buffer, fenceFd);
        cSurface->AttachBuffer(buffer);
        cSurface->DetachBuffer(buffer);
        cSurface->SetQueueSize(queueSize);
        cSurface->SetDefaultWidthAndHeight(width, height);
        cSurface->SetDefaultUsage(usage);
        cSurface->SetTransform(transform);

        SurfaceFuzzTest2();

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

