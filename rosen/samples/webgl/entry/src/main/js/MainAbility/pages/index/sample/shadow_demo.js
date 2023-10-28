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

import { createProgram, loadImage } from "../utils/extension";
import { Images } from '../utils/Images';
import { Matrix4 } from "../utils/Matrix";

let lastTime = Date.now();
const WIDTH = 1024, HEIGHT = 1024;
let g_modelMatrix = new Matrix4();
let g_mvpMatrix = new Matrix4();
const SHADOW_VS_CODE = `
        attribute vec4 a_Position;
        uniform mat4 u_MvpMatrix;
        void main(){
            gl_Position =  u_MvpMatrix * a_Position;
        }
    `;
const SHADOW_FS_CODE = `
        precision mediump float;
        void main(){
            gl_FragColor = vec4(gl_FragCoord.z, 0.0, 0.0, 0.0);
        }
    `;

const NORMAL_VS_CODE = `
        attribute vec4 a_Position;
        attribute vec4 a_Color;
        attribute vec2 a_TexCoord;
        uniform mat4 u_MvpMatrix;
        uniform mat4 u_MvpMatrixFromLight;
        varying vec4 v_PositionFromLight;
        varying vec4 v_Color;
        varying vec2 v_TexCoord;
        void main(){
            gl_Position =  u_MvpMatrix * a_Position;
            v_PositionFromLight = u_MvpMatrixFromLight * a_Position;
            v_Color = a_Color;
            v_TexCoord = a_TexCoord;
        }
    `;

const NORMAL_FS_CODE = `
        precision mediump float;
        uniform sampler2D u_ShadowMap;
        uniform sampler2D u_Sampler;
        varying vec2 v_TexCoord;
        varying vec4 v_PositionFromLight;
        varying vec4 v_Color;
        void main(){
            vec4 texColor = texture2D(u_Sampler,v_TexCoord);
            vec3 shadowCoord = (v_PositionFromLight.xyz/v_PositionFromLight.w)/2.0 + 0.5;
            vec4 rgbaDepth = texture2D(u_ShadowMap,shadowCoord.xy);
            float depth = rgbaDepth.r;
            float visibility = (shadowCoord.z > depth + 0.005) ? 0.6 : 1.0;
            gl_FragColor = vec4(texColor.rgb * visibility,v_Color.a);
        }
    `;
let currentAngle = 0.0;
let mvpMatrixFromLight_t = new Matrix4();
let mvpMatrixFromLight_p = new Matrix4();

export async function shadow_demo(gl) {
    let floorImage = await loadImage(Images.FLOOR);
    let boxImage = await loadImage(Images.BOX);
    let shadowProgram = createProgram(gl, SHADOW_VS_CODE, SHADOW_FS_CODE, ["a_Position"], ["u_MvpMatrix"]);
    let program = createProgram(gl, NORMAL_VS_CODE, NORMAL_FS_CODE, ["a_Position", "a_Color", "a_TexCoord"], ["u_MvpMatrix", "u_MvpMatrixFromLight", "u_ShadowMap", "u_Sampler"]);
    let triangle = initVertexBuffersTriangle(gl);
    let plane = initVertexBuffersPlane(gl);
    let fbo = initFramebuffer(gl);
    gl.activeTexture(gl.TEXTURE0);
    gl.bindTexture(gl.TEXTURE_2D, fbo.texture);
    gl.clearColor(0, 0, 0, 1);
    gl.enable(gl.DEPTH_TEST);
    let matrixLight = new Matrix4();
    matrixLight.setPerspective(60.0, gl.width / gl.height, 1.0, 100.0);
    matrixLight.lookAt(-3, 10, 2, 0, 0, 0, 0, 1, 0);
    let matrixCamera = new Matrix4();
    matrixCamera.setPerspective(60, gl.width / gl.height, 1.0, 100.0);
    matrixCamera.lookAt(0, 2, 10, 0, 0, 0, 0, 1, 0);
    let tick = () => {
        currentAngle = anim(currentAngle);
        gl.bindFramebuffer(gl.FRAMEBUFFER, fbo);
        gl.viewport(0, 0, HEIGHT, HEIGHT);
        gl.clear(gl.COLOR_BUFFER_BIT | gl.DEPTH_BUFFER_BIT | gl.STENCIL_BUFFER_BIT); // 清除FBO
        gl.useProgram(shadowProgram);
        // 绘制三角形和平面（用于生成阴影贴图）设置旋转角度以模型矩阵并绘制三角形
        g_modelMatrix.setRotate(currentAngle, 1, 1, 0);
        draw(gl, shadowProgram, triangle, matrixLight);
        mvpMatrixFromLight_t.set(g_mvpMatrix);
        g_modelMatrix.setRotate(0, 0, 1, 1);
        draw(gl, shadowProgram, plane, matrixLight);
        mvpMatrixFromLight_p.set(g_mvpMatrix);
        gl.bindFramebuffer(gl.FRAMEBUFFER, null); // 将绘图目标更改为颜色缓冲区
        gl.viewport(0, 0, gl.width, gl.height);
        gl.clear(gl.COLOR_BUFFER_BIT | gl.DEPTH_BUFFER_BIT | gl.STENCIL_BUFFER_BIT);
        gl.useProgram(program); // 为常规绘制设置着色器
        gl.uniform1i(program.u_ShadowMap, 0);
        gl.uniformMatrix4fv(program.u_MvpMatrixFromLight, false, mvpMatrixFromLight_t.getElements());
        g_modelMatrix.setRotate(currentAngle, 0, 1, 0);
        draw2(gl, program, triangle, matrixCamera, boxImage);
        gl.uniformMatrix4fv(program.u_MvpMatrixFromLight, false, mvpMatrixFromLight_p.getElements());
        g_modelMatrix.setRotate(0, 0, 1, 1);
        draw2(gl, program, plane, matrixCamera, floorImage);
        gl.flush();
        if (!gl.closed) {
            requestAnimationFrame(tick);
        }
    };
    tick();
}

function draw(gl, program, o, viewProjMatrix) {
    initAttributeVariable(gl, program.a_Position, o.vertexBuffer);
    if (program.a_Color != undefined) // 如果a_Color定义为属性
        initAttributeVariable(gl, program.a_Color, o.colorBuffer);
    gl.bindBuffer(gl.ELEMENT_ARRAY_BUFFER, o.indexBuffer);
    // 计算模型视图项目矩阵并将其传递给u_MvpMatrix
    g_mvpMatrix.set(viewProjMatrix);
    g_mvpMatrix.multiply(g_modelMatrix);
    gl.uniformMatrix4fv(program.u_MvpMatrix, false, g_mvpMatrix.getElements());
    gl.drawElements(gl.TRIANGLES, o.numIndices, gl.UNSIGNED_BYTE, 0);
}

function draw2(gl, program, o, viewProjMatrix, image) {
    initAttributeVariable(gl, program.a_Position, o.vertexBuffer);
    initAttributeVariable(gl, program.a_TexCoord, o.uvsBuffer);
    gl.activeTexture(gl.TEXTURE1);
    gl.bindTexture(gl.TEXTURE_2D, o.texture);
    gl.texImage2D(gl.TEXTURE_2D, 0, gl.RGBA, gl.RGBA, gl.UNSIGNED_BYTE, image);
    gl.texParameteri(gl.TEXTURE_2D, gl.TEXTURE_MIN_FILTER, gl.LINEAR);
    gl.uniform1i(program.u_Sampler, 1);
    if (program.a_Color != undefined) // 如果a_Color定义为属性
        initAttributeVariable(gl, program.a_Color, o.colorBuffer);
    gl.bindBuffer(gl.ELEMENT_ARRAY_BUFFER, o.indexBuffer);
    // 计算模型视图项目矩阵并将其传递给u_MvpMatrix
    g_mvpMatrix.set(viewProjMatrix);
    g_mvpMatrix.multiply(g_modelMatrix);
    gl.uniformMatrix4fv(program.u_MvpMatrix, false, g_mvpMatrix.getElements());
    gl.drawElements(gl.TRIANGLES, o.numIndices, gl.UNSIGNED_BYTE, 0);
}

function initAttributeVariable(gl, a_attribute, buffer) {
    gl.bindBuffer(gl.ARRAY_BUFFER, buffer);
    gl.vertexAttribPointer(a_attribute, buffer.num, buffer.type, false, 0, 0);
    gl.enableVertexAttribArray(a_attribute);
}

function initVertexBuffersPlane(gl) {
    let vertices = new Float32Array([
        8.0, -2.5, 8.0,
        -8.0, -2.5, 8.0,
        -8.0, -2.5, -8.0,
        8.0, -2.5, -8.0,
    ]);
    let uvs = new Float32Array([
        1.0, 1.0,
        0.0, 1.0,
        0.0, 0.0,
        1.0, 0.0,
    ]);
    let colors = new Float32Array([
        1.0, 0.0, 1.0,
        1.0, 1.0, 1.0,
        1.0, 1.0, 0.0,
        0.0, 1.0, 1.0
    ]);
    let indices = new Uint8Array([0, 1, 2, 0, 2, 3]);
    let o = new Object();
    o.vertexBuffer = initArrayBuffer(gl, vertices, 3, gl.FLOAT);
    o.colorBuffer = initArrayBuffer(gl, colors, 3, gl.FLOAT);
    o.indexBuffer = initElementArrayBuffer(gl, indices, gl.UNSIGNED_BYTE);
    o.uvsBuffer = initArrayBuffer(gl, uvs, 2, gl.FLOAT);
    o.texture = gl.createTexture();
    if (!o.vertexBuffer || !o.colorBuffer || !o.indexBuffer)
        return null;
    o.numIndices = indices.length;
    gl.bindBuffer(gl.ARRAY_BUFFER, null);
    gl.bindBuffer(gl.ELEMENT_ARRAY_BUFFER, null);
    return o;
}


const triangleVertices = new Float32Array([
    -1.0, 1.0, -1.0,
    1.0, 1.0, -1.0,
    -1.0, -1.0, -1.0,
    1.0, -1.0, -1.0,
    -1.0, 1.0, 1.0,
    1.0, 1.0, 1.0,
    -1.0, -1.0, 1.0,
    1.0, -1.0, 1.0,
    -1, -1, -1,
    1, -1, -1,
    -1, -1, 1,
    1, -1, 1,
]);
const triangleUVS = new Float32Array([
    0.0, 1.0,
    1.0, 1.0,
    0.0, 0.0,
    1.0, 0.0,
    0.0, 1.0,
    1.0, 1.0,
    0.0, 0.0,
    1.0, 0.0,
    0.0, 1.0,
    1.0, 1.0,
    0.0, 0.0,
    1.0, 0.0,
    0.0, 1.0,
    1.0, 1.0,
    0.0, 0.0,
    1.0, 0.0,
]);
const triangleColors = new Float32Array([
    1.0, 0.5, 1.0,
    1.0, 1.0, 0.0,
    0.0, 0.5, 1.0,
    1.0, 0.5, 1.0,
    1.0, 1.0, 0.0,
    0.0, 0.5, 1.0,
    1.0, 0.5, 1.0,
    1.0, 1.0, 0.0,
    0.0, 0.5, 1.0,
    1.0, 0.5, 1.0,
    1.0, 1.0, 0.0,
    0.0, 0.5, 1.0,
    1.0, 0.5, 1.0,
    1.0, 1.0, 0.0,
    0.0, 0.5, 1.0,
    1.0, 0.5, 1.0,
    1.0, 1.0, 0.0,
    0.0, 0.5, 1.0,
]);
const triangleIndices = new Uint8Array([
    0, 1, 2,
    1, 2, 3,
    4, 5, 6,
    5, 6, 7,
]);

function initVertexBuffersTriangle(gl) {
    let vertices = triangleVertices
    let uvs = triangleUVS;
    let colors = triangleColors;
    let indices = triangleIndices;
    let o = new Object();
    o.vertexBuffer = initArrayBuffer(gl, vertices, 3, gl.FLOAT); // vertexBuffer
    o.colorBuffer = initArrayBuffer(gl, colors, 3, gl.FLOAT); // colorBuffer
    o.uvsBuffer = initArrayBuffer(gl, uvs, 2, gl.FLOAT); // uvsBuffer
    o.indexBuffer = initElementArrayBuffer(gl, indices, gl.UNSIGNED_BYTE); // indexBuffer
    if (!o.vertexBuffer || !o.colorBuffer || !o.indexBuffer)
        return null;
    o.numIndices = indices.length;
    o.texture = gl.createTexture();
    gl.bindBuffer(gl.ARRAY_BUFFER, null);
    gl.bindBuffer(gl.ELEMENT_ARRAY_BUFFER, null);
    return o;
}

// 创建一个buffer 并将数据写入
function initArrayBuffer(gl, data, num, type) {
    let buffer = gl.createBuffer();
    gl.bindBuffer(gl.ARRAY_BUFFER, buffer);
    gl.bufferData(gl.ARRAY_BUFFER, data, gl.STATIC_DRAW);
    buffer.num = num;
    buffer.type = type;
    return buffer;
}

// 创建一个buffer 并将数据写入
function initElementArrayBuffer(gl, data, type) {
    let buffer = gl.createBuffer();
    gl.bindBuffer(gl.ELEMENT_ARRAY_BUFFER, buffer);
    gl.bufferData(gl.ELEMENT_ARRAY_BUFFER, data, gl.STATIC_DRAW);
    buffer.type = type;
    return buffer;
}

function initFramebuffer(gl) {
    let framebuffer, texture, depthBuffer;
    let error = () => {
        if (framebuffer)
            gl.deleteFramebuffer(framebuffer);
        if (texture)
            gl.deleteTexture(texture);
        if (depthBuffer)
            gl.deleteRenderbuffer(depthBuffer);
        return null;
    };
    framebuffer = gl.createFramebuffer();
    if (!framebuffer)
        return error();
    texture = gl.createTexture();
    if (!texture)
        return error();
    gl.bindTexture(gl.TEXTURE_2D, texture);
    gl.texImage2D(gl.TEXTURE_2D, 0, gl.RGBA, WIDTH, HEIGHT, 0, gl.RGBA, gl.UNSIGNED_BYTE, null);
    gl.texParameteri(gl.TEXTURE_2D, gl.TEXTURE_MIN_FILTER, gl.LINEAR);
    gl.texParameteri(gl.TEXTURE_2D, gl.TEXTURE_MAG_FILTER, gl.LINEAR);
    gl.texParameteri(gl.TEXTURE_2D, gl.TEXTURE_WRAP_S, gl.CLAMP_TO_EDGE);
    gl.texParameteri(gl.TEXTURE_2D, gl.TEXTURE_WRAP_T, gl.CLAMP_TO_EDGE);
    depthBuffer = gl.createRenderbuffer();
    if (!depthBuffer)
        return error();
    gl.bindRenderbuffer(gl.RENDERBUFFER, depthBuffer);
    gl.renderbufferStorage(gl.RENDERBUFFER, gl.DEPTH_COMPONENT16, WIDTH, HEIGHT);
    gl.bindFramebuffer(gl.FRAMEBUFFER, framebuffer);
    gl.framebufferTexture2D(gl.FRAMEBUFFER, gl.COLOR_ATTACHMENT0, gl.TEXTURE_2D, texture, 0);
    gl.framebufferRenderbuffer(gl.FRAMEBUFFER, gl.DEPTH_ATTACHMENT, gl.RENDERBUFFER, depthBuffer);
    let e = gl.checkFramebufferStatus(gl.FRAMEBUFFER);
    if (gl.FRAMEBUFFER_COMPLETE !== e)
        return error();
    framebuffer.texture = texture;
    gl.bindFramebuffer(gl.FRAMEBUFFER, null);
    gl.bindTexture(gl.TEXTURE_2D, null);
    gl.bindRenderbuffer(gl.RENDERBUFFER, null);
    return framebuffer;
}

function anim(angle) {
    let now = Date.now();
    let offset = now - lastTime;
    lastTime = now;
    let newAngle = angle + (30 * offset) / 1000.0;
    return newAngle % 360;
}
