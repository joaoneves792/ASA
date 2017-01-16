/***
* Instituto Superior Tecnico
* Analise e Sintese de Algoritmos - 2014
* 
* Projecto 2 
*
* Grupo 183
* Rogerio Marcial Pires n-49746
* Joao Neves n-70171
*
***/

/* Bibliotecas */
#include <stdio.h>
#include <stdlib.h>
#include <limits.h>

/* Estruturas de dados */

/* Elementos de uma lista de inteiros (usada para a lista de adjacencias e para filas)*/
typedef struct listElem{
    int value;
    struct listElem* next;
}ListElem;

/*Fila*/	
typedef struct{
    ListElem* first;
    ListElem* last;
}Queue;


/* Grafo: V: numero de vertices , adjList: ponteiro para vector com listas dos arcos de cada vertice
*/
typedef struct graph {
    int V;
    ListElem** adjList;
} Graph;
 
/* Cria um grafo com V vertices
*/
Graph* createGraph(int V){
    int i;
    Graph* graph = (Graph*) malloc(sizeof(Graph));
	
    /* Numero de vertices */
    graph->V = V;
	
    /* Array de Listas de Adjacencias, dimensao V */
    graph->adjList = (ListElem**)malloc(V*sizeof(ListElem*));
	
    /* Inicializa o vector de listas de Vertices Adjacentes a NULL */
    for (i = 0; i < V; ++i) {
        graph->adjList[i] = NULL;
    }
		
    return graph;
}

 
/* Adiciona um arco a lista de adjacencias (sem direcao)
*/
void addEdge(Graph* graph, int v1, int v2){
    ListElem* newV;
    /* Novo arco adicionado ao principio da lista de adjacencias do vertice v1*/
    newV = (ListElem*) malloc(sizeof(ListElem));
    newV->value = v2;
    newV->next = graph->adjList[v1];
    graph->adjList[v1] = newV;
    /* Novo arco adicionado ao principio da lista de adjacencias do vertice v2*/
    newV = (ListElem*) malloc(sizeof(ListElem));
    newV->value = v1;
    newV->next = graph->adjList[v2];
    graph->adjList[v2] = newV;
}

void enqueue(Queue* q, int v){
    ListElem* new = (ListElem*)malloc(sizeof(ListElem));
    new->value = v;  
    new->next = NULL;  
    if(q->first == NULL){ /*A fila esta vazia*/
        q->first = new;
        q->last = new;
    }else{ /*A fila tem pelo menos um elemento*/
        q->last->next = new;
        q->last = new;
    }
}

int dequeue(Queue* q){
    ListElem* first;
    int retVal;
     
    if(q->first == NULL) /*Se a fila esta vazia*/
        return -1;
    retVal = q->first->value;
    if(q->first == q->last){ /*Se a fila apenas tem um elemento*/
        free(q->first);
        q->first = NULL;
        q->last = NULL;
    }else{ /*Se a fila tem mais do que um elemento*/
        first = q->first;
        q->first = first->next;
        free(first);
    }
    return retVal;
}

void findBlocks(Graph* graph, int numCritical, int* criticalPoints){
    int minNumBlocks = INT_MAX;
    int numBlocks = 0;
    int i, j, u, v, s;
    char* visited;
    char* isCritical;
    Queue q;
    ListElem* adjVertex;

    /*Initialize the queue*/
    q.first = NULL;
    q.last = NULL;
    /*Create the visited and isCritical vectors*/
    visited = (char*)calloc(graph->V, sizeof(char));
    isCritical = (char*)calloc(graph->V, sizeof(char));
  
    /*Initialize the isCritical vector*/
    for(i=0; i<graph->V; i++)
        isCritical[i] = 0;
    for(i=0; i<numCritical; i++)
        isCritical[criticalPoints[i]] = 1;

    /*Main loop - Do this for every critical point*/
    for(i=0; i<numCritical; i++){
        numBlocks = 0;
        s = criticalPoints[i];     /*Set the starting vertex*/
        for(j=0; j<graph->V; j++)
            visited[j] = 0;
        visited[s] = 1;
        enqueue(&q, s);
        while( (u=dequeue(&q)) != -1){ /* While there are vertexes on the queue*/
            adjVertex = graph->adjList[u];
            while(adjVertex){/* Do this for every vertex adjacent to u*/
                v = adjVertex->value;
                if(isCritical[v] && ( v != s )){
                    numBlocks++;
                    adjVertex = adjVertex->next;
                    continue;
                }
                if(!visited[v]){
                    visited[v] = 1;
                    enqueue(&q, v);
                }
                adjVertex = adjVertex->next;
            }


        }

        if(numBlocks < minNumBlocks)
            minNumBlocks = numBlocks;

    }

    printf("%d\n", minNumBlocks);

    free(visited);
    free(isCritical);
}

int main (int argc, char* argv[]) {
    int i,j;
    int vertices, edges; 
    int v1, v2;
    int numProblems, numCritical;
    int* criticalPoints;
    Graph* graph;


    scanf("%d%d", &vertices, &edges);

    graph = createGraph(vertices);

    /** Introduz as listas de adjacencias no grafo */
    for(i=0; i< edges; i++){
        scanf("%d%d", &v1, &v2);
        addEdge(graph, v1, v2); 
    }

    scanf("%d", &numProblems);

    criticalPoints = NULL;
    /* Para cada problema lemos os pontos criticos e corremos findBlocks() para calcular o num min de bloqueios*/
    for(i=0; i<numProblems; i++){
        scanf("%d", &numCritical);
        criticalPoints = (int*)calloc(numCritical, sizeof(int));
        for(j=0; j<numCritical; j++)
            scanf("%d", &criticalPoints[j]);
        findBlocks(graph, numCritical, criticalPoints);
        free(criticalPoints);
    }



    return 0;
}
	

