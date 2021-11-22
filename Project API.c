#include <stdio.h>
#include <string.h>
#include<stdlib.h>
#include<limits.h>

typedef struct s {
    struct s *next;
    struct s *pred;
    long pathCost;
    int graphIndex;
}nodo_t;

long dijkstra(unsigned int **adiacencyMatrix, int nodesNumber);
nodo_t * BuildTopK(nodo_t *head, long cost, int index, int k, nodo_t **tail, int *listsNodesNumber, int full);
void printRanking(nodo_t *head);
void freelist(nodo_t * head );

int main( int argc, char * argv[] ) {

    int i,j,k;
    int counter;
    int saveDigits;
    int arrayPosition;
    unsigned int weight;
    int length;
    int digits = 0;
    int multiplier = 1;
    int nodesNumber = 0;
    int rankingLength = 0;
    char * nodeWeights;
    unsigned int ** graph;
    char data[25];
    char command[14];
    int graphIndex = 0;
    long * topK;
    long minimumPath;
    nodo_t *head = NULL;
    nodo_t *maxPath[1];
    int listsNodesNumber = 0;
    int full;
    full = 0;
    *maxPath=NULL;
    
    //---------------Acquisisco d e k
    if( !fgets(data, 25, stdin ) )
        return 0;
    length = (int)strlen(data);
    i = length-2;
    while (data[i]!=' '){
        rankingLength+=multiplier*(data[i]-48);
        i--;
        multiplier*=10;
    }
    i--;
    multiplier=1;
    while (i>=0){
        nodesNumber+=multiplier*(data[i]-48);
        i--;
        multiplier*=10;
    }
    //-------------------fine acquisizione d e k

    if( !fgets(command, 14, stdin)){
        return 0;
    }//Acquisisco primo comando
    nodeWeights = (char *)malloc(nodesNumber * sizeof(char) * 50);//Creo spazio per la stringa contenente i pesi dei nodi
    topK = (long *)malloc(rankingLength * sizeof(long));
    graph = (unsigned int **)malloc( nodesNumber * sizeof(unsigned int *) );

    //----------Finchè il comando è AggiungiGrafo acquisisco tutti i "nodesNumber" pesi di ogni nodo
    //----------Inoltre li salvo in una matrice di int
    while(strcmp(command, "AggiungiGrafo")==0 || strcmp(command, "TopK\n")==0){
        if(strcmp(command, "AggiungiGrafo")==0){
            if( !fgets(nodeWeights, nodesNumber*10, stdin) )
                return 0;
            for( i=0; i<nodesNumber; i++ ) {
                if( !fgets(nodeWeights, nodesNumber*10, stdin))
                    return 0;
                // printf("\nnodesWeight %s\n", nodeWeights);
                k=0;
                arrayPosition=0;
                multiplier=1;
                graph[i] = (unsigned int *)malloc( nodesNumber * sizeof(unsigned int) );
                for(j=0; j<nodesNumber; j++ ){
                    while (nodeWeights[k]>='0' && nodeWeights[k]<='9' && nodeWeights[k]!='\n'){
                        multiplier*=10;
                        digits++;
                        k++;
                    }
                    multiplier/=10;
                    weight=0;
                    saveDigits=digits;
                    for( counter = 0; counter < saveDigits; counter++ ){
                        weight += (nodeWeights[k - digits] - 48) * multiplier;
                        multiplier/=10;
                        digits--;
                    }
                    if( saveDigits==0 )
                        weight += (nodeWeights[k - digits] - 48) * multiplier;
                    digits = 0;
                    if(weight==0)
                        weight=INT_MAX;
                    graph[i][arrayPosition] = weight;
                    arrayPosition++;
                    multiplier=1;
                    k++;
                }
            }
            minimumPath = dijkstra(graph,nodesNumber);
            for( i=0; i<nodesNumber; i++ )
                free(graph[i]);
            if( graphIndex == rankingLength )
                full = 1;
            head = BuildTopK(head, minimumPath, graphIndex, rankingLength, maxPath, &listsNodesNumber, full);
            graphIndex++;
        } else if ( strcmp(command, "TopK\n")==0 )
            printRanking(head);
        if( !fgets(command, 14, stdin) ){
            free(nodeWeights);
            free(topK);
            free(graph);
            freelist(head);
            return 0;
        }

    }
    //-----------Fine acquisizione pesi grafo
    //free delle malloc
    free(nodeWeights);
    free(topK);
    free(graph);
    freelist(head);
    return 0;
}

long dijkstra(unsigned int **adiacencyMatrix, int nodesNumber){

    long lowerPath = 0;
    unsigned int distance[nodesNumber];
    int included[nodesNumber];
    unsigned int minDistance;
    int count, nextNode, i;

    for( i=1; i < nodesNumber; i++ ){
        distance[i]=UINT_MAX;
        included[i]=0;
    }
    included[0]=1;
    distance[0]=0;
    nextNode = 0;
    for( count=0; count<nodesNumber-1; count++ ){
        minDistance=UINT_MAX;
        for( i=0; i < nodesNumber; i++ ){
            if( distance[i] < minDistance && !included[i]){
                minDistance=distance[i];
                nextNode=i;
            }
        }
        included[nextNode]=1;
        for( i=0; i < nodesNumber; i++ ){
            if( included[i]==0 ){
                if( distance[i] > distance[nextNode] + adiacencyMatrix[nextNode][i] && distance[nextNode]!=UINT_MAX && adiacencyMatrix[nextNode][i]!=UINT_MAX){
                    distance[i] = distance[nextNode] + adiacencyMatrix[nextNode][i];
                }
            }
        }
    }
    for( i=1; i < nodesNumber; i++ ){
        if(distance[i]<INT_MAX)
            lowerPath+=distance[i];
    }
    return lowerPath;
}

nodo_t * BuildTopK(nodo_t *head, long cost, int index, int k, nodo_t **tail, int *listsNodesNumber, int full){
    nodo_t *new = NULL;
    nodo_t *tmp;
    int ranked;
    ranked=2;
    if (head) {
         if(*tail){
             if( cost >= (*tail)->pathCost && *listsNodesNumber==k ){
                 return head;
             }
            if( cost >= (*tail)->pathCost && !full ){//se è il costo più alto ma ho spazio nella classifica lo aggiungo
                if( ( new = (nodo_t *)malloc( sizeof(nodo_t)))==NULL )
                    return 0;
                new->pathCost=cost;
                new->graphIndex=index;
                new->next=NULL;
                new->pred=(*tail);
                (*tail)->next = new;
                (*tail)=new;
                *listsNodesNumber= *listsNodesNumber+1;
                return head;
            } else if((*tail)->pred){
                if( cost >= (*tail)->pred->pathCost && cost < (*tail)->pathCost){//se devo metterlo al posto dell'ultimo differenzio tra classifica giÃ  piena oppure ancora spazio in classifica e lo aggiungo
                    if( *listsNodesNumber < k ){ //ho ancora spazio in classifica
                        if( ( new = (nodo_t *)malloc( sizeof(nodo_t)))==NULL )
                            return 0;
                        new->pathCost=cost;
                        new->graphIndex=index;
                        new->next=(*tail);
                        new->pred=(*tail)->pred;
                        (*tail)->pred->next = new;
                        (*tail)->pred=new;
                        *listsNodesNumber= *listsNodesNumber+1;
                        return head;
                    } else { //la classifica è piena
                        (*tail)->pathCost=cost;
                        (*tail)->graphIndex=index;
                        return head;
                    }
                }
            }
        }
        if (head->pathCost > cost ) {//Se ho trovato il pathCost più basso lo aggiungo come primo nodo della lista
            if( ( new = (nodo_t *)malloc( sizeof(nodo_t)))==NULL )
                return 0;
            new->pathCost=cost;
            new->graphIndex=index;
            new->next = head;
            new->pred=NULL;
            head->pred=new;
            head = new;
            if(full){
                (*tail)=(*tail)->pred;
                free((*tail)->next);
                (*tail)->next=NULL;
            } else{
                *(listsNodesNumber) = *(listsNodesNumber)+1;
            }
            return head;
        } else {
            for (tmp = head; tmp->next && tmp->next->pathCost <= cost; tmp = tmp->next){
                tmp->next->pred = tmp;
                ranked++;
            }
            if (tmp->next) {//se è in mezzo alla lista
                if( ( new = (nodo_t *)malloc( sizeof(nodo_t)))==NULL )
                    return 0;
                new->pathCost=cost;
                new->graphIndex=index;
                new->next = tmp->next;
                new->next->pred=new;
                new->pred=tmp;
                tmp->next = new;
                ranked++;
                for (tmp=new; tmp->next->next; tmp = tmp->next){//elimino l'ultimo elemento se sfora k
                    tmp->next->pred = tmp;
                    ranked++;
                }
                if(full){
                    free(tmp->next);
                    tmp->next=NULL;
                    (*tail)=tmp;
                    ranked--;
                } else {
                    tmp->next->pred = tmp;
                    (*tail)=tmp->next;
                    *(listsNodesNumber) = *(listsNodesNumber)+1;
                }
                return head;
            } else{//se invece hanno tutti un costo minore di pathCost e ho ancora spazio nella classifica, lo aggiungo come ultimo classificato
                if(!full){
                    if( ( new = (nodo_t *)malloc( sizeof(nodo_t)))==NULL )
                        return 0;
                    new->pathCost=cost;
                    new->graphIndex=index;
                    new->next = NULL;
                    new->pred=tmp;
                    tmp->next=new;
                    ranked++;
                } else
                    (*tail)=tmp;
               *listsNodesNumber=ranked;
                return head;
            }
        }
    } else {//Se la lista è vuota, creo il primo nodo
        if( ( new = (nodo_t *)malloc( sizeof(nodo_t)))==NULL ) {
            return head;
        }
        new->pathCost=cost;
        new->graphIndex=index;
        new->next = NULL;
        new->pred=NULL;
        head = new;
        *tail=head;
        *listsNodesNumber=1;
        return head;
    }
}

void printRanking(nodo_t *head ){
    nodo_t *tmp;
    if (head) {
        if(!head->next)
            printf("%d ", head->graphIndex);
        else{
            for (tmp = head; tmp->next ; tmp = tmp->next)
                printf("%d ", tmp->graphIndex);
            printf("%d", tmp->graphIndex);
        }
    }
    printf("\n");
}

void freelist(nodo_t * head ){
    nodo_t *tmp;
    while(head){
        tmp = head;
        head = head->next;
        free(tmp);
    }
}
