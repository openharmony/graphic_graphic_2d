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

#include "eglwrapperdisplay_fuzzer.h"

#include <securec.h>

#include <EGL/egl.h>
#include "egl_wrapper_display.h"
#include "egl_wrapper_object.h"

namespace OHOS {
    namespace {
        const uint8_t* data_ = nullptr;
        size_t size_ = 0;
        size_t pos;
    }

    /*
    * describe: get data from outside untrusted data(data_) which size is according to sizeof(T)
    * tips: only support basic type
    */
    template<class T>
    T GetData()
    {
        T object {};
        size_t objectSize = sizeof(object);
        if (data_ == nullptr || objectSize > size_ - pos) {
            return object;
        }
        errno_t ret = memcpy_s(&object, objectSize, data_ + pos, objectSize);
        if (ret != EOK) {
            return {};
        }
        pos += objectSize;
        return object;
    }

    bool DoSomethingInterestingWithMyAPI(const uint8_t* data, size_t size)
    {
        if (data == nullptr || size < 0) {
            return false;
        }

        // initialize
        data_ = data;
        size_ = size;
        pos = 0;

        auto eglDisplay = eglGetDisplay(EGL_DEFAULT_DISPLAY);
        EGLDisplay display1 = GetData<EGLDisplay>();
        EGLDisplay display2 = GetData<EGLDisplay>();
        EGLint major1 = GetData<EGLint>();
        EGLint minor1 = GetData<EGLint>();
        EGLSurface draw1 = GetData<EGLSurface>();
        EGLSurface read1 = GetData<EGLSurface>();
        EGLContext ctx1 = GetData<EGLContext>();
        EGLenum platform1 = GetData<EGLenum>();
        EGLNativeDisplayType disp1 = GetData<EGLNativeDisplayType>();
        EGLAttrib attribList1 = GetData<EGLAttrib>();
        EGLenum platform2 = GetData<EGLenum>();
        void* disp2 = GetData<void*>();
        EGLint attribList2 = GetData<EGLint>();
        EGLContext ctx2 = GetData<EGLContext>();
        EGLSurface surf1 = GetData<EGLSurface>();
        EGLConfig config1 = GetData<EGLConfig>();
        EGLContext shareList1 = GetData<EGLContext>();
        EGLint attribList3 = GetData<EGLint>();
        EGLConfig config2 = GetData<EGLConfig>();
        NativeWindowType window1 = GetData<NativeWindowType>();
        EGLint attribList4 = GetData<EGLint>();
        EGLContext context1 = GetData<EGLContext>();
        EGLSurface surf2 = GetData<EGLSurface>();
        EglWrapperDisplay* disp3 =  EglWrapperDisplay::GetWrapperDisplay(display1);
        EglWrapperObject *obj1 = new  EglWrapperObject(disp3);
        EglWrapperDisplay* disp4 =  EglWrapperDisplay::GetWrapperDisplay(display2);
        EglWrapperObject *obj2 = new  EglWrapperObject(disp4);
        EGLSurface surf3 = GetData<EGLSurface>();
        NativePixmapType target1 = GetData<NativePixmapType>();
        EGLConfig config3 = GetData<EGLConfig>();
        EGLint attribList5 = GetData<EGLint>();
        EGLConfig config4 = GetData<EGLConfig>();
        EGLNativePixmapType pixmap1 = GetData<EGLNativePixmapType>();
        EGLint attribList6 = GetData<EGLint>();
        EGLContext ctx3 = GetData<EGLContext>();
        EGLint attribList7 = GetData<EGLint>();
        EGLint value1 = GetData<EGLint>();
        EGLSurface surf4 = GetData<EGLSurface>();
        EGLint attribList8 = GetData<EGLint>();
        EGLint value2 = GetData<EGLint>();
        EGLSurface surf5 = GetData<EGLSurface>();
        EGLSurface surf6 = GetData<EGLSurface>();
        EGLint buffer1 = GetData<EGLint>();
        EGLSurface surf7 = GetData<EGLSurface>();
        EGLint buffer2 = GetData<EGLint>();
        EGLSurface surf8 = GetData<EGLSurface>();
        EGLint attribList9 = GetData<EGLint>();
        EGLint value3 = GetData<EGLint>();
        EGLenum buftype1 = GetData<EGLenum>();
        EGLClientBuffer buffer3 = GetData<EGLClientBuffer>();
        EGLConfig config5 = GetData<EGLConfig>();
        EGLint attribList10 = GetData<EGLint>();
        EGLContext ctx4 = GetData<EGLContext>();
        EGLenum target2 = GetData<EGLenum>();
        EGLClientBuffer buffer4 = GetData<EGLClientBuffer>();
        EGLAttrib attribList11 = GetData<EGLAttrib>();
        EGLImage img1 = GetData<EGLImage>();
        EGLConfig config6 = GetData<EGLConfig>();
        void *nativeWindow = GetData<void*>();
        EGLAttrib attribList12 = GetData<EGLAttrib>();
        EGLConfig config7 = GetData<EGLConfig>();
        void *nativePixmap = GetData<void*>();
        EGLAttrib attribList13 = GetData<EGLAttrib>();
        EGLSurface surf9 = GetData<EGLSurface>();
        EGLint attribList14 = GetData<EGLint>();
        EGLSurface surf10 = GetData<EGLSurface>();
        EGLContext ctx5 = GetData<EGLContext>();
        EGLenum target3 = GetData<EGLenum>();
        EGLClientBuffer buffer5 = GetData<EGLClientBuffer>();
        EGLint attribList15 = GetData<EGLint>();
        EGLImageKHR img2 = GetData<EGLImageKHR>();
        EGLConfig config8 = GetData<EGLConfig>();
        EGLStreamKHR stream = GetData<EGLStreamKHR>();
        EGLint attribList16 = GetData<EGLint>();
        EGLSurface draw2 = GetData<EGLSurface>();
        EGLint rects1 = GetData<EGLint>();
        EGLint nRects1 = GetData<EGLint>();
        EGLSurface surf11 = GetData<EGLSurface>();
        EGLint rects2 = GetData<EGLint>();
        EGLint nRects2 = GetData<EGLint>();

        // test
        EglWrapperDisplay* eglWrapperDisplay =  EglWrapperDisplay::GetWrapperDisplay(eglDisplay);
        eglWrapperDisplay->Init(&major1, &minor1);
        eglWrapperDisplay->MakeCurrent(draw1, read1, ctx1);
        EglWrapperDisplay::GetEglDisplay(platform1, disp1, &attribList1);
        EglWrapperDisplay::GetEglDisplayExt(platform2, disp2, &attribList2);
        eglWrapperDisplay->ValidateEglContext(ctx2);
        eglWrapperDisplay->ValidateEglSurface(surf1);
        eglWrapperDisplay->CreateEglContext(config1, shareList1, &attribList3);
        eglWrapperDisplay->CreateEglSurface(config2, window1, &attribList4);
        eglWrapperDisplay->DestroyEglContext(context1);
        eglWrapperDisplay->DestroyEglSurface(surf2);
        eglWrapperDisplay->AddObject(obj1);
        eglWrapperDisplay->RemoveObject(obj2);
        eglWrapperDisplay->CopyBuffers(surf3, target1);
        eglWrapperDisplay->CreatePbufferSurface(config3, &attribList5);
        eglWrapperDisplay->CreatePixmapSurface(config4, pixmap1, &attribList6);
        eglWrapperDisplay->QueryContext(ctx3, attribList7, &value1);
        eglWrapperDisplay->QuerySurface(surf4, attribList8, &value2);
        eglWrapperDisplay->SwapBuffers(surf5);
        eglWrapperDisplay->BindTexImage(surf6, buffer1);
        eglWrapperDisplay->ReleaseTexImage(surf7, buffer2);
        eglWrapperDisplay->SurfaceAttrib(surf8, attribList9, value3);
        eglWrapperDisplay->CreatePbufferFromClientBuffer(buftype1, buffer3, config5, &attribList10);
        eglWrapperDisplay->CreateImage(ctx4, target2, buffer4, &attribList11);
        eglWrapperDisplay->DestroyImage(img1);
        eglWrapperDisplay->CreatePlatformWindowSurface(config6, nativeWindow, &attribList12);
        eglWrapperDisplay->CreatePlatformPixmapSurface(config7, nativePixmap, &attribList13);
        eglWrapperDisplay->LockSurfaceKHR(surf9, &attribList14);
        eglWrapperDisplay->UnlockSurfaceKHR(surf10);
        eglWrapperDisplay->CreateImageKHR(ctx5, target3, buffer5, &attribList15);
        eglWrapperDisplay->DestroyImageKHR(img2);
        eglWrapperDisplay->CreateStreamProducerSurfaceKHR(config8, stream, &attribList16);
        eglWrapperDisplay->SwapBuffersWithDamageKHR(draw2, &rects1, nRects1);
        eglWrapperDisplay->SetDamageRegionKHR(surf11, &rects2, nRects2);

        return true;
    }
}

/* Fuzzer entry point */
extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size)
{
    /* Run your code on data */
    OHOS::DoSomethingInterestingWithMyAPI(data, size);
    return 0;
}