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

import { Shape } from "./Shape.js";
import { Point } from "./Point.js";

export class PlaneShape extends Shape {
    width; //边长
    height; //边长
    center; //中心点
    widthSegments; //分段数
    heightSegments; //分段数
    image;
    repeatX = 1;
    repeatY = 1;
    alpha = 1.0;
    vertexBuffer = null;
    colorBuffer = null;
    indexBuffer = null;
    normalBuffer = null;
    vertexBufferData;
    colorBufferData;
    indexBufferData;
    normalBufferData = null;
    uvsBuffer = null;
    uvsBufferData = null;
    texture = null;
    vao = null;
    ext = null;
    attributes = ["a_Position", "a_Color", "a_Normal", "a_TexCoord"];
    uniforms = ["u_MvpMatrix", "u_NormalMatrix", "u_ModelMatrix", "alpha", "lightPosition", "lightDirection", "lightInnerCutoff", "lightOuterCutoff", "u_FogColor", "u_FogDist", "u_Eye", "v_UseTexture", "u_Sampler"];

    constructor(width = 1, height = 1, center = new Point(), widthSegments = 1, heightSegments = 1) {
        super();
        this.width = width;
        this.height = height;
        this.center = center;
        this.widthSegments = widthSegments;
        this.heightSegments = heightSegments;
        this.refreshData();
    }

    setImage(img, repeatX = 1, repeatY = 1) {
        this.image = img;
        this.repeatX = repeatX;
        this.repeatY = repeatY;
        this.refreshData();
        return this;
    }

    setAlpha(v) {
        this.alpha = v;
        return this;
    }

    refreshData() {
        this.vertices = [];
        this.indices = [];
        this.color = [];
        this.uvs = [];
        this.normal = [];
        let offset = this.vertices.length / 3;
        let p1 = new Point(this.center.x - this.width / 2, this.center.y, this.center.z - this.height / 2);
        let p2 = new Point(this.center.x + this.width / 2, this.center.y, this.center.z - this.height / 2);
        let p3 = new Point(this.center.x - this.width / 2, this.center.y, this.center.z + this.height / 2);
        let p4 = new Point(this.center.x + this.width / 2, this.center.y, this.center.z + this.height / 2);
        for (let i = 0; i <= this.heightSegments; i++) {
            for (let j = 0; j <= this.widthSegments; j++) {
                let x = p1.x + j * (p2.x - p1.x) / this.widthSegments;
                let z = p1.z + i * (p3.z - p1.z) / this.heightSegments;
                this.vertices.push(x, p1.y, z);
                this.uvs.push(j / this.widthSegments * this.repeatX, i / this.heightSegments * this.repeatY);
                if (i < this.widthSegments && j < this.heightSegments) {
                    let first = i * (this.widthSegments + 1) + j + offset;
                    let second = first + this.widthSegments + 1;
                    this.indices.push(first, second, first + 1, second, second + 1, first + 1);
                    this.normal.push(0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0);
                    this.color.push(1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0);
                }
            }
        }
    }


    initBuffer(gl) {
        if (!this.vertexBuffer) {
            this.vertexBuffer = gl.createBuffer();
            this.vertexBufferData = new Float32Array(this.vertices);
        }
        if (!this.colorBuffer) {
            this.colorBuffer = gl.createBuffer();
            this.colorBufferData = new Float32Array(this.color);
        }
        if (!this.indexBuffer) {
            this.indexBuffer = gl.createBuffer();
            this.indexBufferData = new Uint8Array(this.indices);
        }
        if (!this.uvsBuffer) {
            this.uvsBuffer = gl.createBuffer();
            this.uvsBufferData = new Float32Array(this.uvs);
            this.texture = gl.createTexture();
        }
        if (!this.normalBuffer) {
            this.normalBuffer = gl.createBuffer();
            this.normalBufferData = new Float32Array(this.normal);
        }
    }

    init(gl, camera) {
        if (!this.program) {
            this.createProgram(gl, this.attributes, this.uniforms);
        }
        gl.useProgram(this.program);
        this.initBuffer(gl);
        gl.bindBuffer(gl.ARRAY_BUFFER, this.vertexBuffer);
        gl.bufferData(gl.ARRAY_BUFFER, this.vertexBufferData, gl.STATIC_DRAW);
        gl.vertexAttribPointer(this.program.a_Position, 3, gl.FLOAT, false, Float32Array.BYTES_PER_ELEMENT * 3, 0);
        gl.enableVertexAttribArray(this.program.a_Position);
        gl.bindBuffer(gl.ARRAY_BUFFER, this.colorBuffer);
        gl.bufferData(gl.ARRAY_BUFFER, this.colorBufferData, gl.STATIC_DRAW);
        gl.vertexAttribPointer(this.program.a_Color, 3, gl.FLOAT, false, Float32Array.BYTES_PER_ELEMENT * 3, 0);
        gl.enableVertexAttribArray(this.program.a_Color);
        gl.bindBuffer(gl.ELEMENT_ARRAY_BUFFER, this.indexBuffer);
        gl.bufferData(gl.ELEMENT_ARRAY_BUFFER, this.indexBufferData, gl.STATIC_DRAW);
        this.initImage(gl);
        gl.uniformMatrix4fv(this.program.u_MvpMatrix, false, camera.getElements());
        gl.bindBuffer(gl.ARRAY_BUFFER, this.normalBuffer);
        gl.bufferData(gl.ARRAY_BUFFER, this.normalBufferData, gl.STATIC_DRAW);
        gl.vertexAttribPointer(this.program.a_Normal, 3, gl.FLOAT, false, Float32Array.BYTES_PER_ELEMENT * 3, 0);
        gl.enableVertexAttribArray(this.program.a_Normal);
        gl.uniformMatrix4fv(this.program.u_ModelMatrix, false, Shape.modelMatrix.getElements());
        Shape.normalMatrix.setInverseOf(Shape.modelMatrix);
        Shape.normalMatrix.transpose();
        gl.uniformMatrix4fv(this.program.u_NormalMatrix, false, Shape.normalMatrix.getElements());
        gl.uniform1f(this.program.alpha, this.alpha);
        gl.uniform3fv(this.program.lightPosition, new Float32Array([camera.pointLightX, camera.pointLightY, camera.pointLightZ]));
        gl.uniform3fv(this.program.lightDirection, Shape.lightDirection);
        gl.uniform1f(this.program.lightInnerCutoff, Shape.lightInner); //cos 15
        gl.uniform1f(this.program.lightOuterCutoff, Shape.lightOuter); //cos 30
        this.initFog(gl, camera);
        this.initFinish = true;
    }

    initFog(gl, camera) {
        gl.uniform3fv(this.program.u_FogColor, Shape.fogColor);
        gl.uniform2fv(this.program.u_FogDist, Shape.fog);
        gl.uniform4fv(this.program.u_Eye, new Float32Array([camera.eyeX, camera.eyeY, camera.eyeZ, 1.0]));
    }

    initImage(gl) {
        gl.uniform1i(this.program.v_UseTexture, 0);
        if (this.image) {
            gl.bindBuffer(gl.ARRAY_BUFFER, this.uvsBuffer);
            gl.bufferData(gl.ARRAY_BUFFER, this.uvsBufferData, gl.STATIC_DRAW);
            gl.vertexAttribPointer(this.program.a_TexCoord, 2, gl.FLOAT, false, Float32Array.BYTES_PER_ELEMENT * 2, 0);
            gl.enableVertexAttribArray(this.program.a_TexCoord);
            gl.pixelStorei(gl.UNPACK_FLIP_Y_WEBGL, 1);
            gl.activeTexture(gl.TEXTURE0);
            gl.bindTexture(gl.TEXTURE_2D, this.texture);
            gl.texImage2D(gl.TEXTURE_2D, 0, gl.RGBA, gl.RGBA, gl.UNSIGNED_BYTE, this.image);
            gl.generateMipmap(gl.TEXTURE_2D);
            gl.texParameteri(gl.TEXTURE_2D, gl.TEXTURE_MIN_FILTER, gl.LINEAR);
            gl.texParameteri(gl.TEXTURE_2D, gl.TEXTURE_MAG_FILTER, gl.LINEAR);
            gl.texParameteri(gl.TEXTURE_2D, gl.TEXTURE_WRAP_S, gl.MIRRORED_REPEAT);
            gl.texParameteri(gl.TEXTURE_2D, gl.TEXTURE_WRAP_T, gl.MIRRORED_REPEAT);
            gl.uniform1i(this.program.v_UseTexture, 1);
        }
    }
}
