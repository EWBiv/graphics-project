#include "Player.h"
#include "Input.h"
#include <iostream> //Used for debugging to console

using namespace std;



Player::Player(Mesh* mesh, Material* mat) : Entity(mesh,mat,true)
{
    movementSpeed = .015f;
    nonJumpY = -4.5f;
    actionCooldown = .01f;

    ResetValues();
}

void Player::ResetValues()
{
    currentLane = Lane::MiddleLane;
    currentActionCooldown = 0.0f;
    coolingDown = false;
    movingLeft = false;
    movingRight = false;
    jumpingState = JumpingState::NotJumping;
    verticalForce = 0.0f;
}

void Player::NewGame()
{
    this->GetTransform()->SetPosition(0.0f, nonJumpY, -35.0f);
    ResetValues();
}

Player::~Player()
{
}

bool Player::Update(float dt, vector<Entity*> obstacles)
{
    if (!coolingDown)
    {
        //Get a reference to the input manager
        Input& input = Input::GetInstance();

        bool leftRestricted = false;
        bool rightRestricted = false;

        switch (currentLane)
        {
        case Lane::LeftLane:
            leftRestricted = true;
            break;
        case Lane::MiddleLane:
            break;
        case Lane::RightLane:
            rightRestricted = true;
            break;
        default:
            //not supposed to be here
            break;
        }

        if (input.KeyPress('A') && leftRestricted == false)
        {
            movingLeft = true;
            futureLane = ((Lane)((int)currentLane - 1));
            coolingDown = true;
        }

        else if (input.KeyPress('D') && rightRestricted == false)
        {
            movingRight = true;
            futureLane = ((Lane)((int)currentLane + 1));
            coolingDown = true;
        }

        //Add check for being 'on ground'
        if (input.KeyDown(' ') && jumpingState == JumpingState::NotJumping)
        {
            jumpingState = JumpingState::InitialForce;
        }
    }
    else if(movingLeft || movingRight)
    {
        if (movingLeft)
            moveLeft(dt);

        else if (movingRight)
            moveRight(dt);
    }
    else    //(cooling down)
    {
        currentActionCooldown += dt;
        //cout << currentActionCooldown;
        if (currentActionCooldown >= actionCooldown)
        {
            coolingDown = false;
            currentActionCooldown = 0.0f;
        }
    }

    switch(jumpingState)
    {
    case JumpingState::NotJumping:
        break;
    case JumpingState::InitialForce:
        verticalForce = 10.0f;
        jumpingState = JumpingState::Falling;
        break;
    case JumpingState::Falling:
        if(this->GetTransform()->GetPosition().y <= nonJumpY)
        {
            verticalForce = 0;
            jumpingState = JumpingState::Landed;
        }
        else
        {              //was 9.81, but it was too 'floaty' in this instance.
            verticalForce -= 20 * dt;
        }
        break;
    case JumpingState::Landed:
        coolingDown = true;
        jumpingState = JumpingState::NotJumping;
        break;
    default:
        //not supposed to be here
        break;
    }

    this->GetTransform()->MoveGlobal(0.0f, verticalForce * dt, 0.0f);

    return checkObstacleCollision(obstacles);
}

void Player::moveLeft(float dt)
{    
    //Once reached destination
    if (abs(this->GetTransform()->GetPosition().x - ((float)(futureLane) * 3)) <= .05f)
    {
        movingLeft = false;
        currentLane = futureLane;
    }
    
    else
    {
        //Current speed
        float speed = movementSpeed * dt;
        this->GetTransform()->MoveGlobal(-movementSpeed, 0.0f, 0.0f);
    }
}

void Player::moveRight(float dt)
{
    //https://www.cplusplus.com/reference/cmath/abs/ - for absolute value
    if (abs(this->GetTransform()->GetPosition().x - ((float)(futureLane) * 3)) <= .05f)
    {
        movingRight = false;
        currentLane = futureLane;
    }

    else
    {
        //Current speed
        float speed = movementSpeed * dt;
        this->GetTransform()->MoveGlobal(movementSpeed, 0.0f, 0.0f);
    }
}

bool Player::checkObstacleCollision(vector<Entity*> obstacles)
{
    for (int i = 0; i < obstacles.size(); i++)
    {
        //Collision check from obstacle i to player - if simple collision check
        Entity* o = obstacles[i];
        
        
       
        //if object is being drawn - otherwise ignore it
        if (obstacles[i]->GetDrawState())
        {
            bool intersects = obstacleIntersectCheck(obstacles[i]);

            //We got a hit!
            if (intersects)
                return true;
        }
    }
    return false;
}

bool Player::obstacleIntersectCheck(Entity* obstacle)
{
    //Getting all the numbers ready, using some vars to simplify
    Transform* obstacleT = obstacle->GetTransform();
    Transform* playerT = this->GetTransform();

    //Obstacle scale -> each of their distances from edge to center -> half the scale of that direction (the size of each part of the cube is 1 that is scaled(1,1,1)
    //So, a (1,1,1) scaled cube is .5 distance from center to center of each face.
    DirectX::XMFLOAT3 obstacleScale = obstacleT->GetScale();
    float obsXScale = obstacleScale.x / 2;
    float obsYScale = obstacleScale.y / 2;
    float obsZScale = obstacleScale.z / 2;

    //X,Y,Z - (the 'center' of each object
    DirectX::XMFLOAT3 obstaclePos = obstacleT->GetPosition();
    float obsX = obstaclePos.x;
    float obsY = obstaclePos.y;
    float obsZ = obstaclePos.z;

    //Player is cube, only one scale needed
    float playerScale = playerT->GetScale().x / 2;

    DirectX::XMFLOAT3 playerPos = playerT->GetPosition();
    float playerX = playerPos.x;
    float playerY = playerPos.y;
    float playerZ = playerPos.z;

    //AABB refresher
    //https://developer.mozilla.org/en-US/docs/Games/Techniques/3D_collision_detection

    if (playerX - playerScale <= obsX + obsXScale && playerX + playerScale >= obsX - obsXScale)
        if (playerY - playerScale <= obsY + obsYScale && playerY + playerScale >= obsY - obsYScale)
            if (playerZ - playerScale <= obsZ + obsZScale && playerZ + playerScale >= obsZ - obsZScale)
                return true;

    return false;
}

//void Player::lostGame()
//{
//}


