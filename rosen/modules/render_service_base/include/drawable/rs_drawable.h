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

#ifndef RENDER_SERVICE_BASE_PROPERTY_RS_DRAWABLE_H
#define RENDER_SERVICE_BASE_PROPERTY_RS_DRAWABLE_H

namespace OHOS::Rosen {
class RSPaintFilterCanvas;

// Pure virtual base class
class RSDrawable {
public:
    RSDrawable() = default;
    virtual ~RSDrawable() = default;

    // not copyable and moveable
    RSDrawable(const RSDrawable&) = delete;
    RSDrawable(const RSDrawable&&) = delete;
    RSDrawable& operator=(const RSDrawable&) = delete;
    RSDrawable& operator=(const RSDrawable&&) = delete;

    //
    virtual void Draw(RSPaintFilterCanvas& canvas) const = 0;
};

// class RSCustomModifierDrawableNG : RSDrawable {
// public:
//     RSCustomModifierDrawableNG(const std::shared_ptr<RSRenderModifier>& modifier)
//     {
//         // auto property = modifier->GetProperty();
//         // if (auto drawCmdProperty = std::static_pointer_cast<RSRenderProperty<Drawing::DrawCmdListPtr>>(property))
//         {
//         //     drawCmdList_ = drawCmdProperty->GetRef();
//         // }
//     }
//     ~RSCustomModifierDrawableNG() override = default;

//     //Generate
//     //Update

//     void OnSync()
//     {
//         if (stagingDrawCmdLists_.empty()) {
//             return;
//         }
//         std::swap(drawCmdLists_, stagingDrawCmdLists_);
//         stagingDrawCmdLists_.clear();
//     }

//     void Draw(RSPaintFilterCanvas& canvas) const override {}

// protected:
//     std::vector<std::shared_ptr<Drawing::DrawCmdList>> drawCmdLists_;
//     std::vector<std::shared_ptr<Drawing::DrawCmdList>> stagingDrawCmdLists_;
// };

} // namespace OHOS::Rosen
#endif // RENDER_SERVICE_BASE_PROPERTY_RS_DRAWABLE_H
