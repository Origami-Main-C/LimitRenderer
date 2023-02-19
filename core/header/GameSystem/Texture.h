#pragma once
#define STB_IMAGE_IMPLEMENTATION

#include<stb_image.h>
#include <GameSystem/Log.h>


class Texture {
public:
    unsigned int ID;

    Texture(const char *path) {
        glGenTextures(1, &ID);
        int width, height, nrComponents;
        unsigned char *data = stbi_load(path, &width, &height, &nrComponents, 0);
        if (data) {
            GLenum format;
            if (nrComponents == 1) {
                format = GL_RED;
            } else if (nrComponents == 3) {
                format = GL_RGB;
            } else if (nrComponents == 4) {
                format = GL_RGBA;
            }
            glBindTexture(GL_TEXTURE_2D, ID);
            glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
            glGenerateMipmap(GL_TEXTURE_2D);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            LOG_Log("TEXTURE", "LOAD", path);
        } else {
            LOG_Error("TEXTURE", "FAILED_TO_LOAD_AT_PATH", path);
        }
        stbi_image_free(data);
    }

    void Bind() {
        glBindTexture(GL_TEXTURE_2D, ID);
    }
};

//use with fbo
class FrameTexture {
public:
    unsigned int ID;

    FrameTexture() {
        glGenTextures(1, &ID);
    }

    void Bind() {
        glBindTexture(GL_TEXTURE_2D, ID);

    }

    void Data(int width, int height) {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, ID, 0);
    }
};