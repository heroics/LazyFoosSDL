// Using SDL and Standard IO
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>
#include <sstream>
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

#if defined(_SDL_TTF_H) || defined(SDL_TTF_H)
    // Creates image from font string
    bool loadFromRenderedText(string textureText, SDL_Color textColor);
#endif

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

// The application time based timer
class Timer
{
public:
    // Initializes variables
    Timer();

    // Clock Actions
    void start();
    void stop();
    void pause();
    void unpause();

    // Gets the timer's current time
    Uint32 getTicks();

    // Checks the status of the timer
    bool isStarted();
    bool isPaused();

private:
    // The clock time when started
    Uint32 startTicks;

    // The ticks stored when the time timer was paused
    Uint32 pausedTicks;

    // The timer status
    bool started;
    bool paused;
};

// Starts up SDL and create window
bool init();

// Load media
bool loadMedia();

// Frees Media and shuts down SDL
void close();

// Rendering Window
SDL_Window *gWindow = NULL;

// The Window Renderer
SDL_Renderer *gRenderer = NULL;

//Scene texture
TTF_Font *gFont = NULL;

// Scene Texture
TextureWrapper gTimeTexture;
TextureWrapper gFPSTexture;

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

#if defined(_SDL_TTF_H) || defined(SDL_TTF_H)
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
#endif

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

void TextureWrapper::setColor(Uint8 red, Uint8 green, Uint8 blue)
{
    // Modulate Texture
    SDL_SetTextureColorMod(texture, red, green, blue);
}
void TextureWrapper::setBlendMode(SDL_BlendMode blending)
{
    //Set blending function
    SDL_SetTextureBlendMode(texture, blending);
}

void TextureWrapper::setAlpha(Uint8 alpha)
{
    SDL_SetTextureAlphaMod(texture, alpha);
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

int TextureWrapper::getWidth()
{
    return width;
}

int TextureWrapper::getHeight()
{
    return height;
}

Timer::Timer()
{
    // Initialize the variables
    startTicks = 0;
    pausedTicks = 0;

    paused = false;
    started = false;
}

void Timer::start()
{
    // Start the timer
    started = true;

    // Unpaused the timer
    paused = false;

    // Get the current clock time
    startTicks = SDL_GetTicks();
    pausedTicks = 0;
}

void Timer::stop()
{
    // Stop the timer
    started = false;

    // Unpaused the timer
    paused = false;

    // Clear the tick variables
    startTicks = 0;
    pausedTicks = 0;
}

void Timer::pause()
{
    // If the timer is running and paused
    if (started && !paused)
    {
        // Pause the timer
        paused = true;

        // Calculate the paused ticks
        pausedTicks = SDL_GetTicks() - startTicks;
        startTicks = 0;
    }
}

void Timer::unpause()
{
    // If the timer is running and paused
    if (started && paused)
    {
        // Unpause the timer
        paused = false;

        // Calculate the paused ticks
        startTicks = SDL_GetTicks() - pausedTicks;

        // Reset the paused ticks
        pausedTicks = 0;
    }
}

Uint32 Timer::getTicks()
{
    // The actual timer time
    Uint32 time = 0;

    // If the timer is running
    if (started)
    {
        // If the timer is paused
        if (paused)
        {
            // Return the number of ticks when the timer was paused
            time = pausedTicks;
        }
        else
        {
            // Return the current time minus the start time
            time = SDL_GetTicks() - startTicks;
        }
    }
    return time;
}

bool Timer::isStarted()
{
    // Timer is running and paused or unpaused
    return started;
}

bool Timer::isPaused()
{
    // Timer is running and paused
    return paused && started;
}

bool init()
{
    // Initialization flag
    bool success = true;

    // Initialize SDL
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) < 0)
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
        gWindow = SDL_CreateWindow("SDL Tutorial XXIV", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);

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

                // Initialize SDL_ttf
                if (TTF_Init() == -1)
                {
                    printf("SDL_TTF could not initialize! SDL_TTF Error: %s \n", TTF_GetError());
                }
            }
        }
    }
    return success;
}

bool loadMedia()
{
    // Loading success flag
    bool success = true;

    // Open the font
    gFont = TTF_OpenFont("fonts/gamefont.ttf", 14);

    if (gFont == NULL)
    {
        printf("LAZY LOAD [FAILED] - %s\n", TTF_GetError());
        success = false;
    }
    else
    {
        // Set Text color as black
        SDL_Color textColor = {0, 0, 0, 255};
        // Load prompt texture
        if (!gFPSTexture.loadFromRenderedText("Jordan", textColor))
        {
            printf("TEXT RENDER [FAILED]\n");
            success = false;
        }
    }

    return success;
}

void close()
{
    // Free loaded images
    gTimeTexture.free();
    gFPSTexture.free();

    // Free Global Font
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

            // Set text color as black
            SDL_Color textColor = {0, 0, 0, 255};

            // The application timer
            Timer fpsTimer;

            // In memory text stream
            stringstream timeText;

            // Current time start time
            Uint32 startTime = 0;
            fpsTimer.start();

            // While application is running
            while (!quit)
            {
                // Handle events on queue

                while (SDL_PollEvent(&eventHandler) != 0)
                {
                    //User requests quit
                    if (eventHandler.type == SDL_QUIT)
                    {
                        quit = true;
                    }
                }

                // Calculate and correct fps
                float averageFPS = startTime / (fpsTimer.getTicks() / 1000.f);
                if (averageFPS > 2000000)
                {
                    averageFPS = 0;
                }

                // Set text to be rendered
                timeText.str("");
                timeText << "Average Frames Per Second " << averageFPS;

                //Render text
                if (!gFPSTexture.loadFromRenderedText(timeText.str().c_str(), textColor))
                {
                    printf("Unable to render time texture!\n");
                }

                // Clear screen
                SDL_SetRenderDrawColor(gRenderer, 0xFF, 0xFF, 0xFF, 0xFF);
                SDL_RenderClear(gRenderer);

                //Render textures
                gFPSTexture.render((SCREEN_WIDTH - gFPSTexture.getWidth()) / 2, 0);

                //Update screen
                SDL_RenderPresent(gRenderer);
                startTime++;
            }
        }
    }
    close();
    return 0;
}
