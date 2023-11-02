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

#ifndef SKIAVERTICES_H
#define SKIAVERTICES_H

#include "include/core/SkVertices.h"
#include "impl_interface/vertices_impl.h"

namespace OHOS {
namespace Rosen {
namespace Drawing {
class SkiaVertices : public VerticesImpl {
public:
    static inline constexpr AdapterType TYPE = AdapterType::SKIA_ADAPTER;

    SkiaVertices() noexcept;
    explicit SkiaVertices(sk_sp<SkVertices> vertices) noexcept;
    ~SkiaVertices() override {}

    AdapterType GetType() const override
    {
        return AdapterType::SKIA_ADAPTER;
    }

    bool MakeCopy(VertexMode mode,
        int vertexCount, const Point positions[], const Point texs[], const ColorQuad colors[],
        int indexCount, const uint16_t indices[]) override;

    bool MakeCopy(VertexMode mode,
        int vertexCount, const Point positions[], const Point texs[], const ColorQuad colors[]) override;

    const sk_sp<SkVertices> GetVertices() const;

    std::shared_ptr<Data> Serialize() const override;
    bool Deserialize(std::shared_ptr<Data> data) override;

    class SkiaBuilder : public VerticesImpl::BuilderImpl {
    public:
        static inline constexpr AdapterType TYPE = AdapterType::SKIA_ADAPTER;

        SkiaBuilder() noexcept {}
        ~SkiaBuilder() override {}

        AdapterType GetType() const override
        {
            return AdapterType::SKIA_ADAPTER;
        }

        void Init(VertexMode mode, int vertexCount, int indexCount, uint32_t flags) override;
        bool IsValid() override;

        Point* Positions() override;
        uint16_t* Indices() override;
        Point* TexCoords() override;
        ColorQuad* Colors() override;

        std::shared_ptr<VerticesImpl> Detach() override;
    
    private:
        std::shared_ptr<SkVertices::Builder> skiaVerticesBuilder_;
    };

private:
    sk_sp<SkVertices> skiaVertices_;
};
} // namespace Drawing
} // namespace Rosen
} // namespace OHOS
#endif
