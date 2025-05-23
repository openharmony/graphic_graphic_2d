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
#ifndef FRAMEWORKS_OPENGL_WRAPPER_EGL_WRAPPER_LAYER_H
#define FRAMEWORKS_OPENGL_WRAPPER_EGL_WRAPPER_LAYER_H
#include <string>
#include <vector>
#include <EGL/egl.h>
#include "egl_defs.h"
#include "egl_system_layers_manager.h"

namespace OHOS {
using GetNextLayerAddr = void *(*)(void *, const char *);
using LayerInitFunc = EglWrapperFuncPointer (*)(const void *, GetNextLayerAddr);
using LayerSetupFunc = EglWrapperFuncPointer (*)(const char *, EglWrapperFuncPointer);
using FunctionTable = std::map<std::string, EglWrapperFuncPointer>;

class EglWrapperLayer {
public:
    static EglWrapperLayer& GetInstance();
    ~EglWrapperLayer();
    bool Init(EglWrapperDispatchTable *table);
#if USE_IGRAPHICS_EXTENDS_HOOKS
    bool GetIGraphicsLogicStatus() const { return iGraphicsLogic; }
#endif
    bool InitBundleInfo();

private:
    EglWrapperLayer() : initialized_(false), dlhandle_(nullptr) {};
    bool LoadLayers(void);
    bool LoadLayers(const std::string& libname, const std::vector<std::string> &layerPaths);
    bool DoLoadLayers(const std::vector<std::string>& layers);
    bool LoadLayerFuncs(const std::string& realLayerPath);
    void InitLayers(EglWrapperDispatchTable *table);
    void SetupLayerFuncTbl(EglWrapperDispatchTable *table);

    std::vector<LayerInitFunc> layerInit_;
    std::vector<LayerSetupFunc> layerSetup_;
    std::vector<FunctionTable> layerFuncTbl_;
    bool initialized_;
    void *dlhandle_;
    EglSystemLayersManager eglSystemLayersManager_;
#if USE_IGRAPHICS_EXTENDS_HOOKS
    bool iGraphicsLogic = false;
#endif
};
} // namespace OHOS
#endif // FRAMEWORKS_OPENGL_WRAPPER_EGL_WRAPPER_LAYER_H
