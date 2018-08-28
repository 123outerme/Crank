#include "csGraphics.h"

/** \brief Initializes a cSprite object. You may want to create a wrapper method.
 *
 * \param sprite - a pointer to your sprite.
 * \param texture - a SDL_Texture* that holds your sprite's image
 * \param x - x position onscreen
 * \param y - y position onscreen
 * \param w - width of your sprite
 * \param h - height of your sprite
 * \param scale - size * this == drawn size
 * \param flip - SDL_RenderFlip value
 * \param degrees - rotation angle in degrees
 * \param subclass - void*. Do with it what you will, isn't used internally
 * \param fixed - if true, won't be affected by a scene's camera
 * \param drawPriority - 0 - not drawn. 1-5 - drawn. Lower number = drawn later
 */
void initCSprite(cSprite* sprite, SDL_Texture* texture, int id, SDL_Rect drawRect, SDL_Rect srcClipRect, double scale, SDL_RendererFlip flip, double degrees, bool fixed, void* subclass, int drawPriority)
{
    sprite->texture = texture;
    sprite->id = id;
    sprite->drawRect = drawRect;
    sprite->srcClipRect = srcClipRect;
    sprite->scale = scale;
    sprite->degrees = degrees;
    sprite->flip = flip;
    sprite->fixed = fixed;
    sprite->subclass = subclass;
    sprite->drawPriority = drawPriority;
}

/** \brief clears out a cSprite and its memory
 *
 * \param sprite - cSprite pointer
 */
void destroyCSprite(cSprite* sprite)
{
    SDL_DestroyTexture(sprite->texture);
    sprite->drawRect = (SDL_Rect) {0, 0, 0, 0};
    sprite->srcClipRect = (SDL_Rect) {0, 0, 0, 0};
    sprite->id = 0;
    sprite->scale = 0;
    sprite->degrees = 0;
    sprite->flip = SDL_FLIP_NONE;
    sprite->fixed = false;
    free(sprite->subclass);
    sprite->subclass = NULL;
    sprite->drawPriority = 0;

}

/** \brief draws a cSprite to the screen
 *
 * \param sprite - cSprite you want drawn
 * \param camera - cCamera to be used for drawing
 * \param update - if true, immediately presents renderer
 */
void drawCSprite(cSprite sprite, cCamera camera, bool update)
{
    int x = sprite.drawRect.x;
    int y = sprite.drawRect.y;
    if (!sprite.fixed)
    {
        float s = sin(degToRad(camera.degrees));
        float c = cos(degToRad(camera.degrees));
        x -= (windowW / 2 - sprite.drawRect.w / 2);
        y -= (windowH / 2 - sprite.drawRect.h / 2);

        int xnew = x * c - y * s;
        int ynew = x * s + y * c;

        x = xnew + (windowW / 2 - sprite.drawRect.w / 2);
        y = ynew + (windowH / 2 - sprite.drawRect.h / 2);
    }
    x -= !sprite.fixed * (camera.rect.x * windowW / camera.rect.w);
    y -= !sprite.fixed * (camera.rect.y * windowH / camera.rect.h);
    SDL_RenderCopyEx(mainRenderer, sprite.texture, &(sprite.srcClipRect), &((SDL_Rect) {.x = x, .y = y, .w = sprite.drawRect.w * sprite.scale * (sprite.fixed ? 1.0 : camera.zoom), .h = sprite.drawRect.h * sprite.scale * (sprite.fixed ? 1.0 : camera.zoom)}), sprite.degrees + !sprite.fixed * camera.degrees, NULL, sprite.flip);
    if (update)
        SDL_RenderPresent(mainRenderer);
}

/** \brief Initializes a cSprite object. You may want to create a wrapper method.
 *
 * \param model - a pointer to your model.
 * \param sprites - a pointer that holds your sprites
 * \param x - x position onscreen
 * \param y - y position onscreen
 * \param w - width of your sprite
 * \param h - height of your sprite
 * \param scale - size * this == drawn size
 * \param flip - SDL_RenderFlip value
 * \param degrees - rotation angle in degrees
 * \param fixed - if true, won't be affected by a scene's camera
 * \param subclass - void*. Do with it what you will, isn't used internally
 * \param drawPriority - 0 - not drawn. 1-5 - drawn. Lower number = drawn later
 */
void initC2DModel(c2DModel* model, cSprite* sprites, int numSprites, int x, int y, int w, int h, double scale, SDL_RendererFlip flip, double degrees, bool fixed, void* subclass, int drawPriority)
{
    model->sprites = (numSprites) ? sprites : NULL;
    model->numSprites = numSprites;
    model->rect = (SDL_Rect) {.x = x, .y = y, .w = w, .h = h};
    model->scale = scale;
    model->flip = flip;
    model->degrees = degrees;
    model->fixed = fixed;
    model->subclass = subclass;
    model->drawPriority = drawPriority;
}

/** \brief clears out a c2DModel and its memory
 *
 * \param model - c2DModel pointer
 */
void destroyC2DModel(c2DModel* model)
{
    for(int i = 0; i < model->numSprites; i++)
        destroyCSprite(&model->sprites[i]);
    model->rect = (SDL_Rect) {0, 0, 0, 0};
    model->scale = 0;
    model->degrees = 0;
    model->flip = SDL_FLIP_NONE;
    model->fixed = false;
    free(model->subclass);
    model->subclass = NULL;
    model->drawPriority = 0;
}

/** \brief draws a c2DModel to the screen
 *
 * \param model - c2DModel you want drawn
 * \param camera - cCamera to be used for drawing
 * \param update - if true, immediately presents renderer
 */
void drawC2DModel(c2DModel model, cCamera camera, bool update)
{
    for(int i = 0; i < model.numSprites; i++)
    {
        for(int priority = 5; priority >= 1; priority--)
        {
            if (model.sprites[i].drawPriority == priority)
            {
                int x = model.rect.x + model.sprites[i].drawRect.x;
                int y = model.rect.y + model.sprites[i].drawRect.y;
                if (!model.fixed | !model.sprites[i].fixed)
                {
                    float s = sin(degToRad(camera.degrees));
                    float c = cos(degToRad(camera.degrees));
                    x -= (windowW / 2 - model.rect.w / 2);
                    y -= (windowH / 2 - model.rect.h / 2);

                    int xnew = x * c - y * s;
                    int ynew = x * s + y * c;

                    x = xnew + (windowW / 2 - model.rect.w / 2);
                    y = ynew + (windowH / 2 - model.rect.h / 2);
                }
                x -= (!model.fixed | !model.sprites[i].fixed) * (camera.rect.x * windowW / camera.rect.w);
                y -= (!model.fixed | !model.sprites[i].fixed) * (camera.rect.y * windowH / camera.rect.h);
                SDL_RenderCopyEx(mainRenderer, model.sprites[i].texture, &(model.sprites[i].srcClipRect), &((SDL_Rect) {.x = x, .y = y, .w = model.sprites[i].drawRect.w * (model.sprites[i].scale * model.scale * ((model.fixed | model.sprites[i].fixed) ? 1.0 : camera.zoom)), .h = model.sprites[i].drawRect.h * (model.sprites[i].scale * model.scale * ((model.fixed | model.sprites[i].fixed) ? 1.0 : camera.zoom))}), model.sprites[i].degrees + model.degrees + !(model.fixed | model.sprites[i].fixed) * camera.degrees, NULL, (model.sprites[i].flip + model.flip) % 4);
            }
        }
    }
    if (update)
        SDL_RenderPresent(mainRenderer);
}

/** \brief initializes a cText object
 *
 * \param text - a pointer to your cText
 * \param string - the string you want
 * \param rect - SDL_Rect containing bounding box of text
 * \param textColor - color of text
 * \param bgColor - color of background box
 * \param degrees - rotation angle in degrees
 * \param flip - SDL_RenderFlip value
 * \param fixed - if true, won't be affected by a scene's camera
 * \param drawPriority - 0 - not drawn. 1-5 - drawn. Lower number = drawn later
 */
void initCText(cText* text, char* string, SDL_Rect rect, SDL_Color textColor, SDL_Color bgColor, SDL_RendererFlip flip, double degrees, bool fixed, int drawPriority)
{
    text->string = calloc(strlen(string), sizeof(char));
    strcpy(text->string, string);
    text->rect = rect;
    text->textColor = textColor;
    text->bgColor = bgColor;
    text->flip = flip;
    text->degrees = degrees;
    text->fixed = fixed;
    text->drawPriority = drawPriority;
}

/** \brief clears out a cText and its memory
 *
 * \param text - cText pointer
 */
void destroyCText(cText* text)
{
    free(text->string);
    text->string = NULL;
    text->rect = (SDL_Rect) {0, 0, 0, 0};
    text->textColor = (SDL_Color) {0, 0, 0, 0};
    text->bgColor = (SDL_Color) {0, 0, 0, 0};
    text->fixed = false;
    text->drawPriority = 0;
}

/** \brief draws a cText to the screen
 *
 * \param text - cText you want drawn
 * \param camera - cCamera to be used for drawing
 * \param update - if true, immediately presents renderer
 */
void drawCText(cText text, cCamera camera, bool update)
{
    Uint8 r, g, b, a;
    SDL_GetRenderDrawColor(mainRenderer, &r, &g, &b, &a);
    SDL_SetRenderDrawColor(mainRenderer, text.bgColor.r, text.bgColor.g, text.bgColor.b, text.bgColor.a);
    int* wh = loadTextTexture(text.string, &text.texture, text.rect.w, text.textColor, true);
    text.rect.w = wh[0];
    text.rect.h = wh[1];
    if (!text.fixed)
    {
        float s = sin(degToRad(camera.degrees));
        float c = cos(degToRad(camera.degrees));
        text.rect.x -= (windowW / 2 - text.rect.w / 2);
        text.rect.y -= (windowH / 2 - text.rect.h / 2);

        int xnew = text.rect.x * c - text.rect.y * s;
        int ynew = text.rect.x * s + text.rect.y * c;

        text.rect.x = xnew + (windowW / 2 - text.rect.w / 2) - (camera.rect.x * windowW / camera.rect.w);
        text.rect.y = ynew + (windowH / 2 - text.rect.h / 2) - (camera.rect.y * windowH / camera.rect.h);
    }
    SDL_SetRenderDrawColor(mainRenderer, text.bgColor.r, text.bgColor.g, text.bgColor.b, text.bgColor.a);
    SDL_RenderCopyEx(mainRenderer, text.texture, NULL, &text.rect, text.degrees + !text.fixed * camera.degrees, NULL, text.flip);
    SDL_SetRenderDrawColor(mainRenderer, r, g, b, a);
    SDL_DestroyTexture(text.texture);
}

/** \brief Loads in an image resource
 *
 * \param res - cResource pointer
 * \param subclass - struct containing your data
 * \param drawingMethod - function pointer to your drawing method. Must have only one argument, which is your subclass
 */
void initCResource(cResource* res, void* subclass, void (*drawingRoutine)(void*), int drawPriority)
{
    res->subclass = subclass;
    res->drawingRoutine = drawingRoutine;
    res->drawPriority = drawPriority;
}

/** \brief draws a CResource
 *
 * \param res - pointer to your cResource
 */
void drawCResource(cResource* res)
{
    (*res->drawingRoutine)(res->subclass);
}

/** \brief clears out a cResource and its memory
 *
 * \param res - cResource pointer
 */
void destroyCResource(cResource* res)
{
    res->subclass = NULL;
    res->drawingRoutine = NULL;
    res->drawPriority = 0;
}

/** \brief initializes a cCamera and its memory
 *
 * \param camera - a cCamera pointer
 * \param rect - the bounding rect of the camera
 * \param degrees - angle of rotation in degrees
 */
void initCCamera(cCamera* camera, SDL_Rect rect, double zoom, double degrees)
{
    camera->rect = rect;
    camera->zoom = zoom;
    camera->degrees = degrees;
}

/** \brief clears out a cCamera and its memory
 *
 * \param camera - a cCamera pointer
 */
void destroyCCamera(cCamera* camera)
{
    camera->rect = (SDL_Rect) {0, 0, 0, 0};
    camera->zoom = 1.0;
    camera->degrees = 0.0;
}

/** \brief Initializes a cScene object.
 *
 * \param scenePtr - pointer to your cScene
 * \param sprites[] - array of cSprites to be drawn
 * \param spriteCount - how many elements in sprites[]
 * \param models[] - array of c2DModels to be drawn
 * \param modelCount - how many elements in model[]
 * \param resources[] - array of cResources
 * \param resCount - how many elements in resources[]
 * \param strings[] - array of cTexts
 * \param stringCount - how many elements in strings[]
 */
void initCScene(cScene* scenePtr, SDL_Color bgColor, cCamera* camera, cSprite* sprites[], int spriteCount, c2DModel* models[], int modelCount, cResource* resources[], int resCount, cText* strings[], int stringCount)
{
    scenePtr->camera = camera;
    scenePtr->bgColor = bgColor;
    if (spriteCount > 0)
    {
        scenePtr->sprites = calloc(spriteCount, sizeof(cSprite*));
        for(int i = 0; i < spriteCount; i++)
            scenePtr->sprites[i] = sprites[i];
    }
    else
        scenePtr->sprites = NULL;
    scenePtr->spriteCount = spriteCount;

    if (modelCount > 0)
    {
        scenePtr->models = calloc(modelCount, sizeof(c2DModel*));
        for(int i = 0; i < modelCount; i++)
            scenePtr->models[i] = models[i];
    }
    else
        scenePtr->models = NULL;
    scenePtr->modelCount = modelCount;

    if (resCount > 0)
    {
        scenePtr->resources = calloc(resCount, sizeof(cResource*));
        for(int i = 0; i < resCount; i++)
            scenePtr->resources[i] = resources[i];
    }
    else
        scenePtr->resources = NULL;
    scenePtr->resCount = resCount;

    if (stringCount > 0)
    {
        scenePtr->strings = calloc(stringCount, sizeof(cText*));
        for(int i = 0; i < stringCount; i++)
            scenePtr->strings[i] = strings[i];
    }
    else
        scenePtr->strings = NULL;
    scenePtr->stringCount = stringCount;
}

/** \brief clears out a cScene and all its memory, including sprites.
 *
 * \param scenePtr - pointer to your cScene
 */
void destroyCScene(cScene* scenePtr)
{
    destroyCCamera(scenePtr->camera);

    if (scenePtr->spriteCount > 0)
    {
        for(int i = 0; i < scenePtr->spriteCount; i++)
            destroyCSprite(scenePtr->sprites[i]);
        scenePtr->spriteCount = 0;
        free(scenePtr->sprites);
    }

    if (scenePtr->modelCount > 0)
    {
        for(int i = 0; i < scenePtr->modelCount; i++)
            destroyC2DModel(scenePtr->models[i]);
        scenePtr->modelCount = 0;
        free(scenePtr->models);
    }

    if (scenePtr->resCount > 0)
    {
        for(int i = 0; i < scenePtr->resCount; i++)
            destroyCResource(scenePtr->resources[i]);
        scenePtr->resCount = 0;
        free(scenePtr->resources);
    }

    if (scenePtr->stringCount > 0)
    {
        for(int i = 0; i < scenePtr->stringCount; i++)
            destroyCText(scenePtr->strings[i]);
        scenePtr->stringCount = 0;
        free(scenePtr->strings);
    }

    if (scenePtr->camera)
        destroyCCamera(scenePtr->camera);
}

/** \brief draws the CScene.
 *
 * \param scenePtr - pointer to your cScene
 * \param redraw - if nonzero, will update the screen
 */
void drawCScene(cScene* scenePtr, bool redraw)
{
    SDL_SetRenderDrawColor(mainRenderer, scenePtr->bgColor.r, scenePtr->bgColor.g, scenePtr->bgColor.b, scenePtr->bgColor.a);
    SDL_RenderClear(mainRenderer);
    for(int priority = 5; priority >= 1; priority--)
    {
        for(int i = 0; i < scenePtr->spriteCount; i++)
        {
            if (scenePtr->sprites[i]->drawPriority == priority)
                drawCSprite(*(scenePtr->sprites[i]), *(scenePtr->camera), false);
        }

        for(int i = 0; i < scenePtr->modelCount; i++)
        {
            if (scenePtr->models[i]->drawPriority == priority)
                drawC2DModel(*(scenePtr->models[i]), *(scenePtr->camera), false);
        }

        for(int i = 0; i < scenePtr->resCount; i++)
        {
            if (scenePtr->resources[i]->drawPriority == priority)
                drawCResource(scenePtr->resources[i]);
        }

        for(int i = 0; i < scenePtr->stringCount; i++)
        {
            if (scenePtr->strings[i]->drawPriority == priority)
                drawCText(*(scenePtr->strings[i]), *(scenePtr->camera), false);
        }
    }

    if (redraw)
        SDL_RenderPresent(mainRenderer);
}

/** \brief Draws text to the screen using mainFont, wrapped and bounded
 *
 * \param input - text to be drawn
 * \param x - x value of first letter
 * \param y - y value of first letter
 * \param maxW - how wide the text can get before wrapping
 * \param maxH - how tall the text can draw before being cut off
 * \param color - SDL_Color struct of color to be used
 * \param render - if true, immediately presents renderer
 */
void drawText(char* input, int x, int y, int maxW, int maxH, SDL_Color color, bool render)
{
    if (canDrawText)
    {
        SDL_Texture* txtTexture = NULL;
        int* wh;
        wh = loadTextTexture(input, &txtTexture, maxW, color, true);
        SDL_RenderCopy(mainRenderer, txtTexture, &((SDL_Rect){.w = *wh > maxW ? maxW : *wh, .h = *(wh + 1) > maxH ? maxH : *(wh + 1)}),
                                                 &((SDL_Rect){.x =  x, .y = y, .w = *wh > maxW ? maxW : *wh, .h = *(wh + 1) > maxH ? maxH : *(wh + 1)}));


        if (render)
            SDL_RenderPresent(mainRenderer);
        SDL_DestroyTexture(txtTexture);
    }
}

//
#define IMG_INIT_FLAGS IMG_INIT_PNG

/** \brief Initializes an SDL window and all of CoSprite's inner stuff.
 * \return Code 0: No error. Code 1: SDL systems failed to initialize. Code 2: Window could not be created Code 3: Renderer failed to initialize
 */
int initCoSprite(char* iconPath, char* windowName, int windowWidth, int windowHeight, char* fontPath, int fontSize)
{
    int status = 0;
    mainWindow = NULL;
    if(SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS | SDL_INIT_AUDIO) < 0)
    {
        printf("SDL could not initialize! SDL_Error: %s\n", SDL_GetError());
        return 1;
    }
    else
    {
        if(!(IMG_Init(IMG_INIT_PNG) & IMG_INIT_FLAGS))
        {
            printf("SDL_image could not initialize! SDL_image Error: %s\n", IMG_GetError());
            return 1;
        }
        //Initialize SDL_ttf
        if(TTF_Init() == -1)
        {
            printf("SDL_ttf could not initialize! SDL_ttf Error: %s\n", TTF_GetError());
            return 1;
        }
        if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 1024) == -1)
        {
            printf("SDL_mixer could not initialize! SDL_mixer Error: %s\n", SDL_GetError());
            return 1;
        }
        else
        //Mix_Init(MIX_INIT_OGG);  //deprecated?
        soundVolume = MIX_MAX_VOLUME;
        Mix_AllocateChannels(32);
        Mix_Volume(-1, soundVolume);  //sets all channels to the sound level soundVolume
        musicVolume = MIX_MAX_VOLUME;
        Mix_VolumeMusic(musicVolume);
        mainScreen = NULL;
        mainRenderer = NULL;
        mainFont = NULL;
        mainWindow = SDL_CreateWindow(windowName, SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, windowWidth, windowHeight, SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE);
        if (!mainWindow)
        {
            printf("Window could not be created! SDL_Error: %s\n", SDL_GetError());
            return 2;
        }
        else
        {
            windowW = windowWidth;
            windowH = windowHeight;
            mainScreen = SDL_GetWindowSurface(mainWindow);
            mainRenderer = SDL_CreateRenderer(mainWindow, -1, SDL_RENDERER_ACCELERATED);
            if(!mainRenderer)
            {
                printf("Renderer could not be created! SDL Error: %s\n", SDL_GetError());
                return 3;
            }
            else
            {
                SDL_Surface* iconSurface = IMG_Load(iconPath);
                SDL_SetWindowIcon(mainWindow, iconSurface);
                SDL_FreeSurface(iconSurface);
                SDL_SetRenderDrawBlendMode(mainRenderer, SDL_BLENDMODE_BLEND);
                SDL_SetRenderDrawColor(mainRenderer, 0xFF, 0xFF, 0xFF, 0xFF);
                SDL_RenderSetLogicalSize(mainRenderer, windowWidth, windowHeight);
                SDL_RenderClear(mainRenderer);
                loadTTFont(fontPath, &mainFont, fontSize);
                //loadTTFont(FONT_FILE_NAME, &smallFont, 20);
                if (!mainFont)
                {
                    canDrawText = false;
                    status = 4;
                }
                canDrawText = true;
                srand((unsigned int) time(NULL));
                /*if (checkFile(CONFIG_FILE_NAME, SIZE_OF_SCANCODE_ARRAY))
                {
                    loadConfig(CONFIG_FILE_NAME);
                }
                else
                    initConfig(CONFIG_FILE_NAME);*/
            }
        }
    }
    return status;
}

void closeCoSprite()
{
    TTF_CloseFont(mainFont);
    //TTF_CloseFont(smallFont);
	if (mainScreen)
        SDL_FreeSurface(mainScreen);
    if (mainWindow)
        SDL_DestroyWindow(mainWindow);
    if (mainRenderer)
        SDL_DestroyRenderer(mainRenderer);
    /*for(int i = 0; i < MAX_SOUNDS; i++)
    {
        if (audioArray[i])
            Mix_FreeChunk(audioArray[i]);
    }*/

    /*for(int i = 0; i < MAX_MUSIC; i++)
    {
        if (musicArray[i])
            Mix_FreeMusic(musicArray[i]);
    }*/

    TTF_Quit();
    IMG_Quit();
    Mix_CloseAudio();
    SDL_Quit();
}

/** \brief Loads an image into a SDL_Texture*
 *
 * \param imgPath - valid string filepath (relative or absolute)
 * \param dest - pointer to your SDL_Texture*
 * \return true if created, false otherwise
 */
bool loadIMG(char* imgPath, SDL_Texture** dest)
{
    SDL_Surface* surf = IMG_Load(imgPath);
    if (!surf)
    {
	printf("Unable to load image for %s! SDL_Error: %s\n", imgPath, SDL_GetError());
        return false;
    }
    SDL_SetColorKey(surf, 1, SDL_MapRGB(surf->format, 255, 28, 198));
    *dest = SDL_CreateTextureFromSurface(mainRenderer, surf);
    if (!(*dest))
    {
        printf("Unable to create texture for %s! SDL_Error: %s\n", imgPath, SDL_GetError());
        return false;
    }
    SDL_FreeSurface(surf);
    return true;
}

/** \brief Loads a .ttf font
 *
 * \param filePath - valid string filepath (relative or absolute)
 * \param dest - pointer to your SDL_Texture*
 * \param sizeInPts - font size, measured in points
 * \return true if created, false otherwise
 */
bool loadTTFont(char* filePath, TTF_Font** dest, int sizeInPts)
{
    *dest = TTF_OpenFont(filePath, sizeInPts);
    if (!*dest)
    {
        printf("Font could not be loaded! SDL Error: %s\n", SDL_GetError());
        return false;
    }
    return true;
}

/** \brief converts text to a texture
 *
 * \param text - text you want converted to a texture
 * \param dest - pointer to your SDL_Texture*
 * \param maxW - How wide the text can be before wrapping
 * \param color - SDL_Color struct of color to be used
 * \param isBlended - true always
 * \return int[2] holding {width, height}
 *
 */
int* loadTextTexture(char* text, SDL_Texture** dest, int maxW, SDL_Color color, bool isBlended)
{
    static int wh[] = {0, 0};
    SDL_Surface* txtSurface = NULL;
    if (isBlended)
        txtSurface = TTF_RenderText_Blended_Wrapped(mainFont, text, color, maxW);
//    else
//        txtSurface = TTF_RenderText(smallFont, text, color, ((SDL_Color) {181, 182, 173}));
    *dest = SDL_CreateTextureFromSurface(mainRenderer, txtSurface);
    if (!*dest)
    {
        printf("Text texture could not be loaded! SDL Error: %s\n", SDL_GetError());
    }
    else
    {
        wh[0] = txtSurface->w;
        wh[1] = txtSurface->h;
    }
    SDL_FreeSurface(txtSurface);
    return wh;
}
//