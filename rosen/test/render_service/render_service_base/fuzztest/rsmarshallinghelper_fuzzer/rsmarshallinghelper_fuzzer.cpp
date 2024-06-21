/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
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

#include "rsmarshallinghelper_fuzzer.h"

#include <cstddef>
#include <cstdint>
#include <securec.h>

#include "animation/rs_render_particle.h"
#include "render/rs_filter.h"
#include "transaction/rs_marshalling_helper.h"
#include "transaction/rs_transaction_data.h"

namespace OHOS {
namespace Rosen {
namespace {
const uint8_t* g_data = nullptr;
size_t g_size = 0;
size_t g_pos;
} // namespace

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

bool DoUnmarshalling(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    Parcel parcel;
    std::shared_ptr<RSLinearGradientBlurPara> val;
    std::shared_ptr<MotionBlurParam> motionBlurParam;
    std::shared_ptr<EmitterUpdater> emitterUpdater;
    std::vector<std::shared_ptr<EmitterUpdater>> emitterUpdaters;
    std::shared_ptr<ParticleNoiseField> particleNoiseField;
    std::shared_ptr<ParticleNoiseFields> noiseFields;
    std::shared_ptr<ParticleRenderParams> params;
    std::vector<std::shared_ptr<ParticleRenderParams>> particleRenderParams;
    std::shared_ptr<RSPath> path;
    std::shared_ptr<RSFilter> filter;
    std::shared_ptr<RSRenderModifier> modifier;
    std::shared_ptr<RSRenderPropertyBase> property;
    std::shared_ptr<RSImageBase> base = std::make_shared<RSImageBase>();
    std::shared_ptr<RSImage> image = std::make_shared<RSImage>();
    std::shared_ptr<Media::PixelMap> map = std::make_shared<Media::PixelMap>();
    std::shared_ptr<RectT<float>> rectt = std::make_shared<RectT<float>>();
    EmitterConfig emitterConfig;
    ParticleVelocity velocity;
    RenderParticleColorParaType color;
    RRectT<float> rrect;
    bool isMalloc = false;

    float start = GetData<float>();
    float end = GetData<float>();
    Range<float> value(start, end);
    Range<float> random(start, end);
    std::vector<std::shared_ptr<ChangeInOverLife<float>>> valChangeOverLife;
    parcel.WriteInt16(1);
    parcel.WriteUint64(1);
    std::shared_ptr<RSInterpolator> interpolator = RSInterpolator::Unmarshalling(parcel);
    auto changeInOverLife = std::make_shared<ChangeInOverLife<float>>(0.f, 0.f, 0, 0, interpolator);
    valChangeOverLife.push_back(changeInOverLife);
    RenderParticleParaType<float> val2(value, ParticleUpdator::RANDOM, random, std::move(valChangeOverLife));

    filter = RSFilter::CreateBlurFilter(start, end);
    filter->type_ = RSFilter::BLUR;

    RSTransactionData rsTransactionData;
    rsTransactionData.UnmarshallingCommand(parcel);
    RSMarshallingHelper::Unmarshalling(parcel, val);
    RSMarshallingHelper::Unmarshalling(parcel, motionBlurParam);
    RSMarshallingHelper::Unmarshalling(parcel, emitterUpdater);
    RSMarshallingHelper::Unmarshalling(parcel, emitterUpdaters);
    RSMarshallingHelper::Unmarshalling(parcel, particleNoiseField);
    RSMarshallingHelper::Unmarshalling(parcel, noiseFields);
    RSMarshallingHelper::Unmarshalling(parcel, emitterConfig);
    RSMarshallingHelper::Unmarshalling(parcel, velocity);
    RSMarshallingHelper::Unmarshalling(parcel, val2);
    RSMarshallingHelper::Unmarshalling(parcel, color);
    RSMarshallingHelper::Unmarshalling(parcel, params);
    RSMarshallingHelper::Unmarshalling(parcel, particleRenderParams);
    RSMarshallingHelper::Unmarshalling(parcel, path);
    RSMarshallingHelper::Unmarshalling(parcel, filter);
    RSMarshallingHelper::Unmarshalling(parcel, base);
    RSMarshallingHelper::Unmarshalling(parcel, image);
    RSMarshallingHelper::Unmarshalling(parcel, map);
    RSMarshallingHelper::Unmarshalling(parcel, rectt);
    RSMarshallingHelper::Unmarshalling(parcel, rrect);
    RSMarshallingHelper::Unmarshalling(parcel, modifier);
    RSMarshallingHelper::Unmarshalling(parcel, property);
    RSMarshallingHelper::ReadFromParcel(parcel, 1, isMalloc);
    RSMarshallingHelper::ReadFromAshmem(parcel, 1, isMalloc);
    return true;
}
} // namespace Rosen
} // namespace OHOS

/* Fuzzer entry point */
extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size)
{
    /* Run your code on data */
    OHOS::Rosen::DoUnmarshalling(data, size);
    return 0;
}