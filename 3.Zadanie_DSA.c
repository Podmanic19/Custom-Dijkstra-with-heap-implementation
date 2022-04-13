#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>

typedef struct cell{
	int x;
	int y;
	int distance;
	char type;
	char visited;
	struct cell* p_prev;
}CELL,*HEAP;									

typedef struct map {
	int width;
	int height;
	char princess;
	CELL** map;
}MAP;

typedef struct path {
	int length;
	int distance;
	CELL* path;
}PATH;

void printmap(MAP map);													//prints the given map
void permutate(int* p, int start, int end, int** options, int* iter);	//permutates the numbers pressent int arr
void swappostitions(int* a, int* b);									//swaps the two numbers 

char** generaterand(int* n, int* m, int* t);							//generates a random map

int getcost(CELL given);												//returns the time cost of moving into the given cell
int exists(MAP map, int x, int y);										//determines whether a node with given indexes exists
int getheapsize(HEAP heap);												//returns the number of elements currently in heap
int findsmaller(HEAP heap, int index);									//finds the index of the smaller of child of given element
int listlength(CELL goal, CELL start);									//finds the number of elements in linked list
int paths_num(int importants);											//returns how many paths will exist based on the number of princesses
int factorial(int number);												//calculates the factorial of given number
int madeit(PATH path,int t);											//returns 0 if the path to dragon is shorter or equal to the given time
int paths_num(int importants);											//returns the number of paths that have to be generated

int* findshortest(int** options, PATH** paths, int p);					//finds the shortest combination of indexes
int* pathtoarr(PATH result);											//converts a path to an array of xy indexes

int** getcombinations(int p, MAP map);									//gets the combinations of princess indexes

CELL findparent(HEAP heap, int index);
CELL findright(HEAP heap, int index);
CELL findleft(HEAP heap, int index);									//finds the position of given relatives to node
CELL relax(CELL* current, CELL* target,int* flag);						//tries to shorten the path to the given cell
CELL lookup(MAP map, CELL current,int* flag);							//returns the cell above the current, if the cell had alredy been visited, flag =1
CELL lookright(MAP map, CELL current,int* flag);						//returns the cell right of the current, if the cell had alredy been visited, flag =1
CELL lookdown(MAP map, CELL current,int* flag);							//returns the cell below the current, if the cell had alredy been visited, flag =1
CELL lookleft(MAP map, CELL current, int* flag);						//returns the cell left of the current,if the cell had alredy been visited, flag =1
CELL dragon(MAP map);													//returns the cell with dragon on it
CELL princess(MAP* map);												//returns the cell with princess on it
		
CELL* getimportants(MAP map);											//returns an array of imporant cells (dragon + all princessess)

PATH** getpaths(MAP map);												//generates paths for permutations
PATH listtopath(CELL goal, CELL start);									//finds the number of elements in linked list
PATH getcomplete(MAP map, PATH todrake);								//returns the final path
PATH reverse(PATH path);												//reverses the path		
PATH dijkstra(MAP* map, CELL start, CELL goal);							//dijkstra returns the shortest path from start to goal
PATH merge(PATH first, PATH second);									//merges the two given paths
PATH unitepaths(int* seq, PATH** paths, PATH drake, int p);				//manages merging

HEAP allocheap(MAP map);												//allocates the space needed for a 1D array of CELL-s
HEAP insertheap(HEAP heap,CELL cell);									//inserts the given cell into heap
HEAP swapup(HEAP heap,int position);									//swaps the element at given position with its parent
HEAP swapdown(HEAP heap, int position);									//swaps the element at given position with its smaller child	
HEAP poptop(HEAP heap, CELL* cell);										//rewrites the cell argument to the current heap root and adjusts the heap	
			
MAP allocmap(int n,int m);												//allocates the space needed for a 2D array of CELL-s
MAP setmap(char** mapa, int x, int y);									//converts the map to map of CELL-s
MAP setvisited(MAP map, CELL cell);										//sets the given cell of given map as visited
MAP copymap(MAP map);													//creates a copy of given map


int exists(MAP map, int x, int y) {
	if ((x >= 0 && x < map.width) && (y >= 0 && y < map.height)) {
		if (map.map[y][x].type != 'N')													//ignore rocks
			return 1;
	}
	return 0;
}

CELL relax(CELL* current,CELL* target,int *flag) {
	if ((*target).distance < 0) {														//if this is the first time the cell is being relaxed
		(*target).distance = 0;															//set the distance to 0
		(*target).distance = (*current).distance + getcost((*target));					//get the actual distance
		(*target).p_prev = current;
		*flag = 1;
		return (*target);
	}
	if ((*target).distance > ((*current).distance + getcost((*target)))) {				//if the cell had been relaxed before but can be again
		(*target).distance = (*current).distance + getcost((*target));
		(*target).p_prev = current;
		*flag = 1;
		return (*target);
	}
	*flag = 0;																			//cell had not been changed, no need to add to heap
	return (*target);
}

CELL findparent(HEAP heap,int index) {
	return heap[index / 2];
}

CELL findright(HEAP heap, int index) {
	return heap[index * 2 + 1];
}

CELL findleft(HEAP heap, int index) {
	return heap[index * 2];
}

CELL cellinit(int x, int y, int type) {
	CELL cell;

	cell.x = x;
	cell.y = y;
	cell.type = type;
	cell.distance = -1;
	cell.visited = 0;
	cell.p_prev = NULL;

	return cell;
}

CELL dragon(MAP map) {
	for (int i = 0; i < map.height; i++) {
		for (int j = 0; j < map.width; j++) {
			if (map.map[i][j].type == 'D') {
				return map.map[i][j];
			}
		}
	}
}

MAP allocmap(int x,int y) {
	MAP map;
	map.height = y;
	map.width = x;
	map.princess = 0;

	map.map = (CELL**)malloc(y * sizeof(CELL*));
	for (int i = 0; i < y; i++) {
		map.map[i] = (CELL*)malloc(x * sizeof(CELL));
	}
	return map;
}

MAP setmap(char** mapa, int x, int y) {
	MAP map;
	int i, j;

	map = allocmap(x, y);
	for (i = 0; i < y; i++) {
		for (j = 0; j < x; j++) {
			(map.map[i][j]) = cellinit(j, i, mapa[i][j]);
			if (mapa[i][j] == 'P') map.princess++;							//saves the number of princessess
		}
	}
	map.map[0][0].distance = 0;
	return map;
}

MAP copymap(MAP map) {
	MAP copy;
	int i, j;

	copy = allocmap(map.width, map.height);
	for (i = 0; i < map.height; i++) {
		for (j = 0; j < map.width; j++) {
			(copy.map[i][j]) = cellinit(j, i, map.map[i][j].type);
			if (map.map[i][j].type == 'P') copy.princess++;							//saves the number of princessess
		}
	}
	return copy;
}

MAP setvisited(MAP map, CELL current) {
	if (exists(map, current.x, current.y)) {										//something went wrong here
		map.map[current.y][current.x].visited = 1;
	}
	return map;
}

HEAP allocheap(MAP map) {
	HEAP heap;

	heap = (HEAP)malloc(map.width*map.height*sizeof(CELL));		//elemets of heap will be indexed from 1
	heap[0].x = 0;												//0.th x - index will store the number of elements currently in heap
	heap[0].y = 0;
	return heap;
}

int getheapsize(HEAP heap) {
	return heap[0].x;
}

int getcost(CELL given) {
	if (given.type == 'N') {
		return -1;
	}
	else if (given.type == 'H') {
		return 2;
	}
	else return 1;
}

void printmap(MAP map) {
	for (int i = 0; i < map.height; i++) {
		for (int j = 0; j < map.width; j++) {
			printf("%c", ((map.map[i][j]).type));
		}
		putchar('\n');
	}
}

int paths_num(int importants) {
	int i=0;
	while (importants > 0) {
		i += importants;
		importants--;
	}
	return i;
}

CELL lookup(MAP map, CELL current, int* flag) {
	CELL target;
	if (exists(map, current.x, current.y - 1)) {
		return relax(&(map.map[current.y][current.x]), &(map.map[current.y-1][current.x]),flag);
	}
	target.distance = -1;
	return target;
}

CELL lookright(MAP map, CELL current, int* flag) {
	CELL target;
	if (exists(map, current.x+1,current.y)) {
		return relax(&(map.map[current.y][current.x]), &(map.map[current.y][current.x+1]), flag);
	}
	target.distance = -1;
	return target;
}

CELL lookdown(MAP map, CELL current, int* flag) {
	CELL target;
	if (exists(map,current.x,current.y+1)) {
		return relax(&(map.map[current.y][current.x]), &(map.map[current.y+1][current.x]), flag);
	}
	target.distance = -1;
	return target;
}

CELL lookleft(MAP map, CELL current,int* flag) {
	CELL target;
	if (exists(map,current.x-1,current.y)) {
		return relax(&(map.map[current.y][current.x]), &(map.map[current.y][current.x-1]), flag);
	}
	target.distance = -1;
	return target;
}

int listlength(CELL start, CELL goal) {
	CELL* current;
	int i=1;

	current = &goal;
	while (1) {
		if ((*current).x == start.x) {
			if ((*current).y == start.y) {
				break;
			}
		}
		i++;
		current = current->p_prev;
	}
	return i;
}

PATH listtopath(CELL goal, CELL start) {
	PATH path;
	CELL* current;
	path.length = listlength(start, goal);

	current = &goal;
	path.path = (CELL*)malloc((path.length) * sizeof(CELL));
	for (int i = path.length-1; i >= 0; i--) {
		path.path[i] = *(current);
		current = current->p_prev;
	}
	path.distance = path.path[path.length - 1].distance + 1;
	return path;
}

PATH dijkstra(MAP* map,CELL start,CELL goal) {
	CELL current = start;
	PATH path;
	CELL surrounding[4];
	int flag[4];
	HEAP heap = allocheap(*map);

	if (current.distance < 0) {											//if the start is the actual start of map
		current.distance = 0;
	}
	heap = insertheap(heap, current);
	while (getheapsize(heap) >= 0) {
		heap = poptop(heap, &current);
		*map = setvisited(*map, current);
		surrounding[0] = lookup(*map, current,&flag[0]);
		surrounding[1] = lookright(*map, current,&flag[1]);
		surrounding[2] = lookdown(*map, current,&flag[2]);
		surrounding[3] = lookleft(*map, current,&flag[3]);
		for (int i = 0; i < 4; i++) {
			if (surrounding[i].visited == 0 && exists(*map,surrounding[i].x,surrounding[i].y)) {			//this might not be entirely correct]
				if (flag[i] == 1) {
					heap = insertheap(heap, surrounding[i]);												//if the node is accessible and exists add it to heap
				}
			}
		}
		if ((current.x == goal.x) && (current.y == goal.y)) {
			path = listtopath(current, start);
			return path;
		}
	}
	path.length = 0;
	return path;
}

HEAP swapup(HEAP heap, int position) {
	CELL temp = findparent(heap,position);
	
	if (heap[position].distance < findparent(heap, position).distance) {
		heap[position / 2] = heap[position];
		heap[position] = temp;
		heap = swapup(heap, position);
	}
	return heap;
}

HEAP swapdown(HEAP heap, int position) {
	int childpos = findsmaller(heap, position);
	CELL temp = heap[childpos];							//the smaller of children of the given element

	if (heap[position].distance > temp.distance) {
		heap[childpos] = heap[position];
		heap[position] = temp;
		heap = swapdown(heap, childpos);
	}

	return heap;
}

int findsmaller(HEAP heap, int index) {
	if (index*2 < heap[0].x && index * 2 + 1 < heap[0].x) {
		return (findleft(heap, index).distance < findright(heap, index).distance) ? (index * 2) : (index * 2 + 1);
	}
	if (index * 2 < heap[0].x) {
		return  (index * 2);
	}
	return index;
}

HEAP insertheap(HEAP heap, CELL cell) {
	int position = getheapsize(heap) + 1;

	heap[position] = cell;
	heap = swapup(heap, position);								
	heap[0].x++;											//increase the amount of elements in heap
	return heap;
}

HEAP poptop(HEAP heap, CELL* cell) {
	*cell = heap[1];										//returns the root cell through a pointer
	heap[1] = heap[heap[0].x];								//moves the last element to the first position
	swapdown(heap, 1);
	heap[0].x--;
	return heap;
}

CELL princess(MAP* map) {
	for (int i = 0; i < (*map).height; i++) {
		for (int j = 0; j < (*map).width; j++) {
			if ((*map).map[i][j].type == 'P') {
				(*map).map[i][j].type = 'C';
				return (*map).map[i][j];
			}
		}
	}
}

CELL* getprincessess(MAP map) {
	CELL* importants;

	importants = (CELL*)malloc((map.princess) * sizeof(CELL));								//array of important cells
	for (int i = 0; i < map.princess; i++) {
		importants[i] = princess(&map);
	}
	return importants;
}

CELL* getimportants(MAP map) {
	CELL* importants;

	importants = (CELL*)malloc((map.princess + 1) * sizeof(CELL));								//array of important cells
	importants[0] = dragon(map);
	for (int i = 1; i <= map.princess; i++) {
		importants[i] = princess(&map);
	}
	return importants;
}

PATH reverse(PATH path) {								
	PATH reverse;
	int counter = path.length-1;

	reverse.path = (CELL*)malloc(path.length * sizeof(CELL));
	for (int i = 0;i < path.length; i++) {
		reverse.path[i] = path.path[counter--];
	}
	reverse.distance = path.distance;
	reverse.length = path.length;
	return reverse;
}

int factorial(int number) {
	int sum = 1;
	while (number) {
		sum *= number;
		number--;
	}
	return sum;
}

PATH** getpaths(MAP map) {
	MAP copy = copymap(map);
	CELL* importants;
	PATH** paths;
	int k = 0;

	importants = getimportants(copy);
	paths = (PATH**)malloc(paths_num(copy.princess) * sizeof(PATH*));							//array of paths
	for (int i = 0; i < copy.princess; i++) {
		paths[i] = (PATH*)malloc(paths_num(copy.princess) * sizeof(PATH));
	}
	for (int i = 0; i < copy.princess; i++) {
		for (int j = i; j < copy.princess; j++) {
			paths[i][j] = dijkstra(&copy, importants[i], importants[j+1]);
			copy = copymap(map);
		}
	}
	return paths;
}

void swappostitions(int* a, int* b)
{
	int temp;
	temp = *a;
	*a = *b;
	*b = temp;
}

void permutate(int* p, int start, int end,int** options,int* iter)
{
	if (start == end)
	{
		for (int i = 0; i <= end; i++) {
			options[*iter][i] = p[i];
		}
		(*iter)++;
		return;
	}
	for (int i = start; i <= end; i++)
	{
		swappostitions((p + i), (p + start));
		permutate(p, start + 1, end,options,iter);
		swappostitions((p + i), (p + start));
	}
}

int** getcombinations(int p, MAP map) {
	int permutations = factorial(p);
	int iter = 0;
	int* princess;
	int** options;

	princess = (int*)malloc(p * sizeof(int));
	options = (int**)malloc(permutations * sizeof(int*));
	for (int i = 0; i < permutations; i++) {
		options[i] = (int*)malloc(p * sizeof(int));
	}
	for (int i = 0; i < p; i++) {
		princess[i] = i + 1;
	}
	permutate(princess, 0, p - 1,options,&iter);
	return options;
}

int* findshortest(int** options,PATH** paths,int p) {
	int min = 0;
	int current=0;
	int permutations = factorial(p);
	int* seq = (int*)malloc(p*sizeof(int));

	for (int i = 0; i < permutations; i++) {
		current += paths[0][options[i][0]-1].distance;
		if (current > min && min != 0) {													//if you alredy know its gonna be longer
			continue;
		}
		for (int j = 0; j < p-1; j++) {
			int a = options[i][j];
			int b = options[i][j + 1];
			if (a > b) {
				swappostitions(&a, &b);
			}
			current += paths[a][b-1].distance;
			if (current > min && min != 0) {												//if you alredy know its gonna be longer
				break;
			}
		}
		if (min == 0 || min > current) {
			min = current;
			for (int k = 0; k < p; k++) {
				seq[k] = options[i][k];
			}
		}
		current = 0;
	}
	return seq;
}

PATH merge(PATH first, PATH second) {
	PATH merged;
	int k = 0;

	merged.path = (CELL*)malloc((first.length + second.length - 1) * sizeof(CELL));
	merged.length = first.length + second.length - 1;

	if (first.length == 0 || second.length == 0) {											//if one of the paths doesnt exist
		merged.length = 0;
		return merged;
	}
	if ((first.path[first.length - 1].x == second.path[0].x) && (first.path[first.length - 1].y == second.path[0].y)) {
		for (int i = 0; i < first.length; i++) {
			merged.path[k++] = first.path[i];
		}
		for (int i = 1; i < second.length; i++) {
			merged.path[k++] = second.path[i];
		}
	}
	else{
		second = reverse(second);
		merged = merge(first, second);
	}
	return merged;
}

PATH unitepaths(int* seq, PATH** paths, PATH drake,int p) {
	PATH united = merge(drake,paths[0][seq[0]-1]);

	for (int i = 0; i < p-1; i++) {
		int a = seq[i];
		int b = seq[i+1];
		if (a > b) {
			swappostitions(&a, &b);
		}
		united = merge(united, paths[a][b - 1]);
	}
	return united;
}

PATH getcomplete(MAP map,PATH todrake) {
	PATH** paths,final;
	MAP copy = copymap(map);
	int** combinations;
	int* shortest;

	paths = getpaths(map);
	combinations = getcombinations(map.princess,copy);
	shortest = findshortest(combinations, paths, map.princess);
	final = unitepaths(shortest,paths,todrake,map.princess);
	return final;
}

int* pathtoarr(PATH result) {
	int* arr;
	int k=0;

	arr = (int*)malloc(2 * (result.length) * sizeof(int));
	for (int i = 0; i < result.length*2; i++) {
		if (i % 2 == 0) {
			arr[i] = result.path[k].x;
		}
		else {
			arr[i] = result.path[k].y;
			k++;
		}
	}
	return arr;
}

int madeit(PATH path, int t) {
	int sum=0;

	for (int i = 0; i < path.length; i++) {
		sum += getcost(path.path[i]);
	}
	return (sum <= t);
}

int* zachran_princezne(char** mapa, int n, int m, int t, int* dlzka_cesty) {
	int x = m;
	int y = n;				//n and m is just way too confusing
	MAP map;
	PATH todrake, tocollect;

	if (mapa[0][0] == 'N') {
		*dlzka_cesty = 0;
		printf("Invalid map.\n");
		return NULL;
	}
	map = setmap(mapa, x, y);
	printmap(map);
	todrake = dijkstra(&map, map.map[0][0], dragon(map));
	if (todrake.length == 0) {
		printf("Path to dragon does not exist.\n");
		return NULL;
	}
	if (madeit(todrake, t) == 0) {
		printf("The path to dragon is too long, Princesses were not saved in time.\n");
		return NULL;
	}
	free(map.map);
	map = setmap(mapa, x, y);
	tocollect = getcomplete(map,todrake);
	if (tocollect.length == 0) {								//if the path doesnt exist its length is 0
		printf("Path to one or more of the princessess does not exist.\n");
		return NULL;
	}
	*(dlzka_cesty) = tocollect.length;
	return pathtoarr(tocollect);
}

char** generaterand(int *n,int *m,int *t) {
	int num,seed,princessess;
	char** mapa;

	printf("PLease select the height of the map.\n");
	scanf("%d",n);
	printf("Please select the width of the map.\n");
	scanf("%d",m);
	printf("Please enter a seed.\n");
	scanf("%d", &seed);
	printf("Please enter the number of abducted princesses, awaiting their saviour and fearing for their lives,lost in\nthe wilderness and dreading the time when the mighty dragon awakens from his slumber.\n");
	scanf("%d", &princessess);
	printf("Please enter the time when the dragon awakens from his long slumber,killing and devouring the princesses,\nmoving on to terrorize other kingdoms and spreading doom wherever the shadow of his mighty wings spreads.\n");
	scanf("%d", t);
	srand(seed);
	mapa = (char**)malloc((*n) * sizeof(char*));
	for (int i = 0; i < (*n); i++) {
		mapa[i] = (char*)malloc((*m) * sizeof(char));
	}
	for (int i = 0; i < (*n); i++) {
		for (int j = 0; j < (*m); j++) {
		    num = rand();
			if (num % 5 == 0) {
				mapa[i][j] = 'N';
				continue;
			}
			if (num % 2 == 1) {
				mapa[i][j] = 'H';
				continue;
			}
			if (num % 2 == 0) {
				mapa[i][j] = 'C';
				continue;
			}
		}
	}
	if (princessess <= 0) {
		princessess = 1;
	}
	for (int i = 0; i < princessess; i++) {
		mapa[rand() % (*n)][rand() % (*m)] = 'P';
	}
	mapa[rand() % (*n)][rand() % (*m)] = 'D';
	if (rand() % 2) {
		mapa[0][0] = 'C';
	}
	else {
		mapa[0][0] = 'H';
	}
	return mapa;
}

int main()
{
	char** mapa;
	int i, test, dlzka_cesty, cas, * cesta,flag = 0;
	int n = 0, m = 0, t = 0;
	FILE* f;
	while (1) {
		printf("Zadajte cislo testu (0 ukonci program):\n");
		scanf("%d", &test);
		dlzka_cesty = 0;
		n = m = t = 0;
		switch (test) {
		case 0://ukonci program
			return 0;
		case 1://nacitanie mapy zo suboru
			f = fopen("test1.txt", "r");
			if (f)
				fscanf(f, "%d %d %d", &n, &m, &t);
			else
				continue;
			mapa = (char**)malloc(n * sizeof(char*));
			for (i = 0; i < n; i++) {
				mapa[i] = (char*)malloc(m * sizeof(char));
				for (int j = 0; j < m; j++) {
					char policko = fgetc(f);
					if (policko == '\n') policko = fgetc(f);
					mapa[i][j] = policko;
				}
			}
			fclose(f);
			cesta = zachran_princezne(mapa, n, m, t, &dlzka_cesty);
			break;
		case 2://nacitanie preddefinovanej mapy
			n = 10;
			m = 10;
			t = 12;
			mapa = (char**)malloc(n * sizeof(char*));
			mapa[0] = _strdup("CCHCNHCCHN");
			mapa[1] = _strdup("NNCCCHHCCC");
			mapa[2] = _strdup("DNCCNNHHHC");
			mapa[3] = _strdup("CHHHCCCCCC");
			mapa[4] = _strdup("CCCCCNHHHH");
			mapa[5] = _strdup("PCHCCCNNNN");
			mapa[6] = _strdup("NNNNNHCCCC");
			mapa[7] = _strdup("CCCCCPCCCC");
			mapa[8] = _strdup("CCCNNHHHHH");
			mapa[9] = _strdup("HHHPCCCCCC");
			cesta = zachran_princezne(mapa, n, m, t, &dlzka_cesty);
			break;
		case 3: //pridajte vlastne testovacie vzorky
			mapa = generaterand(&n,&m,&t);
			cesta = zachran_princezne(mapa, n, m, t, &dlzka_cesty);
			break;
		default:
			continue;
		}
		cas = 0;
		for (i = 0; i < dlzka_cesty; i++) {
			printf("%d %d\n", cesta[i * 2], cesta[i * 2 + 1]);
			if (mapa[cesta[i * 2 + 1]][cesta[i * 2]] == 'H')
				cas += 2;
			else
				cas += 1;
			if (mapa[cesta[i * 2 + 1]][cesta[i * 2]] == 'D' && cas <= t) {
				flag = 1;
			}
			if (mapa[cesta[i * 2 + 1]][cesta[i * 2]] == 'D' && cas > t && flag == 0)
				printf("Nestihol si zabit draka!\n");
			if (mapa[cesta[i * 2 + 1]][cesta[i * 2]] == 'N')
				printf("Prechod cez nepriechodnu prekazku!\n");
			if (i > 0 && abs(cesta[i * 2 + 1] - cesta[(i - 1) * 2 + 1]) + abs(cesta[i * 2] - cesta[(i - 1) * 2]) > 1)
				printf("Neplatny posun Popolvara!\n");
		}
		printf("%d\n", cas);
		free(cesta);
		for (i = 0; i < n; i++) {
			free(mapa[i]);
		}
		free(mapa);
	}
	return 0;
}