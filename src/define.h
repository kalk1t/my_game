#define DEBUG_OUTPUT 1
#define MAX_BULLETS 100



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