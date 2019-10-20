// Using SDL and Standard IO
#include <SDL2/SDL.h>

#include <SDL2/SDL_image.h>

#include <stdio.h>

#include <string>

using namespace std;

// Screen dimension contants
const int SCREEN_WIDTH = 640;
const int SCREEN_HEIGHT = 480;

// Texture Wrapper Class
class TextureWrapper
{
public:
    // Initializes variables
    TextureWrapper();

    // Deallocates Memory
    ~TextureWrapper();

    // Loads image at specified path
    bool loadFromFile(string path);

    // Deallocates texture
    void free();

    // Set color modulation
    void setColor(Uint8 red, Uint8 green, Uint8 blue);

    // Render texture at given point
    void render(int x, int y, SDL_Rect *clip = NULL);

    // Get image dimensions
    int getWidth();
    int getHeight();

private:
    // The actual hardware texture
    SDL_Texture *texture;

    // Image dimension
    int width;
    int height;
};

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
SDL_Texture *gOtherTexture = NULL;

// Scene Texture
TextureWrapper gModulationTexture;

TextureWrapper::TextureWrapper()
{
    // Initialize
    texture = NULL;
    width = 0;
    height = 0;
}

TextureWrapper::~TextureWrapper()
{
    // Deallocate
    free();
}

bool TextureWrapper::loadFromFile(string path)
{
    // Get rid of preexisting texture
    free();

    // The final texture
    SDL_Texture *newTexture = NULL;

    // Load image at specified path
    SDL_Surface *loadedSuface = IMG_Load(path.c_str());
    if (loadedSuface == NULL)
    {
        printf("TEXTURE WRAPPER SDL_IMAGE [FAILED] - %s\n", SDL_GetError());
    }
    else
    {
        // Color key image
        SDL_SetColorKey(loadedSuface, SDL_TRUE, SDL_MapRGB(loadedSuface->format, 0, 0xFF, 0xFF));

        // Create texture from surface pixels
        newTexture = SDL_CreateTextureFromSurface(gRenderer, loadedSuface);
        if (newTexture == NULL)
        {
            printf("TEXTURE WRAPPER LOAD IMAGE [FAILED] - %s | SDL_IMAGE ERROR: %s\n", path.c_str(), IMG_GetError());
        }
        else
        {
            // Get image dimensions
            width = loadedSuface->w;
            height = loadedSuface->h;
        }
        // Get rid of old loaded surface
        SDL_FreeSurface(loadedSuface);
    }
    // Return success
    texture = newTexture;
    return texture != NULL;
}

void TextureWrapper::free()
{
    // Free texture if it exists
    if (texture != NULL)
    {
        SDL_DestroyTexture(texture);
        texture = NULL;
        width = 0;
        height = 0;
    }
}

void TextureWrapper::render(int x, int y, SDL_Rect *clip)
{
    // Set Rendering space and render to screen
    SDL_Rect renderQuad = {
        x,
        y,
        width,
        height};

    // Set clip rendering dimension
    if (clip != NULL)
    {
        renderQuad.w = clip->w;
        renderQuad.h = clip->h;
    }

    // Render to screen
    SDL_RenderCopy(gRenderer, texture, clip, &renderQuad);
}

void TextureWrapper::setColor(Uint8 red, Uint8 green, Uint8 blue)
{
    // Modulate Texture
    SDL_SetTextureColorMod(texture, red, green, blue);
}

int TextureWrapper::getWidth()
{
    return width;
}

int TextureWrapper::getHeight()
{
    return height;
}

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
        gWindow = SDL_CreateWindow("SDL Tutorial XII", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);

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
    // Loading success flag
    bool success = true;

    // Load Player texture
    if (!gModulationTexture.loadFromFile("images/colorsheet.png"))
    {
        printf("Sprite Sheet [FAILED]!\n");
        success = false;
    }
    return success;
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

            // Modulation components
            Uint8 red = 255;
            Uint8 green = 255;
            Uint8 blue = 255;

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
                    // On keypress  change RBG Values
                    else if (eventHandler.type == SDL_KEYDOWN)
                    {
                        switch (eventHandler.key.keysym.sym)
                        {
                        // Increase red
                        case SDLK_q:
                            red += 32;
                            break;
                        // Increase green
                        case SDLK_w:
                            green += 32;
                            break;
                        // Increase blue
                        case SDLK_e:
                            blue += 32;
                            break;
                        // Decrease red
                        case SDLK_a:
                            red -= 32;
                            break;
                        // Decrease green
                        case SDLK_s:
                            green -= 32;
                            break;
                        // Decrease blue
                        case SDLK_d:
                            blue -= 32;
                            break;
                        }
                    }
                }

                // Clear screen
                SDL_SetRenderDrawColor(gRenderer, 0xFF, 0xFF, 0xFF, 0xFF);
                SDL_RenderClear(gRenderer);

                // Modulate and render texture
                gModulationTexture.setColor(red, green, blue);
                gModulationTexture.render(0, 0);

                // Update screen
                SDL_RenderPresent(gRenderer);
            }
        }
    }
    close();
    return 0;
}
