#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <math.h>
#include <time.h>

typedef struct t_nodo
{
	char *clave;
	int encriptado;
	int total_de_palabras;
	struct t_nodo *sgte;
}Nodo;

void creartablahash(Nodo *TablaHash[],char *nombre_archivo,int tamanio_tabla, int porcentaje_tabla);
void inicializar(Nodo *tabla[],int tamanio_tabla);
Nodo *creaNodo(char *clave,int encriptado);
void insertar_dato(Nodo *tabla[],char *palabra,int tamanio_tabla);
void mostrar_tabla(Nodo *tabla[], int tamanio_tabla);
int radix(char *palabra);


unsigned int chti(char letra){
	if(letra == 0xF1){
		return 14;
	}

	return((letra - 'a') + 1);
}

int valid_identifier_start(char ch)
{
    return ((ch >= 'A' && ch <= 'Z') || (ch >= 'a' && ch <= 'z') ||
            ((unsigned char)ch >= 0xC0));
}

void procesar_palabra(char *palabra){
	int largo_palabra,i;
	largo_palabra = strlen(palabra);

	for(i = 0; i < largo_palabra; i++){
		if(!valid_identifier_start(palabra[i])){
			palabra[i] = 'o';
		}
	}
}

int potencia(int base, int exponente){
	int i,resultado = 1;

	for(i = 0; i < exponente; i++){
		resultado *= base;
	}

	return(resultado);
}

int radix(char *palabra){
	int i,exponente=0,valor,resultado = 0, parada;
	int largo_palabra = strlen(palabra);

	if(largo_palabra > 10){
		parada = largo_palabra - 10;
	}else{
		parada = 0;
	}

	for(i = largo_palabra; i > parada; i--){
		if(valid_identifier_start(palabra[i])){
				valor = chti(palabra[i]);
				if(valor < 0){
					valor = 27; //si no pertenece a las 26 letras del alfabeto
				}
				resultado += valor*potencia(27,exponente);// |x|*(b**i)
				exponente++;
		}
	}
	return(resultado);
}

void inicializar_tabla(Nodo *tabla[], int tamanio_tabla)
{
		int i;
		for(i=0;i<tamanio_tabla;i++)
		{
				tabla[i] = NULL;
		}
		printf("Inicio con exito\n");
}

//Función propuesta por Brian Kernighan y Dennis Ritchie
unsigned int string_hash(char *palabra,int tamanio_tabla){
	int i,len;
	unsigned int h = 0;
	len = strlen(palabra);

	for(i=0; i < len; i++){
		h = h*131 + *(palabra +  i);
	}

	return(h%tamanio_tabla);
}

//Funcion propuesta por Robert Sedgwicks
unsigned int RSHash(char *palabra, int tamanio_tabla){
	unsigned int b = 378551;
	unsigned int a = 63689;
	unsigned int hash = 0;

	int i,len;
	len = strlen(palabra);
	for(i = 0; i < len; i++)
	{
		hash = hash * a + *(palabra + i);
		a = a * b;
	}
	return (hash%tamanio_tabla);
}

Nodo *creaNodo(char *clave,int encriptado)
{
		int len = strlen(clave);
		Nodo *aux = (Nodo *)malloc(sizeof(Nodo));
		aux->clave = (char *)malloc(sizeof(char)*(len+1));
		aux->encriptado = encriptado;
		strcpy(aux->clave,clave);
		aux->sgte = NULL;

		return aux;
}

int calculo_porcentaje_tabla(Nodo *tabla[],int tamanio_tabla, double porcentaje_de_uso){
	int i,cantidad_palabras;
	double porcentaje_actual;

	for(i = 0; i < tamanio_tabla;i++){
		if(tabla[i] != NULL){
			cantidad_palabras++;
		}
	}

	porcentaje_actual = cantidad_palabras/100.0;
	if(porcentaje_actual <= porcentaje_de_uso){
		return 1;
	}
	return 0;
}

void insertar_dato(Nodo *tabla[],char *palabra,int tamanio_tabla)
{
	int i,h,encriptado,t = 0,m_prima;
	Nodo *aux1;

	m_prima = tamanio_tabla-1000;
	h = (string_hash(palabra,tamanio_tabla) + (RSHash(palabra,m_prima) * t)) % tamanio_tabla;
	printf("palabra: %s -- h: %d \n",palabra,h);
	int continuar=1;
	while(continuar){
		if(tabla[h] != NULL){
			t++;
			h = (string_hash(palabra,tamanio_tabla) + (RSHash(palabra,m_prima) * t)) % tamanio_tabla;
			printf("t: %d -- h: %d \n",t,h);

		}else{
			continuar = 0;
		}
	}

	encriptado = radix(palabra);
	aux1 = creaNodo(palabra,encriptado);
	tabla[h] = aux1;

}

void mostrar_tabla(Nodo *tabla[], int tamanio_tabla)
{
		int i;
		Nodo *aux;
		printf("Tabla Hash\n-----------------\n");
		for(i=0;i<tamanio_tabla;i++)
		{
			aux = tabla[i];
			printf("%d]: ",i);
			while(aux)
			{
				printf("(%s) -> ",aux->clave);
				aux = aux->sgte;
			}
			printf("\n\n");
		}
}

void borrar_tabla_hash(Nodo *TablaHash[], int tamanio_tabla){
     int i;

     for(i=0;i<tamanio_tabla;i++){
			 	Nodo *nodo;
   			while(TablaHash[i]) {
      		nodo = TablaHash[i];
      		TablaHash[i] = nodo->sgte;
      		free(nodo);
   			}
				TablaHash[i] = NULL;
     }
}

void leer_archivo(Nodo *tabla[],char *nombre_archivo,int tamanio_tabla, double porcentaje_de_uso){
	FILE *archivo;
	char *buffer;
	archivo = fopen(nombre_archivo,"r");

	while(!feof(archivo)){
		buffer = (char *)malloc(sizeof(char)*50);
		fscanf(archivo,"%s",buffer);
		if(calculo_porcentaje_tabla(tabla,tamanio_tabla,porcentaje_de_uso)){
			printf("Inserta dato en tabla\n");
			insertar_dato(tabla,buffer,tamanio_tabla);
		}
		free(buffer);
	}
	fclose(archivo);
}

void escribir_archivo(Nodo *tabla[], int tamanio_tabla){
	FILE *archivo;

	archivo = fopen("tabla_hash.txt","w");

	int i,j=0;
	Nodo *aux;
	fprintf(archivo,"Tabla Hash\n-----------------\n");
	for(i=0;i<tamanio_tabla;i++)
	{
		aux = tabla[i];
		fprintf(archivo,"%d]: ",i);
		while(aux)
		{
			fprintf(archivo,"({%d}=>%s:%d) -> ",j,aux->clave,aux->encriptado);
			aux = aux->sgte;
			j++;
		}
		fprintf(archivo,"\n\n");
		j=0;
	}

	fclose(archivo);
}

void cantidad_palabras_por_lista(Nodo *tabla[],int tamanio_tabla){
	int cantidad_palabras = 0;
	int i;
	Nodo *aux;

	for(i=0;i<tamanio_tabla;i++)
	{
		aux = tabla[i];
		while(aux) {
			cantidad_palabras++;
			aux = aux->sgte;
		}
		tabla[i]->total_de_palabras = cantidad_palabras;
		cantidad_palabras = 0;
	}

	free(aux);
}

void maximo_palabras(Nodo *tabla[], int tamanio_tabla){
	int maximo = 0;
	int i;

	for(i=0;i<tamanio_tabla;i++)
	{
		if (maximo < tabla[i]->total_de_palabras) {
			maximo = tabla[i]->total_de_palabras;
		}
	}

	printf("Maximo de palabras: %d\n",maximo);
}

void minimo_palabras(Nodo *tabla[], int tamanio_tabla){
	int minimo = 999999;
	int i;

	for(i=0;i<tamanio_tabla;i++)
	{
		if (minimo > tabla[i]->total_de_palabras) {
			minimo = tabla[i]->total_de_palabras;
		}
	}

	printf("Minimo de palabras: %d\n",minimo);
}

void promedio_palabras(Nodo *tabla[], int tamanio_tabla){
	float promedio;
	int suma = 0,i;

	for(i=0;i<tamanio_tabla;i++)
	{
		suma += tabla[i]->total_de_palabras;
	}

	promedio = suma/tamanio_tabla;
	printf("Promedio de palabras: %.1f\n",promedio);
}

void buscar_palabra(char *palabra, Nodo *tabla[], int tamanio_tabla){

	int salida = 1,posicion=0,encriptado,h;
	Nodo *aux;
	double tiempo_transcurrido;
	clock_t inicio,fin;

	encriptado = radix(palabra);
	h = string_hash(palabra,tamanio_tabla);
	inicio = clock();

	aux = tabla[h];
	while(aux) {
		if(encriptado != aux->encriptado){
			aux = aux->sgte;
			posicion++;
		}else{
			fin = clock();
			printf("Encontrado <%s> en el indice [%d] de la tabla en la posicion (%d) de la lista.\n",aux->clave,h,posicion);
			tiempo_transcurrido = (double)(fin-inicio)/CLOCKS_PER_SEC;
			printf("Tiempo transcurrido en CPU: %f\n", tiempo_transcurrido);
			salida = 0;
			break;
		}
	}

	if(salida == 1){
		printf("No se encontro <%s> en la tabla.\n",palabra);
	}

	free(aux);
}

int obtener_tamanio_tabla(char *nombre_archivo){
	FILE *archivo;
	char *buffer;
	int cantidad_palabras = 0;
	archivo = fopen(nombre_archivo,"r");

	while(!feof(archivo)){
		buffer = (char *)malloc(sizeof(char)*50);
		fscanf(archivo,"%s",buffer);
		cantidad_palabras++;
		free(buffer);
	}
	fclose(archivo);

	return(cantidad_palabras);
}

void creartablahash(Nodo *TablaHash[],char *nombre_archivo,int tamanio_tabla, int porcentaje_tabla){
	int total_palabras=0,i;
	double porcentaje_de_uso;

	porcentaje_de_uso = tamanio_tabla * (porcentaje_tabla/100.0);

	inicializar_tabla(TablaHash, tamanio_tabla);//inicializa la tabla en NULL

	leer_archivo(TablaHash,nombre_archivo,tamanio_tabla,porcentaje_de_uso);//se crea la tabla a partir del archivo
	cantidad_palabras_por_lista(TablaHash,tamanio_tabla);//cada indice de la tabla tiene la cantidad de palabras

	for(i = 0; i < tamanio_tabla; i++){
		total_palabras += TablaHash[i]->total_de_palabras;
	}

	printf("Cantidad de palabras procesadas:  %d \n",total_palabras);
}
