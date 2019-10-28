// Using SDL and Standard IO
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_mixer.h>
#include <stdio.h>
#include <string>

using namespace std;

// Screen dimension contants
const int SCREEN_WIDTH = 640;
const int SCREEN_HEIGHT = 480;

// Button contants
const int BUTTON_WIDTH = 300;
const int BUTTON_HEIGHT = 200;

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
TextureWrapper gPromptTexture;

//The music that will be played
Mix_Music *gMusic = NULL;

//The sound effects that will be used
Mix_Chunk *gScratch = NULL;
Mix_Chunk *gHigh = NULL;
Mix_Chunk *gMedium = NULL;
Mix_Chunk *gLow = NULL;

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
        gWindow = SDL_CreateWindow("SDL Tutorial XXI", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);

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

                // Initialize SDL_mixer
                if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0)
                {
                    printf("SDL_MIXER [FAILED] - %s\n");
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

    // Load sprites
    if (!gPromptTexture.loadFromFile("images/prompt.png"))
    {
        printf("SPRITE LOAD [FAILED]");
        success = false;
    }

    // Load Music
    gMusic = Mix_LoadMUS("audio/beat.wav");
    if (gMusic == NULL)
    {
        printf("BEAT [FAILED] - %s\n", Mix_GetError());
        success = false;
    }

    // Load sound effects
    gScratch = Mix_LoadWAV("audio/scratch.wav");
    if (gScratch == NULL)
    {
        printf("SCRATCH FAILED [FAILED] - %s\n", Mix_GetError());
        success = false;
    }

    gHigh = Mix_LoadWAV("audio/high.wav");
    if (gHigh == NULL)
    {
        printf("HIGH [FAILED] - %s\n");
        success = false;
    }
    gMedium = Mix_LoadWAV("audio/medium.wav");
    if (gMedium == NULL)
    {
        printf("MEDIUM [FAILED] - %s\n");
        success = false;
    }
    gLow = Mix_LoadWAV("audio/low.wav");
    if (gLow == NULL)
    {
        printf("LOW [FAILED] - %s\n");
        success = false;
    }
    return success;
}

void close()
{
    // Free loaded images
    gPromptTexture.free();

    //Free the sound effects
    Mix_FreeChunk(gScratch);
    Mix_FreeChunk(gHigh);
    Mix_FreeChunk(gMedium);
    Mix_FreeChunk(gLow);
    gScratch = NULL;
    gHigh = NULL;
    gMedium = NULL;
    gLow = NULL;

    //Free the music
    Mix_FreeMusic(gMusic);
    gMusic = NULL;

    // Destory window
    SDL_DestroyWindow(gWindow);
    SDL_DestroyRenderer(gRenderer);

    gRenderer = NULL;
    gWindow = NULL;

    // Quit SDL subsystems
    Mix_Quit();
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

            // Current rendered texture
            TextureWrapper *currentTexture = NULL;

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
                    //Handle key press
                    else if (eventHandler.type == SDL_KEYDOWN)
                    {
                        switch (eventHandler.key.keysym.sym)
                        {
                        //Play high sound effect
                        case SDLK_1:
                            Mix_PlayChannel(-1, gHigh, 0);
                            break;

                        //Play medium sound effect
                        case SDLK_2:
                            Mix_PlayChannel(-1, gMedium, 0);
                            break;

                        //Play low sound effect
                        case SDLK_3:
                            Mix_PlayChannel(-1, gLow, 0);
                            break;

                        //Play scratch sound effect
                        case SDLK_4:
                            Mix_PlayChannel(-1, gScratch, 0);
                            break;

                        case SDLK_9:
                            //If there is no music playing
                            if (Mix_PlayingMusic() == 0)
                            {
                                //Play the music
                                Mix_PlayMusic(gMusic, -1);
                            }
                            //If music is being played
                            else
                            {
                                //If the music is paused
                                if (Mix_PausedMusic() == 1)
                                {
                                    //Resume the music
                                    Mix_ResumeMusic();
                                }
                                //If the music is playing
                                else
                                {
                                    //Pause the music
                                    Mix_PauseMusic();
                                }
                            }
                            break;

                        case SDLK_0:
                            //Stop the music
                            Mix_HaltMusic();
                            break;
                        }
                    }
                }

                // Clear screen
                SDL_SetRenderDrawColor(gRenderer, 0xFF, 0xFF, 0xFF, 0xFF);
                SDL_RenderClear(gRenderer);

                //Render prompt
                gPromptTexture.render(0, 0);

                // Update screen
                SDL_RenderPresent(gRenderer);
            }
        }
    }
    close();
    return 0;
}
