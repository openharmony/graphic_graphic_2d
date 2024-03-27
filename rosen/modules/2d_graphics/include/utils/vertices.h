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

#ifndef VERTICES_H
#define VERTICES_H

#include "drawing/engine_adapter/impl_interface/vertices_impl.h"

namespace OHOS {
namespace Rosen {
namespace Drawing {

enum class VertexMode {
    TRIANGLES_VERTEXMODE,
    TRIANGLESSTRIP_VERTEXMODE,
    TRIANGLEFAN_VERTEXMODE,

    LAST_VERTEXMODE = TRIANGLEFAN_VERTEXMODE,
};

enum class BuilderFlags {
    HAS_TEXCOORDS_BUILDER_FLAG = 1 << 0,
    HAS_COLORS_BUILDER_FLAG = 1 << 1,
};

class Vertices {
public:
    Vertices() noexcept;
    explicit Vertices(std::shared_ptr<VerticesImpl>) noexcept;
    virtual ~Vertices() {};

    /**
     * @brief Make a copy from vertices data.
     * @param mode         vertex mode.
     * @param vertexCount  Vertex count.
     * @param positions    Positions data pointer.
     * @param tex          Texture coordinate data pointer.
     * @param colors       Color data pointer.
     * @param indexCount   Index count.
     * @param indices      Index data pointer.
     * @return             Returns true if MakeCopy succeed.
     */
    bool MakeCopy(VertexMode mode,
        int vertexCount, const Point positions[], const Point texs[], const ColorQuad colors[],
        int indexCount, const uint16_t indices[]);

    /**
     * @brief Make a copy from vertices data.
     * @param mode         vertex mode.
     * @param vertexCount  Vertex count.
     * @param positions    Positions data pointer.
     * @param tex          Texture coordinate data pointer.
     * @param colors       Color data pointer.
     * @return             Returns true if MakeCopy succeed.
     */
    bool MakeCopy(VertexMode mode,
        int vertexCount, const Point positions[], const Point texs[], const ColorQuad colors[]);

    /**
     * @brief Get the adaptation layer instance, called in the adaptation layer.
     * @return  Adaptation Layer instance.
     */
    template<typename T>
    T* GetImpl() const
    {
        if (verticesImplPtr_ == nullptr) {
            return nullptr;
        }
        return verticesImplPtr_->DowncastingTo<T>();
    }
    std::shared_ptr<Data> Serialize() const;
    bool Deserialize(std::shared_ptr<Data> data);
    class Builder {
    public:
        Builder(VertexMode mode, int vertexCount, int indexCount, uint32_t flags);
        virtual ~Builder() {};

        /**
         * @brief Return if builder is valid.
         */
        bool IsValid();

        /**
         * @brief Return positions data pointer in the builder.
         */
        Point* Positions();

        /**
         * @brief Return indices data pointer in the builder.
         */
        uint16_t* Indices();

        /**
         * @brief Return texture coordinate data pointer in the builder.
         */
        Point* TexCoords();

        /**
         * @brief Return color data pointer in theBuilder.
         */
        ColorQuad* Colors();

        /**
         * @brief Detach the built vertices object. After the first call, this will always return null.
         * @return Return a shared pointer of Vertices object or nullptr.
         */
        std::shared_ptr<Vertices> Detach();

    private:
        std::shared_ptr<VerticesImpl::BuilderImpl> builderImplPtr_;
    };

private:
    std::shared_ptr<VerticesImpl> verticesImplPtr_;
};
} // namespace Drawing
} // namespace Rosen
} // namespace OHOS
#endif
