#include <SDL2/SDL.h>
#include <SDL2/SDL_events.h>
#include <SDL2/SDL_hints.h>
#include <SDL2/SDL_mouse.h>
#include <SDL2/SDL_video.h>

#include <chrono>

#include <glm/gtx/transform.hpp>
#include <glm/gtx/rotate_vector.hpp>

#include "Config.hpp"
#include "KeyboardState.hpp"
#include "ReturnCode.hpp"
#include "VkResultString.hpp"
#include "ExtensionsAndLayers.hpp"
#include "Renderer.hpp"
#include "Camera.hpp"
#include "MouseState.hpp"
#include "Math.hpp"

SDL_Window* createWindow();

void genPolarX(std::vector<float>& x, float r, uint32_t numElems){
    float t = 2*PI/numElems;
    x.resize(numElems);
    for(size_t i = 0; i < numElems; i++){
        x[i] = r*cos(t*i);
    }
}

void genPolarY(std::vector<float>& y, float r, uint32_t numElems){
    float t = 2*PI/numElems;
    y.resize(numElems);
    for(size_t i = 0; i < numElems; i++){
        y[i] = r*sin(t*i);
    }
}

void genLinear(std::vector<float>& x, float start, float stop, uint32_t numElems){
    float step = (stop-start)/numElems;
    x.resize(numElems);
    for(size_t i = 0; i < numElems; i++)
        x[i] = start + i*step;
}

float genZ(float x, float y){
    float scale = 2;
    return sin(scale*(x*x + y*y))/scale;
}

float getRand(){
    return rand()/float(RAND_MAX);
}

glm::vec4 getRandColor(){
    static int i = 0;
    glm::vec4 col = {getRand(), getRand(), getRand(), 2};
    return glm::normalize(col);
}

glm::vec3 operator * (float s, const glm::vec3& v){
    return {s*v.x, s*v.y, s*v.z};
}

int main(){
    // create window to render to
    SDL_Window *window = createWindow();

    // create renderer
    Renderer renderer(window);

    renderer.uniformData.numPointLights = 9;
    for(size_t i = 0; i < 9; i++){
        renderer.uniformData.pointLights[i].color = getRandColor();
    }
    renderer.uniformData.ambient = {0.25, 0.5 , 1, 0.08};

    // set to false when need to exit game loop
    bool gameIsRunning = true;

    float linearSpeed = 10.f;
    float angularSpeed = 0.1f;
    float totalFrameTime = 0.f;
    uint64_t frameNumber = 0;

    // change in time from last frame
    float deltaTime;

    float fieldOfView = 45.f;
    int windowWidth = WINDOW_WIDTH, windowHeight = WINDOW_HEIGHT;
    float aspectRatio = static_cast<float>(windowWidth)/windowHeight;
    Camera camera(fieldOfView, aspectRatio, glm::vec3(0, 14, -25), 4*Camera::YAxis + 3*Camera::ZAxis, 12*Camera::ZAxis - 7*Camera::YAxis);

    glm::vec3 move;
    glm::vec2 rotation;
    glm::mat4 projectionMatrix = camera.getProjectionMatrix();
    glm::mat4 viewMatrix;

    // keep track of keyboard state
    KeyboardState keyboard;
    // keep track of mouse state
    MouseState mouse;

    Material defaultMaterial = *renderer.getMaterial("defaultMaterial");

    Mesh terrain;
    // createSphereMesh(terrain, 100, 100);
    // createRectangleMesh(terrain, 30, 30, {1, 0.5, 0.25});
    std::vector<float> X;
    genLinear(X, -5, 5, 50);
    createSurface(terrain, X, X, genZ);

    renderer.uploadMesh(terrain);

    RenderObject terrainObj(&terrain, &defaultMaterial);
   terrainObj.setScale({3, 3, 3});
//    terrainObj.setRotation(Camera::XAxis, 90);
    renderer.addRenderObject(terrainObj);
    std::cout << "Terrain Size : " << terrain.vertices.size() * sizeof(Vertex) + terrain.indices.size() * 4 << std::endl;

    // the game loop
    while(gameIsRunning){
        // get start time
        auto start = std::chrono::high_resolution_clock::now();

        // camera motion and rotation vectors
        move = glm::vec3(0);
        rotation = glm::vec2(0);

        // get all events one by one
        // poll event returns 0 when there are no events in the queue
        SDL_Event event;
        while(SDL_PollEvent(&event)){
            // check of quit event
            if(event.type == SDL_QUIT){
                gameIsRunning = false;
            }

            // NOTE : There is a minor bug here
            // When we start the window, aspecti ratio is not set properly and objects are rendered distored
            // to fix that I recreate swapchain when frame number is 0
            if(((event.type == SDL_WINDOWEVENT) && (event.window.event == SDL_WINDOWEVENT_RESIZED)) || (frameNumber == 0)){
                // inform renderer about
                renderer.windowResized();

                // update aspect ratio and projection matrix of camera
                camera.updateAspectRatio(window);
                projectionMatrix = camera.getProjectionMatrix();
            }

            // get keyboard state for each event
            if(keyboard.getState(event)){
                // process keyboard events
                if(keyboard.keyW == KeyState::Pressed){
                    move.x += linearSpeed;
                }else if(keyboard.keyS == KeyState::Pressed){
                    move.x -= linearSpeed;
                }

                if(keyboard.keyA == KeyState::Pressed){
                    move.y += linearSpeed;
                }else if(keyboard.keyD == KeyState::Pressed){
                    move.y -= linearSpeed;
                }

                if(keyboard.keyX == KeyState::Pressed){
                    move.z -= linearSpeed;
                }else if(keyboard.keyZ == KeyState::Pressed){
                    move.z += linearSpeed;
                }
            }else if(mouse.getState(event)){ // get mouse state
                // process mouse events
                if(mouse.leftBtn == MouseBtnState::Pressed && mouse.inMotion){
                    rotation.x = mouse.xrel * angularSpeed;
                    rotation.y -= mouse.yrel * angularSpeed;
                }
            }
        }

        // update camera orientation and location
        camera.update(move, rotation, deltaTime);

        for(size_t i = 0; i < 9; i++){
            float radius = 2+i;
            renderer.uniformData.pointLights[i].position = glm::vec4(sphericalToCartesian(radius, glm::radians(float(radius*frameNumber)), PI/2), 1) + glm::vec4{0, 2, 0, 0};
        }

        // update camera position
        renderer.uniformData.viewPosition = camera.getPosition();

        // update uniform data
        renderer.uniformData.viewMatrix = camera.getViewMatrix();
        renderer.uniformData.projectionMatrix = camera.getProjectionMatrix();

        // draw to screen
        renderer.draw();

        auto stop = std::chrono::high_resolution_clock::now();;
        deltaTime = std::chrono::duration_cast<std::chrono::milliseconds>(stop-start).count();
        totalFrameTime += deltaTime;
        frameNumber++;
    }

    float avgFrameTime = totalFrameTime / frameNumber;
    std::cout << "Average Frame Time : " << avgFrameTime << " milliseconds." << std::endl;

    // cleanup renderer
    renderer.cleanup();

    // destroy window
    SDL_DestroyWindow(window);

    // quit sdl
    SDL_Quit();

    return EXIT_SUCCESS;
}

SDL_Window* createWindow(){
    // if sdl isn't initialized then do it
    if(SDL_WasInit(SDL_INIT_VIDEO) == SDL_FALSE){
        // sdl init returns a negative number on failure
        if (SDL_Init(SDL_INIT_VIDEO) < 0) {
            std::cerr << "Failed to initialize SDL [" << SDL_GetError() << "]"
                      << std::endl;
        }
    }

    // create window
    SDL_Window *window = nullptr;

    // setup window flags
    uint32_t windowCreationFlags =
        SDL_WindowFlags::SDL_WINDOW_VULKAN |
        SDL_WindowFlags::SDL_WINDOW_SHOWN |
        SDL_WindowFlags::SDL_WINDOW_RESIZABLE |
        SDL_WindowFlags::SDL_WINDOW_ALLOW_HIGHDPI;

    // create sdl window
    window = SDL_CreateWindow(WINDOW_TITLE, SDL_WINDOWPOS_UNDEFINED,
                              SDL_WINDOWPOS_UNDEFINED, MIN_WINDOW_WIDTH,
                              MIN_WINDOW_HEIGHT, windowCreationFlags);

    // check if created or not
    if (window == nullptr) {
        std::cerr << "Failed to create game window : [" << SDL_GetError() << "]"
                  << std::endl;

        exit(-1);
    }

    // set minimum size for created window
    SDL_SetWindowMinimumSize(window, MIN_WINDOW_WIDTH, MIN_WINDOW_HEIGHT);

    return window;
}
