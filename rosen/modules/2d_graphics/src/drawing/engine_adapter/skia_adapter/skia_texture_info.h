/*
 * Copyright (c) 2023 Huawei Device Co., Ltd.
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

#ifndef SKIA_TEXTURE_INFO_H
#define SKIA_TEXTURE_INFO_H
#ifdef RS_ENABLE_GPU
#ifdef USE_M133_SKIA
#include "include/gpu/ganesh/GrBackendSurface.h"
#include "include/gpu/ganesh/gl/GrGLBackendSurface.h"
#ifdef RS_ENABLE_VK
#include "include/gpu/ganesh/vk/GrVkTypes.h"
#include "include/gpu/ganesh/vk/GrVkBackendSurface.h"
#endif
#else
#include "include/gpu/GrBackendSurface.h"
#endif
#endif

#include "image/image.h"

namespace OHOS {
namespace Rosen {
namespace Drawing {
class SkiaTextureInfo {
public:
    static GrSurfaceOrigin ConvertToGrSurfaceOrigin(const TextureOrigin& origin)
    {
        switch (origin) {
            case TextureOrigin::TOP_LEFT:
                return GrSurfaceOrigin::kTopLeft_GrSurfaceOrigin;
            case TextureOrigin::BOTTOM_LEFT:
                return GrSurfaceOrigin::kBottomLeft_GrSurfaceOrigin;
            default:
                return GrSurfaceOrigin::kTopLeft_GrSurfaceOrigin;
        }
    }
#ifdef RS_ENABLE_VK
    static GrBackendTexture ConvertToGrBackendVKTexture(const TextureInfo& info);
    
    static void ConvertToVKTexture(const GrBackendTexture& backendTexture, TextureInfo& info);
#endif
#ifdef RS_ENABLE_GPU
    static TextureInfo ConvertToTextureInfo(const GrBackendTexture& grBackendTexture);

    static GrBackendTexture ConvertToGrBackendTexture(const TextureInfo& info);
#endif
};
} // namespace Drawing
} // namespace Rosen
} // namespace OHOS
#endif // SKIA_TEXTURE_INFO_H