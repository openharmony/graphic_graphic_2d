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

#ifndef FRAMEWORKS_ANIMATION_SERVER_SERVER_INCLUDE_TEXTURE_H
#define FRAMEWORKS_ANIMATION_SERVER_SERVER_INCLUDE_TEXTURE_H

#include <cstdint>
#include <string>

#include <egl_surface.h>

class Texture {
public:
    Texture(void *buffer, int32_t width, int32_t height);
    ~Texture();

    void Bind(uint32_t slot = 0) const;
    void Unbind();

    inline int32_t GetWidth() const
    {
        return width_;
    }

    inline int32_t GetHeight() const
    {
        return height_;
    }

private:
    uint32_t rendererID_ = 0;
    void *buffer_ = nullptr;
    int32_t width_ = 0;
    int32_t height_ = 0;
};

#endif // FRAMEWORKS_ANIMATION_SERVER_SERVER_INCLUDE_TEXTURE_H
