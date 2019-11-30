// Using SDL and Standard IO
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>
#include <sstream>
#include <stdio.h>
#include <string>
#include <vector>
using namespace std;
// Screen dimension contants
const int SCREEN_WIDTH = 640;
const int SCREEN_HEIGHT = 480;
const int SCREEN_FPS = 60;
const int SCREEN_TICK_PER_FRAME = 1000 / SCREEN_FPS;
// A circle structure
struct Circle
{
    int x, y, radius;
};
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
    void render(int x, int y, SDL_Rect *clip = NULL, double angle = 0.0,
                SDL_Point *center = NULL, SDL_RendererFlip flip = SDL_FLIP_NONE);
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
class Dot
{
public:
    // The dimensions of the dot
    static const int DOT_WIDTH = 20;
    static const int DOT_HEIGHT = 20;
    // Maximum axis velocity of the dot
    static const double constexpr DOT_VELOCITY = .3f;
    // Initializes the Variables
    Dot(int x, int y);
    // Takes keys presses and adjust the dot's velocity
    void handleInput(SDL_Event &event);
    // Move the Dot
    void move(SDL_Rect &otherCollider, Circle &circle);
    // Draws the dot on the screen
    void render();
    // Gets the collision circle
    Circle &getColliders();

private:
    // The X and Y offsets of the dot
    double positionX, positionY;
    // The Velocity of the dots
    float velocityX, velocityY;
    // Move the collision boxes
    Circle colliders;
    // Moves the collision boxes relative to the dot's offset
    void shiftColliders();
};
// Starts up SDL and create window
bool init();
// Load media
bool loadMedia();
// Frees Media and shuts down SDL
void close();
// Circle/Circle collision detector
bool checkCollision(Circle &a, Circle &b);
// Circle/Box collision detector
bool checkCollision(Circle &a, SDL_Rect &b);
// Calculates distance squared between two points
double distanceSquared(int x1, int y1, int x2, int y2);
// Rendering Window
SDL_Window *gWindow = NULL;
// The Window Renderer
SDL_Renderer *gRenderer = NULL;
// Scene texture
TTF_Font *gFont = NULL;
// Scene Texture
TextureWrapper gDotTexture;
Dot::Dot(int x, int y)
{
    // Initialize the offsets
    positionX = x;
    positionY = y;
    // Initialize the velocity
    velocityX = 0;
    velocityY = 0;
    // Set collision circle size
    colliders.radius = DOT_WIDTH / 2;
    // Initialize colliders relative to postion
    shiftColliders();
}
void Dot::handleInput(SDL_Event &event)
{
    // If a key was pressed
    if (event.type == SDL_KEYDOWN && event.key.repeat == 0)
    {
        // Adjust the velocity
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
        // Adjust the velocity
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
void Dot::move(SDL_Rect &square, Circle &circle)
{
    // Move the dot left or right
    positionX += velocityX;
    shiftColliders();
    // If the dot went too far to the left or right
    if ((positionX - colliders.radius < 0) || (positionX + colliders.radius > SCREEN_WIDTH) ||
        checkCollision(colliders, square) || checkCollision(colliders, circle))
    {
        // Move back
        positionX -= velocityX;
        shiftColliders();
    }
    // Move the dot up or down
    positionY += velocityY;
    shiftColliders();
    // If the dot went too far to high or low
    if ((positionY - colliders.radius < 0) || (positionY + colliders.radius > SCREEN_HEIGHT) ||
        checkCollision(colliders, square) || checkCollision(colliders, circle))
    {
        // Move back
        positionY -= velocityY;
        shiftColliders();
    }
}
void Dot::shiftColliders()
{
    colliders.x = positionX;
    colliders.y = positionY;
}
Circle &Dot::getColliders() { return colliders; }
bool checkCollision(Circle &a, Circle &b)
{
    // Calculate total radius squared
    int totalRadiusSquared = a.radius + b.radius;
    totalRadiusSquared = totalRadiusSquared * totalRadiusSquared;
    // If the distance between the centers of the circle is the less than sum of their radii
    if (distanceSquared(a.x, a.y, b.x, b.y) < (totalRadiusSquared))
    {
        // The circle have collided
        return true;
    }
    // If neither set of collision boxes touched
    return false;
}
bool checkCollision(Circle &a, SDL_Rect &b)
{
    // Closest point on collision box
    int collisionX, collisionY;
    // Find closest X offset
    if (a.x < b.x)
    {
        collisionX = b.x;
    }
    else if (a.x > b.x + b.w)
    {
        collisionX = b.x + b.w;
    }
    else
    {
        collisionX = a.x;
    }
    // Find closet y offset
    if (a.y < b.y)
    {
        collisionY = b.y;
    }
    else if (a.y > b.y + b.h)
    {
        collisionY = b.y + b.h;
    }
    else
    {
        collisionY = a.y;
    }
    // If the closest point inside the circle
    if (distanceSquared(a.x, a.y, collisionX, collisionY) < (a.radius * a.radius))
    {
        return true;
    }
    return false;
}
double distanceSquared(int x1, int y1, int x2, int y2)
{
    int deltaX = x2 - x1;
    int deltaY = y2 - y1;
    return (deltaX * deltaX) + (deltaY * deltaY);
}
void Dot::render()
{
    // Draw the dot
    gDotTexture.render(positionX - colliders.radius, positionY - colliders.radius);
}
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
        SDL_SetColorKey(loadedSuface, SDL_TRUE,
                        SDL_MapRGB(loadedSuface->format, 0, 0xFF, 0xFF));
        // Create texture from surface pixels
        newTexture = SDL_CreateTextureFromSurface(gRenderer, loadedSuface);
        if (newTexture == NULL)
        {
            printf("TEXTURE WRAPPER LOAD IMAGE [FAILED] - %s | SDL_IMAGE ERROR: %s\n",
                   path.c_str(), IMG_GetError());
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
bool TextureWrapper::loadFromRenderedText(string textureText,
                                          SDL_Color textColor)
{
    // Get red of preexisting texture
    free();
    // Render text surface
    SDL_Surface *textSurface =
        TTF_RenderText_Blended(gFont, textureText.c_str(), textColor);
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
    // Set blending function
    SDL_SetTextureBlendMode(texture, blending);
}
void TextureWrapper::setAlpha(Uint8 alpha)
{
    SDL_SetTextureAlphaMod(texture, alpha);
}
void TextureWrapper::render(int x, int y, SDL_Rect *clip, double angle,
                            SDL_Point *center, SDL_RendererFlip rendererFlip)
{
    // Set Rendering space and render to screen
    SDL_Rect renderQuad = {x, y, width, height};
    // Set clip rendering dimension
    if (clip != NULL)
    {
        renderQuad.w = clip->w;
        renderQuad.h = clip->h;
    }
    // Render to screen
    SDL_RenderCopyEx(gRenderer, texture, clip, &renderQuad, angle, center,
                     rendererFlip);
}
int TextureWrapper::getWidth() { return width; }
int TextureWrapper::getHeight() { return height; }
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
        gWindow = SDL_CreateWindow("SDL Tutorial XXX", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
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
                // Initialize renderer color
                SDL_SetRenderDrawColor(gRenderer, 0xFF, 0xFF, 0xFF, 0xFF);
                // Initialize PNG loading
                int imgFlags = IMG_INIT_PNG;
                if (!(IMG_Init(imgFlags) & imgFlags))
                {
                    printf("SDL_image [FAILED] - %s\n", IMG_GetError());
                    success = false;
                }
                // Initialize SDL_ttf
                if (TTF_Init() == -1)
                {
                    printf("SDL_TTF could not initialize! SDL_TTF Error: %s \n",
                           TTF_GetError());
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
    if (!gDotTexture.loadFromFile("images/dot.bmp"))
    {
        printf("DOT TEXTURE [FAILED]");
        return false;
    }
    return success;
}
void close()
{
    // Free loaded images
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
bool checkCollision(SDL_Rect rectA, SDL_Rect rectB)
{
    // The sides of rectangles
    int leftA, leftB;
    int rightA, rightB;
    int topA, topB;
    int bottomA, bottomB;
    // Calculate the sides of Rect A
    leftA = rectA.x;
    rightA = rectA.x + rectA.w;
    topA = rectA.y;
    bottomA = rectA.y + rectA.h;
    // Calculate the sides of Rect B
    leftB = rectB.x;
    rightB = rectB.x + rectB.w;
    topB = rectB.y;
    bottomB = rectB.y + rectB.h;
    // If any of the sides from A are outside of B
    if (bottomA <= topB)
    {
        return false;
    }
    if (topA >= bottomB)
    {
        return false;
    }
    if (rightA <= leftB)
    {
        return false;
    }
    if (leftA >= rightB)
    {
        return false;
    }
    // if none of the sides from A are outside B
    return true;
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
            // The dot that will be moving around on the screen
            Dot dot(Dot::DOT_WIDTH / 2, Dot::DOT_HEIGHT / 2);
            // The dot that will be collided against
            Dot otherDot(SCREEN_WIDTH / 4, SCREEN_HEIGHT / 4);
            //Set the wall
            SDL_Rect wall;
            wall.x = 300;
            wall.y = 40;
            wall.w = 40;
            wall.h = 400;
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
                    // Handle input for the dot
                    dot.handleInput(eventHandler);
                }
                // Move the dot
                dot.move(wall, otherDot.getColliders());
                // Clear screen
                SDL_SetRenderDrawColor(gRenderer, 0xFF, 0xFF, 0xFF, 0xFF);
                SDL_RenderClear(gRenderer);
                // Render wall
                SDL_SetRenderDrawColor(gRenderer, 0x00, 0x00, 0x00, 0xFF);
                SDL_RenderDrawRect(gRenderer, &wall);
                // Draw the dot
                dot.render();
                otherDot.render();
                // Update screen
                SDL_RenderPresent(gRenderer);
            }
        }
    }
    close();
    return 0;
}