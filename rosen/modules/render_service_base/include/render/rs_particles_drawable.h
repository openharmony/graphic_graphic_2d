/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
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

#ifndef RENDER_SERVICE_CLIENT_CORE_RENDER_RS_PARTICLE_DRAWABLE_H
#define RENDER_SERVICE_CLIENT_CORE_RENDER_RS_PARTICLE_DRAWABLE_H

#include "animation/rs_render_particle.h"
#include "draw/canvas.h"
#include "image/image.h"
#include "render/rs_image.h"
namespace OHOS {
namespace Rosen {
class RSB_EXPORT RSParticlesDrawable {
public:
    explicit RSParticlesDrawable(const std::vector<std::shared_ptr<RSRenderParticle>>& particles,
        std::vector<std::shared_ptr<RSImage>>& imageVector, size_t imageCount);
    RSParticlesDrawable() = default;
    ~RSParticlesDrawable() = default;
    void Draw(Drawing::Canvas& canvas, std::shared_ptr<RectF> bounds);

private:
    std::shared_ptr<Drawing::Image> MakeCircleImage(int radius);
    Drawing::RSXform MakeRSXform(Vector2f ofs, Vector2f position, float spin, float scale);
    void CaculatePointAtlsArry(
        const std::shared_ptr<RSRenderParticle>& particle, Vector2f position, float opacity, float scale);
    void CaculateImageAtlsArry(Drawing::Canvas& canvas, const std::shared_ptr<RSRenderParticle>& particle,
        Vector2f position, float opacity, float scale);
    void DrawImageFill(Drawing::Canvas& canvas, const std::shared_ptr<RSRenderParticle>& particle,
        Vector2f position, float opacity, float scale);
    void DrawParticles(Drawing::Canvas& canvas);
    void DrawCircle(Drawing::Canvas& canvas);
    void DrawImages(Drawing::Canvas& canvas);
    bool CheckImageNull(std::shared_ptr<Drawing::Image>& image, const std::shared_ptr<Drawing::Image>& drawImage);

    std::vector<std::shared_ptr<RSRenderParticle>> particles_;
    std::shared_ptr<Drawing::Image> circleImage_;
    std::vector<std::shared_ptr<RSImage>> imageVector_;
    size_t imageCount_;

    size_t pointCount_ = 0;
    std::vector<Drawing::RSXform> pointRsxform_;
    std::vector<Drawing::Rect> pointTex_;
    std::vector<Drawing::ColorQuad> pointColors_;

    std::vector<int> count_; // Counts the number of each image
    std::vector<std::vector<Drawing::RSXform>> imageRsxform_;
    std::vector<std::vector<Drawing::Rect>> imageTex_;
    std::vector<std::vector<Drawing::ColorQuad>> imageColors_;
};

} // namespace Rosen
} // namespace OHOS
#endif // RENDER_SERVICE_CLIENT_CORE_RENDER_RS_PARTICLE_DRAWABLE_H