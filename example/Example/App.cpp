#include "App.h"

#include <iostream>
#include <SDL.h>
#include <GL/glew.h>

App::App()
{
}

App::~App()
{
}

void App::Init()
{
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        std::cout << "SDL could not initialize. SDL Error: " << SDL_GetError();
        quit = true;
        return;
    }
    Uint32 flags = SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN | SDL_WINDOW_MAXIMIZED;

    window = SDL_CreateWindow(
        "Elit3D Example",
        SDL_WINDOWPOS_UNDEFINED,
        SDL_WINDOWPOS_UNDEFINED,
        -1,
        -1,
        flags
    );

    if (window == nullptr) {
        std::cout << "Window could not be created. SDL Error: " << SDL_GetError();
        quit = true;
        return;
    }

    context = SDL_GL_CreateContext(window);

    if (context == nullptr) {
        std::cout << "OpenGL context could not be created! SDL Error: " << SDL_GetError();
        quit = true;
        return;
    }

    GLenum error = glewInit();
    if (error != GLEW_OK) {
        std::cout << "Unable to initialize OpenGL with glewInit()! Error: " << glewGetErrorString(error);
        quit = true;
        return;
    }

    if (SDL_GL_SetSwapInterval(1) < 0) {
        std::cout << "Warning: Unable to set VSync! SDL Error: %s" << SDL_GetError();
    }

    const char* defaultVertexShader = "#version 330 core\n"
        "layout (location = 0) in vec3 aPos;\n"
        "void main()\n"
        "{\n"
        "   gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);\n"
        "}\0";

    const char* fragmentShaderSource = "#version 330 core\n"
        "out vec4 FragColor;\n"
        "\n"
        "void main()\n"
        "{\n"
        "    FragColor = vec4(1.0f, 0.5f, 0.2f, 1.0f);\n"
        "} ";

    unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &defaultVertexShader, nullptr);
    glCompileShader(vertexShader);

    unsigned int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
    glCompileShader(fragmentShader);

    unsigned int shaderProgram = glCreateProgram();

    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);

    glUseProgram(shaderProgram);

    float vertices[] = {
        -.5f, -.5f, 0.f,
         .5f, -.5f, 0.f,
         0.f,  .5f, 0.f,
    };

    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);

    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
}

void App::CleanUp()
{
    SDL_DestroyWindow(window);
    SDL_Quit();
}

void App::Update()
{
    SDL_Event event;
    while (SDL_PollEvent(&event))
    {
        switch (event.type)
        {
        case SDL_QUIT:
            quit = true;
            break;
        default:
            break;
        }
    }

    glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glBindVertexArray(VAO);
    glDrawArrays(GL_TRIANGLES, 0, 3);

    SDL_GL_SwapWindow(window);
}
