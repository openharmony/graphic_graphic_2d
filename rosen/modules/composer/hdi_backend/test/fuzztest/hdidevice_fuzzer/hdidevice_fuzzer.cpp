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
        constexpr int DEFAULT_FENCE = 100;
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

    void HdiDeviceFuzzTest3()
    {
        // get data
        uint32_t screenId = GetData<uint32_t>();
        uint32_t layerId = GetData<uint32_t>();
        GraphicHDRMetaData metaData = GetData<GraphicHDRMetaData>();
        uint8_t metaData2 = GetData<uint8_t>();
        GraphicHDRMetadataKey key = GetData<GraphicHDRMetadataKey>();
        GraphicPresentTimestamp timestamp = GetData<GraphicPresentTimestamp>();
        uint32_t maskInfo = GetData<uint32_t>();

        // test
        HdiDevice *device = HdiDevice::GetInstance();
        std::vector<GraphicHDRMetaData> metaDatas = {metaData};
        device->SetLayerMetaData(screenId, layerId, metaDatas);
        std::vector<uint8_t> metaDatas2 = {metaData2};
        device->SetLayerMetaDataSet(screenId, layerId, key, metaDatas2);
        device->SetLayerTunnelHandle(screenId, layerId, nullptr);
        device->GetPresentTimestamp(screenId, layerId, timestamp);
        device->SetLayerMaskInfo(screenId, layerId, maskInfo);

        // DISPLAY ENGINE
        std::vector<std::string> keys{};
        device->GetSupportedLayerPerFrameParameterKey(keys);
        std::vector<int8_t> valueBlob{static_cast<int8_t>(1)};
        std::string validKey = "ArsrDoEnhance";
        device->SetLayerPerFrameParameter(screenId, layerId, validKey, valueBlob);
    }

    void HdiDeviceFuzzTest2()
    {
        // get data
        GraphicColorGamut gamut = GetData<GraphicColorGamut>();
        uint32_t screenId = GetData<uint32_t>();
        uint32_t layerId = GetData<uint32_t>();
        int32_t fenceFd = GetData<int32_t>() % 32768; // maximum fd of linux is 32768
        // fd 0,1,2 represent stdin, stdout and stderr respectively, they should not be closed.
        fenceFd = ((fenceFd >= 0 && fenceFd <= 2) ? DEFAULT_FENCE : fenceFd);
        sptr<SyncFence> fence = new SyncFence(fenceFd);
        GraphicLayerAlpha alpha = GetData<GraphicLayerAlpha>();
        GraphicIRect layerRect = GetData<GraphicIRect>();
        GraphicTransformType ttype = GetData<GraphicTransformType>();
        GraphicIRect visible = GetData<GraphicIRect>();
        std::vector<GraphicIRect> visibles = { visible };
        GraphicIRect dirty = GetData<GraphicIRect>();
        std::vector<GraphicIRect> dirtyRegions = { dirty };
        GraphicCompositionType ctype = GetData<GraphicCompositionType>();
        GraphicBlendType btype = GetData<GraphicBlendType>();
        GraphicIRect crop = GetData<GraphicIRect>();
        uint32_t zorder = GetData<uint32_t>();
        bool isPreMulti = GetData<bool>();
        float matrixElement = GetData<float>();
        std::vector<float> matrix = { matrixElement };
        GraphicColorDataSpace colorSpace = GetData<GraphicColorDataSpace>();
        
        uint32_t deletingIndex = GetData<uint32_t>();
        std::vector<uint32_t> deletingList = { deletingIndex };

        // test
        HdiDevice *device = HdiDevice::GetInstance();
        device->GetScreenColorGamut(screenId, gamut);
        device->SetLayerAlpha(screenId, layerId, alpha);
        device->SetLayerSize(screenId, layerId, layerRect);
        device->SetTransformMode(screenId, layerId, ttype);
        device->SetLayerVisibleRegion(screenId, layerId, visibles);
        device->SetLayerDirtyRegion(screenId, layerId, dirtyRegions);
        GraphicLayerBuffer layerBuffer = {nullptr, INVALID_BUFFER_CACHE_INDEX, fence, deletingList};
        device->SetLayerBuffer(screenId, layerId, layerBuffer);
        device->SetLayerCompositionType(screenId, layerId, ctype);
        device->SetLayerBlendType(screenId, layerId, btype);
        device->SetLayerCrop(screenId, layerId, crop);
        device->SetLayerZorder(screenId, layerId, zorder);
        device->SetLayerPreMulti(screenId, layerId, isPreMulti);
        device->SetLayerColorTransform(screenId, layerId, matrix);
        device->SetLayerColorDataSpace(screenId, layerId, colorSpace);
        device->GetLayerColorDataSpace(screenId, layerId, colorSpace);

        HdiDeviceFuzzTest3();
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
        int32_t fenceFd = GetData<int32_t>() % 32768; // maximum fd of linux is 32768
        // fd 0,1,2 represent stdin, stdout and stderr respectively, they should not be closed.
        if (fenceFd >= 0 && fenceFd <= 2) {
            fenceFd = DEFAULT_FENCE;
        }
        sptr<SyncFence> fence = new SyncFence(fenceFd);
        uint32_t cacheIndex = GetData<uint32_t>();
        GraphicIRect damageRect = GetData<GraphicIRect>();
        std::vector<GraphicIRect> damageRects = { damageRect };
        GraphicGamutMap gamutMap = GetData<GraphicGamutMap>();

        uint32_t formatCount = GetData<uint32_t>();
        GraphicHDRFormat formats = GetData<GraphicHDRFormat>();
        float maxLum = GetData<float>();
        float maxAverageLum = GetData<float>();
        float minLum = GetData<float>();

        // test
        HdiDevice *device = HdiDevice::GetInstance();
        device->PrepareScreenLayers(screenId, needFlush);
        std::vector<uint32_t> layersId;
        std::vector<int32_t> types;
        device->GetScreenCompChange(screenId, layersId, types);
        device->SetScreenClientBuffer(screenId, nullptr, cacheIndex, fence);
        device->SetScreenClientDamage(screenId, damageRects);
        std::vector<uint32_t> layers;
        std::vector<sptr<SyncFence>> fences;
        device->GetScreenReleaseFence(screenId, layers, fences);
        std::vector<GraphicColorGamut> gamuts;
        device->GetScreenSupportedColorGamuts(screenId, gamuts);
        device->SetScreenGamutMap(screenId, gamutMap);
        GraphicHDRCapability info = {
            .formatCount = formatCount,
            .maxLum = maxLum,
            .maxAverageLum = maxAverageLum,
            .minLum = minLum,
        };
        info.formats.push_back(formats);
        device->GetHDRCapabilityInfos(screenId, info);
        std::vector<GraphicHDRMetadataKey> keys;
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

