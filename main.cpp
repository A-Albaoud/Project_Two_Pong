#include "CS3113/cs3113.h"


/**
* Author: Abdulrahman Albaoud
* Assignment: Pong Clone
* Date due: [02/28/2026]
* I pledge that I have completed this assignment without
* collaborating with anyone else, in conformance with the
* NYU School of Engineering Policies and Procedures on
* Academic Misconduct.
**/

enum GameState {ONEP, TWOP };
enum Winner    {NEITHER, GANON, LINK};
enum Balls     {ZERO, ONE, TWO, THREE};

// Global Constants
constexpr int SCREEN_WIDTH  = 1500 / 2,
              SCREEN_HEIGHT = 1600 / 2,
              FPS           = 60,
              SIZE          = 400 / 2,
              SPEED         = 250;

constexpr char    BG_COLOUR[]     = "#F8F1C8";
constexpr Vector2 ORIGIN          = { SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2 },
                  BASE_SIZE       = { (float) SIZE, (float) SIZE },
                  LINK_INIT_POS   = { ORIGIN.x - 200.0f, ORIGIN.y + 295},
                  GANON_INIT_POS  = { ORIGIN.x - 200.0f, 0.0f + 100},
                  RUPEE_INIT_POS  = { ORIGIN.x, ORIGIN.y};

// Images owned by Nintendo — please don't sue me.
constexpr char LINK_FP[]    = "assets/game/awakening-link.png";
constexpr char RUPEE_FP[]   = "assets/game/awakening-rupee.png";
constexpr char LINK_OOT[]   = "assets/game/Link_OOT.png";
constexpr char GANNON_OOT[] = "assets/game/Ganon_OOT.png";


// Global Variables
AppStatus gAppStatus     = RUNNING;
GameState gGameState     = TWOP;
Winner    gWinner;
Balls     gBalls;
float     gAngle         = 0.0f,
          gRupeeAngle    = 0.0f,
          gPreviousTicks = 0.0f;

Vector2 gLinkPosition  = LINK_INIT_POS,
        gLinkMovement  = { 0.0f, 0.0f },
        gLinkScale     = BASE_SIZE,

        gRupeePosition = RUPEE_INIT_POS,
        gRupeeMovement = { 0.5f, 0.4f },
        gRupeeScale    = { BASE_SIZE.x / 5.0f , BASE_SIZE.y / 5.0f * 1.6f},

        gGanonPosition  = GANON_INIT_POS,
        gGanonMovement  = { 0.0f, 0.0f },
        gGanonScale     = BASE_SIZE,


        gRupeePosition2 = {RUPEE_INIT_POS.x + 50,RUPEE_INIT_POS.y},
        gRupeeMovement2 = { 0.5f, 0.7f },


        gRupeePosition3 = {RUPEE_INIT_POS.x + 90,RUPEE_INIT_POS.y},
        gRupeeMovement3 = { 0.5f, 0.8f },

        gRupeePosition4 = {RUPEE_INIT_POS.x - 50,RUPEE_INIT_POS.y},
        gRupeeMovement4 = { 0.3f, 0.65f };


Texture2D gLinkTexture;
Texture2D gRupeeTexture;
Texture2D gGanonTexture;


int Link_Score  = 0;
int Ganon_Score = 0;

unsigned int startTime;

// Function Declarations
void initialise();
void processInput();
void update();
void render();
void shutdown();
bool isColliding(const Vector2 *positionA, const Vector2 *scaleA, const Vector2 *positionB, const Vector2 *scaleB);

// Function Definitions

/**
 * @brief Checks for a square collision between 2 Rectangle objects.
 * 
 * @see 
 * 
 * @param positionA The position of the first object
 * @param scaleA The scale of the first object
 * @param positionB The position of the second object
 * @param scaleB The scale of the second object
 * @return true if a collision is detected,
 * @return false if a collision is not detected
 */
bool isColliding(const Vector2 *positionA,  const Vector2 *scaleA, 
                 const Vector2 *positionB, const Vector2 *scaleB)
{
    float xDistance = fabs(positionA->x - positionB->x) - ((scaleA->x + scaleB->x) / 2.0f);
    float yDistance = fabs(positionA->y - positionB->y) - ((scaleA->y + scaleB->y) / 2.0f);

    if (xDistance < 0.0f && yDistance < 0.0f) return true;

    return false;
}

void renderObject(const Texture2D *texture, const Vector2 *position, 
                  const Vector2 *scale, const float angle = gAngle)
{
    // Whole texture (UV coordinates)
    Rectangle textureArea = {
        // top-left corner
        0.0f, 0.0f,

        // bottom-right corner (of texture)
        static_cast<float>(texture->width),
        static_cast<float>(texture->height)
    };

    // Destination rectangle – centred on gPosition
    Rectangle destinationArea = {
        position->x,
        position->y,
        static_cast<float>(scale->x),
        static_cast<float>(scale->y)
    };

    // Origin inside the source texture (centre of the texture)
    Vector2 originOffset = {
        static_cast<float>(scale->x) / 2.0f,
        static_cast<float>(scale->y) / 2.0f
    };

    // Render the texture on screen
    DrawTexturePro(
        *texture, 
        textureArea, destinationArea, originOffset,
        angle, WHITE
    );
}

void initialise()
{
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "User Input / Collision Detection");

    startTime = time(NULL);

    gLinkTexture  = LoadTexture(LINK_OOT);
    gRupeeTexture = LoadTexture(RUPEE_FP);
    gGanonTexture = LoadTexture(GANNON_OOT);

    SetTargetFPS(FPS);
}

void processInput() 
{
    gLinkMovement  = { 0.0f, 0.0f };

    if      (IsKeyDown(KEY_A) && (gLinkPosition.x - 95 > 0))        gLinkMovement.x = -1;
    else if (IsKeyDown(KEY_D) && (gLinkPosition.x + 90 < SCREEN_WIDTH))  gLinkMovement.x =  1;
    //if      (IsKeyDown(KEY_W)) gLinkMovement.y = -1;
    //else if (IsKeyDown(KEY_S)) gLinkMovement.y =  1;
    if(gGameState == TWOP){
        gGanonMovement = { 0.0f, 0.0f };
        if      (IsKeyDown(KEY_LEFT) && (gGanonPosition.x - 95 > 0))              gGanonMovement.x = -1;
        else if (IsKeyDown(KEY_RIGHT) && (gGanonPosition.x + 90 < SCREEN_WIDTH))  gGanonMovement.x =  1;
        if (IsKeyPressed(KEY_T)) 
        {
            gGameState = ONEP;
            gGanonMovement.x = 0.65;
        }
    }

    if(gGameState == ONEP)
    {
        if (IsKeyPressed(KEY_Y)) gGameState = TWOP;
    }


    if(IsKeyPressed(KEY_ONE))   {gBalls = ONE;}
    if(IsKeyPressed(KEY_TWO))   {gBalls = TWO;}
    if(IsKeyPressed(KEY_THREE)) {gBalls = THREE;}

    if (IsKeyPressed(KEY_Q) || WindowShouldClose()) gAppStatus = TERMINATED;
}

void update() 
{





    // Delta time
    float ticks = (float) GetTime();
    float deltaTime = ticks - gPreviousTicks;
    gPreviousTicks  = ticks;
    gRupeeAngle += 1.0f;


    gLinkPosition = {
        gLinkPosition.x + SPEED * gLinkMovement.x * deltaTime,
        gLinkPosition.y + SPEED * gLinkMovement.y * deltaTime
    };

    gGanonPosition.x = gGanonPosition.x + SPEED * gGanonMovement.x * deltaTime;

    gRupeePosition = {
        gRupeePosition.x + SPEED * gRupeeMovement.x * deltaTime,
        gRupeePosition.y + SPEED * gRupeeMovement.y * deltaTime
    };

    if(gBalls >= ONE)
    {
        gRupeePosition2 = 
        {
        gRupeePosition2.x + SPEED * gRupeeMovement2.x * deltaTime,
        gRupeePosition2.y + SPEED * gRupeeMovement2.y * deltaTime
        };
        if (gRupeePosition2.x - 20 < 0 || gRupeePosition2.x + 20 > SCREEN_WIDTH) gRupeeMovement2.x *= -1;

        if (isColliding(&gLinkPosition, &gLinkScale, &gRupeePosition2, &gRupeeScale))
        {
            gRupeeMovement2.y *= -1;
            gRupeePosition2.y = gLinkPosition.y - 150;
        }
        if (isColliding(&gGanonPosition, &gGanonScale, &gRupeePosition2, &gRupeeScale))
        {
            gRupeeMovement2.y *= -1;
            gRupeePosition2.y = gGanonPosition.y + 150;
        }
        if(gRupeePosition2.y < 0) 
        {
            Link_Score++;
            gRupeePosition2 = RUPEE_INIT_POS;

        }

        if(gRupeePosition2.y > SCREEN_HEIGHT) 
        {
            Ganon_Score++;
            gRupeePosition2 = RUPEE_INIT_POS;

        }
    };

    if(gBalls >= TWO)
    {
        gRupeePosition3 = 
        {
        gRupeePosition3.x + SPEED * gRupeeMovement3.x * deltaTime,
        gRupeePosition3.y + SPEED * gRupeeMovement3.y * deltaTime
        };
        if (gRupeePosition3.x - 20 < 0 || gRupeePosition3.x + 20 > SCREEN_WIDTH) gRupeeMovement3.x *= -1;

        if (isColliding(&gLinkPosition, &gLinkScale, &gRupeePosition3, &gRupeeScale))
        {
            gRupeeMovement3.y *= -1;
            gRupeePosition3.y = gLinkPosition.y - 150;
        }
        if (isColliding(&gGanonPosition, &gGanonScale, &gRupeePosition3, &gRupeeScale))
        {
            gRupeeMovement3.y *= -1;
            gRupeePosition3.y = gGanonPosition.y + 150;
        }
        if(gRupeePosition3.y < 0) 
        {
            Link_Score++;
            gRupeePosition3 = RUPEE_INIT_POS;

        }

        if(gRupeePosition3.y > SCREEN_HEIGHT) 
        {
            Ganon_Score++;
            gRupeePosition3 = RUPEE_INIT_POS;

        }
    };

    if(gBalls >= THREE)
    {
        gRupeePosition4 = 
        {
        gRupeePosition4.x + SPEED * gRupeeMovement4.x * deltaTime,
        gRupeePosition4.y + SPEED * gRupeeMovement4.y * deltaTime
        };
        if (gRupeePosition4.x - 20 < 0 || gRupeePosition4.x + 20 > SCREEN_WIDTH) gRupeeMovement4.x *= -1;

        if (isColliding(&gLinkPosition, &gLinkScale, &gRupeePosition4, &gRupeeScale))
        {
            gRupeeMovement4.y *= -1;
            gRupeePosition4.y = gLinkPosition.y - 150;
        }
        if (isColliding(&gGanonPosition, &gGanonScale, &gRupeePosition4, &gRupeeScale))
        {
            gRupeeMovement4.y *= -1;
            gRupeePosition4.y = gGanonPosition.y + 150;
        }
        if(gRupeePosition4.y < 0) 
        {
            Link_Score++;
            gRupeePosition4 = RUPEE_INIT_POS;

        }

        if(gRupeePosition4.y > SCREEN_HEIGHT) 
        {
            Ganon_Score++;
            gRupeePosition4 = RUPEE_INIT_POS;

        }
    };



    

    if (gRupeePosition.x - 20 < 0 || gRupeePosition.x + 20 > SCREEN_WIDTH) gRupeeMovement.x *= -1;

    if (isColliding(&gLinkPosition, &gLinkScale, &gRupeePosition, &gRupeeScale))
    {
        gRupeeMovement.y *= -1;
        gRupeePosition.y = gLinkPosition.y - 150;
    }
    if (isColliding(&gGanonPosition, &gGanonScale, &gRupeePosition, &gRupeeScale))
    {
        gRupeeMovement.y *= -1;
        gRupeePosition.y = gGanonPosition.y + 150;
    }

    if(gRupeePosition.y < 0) 
    {
        Link_Score++;
        gRupeePosition = RUPEE_INIT_POS;

    }

    if(gRupeePosition.y > SCREEN_HEIGHT) 
    {
        Ganon_Score++;
        gRupeePosition = RUPEE_INIT_POS;

    }

    if(Ganon_Score >= 7) { gWinner = GANON ;}
    if(Link_Score >= 7)  { gWinner = LINK ;}


   
    if (gGameState == ONEP)
    {
        //if(gGanonPosition.x + 90 < SCREEN_WIDTH) gGanonMovement.x =  1;
        //if(gGanonPosition.x - 95 > 0)            gGanonMovement.x = -1;
        if (gGanonPosition.x - 90 < 0 || gGanonPosition.x + 90 > SCREEN_WIDTH) gGanonMovement.x *= -1;
        
    }
}

void render()
{
    BeginDrawing();
    ClearBackground(ColorFromHex(BG_COLOUR));

    if(gWinner == GANON)
    {
        DrawText("Ganon Paid for Lunch", SCREEN_WIDTH/2 -250, SCREEN_HEIGHT/2, 50, BLACK);
    }

    if(gWinner == LINK)
    {
        DrawText("Link Paid for Lunch", SCREEN_WIDTH/2 - 250, SCREEN_HEIGHT/2, 50, BLACK);
    }

    // Render Link
    renderObject(&gLinkTexture, &gLinkPosition, &gLinkScale);

    // Render Ganon
    renderObject(&gGanonTexture, &gGanonPosition, &gGanonScale);

    // Render the rupee
    renderObject(&gRupeeTexture, &gRupeePosition, &gRupeeScale, gRupeeAngle);

    if(gBalls >= ONE)
    {
    renderObject(&gRupeeTexture, &gRupeePosition2, &gRupeeScale, gRupeeAngle);
    }

    if(gBalls >= TWO)
    {
    renderObject(&gRupeeTexture, &gRupeePosition3, &gRupeeScale, gRupeeAngle);
    }

    if(gBalls >= THREE)
    {
    renderObject(&gRupeeTexture, &gRupeePosition4, &gRupeeScale, gRupeeAngle);
    }



    EndDrawing();
}

void shutdown() 
{ 
    CloseWindow(); 
    UnloadTexture(gLinkTexture);
    UnloadTexture(gRupeeTexture);
    UnloadTexture(gGanonTexture);
}

int main(void)
{
    initialise();

    while (gAppStatus == RUNNING)
    {
        processInput();
        update();
        render();

    }


    shutdown();

    return 0;
}