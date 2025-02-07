/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#include <ff-graphics-opengl/gl.hpp>

#include <ff/Console.hpp>

#ifdef WIN32
    #define NOMINMAX
    #include <Windows.h>
#else
    #include <signal.h>
#endif

namespace ff {
    bool checkOpenGLErrors(char const* const funcName, char const* const fileName, int lineNumber) {
        bool hasError = false;
        while(GLenum error = glGetError()) {
            if(!hasError) {
                std::string source_path(fileName);
                const size_t lastSlash = source_path.find_last_of("\\/");
                std::string source_filename(source_path, lastSlash + 1);
                const size_t firstDot = source_filename.find_first_of(".");
                const std::string cleanIdentifier(source_filename, 0, firstDot);
                FF_CONSOLE_ERROR("OpenGL error occured at %s:%s.", cleanIdentifier, lineNumber);
            }
            hasError = true;
            std::string meaning;
            switch(error) {
                case 0x500:
                    meaning = "GL_INVALID_ENUM";
                    break;
                case 0x501:
                    meaning = "GL_INVALID_VALUE";
                    break;
                case 0x502:
                    meaning = "GL_INVALID_OPERATION";
                    break;
                case 0x503:
                    meaning = "GL_STACK_OVERFLOW";
                    break;
                case 0x504:
                    meaning = "GL_STACK_UNDERFLOW";
                    break;
                case 0x505:
                    meaning = "GL_OUT_OF_MEMORY";
                    break;
                case 0x506:
                    meaning = "GL_INVALID_FRAMEBUFFER_OPERATION";
                    break;
                case 0x507:
                    meaning = "GL_CONTEXT_LOST";
                    break;
                case 0x8031:
                    meaning = "GL_TABLE_TOO_LARGE1";
                    break;
                default:
                    meaning = "Unknown";
                    break;
            }
            FF_CONSOLE_ERROR("OpenGL error code [0x%04x, %s]", error, meaning);
        }
        if(hasError) {
            Console::attemptToBreakDebugger();
        }
        FF_ASSERT(!hasError, "An error occurred in OpenGL. OpenGL errors are set to be fatal.");
        return hasError;
    }
}
