//
// Created by lewis-v on 2020/10/18.
//

#include "YUVGLRender.h"

#include "../base/Macros.h"

NS_KP_BEGIN
#define BYTES_PER_FLOAT 4
#define POSITION_COMPONENT_COUNT 2
#define TEXTURE_COORDINATES_COMPONENT_COUNT 2
#define STRIDE ((POSITION_COMPONENT_COUNT + TEXTURE_COORDINATES_COMPONENT_COUNT)*BYTES_PER_FLOAT)

    static const GLfloat VERTEX_DATA[] = {0.0f, 0.0f, 0.5f, 0.5f, -1.0f, -1.0f, 0.0f, 1.0f,
                                          1.0f, -1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 0.0f, -1.0f,
                                          1.0f, 0.0f,
                                          0.0f, -1.0f, -1.0f, 0.0f, 1.0f};

    void YUVGLRender::render(AVFrame *frame, int width, int height) {
        glUseProgram(_program);
        GLubyte *y = frame->data[0];
        GLubyte *u = frame->data[1];
        GLubyte *v = frame->data[2];
        GLint y_width = frame->linesize[0];
        GLint u_width = frame->linesize[1];
        GLint v_width = frame->linesize[2];

        logD("render width:%d  height:%d y_width:%d u_width:%d  v_width:%d", width, height, y_width,
             u_width, v_width);
        auto h = height;
        //Y
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, static_cast<GLuint>(yTexture));
        glTexImage2D(GL_TEXTURE_2D, 0, GL_LUMINANCE, y_width, h, 0, GL_LUMINANCE,
                     GL_UNSIGNED_BYTE, y);
        glUniform1i(yTextureLocation, 0);
        //U
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, static_cast<GLuint>(uTexture));
        glTexImage2D(GL_TEXTURE_2D, 0, GL_LUMINANCE, u_width, h / 2, 0,
                     GL_LUMINANCE, GL_UNSIGNED_BYTE, u);
        glUniform1i(uTextureLocation, 1);
        //V
        glActiveTexture(GL_TEXTURE2);
        glBindTexture(GL_TEXTURE_2D, static_cast<GLuint>(vTexture));
        glTexImage2D(GL_TEXTURE_2D, 0, GL_LUMINANCE, v_width, h / 2, 0,
                     GL_LUMINANCE, GL_UNSIGNED_BYTE, v);
        glUniform1i(vTextureLocation, 2);

        //设置定点缓存指针
        glVertexAttribPointer(static_cast<GLuint>(vertAttributePositionLocation),
                              POSITION_COMPONENT_COUNT, GL_FLOAT, GL_FALSE, STRIDE, VERTEX_DATA);
        glEnableVertexAttribArray(static_cast<GLuint>(vertAttributePositionLocation));

        glVertexAttribPointer(static_cast<GLuint>(vertAttributeTexCoordLocation),
                              POSITION_COMPONENT_COUNT, GL_FLOAT, GL_FALSE, STRIDE,
                              &VERTEX_DATA[POSITION_COMPONENT_COUNT]);
        glEnableVertexAttribArray(static_cast<GLuint>(vertAttributeTexCoordLocation));

        glDrawArrays(GL_TRIANGLE_FAN, 0, 6);
    }

    YUVGLRender::YUVGLRender() {
        if (!initProgram()) {
            return;
        }
        if (!parseVertexAttribs()) {
            return;
        }
        initTexture(&yTexture);
        initTexture(&uTexture);
        initTexture(&vTexture);
    }

    YUVGLRender::~YUVGLRender() {
        glDeleteProgram(_program);
    }


    bool YUVGLRender::initProgram() {
        GLuint vertShader = 0;
        GLuint fragShader = 0;
//初始化普通的画图片的定点着色器和片元着色器
        const char *vert = R"(

precision mediump float;


attribute vec4 a_Position;
attribute vec2 a_TextureCoordinates;

varying vec2 v_TextureCoordinates;

void main()
{
    gl_Position = a_Position;
    v_TextureCoordinates = a_TextureCoordinates;
}

    )";
        if (!compileShader(vertShader, GL_VERTEX_SHADER, vert)) {
            logE("RenderTexture: ERROR: Failed to compile vertex shader");
            return false;
        }
        logI("vertData finish");
        /*
         #if 1//标清标准BT.601
            "{                                                                              \n"
            "  vec4 c = vec4((texture2D(tex_y, v_TextureCoordinates).r - 16./255.) * 1.164);\n"
            "  vec4 U = vec4(texture2D(tex_u, v_TextureCoordinates).r - 128./255.);         \n"
            "  vec4 V = vec4(texture2D(tex_v, v_TextureCoordinates).r - 128./255.);         \n"
            "  c += V * vec4(1.596, -0.813, 0, 0);                                          \n"
            "  c += U * vec4(0, -0.392, 2.017, 0);                                          \n"
            "  c.a = 1.0;                                                                   \n"
            "  gl_FragColor = c;                                                            \n"
            "}                                                                              \n";
#else//高清标准BT.709
    "{                                                  \n"
    "       highp float y = texture2D(tex_y, v_TextureCoordinates).r;           \n"
    "       highp float u = texture2D(tex_u, v_TextureCoordinates).r - 0.5;     \n"
    "       highp float v = texture2D(tex_v, v_TextureCoordinates).r - 0.5;     \n"
    "       highp float r = y + 1.402 * v;                                      \n"
    "       highp float g = y - 0.344 * u - 0.714 * v;                          \n"
    "       highp float b = y + 1.772 * u;                                      \n"
    "       gl_FragColor = vec4(r, g, b, 1.0);                                  \n"
    "}                                                                          \n";
#endif
         */
        const char *frag = R"(
precision mediump float;

uniform sampler2D tex_y;
uniform sampler2D tex_u;
uniform sampler2D tex_v;

varying vec2 v_TextureCoordinates;
const mat3 g_bt709 = mat3(1.164,1.164,1.164,
               0.0,-0.213,2.112,
               1.793,-0.533,0.0);

void main()
{

    vec3 yuv;
    vec3 rgb;

    yuv.r = texture2D(tex_y,v_TextureCoordinates).r - (16.0 / 255.0);
    yuv.g = texture2D(tex_u,v_TextureCoordinates).r - 0.5;//
    yuv.b = texture2D(tex_v,v_TextureCoordinates).r - 0.5;

    rgb = g_bt709*yuv;

    gl_FragColor = vec4(rgb,1.0);



}

    )";
        if (!compileShader(fragShader, GL_FRAGMENT_SHADER, frag)) {
            logE("RenderTexture: ERROR: Failed to compile fragment shader");
            glDeleteShader(vertShader);
            return false;
        }
        logI("fragData finish");
        // create program
        _program = glCreateProgram();
        if (!_program) {
            glDeleteShader(vertShader);
            glDeleteShader(fragShader);
            return false;
        }
        //将shader关联到程序中
        glAttachShader(_program, vertShader);
        GLUtil::checkGLError("glAttachShader v");
        glAttachShader(_program, fragShader);
        GLUtil::checkGLError("glAttachShader f");
        glLinkProgram(_program);
        GLUtil::checkGLError("linkProgram");
        glDeleteShader(vertShader);
        glDeleteShader(fragShader);

        GLint status = 0;
        glGetProgramiv(_program, GL_LINK_STATUS, &status);
        if (GL_FALSE == status) {
            logE("RenderTexture: ERROR: %s: failed to link program status:%d", __FUNCTION__,
                 status);
            glDeleteProgram(_program);
            _program = 0;
            return false;
        }

        //这里会获取程序中参数对应的id，之后参数传递需要通过这些id来传递
        if (!parseVertexAttribs()) {
            logE("parseVertexAttribs: ERROR: %s", __FUNCTION__);
            return false;
        }
        return true;
    }

    bool YUVGLRender::parseVertexAttribs() {
        vertAttributePositionLocation = glGetAttribLocation(_program, "a_Position");
        if (-1 == vertAttributePositionLocation) {
            logE("YUVGLRender: %s: can not find vertex attribute of a_Position",
                 __FUNCTION__);
            return false;
        }

        vertAttributeTexCoordLocation = glGetAttribLocation(_program,
                                                            "a_TextureCoordinates");
        if (-1 == vertAttributeTexCoordLocation) {
            logE("YUVGLRender: %s %d: can not find vertex uniform of a_TextureCoordinates",
                 __FUNCTION__, __LINE__);
            return false;
        }

        yTextureLocation = glGetUniformLocation(_program, "tex_y");
        if (-1 == yTextureLocation) {
            logE("YUVGLRender: %s %d: can not find vertex uniform of tex_y",
                 __FUNCTION__, __LINE__);
            return false;
        }

        uTextureLocation = glGetUniformLocation(_program, "tex_u");
        if (-1 == uTextureLocation) {
            logE("YUVGLRender: %s %d: can not find vertex uniform of tex_u",
                 __FUNCTION__, __LINE__);
            return false;
        }

        vTextureLocation = glGetUniformLocation(_program, "tex_v");
        if (-1 == vTextureLocation) {
            logE("YUVGLRender: %s %d: can not find vertex uniform of tex_v",
                 __FUNCTION__, __LINE__);
            return false;
        }
        return true;
    }

    void YUVGLRender::initTexture(GLuint *texture) {
        glGenTextures(1, texture);
        GLUtil::checkGLError("glGenTextures");
        glBindTexture(GL_TEXTURE_2D, *texture);
        GLUtil::checkGLError("glBindTexture 1");
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        GLUtil::checkGLError("glTexParameteri");
    }


NS_KP_END
