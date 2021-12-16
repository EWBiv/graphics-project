#pragma once

#include "DXCore.h"
#include <DirectXMath.h>
#include <wrl/client.h> // Used for ComPtr - a smart pointer for COM objects
#include "Mesh.h"
#include <vector>
#include "Entity.h"
#include "Camera.h"
#include "Material.h"
#include "SimpleShader.h"
#include "Lights.h"
#include "Sky.h"
#include "Player.h"
#include "Chunk.h"
#include "WICTextureLoader.h"

#include "SpriteBatch.h"
#include "SpriteFont.h"

enum GameState
{
	StartMenu,
	InGame,
	RetryMenu
};

class Game : public DXCore
{

public:
	Game(HINSTANCE hInstance);
	~Game();

	// Overridden setup and game loop methods, which
	// will be called automatically
	void Init();
	void OnResize();
	void Update(float deltaTime, float totalTime);
	void Draw(float deltaTime, float totalTime);

private:
	//Keeping track of game state
	GameState currentGameState;
	void CreateStartingChunks();
	void RestartGame();

	//
	void InGame(float deltaTime, float totalTime);

	//Ambient Color
	DirectX::XMFLOAT3 ambientColor;
	Light backLight;
	Light exitLight;
	Light bridgeLight;
	Light bridgeLight1;
	Light bridgeLight2;
	Light bridgeLight3;

	Light lightsArr[6];

	// Initialization helper methods - feel free to customize, combine, etc.
	void LoadShaders(); 
	void SetupGameObjects();
	void SetupLights();

	//Chunk stuff
	int chunkNumber;
	int chunkSlotAmount;


	//Values (no 'magic numbers'!)
	float speed;
	float speedDeltaPerChunk;

	std::vector<Chunk*> chunks;
	std::vector<Entity*> InitializeChunkObstacleList();


	//Vectors for holding objects that can collide with the player
	std::vector<Entity*> forwardChunkObstacles;
	std::vector<Entity*> backChunkObstacles;
	std::vector<Entity*> allObstacles; //Used solely for collision detection (passing the whole list, makes it easier)
	DirectX::XMFLOAT3 floorInitialPosition; //Don't want to make the floor an 'obstacle' but it needs a reset position

	//Vectors for holding general objects
	std::vector<Entity*> entities;
	std::vector<Material*> materials;
	std::vector<Mesh*> meshes;

	Player* player;
	Camera* camera1;

	Sky* skyInstance;
	
	// Note the usage of ComPtr below
	//  - This is a smart pointer for objects that abide by the
	//    Component Object Model, which DirectX objects do
	//  - More info here: https://github.com/Microsoft/DirectXTK/wiki/ComPtr

	// Buffers to hold actual geometry data
	Microsoft::WRL::ComPtr<ID3D11Buffer> vertexBuffer;
	Microsoft::WRL::ComPtr<ID3D11Buffer> indexBuffer;


	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> obstacleRoughness;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> obstacleMetal;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> obstacleNormal;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> noMetal;

	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> obstacleGreen;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> obstacleOrange;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> obstaclePink;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> obstaclePurple;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> obstacleTurquoise;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> obstacleYellow;

	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> floorSRV;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> playerSRV;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> playerMetal;

	Microsoft::WRL::ComPtr<ID3D11SamplerState> normalSamplerState;

	void CreateObstacleMaterial(Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> albedo);
	
	// Shaders and shader-related constructs
	SimplePixelShader* pixelShader;
	SimpleVertexShader* vertexShader;

	SimplePixelShader* pixelShaderSky;
	SimpleVertexShader* vertexShaderSky;

	//For text
	DirectX::SpriteBatch* sBatch;
	DirectX::SpriteFont* cambriaFont26;
	void DisplayHUD();
	void DisplayText(std::string text, std::string text2, float xPos, float yPos, float xPos2, float yPos2);
	
};

