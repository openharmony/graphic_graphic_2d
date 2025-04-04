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

#include <securec.h>
#include <surface_tunnel_handle.h>
#include "hdi_layer.h"
using namespace OHOS::Rosen;

namespace OHOS {
    namespace {
        constexpr size_t STR_LEN = 10;
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

    std::shared_ptr<HdiLayerInfo> GetLayerInfoFromData()
    {
        // get data
        int32_t zOrder = GetData<int32_t>();
        GraphicLayerAlpha alpha = GetData<GraphicLayerAlpha>();
        GraphicTransformType transformType = GetData<GraphicTransformType>();
        GraphicCompositionType compositionType = GetData<GraphicCompositionType>();
        GraphicIRect visibleRegion = GetData<GraphicIRect>();
        std::vector<GraphicIRect> visibleRegions;
        visibleRegions.emplace_back(visibleRegion);
        GraphicIRect dirtyRegion = GetData<GraphicIRect>();
        std::vector<GraphicIRect> dirtyRegions;
        dirtyRegions.emplace_back(dirtyRegion);
        GraphicBlendType blendType = GetData<GraphicBlendType>();
        GraphicIRect crop = GetData<GraphicIRect>();
        bool preMulti = GetData<bool>();
        GraphicIRect layerRect = GetData<GraphicIRect>();
        bool change = GetData<bool>();

        // test
        std::shared_ptr<HdiLayerInfo> layerInfo = HdiLayerInfo::CreateHdiLayerInfo();
        layerInfo->SetZorder(zOrder);
        layerInfo->SetAlpha(alpha);
        layerInfo->SetTransform(transformType);
        layerInfo->SetCompositionType(compositionType);
        layerInfo->SetVisibleRegions(visibleRegions);
        layerInfo->SetDirtyRegions(dirtyRegions);
        layerInfo->SetBlendType(blendType);
        layerInfo->SetCropRect(crop);
        layerInfo->SetPreMulti(preMulti);
        layerInfo->SetLayerSize(layerRect);
        layerInfo->SetTunnelHandleChange(change);
        return layerInfo;
    }

    void HdiLayerFuzzTest001()
    {
        // get data
        GraphicCompositionType type = GetData<GraphicCompositionType>();
        uint32_t screenId = GetData<uint32_t>();

        // test
        std::shared_ptr<HdiLayer> hdiLayer = HdiLayer::CreateHdiLayer(screenId);
        std::shared_ptr<HdiLayerInfo> layerInfo = nullptr;
        sptr<SyncFence> fence = nullptr;
        Rosen::HdiDevice* hdiDeviceMock = nullptr;

        hdiLayer->Init(layerInfo);
        hdiLayer->UpdateLayerInfo(layerInfo);
        hdiLayer->MergeWithFramebufferFence(fence);
        hdiLayer->MergeWithLayerFence(fence);
        hdiLayer->UpdateCompositionType(type);
        hdiLayer->SetHdiDeviceMock(hdiDeviceMock);
        hdiLayer->SetReleaseFence(fence);
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
        GraphicCompositionType type = GetData<GraphicCompositionType>();
        std::string result = GetStringFromData(STR_LEN);
        uint32_t sequence = GetData<uint32_t>();
        uint32_t index = GetData<uint32_t>();
        uint32_t deleting = GetData<uint32_t>();
        std::vector<uint32_t> deletingList = { deleting };
        std::string windowName = GetStringFromData(STR_LEN);
        bool doLayerInfoCompare = GetData<bool>();

        // test
        std::shared_ptr<HdiLayerInfo> layerInfo = GetLayerInfoFromData();
        std::shared_ptr<HdiLayer> hdiLayer = HdiLayer::CreateHdiLayer(screenId);
        Rosen::HdiDevice* hdiDeviceMock = Rosen::HdiDevice::GetInstance();
        hdiLayer->doLayerInfoCompare_ = doLayerInfoCompare;
        hdiLayer->Init(layerInfo);
        hdiLayer->SetLayerStatus(inUsing);
        hdiLayer->UpdateLayerInfo(layerInfo);
        hdiLayer->RecordPresentTime(timestamp);
        sptr<SyncFence> fence = SyncFence::INVALID_FENCE;
        hdiLayer->MergeWithFramebufferFence(fence);
        hdiLayer->MergeWithLayerFence(fence);
        hdiLayer->UpdateCompositionType(type);
        hdiLayer->SavePrevLayerInfo();
        hdiLayer->SetLayerCrop();
        hdiLayer->SetLayerVisibleRegion();
        hdiLayer->SetHdiDeviceMock(hdiDeviceMock);
        hdiLayer->CheckAndUpdateLayerBufferCahce(sequence, index, deletingList);
        hdiLayer->SetLayerMetaData();
        hdiLayer->SetLayerMetaDataSet();
        hdiLayer->SetReleaseFence(fence);
        hdiLayer->SelectHitchsInfo(windowName, result);
        hdiLayer->RecordMergedPresentTime(timestamp);
        hdiLayer->Dump(result);
        hdiLayer->DumpByName(windowName, result);
        hdiLayer->DumpMergedResult(result);
        hdiLayer->ClearDump();

        HdiLayerFuzzTest001();
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

