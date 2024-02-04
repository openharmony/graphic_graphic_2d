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

#include "utils/vertices.h"
#include "impl_factory.h"
#include "utils/log.h"

namespace OHOS {
namespace Rosen {
namespace Drawing {
Vertices::Vertices() noexcept : verticesImplPtr_(ImplFactory::CreateVerticesImpl()) {}

Vertices::Vertices(std::shared_ptr<VerticesImpl> vertices) noexcept
    : verticesImplPtr_(vertices) {}

bool Vertices::MakeCopy(VertexMode mode,
    int vertexCount, const Point positions[], const Point texs[], const ColorQuad colors[],
    int indexCount, const uint16_t indices[])
{
    if (verticesImplPtr_ == nullptr) {
        return false;
    }
    return verticesImplPtr_->MakeCopy(mode, vertexCount, positions, texs, colors,
        indexCount, indices);
}

bool Vertices::MakeCopy(VertexMode mode,
    int vertexCount, const Point positions[], const Point texs[], const ColorQuad colors[])
{
    if (verticesImplPtr_ == nullptr) {
        return false;
    }
    return verticesImplPtr_->MakeCopy(mode, vertexCount, positions, texs, colors);
}

std::shared_ptr<Data> Vertices::Serialize() const
{
    return verticesImplPtr_->Serialize();
}

bool Vertices::Deserialize(std::shared_ptr<Data> data)
{
    return verticesImplPtr_->Deserialize(data);
}

Vertices::Builder::Builder(VertexMode mode, int vertexCount, int indexCount, uint32_t flags)
    : builderImplPtr_(ImplFactory::CreateVerticesBuilderImpl())
{
    builderImplPtr_->Init(mode, vertexCount, indexCount, flags);
}

bool Vertices::Builder::IsValid()
{
    return builderImplPtr_->IsValid();
}

Point* Vertices::Builder::Positions()
{
    return builderImplPtr_->Positions();
}

uint16_t* Vertices::Builder::Indices()
{
    return builderImplPtr_->Indices();
}

Point* Vertices::Builder::TexCoords()
{
    return builderImplPtr_->TexCoords();
}

ColorQuad* Vertices::Builder::Colors()
{
    return builderImplPtr_->Colors();
}

std::shared_ptr<Vertices> Vertices::Builder::Detach()
{
    return std::make_shared<Vertices>(builderImplPtr_->Detach());
}

} // namespace Drawing
} // namespace Rosen
} // namespace OHOS