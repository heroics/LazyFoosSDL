// Using SDL and Standard IO
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>
#include <stdio.h>
#include <string>
#include <cmath>

using namespace std;

// Screen dimension contants
const int SCREEN_WIDTH = 640;
const int SCREEN_HEIGHT = 480;
const int WALKING_ANIMATION_FRAMES = 4;

// Loads Individual image
SDL_Surface *loadSurface(string path);

// Rendering Window
SDL_Window *gWindow = NULL;

// The Window Renderer
SDL_Renderer *gRenderer = NULL;

// Globally used font
TTF_Font *gFont = NULL;

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

    // Creates image from font string
    bool loadFromRenderedText(string textureText, SDL_Color textColor);

    // Deallocates texture
    void free();

    // Set color modulation
    void setColor(Uint8 red, Uint8 green, Uint8 blue);

    // Set Blending
    void setBlendMode(SDL_BlendMode blending);

    // Set Alpha Modulation
    void setAlpha(Uint8 alpha);

    // Render texture at given point
    void render(int x, int y, SDL_Rect *clip = NULL, double angle = 0.0, SDL_Point *center = NULL, SDL_RendererFlip flip = SDL_FLIP_NONE);

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

// Rendered Texture
TextureWrapper gTextTexture;

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

bool TextureWrapper::loadFromRenderedText(string textureText, SDL_Color textColor)
{
    // Get red of preexisting texture
    free();

    // Render text surface
    SDL_Surface *textSurface = TTF_RenderText_Blended(gFont, textureText.c_str(), textColor);

    if (textSurface == NULL)
    {
        printf("Render Text Surface [FAILED] - %s\n", TTF_GetError());
    }
    else
    {
        // Create texture from surface pixel
        texture = SDL_CreateTextureFromSurface(gRenderer, textSurface);
        if (texture == NULL)
        {
            printf("Render Texture Text [FAILED] - %s\n");
        }
        else
        {

            // Get image dimensions
            width = textSurface->w;
            height = textSurface->h;
        }
        // Get rid of old surface
        SDL_FreeSurface(textSurface);
    }
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

void TextureWrapper::render(int x, int y, SDL_Rect *clip, double angle, SDL_Point *center, SDL_RendererFlip rendererFlip)
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
    SDL_RenderCopyEx(gRenderer, texture, clip, &renderQuad, angle, center, rendererFlip);
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

// Starts up SDL and create window
bool init();

// Load media
bool loadMedia();

// Frees Media and shuts down SDL
void close();

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
        gWindow = SDL_CreateWindow("SDL Tutorial XVI", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);

        if (gWindow == NULL)
        {
            printf("WINDOW [FAILED] - %s\n", SDL_GetError());
            success = false;
        }
        else
        {
            // Create renderer for window
            gRenderer = SDL_CreateRenderer(gWindow, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
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
                    printf("SDL_image [FAILED] - %s\n", IMG_GetError());
                    success = false;
                }

                // Initialize SDL_TTF =
                if (TTF_Init() == -1)
                {
                    printf("SDL_TTF [FAILED] - %s\n", TTF_GetError());
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
            printf("Image Optimization: %s [FAILED] - %s\n", path.c_str(), SDL_GetError());
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

    // Open the Font
    gFont = TTF_OpenFont("fonts/gamefont.ttf", 28);

    if (gFont == NULL)
    {
        printf("Font Load [FAILED] - %s\n", TTF_GetError());
    }
    else
    {
        // Render Text
        SDL_Color textColor = {0, 0, 0};
        if (!gTextTexture.loadFromRenderedText("You are here because you want to be.", textColor))
        {
            printf("Text Render [FAILED]");
            success = false;
        }
    }

    return success;
}

void close()
{
    // Free loaded images
    gTextTexture.free();

    // Free global font
    TTF_CloseFont(gFont);
    gFont = NULL;

    // Destory window
    SDL_DestroyWindow(gWindow);
    SDL_DestroyRenderer(gRenderer);

    gRenderer = NULL;
    gWindow = NULL;

    // Quit SDL subsystems
    TTF_Quit();
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

            // Current animation frame
            double degrees = 0.0;

            // Flip type
            SDL_RendererFlip flipType = SDL_FLIP_NONE;

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

                // Clear screen
                SDL_SetRenderDrawColor(gRenderer, 0xFF, 0xFF, 0xFF, 0xFF);
                SDL_RenderClear(gRenderer);

                // Render current frame
                gTextTexture.render((SCREEN_WIDTH - gTextTexture.getWidth()) / 2, (SCREEN_HEIGHT - gTextTexture.getHeight()) / 2);
                // Update screen
                SDL_RenderPresent(gRenderer);
            }
        }
    }
    close();
    return 0;
}
