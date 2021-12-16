#include "Chunk.h"
#include "Player.h"

using namespace std;

Chunk::Chunk(std::vector<Entity*> obstaclesSubset, int chunkNum, int _slotAmount)
{
	slotAmount = _slotAmount;

	//Future to do: have a base number that slowly increases, less variance
	//int baseObstacleNumber = 3 + (chunkNum / 5);
	//int randomAdditional = rand() % 3;

	obstacleProbability = .05f + (chunkNum * .005f);
	if(obstacleProbability > .65f) obstacleProbability = .65f;
	
	//Figuring out which 'slots' will contain obstacles
	for (int i = 0; i < slotAmount; i++)
	{
		float prob = ((float)(rand() % 101))/ 100.0f;
		if (prob <= obstacleProbability)
			slotList.push_back(true);
		
		else
			slotList.push_back(false);
	}

	obstacles = obstaclesSubset;
}

Chunk::~Chunk()
{
}

vector<bool> Chunk::ObstaclesToDraw()
{
	return slotList;
}

void Chunk::ArrangeObstacles(float startZ)
{
	for (int i = 0; i < slotList.size(); i++)
	{
		if (slotList[i])
		{
			float x;
			switch (i % 3)
			{
			case 0:
				x = Lane::LeftLane * 3.0f;
				break;

			case 1:
				x = Lane::MiddleLane;
				break;

			case 2:
			default:
				x = Lane::RightLane * 3.0f;
				break;
			}

			//then find z using / 3 rather than % 3
			int zIncrement = i / 3;
			float z = startZ + (zIncrement * 6.6666667);


			obstacles[i]->GetTransform()->SetPosition(x, -4.5f, z);

		}
		obstacles[i]->SetDrawState(slotList[i]);
	}
	forwardZ = startZ;
}

void Chunk::MoveChunk(float speed, float dt)
{
	for (unsigned int i = 0; i < obstacles.size(); i++)
	{
		obstacles[i]->GetTransform()->MoveGlobal(0.0f, 0.0f, speed * dt);
	}
	forwardZ += (speed * dt);
}

float Chunk::GetForwardZ()
{
	return forwardZ;
}

std::vector<Entity*> Chunk::GetObstacles()
{
	return obstacles;
}
