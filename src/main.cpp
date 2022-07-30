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

int main(){
    // create window to render to
    SDL_Window *window = createWindow();

    // create renderer
    Renderer renderer(window);

    // set to false when need to exit game loop
    bool gameIsRunning = true;

    float linearSpeed = 1000.f;
    float angularSpeed = 20.f;
    float avgFrameTime = 0.f;
    size_t frameNumber = 1;

    // change in time from last frame
    float deltaTime;

    float fieldOfView = 70.f;
    float aspectRatio = (float)WINDOW_WIDTH/WINDOW_HEIGHT;
    Camera camera(fieldOfView, aspectRatio, glm::vec3(-20, -20, -20), Camera::ZAxis + Camera::YAxis + Camera::XAxis, Camera::YAxis);

    glm::vec3 move;
    glm::vec2 rotation;
    renderer.cameraData.projection = camera.getProjectionMatrix();

    // keep track of keyboard state
    KeyboardState keyboard;
    // keep track of mouse state
    MouseState mouse;

    // float mouseX = 0, mouseY = 0;
    int32_t windowWidth = WINDOW_WIDTH, windowHeight = WINDOW_HEIGHT;

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

            if(event.type == SDL_WINDOWEVENT && event.window.type == SDL_WINDOWEVENT_RESIZED){
                // inform renderer about
                renderer.windowResized();

                // get window size
                SDL_GetWindowSize(window, &windowWidth, &windowHeight);

                // update camera aspect ratio
                aspectRatio = (float)windowWidth/windowHeight;
                camera.setAspectRatio(aspectRatio);

                // update projection matrix in renderer
                renderer.cameraData.projection = camera.getProjectionMatrix();
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
                    move.y -= linearSpeed;
                }else if(keyboard.keyD == KeyState::Pressed){
                    move.y += linearSpeed;
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
                    rotation.y = mouse.yrel * angularSpeed;
                }
            }
        }

        // update camera orientation and location
        camera.update(move, rotation, deltaTime/100);

        // update camera view
        renderer.cameraData.view = camera.getViewMatrix();

        // draw to screen
        renderer.draw();

        auto stop = std::chrono::high_resolution_clock::now();;
        deltaTime = std::chrono::duration_cast<std::chrono::milliseconds>(stop-start).count();
        avgFrameTime = avgFrameTime * frameNumber + deltaTime;
        avgFrameTime /= ++frameNumber;
    }

    std::cout << "Average Frame Time : " << avgFrameTime << std::endl;

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
