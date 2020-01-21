#include<stdio.h>
#include<stdlib.h>
#include<math.h>
#define tall 20
#define width 20
void spread(int**, int**, int, int);
void initBoard(int**, int**, int, int);
void evolve(int**, int**, int, int);
void clear(int **, int, int);
int main() {

	int **marked; int* mStorage;
	int **score; int* sStorage;
	mStorage = malloc(tall * width * sizeof(int));
	marked = malloc(tall * sizeof(int *));
	for (int i = 0; i < tall; i++) {
		marked[i] = malloc(width * sizeof(int));
		for (int j = 0; j < width; j++)
			marked[i][j] = 0;
	}
	sStorage = malloc(tall * width * sizeof(int));
	score = malloc(tall * sizeof(int *));
	for (int i = 0; i < tall; i++) {
		score[i] = malloc(width * sizeof(int));
		for (int j = 0; j < width; j++)
			score[i][j] = 0;
	}
	initBoard(marked, score, tall, width);
	for (; ; ) {
		spread(marked, score, tall, width);
		evolve(marked, score, tall, width);

		{
			char control;
			scanf_s("%c", &control);
			if (control == 'o')
				return 0;
		}
	}



}
void evolve(int** marked, int** score, int rSize, int cSize) {
	for (int i = 0; i < rSize; i++) {
		for (int j = 0; j < cSize; j++) {
			if (score[i][j] == 3)
				marked[i][j] = 1;
			else if (score[i][j] > 3 || score[i][j] < 2)
				marked[i][j] = 0;
			score[i][j] = 0;
			printf("%d", marked[i][j]);
		}
		printf("\n");
	}
};
void spread(int** marked, int** score, int rSize, int cSize) {
	for (int i = 0; i < rSize; i++)
		for (int j = 0; j < cSize; j++) {
			if (marked[i][j]) {
				if (i > 0) {
					score[i - 1][j]++;
					if (j > 0) {
						score[i - 1][j - 1]++;
					}
					if (j < cSize - 1) {
						score[i - 1][j + 1]++;
					}
				}
				if (i < rSize - 1) {
					score[i + 1][j]++;
					if (j > 0)
						score[i + 1][j - 1]++;
					if (j < cSize - 1)
						score[i + 1][j + 1]++;
				}
				if (j > 0)
					score[i][j - 1]++;
				if (j < rSize - 1)
					score[i][j + 1]++;
			}
		}


}
void initBoard(int** marked, int** score, int rSize, int cSize) {
	for (int i = 0; i < rSize; i++) {
		printf("¿é¤J²Ä %d ¦C : ", i);
		for (int j = 0; j < cSize; j++) {
			scanf_s("%d", marked[i] + j);
			score[i][j] = 0;
		}
	}
}
void clear(int ** score, int rSize, int cSize) {
	for (int i = 0; i < rSize; i++)
		for (int j = 0; j < cSize; j++)
			score[i][j] = 0;


};