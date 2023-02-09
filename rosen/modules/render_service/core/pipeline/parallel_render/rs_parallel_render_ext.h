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

#ifndef RENDER_SERVICE_CORE_PIPELINE_PARALLEL_RENDER_RS_PARALLEL_RENDER_EXT_H
#define RENDER_SERVICE_CORE_PIPELINE_PARALLEL_RENDER_RS_PARALLEL_RENDER_EXT_H
#include <cstdint>
#include <dlfcn.h>
namespace OHOS {
namespace Rosen {
class RSParallelRenderExt {
public:
    ~RSParallelRenderExt()
    {
        CloseParallelRenderExt();
    }

    static bool OpenParallelRenderExt()
    {
        if (parallelRenderExtHandle_) {
            return true;
        }
        parallelRenderExtHandle_ = dlopen("libparallel_render.z.so", RTLD_NOW);
        if (parallelRenderExtHandle_ == nullptr) {
            return false;
        }
        return GetParallelRenderEnable();
    }

    static void CloseParallelRenderExt()
    {
        if (parallelRenderExtHandle_) {
            dlclose(parallelRenderExtHandle_);
            FreeFuncHandle();
        }
    }

    static inline void* parallelRenderExtHandle_ = nullptr;
    static inline void* initParallelRenderLBFunc_ = nullptr;
    static inline void* setSubRenderThreadNumFunc_ = nullptr;
    static inline void* addRenderLoadFunc_ = nullptr;
    static inline void* updateLoadCostFunc_ = nullptr;
    static inline void* loadBalancingFunc_ = nullptr;
    static inline void* clearRenderLoadFunc_ = nullptr;
    static inline void* freeParallelRenderLBFunc_ = nullptr;
    static inline void* setCoreLevelFunc_ = nullptr;

private:
    RSParallelRenderExt() = default;
    
    static bool GetParallelRenderEnable()
    {
        return GetInitParallelRenderLoadBalancingFunc() &&
            GetSetSubRenderThreadNumFunc() &&
            GetAddRenderLoadFunc() &&
            GetUpdateLoadCostFunc() &&
            GetLoadBalancingFunc() &&
            GetClearRenderLoadFunc() &&
            GetFreeParallelRenderLoadBalancing() &&
            GetSetCoreLevelFunc();
    }

    static bool GetInitParallelRenderLoadBalancingFunc()
    {
        initParallelRenderLBFunc_ = dlsym(parallelRenderExtHandle_, "InitParallelRenderLoadBalancing");
        return initParallelRenderLBFunc_ != nullptr;
    }

    static bool GetSetSubRenderThreadNumFunc()
    {
        setSubRenderThreadNumFunc_ = dlsym(parallelRenderExtHandle_, "SetSubRenderThreadNum");
        return setSubRenderThreadNumFunc_ != nullptr;
    }

    static bool GetAddRenderLoadFunc()
    {
        addRenderLoadFunc_ = dlsym(parallelRenderExtHandle_, "AddRenderLoad");
        return addRenderLoadFunc_ != nullptr;
    }

    static bool GetUpdateLoadCostFunc()
    {
        updateLoadCostFunc_ = dlsym(parallelRenderExtHandle_, "UpdateLoadCost");
        return updateLoadCostFunc_ != nullptr;
    }

    static bool GetLoadBalancingFunc()
    {
        loadBalancingFunc_ = dlsym(parallelRenderExtHandle_, "LoadBalancing");
        return loadBalancingFunc_ != nullptr;
    }

    static bool GetClearRenderLoadFunc()
    {
        clearRenderLoadFunc_ = dlsym(parallelRenderExtHandle_, "ClearRenderLoad");
        return clearRenderLoadFunc_ != nullptr;
    }

    static bool GetFreeParallelRenderLoadBalancing()
    {
        freeParallelRenderLBFunc_ = dlsym(parallelRenderExtHandle_, "FreeParallelRenderLoadBalancing");
        return freeParallelRenderLBFunc_ != nullptr;
    }

    static bool GetSetCoreLevelFunc()
    {
        setCoreLevelFunc_ = dlsym(parallelRenderExtHandle_, "SetCoreLevel");
        return setCoreLevelFunc_ != nullptr;
    }

    static void FreeFuncHandle()
    {
        parallelRenderExtHandle_ = nullptr;
        initParallelRenderLBFunc_ = nullptr;
        setSubRenderThreadNumFunc_ = nullptr;
        addRenderLoadFunc_ = nullptr;
        updateLoadCostFunc_ = nullptr;
        loadBalancingFunc_ = nullptr;
        clearRenderLoadFunc_ = nullptr;
        freeParallelRenderLBFunc_ = nullptr;
        setCoreLevelFunc_ = nullptr;
    }
};

} // namespace Rosen
} // namespace OHOS
#endif // RENDER_SERVICE_CORE_PIPELINE_PARALLEL_RENDER_RS_PARALLEL_RENDER_EXT_H