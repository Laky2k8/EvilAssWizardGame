#include "raylib.h"
#include <string>
#include <cmath>
#include <algorithm>
#include "rlgl.h"

#include "Player.h"
#include "Sprite.h"


#define TITLE "Evil Ass Wizard Game"
#define VERSION_NUM "0.5"
#define LAKYSTRATEGY_ERROR "EAWG::Error: "

#define DR 0.0174532925

using namespace std;

float mouseSensitivity = 0.005f;

int resolution = 1;
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
int map_width = 21, map_height = 15;
int tile_size = 64;
int map_walls[] =
{
        1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
        1,0,4,0,0,0,1,0,0,0,0,0,0,0,1,0,0,0,0,0,1,
        1,1,1,1,1,0,1,0,1,1,1,1,1,0,0,0,1,1,1,0,1,
        1,0,0,0,0,0,0,0,1,0,1,0,0,0,1,0,1,0,0,0,1,
        1,0,1,1,1,1,0,1,1,0,1,0,1,1,1,0,1,1,1,1,1,
        1,0,1,0,0,1,0,0,0,0,1,0,1,4,0,0,0,0,0,0,1,
        1,0,1,5,1,6,1,5,1,0,1,0,1,0,1,1,1,1,1,0,1,
        1,0,1,0,0,0,0,0,1,3,1,0,1,0,1,0,0,0,0,0,1,
        1,0,1,0,0,0,0,0,1,2,1,0,1,1,1,5,1,1,1,0,1,
        1,0,5,0,0,0,0,0,5,0,1,0,0,0,1,0,1,0,1,0,1,
        1,0,1,1,1,7,1,5,1,0,1,1,1,0,1,0,1,0,1,0,1,
        1,0,0,0,1,1,1,0,1,4,0,0,0,0,1,0,1,0,1,0,1,
        1,1,1,0,1,0,1,3,1,1,1,1,1,2,1,4,1,0,1,0,1,
        1,2,0,0,0,0,0,0,5,0,0,0,0,0,0,0,1,0,0,0,1,
        1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1
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

Texture2D wallTextures[7];


Vector2 tile_to_pos(int tileX, int tileY)
{
	return { tileX * tile_size + tile_size / 2,
			 tileY * tile_size + tile_size / 2 };
}


int main() 
{
	const int screenWidth = 1280;
	const int screenHeight = 720;
	InitWindow(screenWidth, screenHeight, TITLE);
	SetTargetFPS(180);
	//DisableCursor();

	Image wizard = LoadImage("assets/wizard.png");
	Image wizard_interact = LoadImage("assets/wizard_interact.png");
	bool isInteracting = false;

	// Load textures (hardcoded cuz we don't have THAT many)
	wallTextures[0] = LoadTexture("assets/tiles/rock.png");
	wallTextures[1] = LoadTexture("assets/tiles/bars.png");
	wallTextures[2] = LoadTexture("assets/tiles/bars_hand.png");
	wallTextures[3] = LoadTexture("assets/tiles/bars_eyes.png");
	wallTextures[4] = LoadTexture("assets/tiles/door.png");
	wallTextures[5] = LoadTexture("assets/tiles/painting.png");
	wallTextures[6] = LoadTexture("assets/tiles/painting_laky.png");



	Player plr({300, 300}, 200, 60);
	plr.angle = -90;
		plr.deltaPos.x = cos(degToRad(plr.angle));
		plr.deltaPos.y = -sin(degToRad(plr.angle));

	int startTileX = 19;
	int startTileY = 13;
	plr.position = { startTileX * tile_size + tile_size / 2,
					startTileY * tile_size + tile_size / 2 };

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

			if(map_walls[interactY_add * map_width + interactX_add] == 5)
			{
				// If there's a door and we interact with it, open it
				map_walls[interactY_add * map_width + interactX_add] = 0;
			}

			isInteracting = true;

		}


		BeginDrawing();
		ClearBackground({76, 76, 76, 255});
		DrawText((string(TITLE) + " " + string(VERSION_NUM)).c_str(), 720, 20, 20, WHITE);

		#ifdef DEBUG
			drawMap2D();
		#endif
		raycast(plr);
		#ifdef DEBUG
			drawPlayer(plr, 8, YELLOW);
		#endif

		if(isInteracting)
		{
			DrawTexturePro(LoadTextureFromImage(wizard_interact), {0,0,128,128}, {screenWidth - 500, screenHeight - 500, 500, 500}, {0,0}, 0, WHITE);

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
			DrawTexturePro(LoadTextureFromImage(wizard), {0,0,128,128}, {screenWidth - 500, screenHeight - 500, 500, 500}, {0,0}, 0, WHITE);
		}

		

		EndDrawing();
	}
	CloseWindow();

	for(int i = 0; i < 5; i++)
	{
		UnloadTexture(wallTextures[i]);
	}

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

	float hitXpos = 0; // Texture coordinate

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

			hitXpos = fmod(rayY, tile_size); // Get texture coords
		}
		else
		{
			rayX = horizX;
			rayY = horizY;
			distance = distHoriz;

			vert_map_texture = horiz_map_texture;

			shade = 178;
			hitVertical = false;

			hitXpos = fmod(rayX, tile_size); // Get texture coords
		}

		// Draw the  hit with green :]
		#ifdef DEBUG
			DrawLineEx(plr.position, {rayX, rayY}, 2, GREEN);
		#endif


		// Fix Fisheye
		float ca_deg = plr.angle - rayAngle;            // difference in degrees
		if(ca_deg < -180.0f) ca_deg += 360.0f;
		if(ca_deg >  180.0f) ca_deg -= 360.0f;

		float ca = degToRad(ca_deg);
		distance = distance * cos(ca);




		float projectionPlaneDistance = (GetScreenWidth() / 2) / tan(degToRad(plr.fov / 2));
		float line_height = (tile_size * projectionPlaneDistance) / distance;
		
		// Center our beautiful lines on screen
		float line_offset = (GetScreenHeight() / 2) - line_height / 2;

		float texX = (hitXpos / tile_size) * 48; // Get texture coloumn to draw (the texture is 48x48)
		
		// Handle texture clipping when line extends beyond screen
		float texY = 0;
		float texHeight = 48;
		float drawHeight = line_height;
		float drawOffset = line_offset;
		
		if(line_height > GetScreenHeight())
		{
			// Calculate which part of the texture to show
			float ratio = GetScreenHeight() / line_height;
			texHeight = 48 * ratio;
			texY = (48 - texHeight) / 2; // Center the visible portion
			drawHeight = GetScreenHeight();
			drawOffset = 0;
		}

		int texIndex = horiz_map_texture;
		if(texIndex < 0) texIndex = 0;
		if(texIndex > wallTextures->width - 1) texIndex = wallTextures->width - 1;

		// Draw wall slice
		Rectangle source = {texX, texY, 1, texHeight};
		Rectangle dest = {ray * resolution, drawOffset, resolution, drawHeight};
		Color tint = hitVertical ? Color{230, 230, 230, 255} : Color{178, 178, 178, 255};
		DrawTexturePro(wallTextures[texIndex], source, dest, {0, 0}, 0, tint);
		
		
		rayAngle -= ray_angle_increment;
		rayAngle = FixAng(rayAngle);
	}

}
