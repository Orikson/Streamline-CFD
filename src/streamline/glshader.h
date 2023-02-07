#ifndef SHADER_H
#define SHADER_H

#include <QOpenGLFunctions>

#include <iostream>
#include <fstream>
#include <sstream>

#include <glm/glm.hpp>

#include <vector>
using std::vector;
#include <string>
using std::string;


/**
 * @brief Defines a shader class to bind and store information on a set of vertex/fragment/(geometry) shaders. (adpated from https://learnopengl.com/code_viewer_gh.php?code=includes/learnopengl/shader.h)
 */
class Shader {
    public:
        unsigned int ID;
        QOpenGLFunctions* f;

        Shader(QOpenGLFunctions* f, const char* vertexPath, const char* fragmentPath, const char* geometryPath = NULL) {
            this->f = f;

            string vertexCode;
            string fragmentCode;
            string geometryCode;
            std::ifstream vShaderFile;
            std::ifstream fShaderFile;
            std::ifstream gShaderFile;

            // ensure ifstream objects can throw exceptions:
            vShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
            fShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
            gShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);

            try {
                // open files
                vShaderFile.open(vertexPath);
                fShaderFile.open(fragmentPath);
                std::stringstream vShaderStream, fShaderStream;

                // read file's buffer contents into streams
                vShaderStream << vShaderFile.rdbuf();
                fShaderStream << fShaderFile.rdbuf();

                // close file handlers
                vShaderFile.close();
                fShaderFile.close();

                // convert stream into string
                vertexCode = vShaderStream.str();
                fragmentCode = fShaderStream.str();

                // if geometry shader path is present, also load a geometry shader
                if (geometryPath != nullptr) {
                    gShaderFile.open(geometryPath);
                    std::stringstream gShaderStream;
                    gShaderStream << gShaderFile.rdbuf();
                    gShaderFile.close();
                    geometryCode = gShaderStream.str();
                }
            }
            catch (std::ifstream::failure& e) {
                qDebug() << "ERROR::SHADER::FILE_NOT_SUCCESFULLY_READ: " << e.what();
            }

            const char* vShaderCode = vertexCode.c_str();
            const char* fShaderCode = fragmentCode.c_str();

            // 2. compile shaders
            unsigned int vertex, fragment;

            // vertex shader
            vertex = f->glCreateShader(GL_VERTEX_SHADER);
            f->glShaderSource(vertex, 1, &vShaderCode, NULL);
            f->glCompileShader(vertex);
            checkCompileErrors(vertex, "VERTEX");

            // fragment Shader
            fragment = f->glCreateShader(GL_FRAGMENT_SHADER);
            f->glShaderSource(fragment, 1, &fShaderCode, NULL);
            f->glCompileShader(fragment);
            checkCompileErrors(fragment, "FRAGMENT");

            // if geometry shader is given, compile geometry shader
            unsigned int geometry;
            if (geometryPath != nullptr) {
                const char* gShaderCode = geometryCode.c_str();
                geometry = f->glCreateShader(GL_GEOMETRY_SHADER);
                f->glShaderSource(geometry, 1, &gShaderCode, NULL);
                f->glCompileShader(geometry);
                checkCompileErrors(geometry, "GEOMETRY");
            }

            // shader Program
            ID = f->glCreateProgram();
            f->glAttachShader(ID, vertex);
            f->glAttachShader(ID, fragment);
            if (geometryPath != nullptr)
                f->glAttachShader(ID, geometry);
            f->glLinkProgram(ID);
            checkCompileErrors(ID, "PROGRAM");

            // delete the shaders as they're linked into our program now and no longer necessery
            f->glDeleteShader(vertex);
            f->glDeleteShader(fragment);
            if (geometryPath != nullptr)
                f->glDeleteShader(geometry);
        }

        void use() {
            f->glUseProgram(ID);
        }

        void setBool(const std::string& name, bool value) {
            f->glUniform1i(f->glGetUniformLocation(ID, name.c_str()), (int)value);
        }

        void setInt(const std::string& name, int value) {
            f->glUniform1i(f->glGetUniformLocation(ID, name.c_str()), value);
        }

        void setFloat(const std::string& name, float value) {
            f->glUniform1f(f->glGetUniformLocation(ID, name.c_str()), value);
        }

        void setVec2(const std::string& name, const glm::vec2& value) {
            f->glUniform2fv(f->glGetUniformLocation(ID, name.c_str()), 1, &value[0]);
        }
        void setVec2(const std::string& name, float x, float y) {
            f->glUniform2f(f->glGetUniformLocation(ID, name.c_str()), x, y);
        }

        void setVec3(const std::string& name, const glm::vec3& value) {
            f->glUniform3fv(f->glGetUniformLocation(ID, name.c_str()), 1, &value[0]);
        }
        void setVec3(const std::string& name, float x, float y, float z) {
            f->glUniform3f(f->glGetUniformLocation(ID, name.c_str()), x, y, z);
        }

        void setVec4(const std::string& name, const glm::vec4& value) {
            f->glUniform4fv(f->glGetUniformLocation(ID, name.c_str()), 1, &value[0]);
        }
        void setVec4(const std::string& name, float x, float y, float z, float w) {
            f->glUniform4f(f->glGetUniformLocation(ID, name.c_str()), x, y, z, w);
        }

        void setMat2(const std::string& name, const glm::mat2& mat) {
            f->glUniformMatrix2fv(f->glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE, &mat[0][0]);
        }

        void setMat3(const std::string& name, const glm::mat3& mat) {
            f->glUniformMatrix3fv(f->glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE, &mat[0][0]);
        }

        void setMat4(const std::string& name, const glm::mat4& mat) {
            f->glUniformMatrix4fv(f->glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE, &mat[0][0]);
        }

    private:
        void checkCompileErrors(GLuint shader, string type) {
            GLint success;
            GLchar infoLog[1024];
            if (type != "PROGRAM") {
                f->glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
                if (!success) {
                    f->glGetShaderInfoLog(shader, 1024, NULL, infoLog);
                    std::cout << "ERROR::SHADER_COMPILATION_ERROR of type: " << type << "\n" << infoLog << std::endl;
                }
            }
            else {
                f->glGetProgramiv(shader, GL_LINK_STATUS, &success);
                if (!success) {
                    f->glGetProgramInfoLog(shader, 1024, NULL, infoLog);
                    std::cout << "ERROR::PROGRAM_LINKING_ERROR of type: " << type << "\n" << infoLog << std::endl;
                }
            }
        }
};

#endif // SHADER_H