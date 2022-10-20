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

#include "hdidevice_fuzzer.h"

#include <securec.h>

#include "hdi_device.h"
#include "hdi_backend.h"
#include "surface.h"
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

    void HdiDeviceFuzzTest2()
    {
        // get data
        uint32_t screenId = GetData<uint32_t>();
        uint32_t layerId = GetData<uint32_t>();
        int32_t fenceFd = GetData<int32_t>();
        sptr<SyncFence> fence = new SyncFence(fenceFd);
        LayerAlpha alpha = GetData<LayerAlpha>();
        IRect layerRect = GetData<IRect>();
        TransformType ttype = GetData<TransformType>();
        uint32_t num = GetData<uint32_t>();
        IRect visible = GetData<IRect>();
        IRect dirty = GetData<IRect>();
        BufferHandle handle = GetData<BufferHandle>();
        CompositionType ctype = GetData<CompositionType>();
        BlendType btype = GetData<BlendType>();
        IRect crop = GetData<IRect>();
        uint32_t zorder = GetData<uint32_t>();
        bool isPreMulti = GetData<bool>();
        float matrix = GetData<float>();
        ColorDataSpace colorSpace = GetData<ColorDataSpace>();
        HDRMetaData metaData = GetData<HDRMetaData>();
        HDRMetadataKey key = GetData<HDRMetadataKey>();
        uint8_t metaData2 = GetData<uint8_t>();
        ExtDataHandle extHandle = GetData<ExtDataHandle>();
        PresentTimestamp timestamp = GetData<PresentTimestamp>();

        // test
        Base::HdiDevice *device = HdiDevice::GetInstance();
        device->SetLayerAlpha(screenId, layerId, alpha);
        device->SetLayerSize(screenId, layerId, layerRect);
        device->SetTransformMode(screenId, layerId, ttype);
        device->SetLayerVisibleRegion(screenId, layerId, num, visible);
        device->SetLayerDirtyRegion(screenId, layerId, dirty);
        device->SetLayerBuffer(screenId, layerId, &handle, fence);
        device->SetLayerCompositionType(screenId, layerId, ctype);
        device->SetLayerBlendType(screenId, layerId, btype);
        device->SetLayerCrop(screenId, layerId, crop);
        device->SetLayerZorder(screenId, layerId, zorder);
        device->SetLayerPreMulti(screenId, layerId, isPreMulti);
        device->SetLayerColorTransform(screenId, layerId, &matrix);
        device->SetLayerColorDataSpace(screenId, layerId, colorSpace);
        device->GetLayerColorDataSpace(screenId, layerId, colorSpace);
        std::vector<HDRMetaData> metaDatas = {metaData};
        device->SetLayerMetaData(screenId, layerId, metaDatas);
        std::vector<uint8_t> metaDatas2 = {metaData2};
        device->SetLayerMetaDataSet(screenId, layerId, key, metaDatas2);
        device->SetLayerTunnelHandle(screenId, layerId, &extHandle);
        device->GetPresentTimestamp(screenId, layerId, timestamp);
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

        // get data
        uint32_t screenId = GetData<uint32_t>();
        bool needFlush = GetData<bool>();
        BufferHandle buffer = GetData<BufferHandle>();
        int32_t fenceFd = GetData<int32_t>();
        sptr<SyncFence> fence = new SyncFence(fenceFd);
        uint32_t num = GetData<uint32_t>();
        IRect damageRect = GetData<IRect>();
        ColorGamut gamut = GetData<ColorGamut>();
        GamutMap gamutMap = GetData<GamutMap>();

        uint32_t formatCount = GetData<uint32_t>();
        HDRFormat formats = GetData<HDRFormat>();
        float maxLum = GetData<float>();
        float maxAverageLum = GetData<float>();
        float minLum = GetData<float>();

        // test
        Base::HdiDevice *device = HdiDevice::GetInstance();
        device->PrepareScreenLayers(screenId, needFlush);
        std::vector<uint32_t> layersId;
        std::vector<int32_t> types;
        device->GetScreenCompChange(screenId, layersId, types);
        device->SetScreenClientBuffer(screenId, &buffer, fence);
        device->SetScreenClientDamage(screenId, num, damageRect);
        std::vector<uint32_t> layers;
        std::vector<sptr<SyncFence>> fences;
        device->GetScreenReleaseFence(screenId, layers, fences);
        std::vector<ColorGamut> gamuts;
        device->GetScreenSupportedColorGamuts(screenId, gamuts);
        device->GetScreenColorGamut(screenId, gamut);
        device->SetScreenGamutMap(screenId, gamutMap);
        HDRCapability info = {
            .formatCount = formatCount,
            .formats = &formats,
            .maxLum = maxLum,
            .maxAverageLum = maxAverageLum,
            .minLum = minLum,
        };
        device->GetHDRCapabilityInfos(screenId, info);
        std::vector<HDRMetadataKey> keys;
        device->GetSupportedMetaDataKey(screenId, keys);
        device->Commit(screenId, fence);

        HdiDeviceFuzzTest2();

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

