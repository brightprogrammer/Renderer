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

SDL_Window* createWindow();
bool handleEvents();

int main(){
    // create window to render to
    SDL_Window *window = createWindow();

    // create renderer
    Renderer renderer(window);

    renderer.uniformData.lightPosition = {1, 1, 1};
    renderer.uniformData.lightColor = {1, 0.5, 0.25, 2};
    renderer.uniformData.ambientLightColor = {0.25, 0.5 , 1, 0.08};

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
    Camera camera(fieldOfView, aspectRatio, glm::vec3(0, 0, 0), Camera::YAxis, Camera::ZAxis);

    glm::vec3 move;
    glm::vec2 rotation;
    glm::mat4 projectionMatrix = camera.getProjectionMatrix();
    glm::mat4 viewMatrix;

    // keep track of keyboard state
    KeyboardState keyboard;
    // keep track of mouse state
    MouseState mouse;

    float radius = 2.5;

    Mesh apple;
    Material defaultMaterial = *renderer.getMaterial("defaultMaterial");

    // if(apple.loadFromObj("../assets/apple.obj") != FAILED){
    //     renderer.uploadMesh(apple);

    //     RenderObject obj(&apple, &defaultMaterial);
    //     obj.setPosition({1, 0, 3});
    //     obj.move({0, 0.2, -0.1});
    //     obj.setScale({12, 12, 12});
    //     obj.setRotation({0,0,1}, 180);
    //     renderer.addRenderObject(obj);
    // }

    // create sphere mesh
    Mesh sphere;

    // create sphere mesh
    uint32_t slices = 4, circles = 4;
    createSphereMesh(sphere, slices, circles, {0, 1, 1});

    // upload mesh data to gpu
    renderer.uploadMesh(sphere);

    // create renderable object
    RenderObject sphereObj(&sphere, &defaultMaterial);
    sphereObj.setPosition({-1, 0, 4});

    renderer.addRenderObject(sphereObj);

    // create a plane
    Mesh plane;

    float width = 5;
    float height = 5;
    createRectangleMesh(plane, width, height, {1, 0, 0});

    // upload mesh data
    renderer.uploadMesh(plane);

    // create renderable object
    RenderObject planeObj(&plane, &defaultMaterial);
    planeObj.setRotation({1, 0 ,0}, -90); // rotate about x axis 90 degrees
    planeObj.setPosition({0, -2, 2});

    renderer.addRenderObject(planeObj);

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

        // update light position
        renderer.uniformData.lightPosition = {-1 + radius * sin(glm::radians(float(frameNumber))), 0, 4 + radius * cos(glm::radians(float(frameNumber)))};

        // update camera position
        renderer.uniformData.viewPosition = camera.getPosition();

        // update camera view
        viewMatrix = camera.getViewMatrix();
        // update uniform data
        renderer.uniformData.projectionViewMatrix = projectionMatrix * viewMatrix;

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
