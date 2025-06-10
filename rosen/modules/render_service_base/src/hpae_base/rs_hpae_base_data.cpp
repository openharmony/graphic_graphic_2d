#include "hpae_base/rs_hpae_base_data.h"
#include "hpae_base/rs_hpae_log.h"

namespace OHOS::Rosen {
RSHpaeBaseData &RSHpaeBaseData::GetInstance()
{
    static RSHpaeBaseData hpaeBaseDataInstance;
    return hpaeBaseDataInstance;
}

RSHpaeBaseData::RSHpaeBaseData()
{
}

RSHpaeBaseData::~RSHpaeBaseData() noexcept
{
}

void RSHpaeBaseData::Reset()
{
    // TODO: clear buffer queue
}

void RSHpaeBaseData::SyncHpaeStatus(HpaeStatus status)
{
    hpaeStatus_ = status;
}

void RSHpaeBaseData::SetHpaeInputBuffer(HpaeBufferInfo& inputBuffer)
{
    inputBufferQueue_.clear();
    inputBufferQueue_.push_back(inputBuffer);
}

void RSHpaeBaseData::SetHpaeOutputBuffer(HpaeBufferInfo& outputBuffer)
{
    outputBufferQueue_.clear();
    outputBufferQueue_.push_back(outputBuffer);
}

// on process
// 从inputBufferCache返回一块1/4 ion buffer,用于作为GreyAndStretch的输出和AAEBlur的输入
// inputBufferCache数据来源于render_service.so
// 可考虑Buffer的轮转
// 考虑好Buffer的释放
HpaeBufferInfo RSHpaeBaseData::RequestHpaeInputBuffer()
{
    HpaeBufferInfo result;
    HPAE_TRACE_NAME_FMT("RequestHpaeInputBuffer: %zu", inputBufferQueue_.size());
    if (!inputBufferQueue_.empty()) {
        result = inputBufferQueue_.back();
        inputBufferQueue_.clear();
    }
    return result;
}

// 从outputBufferCache返回一块1/4 ion buffer, 用于作为AEEBlur的输出
// outputBufferCache数据来源于render_service.so
// 可考虑Buffer的轮转
// 考虑好Buffer的释放
HpaeBufferInfo RSHpaeBaseData::RequestHpaeOutputBuffer()
{
    HpaeBufferInfo result;
    HPAE_TRACE_NAME_FMT("RequestHpaeOutputBuffer: %zu", outputBufferQueue_.size());
    if (!outputBufferQueue_.empty()) {
        result = outputBufferQueue_.back();
        outputBufferQueue_.clear();
    }
    return result;
}

bool RSHpaeBaseData::GetHasHpaeBlurNode()
{
    return hpaeStatus_.gotHpaeBlurNode;
}

NodeId RSHpaeBaseData::GetBlurNodeId()
{
    return hpaeStatus_.blurNodeId;
}

Vector4f RSHpaeBaseData::GetPixelStretch()
{
    if (hpaeStatus_.pixelStretch) {
        return *hpaeStatus_.pixelStretch;
    } else {
        return Vector4f(0.f, 0.f, 0.f, 0.f);
    }
}

Vector2f RSHpaeBaseData::GetGreyCoef()
{
    if (hpaeStatus_.greyCoef) {
        return *hpaeStatus_.greyCoef;
    } else {
        return Vector2f(0.f, 0.f);
    }
}

int RSHpaeBaseData::GetTileMode()
{
    return hpaeStatus_.tileMode;
}

float RSHpaeBaseData::GetBlurRadius()
{
    return hpaeStatus_.blurRadius;
}

float RSHpaeBaseData::GetBrightness()
{
    return hpaeStatus_.brightness;
}

float RSHpaeBaseData::GetSaturation()
{
    return hapeStatus_.saturation;
}

void RSHpaeBaseData::SetIsFirstFrame(bool status)
{
    isFirstFrame_ = status;
    if (isFirstFrame_) {
        needReset_ = true;
    }
}

bool RSHpaeBaseData::GetIsFirstFrame()
{
    return isFirstFrame_;
}

bool RSHpaeBaseData::GetNeedReset()
{
    return needReset_;
}

void RSHpaeBaseData::SetResetDone()
{
    needReset_ = false;
}

void RSHpaeBaseData::SetBlurContentChanged(bool status)
{
    blurContentChanged_ = status;
}

bool RSHpaeBaseData::GetBlurContentChanged()
{
    return blurContentChanged_;
}

} // OHOS::Rosen