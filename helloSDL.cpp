// Using SDL and Standard IO
#include <SDL2/SDL.h>

#include <SDL2/SDL_image.h>

#include <stdio.h>

#include <string>

using namespace std;

// Screen dimension contants
const int SCREEN_WIDTH = 640;
const int SCREEN_HEIGHT = 480;

// Starts up SDL and create window
bool init();

// Load media
bool loadMedia();

// Frees Media and shuts down SDL
void close();

// Loads Individual image
SDL_Surface *loadSurface(string path);

// Rendering Window
SDL_Window *gWindow = NULL;

// The Window Renderer
SDL_Renderer *gRenderer = NULL;

// Current displayed texture
SDL_Texture *gTexture = NULL;

bool init()
{
    // Initialization flag
    bool success = true;

    // Initialize SDL
    if (SDL_Init(SDL_INIT_VIDEO) < 0)
    {
        printf("SDL [FAILED] - %s\n", SDL_GetError());
        success = false;
    }
    else
    {
        // Set texture filtering to linear
        if (!SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "1"))
        {
            printf("Linear texturing filtering not enabled [WARNING]");
        }

        // Create window
        gWindow = SDL_CreateWindow("SDL Tutorial VIII", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);

        if (gWindow == NULL)
        {
            printf("WINDOW [FAILED] - %s\n", SDL_GetError());
            success = false;
        }
        else
        {
            // Create renderer for window
            gRenderer = SDL_CreateRenderer(gWindow, -1, SDL_RENDERER_ACCELERATED);
            if (gRenderer == NULL)
            {
                printf("SDL_IMAGE [FAILED] - %s\n", IMG_GetError());
                success = false;
            }
            else
            {
                //Initialize renderer color
                SDL_SetRenderDrawColor(gRenderer, 0xFF, 0xFF, 0xFF, 0xFF);

                //Initialize PNG loading
                int imgFlags = IMG_INIT_PNG;
                if (!(IMG_Init(imgFlags) & imgFlags))
                {
                    printf("SDL_image [FAILED] - %s", IMG_GetError());
                    success = false;
                }
            }
        }
    }
    return success;
}

SDL_Texture *loadTexture(string path)
{
    // The final texture
    SDL_Texture *newTexture = NULL;

    // Load image at specified path
    SDL_Surface *loadedSurface = IMG_Load(path.c_str());
    if (loadSurface == NULL)
    {
        printf("Load Surface [Failed] - %s\n", SDL_GetError());
    }
    else
    {
        // Create texture from surface pixels
        newTexture = SDL_CreateTextureFromSurface(gRenderer, loadedSurface);
        if (newTexture == NULL)
        {
            printf("Image Optimization: %s [FAILED] - %s ", path.c_str(), SDL_GetError());
        }
        // Free old surface
        SDL_FreeSurface(loadedSurface);
    }
    return newTexture;
}

bool loadMedia()
{
    return true;
}

void close()
{
    // Destory window
    SDL_DestroyWindow(gWindow);
    SDL_DestroyRenderer(gRenderer);
    gRenderer = NULL;
    gWindow = NULL;

    // Quit SDL subsystems
    IMG_Quit();
    SDL_Quit();
}

int main(int argc, char const *argv[])
{
    // Start up SDL and Create Window
    if (!init())
    {
        printf("init [FAILED]");
    }
    else
    {
        // Load media
        if (!loadMedia())
        {
            printf("Load media [FAILED]");
        }
        else
        {
            // Main loop flag
            bool quit = false;

            // Event Handler
            SDL_Event eventHandler;

            // While application is running
            while (!quit)
            {
                // Handle events on queue
                while (SDL_PollEvent(&eventHandler) != 0)
                {
                    // User requests quit
                    if (eventHandler.type == SDL_QUIT)
                    {
                        quit = true;
                    }
                }

                //Clear screen
                SDL_SetRenderDrawColor(gRenderer, 0xFF, 0xFF, 0xFF, 0xFF);
                SDL_RenderClear(gRenderer);

                // Top Left Corner Viewport
                SDL_Rect topLeftViewPort;
                topLeftViewPort.x = 0;
                topLeftViewPort.y = 0;
                topLeftViewPort.w = SCREEN_WIDTH / 2;
                topLeftViewPort.h = SCREEN_HEIGHT / 2;
                SDL_RenderSetViewport(gRenderer, &topLeftViewPort);

                // Render texture to screen
                SDL_RenderCopy(gRenderer, gTexture, NULL, NULL);

                // Bottom viewport
                SDL_Rect bottomViewPort;
                bottomViewPort.x = 0;
                bottomViewPort.y = SCREEN_HEIGHT / 2;
                bottomViewPort.w = SCREEN_WIDTH;
                bottomViewPort.h = SCREEN_HEIGHT / 2;
                SDL_RenderSetViewport(gRenderer, &bottomViewPort);

                

                // Update screen
                SDL_RenderPresent(gRenderer);
            }
        }
    }
    close();
    return 0;
}