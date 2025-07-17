#define DEBUG_OUTPUT 1
#define MAX_BULLETS 100
#define MAX_ENEMIES 10

unsigned int playerTexture;
unsigned int bulletTexture;
unsigned int enemyTexture;
unsigned int backgroundTexture;

typedef struct {
	float x;
	float y;
	float speed;
}Player;

typedef struct {
	float x;
	float y;
	float speed;
	int active; //1  if bullet is active, 0 if not
}Bullet;

typedef struct {
	float x;
	float y;
	float speed;
	int alive;
}Enemy;