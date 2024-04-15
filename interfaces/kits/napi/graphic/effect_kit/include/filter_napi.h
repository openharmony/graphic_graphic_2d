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

#ifndef FILTER_NAPI_H
#define FILTER_NAPI_H

#include <memory>
#include <mutex>
#include <vector>
#include "pixel_map_napi.h"
#include "include/core/SkImageFilter.h"

namespace OHOS {
namespace Rosen {
static std::mutex getPixelMapAsyncExecuteMutex_;
static std::mutex getPixelMapAsyncCompleteMutex_;
class FilterNapi {
public:
    FilterNapi();
    ~FilterNapi();
    static napi_value Init(napi_env env, napi_value exports);
    std::shared_ptr<Media::PixelMap> GetDstPixelMap();
private:
    static thread_local napi_ref sConstructor_;
    static void Destructor(napi_env env, void* nativeObject, void* finalize_hint);
    static void GetPixelMapAsyncExecute(napi_env env, void* data);
    static void GetPixelMapAsyncComplete(napi_env env, napi_status status, void* data);
    static napi_value CreateEffect(napi_env env, napi_callback_info info);
    static napi_value Constructor(napi_env env, napi_callback_info info);
    static napi_value GetPixelMap(napi_env env, napi_callback_info info);
    static napi_value GetPixelMapAsync(napi_env env, napi_callback_info info);
    static napi_value Blur(napi_env env, napi_callback_info info);
    static napi_value Brightness(napi_env env, napi_callback_info info);
    static napi_value Grayscale(napi_env env, napi_callback_info info);
    static napi_value Invert(napi_env env, napi_callback_info info);
    static napi_value SetColorMatrix(napi_env env, napi_callback_info info);
    void AddNextFilter(sk_sp<SkImageFilter> filter);
    void Render(bool forceCPU);
    std::vector<sk_sp<SkImageFilter> > skFilters_;
    std::shared_ptr<Media::PixelMap> srcPixelMap_ = nullptr;
    std::shared_ptr<Media::PixelMap> dstPixelMap_ = nullptr;
    napi_env env_;
    napi_ref wrapper_;
};
}
}
#endif // FILTER_NAPI_H
