#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <sys/wait.h>
#include "mymemlib.h"

int processTest() {
	int* v = mymalloc(sizeof(int) * 3);
	v[0] = 1;
	v[1] = 2;
	v[2] = 3;
	pid_t pid = fork();
	if (pid < 0) {
		printf("Fork error!\n");
		return errno;
	}
	else if (pid == 0) {
		v[2] = 100;
		printf("COPIL:\n");
		for (int i = 0; i < 3; i++) {
			printf("%d ", v[i]);
		}
		printf("\n");
		exit(0);
	}
	else {
		wait(NULL);
		printf("PARINTE:\n");
		for (int i = 0; i < 3; i++) {
			printf("%d ", v[i]);
		}
		printf("\n");
		v[2] = 50;
	}
	printf("PARINTE:\n");
	for (int i = 0; i < 3; i++) {
		printf("%d ", v[i]);
	}
	printf("\n");
	myfree(v);
	return 0;
}

int standardTest() {
	int* v = mymalloc(100 * sizeof(int));
	for (int i = 0; i < 100; i++) {
		v[i] = i;
	}
	for (int i = 0; i < 50; i += 1) {
		printf("%d | ", v[i]);
	}
	printf("\n\n");
	int* v_rall = myrealloc(v, 50);
	for (int i = 0; i < 50; i += 1) {
		printf("%d | ", v_rall[i]);
	}
	printf("\n\n");
	myfree(v);
	int* v2 = mycalloc(10, sizeof(int));
	v2[5] = 99;
	for (int i = 0; i < 10; i++) {
		printf("%d | ", v2[i]);
	}
	printf("\n\n");
	myfree(v2);
	return 0;
}

int main() {
	processTest();
	printf("\n");
	standardTest();
	return 0;
}
