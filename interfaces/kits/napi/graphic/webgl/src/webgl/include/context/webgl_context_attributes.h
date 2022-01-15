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

#ifndef ROSENRENDER_ROSEN_WEBGL_CONTEXT_ATTRIBUTES
#define ROSENRENDER_ROSEN_WEBGL_CONTEXT_ATTRIBUTES

#include <string>

namespace OHOS {
namespace Rosen {
class WebGLContextAttributes {
public:
    bool alpha = true;
    bool antialias = true;
    bool depth = true;
    bool failIfMajorPerformanceCaveat = false;
    bool desynchronized = false;
    bool premultipliedAlpha = true;
    bool preserveDrawingBuffer = false;
    std::string powerPreference = "default";
    bool stencil = false;
    WebGLContextAttributes() = default;
    virtual ~WebGLContextAttributes() {}
};
} // namespace Rosen
} // namespace OHOS

#endif // ROSENRENDER_ROSEN_WEBGL_CONTEXT_ATTRIBUTES
