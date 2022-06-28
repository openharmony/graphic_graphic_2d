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

#include "hdilayer_fuzzer.h"

#include "hdi_layer.h"
using namespace OHOS::Rosen;

namespace OHOS {
    namespace {
        constexpr size_t STR_LEN = 10;
        constexpr char STR_END_CHAR = '\0';
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
        std::memcpy(&object, data_ + pos, objectSize);
        pos += objectSize;
        return object;
    }

    /*
    * get a string from data_
    */
    std::string GetStringFromData(int strlen)
    {
        char cstr[strlen];
        cstr[strlen-1] = STR_END_CHAR;
        for (int i = 0; i < strlen-1; i++) {
            cstr[i] = GetData<char>();
        }
        std::string str(cstr);
        return str;
    }

    std::shared_ptr<HdiLayerInfo> GetLayerInfoFromData()
    {
        // get data
        int32_t zOrder = GetData<int32_t>();
        LayerAlpha alpha = GetData<LayerAlpha>();
        TransformType transformType = GetData<TransformType>();
        CompositionType compositionType = GetData<CompositionType>();
        uint32_t num = GetData<uint32_t>();
        IRect visibleRegion = GetData<IRect>();
        IRect dirtyRegion = GetData<IRect>();
        BlendType blendType = GetData<BlendType>();
        IRect crop = GetData<IRect>();
        bool preMulti = GetData<bool>();
        IRect layerRect = GetData<IRect>();
        void* info = static_cast<void*>(GetStringFromData(STR_LEN).data());
        bool change = GetData<bool>();
        ExtDataHandle handle = GetData<ExtDataHandle>();

        // test
        std::shared_ptr<HdiLayerInfo> layerInfo = HdiLayerInfo::CreateHdiLayerInfo();
        layerInfo->SetZorder(zOrder);
        layerInfo->SetAlpha(alpha);
        layerInfo->SetTransform(transformType);
        layerInfo->SetCompositionType(compositionType);
        layerInfo->SetVisibleRegion(num, visibleRegion);
        layerInfo->SetDirtyRegion(dirtyRegion);
        layerInfo->SetBlendType(blendType);
        layerInfo->SetCropRect(crop);
        layerInfo->SetPreMulti(preMulti);
        layerInfo->SetLayerSize(layerRect);
        layerInfo->SetLayerAdditionalInfo(info);
        layerInfo->SetTunnelHandleChange(change);
        layerInfo->SetTunnelHandle(&handle);

        return layerInfo;
    }

    bool DoSomethingInterestingWithMyAPI(const uint8_t* data, size_t size)
    {
        if (data == nullptr || size < 0) {
            return false;
        }

        // initialize
        data_ = data;
        size_ = size;
        pos = 0;

        // get data
        uint32_t screenId = GetData<uint32_t>();
        bool inUsing = GetData<bool>();
        int64_t timestamp = GetData<int64_t>();
        float matrix = GetData<float>();
        ColorDataSpace colorSpace = GetData<ColorDataSpace>();
        std::vector<HDRMetaData> hdrMetaData;
        for (int i = 0; i < 10; i++) { // add 10 elements to the vector
            HDRMetaData ele = GetData<HDRMetaData>();
            hdrMetaData.push_back(ele);
        }
        HDRMetadataKey key = GetData<HDRMetadataKey>();
        std::vector<uint8_t> metaData;
        for (int i = 0; i < 10; i++) { // add 10 elements to the vector
            uint8_t ele = GetData<uint8_t>();
            metaData.push_back(ele);
        }

        // test
        std::shared_ptr<HdiLayerInfo> layerInfo = GetLayerInfoFromData();
        std::shared_ptr<HdiLayer> hdiLayer = HdiLayer::CreateHdiLayer(screenId);
        hdiLayer->Init(layerInfo);
        hdiLayer->SetLayerStatus(inUsing);
        hdiLayer->UpdateLayerInfo(layerInfo);
        hdiLayer->RecordPresentTime(timestamp);
        hdiLayer->SetLayerColorTransform(&matrix);
        hdiLayer->SetLayerColorDataSpace(colorSpace);
        hdiLayer->SetLayerMetaData(hdrMetaData);
        hdiLayer->SetLayerMetaDataSet(key, metaData);
        sptr<SyncFence> fence = SyncFence::INVALID_FENCE;
        hdiLayer->MergeWithFramebufferFence(fence);
        hdiLayer->MergeWithLayerFence(fence);

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

