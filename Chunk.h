#pragma once
#include <vector>
#include "Entity.h"



//Class containing data regarding each new segment of the endless running space
class Chunk
{
public:
	Chunk(std::vector<Entity*> obstaclesSubset, int chunkNum, int slotAmount);
	~Chunk();
	std::vector<bool> ObstaclesToDraw();
	
	void ArrangeObstacles(float startZ);
	void MoveChunk(float speed, float dt);
	float GetForwardZ();
	std::vector<Entity*> GetObstacles();

private:
	std::vector<Entity*> obstacles;
	std::vector<bool> slotList;

	int slotAmount;
	float obstacleProbability;
	float forwardZ;
};

