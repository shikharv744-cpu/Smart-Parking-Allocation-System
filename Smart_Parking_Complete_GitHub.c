
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>

#define MAX 20
#define INF 99999
#define MAX_CHILDREN 10

/* =========================================================
   SMART PARKING ALLOCATION AND RESERVATION PLATFORM
   Combined Unit 1 + Unit 2 + Unit 3
   ========================================================= */

typedef struct NaryNode {
    char name[30];
    struct NaryNode *child[MAX_CHILDREN];
    int childCount;
} NaryNode;

typedef struct AVLNode {
    int slotID;
    int height;
    struct AVLNode *left, *right;
} AVLNode;

typedef struct {
    int slotID;
    int distance;
} HeapNode;

HeapNode heap[MAX];
int heapSize = 0;

char zoneName[6] = {'G','N','C','E','S','W'};
int graph[6][6];
int edgeCount = 0;

typedef struct {
    int u, v, w;
} Edge;

Edge edges[MAX];

/* ---------- Utility ---------- */

int max(int a, int b) {
    return (a > b) ? a : b;
}

void initGraph() {
    int i, j;
    for (i = 0; i < 6; i++)
        for (j = 0; j < 6; j++)
            graph[i][j] = (i == j) ? 0 : INF;
    edgeCount = 0;
}

int zoneIndex(char z) {
    int i;
    for (i = 0; i < 6; i++)
        if (zoneName[i] == z)
            return i;
    return -1;
}

void printZones() {
    printf("\nZones: G(Entry), N, C, E, S, W\n");
}

/* =========================================================
   UNIT 1 : N-ARY TREE
   ========================================================= */

NaryNode *createNaryNode(const char *name) {
    NaryNode *node = (NaryNode *)malloc(sizeof(NaryNode));
    strcpy(node->name, name);
    node->childCount = 0;
    return node;
}

void addChild(NaryNode *parent, const char *name) {
    if (parent->childCount >= MAX_CHILDREN) {
        printf("Maximum children reached.\n");
        return;
    }
    parent->child[parent->childCount++] = createNaryNode(name);
}

void displayNary(NaryNode *root, int level) {
    int i;
    if (root == NULL) return;

    for (i = 0; i < level; i++)
        printf("  ");
    printf("|-- %s\n", root->name);

    for (i = 0; i < root->childCount; i++)
        displayNary(root->child[i], level + 1);
}

void freeNary(NaryNode *root) {
    int i;
    if (root == NULL) return;
    for (i = 0; i < root->childCount; i++)
        freeNary(root->child[i]);
    free(root);
}

void naryTreeModule() {
    NaryNode *city, *zone, *lot, *floorNode;
    int zones, lots, floors, slots;
    int i, j, k, s;
    char buffer[50];

    city = createNaryNode("City");

    printf("\nEnter number of parking zones: ");
    scanf("%d", &zones);

    if (zones > MAX_CHILDREN) zones = MAX_CHILDREN;

    for (i = 0; i < zones; i++) {
        printf("Enter Zone %d name: ", i + 1);
        scanf("%s", buffer);
        zone = createNaryNode(buffer);
        city->child[city->childCount++] = zone;

        printf("Enter number of parking lots in %s: ", buffer);
        scanf("%d", &lots);
        if (lots > MAX_CHILDREN) lots = MAX_CHILDREN;

        for (j = 0; j < lots; j++) {
            printf("Enter Lot %d name: ", j + 1);
            scanf("%s", buffer);
            lot = createNaryNode(buffer);
            zone->child[zone->childCount++] = lot;

            printf("Enter number of floors in %s: ", buffer);
            scanf("%d", &floors);
            if (floors > MAX_CHILDREN) floors = MAX_CHILDREN;

            for (k = 0; k < floors; k++) {
                sprintf(buffer, "Floor_%d", k + 1);
                floorNode = createNaryNode(buffer);
                lot->child[lot->childCount++] = floorNode;

                printf("Enter number of slots in Floor_%d: ", k + 1);
                scanf("%d", &slots);
                if (slots > MAX_CHILDREN) slots = MAX_CHILDREN;

                for (s = 0; s < slots; s++) {
                    printf("Enter slot ID: ");
                    scanf("%s", buffer);
                    addChild(floorNode, buffer);
                }
            }
        }
    }

    printf("\n--- PARKING HIERARCHY ---\n");
    displayNary(city, 0);

    freeNary(city);
}

/* =========================================================
   UNIT 1 : AVL TREE
   ========================================================= */

int height(AVLNode *node) {
    return node ? node->height : 0;
}

AVLNode *newAVLNode(int id) {
    AVLNode *node = (AVLNode *)malloc(sizeof(AVLNode));
    node->slotID = id;
    node->height = 1;
    node->left = node->right = NULL;
    return node;
}

AVLNode *rightRotate(AVLNode *y) {
    AVLNode *x = y->left;
    AVLNode *t = x->right;

    x->right = y;
    y->left = t;

    y->height = 1 + max(height(y->left), height(y->right));
    x->height = 1 + max(height(x->left), height(x->right));

    return x;
}

AVLNode *leftRotate(AVLNode *x) {
    AVLNode *y = x->right;
    AVLNode *t = y->left;

    y->left = x;
    x->right = t;

    x->height = 1 + max(height(x->left), height(x->right));
    y->height = 1 + max(height(y->left), height(y->right));

    return y;
}

int balanceFactor(AVLNode *node) {
    return node ? height(node->left) - height(node->right) : 0;
}

AVLNode *insertAVL(AVLNode *root, int id) {
    int balance;

    if (root == NULL)
        return newAVLNode(id);

    if (id < root->slotID)
        root->left = insertAVL(root->left, id);
    else if (id > root->slotID)
        root->right = insertAVL(root->right, id);
    else
        return root;

    root->height = 1 + max(height(root->left), height(root->right));
    balance = balanceFactor(root);

    if (balance > 1 && id < root->left->slotID)
        return rightRotate(root);

    if (balance < -1 && id > root->right->slotID)
        return leftRotate(root);

    if (balance > 1 && id > root->left->slotID) {
        root->left = leftRotate(root->left);
        return rightRotate(root);
    }

    if (balance < -1 && id < root->right->slotID) {
        root->right = rightRotate(root->right);
        return leftRotate(root);
    }

    return root;
}

AVLNode *searchAVL(AVLNode *root, int id) {
    if (root == NULL || root->slotID == id)
        return root;

    if (id < root->slotID)
        return searchAVL(root->left, id);

    return searchAVL(root->right, id);
}

void inorderAVL(AVLNode *root) {
    if (root == NULL) return;
    inorderAVL(root->left);
    printf("%d ", root->slotID);
    inorderAVL(root->right);
}

void freeAVL(AVLNode *root) {
    if (root == NULL) return;
    freeAVL(root->left);
    freeAVL(root->right);
    free(root);
}

void avlModule() {
    AVLNode *root = NULL;
    int n, id, searchID, i;

    printf("\nEnter number of parking slots: ");
    scanf("%d", &n);

    for (i = 0; i < n; i++) {
        printf("Enter numeric slot ID %d: ", i + 1);
        scanf("%d", &id);
        root = insertAVL(root, id);
    }

    printf("\nAVL Inorder Slot IDs: ");
    inorderAVL(root);

    printf("\nEnter slot ID to search: ");
    scanf("%d", &searchID);

    if (searchAVL(root, searchID))
        printf("Slot %d is available in AVL tree.\n", searchID);
    else
        printf("Slot %d not found.\n", searchID);

    freeAVL(root);
}

/* =========================================================
   UNIT 1 : MIN-HEAP
   ========================================================= */

void swapHeap(HeapNode *a, HeapNode *b) {
    HeapNode temp = *a;
    *a = *b;
    *b = temp;
}

void heapifyUp(int index) {
    int parent;
    while (index > 0) {
        parent = (index - 1) / 2;
        if (heap[parent].distance <= heap[index].distance)
            break;
        swapHeap(&heap[parent], &heap[index]);
        index = parent;
    }
}

void heapifyDown(int index) {
    int left, right, smallest;

    while (1) {
        left = 2 * index + 1;
        right = 2 * index + 2;
        smallest = index;

        if (left < heapSize &&
            heap[left].distance < heap[smallest].distance)
            smallest = left;

        if (right < heapSize &&
            heap[right].distance < heap[smallest].distance)
            smallest = right;

        if (smallest == index)
            break;

        swapHeap(&heap[index], &heap[smallest]);
        index = smallest;
    }
}

void insertHeap(int slotID, int distance) {
    if (heapSize >= MAX) {
        printf("Heap is full.\n");
        return;
    }

    heap[heapSize].slotID = slotID;
    heap[heapSize].distance = distance;
    heapifyUp(heapSize);
    heapSize++;
}

HeapNode extractMin() {
    HeapNode result = {-1, -1};

    if (heapSize == 0)
        return result;

    result = heap[0];
    heap[0] = heap[heapSize - 1];
    heapSize--;
    if (heapSize > 0)
        heapifyDown(0);

    return result;
}

void minHeapModule() {
    int n, i, id, distance;
    HeapNode result;

    heapSize = 0;

    printf("\nEnter number of available slots: ");
    scanf("%d", &n);

    for (i = 0; i < n; i++) {
        printf("Enter slot ID: ");
        scanf("%d", &id);
        printf("Enter distance from entry: ");
        scanf("%d", &distance);
        insertHeap(id, distance);
    }

    result = extractMin();

    if (result.slotID != -1)
        printf("\nNearest parking slot = %d\nDistance = %d\n",
               result.slotID, result.distance);
    else
        printf("No parking slot available.\n");
}

/* =========================================================
   UNIT 2 : GRAPH INPUT
   ========================================================= */

void inputGraph() {
    int m, i;
    char a, b;
    int u, v, w;

    initGraph();
    printZones();

    printf("\nEnter number of roads: ");
    scanf("%d", &m);

    if (m > MAX) m = MAX;

    for (i = 0; i < m; i++) {
        printf("\nRoad %d\n", i + 1);
        printf("Enter source zone: ");
        scanf(" %c", &a);
        printf("Enter destination zone: ");
        scanf(" %c", &b);
        printf("Enter distance: ");
        scanf("%d", &w);

        u = zoneIndex(a);
        v = zoneIndex(b);

        if (u == -1 || v == -1) {
            printf("Invalid zone. Road ignored.\n");
            i--;
            continue;
        }

        graph[u][v] = w;
        graph[v][u] = w;

        edges[edgeCount].u = u;
        edges[edgeCount].v = v;
        edges[edgeCount].w = w;
        edgeCount++;
    }
}

void displayGraph() {
    int i, j;

    printf("\n--- ADJACENCY MATRIX ---\n");
    printf("    ");
    for (i = 0; i < 6; i++)
        printf("%4c", zoneName[i]);
    printf("\n");

    for (i = 0; i < 6; i++) {
        printf("%c : ", zoneName[i]);
        for (j = 0; j < 6; j++) {
            if (graph[i][j] == INF)
                printf("%4s", "-");
            else
                printf("%4d", graph[i][j]);
        }
        printf("\n");
    }

    printf("\n--- ADJACENCY LIST ---\n");
    for (i = 0; i < 6; i++) {
        printf("%c -> ", zoneName[i]);
        for (j = 0; j < 6; j++) {
            if (graph[i][j] != INF && i != j)
                printf("%c(%d) ", zoneName[j], graph[i][j]);
        }
        printf("\n");
    }
}

/* =========================================================
   UNIT 2 : BFS / DFS / CONNECTED COMPONENTS
   ========================================================= */

void BFS(int start) {
    int queue[6], visited[6] = {0};
    int front = 0, rear = 0;
    int i, u;

    queue[rear++] = start;
    visited[start] = 1;

    printf("BFS: ");

    while (front < rear) {
        u = queue[front++];
        printf("%c ", zoneName[u]);

        for (i = 0; i < 6; i++) {
            if (graph[u][i] != INF && !visited[i]) {
                visited[i] = 1;
                queue[rear++] = i;
            }
        }
    }
    printf("\n");
}

void DFSUtil(int u, int visited[]) {
    int i;

    visited[u] = 1;
    printf("%c ", zoneName[u]);

    for (i = 0; i < 6; i++)
        if (graph[u][i] != INF && !visited[i])
            DFSUtil(i, visited);
}

void DFS(int start) {
    int visited[6] = {0};
    printf("DFS: ");
    DFSUtil(start, visited);
    printf("\n");
}

void connectedComponents() {
    int visited[6] = {0};
    int i, components = 0;

    for (i = 0; i < 6; i++) {
        if (!visited[i]) {
            components++;
            printf("Component %d: ", components);
            DFSUtil(i, visited);
            printf("\n");
        }
    }

    printf("Total connected components = %d\n", components);
}

/* =========================================================
   UNIT 2 : DIJKSTRA
   ========================================================= */

void dijkstra(int start) {
    int dist[6], used[6] = {0};
    int i, j, u, minDist;

    for (i = 0; i < 6; i++)
        dist[i] = graph[start][i];

    dist[start] = 0;

    for (i = 0; i < 6; i++) {
        u = -1;
        minDist = INF;

        for (j = 0; j < 6; j++)
            if (!used[j] && dist[j] < minDist) {
                minDist = dist[j];
                u = j;
            }

        if (u == -1)
            break;

        used[u] = 1;

        for (j = 0; j < 6; j++)
            if (!used[j] &&
                graph[u][j] != INF &&
                dist[u] + graph[u][j] < dist[j])
                dist[j] = dist[u] + graph[u][j];
    }

    printf("\nDijkstra shortest paths from %c:\n", zoneName[start]);
    for (i = 0; i < 6; i++) {
        if (dist[i] == INF)
            printf("%c -> unreachable\n", zoneName[i]);
        else
            printf("%c -> %d\n", zoneName[i], dist[i]);
    }
}

/* =========================================================
   UNIT 2 : BELLMAN-FORD
   ========================================================= */

void bellmanFord(int start) {
    int dist[6];
    int i, j, changed;

    for (i = 0; i < 6; i++)
        dist[i] = INF;

    dist[start] = 0;

    for (i = 1; i <= 5; i++) {
        changed = 0;

        for (j = 0; j < edgeCount; j++) {
            int u = edges[j].u;
            int v = edges[j].v;
            int w = edges[j].w;

            if (dist[u] != INF && dist[u] + w < dist[v]) {
                dist[v] = dist[u] + w;
                changed = 1;
            }

            if (dist[v] != INF && dist[v] + w < dist[u]) {
                dist[u] = dist[v] + w;
                changed = 1;
            }
        }

        if (!changed)
            break;
    }

    printf("\nBellman-Ford shortest paths from %c:\n", zoneName[start]);
    for (i = 0; i < 6; i++) {
        if (dist[i] == INF)
            printf("%c -> unreachable\n", zoneName[i]);
        else
            printf("%c -> %d\n", zoneName[i], dist[i]);
    }
}

/* =========================================================
   UNIT 2 : FLOYD-WARSHALL
   ========================================================= */

void floydWarshall() {
    int d[6][6];
    int i, j, k;

    for (i = 0; i < 6; i++)
        for (j = 0; j < 6; j++)
            d[i][j] = graph[i][j];

    for (k = 0; k < 6; k++)
        for (i = 0; i < 6; i++)
            for (j = 0; j < 6; j++)
                if (d[i][k] != INF && d[k][j] != INF &&
                    d[i][k] + d[k][j] < d[i][j])
                    d[i][j] = d[i][k] + d[k][j];

    printf("\n--- FLOYD-WARSHALL ALL-PAIRS SHORTEST PATH ---\n");
    printf("    ");
    for (i = 0; i < 6; i++)
        printf("%5c", zoneName[i]);
    printf("\n");

    for (i = 0; i < 6; i++) {
        printf("%c : ", zoneName[i]);
        for (j = 0; j < 6; j++) {
            if (d[i][j] == INF)
                printf("%5s", "-");
            else
                printf("%5d", d[i][j]);
        }
        printf("\n");
    }
}

/* =========================================================
   UNIT 2 : PRIM
   ========================================================= */

void prim() {
    int key[6], parent[6], inMST[6] = {0};
    int i, j, u, minKey, total = 0;

    for (i = 0; i < 6; i++) {
        key[i] = INF;
        parent[i] = -1;
    }

    key[0] = 0;

    printf("\n--- PRIM MST ---\n");

    for (i = 0; i < 6; i++) {
        u = -1;
        minKey = INF;

        for (j = 0; j < 6; j++)
            if (!inMST[j] && key[j] < minKey) {
                minKey = key[j];
                u = j;
            }

        if (u == -1)
            break;

        inMST[u] = 1;

        if (parent[u] != -1) {
            printf("%c - %c : %d\n",
                   zoneName[parent[u]], zoneName[u], key[u]);
            total += key[u];
        }

        for (j = 0; j < 6; j++)
            if (graph[u][j] != INF &&
                !inMST[j] &&
                graph[u][j] < key[j]) {
                key[j] = graph[u][j];
                parent[j] = u;
            }
    }

    printf("Total MST cost = %d\n", total);
}

/* =========================================================
   UNIT 2 : KRUSKAL
   ========================================================= */

int parentDSU[6];

int findSet(int x) {
    if (parentDSU[x] == x)
        return x;
    parentDSU[x] = findSet(parentDSU[x]);
    return parentDSU[x];
}

void unionSet(int a, int b) {
    a = findSet(a);
    b = findSet(b);
    if (a != b)
        parentDSU[b] = a;
}

void sortEdges() {
    int i, j;
    Edge temp;

    for (i = 0; i < edgeCount - 1; i++)
        for (j = 0; j < edgeCount - i - 1; j++)
            if (edges[j].w > edges[j + 1].w) {
                temp = edges[j];
                edges[j] = edges[j + 1];
                edges[j + 1] = temp;
            }
}

void kruskal() {
    int i, count = 0, total = 0;

    sortEdges();

    for (i = 0; i < 6; i++)
        parentDSU[i] = i;

    printf("\n--- KRUSKAL MST ---\n");

    for (i = 0; i < edgeCount && count < 5; i++) {
        int u = edges[i].u;
        int v = edges[i].v;

        if (findSet(u) != findSet(v)) {
            printf("%c - %c : %d\n",
                   zoneName[u], zoneName[v], edges[i].w);
            total += edges[i].w;
            unionSet(u, v);
            count++;
        }
    }

    if (count == 5)
        printf("Total MST cost = %d\n", total);
    else
        printf("Graph is disconnected, complete MST not possible.\n");
}

/* =========================================================
   UNIT 3 : 0/1 KNAPSACK
   Parking reservation acceptance
   ========================================================= */

void knapsackModule() {
    int n, capacity, i, w, value;
    int wt[MAX], val[MAX];
    int dp[MAX + 1][MAX + 1];

    printf("\nEnter number of reservation requests: ");
    scanf("%d", &n);

    printf("Enter available slot-hours capacity: ");
    scanf("%d", &capacity);

    if (n > MAX) n = MAX;
    if (capacity > MAX) capacity = MAX;

    for (i = 1; i <= n; i++) {
        printf("\nRequest %d slot-hours: ", i);
        scanf("%d", &w);
        printf("Request %d revenue: ", i);
        scanf("%d", &value);
        wt[i] = w;
        val[i] = value;
    }

    for (i = 0; i <= n; i++)
        dp[i][0] = 0;

    for (i = 0; i <= capacity; i++)
        dp[0][i] = 0;

    for (i = 1; i <= n; i++) {
        for (w = 1; w <= capacity; w++) {
            if (wt[i] <= w)
                dp[i][w] = max(dp[i - 1][w],
                               val[i] + dp[i - 1][w - wt[i]]);
            else
                dp[i][w] = dp[i - 1][w];
        }
    }

    printf("\nMaximum reservation revenue = %d\n",
           dp[n][capacity]);

    printf("Selected requests: ");
    w = capacity;
    for (i = n; i >= 1; i--) {
        if (dp[i][w] != dp[i - 1][w]) {
            printf("%d ", i);
            w -= wt[i];
        }
    }
    printf("\n");
}

/* =========================================================
   UNIT 3 : LCS
   Common zone-visit pattern
   ========================================================= */

void lcsModule() {
    char a[50], b[50];
    int m, n, i, j;
    int dp[51][51];
    char result[51];
    int index;

    printf("\nEnter first zone sequence: ");
    scanf("%s", a);

    printf("Enter second zone sequence: ");
    scanf("%s", b);

    m = strlen(a);
    n = strlen(b);

    for (i = 0; i <= m; i++)
        for (j = 0; j <= n; j++)
            dp[i][j] = 0;

    for (i = 1; i <= m; i++) {
        for (j = 1; j <= n; j++) {
            if (a[i - 1] == b[j - 1])
                dp[i][j] = dp[i - 1][j - 1] + 1;
            else
                dp[i][j] = max(dp[i - 1][j], dp[i][j - 1]);
        }
    }

    index = dp[m][n];
    result[index] = '\0';

    i = m;
    j = n;

    while (i > 0 && j > 0) {
        if (a[i - 1] == b[j - 1]) {
            result[index - 1] = a[i - 1];
            index--;
            i--;
            j--;
        } else if (dp[i - 1][j] > dp[i][j - 1]) {
            i--;
        } else {
            j--;
        }
    }

    printf("\nLCS length = %d\n", dp[m][n]);
    printf("Common zone-visit pattern = %s\n", result);
}

/* =========================================================
   UNIT 3 : MATRIX CHAIN MULTIPLICATION
   Parking analytics matrices
   ========================================================= */

void matrixChainModule() {
    int n, p[MAX + 1];
    int dp[MAX + 1][MAX + 1];
    int i, j, k, len, q;

    printf("\nEnter number of matrices: ");
    scanf("%d", &n);

    if (n > MAX) n = MAX;

    printf("Enter dimensions array of size %d:\n", n + 1);
    for (i = 0; i <= n; i++)
        scanf("%d", &p[i]);

    for (i = 1; i <= n; i++)
        dp[i][i] = 0;

    for (len = 2; len <= n; len++) {
        for (i = 1; i <= n - len + 1; i++) {
            j = i + len - 1;
            dp[i][j] = INF;

            for (k = i; k < j; k++) {
                q = dp[i][k] + dp[k + 1][j]
                    + p[i - 1] * p[k] * p[j];

                if (q < dp[i][j])
                    dp[i][j] = q;
            }
        }
    }

    printf("Minimum scalar multiplications = %d\n",
           dp[1][n]);
}

/* =========================================================
   UNIT 3 : RESOURCE ALLOCATION
   EV charger allocation among parking lots
   ========================================================= */

void resourceAllocationModule() {
    int lots, chargers, i, j;
    int profit[MAX][MAX];
    int dp[MAX + 1][MAX + 1];

    printf("\nEnter number of parking lots: ");
    scanf("%d", &lots);

    printf("Enter total number of EV chargers: ");
    scanf("%d", &chargers);

    if (lots > MAX) lots = MAX;
    if (chargers > MAX) chargers = MAX;

    printf("\nEnter expected revenue for each lot for 0..%d chargers.\n",
           chargers);

    for (i = 0; i < lots; i++) {
        printf("\nParking Lot %d:\n", i + 1);
        for (j = 0; j <= chargers; j++) {
            printf("Revenue with %d chargers: ", j);
            scanf("%d", &profit[i][j]);
        }
    }

    for (j = 0; j <= chargers; j++)
        dp[0][j] = profit[0][j];

    for (i = 1; i < lots; i++) {
        for (j = 0; j <= chargers; j++) {
            dp[i][j] = 0;

            for (int x = 0; x <= j; x++) {
                int value = dp[i - 1][j - x] + profit[i][x];
                if (value > dp[i][j])
                    dp[i][j] = value;
            }
        }
    }

    printf("\nMaximum EV charger allocation revenue = %d\n",
           dp[lots - 1][chargers]);
}

/* =========================================================
   UNIT 1 MENU
   ========================================================= */

void unit1Menu() {
    int choice;

    do {
        printf("\n====================================");
        printf("\n           UNIT 1 MENU");
        printf("\n====================================");
        printf("\n1. N-ary Tree Parking Hierarchy");
        printf("\n2. AVL Tree Slot Management");
        printf("\n3. Min-Heap Nearest Slot");
        printf("\n4. Back to Main Menu");
        printf("\nEnter choice: ");
        scanf("%d", &choice);

        switch (choice) {
            case 1: naryTreeModule(); break;
            case 2: avlModule(); break;
            case 3: minHeapModule(); break;
            case 4: break;
            default: printf("Invalid choice.\n");
        }
    } while (choice != 4);
}

/* =========================================================
   UNIT 2 MENU
   ========================================================= */

void unit2Menu() {
    int choice, start;
    char z;

    inputGraph();

    do {
        printf("\n====================================");
        printf("\n           UNIT 2 MENU");
        printf("\n====================================");
        printf("\n1. Display Graph");
        printf("\n2. BFS");
        printf("\n3. DFS");
        printf("\n4. Connected Components");
        printf("\n5. Dijkstra");
        printf("\n6. Bellman-Ford");
        printf("\n7. Floyd-Warshall");
        printf("\n8. Prim MST");
        printf("\n9. Kruskal MST");
        printf("\n10. Enter Graph Again");
        printf("\n11. Back to Main Menu");
        printf("\nEnter choice: ");
        scanf("%d", &choice);

        switch (choice) {
            case 1:
                displayGraph();
                break;

            case 2:
                printf("Enter starting zone: ");
                scanf(" %c", &z);
                start = zoneIndex(z);
                if (start != -1) BFS(start);
                else printf("Invalid zone.\n");
                break;

            case 3:
                printf("Enter starting zone: ");
                scanf(" %c", &z);
                start = zoneIndex(z);
                if (start != -1) DFS(start);
                else printf("Invalid zone.\n");
                break;

            case 4:
                connectedComponents();
                break;

            case 5:
                printf("Enter source zone: ");
                scanf(" %c", &z);
                start = zoneIndex(z);
                if (start != -1) dijkstra(start);
                else printf("Invalid zone.\n");
                break;

            case 6:
                printf("Enter source zone: ");
                scanf(" %c", &z);
                start = zoneIndex(z);
                if (start != -1) bellmanFord(start);
                else printf("Invalid zone.\n");
                break;

            case 7:
                floydWarshall();
                break;

            case 8:
                prim();
                break;

            case 9:
                kruskal();
                break;

            case 10:
                inputGraph();
                break;

            case 11:
                break;

            default:
                printf("Invalid choice.\n");
        }
    } while (choice != 11);
}

/* =========================================================
   UNIT 3 MENU
   ========================================================= */

void unit3Menu() {
    int choice;

    do {
        printf("\n====================================");
        printf("\n           UNIT 3 MENU");
        printf("\n====================================");
        printf("\n1. 0/1 Knapsack - Reservation Requests");
        printf("\n2. LCS - Common Zone Route");
        printf("\n3. Matrix Chain Multiplication");
        printf("\n4. Resource Allocation - EV Chargers");
        printf("\n5. Back to Main Menu");
        printf("\nEnter choice: ");
        scanf("%d", &choice);

        switch (choice) {
            case 1: knapsackModule(); break;
            case 2: lcsModule(); break;
            case 3: matrixChainModule(); break;
            case 4: resourceAllocationModule(); break;
            case 5: break;
            default: printf("Invalid choice.\n");
        }
    } while (choice != 5);
}

/* =========================================================
   MAIN MENU
   ========================================================= */

int main() {
    int choice;

    initGraph();

    printf("\n==============================================");
    printf("\n SMART PARKING ALLOCATION AND RESERVATION");
    printf("\n        DSA-II PROJECT - GITHUB");
    printf("\n==============================================");

    do {
        printf("\n\n--------------- MAIN MENU ----------------");
        printf("\n1. Unit 1 - Trees and Heap");
        printf("\n2. Unit 2 - Graph Algorithms");
        printf("\n3. Unit 3 - Dynamic Programming");
        printf("\n4. Exit");
        printf("\nEnter choice: ");
        scanf("%d", &choice);

        switch (choice) {
            case 1:
                unit1Menu();
                break;

            case 2:
                unit2Menu();
                break;

            case 3:
                unit3Menu();
                break;

            case 4:
                printf("\nThank you!\n");
                break;

            default:
                printf("Invalid choice. Try again.\n");
        }
    } while (choice != 4);

    return 0;
}
