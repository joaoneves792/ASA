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
    struct listElem* prev; /*This field is only used in the adjacent list!! IT IS NOT INITIALIZED IN THE QUEUE*/
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
    newV->prev = NULL;
    if(newV->next)
        newV->next->prev = newV;
    graph->adjList[v1] = newV;
    /* Novo arco adicionado ao principio da lista de adjacencias do vertice v2*/
    newV = (ListElem*) malloc(sizeof(ListElem));
    newV->value = v1;
    newV->next = graph->adjList[v2];
    newV->prev = NULL;
    if(newV->next)
        newV->next->prev = newV;
    graph->adjList[v2] = newV;
}

void removeEdge(Graph* graph, int w, ListElem* edgeToRemove){ 
    /*TODO Add some coments here!*/
    if(edgeToRemove->next == NULL && edgeToRemove->prev == NULL)
        graph->adjList[w] = NULL;
    else if(edgeToRemove->next == NULL)
        edgeToRemove->prev->next = NULL;
    else if(edgeToRemove->prev == NULL){
        edgeToRemove->next->prev = NULL;
        graph->adjList[w] = edgeToRemove->next;
    }else{
        edgeToRemove->prev->next = edgeToRemove->next;
        edgeToRemove->next->prev = edgeToRemove->prev;
    }
    free(edgeToRemove);
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

void createResidualNetwork(Graph* graph, Graph** rN){
    int i;
    ListElem* newV;
    ListElem* adjVertex;
    
    /* Just create a copy of graph*/
    *rN = createGraph(graph->V);
    for(i=0; i<(*rN)->V; i++){
        adjVertex = graph->adjList[i];
        while(adjVertex){
            /*Create a copy of each entry in the adjacent List*/
            newV = (ListElem*) malloc(sizeof(ListElem));
            newV->value = adjVertex->value;
            newV->next = (*rN)->adjList[i];
            newV->prev = NULL;
            if(newV->next)
                newV->next->prev = newV;
            (*rN)->adjList[i] = newV;
            
            adjVertex = adjVertex->next;
        }
    }
}

void freeResidualNetwork(Graph* rN){
    int i;
    ListElem* adjVertex;
    ListElem* vertexToRemove;
    for(i=0; i<rN->V; i++){
        adjVertex = rN->adjList[i];
        while(adjVertex){
            vertexToRemove = adjVertex;
            adjVertex = adjVertex->next;
            free(vertexToRemove);
        }
    }
    free(rN->adjList);
    free(rN);
}

void findBlocks(Graph* graph, int numCritical, int* criticalPoints){
    int minNumBlocks = INT_MAX;
    int numBlocks = 0;
    int i, j;
    int u, v, w, x, s; /*These are all vertexes (s is the source critical point in each iteration)*/
    int* d;  /*Distance from s*/
    int* p;  /*Predecessors*/
    char* reachable;
    char* isCritical;
    Queue q;
    Graph* residualNetwork = NULL;
    ListElem* adjVertex;
    ListElem* edgeToRemove;

    /*Initialize the queue*/
    q.first = NULL;
    q.last = NULL;
    /*Create the d, p , reachable and isCritical vectors*/
    d = (int*)calloc(graph->V, sizeof(int));
    p = (int*)calloc(graph->V, sizeof(int));
    isCritical = (char*)calloc(graph->V, sizeof(char));
    reachable = (char*)calloc(graph->V, sizeof(char));  

    /*Initialize the isCritical vector*/
    for(i=0; i<graph->V; i++)
        isCritical[i] = 0;
    for(i=0; i<numCritical; i++)
        isCritical[criticalPoints[i]] = 1;

    /*Main loop - Do this for every critical point*/
    for(i=0; i<numCritical; i++){
        createResidualNetwork(graph, &residualNetwork);
        numBlocks = 0;
        s = criticalPoints[i];     /*Set the starting vertex*/
        for(j=0; j<residualNetwork->V; j++){
            d[j] = -1;
            p[j] = -1;
        }
        d[s] = 0;
        enqueue(&q, s);
        while( (u=dequeue(&q)) != -1){ /* While there are vertexes on the queue*/
            adjVertex = residualNetwork->adjList[u];
            while(adjVertex){/* Do this for every vertex adjacent to u*/
                v = adjVertex->value;
                
                if( d[v] == -1 ){ /*If vertex has not been discovered or is critical (and not s)*/
                    if(isCritical[v]){ /*If v is critical then remove the path from s to v*/
                        w = u; /*Set predecessor*/
                        x = v; /*Set sucessor*/
                        edgeToRemove = adjVertex; /*The first edge to remove is the one we just followed*/
                        removeEdge(residualNetwork, w, edgeToRemove); 
                        while(1){ /*Do this until the end of the path*/
                            x = w; /*Set the successor*/
                            w = p[w]; /*Set the predecessor*/
                            if(w > -1){ /*Get the next edge on this reverse path*/
                               edgeToRemove = residualNetwork->adjList[w];
                               if(edgeToRemove == NULL) /*If this vertex no longer has adjacent vertexes (due to previous removals)*/
                                   break;
                               while(edgeToRemove != NULL && edgeToRemove->value != x)
                                   edgeToRemove = edgeToRemove->next;
                               if(edgeToRemove == NULL) /*If this edge was already removed*/
                                   break;
                            }else
                                break; /*We have reached vertex s (The end of the path)*/
                            removeEdge(residualNetwork, w, edgeToRemove); 
                        }
                    }else{
                        d[v] = d[u] + 1;
                        p[v] = u;
                        enqueue(&q, v);
                    }
                }
                adjVertex = adjVertex->next;
            }


        }

        /*Now we must do another BFS to determine which vertexes are reachable from s*/
        for(j=0; j<residualNetwork->V; j++)
            reachable[j] = 0;
        reachable[s] = 1;
        enqueue(&q, s);
        while( (u=dequeue(&q)) != -1){ /* While there are vertexes on the queue*/
            adjVertex = residualNetwork->adjList[u];
            while(adjVertex){/* Do this for every vertex adjacent to u*/
                v = adjVertex->value;
                if( reachable[v] == 0 ){ /*If vertex has not been discovered*/
                    reachable[v] = 1;
                    enqueue(&q, v);
                }
                adjVertex = adjVertex->next;
            }

        } 

        
        /*Now we go through all edges and count the number of edges between reachable and unreachable vertexes */
        for(j=0; j<graph->V; j++)
            if(reachable[j]){
                adjVertex = graph->adjList[j];
                while(adjVertex){
                    if(!reachable[adjVertex->value]){
                       numBlocks++;                
                    }
                    adjVertex = adjVertex->next;
                }
            }

        
        if(numBlocks < minNumBlocks)
            minNumBlocks = numBlocks;

        freeResidualNetwork(residualNetwork);
    }

    printf("%d\n", minNumBlocks);

    free(d);
    free(p);
    free(isCritical);
    free(reachable);
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
	

