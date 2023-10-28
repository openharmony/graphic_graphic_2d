/*
 * Copyright (C) 2023 Huawei Device Co., Ltd.
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

import { Matrix4 } from "./Matrix.js";
export class Camera {
    data = new Float32Array([]);
    fovy = 30;
    aspect = 1;
    near = 0.1;
    far = 1000;
    matrix4;
    centerX = 0;
    centerY = 0;
    centerZ = 0;
    eyeX = 0;
    eyeY = 8;
    eyeZ = 16;
    angleX = 0;
    angleY = 0;
    angleZ = 0;
    m1 = new Matrix4();
    m2 = new Matrix4();
    mX = new Matrix4();
    mY = new Matrix4();
    mZ = new Matrix4();
    pointLightX = 0;
    pointLightY = 3;
    pointLightZ = 0;
    pointMoveDirect = 0;
    constructor() {
        this.m1.setPerspective(this.fovy, this.aspect, this.near, this.far);
        this.m2.setLookAt(this.eyeX, this.eyeY, this.eyeZ, this.centerX, this.centerY, this.centerZ, 0, 1, 0);
        this.mX.setRotate(this.angleX, 1, 0, 0);
        this.mY.setRotate(this.angleY, 0, 1, 0);
        this.mZ.setRotate(this.angleZ, 0, 0, 1);
        this.matrix4 = this.m1.multiply(this.m2).multiply(this.mX).multiply(this.mY).multiply(this.mZ);
    }
    perspective(fovy = 60, aspect = 1, near = 0.1, far = 100) {
        this.fovy = fovy;
        this.aspect = aspect;
        this.near = near;
        this.far = far;
        this.m1.setPerspective(this.fovy, this.aspect, this.near, this.far);
        this.m2.setLookAt(this.eyeX, this.eyeY, this.eyeZ, this.centerX, this.centerY, this.centerZ, 0, 1, 0);
        this.mX.setRotate(this.angleX, 1, 0, 0);
        this.mY.setRotate(this.angleY, 0, 1, 0);
        this.mZ.setRotate(this.angleZ, 0, 0, 1);
        this.matrix4 = this.m1.multiply(this.m2).multiply(this.mX).multiply(this.mY).multiply(this.mZ);
        return this;
    }
    eye(x, y, z) {
        this.eyeX = x;
        this.eyeY = y;
        this.eyeZ = z;
        this.m1.setPerspective(this.fovy, this.aspect, this.near, this.far);
        this.m2.setLookAt(this.eyeX, this.eyeY, this.eyeZ, this.centerX, this.centerY, this.centerZ, 0, 1, 0);
        this.mX.setRotate(this.angleX, 1, 0, 0);
        this.mY.setRotate(this.angleY, 0, 1, 0);
        this.mZ.setRotate(this.angleZ, 0, 0, 1);
        this.matrix4 = this.m1.multiply(this.m2).multiply(this.mX).multiply(this.mY).multiply(this.mZ);
        return this;
    }
    center(x, y, z) {
        this.centerX = x;
        this.centerY = y;
        this.centerZ = z;
        this.m1.setPerspective(this.fovy, this.aspect, this.near, this.far);
        this.m2.setLookAt(this.eyeX, this.eyeY, this.eyeZ, this.centerX, this.centerY, this.centerZ, 0, 1, 0);
        this.mX.setRotate(this.angleX, 1, 0, 0);
        this.mY.setRotate(this.angleY, 0, 1, 0);
        this.mZ.setRotate(this.angleZ, 0, 0, 1);
        this.matrix4 = this.m1.multiply(this.m2).multiply(this.mX).multiply(this.mY).multiply(this.mZ);
        return this;
    }
    rotate(x, y, z) {
        this.angleX += x;
        this.angleY += y;
        this.angleZ += z;
        this.m1.setPerspective(this.fovy, this.aspect, this.near, this.far);
        this.m2.setLookAt(this.eyeX, this.eyeY, this.eyeZ, this.centerX, this.centerY, this.centerZ, 0, 1, 0);
        this.mX.setRotate(this.angleX, 1, 0, 0);
        this.mY.setRotate(this.angleY, 0, 1, 0);
        this.mZ.setRotate(this.angleZ, 0, 0, 1);
        this.matrix4 = this.m1.multiply(this.m2).multiply(this.mX).multiply(this.mY).multiply(this.mZ);
        return this;
    }
    getElements() {
        return this.matrix4.getElements();
    }
}
