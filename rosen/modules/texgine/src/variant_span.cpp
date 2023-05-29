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

#include "variant_span.h"

#include "texgine_exception.h"
#include "texgine/utils/exlog.h"
#include "text_converter.h"

namespace OHOS {
namespace Rosen {
namespace TextEngine {
VariantSpan::VariantSpan(const std::shared_ptr<TextSpan> &ts) noexcept(true)
{
    ts_ = ts;
}

VariantSpan::VariantSpan(const std::shared_ptr<AnySpan> &as) noexcept(true)
{
    as_ = as;
}

VariantSpan::VariantSpan(std::shared_ptr<TextSpan> &&ts) noexcept(true)
{
    ts_ = std::move(ts);
}

VariantSpan::VariantSpan(std::shared_ptr<AnySpan> &&as) noexcept(true)
{
    as_ = std::move(as);
}

VariantSpan::VariantSpan(std::nullptr_t) noexcept(true)
{
}

VariantSpan::operator bool() const noexcept(false)
{
    CheckPointer(true);
    return *this != nullptr;
}

bool VariantSpan::operator ==(std::nullptr_t) const noexcept(false)
{
    CheckPointer(true);
    return (ts_ == nullptr) && (as_ == nullptr);
}

bool VariantSpan::operator ==(const VariantSpan &rhs) const noexcept(false)
{
    CheckPointer(true);
    return (ts_ == rhs.ts_) && (as_ == rhs.as_);
}

bool VariantSpan::operator !=(std::nullptr_t) const noexcept(false)
{
    CheckPointer(true);
    return !(*this == nullptr);
}

bool VariantSpan::operator !=(const VariantSpan &rhs) const noexcept(false)
{
    CheckPointer(true);
    return !(*this == rhs);
}

double VariantSpan::GetWidth() const noexcept(false)
{
    CheckPointer();
    if (as_) {
        return as_->GetWidth();
    }

    if (ts_) {
        return ts_->GetWidth();
    }

    return 0.0;
}

double VariantSpan::GetHeight() const noexcept(false)
{
    CheckPointer();
    if (as_) {
        return as_->GetHeight();
    }

    if (ts_) {
        return ts_->GetHeight();
    }

    return 0.0;
}

size_t VariantSpan::GetNumberOfCharGroup() const noexcept(false)
{
    CheckPointer();
    if (ts_) {
        return ts_->cgs_.GetNumberOfCharGroup();
    }

    if (as_) {
        return 1;
    }

    return 0;
}

std::vector<double> VariantSpan::GetGlyphWidths() const noexcept(false)
{
    CheckPointer();
    std::vector<double> widths;
    if (ts_) {
        for (const auto &cg : ts_->cgs_) {
            widths.push_back(cg.GetWidth());
        }
    }

    if (as_) {
        widths.push_back(GetWidth());
    }

    return widths;
}

double VariantSpan::GetVisibleWidth() const noexcept(false)
{
    CheckPointer();
    if (ts_) {
        double width = 0;
        double continuousInvisibleWidth = 0;
        for (const auto &cg : ts_->cgs_) {
            width += (cg.visibleWidth + cg.invisibleWidth);
            if (cg.visibleWidth > 0) {
                continuousInvisibleWidth = 0;
            }
            continuousInvisibleWidth += cg.invisibleWidth;
        }
        return width - continuousInvisibleWidth;
    }

    if (as_) {
        return as_->GetWidth();
    }

    return 0.0;
}

void VariantSpan::Dump(const DumpType &dtype) const noexcept(false)
{
    CheckPointer(true);
    if (as_) {
        switch (dtype) {
            case DumpType::NORMAL:
                LOGEX_FUNC_LINE_DEBUG() << "VariantSpan";
                break;
            case DumpType::DONT_RETURN:
                LOGEX_FUNC_LINE_DEBUG(Logger::SetToNoReturn) << "VariantSpan ";
                break;
        }
        return;
    }

    if (ts_ == nullptr) {
        return;
    }

    switch (dtype) {
        case DumpType::NORMAL:
            LOGEX_FUNC_LINE_DEBUG() << "(" << offsetX_ << ", " << offsetY_ << ") " <<
                (ts_->rtl_ ? "<-" : "->") << " " << ts_->cgs_.GetRange() <<
                ": '\033[40m" << TextConverter::ToStr(ts_->cgs_.ToUTF16()) << "\033[0m'";
            break;
        case DumpType::DONT_RETURN:
            LOGEX_FUNC_LINE_DEBUG(Logger::SetToNoReturn) << "(" << offsetX_ << ", " << offsetY_ << ") "<<
                (ts_->rtl_ ? "<-" : "->") << " " << ts_->cgs_.GetRange() <<
                ": '\033[40m" << TextConverter::ToStr(ts_->cgs_.ToUTF16()) << "\033[0m'" << " ";
            break;
    }
}

std::shared_ptr<TextSpan> VariantSpan::TryToTextSpan() const noexcept(false)
{
    CheckPointer(true);
    return ts_;
}

std::shared_ptr<AnySpan> VariantSpan::TryToAnySpan() const noexcept(false)
{
    CheckPointer(true);
    return as_;
}

void VariantSpan::SetTextStyle(const TextStyle &xs) noexcept(true)
{
    xs_ = std::move(xs);
}

TextStyle &VariantSpan::GetTextStyle() noexcept(true)
{
    return xs_;
}

const TextStyle &VariantSpan::GetTextStyle() const noexcept(true)
{
    return xs_;
}

double VariantSpan::GetOffsetX() const noexcept(true)
{
    return offsetX_;
}

double VariantSpan::GetOffsetY() const noexcept(true)
{
    return offsetY_;
}

void VariantSpan::AdjustOffsetX(double offset) noexcept(true)
{
    offsetX_ += offset;
}

void VariantSpan::AdjustOffsetY(double offset) noexcept(true)
{
    offsetY_ += offset;
}

void VariantSpan::Paint(TexgineCanvas &canvas, double offsetx, double offsety) noexcept(false)
{
    CheckPointer();
    if (as_) {
        as_->Paint(canvas, offsetx, offsety);
    }

    if (ts_) {
        ts_->Paint(canvas, offsetx, offsety, xs_);
    }
}

void VariantSpan::PaintShadow(TexgineCanvas &canvas, double offsetx, double offsety) noexcept(false)
{
    CheckPointer();
    if (ts_) {
        ts_->PaintShadow(canvas, offsetx, offsety, xs_.shadows);
    }
}

bool VariantSpan::IsRTL() const noexcept(false)
{
    CheckPointer();
    if (ts_) {
        return ts_->IsRTL();
    }

    return false;
}

void VariantSpan::CheckPointer(bool nullable) const noexcept(false)
{
    if (!nullable && as_ == nullptr && ts_ == nullptr) {
        throw TEXGINE_EXCEPTION(NULLPTR);
    }

    if (as_ != nullptr && ts_ != nullptr) {
        throw TEXGINE_EXCEPTION(ERROR_STATUS);
    }
}
} // namespace TextEngine
} // namespace Rosen
} // namespace OHOS
