//решение задачи о 8 ферзях (look Vikipedia)
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <corecrt_math.h>
#define MAX_LEN 30  // size of board

typedef int solutionType[MAX_LEN]; // lots of solutions

typedef struct {
    solutionType solution;  // one solution
    float energy;  // value enesgy system in this solution
} memberType;

#define INITIAL_TEMPERATURE 30.0  // begin border
#define FINAL_TEMPERATURE 0.5  // end
#define ALPHA 0.98  // size step
#define STEPS_PER_CHANGE 100  // count steps

void tweakSolution(memberType *member){
    int temp, x, y;
    x = rand() % (MAX_LEN);
    do{
        y= rand() % (MAX_LEN);
    } while(x==y);

    temp = member->solution[x];
    member->solution[x]=member->solution[y];
    member->solution[y]=temp;
}

void initializeSolution(memberType*member){
    int i;
    for(i=0;i<MAX_LEN;i++)
        member->solution[i]=i;

    for(i=0;i<MAX_LEN;i++)
        tweakSolution(member);
}

// Evaluation of the decision
void computeEnergy(memberType*member){
    int i, j, x, y, tempx, tempy;
    char board[MAX_LEN][MAX_LEN];
    int confl = 0;
    const int dx[4]={-1,1,-1,1};
    const int dy[4]={-1,1,1,-1};

	memset((void*)board, 0, MAX_LEN * MAX_LEN);

    for(i=0;i<MAX_LEN;i++)
        board[i][member->solution[i]]='Q';

	for (i = 0; i < MAX_LEN; i++) {
		x = i;
		y = member->solution[i];
		for (j = 0; j < 4; j++) {
			tempx = x;
			tempy = y;
			while (true) {
				tempx += dx[i];
				tempy += dy[i];
				if ((tempx < 0) || (tempx > MAX_LEN) || (tempy < 0) || (tempy > MAX_LEN))
					break;
				if (board[tempx][tempy] = 'Q')
					confl++;
			}
		}
	}
	member->energy = (float)confl;
}

void copySolution(memberType* dest, memberType* src) {
	int i;

	for (i = 0; i < MAX_LEN; i++) 
		dest->solution[i] = src->solution[i];

	dest->energy = src->energy;
}

void emitSolution(memberType* member) {
	char board[MAX_LEN][MAX_LEN];
	int x, y;

	memset((void*)board, 0, MAX_LEN * MAX_LEN);

	for (x = 0; x < MAX_LEN; x++)
		board[x][member->solution[x]] = 'Q';

	printf("broad:  ");
	for (y = 0; y < MAX_LEN; y++) {
		for (x = 0; x < MAX_LEN; x++){
			if (board[x][y] == 'Q')
				printf("Q");
			else
				printf(". ");
		}
		printf("\n");
	}
	printf("\n\n");
}

int main() {
	int timer = 0, step, solution = 0, useNew, accepted; 
	float temperature = INITIAL_TEMPERATURE; 
	memberType current, working, best;
	FILE* fp;
	errno_t fplast;

	fplast = fopen_s(&fp,"stats.txt", "w");

	srand(time(NULL));

	initializeSolution(&current);
	computeEnergy(&current);
	best.energy = 100.0;

	copySolution(&working, &current);

	while (temperature > FINAL_TEMPERATURE) {
		printf("temperature: %f\n", temperature);
		accepted = 0;
		
		for (step = 0; step < STEPS_PER_CHANGE; step++) {
			useNew = 0;
			tweakSolution(&working);
			computeEnergy(&working);

			if (working.energy <= current.energy) {
				useNew = 1;
			}
			else {
				float test = rand() % 100;
				float delta = working.energy - current.energy;
				float calc = exp(-delta / temperature);

				if (calc > test) {
					accepted++;
					useNew = 1;
				}
			}
			if (useNew) {
				useNew = 0;
				copySolution(&current, &working);

				if (current.energy < best.energy) {
					copySolution(&best, &current);
					solution = 1;
				}
			}
			else {
				copySolution(&working, &current);
			}
		}
		fprintf(fp, "%d %f %f %d\n", timer++, temperature, best.energy, accepted);
		printf("Best energy = %f\n", best.energy);
		temperature *= ALPHA;
	}
	fclose(fp);
	if (solution) {
		emitSolution(&best);
	}
	return 0;
}