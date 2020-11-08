//
// Created by lewis-v on 2020/11/1.
//

#ifndef PLAYER_RGBGLRENDER_H
#define PLAYER_RGBGLRENDER_H


#include "../base/Macros.h"
#include "BaseGLRender.h"

NS_KP_BEGIN
    class RGBGLRender : public BaseGLRender {
    public:
        RGBGLRender();

        ~RGBGLRender();

        virtual void render(AVFrame *frame, int width, int height) override;

        std::map<std::string, GLuint> textureMap;
    private:

        GLuint _program = 0;
        GLint vertAttributePositionLocation = -1;
        GLint vertAttributeTexCoordLocation = -1;
        GLint rgbTextureLocation = -1;
        GLuint texture = 0;

        float projectionMatrix[16];

        bool initProgram();

        bool parseVertexAttribs();


    };
NS_KP_END


#endif //PLAYER_RGBGLRENDER_H
