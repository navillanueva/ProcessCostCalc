
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <stddef.h>
#include <sys/stat.h>
#include <pthread.h>
#include "queue.h"
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>


#define NUM_CONSUMERS 1

//Creamos el mutex y las variables condición para controlar la escritura en el buffer
pthread_mutex_t mutex;
pthread_cond_t not_full;
pthread_cond_t not_empty;
queue *q;
struct element *procesos;



void *producir(void *arg){
	int *a;
	a=(int *)arg;
	for(int i=a[0]; i<=a[1];i++){
		pthread_mutex_lock(&mutex);
		while(queue_full(q)==1){
			pthread_cond_wait(&not_full, &mutex);
		}
		queue_put(q, (procesos+i));
		pthread_cond_signal(&not_empty);
		pthread_mutex_unlock(&mutex);
	}
	pthread_exit(0);
}


	
void *consumir(void *arg){
	int *a;
	a=(int *)arg;
	struct element *elem;
	for(int i=0; i<a[0];i++){
		pthread_mutex_lock(&mutex);
		while(queue_empty(q)==1){
			pthread_cond_wait(&not_empty, &mutex);
		}
		elem=queue_get(q);
		if(elem->type==1){
			a[1]=a[1]+(elem->time);
		}
		if(elem->type==2){
			a[1]=a[1]+(elem->time*3);
		}
		if(elem->type==3){
			a[1]=a[1]+(elem->time*10);
		}
		pthread_cond_signal(&not_full);
		pthread_mutex_unlock(&mutex);
	}
	pthread_exit(0);
}

/**
 * Entry point
 * @param argc
 * @param argv
 * @return
 */
int main (int argc, const char * argv[] ) {
	if(argc==4){	//comprobamos que num argumentos 4 
		if(atoi(argv[2])>0){     //comprobamos que el num_prod>0
			if(atoi(argv[3])>0){		//comprobamos que tamaño que buffer es mayor que cero
				q=queue_init(atoi(argv[3]));//creamos el buffer circular
				int num_prod=atoi(argv[2]);	//numero de productores
				int queue_size=atoi(argv[3]);	//tamaño de buffer
				FILE *f=fopen(argv[1], "r");	//abrimos archivo 
				int num_op;
				fscanf(f, "%d", &num_op);	//escaneamos la primera linea para ver cuantas operaciones vamos a hacer
				procesos=(struct element *)malloc(num_op*(sizeof( struct element)));	//aqui almacenaremos todas las operaciones
				int cont=1;	//para comprobar que la numeracion de las operaciones es secuencial
				int index;
				int type;
				int time;
				struct element aux;
				for(int i=0;i<num_op;i++){	//extraemos los datos de cada operacion (id, tipo de maquina, tiempo de uso)
					fscanf(f, "%d %d %d", &index, &type, &time);
					if(index!=cont){ //comrobamos que la numeracion de las operaciones es secuencial
						perror("Las operaciones no están enumeradas secuencialmente o hay menos operaciones de las indicadas inicialmente");//si no lo es lanzamos este error 
						exit(-1);
					}
					aux.type=type;		//almacenamos el tipo y el tiempo de cada operacion ene el array
					aux.time=time;
					memcpy((procesos+i), &aux, sizeof(struct element));
					cont++;
				}
				
				//ahora se distribuye las operaciones que le van a tocar a cada productor
				int procByProd[num_prod];	//en este bucle inicializamos el vector a cero
				for(int i=0; i<num_prod; i++) {
					procByProd[i]=0;
				}

				for(int i=0; i<num_op; i++){	//en este bucle repartimos las operaciones totales de una en una entre los productores de forma ciclica
					procByProd[i%num_prod]++;
				}
				
				int assignment[num_prod][2];
				for(int i=0; i<num_prod; i++){ //asignamos de que operacion a que operacion ejecutaria cada productor
					if(i==0){	//primer productor
						assignment[i][0]=0;
						assignment[i][1]=procByProd[i]-1;
					}
					else{	//el resto empiezan en el siguiente al que termino el anterior
						assignment[i][0]=assignment[i-1][1]+1;
						assignment[i][1]=assignment[i][0]+(procByProd[i]-1);			
					}
				}


				pthread_t producer[num_prod];
				
				for(int i=0; i<num_prod; i++){
					pthread_create(&producer[i], NULL, producir, (void *)&assignment[i]);
				}
				
				int total = 0;
				int param_C [2];
				param_C[0]=num_op;
				param_C[1]=total;
				
				pthread_t consumer;
				pthread_create(&consumer, NULL, consumir, (void *)&param_C);
				
				for(int i=0; i<num_prod; i++){
					pthread_join(producer[i], NULL);
				}
				
				pthread_join(consumer, NULL);
				pthread_mutex_destroy(&mutex);
				pthread_cond_destroy(&not_empty);				
				pthread_cond_destroy(&not_full);
				/*PRUEBAS DE LA COLA
				queue *q=queue_init(atoi(argv[3]));
				printf("%d\n", q->max_size);
				printf("%d\n", q->header);
				printf("%d\n", q->nextPosition);
				printf("%d\n", q->num_elem);
				printf("Cola vacía: %d\n", queue_empty(q));
						printf("Cola llena: %d\n", queue_full(q));
				printf("\n");
				printf("\n");
		
		
				printf("Prueba de elemento\n");
				struct element x;
				x.type=3;
				x.time=10;
				printf("%d\n", x.type);
				printf("%d\n", x.time);
				printf("Prueba de inserción de elemento\n");
				queue_put(q, &x);
				printf("Psición siguinete de insercion: %d\n", q->nextPosition);
				printf("Número de elementos: %d\n", q->num_elem);
				printf("Cola vacía: %d\n", queue_empty(q));
				printf("Cola llena: %d\n", queue_full(q));
		
		
				printf("\n");
				printf("\n");
				printf("Prueba de extracción de elemento\n");
				struct element *y=queue_get(q);
				printf("%d\n", y->type);
				printf("%d\n", y->time);
				printf("Posición de la cabeza: %d\n", q->header);
				printf("Número de elementos: %d\n", q->num_elem);
				printf("Cola vacía: %d\n", queue_empty(q));
				printf("Cola llena: %d\n", queue_full(q));
		
				printf("\n");
				printf("\n");
				printf("Prueba de limpieza de datos\n");
				queue_destroy(q);
				*/
				
		
				/*
				max_size; 		//Tamaño máximo que queremos darle a la cola
				int header;		//Posición donde se encontrará el elemento de la cabeza de la cola
				int nextPosition;	//Posición en la que deberemos realizar la siguiente inserción
				int num_elem;	
				*/
	
				total=param_C[1];
 	   			printf("Total: %i €.\n", total);

    				return 0;
    			}
    			else{
    				perror("Al menos un buffer de tamaño 1");
    				return -1;
    			}
    		}
    		else{
    			perror ("Al menos un productor");
    			return -1;
    		}
    	}
    	
    	else{
    		perror("Error en la instrucción");
    		return -1;
    	}
    	
}
