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

#include "mesh.h"

namespace OHOS {
namespace Rosen {
Mesh::Mesh()
{
    float vertices[] = {
     1.0f,  1.0f, 0.0f,    1.0f, 1.0f,  // top right
     1.0f, -1.0f, 0.0f,    1.0f, 0.0f,  // bottom right
    -1.0f, -1.0f, 0.0f,    0.0f, 0.0f,  // bottom left
    -1.0f,  1.0f, 0.0f,    0.0f, 1.0f   // top left 
    };
    unsigned int indices[] = {
        0, 1, 3,  // first triangle
        1, 2, 3   // second triangle
    };
    verticesSize_ = sizeof(vertices);
    indicesSize_ = sizeof(indices);
    verticesNumber_ = sizeof(vertices) / sizeof(float);
    indicesNumber_ = sizeof(indices) / sizeof(unsigned int);

    mVertices_ = new float[verticesNumber_];
    mIndices_ = new unsigned int[indicesNumber_];
    for (int i = 0; i < verticesNumber_; i++) {
        mVertices_[i] = vertices[i];
    }
    for (int i = 0; i < indicesNumber_; i++) {
        mIndices_[i] = indices[i];
    }
    glGenVertexArrays(1, &VAO_);
    glGenBuffers(1, &VBO_);
    glGenBuffers(1, &EBO_);
}

Mesh::~Mesh()
{
    delete[] mVertices_;
    delete[] mIndices_;
    Delete();
}

void Mesh::Use()
{
    glBindVertexArray(VAO_);

    glBindBuffer(GL_ARRAY_BUFFER, VBO_);
    glBufferData(GL_ARRAY_BUFFER, verticesSize_, mVertices_, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO_);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indicesSize_, mIndices_, GL_STATIC_DRAW);

    glVertexAttribPointer(0, DEFAULT_VERTEX_POINT_SIZE, GL_FLOAT, GL_FALSE, DEFAULT_STRIDE * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    // color attribute
    glVertexAttribPointer(1, DEFAULT_TEXTURE_POINT_SIZE, GL_FLOAT, GL_FALSE,
        DEFAULT_STRIDE * sizeof(float), (void*)(DEFAULT_OFFSET * sizeof(float)));
    glEnableVertexAttribArray(1);
}

void Mesh::Delete()
{
    glDeleteVertexArrays(1, &VAO_);
    glDeleteBuffers(1, &VBO_);
    glDeleteBuffers(1, &EBO_);
}
} // namespcae Rosen
} // namespace OHOS