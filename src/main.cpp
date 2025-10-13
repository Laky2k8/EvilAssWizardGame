#include "raylib.h"
#include <string>
#include <cmath>
#include <algorithm>
#include "rlgl.h"

#include "Player.h"
#include "textures.h"

#define TITLE "Evil Ass Wizard Game"
#define VERSION_NUM "0.0.1"
#define LAKYSTRATEGY_ERROR "EAWG::Error: "

#define DR 0.0174532925

using namespace std;

float mouseSensitivity = 0.005f;

int resolution = 3;
float render_resolution;

float degToRad(float a) { return a * PI /180.0;}
float FixAng(float a)
{
	if(a>359)
	{
		a-=360;
	}
	
	if(a<0)
	{
		a+=360;
	}
	return a;
}

void drawPlayer(Player plr, float size, Color color);
void raycast(Player plr);

// --- MAP ----
int map_width = 8, map_height = 8;
int tile_size = 64;
int map_walls[] =
{
	1,1,1,1,1,3,1,1,
	1,0,0,2,0,0,0,1,
	1,0,0,4,0,0,0,1,
	1,4,2,2,0,0,5,0,
	2,0,0,0,0,5,0,0,
	2,0,0,0,5,5,5,0,
	2,0,0,0,0,0,0,1,
	1,1,3,1,3,1,3,1,	
};

int map_floor[]=          //floors
{
	7,7,7,7,7,7,7,7,
	7,0,0,7,7,7,7,7,
	7,0,0,0,6,7,7,7,
	7,7,7,7,6,7,7,7,
	7,7,6,6,6,7,7,7,
	7,7,6,7,7,7,7,7,
	7,7,6,7,8,8,8,7,
	7,7,7,7,7,7,7,7,	
};

int map_ceiling[]=          //ceiling
{
 1,1,1,1,1,1,1,1,
 1,5,5,1,0,0,0,1,
 1,5,5,1,0,0,0,1,
 1,1,1,1,1,1,1,1,
 1,2,1,1,1,1,1,1,
 1,2,1,1,1,1,1,1,
 1,1,1,1,0,0,0,1,
 1,1,1,1,1,1,1,1,	
};

void drawMap2D();

int main() 
{
	const int screenWidth = 1024;
	const int screenHeight = 512;
	InitWindow(screenWidth, screenHeight, TITLE);
	SetTargetFPS(180);
	//DisableCursor();

	Image wizard = LoadImage("assets/wizard.png");
	Image wizard_interact = LoadImage("assets/wizard_interact.png");
	bool isInteracting = false;

	Player plr({300, 300}, 200, 60);
	plr.angle = -90;
		plr.deltaPos.x = cos(degToRad(plr.angle));
		plr.deltaPos.y = -sin(degToRad(plr.angle));

	render_resolution = screenWidth / resolution;

	while (!WindowShouldClose())
	{

		string titleBar = (string(TITLE) + " " + string(VERSION_NUM) + " - " + to_string(GetFPS()) + " FPS");
		SetWindowTitle(titleBar.c_str());

		// Update

		/*float mouseX = GetMouseX();
		static float lastMouseX = mouseX;

		plr.angle += (mouseX - lastMouseX) * mouseSensitivity;
		lastMouseX = mouseX;*/

		plr.angle = FixAng(plr.angle);
		plr.deltaPos.x = cos(degToRad(plr.angle));
		plr.deltaPos.y = -sin(degToRad(plr.angle));


		int xOffset = (plr.deltaPos.x < 0) ? -20 : 20;
		int yOffset = (plr.deltaPos.y < 0) ? -20 : 20;

		int ipx = plr.position.x / tile_size;
		int ipx_add_offset = (plr.position.x + xOffset) / tile_size;
		int ipx_sub_offset = (plr.position.x - xOffset) / tile_size;

		int ipy = plr.position.y / tile_size;
		int ipy_add_offset = (plr.position.y + yOffset) / tile_size;
		int ipy_sub_offset = (plr.position.y - yOffset) / tile_size;

		if(IsKeyDown(KEY_W)) 
		{ 

			if(map_walls[ipy * map_width + ipx_add_offset] == 0)
			{
				plr.position.x += plr.deltaPos.x * plr.speed * GetFrameTime();
			}

			if(map_walls[ipy_add_offset * map_width + ipx] == 0)
			{
				plr.position.y += plr.deltaPos.y * plr.speed * GetFrameTime();
			}
			
		} 
		if(IsKeyDown(KEY_S))
		{
			if(map_walls[ipy * map_width + ipx_sub_offset] == 0)
			{
				plr.position.x -= plr.deltaPos.x * plr.speed * GetFrameTime();
			}

			if(map_walls[ipy_sub_offset * map_width + ipx] == 0)
			{
				plr.position.y -= plr.deltaPos.y * plr.speed * GetFrameTime();
			}
		}

		// Strafing
		/*if(IsKeyDown(KEY_A))
		{
			plr.position.x += plr.deltaPos.y * plr.speed * GetFrameTime();
			plr.position.y -= plr.deltaPos.x * plr.speed * GetFrameTime();
		}
		if(IsKeyDown(KEY_D))
		{
			plr.position.x -= plr.deltaPos.y * plr.speed * GetFrameTime();
			plr.position.y += plr.deltaPos.x * plr.speed * GetFrameTime();
		}*/

		if(IsKeyDown(KEY_A))
		{
			plr.angle += plr.speed * GetFrameTime();

			plr.angle = FixAng(plr.angle);
			plr.deltaPos.x = cos(degToRad(plr.angle));
			plr.deltaPos.y = -sin(degToRad(plr.angle));
		}

		if(IsKeyDown(KEY_D))
		{
			plr.angle -= plr.speed * GetFrameTime();

			plr.angle = FixAng(plr.angle);
			plr.deltaPos.x = cos(degToRad(plr.angle));
			plr.deltaPos.y = -sin(degToRad(plr.angle));
		}

		if(IsKeyDown(KEY_E))
		{
			int interactOffsetX = (plr.deltaPos.x < 0) ? -20 : 20;
			int interactOffsetY = (plr.deltaPos.y < 0) ? -20 : 20;

			int interactX = plr.position.x / tile_size;
			int interactX_add = (plr.position.x + xOffset) / tile_size;

			int interactY = plr.position.y / tile_size;
			int interactY_add = (plr.position.y + yOffset) / tile_size;

			if(map_walls[interactY_add * map_width + interactX_add] == 4)
			{
				// If there's a door and we interact with it, open it
				map_walls[interactY_add * map_width + interactX_add] = 0;
			}

			isInteracting = true;

		}


		BeginDrawing();
		ClearBackground({76, 76, 76, 255});
		DrawText((string(TITLE) + " " + string(VERSION_NUM)).c_str(), 720, 20, 20, WHITE);

		drawMap2D();
		raycast(plr);
		drawPlayer(plr, 8, YELLOW);

		// Draw 128x128 wizard image scaled up to 512x512 at bottom right

		if(isInteracting)
		{
			DrawTexturePro(LoadTextureFromImage(wizard_interact), {0,0,128,128}, {screenWidth - 400, screenHeight - 400, 400, 400}, {0,0}, 0, WHITE);

			// Wait a second and then stop interacting
			static float interactTimer = 0;
			interactTimer += GetFrameTime();
			if(interactTimer > 0.5f)
			{
				isInteracting = false;
				interactTimer = 0;
			}
		}
		else
		{
			DrawTexturePro(LoadTextureFromImage(wizard), {0,0,128,128}, {screenWidth - 400, screenHeight - 400, 400, 400}, {0,0}, 0, WHITE);
		}

		

		EndDrawing();
	}
	CloseWindow();
	return 0;
}

// --- PLAYER ---
void drawPlayer(Player plr, float size, Color color)
{
	DrawRectangle(plr.position.x - (size / 2), plr.position.y - (size/2), size, size, color);
	DrawLine(plr.position.x, plr.position.y, plr.position.x + plr.deltaPos.x * 50, plr.position.y + plr.deltaPos.y * 50, color);
}

void drawMap2D()
{
	int x, y;
	int xOffset, yOffset;

	Color tileColor;

	for(y = 0; y < map_height; y++)
	{
		for(x = 0; x < map_width; x++)
		{
			// If wall (not empty)
			if(map_walls[y * map_width + x] > 0)
			{
				tileColor = WHITE;
			}
			else
			{
				tileColor = BLACK;
			}

			xOffset = x * tile_size;
			yOffset = y * tile_size;

			DrawRectangle(xOffset + 1, yOffset + 1, tile_size - 1, tile_size - 1, tileColor);
		}
	}
}

float dist(Vector2 posA, Vector2 posB, float ang)
{
	float distX = posB.x - posA.x;
	float distY = posB.y - posA.y;
	return sqrt(distX * distX + distY * distY);
}

void raycast(Player plr)
{
	int mX, mY;
	int mapPos;
	int view_dist; // How far we are seeing rn

	float rayX, rayY, rayAngle;
	float xOffset, yOffset;

	float distHoriz, distVert;
	float distance;

	float ray_angle_increment = plr.fov / render_resolution;

	int shade;

	rayAngle = plr.angle + (plr.fov/2.0f);
	rayAngle = FixAng(rayAngle);

	for(int ray = 0; ray < render_resolution; ray++)
	{

		distHoriz = 1000000000;
		distVert = 1000000000;

		distance = 0;
		
		int vert_map_texture = 0;
		int horiz_map_texture = 0;

		// --- Vertical Lines ---
		view_dist = 0;
		float Tan = tan(degToRad(rayAngle));

		float vertX = plr.position.x;
		float vertY = plr.position.y;

		// Looking left
		if(cos(degToRad(rayAngle)) > 0.001)
		{
			// Find the ray's first hit position
			rayX = (int)(plr.position.x / tile_size) * tile_size + tile_size;
			rayY = (plr.position.x - rayX) * Tan + plr.position.y;

			// Find next offset position
			xOffset = 64;
			yOffset = -xOffset * Tan;
		}

		// Looking right
		else if(cos(degToRad(rayAngle)) < -0.001)
		{
			// Find the ray's first hit position
			rayX = (int)(plr.position.x / tile_size) * tile_size - 0.0001;
			rayY = (plr.position.x - rayX) * Tan + plr.position.y;

			// Find next offset position
			xOffset = -64;
			yOffset = -xOffset * Tan;
		}


		// Looking straight up or down
		else
		{
			rayX = plr.position.x;
			rayY = plr.position.y;
			view_dist = 8;
		}

		while(view_dist < 8)
		{
			mX = (int)(rayX / 64);
			mY = (int)(rayY / 64);
			mapPos = mY * map_width + mX;

			if(mapPos >= 0 && mapPos < map_width * map_height && map_walls[mapPos] > 0)
			{
				// We hit a wall!!! Yippeee
				view_dist = 8; // We ain't lookin around anymore

				vert_map_texture = map_walls[mapPos] - 1;

				vertX = rayX;
				vertY = rayY;

				distVert = dist(plr.position, {vertX, vertY}, rayAngle);
			}
			else
			{
				// We didn't hit a wall :[
				// Solution: We just... add the offset and keep looking???? Goddamn I love raycasting this is so cool :DDD
				rayX += xOffset;
				rayY += yOffset;
				view_dist += 1;
			}
		}

		// --- Horizontal Lines ---
		view_dist = 0;
		Tan = 1.0 / Tan;

		float horizX = plr.position.x;
		float horizY = plr.position.y;

		// Looking up
		if(sin(degToRad(rayAngle)) > 0.001)
		{
			// Find the ray's first hit position
			rayY = (int)(plr.position.y / tile_size) * tile_size - 0.0001;
			rayX = (plr.position.y - rayY) * Tan + plr.position.x;

			// Find next offset position
			yOffset = -64;
			xOffset = -yOffset * Tan;
		}

		// Looking down
		else if(sin(degToRad(rayAngle)) < -0.001)
		{
			// Find the ray's first hit position
			rayY = (int)(plr.position.y / tile_size) * tile_size + tile_size;
			rayX = (plr.position.y - rayY) * Tan + plr.position.x;

			// Find next offset position
			yOffset = 64;
			xOffset = -yOffset * Tan;
		}

		// Looking straight left or right
		else
		{
			rayX = plr.position.x;
			rayY = plr.position.y;
			view_dist = 8;
		}

		while(view_dist < 8)
		{
			mX = (int)(rayX / 64);
			mY = (int)(rayY / 64);
			mapPos = mY * map_width + mX;

			if(mapPos >= 0 && mapPos < map_width * map_height && map_walls[mapPos] > 0)
			{
				// We hit a wall!!! Yippeee
				view_dist = 8; // We ain't lookin around anymore

				horiz_map_texture = map_walls[mapPos] - 1;

				horizX = rayX;
				horizY = rayY;

				distHoriz = dist(plr.position, {horizX, horizY}, rayAngle);
			}
			else
			{
				// We didn't hit a wall :[
				// Solution: We just... add the offset and keep looking???? Goddamn I love raycasting this is so cool :DDD
				rayX += xOffset;
				rayY += yOffset;
				view_dist += 1;
			}
		}

		// The ray's pos is the shortest distance of the two

		bool hitVertical = false;

		if(distVert < distHoriz)
		{
			rayX = vertX;
			rayY = vertY;
			distance = distVert;

			horiz_map_texture = vert_map_texture;

			shade = 230;
			hitVertical = true;
		}
		else
		{
			rayX = horizX;
			rayY = horizY;
			distance = distHoriz;

			vert_map_texture = horiz_map_texture;

			shade = 178;
			hitVertical = false;
		}

		// Draw the  hit with green :]
		DrawLineEx(plr.position, {rayX, rayY}, 2, GREEN);


		// Fix Fisheye
		float ca_deg = plr.angle - rayAngle;            // difference in degrees

		if(ca_deg < -180.0f) ca_deg += 360.0f;
		if(ca_deg >  180.0f) ca_deg -= 360.0f;

		float ca = degToRad(ca_deg);
		distance = distance * cos(ca);

		float projectionPlaneDistance = (GetScreenWidth() / 2) / tan(degToRad(plr.fov / 2));
		float line_height = (tile_size * projectionPlaneDistance) / distance;

		// Textured walls
		float texY = 0; // which pixel we're currently at in the line
		float texX = 0;
		float texY_step = 32.0 / (float)line_height; // The amount we step to get to the next pixel
		float texY_offset = 0;

		if(line_height > GetScreenHeight())
		{
			texY_offset = (line_height - GetScreenHeight()) / 2.0;
			line_height = GetScreenHeight();
		}
		
		// Center our beautiful lines on screen
		float line_offset = (GetScreenHeight() / 2) - line_height / 2;

		//DrawLineEx({ray*resolution+530, line_offset}, {ray*resolution+530, line_offset + line_height}, resolution, {0, shade, 0, 255});

		// --- Draw 3D Walls ---
		{
			texY = texY_offset * texY_step + horiz_map_texture * 32; // Always get the texture the map calls for :]

			if(hitVertical)
			{
				float texCoord = fmod(rayY, 64.0f);
				if(texCoord < 0) texCoord += 64.0f;
				texX = (int)(texCoord / 2.0f);
				
				// Flip if looking left (angle between 90 and 270)
				if(rayAngle > 90 && rayAngle < 270)
				{
					texX = 31 - texX;
				}
			}
			else
			{
				float texCoord = fmod(rayX, 64.0f);
				if(texCoord < 0) texCoord += 64.0f;
				texX = (int)(texCoord / 2.0f);
				
				// Flip if looking up (angle between 0 and 180)
				if(rayAngle < 180)
				{
					texX = 31 - texX;
				}
			}

			for(int y = 0; y < line_height; y++)
			{

				float brightness = All_Textures[(int)(texY) * 32 + (int)(texX)];

				rlBegin(RL_LINES);
				rlColor4ub(brightness * shade, brightness * shade, brightness * shade, 255);
				rlVertex2f(ray * resolution+530, y+line_offset);
				rlVertex2f(ray * resolution+535, y+line_offset);
				rlEnd();

				texY += texY_step; // Step to the next pixel in the line
			}
		}

		// --- Draw Floors ---
		{
			const int screenH = GetScreenHeight();
			const int screenW = GetScreenWidth();
			
			// Wall bounds
			int wallTop = (int)line_offset;
			int wallBottom = (int)(line_offset + line_height);
			
			// Only render floor below the wall
			for (int y = wallBottom; y < screenH; y++)
			{
				float screenMiddle = screenH / 2.0f;
				float projectionDistance = (screenW / 2.0f) / tanf(degToRad(plr.fov / 2.0f));
				
				// Distance to floor point at this screen row
				float floorDist = (tile_size / 2.0f * projectionDistance) / (y - screenMiddle);
				
				float rayDirX = cosf(degToRad(rayAngle));
				float rayDirY = -sinf(degToRad(rayAngle));
				
				// World position of floor point
				float floorX = plr.position.x + rayDirX * floorDist;
				float floorY = plr.position.y + rayDirY * floorDist;
				
				// Which map cell is this?
				int mapX = (int)(floorX / tile_size);
				int mapY = (int)(floorY / tile_size);
				
				if (mapX < 0 || mapX >= map_width || mapY < 0 || mapY >= map_height)
					continue;
					
				// Check if there's a wall here
				if (map_walls[mapY * map_width + mapX] > 0)
					continue;
					
				int floorTex = map_floor[mapY * map_width + mapX];
				if (floorTex <= 0)
					continue;
					
				// Texture coordinates within the tile
				float texCoordX = fmodf(floorX, tile_size);
				float texCoordY = fmodf(floorY, tile_size);
				if (texCoordX < 0) texCoordX += tile_size;
				if (texCoordY < 0) texCoordY += tile_size;
				
				// Convert to texture coordinates
				int texX = (int)(texCoordX / 2.0f);
				int texY = (int)(texCoordY / 2.0f);
				if (texX < 0) texX = 0; if (texX > 31) texX = 31;
				if (texY < 0) texY = 0; if (texY > 31) texY = 31;
				
				// Sample texture
				int textureIndex = (floorTex - 1) * 32;
				float brightness = All_Textures[(int)(texY) * 32 + (int)(texX) + textureIndex];
				
				float shade = 1.0f / (1.0f + floorDist * 0.001f);
				if (shade < 0.3f) shade = 0.3f;
				
				unsigned char color = (unsigned char)(brightness * shade * 255.0f);
				
				rlBegin(RL_LINES);
				rlColor4ub(color, color, color, 255);
				rlVertex2f(ray * resolution + 530, y);
				rlVertex2f(ray * resolution + 535, y);
				rlEnd();
			}
		}

		// --- Draw Ceiling ---
		{
			const int screenH = GetScreenHeight();
			const int screenW = GetScreenWidth();
			
			// Wall bounds
			int wallTop = (int)line_offset;
			int wallBottom = (int)(line_offset + line_height);
			
			// Only render ceiling above the wall
			for (int y = 0; y < wallTop; y++)
			{
				float screenMiddle = screenH / 2.0f;
				float projectionDistance = (screenW / 2.0f) / tanf(degToRad(plr.fov / 2.0f));
				
				// Distance to floor point at this screen row
				float ceilingDist = (tile_size / 2.0f * projectionDistance) / (screenMiddle - y);
				
				float rayDirX = cosf(degToRad(rayAngle));
				float rayDirY = -sinf(degToRad(rayAngle));
				
				// World position of floor point
				float floorX = plr.position.x + rayDirX * ceilingDist;
				float floorY = plr.position.y + rayDirY * ceilingDist;
				
				// Which map cell is this?
				int mapX = (int)(floorX / tile_size);
				int mapY = (int)(floorY / tile_size);
				
				if (mapX < 0 || mapX >= map_width || mapY < 0 || mapY >= map_height)
					continue;
					
				// Check if there's a wall here
				if (map_walls[mapY * map_width + mapX] > 0)
					continue;
					
				int ceilTex = map_ceiling[mapY * map_width + mapX];
				if (ceilTex <= 0)
					continue;
					
				// Texture coordinates within the tile
				float texCoordX = fmodf(floorX, tile_size);
				float texCoordY = fmodf(floorY, tile_size);
				if (texCoordX < 0) texCoordX += tile_size;
				if (texCoordY < 0) texCoordY += tile_size;
				
				// Convert to texture coordinatess
				int texX = (int)(texCoordX / 2.0f);
				int texY = (int)(texCoordY / 2.0f);
				if (texX < 0) texX = 0; if (texX > 31) texX = 31;
				if (texY < 0) texY = 0; if (texY > 31) texY = 31;
				
				// Sample texture
				int textureIndex = (ceilTex - 1) * 32;
				float brightness = All_Textures[(int)(texY) * 32 + (int)(texX) + textureIndex];
				
				float shade = 1.0f / (1.0f + ceilingDist * 0.001f);
				if (shade < 0.3f) shade = 0.3f;
				
				unsigned char color = (unsigned char)(brightness * shade * 255.0f);
				
				rlBegin(RL_LINES);
				rlColor4ub(color, color, color, 255);
				rlVertex2f(ray * resolution + 530, y);
				rlVertex2f(ray * resolution + 535, y);
				rlEnd();
			}
		}

		rayAngle -= ray_angle_increment;
		rayAngle = FixAng(rayAngle);
	}

}
