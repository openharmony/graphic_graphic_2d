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

#ifndef VERTICESIMPL_H
#define VERTICESIMPL_H

#include "base_impl.h"

#include "utils/data.h"
#include "utils/point.h"
#include "draw/color.h"

namespace OHOS {
namespace Rosen {
namespace Drawing {

enum class VertexMode;

class VerticesImpl : public BaseImpl {
public:
    VerticesImpl() noexcept {}
    ~VerticesImpl() override {}

    virtual bool MakeCopy(VertexMode mode,
        int vertexCount, const Point positions[], const Point texs[], const ColorQuad colors[],
        int indexCount, const uint16_t indices[]) = 0;

    virtual bool MakeCopy(VertexMode mode,
        int vertexCount, const Point positions[], const Point texs[], const ColorQuad colors[]) = 0;

    virtual std::shared_ptr<Data> Serialize() const = 0;
    virtual bool Deserialize(std::shared_ptr<Data> data) = 0;

    class BuilderImpl : public BaseImpl {
    public:
        BuilderImpl() noexcept {}
        ~BuilderImpl() override {}

        virtual void Init(VertexMode mode, int vertexCount, int indexCount, uint32_t flags) = 0;
        virtual bool IsValid() = 0;

        virtual Point* Positions() = 0;
        virtual uint16_t* Indices() = 0;
        virtual Point* TexCoords() = 0;
        virtual ColorQuad* Colors() = 0;

        virtual std::shared_ptr<VerticesImpl> Detach() = 0;
    };
};
} // namespace Drawing
} // namespace Rosen
} // namespace OHOS
#endif
