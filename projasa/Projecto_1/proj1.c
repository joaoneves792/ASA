/***
* Instituto Superior Tecnico
* Analise e Sintese de Algoritmos - 2014
* 
* Projecto 1 
*
* Grupo 183
* Rogerio Marcial Pires n-49746
* Joao Neves n-70171
*
***/

/* Bibliotecas */
#include <stdio.h>
#include <stdlib.h>


/* Estruturas de dados */

/* Elementos de uma lista de inteiros (usada para a lista de adjacencias e para pilhas)*/
typedef struct listElem{
    int value;
    struct listElem* next;
}ListElem;
	
/* Grafo: V: numero de vertices , adjList: ponteiro para vector com listas dos arcos de cada vertice
*/
typedef struct graph {
    int V;
    ListElem** adjList;
} Graph;
 
/* Variaveis para o algoritmo de Tarjan*/  
int visited, numOfSCC, sizeLargestSCC, onlyShareSameSCC;
int* d;
int* low;
int* SCC; /*Vector com comprimento V com o numero do SCC ao qual pertence o vertice correspondente ao indice*/
char* isOnStack;
ListElem* stack;
 
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

 
/* Adiciona um arco a lista de adjacencias
*/
void addEdge(Graph* graph, int src, int dest){
    /* Novo arco adicionado ao principio da lista de adjacencias do vertice src*/
    ListElem* newV = (ListElem*) malloc(sizeof(ListElem));
    newV->value = dest;
    newV->next = graph->adjList[src];
    graph->adjList[src] = newV;
}


/* Push de um vertice no topo de um stack (implementado com uma lista)*/
void push(ListElem** head, int vertexId){
    ListElem* new = (ListElem*)malloc(sizeof(ListElem));
    new->value = vertexId;
    new->next = *head;
    *head = new;
}

/* Pop de um vertice do topo de uma stack*/
int pop(ListElem** head){
    int vertexId;
    ListElem* next;

    if(*head == NULL)
	return -1;

    vertexId = (*head)->value;
    next = (*head)->next;
    free(*head);
    *head = next;
    return vertexId;
}

/*Faz pop de um vertice do stackA e faz push no stackB devolvendo o ID do vertice  (Optimizacao, assim nao e 
  preciso fazer free e maloc para o mesmo vertice)*/
int popAndPush(ListElem** stackA, ListElem** stackB){
    int vertexId;
    ListElem* elem;

    if(*stackA == NULL)
	return -1;

    vertexId = (*stackA)->value;
    elem = (*stackA);
    *stackA = elem->next;
    
    elem->next = *stackB;
    *stackB = elem;

    return vertexId;
}

/* Tarjan Visit*/
void tarjan_visit(Graph* graph, int v){
    int u, SCCsize;
    ListElem* adjVertex;
    ListElem* SCCstack; /*stack temporario para os elementos de uma SCC*/

    SCCstack = NULL; /*Inicializa o stack vazio*/

    d[v] = visited;
    low[v] = visited++;

    push(&stack, v);
    isOnStack[v] = 1;

    adjVertex = graph->adjList[v];
    while(adjVertex){

        if( (d[adjVertex->value] == -1) ||   /*Se o vertice adjacente nao foi descoberto OU se esta no stack*/
            (isOnStack[adjVertex->value]) ){
                
            if(d[adjVertex->value] == -1)   /*Se ainda nao foi descoberto entao faz visit*/
                tarjan_visit(graph, adjVertex->value);
                
            if(low[v] > low[adjVertex->value]) /*Compara os valores de low e fica com o menor*/
                low[v] = low[adjVertex->value];
        }

        adjVertex = adjVertex->next;
    }

    if( d[v] == low[v] ){   /*Se este vertice for a raiz de um SCC*/
        SCCsize = 0;
        /*Faz pop de todos os vertices que estejam no stack ate chegar ao proprio*/
        do{
            u = popAndPush(&stack, &SCCstack);
            isOnStack[u] = 0;
            SCC[u] = numOfSCC;
            SCCsize++;
        }while(u != v);
        numOfSCC++;

        /*Contamos o numero de elementos de um SCC de cada vez que se faz um conjunto 
          de pops e apenas guardamos o valor se for maior que o valor do maior anterior*/
        if(SCCsize > sizeLargestSCC) 
            sizeLargestSCC = SCCsize; 

        /*Para cada vertice do SCC recem descoberto, vamos verificar se partilha com algum 
          outro vertice que nao pertence ao mesmo SCC*/
        while((v = pop(&SCCstack)) >= 0){
            adjVertex = graph->adjList[v];
            while(adjVertex){
	        if( SCC[adjVertex->value] != SCC[v] ) /*Se o vertice adjacente nao pertence ao mesmo SCC*/
			return;
                adjVertex = adjVertex->next;
            }
        }
        onlyShareSameSCC++; /*A execucao so chega aqui se nenhum dos vertices 
                              do SCC tiver partilhas com outro SCC*/
    }


}

void tarjan(Graph* graph){
    int i;

    numOfSCC = 0;
    sizeLargestSCC = 0;
    onlyShareSameSCC = 0;
    visited = 0;

    /*Cria e inicializa os vectores e o stack*/
    d = (int*)calloc(graph->V, sizeof(int));
    low = (int*)calloc(graph->V, sizeof(int));
    SCC = (int*)calloc(graph->V, sizeof(int));
    isOnStack = (char*)calloc(graph->V, sizeof(char));

    for(i=0; i<graph->V; i++){
        d[i] = -1;
        low[i] = -1;
        isOnStack[i] = 0;
        SCC[i] = -1;
    }

    stack = NULL; /*Inicializa um stack vazio*/

    /*Ciclo Principal*/
    for(i=0; i<graph->V; i++){
        if(d[i] >= 0)         /*Se o vertice ja foi visitado entao salta para o proximo*/
            continue;
        tarjan_visit(graph, i);
    }

}


int main (int argc, char* argv[]) {
    int i;
    int vertices, edges; 
    int src, dst;
    Graph* graph;

    scanf("%d%d", &vertices, &edges);

    graph = createGraph(vertices);

    /** Introduz as listas de adjacencias no grafo */
    for (i=1; i<= edges; i++) {
        scanf("%d%d", &src, &dst);
        addEdge(graph, src-1, dst-1); /*-1 pq o a representacao interna dos vertices 
                                        utiliza numeros de 0 ate V-1, mas o input vem de 1 ate V */
    }

    /*Conta o numero de SCCs, o tamanho do maior SCC e quantos SCCs estao isolados*/	
    tarjan(graph);

    printf("%d\n%d\n%d\n", numOfSCC, sizeLargestSCC, onlyShareSameSCC);

    return 0;
}
	

