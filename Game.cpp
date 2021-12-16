#include "Game.h"
#include "Vertex.h"
#include "Input.h"
#include <algorithm>    // contains random shuffle
#include <iostream>
#include <ctime> //Used for seeding random

// Needed for a helper function to read compiled shader files from the hard drive
#pragma comment(lib, "d3dcompiler.lib")
#include <d3dcompiler.h>

// For the DirectX Math library
using namespace DirectX;

// --------------------------------------------------------
// Constructor
//
// DXCore (base class) constructor will set up underlying fields.
// DirectX itself, and our window, are not ready yet!
//
// hInstance - the application's OS-level handle (unique ID)
// --------------------------------------------------------
Game::Game(HINSTANCE hInstance)
	: DXCore(
		hInstance,					// The application's handle
		"Cube Space Surfers",		// Text for the window's title bar
		1280,						// Width of the window's client area
		720,						// Height of the window's client area
		true)						// Show extra stats (fps) in title bar?
{
	camera1 = 0;

	#if defined(DEBUG) || defined(_DEBUG)
		// Do we want a console window?  Probably only in debug mode
		CreateConsoleWindow(500, 120, 32, 120);
		printf("Console window created successfully.  Feel free to printf() here.\n");
	#endif

}

// --------------------------------------------------------
// Destructor - Clean up anything our game has created:
//  - Release all DirectX objects created here
//  - Delete any objects to prevent memory leaks
// --------------------------------------------------------
Game::~Game()
{
	// Note: Since we're using smart pointers (ComPtr),
	// we don't need to explicitly clean up those DirectX objects
	// - If we weren't using smart pointers, we'd need
	//   to call Release() on each DirectX object created in Game


	//Clean up shaders
	delete vertexShader;
	vertexShader = nullptr;

	delete pixelShader;
	pixelShader = nullptr;

	delete vertexShaderSky;
	vertexShaderSky = nullptr;

	delete pixelShaderSky;
	pixelShaderSky = nullptr;

	delete skyInstance;
	skyInstance = nullptr;

	//Clean up camera (just required one here)
	delete camera1;
	camera1 = nullptr;

	//Clean up lists of meshes, entities (obstacles and player included!) & materials
	for (int i = 0; i < meshes.size(); i++)
	{
		delete meshes[i];
		meshes[i] = nullptr;
	}

	for (int i = 0; i < entities.size(); i++)
	{
		delete entities[i];
		entities[i] = nullptr;
	}

	for (int i = 0; i < materials.size(); i++)
	{
		delete materials[i];
		materials[i] = nullptr;
	}

	for (int i = 0; i < chunks.size(); i++)
	{
		delete chunks[i];
		chunks[i] = nullptr;
	}

	delete sBatch;
	sBatch = nullptr;

	delete cambriaFont26;
	cambriaFont26 = nullptr;
}

// --------------------------------------------------------
// Called once per program, after DirectX and the window
// are initialized but before the game loop.
// --------------------------------------------------------
void Game::Init()
{
	//https://stackoverflow.com/questions/1824656/hiding-the-cursor
	//Official documentation of it:
	//https://docs.microsoft.com/en-us/windows/win32/api/winuser/nf-winuser-showcursor
	ShowCursor(false);

	currentGameState = GameState::StartMenu;

	//https://www.cplusplus.com/reference/cstdlib/srand/
	//Seeding random
	srand(time(NULL));


	//No 'magic numbers'
	speed = -20.0f;
	speedDeltaPerChunk = -0.15f;

	// Helper methods for loading shaders, creating some basic
	// geometry to draw and some simple camera matrices.
	//  - You'll be expanding and/or replacing these later
	LoadShaders();
	SetupGameObjects();
	
	// Tell the input assembler stage of the pipeline what kind of
	// geometric primitives (points, lines or triangles) we want to draw.  
	// Essentially: "What kind of shape should the GPU draw with our data?"
	context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	//Create camera once we have aspect ratio available											//Far z very selective to what we have
	camera1 = new Camera(XMFLOAT3(0,-2,-40), 5.0f, 5.0f, XM_PIDIV2, (float)width / height,0.01f,36.0f);

	SetupLights();
}

// --------------------------------------------------------
// Loads shaders from compiled shader object (.cso) files
// and also created the Input Layout that describes our 
// vertex data to the rendering pipeline. 
// - Input Layout creation is done here because it must 
//    be verified against vertex shader byte code
// - We'll have that byte code already loaded below
// --------------------------------------------------------
void Game::LoadShaders()
{
	vertexShader = new SimpleVertexShader(device.Get(),context.Get(),GetFullPathTo_Wide(L"VertexShader.cso").c_str());
	vertexShaderSky = new SimpleVertexShader(device.Get(), context.Get(), GetFullPathTo_Wide(L"SkyVertexShader.cso").c_str());

	pixelShader = new SimplePixelShader(device.Get(), context.Get(), GetFullPathTo_Wide(L"PixelShader.cso").c_str());
	pixelShaderSky = new SimplePixelShader(device.Get(), context.Get(), GetFullPathTo_Wide(L"SkyPixelShader.cso").c_str());
}



//Creation/storing of meshes, textures & entities
void Game::SetupGameObjects()
{
	// Set up sprite batch and sprite font
	sBatch = new SpriteBatch(context.Get());
	cambriaFont26 = new SpriteFont(device.Get(), GetFullPathTo_Wide(L"../../Assets/Fonts/Cambria26.spritefont").c_str());


	// Create some temporary variables to represent colors
	// - Not necessary, just makes things more readable
	XMFLOAT3 normal = XMFLOAT3(0, 0, -1);
	XMFLOAT2 uv = XMFLOAT2(0, 0);

	Mesh* cubeMesh = new Mesh(GetFullPathTo("../../Assets/Models/cube.obj").c_str(), device, context);
	Mesh* quadMesh = new Mesh(GetFullPathTo("../../Assets/Models/quad.obj").c_str(), device, context);

	meshes.push_back(cubeMesh);
	meshes.push_back(quadMesh);

	D3D11_SAMPLER_DESC normalSamplerDesc = {};

	//Wrap outside of range
	normalSamplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	normalSamplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	normalSamplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;

	//Very nice filtering
	normalSamplerDesc.Filter = D3D11_FILTER_ANISOTROPIC;
	normalSamplerDesc.MaxAnisotropy = 16;				//Highest amount of filtering

	normalSamplerDesc.MaxLOD = D3D11_FLOAT32_MAX; //Almost always use mip-mapping

	//Create sample state - using desc. + pointer to it
	device->CreateSamplerState(&normalSamplerDesc, normalSamplerState.GetAddressOf());

	//WIC Textures
	CreateWICTextureFromFile(device.Get(), context.Get(), GetFullPathTo_Wide(L"../../Assets/Textures/no_normal.png").c_str(), nullptr, obstacleNormal.GetAddressOf());
	CreateWICTextureFromFile(device.Get(), context.Get(), GetFullPathTo_Wide(L"../../Assets/Textures/bronze_roughness.png").c_str(), nullptr, obstacleRoughness.GetAddressOf());
	CreateWICTextureFromFile(device.Get(), context.Get(), GetFullPathTo_Wide(L"../../Assets/Textures/bronze_metal.png").c_str(), nullptr, obstacleMetal.GetAddressOf());

	CreateWICTextureFromFile(device.Get(), context.Get(), GetFullPathTo_Wide(L"../../Assets/Textures/no_metalness.png").c_str(), nullptr, noMetal.GetAddressOf());

	CreateWICTextureFromFile(device.Get(), context.Get(), GetFullPathTo_Wide(L"../../Assets/Textures/obstacle_green.png").c_str(), nullptr, obstacleGreen.GetAddressOf());
	CreateWICTextureFromFile(device.Get(), context.Get(), GetFullPathTo_Wide(L"../../Assets/Textures/obstacle_orange.png").c_str(), nullptr, obstacleOrange.GetAddressOf());
	CreateWICTextureFromFile(device.Get(), context.Get(), GetFullPathTo_Wide(L"../../Assets/Textures/obstacle_pink.png").c_str(), nullptr, obstaclePink.GetAddressOf());
	CreateWICTextureFromFile(device.Get(), context.Get(), GetFullPathTo_Wide(L"../../Assets/Textures/obstacle_purple.png").c_str(), nullptr, obstaclePurple.GetAddressOf());
	CreateWICTextureFromFile(device.Get(), context.Get(), GetFullPathTo_Wide(L"../../Assets/Textures/obstacle_turquoise.png").c_str(), nullptr, obstacleTurquoise.GetAddressOf());
	CreateWICTextureFromFile(device.Get(), context.Get(), GetFullPathTo_Wide(L"../../Assets/Textures/obstacle_yellow.png").c_str(), nullptr, obstacleYellow.GetAddressOf());

	CreateWICTextureFromFile(device.Get(), context.Get(), GetFullPathTo_Wide(L"../../Assets/Textures/floor.png").c_str(), nullptr, floorSRV.GetAddressOf());
	CreateWICTextureFromFile(device.Get(), context.Get(), GetFullPathTo_Wide(L"../../Assets/Textures/player.png").c_str(), nullptr, playerSRV.GetAddressOf());
	CreateWICTextureFromFile(device.Get(), context.Get(), GetFullPathTo_Wide(L"../../Assets/Textures/player_metal.png").c_str(), nullptr, playerMetal.GetAddressOf());

	//Sky stuff
	skyInstance = new Sky(cubeMesh, vertexShaderSky, pixelShaderSky, normalSamplerState, device, GetFullPathTo_Wide(L"../../Assets/Textures/blueGradient.dds").c_str());


	//Making materials and storing them

	CreateObstacleMaterial(obstacleGreen);
	CreateObstacleMaterial(obstacleOrange);
	CreateObstacleMaterial(obstaclePink);
	CreateObstacleMaterial(obstaclePurple);
	CreateObstacleMaterial(obstacleTurquoise);
	CreateObstacleMaterial(obstacleYellow);
	
	Material* floorMat = new Material(pixelShader, vertexShader, XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f));
	floorMat->AddSampler("BasicSampler", normalSamplerState);
	floorMat->AddTextureSRV("Albedo", floorSRV);
	floorMat->AddTextureSRV("NormalMap", obstacleNormal);
	floorMat->AddTextureSRV("RoughnessMap", obstacleRoughness);
	floorMat->AddTextureSRV("MetalnessMap", noMetal);

	Material* playerMat = new Material(pixelShader, vertexShader, XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f));
	playerMat->AddSampler("BasicSampler", normalSamplerState);
	playerMat->AddTextureSRV("Albedo", playerSRV);
	playerMat->AddTextureSRV("NormalMap", obstacleNormal);
	playerMat->AddTextureSRV("RoughnessMap", obstacleRoughness);
	playerMat->AddTextureSRV("MetalnessMap", playerMetal);
	
	materials.push_back(floorMat);
	materials.push_back(playerMat);

	Entity *floor = new Entity(meshes[1],materials[6],true);
	floorInitialPosition = XMFLOAT3(0.0f, -5.0f, 2.0f);
	Transform *floorTransform = floor->GetTransform();
	floorTransform->SetScale(5.0f, 5.0f, 40.0f);
	floorTransform->MoveGlobal(0.0f,-5.0f,2.0f);

	//General Entity list for drawing
	entities.push_back(floor);


	//Chunk stuff
	chunkSlotAmount = 18;
	//Have two chunks - the reason is so that they 'flow' -> to simulate it is continuous. Only one meant that it would reach the end, there would be nothing behind it and it would teleport
	forwardChunkObstacles = InitializeChunkObstacleList();
	backChunkObstacles = InitializeChunkObstacleList();

	//Thank you insert... https://stackoverflow.com/questions/50071664/insert-list-to-end-of-vector
	entities.insert(entities.end(), forwardChunkObstacles.begin(), forwardChunkObstacles.end());
	entities.insert(entities.end(), backChunkObstacles.begin(), backChunkObstacles.end());

	allObstacles.insert(allObstacles.end(), forwardChunkObstacles.begin(), forwardChunkObstacles.end());
	allObstacles.insert(allObstacles.end(), backChunkObstacles.begin(), backChunkObstacles.end());
	
	
	CreateStartingChunks();
	

	//Add player
	player = new Player(meshes[0], materials[7]);
	player->GetTransform()->SetScale(1.0f, 1.0f, 1.0f);
	player->GetTransform()->MoveGlobal(0.0f, -4.5f, -35.0f);

	entities.push_back(player);
}

void Game::CreateObstacleMaterial(Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> albedo)
{
	Material* mat = new Material(pixelShader, vertexShader, XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f));
	mat->AddSampler("BasicSampler", normalSamplerState);
	mat->AddTextureSRV("Albedo", albedo);
	mat->AddTextureSRV("NormalMap", obstacleNormal);
	mat->AddTextureSRV("RoughnessMap", obstacleRoughness);
	mat->AddTextureSRV("MetalnessMap", obstacleMetal);

	materials.push_back(mat);
}

std::vector<Entity*> Game::InitializeChunkObstacleList()
{
	std::vector<Entity*> tempList;
	for (int i = 0; i < chunkSlotAmount; i++)
	{
		Material* m = materials[(rand() % (materials.size() - 2))];

		//Garbage collection handled by the larger entity list 'entities'
		Entity* o = new Entity(meshes[0], m, false);

		Transform* oTransform = o->GetTransform();
		
		float randomXScale = ((float)(rand() % 11)) * .1f;
		float randomYScale = ((float)(rand() % 11)) * .1f;
		float randomZScale = ((float)(rand() % 11)) * .1f;
		oTransform->SetScale(1.0f + randomXScale, 1.0f + randomYScale, 1.0f + randomZScale);

		//Obstacles -> seperate list to keep track of them...
		tempList.push_back(o);
	}
	return tempList;
}





void Game::SetupLights()
{
	//Ambient color trying to match similar blue of sky
	//ambientColor = XMFLOAT3(.682f, .682f, 1.0f);
	ambientColor = XMFLOAT3(.082f, .3f, .6f);

	backLight = {};
	backLight.Type = LIGHT_TYPE_DIRECTIONAL;
	backLight.Direction = XMFLOAT3(-.5f, -.4f, .6f);
	backLight.Color = XMFLOAT3(.7f, .7f, .7f);
	backLight.Intensity = .67f;

	exitLight = {};
	exitLight.Type = LIGHT_TYPE_POINT;
	exitLight.Range = 20.0f;
	exitLight.Position = XMFLOAT3(0.0f, 3.0f, 5.0f);
	exitLight.Intensity = 30.0f;
	exitLight.Color = XMFLOAT3(1.0f, .05f, 0.05f);

	//https://developer.valvesoftware.com/wiki/Lighting - 'Incandeescent tube' light value (!!!!)
	bridgeLight = {};
	bridgeLight.Type = LIGHT_TYPE_POINT;
	bridgeLight.Range = 20.0f;
	bridgeLight.Position = XMFLOAT3(0.0f, 4.0f, -5.0f);
	bridgeLight.Intensity = 10.0f;
	bridgeLight.Color = XMFLOAT3(1.0f, .96078f, .5686f);

	bridgeLight1 = {};
	bridgeLight1.Type = LIGHT_TYPE_POINT;
	bridgeLight1.Range = 20.0f;
	bridgeLight1.Position = XMFLOAT3(0.0f, 4.0f, -15.0f);
	bridgeLight1.Intensity = 10.0f;
	bridgeLight1.Color = XMFLOAT3(1.0f, .96078f, .5686f);

	bridgeLight2 = {};
	bridgeLight2.Type = LIGHT_TYPE_POINT;
	bridgeLight2.Range = 20.0f;
	bridgeLight2.Position = XMFLOAT3(0.0f, 4.0f, -25.0f);
	bridgeLight2.Intensity = 2.0f;
	bridgeLight2.Color = XMFLOAT3(1.0f, .96078f, .5686f);

	bridgeLight3 = {};
	bridgeLight3.Type = LIGHT_TYPE_POINT;
	bridgeLight3.Range = 20.0f;
	bridgeLight3.Position = XMFLOAT3(0.0f, 4.0f, -45.0f);
	bridgeLight3.Intensity = 2.0f;
	bridgeLight3.Color = XMFLOAT3(1.0f, .96078f, .5686f);


	lightsArr[0] = backLight;
	lightsArr[1] = exitLight;
	lightsArr[2] = bridgeLight;
	lightsArr[3] = bridgeLight1;
	lightsArr[4] = bridgeLight2;
	lightsArr[5] = bridgeLight3;
}

// --------------------------------------------------------
// Handle resizing DirectX "stuff" to match the new window size.
// For instance, updating our projection matrix's aspect ratio.
// --------------------------------------------------------
void Game::OnResize()
{
	// Handle base-level DX resize stuff
	DXCore::OnResize();
	
	//update camera projection to adhere to new width/height
	if (camera1)
	{
		camera1->UpdateProjectionMatrix((float)width / height);
	}
}

void Game::CreateStartingChunks()
{
	//if already stuff there, delete it!
	for (int i = 0; i < chunks.size(); i++)
	{
		delete chunks[i];
		chunks[i] = nullptr;
	}

	chunks.clear();

	Chunk* frontChunk = new Chunk(forwardChunkObstacles, 1, chunkSlotAmount);
	Chunk* backChunk = new Chunk(backChunkObstacles, 2, chunkSlotAmount);
	chunkNumber = 3;	//(for next chunk created)

	chunks.push_back(frontChunk);
	chunks.push_back(backChunk);

	frontChunk->ArrangeObstacles(0.0f);
	backChunk->ArrangeObstacles(40.0f);
}

void Game::RestartGame()
{
	//Reset Starting Chunks (this also deletes existing ones)
	CreateStartingChunks();

	//Move player to starting lane and reset all their values tied to movement, etc.
	player->NewGame();

	speed = -20.0f; //reseting speed that gets incremented as a game progresses

	currentGameState = GameState::InGame;
}

void Game::InGame(float deltaTime, float totalTime)
{
	// Example input checking: Quit if the escape key is pressed
	if (Input::GetInstance().KeyDown(VK_ESCAPE))
		Quit();

	//Update player + pass in all obstacles. If returns true, you hit one!
	if (player->Update(deltaTime, allObstacles))
	{
		currentGameState = GameState::RetryMenu;
		ShowCursor(true);
	}

	//Move light w/ player
	//XMFLOAT3 playerPos = player->GetTransform()->GetPosition();
	//lightsArr[2].Position = XMFLOAT3(playerPos.x, lightsArr[2].Position.y, lightsArr[2].Position.z);

	Entity* floor = entities[0];
	//once floor hits threshold, reset floor + objects
	float floorResetZ = -40.0f;
	if (floor->GetTransform()->GetPosition().z < floorResetZ)
		floor->GetTransform()->SetPosition(floorInitialPosition.x, floorInitialPosition.y, floorInitialPosition.z);

	//Move it along
	floor->GetTransform()->MoveGlobal(0.0f, 0.0f, (speed * deltaTime));

	float obstacleResetZ = -80.0f;
	for (int i = 0; i < chunks.size(); i++)
	{
		if (chunks[i]->GetForwardZ() < obstacleResetZ)
		{
			//(Only shuffle right after you go off screen)
			//http://www.cplusplus.com/reference/algorithm/random_shuffle/
			std::vector<Entity*> chunkObstacles = chunks[i]->GetObstacles();
			std::random_shuffle(chunkObstacles.begin(), chunkObstacles.end());

			//Create new 'chunk'
			Chunk* newChunk = new Chunk(chunkObstacles, chunkNumber, chunkSlotAmount);
			chunkNumber++;
			newChunk->ArrangeObstacles(0.0f);

			//Get rid of old chunk
			Chunk* oldChunk = chunks[i];
			delete oldChunk;
			oldChunk = nullptr;

			//Replace w/ new chunk in list
			chunks[i] = newChunk;
			speed += speedDeltaPerChunk;
		}
		chunks[i]->MoveChunk(speed, deltaTime);
	}

	camera1->Update(deltaTime);
}

// --------------------------------------------------------
// Update your game here - user input, move objects, AI, etc.
// --------------------------------------------------------
void Game::Update(float deltaTime, float totalTime)
{
	//Get a reference to the input manager
	Input& input = Input::GetInstance(); //Used for starting/retrying to simplify the 'player' implementation (keep it away from state machine stuff)

	switch (currentGameState)
	{
	case GameState::InGame:
		InGame(deltaTime, totalTime);
		break;
	case GameState::StartMenu:
		if (input.KeyPress('X'))
			currentGameState = GameState::InGame;
		break;
	case GameState::RetryMenu:
		if (input.KeyPress('X'))
		{
			RestartGame();
			currentGameState = GameState::InGame;
			ShowCursor(false);
		}
		break;
	default:
		break;
	}
}





// --------------------------------------------------------
// Clear the screen, redraw everything, present to the user
// --------------------------------------------------------
void Game::Draw(float deltaTime, float totalTime)
{
	// Background color (Cornflower Blue in this case) for clearing
	const float color[4] = { 0.4f, 0.6f, 0.75f, 0.0f };

	// Clear the render target and depth buffer (erases what's on the screen)
	//  - Do this ONCE PER FRAME
	//  - At the beginning of Draw (before drawing *anything*)
	context->ClearRenderTargetView(backBufferRTV.Get(), color);
	context->ClearDepthStencilView(depthStencilView.Get(),D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL,1.0f,0);

	// DRAW EACH ENTITY
	for(int i = 0; i < entities.size(); i++)
	{
		//If it is supposed to be drawn (basically to enable or disable an objects rendering)
		if (entities[i]->GetDrawState() == true)
		{
			SimplePixelShader* p = entities[i]->GetMaterial()->GetPixelShader();

			p->SetData("lights", &lightsArr, sizeof(Light) * 6);
			p->SetFloat3("ambient", ambientColor);
			entities[i]->GetMaterial()->ReadyTexture();
			entities[i]->Draw(context, camera1, totalTime);
		}
	}

	//Draw the sky
	skyInstance->Draw(context,camera1);

	std::string menuText;
	std::string menuText2;
	//Draw the HUD!!
	switch (currentGameState)
	{
		
	case GameState::InGame:
		DisplayHUD();
		break;
	case GameState::StartMenu:
		menuText = "Use 'A', 'D', and 'SPACE' to dodge cubes in this infinite runner!";
		menuText2 = "Press 'X' to Start!";
		DisplayText(menuText, menuText2, 0, 0, width / 2, height / 2);
		break;
	case GameState::RetryMenu:
		menuText = "You survived " + std::to_string(chunkNumber - 2) + " chunks!";
		menuText2 = "Press 'X' to Retry!";
		DisplayText(menuText, menuText2, 0, 0, width / 2, height / 2);
		break;
	default:
		break;
	}

	// Present the back buffer to the user
	//  - Puts the final frame we're drawing into the window so the user can see it
	//  - Do this exactly ONCE PER FRAME (always at the very end of the frame)
	swapChain->Present(0, 0);

	// Due to the usage of a more sophisticated swap chain,
	// the render target must be re-bound after every call to Present()
	context->OMSetRenderTargets(1, backBufferRTV.GetAddressOf(), depthStencilView.Get());
}

void Game::DisplayHUD()
{

	sBatch->Begin();
	//thanks to IGME 540 '2D Rendering With Spritebatch w/ the to_string()
	std::string hudText = "Chunks Survived : " + std::to_string(chunkNumber-2);
	cambriaFont26->DrawString(sBatch, hudText.c_str(), XMFLOAT2(0, 0));
	sBatch->End();

	//End() doesn't reset a couple things Begin() modifies..
	context->OMSetBlendState(0, 0, 0xFFFFFFFF);
	context->RSSetState(0);
	context->OMSetDepthStencilState(0, 0);
}

void Game::DisplayText(std::string text,std::string text2,float xPos, float yPos, float xPos2, float yPos2)
{
	sBatch->Begin();
	cambriaFont26->DrawString(sBatch, text.c_str(), XMFLOAT2(xPos, yPos));
	cambriaFont26->DrawString(sBatch, text2.c_str(), XMFLOAT2(xPos2, yPos2));
	sBatch->End();

	//End() doesn't reset a couple things Begin() modifies..
	context->OMSetBlendState(0, 0, 0xFFFFFFFF);
	context->RSSetState(0);
	context->OMSetDepthStencilState(0, 0);
}