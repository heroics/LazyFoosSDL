#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <stdio.h>
#include <string>
using namespace std;
const int LEVEL_WIDTH = 1280;
const int LEVEL_HEIGHT = 960;
const int SCREEN_WIDTH = 640;
const int SCREEN_HEIGHT = 480;
class TextureWrapper
{
public:
    TextureWrapper();
    ~TextureWrapper();
    bool loadFromFile(string path);
#if defined(_SDL_TTF_H) || defined(SDL_TTF_H)
    bool loadFromRenderedText(string textureText, SDL_Color textColor);
#endif
    void free();
    void setColor(Uint8 red, Uint8 green, Uint8 blue);
    void setBlendMode(SDL_BlendMode blending);
    void setAlpha(Uint8 alpha);
    void render(int x, int y, SDL_Rect *clip = NULL, double angle = 0.0, SDL_Point *center = NULL, SDL_RendererFlip flip = SDL_FLIP_NONE);
    int getWidth();
    int getHeight();

private:
    SDL_Texture *texture;
    int width;
    int height;
};
class Dot
{
public:
    static const int DOT_WIDTH = 20;
    static const int DOT_HEIGHT = 20;
    static const int DOT_VELOCITY = 10;
    Dot();
    void handleInput(SDL_Event &event);
    void move();
    void render(int cameraX, int cameraY);
    int getPosX();
    int getPosY();

private:
    int positionX, positionY;
    int velocityX, velocityY;
};
bool init();
bool loadMedia();
void close();
SDL_Window *gWindow = NULL;
SDL_Renderer *gRenderer = NULL;
TextureWrapper gDotTexture;
TextureWrapper gBGTexture;
TextureWrapper::TextureWrapper()
{
    texture = NULL;
    width = 0;
    height = 0;
}
TextureWrapper::~TextureWrapper()
{
    free();
}
bool TextureWrapper::loadFromFile(string path)
{
    free();
    SDL_Texture *newTexture = NULL;
    SDL_Surface *loadedSurface = IMG_Load(path.c_str());
    if (loadedSurface == NULL)
    {
        printf("TEXTURE WRAPPER SDL_IMAGE [FAILED] - %s\n", SDL_GetError());
    }
    else
    {
        SDL_SetColorKey(loadedSurface, SDL_TRUE, SDL_MapRGB(loadedSurface->format, 0, 0xFF, 0xFF));
        newTexture = SDL_CreateTextureFromSurface(gRenderer, loadedSurface);
        if (newTexture == NULL)
        {
            printf("TEXTURE WRAPPER LOAD IMAGE [FAILED] - %s | SDL_IMAGE ERROR: %s\n",
                   path.c_str(), IMG_GetError());
        }
        else
        {
            width = loadedSurface->w;
            height = loadedSurface->h;
        }
        SDL_FreeSurface(loadedSurface);
    }
    texture = newTexture;
    return texture != NULL;
}
#if defined(_SDL_TTF_H) || defined(SDL_TTF_H)
bool TextureWrapper::loadFromRenderedText(string textureText, SDL_Color textColor)
{
    free();
    SDL_Surface *textSurface = TTF_RenderText_Solid(gFont, textureText.c_str(), textColor);
    if (textSurface != NULL)
    {
        texture = SDL_CreateTextureFromSurface(gRenderer, textSurface);
        if (texture == NULL)
        {
            printf("Unable to create texture from rendered text! SDL Error: %s\n", SDL_GetError());
        }
        else
        {
            width = textSurface->w;
            height = textSurface->h;
        }
        SDL_FreeSurface(textSurface);
    }
    else
    {
        printf("Unable to render text surface! SDL_ttf Error: %s\n", TTF_GetError());
    }
    return texture != NULL;
}
#endif
void TextureWrapper::free()
{
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
    SDL_SetTextureColorMod(texture, red, green, blue);
}
void TextureWrapper::setBlendMode(SDL_BlendMode blending)
{
    SDL_SetTextureBlendMode(texture, blending);
}
void TextureWrapper::setAlpha(Uint8 alpha)
{
    SDL_SetTextureAlphaMod(texture, alpha);
}
void TextureWrapper::render(int x, int y, SDL_Rect *clip, double angle, SDL_Point *center, SDL_RendererFlip rendererFlip)
{
    SDL_Rect renderQuad = {x, y, width, height};
    if (clip != NULL)
    {
        renderQuad.w = clip->w;
        renderQuad.h = clip->h;
    }
    SDL_RenderCopyEx(gRenderer, texture, clip, &renderQuad, angle, center, rendererFlip);
}
int TextureWrapper::getWidth() { return width; }
int TextureWrapper::getHeight() { return height; }
Dot::Dot()
{
    positionX = 0;
    positionY = 0;
    velocityX = 0;
    velocityY = 0;
}
void Dot::handleInput(SDL_Event &event)
{
    if (event.type == SDL_KEYDOWN && event.key.repeat == 0)
    {
        switch (event.key.keysym.sym)
        {
        case SDLK_UP:
        case SDLK_w:
            velocityY -= DOT_VELOCITY;
            break;
        case SDLK_DOWN:
        case SDLK_s:
            velocityY += DOT_VELOCITY;
            break;
        case SDLK_LEFT:
        case SDLK_a:
            velocityX -= DOT_VELOCITY;
            break;
        case SDLK_RIGHT:
        case SDLK_d:
            velocityX += DOT_VELOCITY;
            break;
        }
    }
    else if (event.type == SDL_KEYUP && event.key.repeat == 0)
    {
        switch (event.key.keysym.sym)
        {
        case SDLK_UP:
        case SDLK_w:
            velocityY += DOT_VELOCITY;
            break;
        case SDLK_DOWN:
        case SDLK_s:
            velocityY -= DOT_VELOCITY;
            break;
        case SDLK_LEFT:
        case SDLK_a:
            velocityX += DOT_VELOCITY;
            break;
        case SDLK_RIGHT:
        case SDLK_d:
            velocityX -= DOT_VELOCITY;
            break;
        }
    }
}
void Dot::move()
{
    positionX += velocityX;
    if ((positionX < 0) || (positionX + DOT_WIDTH > LEVEL_WIDTH))
    {
        positionX -= velocityX;
    }
    positionY += velocityY;
    if ((positionY < 0) || (positionY + DOT_HEIGHT > LEVEL_HEIGHT))
    {
        positionY -= velocityY;
    }
}
void Dot::render(int cameraX, int cameraY)
{
    gDotTexture.render(positionX - cameraX, positionY - cameraY);
}
int Dot::getPosX()
{
    return positionX;
}
int Dot::getPosY()
{
    return positionY;
}
bool init()
{
    bool success = true;
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) < 0)
    {
        printf("SDL [FAILED] - %s\n", SDL_GetError());
        success = false;
    }
    else
    {
        if (!SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "1"))
        {
            printf("Linear texturing filtering not enabled [WARNING]");
        }
        gWindow = SDL_CreateWindow("SDL Tutorial XXX", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
        if (gWindow == NULL)
        {
            printf("WINDOW [FAILED] - %s\n", SDL_GetError());
            success = false;
        }
        else
        {
            gRenderer = SDL_CreateRenderer(gWindow, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
            if (gRenderer == NULL)
            {
                printf("SDL_IMAGE [FAILED] - %s\n", IMG_GetError());
                success = false;
            }
            else
            {
                SDL_SetRenderDrawColor(gRenderer, 0xFF, 0xFF, 0xFF, 0xFF);
                int imgFlags = IMG_INIT_PNG;
                if (!(IMG_Init(imgFlags) & imgFlags))
                {
                    printf("SDL_image [FAILED] - %s\n", IMG_GetError());
                    success = false;
                }
            }
        }
    }
    return success;
}
bool loadMedia()
{
    bool success = true;
    if (!gDotTexture.loadFromFile("images/dot.bmp"))
    {
        printf("DOT TEXTURE [FAILED]");
        success = false;
    }
    if (!gBGTexture.loadFromFile("images/bg.png"))
    {
        printf("Failed to load background texture!\n");
        success = false;
    }
    return success;
}
void close()
{
    gDotTexture.free();
    gBGTexture.free();
    SDL_DestroyRenderer(gRenderer);
    SDL_DestroyWindow(gWindow);
    gWindow = NULL;
    gRenderer = NULL;
    IMG_Quit();
    SDL_Quit();
}
int main(int argc, char const *args[])
{
    if (!init())
    {
        printf("init [FAILED]");
    }
    else
    {
        if (!loadMedia())
        {
            printf("Load media [FAILED]");
        }
        else
        {
            bool quit = false;
            SDL_Event eventHandler;
            Dot dot;
            SDL_Rect camera = {0, 0, SCREEN_WIDTH, SCREEN_HEIGHT};
            while (!quit)
            {
                while (SDL_PollEvent(&eventHandler) != 0)
                {
                    if (eventHandler.type == SDL_QUIT)
                    {
                        quit = true;
                    }
                    dot.handleInput(eventHandler);
                }
                dot.move();
                camera.x = (dot.getPosX() + Dot::DOT_WIDTH / 2) - SCREEN_WIDTH / 2;
                camera.y = (dot.getPosY() + Dot::DOT_HEIGHT / 2) - SCREEN_HEIGHT / 2;
                if (camera.x < 0)
                {
                    camera.x = 0;
                }
                if (camera.y < 0)
                {
                    camera.y = 0;
                }
                if (camera.x > LEVEL_WIDTH - camera.w)
                {
                    camera.x = LEVEL_WIDTH - camera.w;
                }
                if (camera.y > LEVEL_HEIGHT - camera.h)
                {
                    camera.y = LEVEL_HEIGHT - camera.h;
                }
                SDL_SetRenderDrawColor(gRenderer, 0xFF, 0xFF, 0xFF, 0xFF);
                SDL_RenderClear(gRenderer);
                gBGTexture.render(0, 0, &camera);
                dot.render(camera.x, camera.y);
                SDL_RenderPresent(gRenderer);
            }
        }
    }
    close();
    return 0;
}