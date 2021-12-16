#pragma once
#include "Entity.h"
#include "Transform.h"
#include "Camera.h"
#include <vector>

//used a refresher on c++ enums https://docs.microsoft.com/en-us/cpp/cpp/enumerations-cpp?view=msvc-170
enum Lane
{
	LeftLane = -1, MiddleLane = 0, RightLane = 1
};

enum JumpingState
{
	NotJumping, InitialForce, Falling, Landed 
};

//Container used to tie entity and camera together for transform updates
class Player : public Entity
{
public:
	Player(Mesh* mesh, Material* mat);
	~Player();

	//Update (for player movement) - if it returns true, the player collided!
	bool Update(float dt, std::vector<Entity*> obstacles);
	void NewGame();
private:
	Lane currentLane;
	Lane futureLane;
	float movementSpeed;
	float actionCooldown;
	float currentActionCooldown;
	bool coolingDown;

	void ResetValues();

	bool movingLeft;
	bool movingRight;
	JumpingState jumpingState;

	//Starting Y being stored (there's no collision detection of the floor or anything here)
	float nonJumpY;

	//The 'force' being applied vertically by jumping force + gravity (1D vector, kinda)
	float verticalForce;

	void moveLeft(float dt);
	void moveRight(float dt);
	bool checkObstacleCollision(std::vector<Entity*> obstacles);
	bool obstacleIntersectCheck(Entity* obstacle);
};

