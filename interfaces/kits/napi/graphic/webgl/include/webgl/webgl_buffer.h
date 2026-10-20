/*
 * Copyright (c) 2021-2023 Huawei Device Co., Ltd.
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

#ifndef WEBGL_BUFFER_H
#define WEBGL_BUFFER_H

#include <algorithm>
#include <vector>

#include "napi/n_exporter.h"
#include "webgl_arg.h"
#include "webgl_object.h"

namespace OHOS {
namespace Rosen {
class WebGLBuffer final : public NExporter, public WebGLObject {
public:
    inline static const std::string className = "WebGLBuffer";
    inline static const int objectType = WEBGL_OBJECT_BUFFER;
    inline static const int DEFAULT_BUFFER = 0;

    bool Export(napi_env env, napi_value exports) override;
    std::string GetClassName() override;
    static napi_value Constructor(napi_env env, napi_callback_info info);
    explicit WebGLBuffer() : bufferId_(0), target_(0) {};
    WebGLBuffer(napi_env env, napi_value exports) : NExporter(env, exports), bufferId_(0), target_(0) {};
    ~WebGLBuffer();
    void SetBufferId(uint32_t bufferId)
    {
        bufferId_ = bufferId;
    }

    uint32_t GetBufferId() const
    {
        return bufferId_;
    }

    GLenum GetTarget() const
    {
        return target_;
    }

    void SetTarget(GLenum target)
    {
        target_ = target;
    }

    static NVal CreateObjectInstance(napi_env env, WebGLBuffer** instance)
    {
        return WebGLObject::CreateObjectInstance<WebGLBuffer>(env, instance);
    }

    void SetBuffer(size_t bufferSize, const uint8_t* data)
    {
        bufferSize_ = bufferSize;
        indexData_.clear();
        if (target_ != GL_ELEMENT_ARRAY_BUFFER || bufferSize == 0) {
            return;
        }
        if (data == nullptr) {
            indexData_.resize(bufferSize, 0);
        } else {
            indexData_.assign(data, data + bufferSize);
        }
    }

    bool UpdateBuffer(size_t offset, const uint8_t* data, size_t size)
    {
        if (target_ != GL_ELEMENT_ARRAY_BUFFER) {
            return true;
        }
        if (offset > indexData_.size() || size > indexData_.size() - offset || (size > 0 && data == nullptr)) {
            return false;
        }
        if (size == 0) {
            return true;
        }
        std::copy(data, data + size, indexData_.begin() + offset);
        return true;
    }

    const uint8_t* GetIndexData() const
    {
        return indexData_.empty() ? nullptr : indexData_.data();
    }

    size_t GetBufferSize() const
    {
        return bufferSize_;
    }
private:
    uint32_t bufferId_ { 0 };
    GLenum target_ { 0 };
    size_t bufferSize_ { 0 };
    std::vector<uint8_t> indexData_ {};
};
} // namespace Rosen
} // namespace OHOS
#endif // WEBGL_BUFFER_H
