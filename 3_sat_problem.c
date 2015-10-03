/*Use test files*/
#include <stdlib.h>   /*para malloc ,free ,abs */
#include <stdio.h>
#include <conio.h>
#define NUMVARS        20 /*50*/   /* # max de variables */
#define NUMCLAUSULAS   50 /*250*/ /* # max de clausulas */
#define MAXITERACIONES 150 /*250*/ /* # max de clausulas */
/*poblacion es una estructura*/
typedef struct
{
	int tampoblacion;   /* # de individuos por poblacion */
	int bpi;        	/* # bits por individuo */
	int **bits;  		/* la poblacion (tam x arreglo bpi)  */
	int *aptitud;   	/* arrgelo de aptitudes */
	int *es_evaluado; 	/* (bandera) arreglo de valores booleanos */
	int **bits2;		/* espacio para mezclar la nueva poblacion */
	int *aptitud2;  	/* mas espacio para mezclar */
	int *es_evaluado2;	/* mas espacio para mezaclar*/
	int *aptitud_acumulada;/* aptitud acumulada*/
}poblacion;

int numvars;        /* # de variables archivo*/
int numclausulas;  /* # variables archivo*/
/*arreglo que guarda la formula que evaluaremos, para ver si es satisfacible*/
int formula[NUMCLAUSULAS][3];
/* (x1 + x3 + -x4)*(x4)*(x2 + -x3)
La representamos en el arreglo como
1  3 -4
4  0  0
2 -3  0
0  0  0
...    (lo demas son ceros, llenar matriz hasta el NUMCLAUSULAS)
*/
int tampoblacion  = 60;         /* # de individuos por poblacion*/
int bpi           = NUMVARS;   /* # de bits por individuo, bpi debe ser mas grande o igual al numero de variables numvars */
int iter          = 100; /* 10000;*/   /* # de iteraciones */
float cruzamiento = 0.90;     /* probabilidad cruzamiento*/
float mutacion    = 0.0001;   /* probabilidad mutacion*/
/*************************prototipos del algoritmo genetico**************************/
/*cruzamiento por corte*/
/*crea una poblacion al azar, y reserva la memoria necesaria*/
poblacion *alloc_poblacion(int tampoblacion, int bpi);
/*libera la memoria*/
void libera_poblacion(poblacion *p);
/*Hace la evaluacion,seleccion,cruzamiento,mutacion */
void evolucionar_poblacion(poblacion *p,float proba_cruzamiento,float proba_mutacion);
/*Imprime toda la poblacion*/
void imprime_poblacion(poblacion *p);
/*Aptitud promedio*/
int aptitud_promedio(poblacion *p);
/*Mejor Aptitud*/
int mejor_aptitud(poblacion *p);
/*Mejor individuo*/
int *mejor_individuo(poblacion *p);
/*Mejor indice*/
int mejor_indice(poblacion *p);
/*Selecciona un indice al azar un índice de acuerdo a la Aptitud*/
int selecciona_bits(poblacion *p);
/********************************prototipos del problema*****************************/
/*evaluar*/
int evaluar(int *bits, int bpi);
/*Aptitud de la solucion optima*/
int aptitud_optimo();
/*Leer archivo con el problema, recibe como parametro el nombre del archivo*/
void iniciar(char* nombre_archivo);

/********************************funciones****************************************/
//para el dev-c++ shed o mingw32
int random(int num)
{
  return rand()&num-1;
}
/*evaluar */
int evaluar(int *bits, int bpi)/* la funcion no usa bpi, se asume que bpi >= numvars */
{
	int aptitud,i,j,var,signo;
	/* los bits son una asignacion a las variables booleanas de la formula*/
	/* Aptitud= # de clausulas que son satisfacibles */
	aptitud = 0;
	for (i = 0; i < numclausulas; i++)
	{
		if (formula[i][0] == 0) /* una clausla que esta vacia es Verdad*/
		{
			aptitud++;
		}
		else
		{
			for (j = 0; j < 3; j++)
			{
				var = formula[i][j]; /*Si la formula termina,salir del ciclo*/
				if (var == 0)
				{
					break;
				}
				signo = (var > 0);   //signo toma cero o uno dependiendo si var > 0
				if (signo == 0)
				{
					var = -var; /* var debe ser positiva,el -1 es porque 0 no es una variable en la formula*/
				}
				var = var - 1;
				if (bits[var] == signo)/*La clausula es satisfacible */
				{
					aptitud++;
					break;
				}
			}
		}
	}//fin for externo
	return aptitud;
}
//fin evaluar

/*Aptitud de la solucion optima,Si todas las clauslas leidas son satisfacibles, alcanzamos el optimo*/
int aptitud_optimo()
{
	return numclausulas;
}
/*Leer archivo*/
void leer_archivo(char* nombre_archivo)
{
	FILE* f;   /*apuntador al archivo*/
	char str[255];
	int i,j,k;
	f = fopen(nombre_archivo, "r");
	if (f == 0)
	{
		printf("No se puede abrir el archivo %s\n", nombre_archivo);
		exit(0);
	}
	i = 0;   /*contador de clausulas*/
	while (!feof(f))
	{
		if (i >= NUMCLAUSULAS)/*espacio suficiente?*/
		{
			printf("error al leer el archivo\n");
			break;
		}
		/*mientras no sea fin de archivo leemos linea por linea*/
		fgets(str,255,f);
		switch (str[0])
		{
			case 'p': /*Leer el # de variables*/
				/*la primer linea del archivo debe ser algo como  str="p sat 40" */
				numvars = atoi(str+5);
				if ((numvars < 0) || (NUMVARS < numvars))
				{
					printf("error,# de variables en archivo excede el limite\n");
					getch();
				}
				break;
			case 'c': /* Comentario*/
				break;
			case '0': case '1': case '2': case '3': case '4':
			case '5': case '6': case '7': case '8': case '9':
			case '-':
				j = 0;
				k = 0;
				while ((str[k] != '\n') &&  (str[k] != '\0'))
				{
					formula[i][j] = atoi(str+k);/*asignar un entero*/
					j++;
					k++;
					/*encontrar espacio en blanco*/
					while ((str[k] != '\n') &&  (str[k] != '\0') && (str[k] != ' '))
					{
						k++;
					}
				}
				/* rellenar lo demas con ceros*/
				while (j < 3)
				{
					formula[i][j++] = 0;
				}
				i++;  /*siguiente clausula, incrementamos contador*/
				break;
			default:
				break; /*cualquier otra cosa ignoramos la linea */
		}
	str[0] = '\0';  /* limpiamos lacadena*/
	}//fin while

	numclausulas = i;
	while (i < NUMCLAUSULAS) /* rellenar el resto con ceros*/
	{
		j = 0;
		while (j < 3 && i < NUMCLAUSULAS)
		{
			formula[i++][j++] = 0;   //checar incrementos
		}
	}
	fclose(f);/* cerrar archivo*/
}

/*Inicia el problema, abriendo el archivo con la funcion leer_archivo*/
void iniciar(char* nombre_archivo)
{
	leer_archivo(nombre_archivo);
}

/*funciones del algortimo genetico*/
/*evaluar toda la poblacion*/
void haz_evaluacion(poblacion *p)
{
	int i;
	for (i = 0; i < p->tampoblacion; i++) /* evaluar cada cadena*/
	{
		if (p->es_evaluado[i] == 0)   //bandera  indica si ya fue evaluado
		{
			p->aptitud[i] = evaluar(p->bits[i],p->bpi);
			p->es_evaluado[i] = 1;
		}
	}
}
/*crea una poblacion al azar, y reserva la memoria necesaria*/
poblacion *alloc_poblacion(int tampoblacion, int bpi)
{
	poblacion *p;
	int i;
	int j;
	p = (poblacion *) malloc(sizeof(poblacion));
	if (p == 0)
    {
		libera_poblacion(p);
		return 0;
	}
	p->bits = (int **) malloc(tampoblacion*sizeof(int *));
	if (p->bits == 0)
    {
		libera_poblacion(p);
		return 0;
	}
	for (i = 0; i < tampoblacion; i++)
	{
		p->bits[i] = (int *) malloc(bpi*sizeof(int));
		if (p->bits[i] == 0)
		{
			libera_poblacion(p);
		return 0;
		}
	}
	p->aptitud = (int *) malloc(tampoblacion*sizeof(int));
	if (p->aptitud == 0)
	{
		libera_poblacion(p);
		return 0;
	}
	p->es_evaluado = (int *) malloc(tampoblacion*sizeof(int));
	if (p->es_evaluado == 0)
    {
		libera_poblacion(p);
		return 0;
	}
	p->bits2 = (int **) malloc(tampoblacion*sizeof(int *));
	if (p->bits2 == 0)
    {
		libera_poblacion(p);
		return 0;
	}
	for (i = 0; i < tampoblacion; i++)
	{
		p->bits2[i] = (int *) malloc(bpi*sizeof(int));
		if (p->bits2[i] == 0)
		{
			libera_poblacion(p);
			return 0;
		}
	}
	p->aptitud2 = (int *) malloc(tampoblacion*sizeof(int));
	if (p->aptitud2 == 0)
	{
		libera_poblacion(p);
		return 0;
	}
	p->es_evaluado2 = (int *) malloc(tampoblacion*sizeof(int));
	if (p->es_evaluado2 == 0)
	{
		libera_poblacion(p);
		return 0;
    }
	p->aptitud_acumulada = (int *) malloc(tampoblacion*sizeof(int));
	if (p->aptitud_acumulada == 0)
  	{
		libera_poblacion(p);
		return 0;
    }
	p->tampoblacion = tampoblacion;
	p->bpi  = bpi;
	for (i = 0; i < tampoblacion; i++) /*crea una poblacion aleatoria*/
	{
		p->es_evaluado[i] = 0;
		for (j = 0; j < bpi; j++)
		p->bits[i][j] = random(2);//random(2);//&01;  /* bit aleatorio */
	}
	/* evaluar la poblacion*/
	haz_evaluacion(p);
	return p;
}


/* libera memoria*/
void libera_poblacion(poblacion *p)
{
	int i;
	if (p != 0)
	{
		if (p->bits != 0)
		{
			for (i = 0; i < p->tampoblacion; i++)
			{
				if (p->bits[i] != 0)
				free(p->bits[i]);
			}
			free(p->bits);
		}
		if (p->aptitud != 0)
			free(p->aptitud);
		if (p->es_evaluado != 0)
			free(p->es_evaluado);
		if (p->aptitud2 != 0)
			free(p->aptitud2);
		if (p->es_evaluado2 != 0)
			free(p->es_evaluado2);
		if (p->bits2 != 0)
		{
			for (i = 0; i < p->tampoblacion; i++)
			{
				if (p->bits2[i] != 0)
					free(p->bits2[i]);
			}
			free(p->bits2);
		}
		if (p->aptitud_acumulada != 0)
			free(p->aptitud_acumulada);
		free(p);
	}
}//fin libera_poblacion

/*Selecciona un indice al azar un índice de acuerdo a la Aptitud,requiere que p->aptitud_acumulada este llena con # cada ves mayores*/
int selecciona_bits(poblacion *p)
{
	int r;
	int i;
	/* p->aptitud_acumulada[p->tampoblacion-1] contiene la suma de todas las Aptitudes*/
    r = random(numvars) % (p->aptitud_acumulada[p->tampoblacion - 1]); //()
	/* buscar el indice i tal que r es menor que aptitud_acumulada[i] y
	mayor o igual a aptitud_acumulada[i-1] (si i>0) */
	i = 0;
	while (p->aptitud_acumulada[i] <= r){
		i++;
	}
	/*verificar*/
	if (i < p->tampoblacion)
		return i;
	/*no debe llegar a este error*/
	printf("No se pudo hacer la Seleccion de bits\n");
	exit(1);
	return 0;
}

/*Hace la seleccion*/
void selecciona(poblacion *p)
{
	int i,j,s;
	int **bits3;
	int *aptitud3,*es_evaluado3;
	/* asumir que toda la poblacion fue evaluada*/
	/* tener en cuenta el mejor indice hasta ahora*/
	s = mejor_indice(p);
	/* Aptitud aculumalda*/
	p->aptitud_acumulada[0] = p->aptitud[0];
	for (i = 1; i < p->tampoblacion; i++)
	{
		p->aptitud_acumulada[i] = p->aptitud[i] + p->aptitud_acumulada[i-1];
	}
	/* intercambiamos bits y bits2 */
	bits3    = p->bits2;
	p->bits2 = p->bits;
	p->bits  = bits3;
	/* intercambiar aptitud y evaluado */
	aptitud3 = p->aptitud2;
	p->aptitud2 = p->aptitud;
	p->aptitud = aptitud3;
	es_evaluado3 = p->es_evaluado2;
	p->es_evaluado2 = p->es_evaluado;
	p->es_evaluado = es_evaluado3;
	/* nos aseguramos que el mejor individuo sobreviva poniendolo al final de la poblacion*/
	for (j = 0; j < p->bpi; j++)
	{
		p->bits[p->tampoblacion-1][j] = p->bits2[s][j];
	}
	p->aptitud[p->tampoblacion-1] = p->aptitud2[s];
	p->es_evaluado[p->tampoblacion-1] = p->es_evaluado2[s];
	/*Como el mejor esta al final el siguiente ciclo tiene -1 */
	for (i = 0; i < p->tampoblacion-1; i++)
	{
		/*selecciona_bits escoge una cadena deacuerdo a su Aptitud*/
		s = selecciona_bits(p);
		for (j = 0; j < p->bpi; j++)
		{
			p->bits[i][j] = p->bits2[s][j];
		}
		p->aptitud[i] = p->aptitud2[s];
		p->es_evaluado[i] = p->es_evaluado2[s];
	}
}

/* cruzamiento escoge una posicion y corta la cadena en 2 cadenas y las mezcla*/
void cruza(poblacion *p,float proba_cruzamiento)
{
	int i,j,cruzando,abit;
	float    aux;
	/* asumimos q las  cadenas de bits estan mezcladas, emparejamos indices vecinos
	(indice i con i+1 */
	aux = (proba_cruzamiento * p->tampoblacion)-1;
	//aux=floor(aux);
	for (i = 0; i < (int)aux; i=i+2)
	{
		/*escoger un punto de cruzamiento*/
		cruzando = (random(numvars) % (p->bpi - 1)) + 1; //()
		/* intercambiar las cadenas hasta ese punto*/
		for (j = 0; j < cruzando; j++)
		{
			abit = p->bits[i][j];
			p->bits[i][j] = p->bits[i+1][j];
			p->bits[i+1][j] = abit;
		}
		/* evaluamos a los dos individuos*/
		p->es_evaluado[i] = 0;
		p->es_evaluado[i+1] = 0;
	}
}

/* si la proba de mutacion es 0.01 cambiamos al azar el 1% de todos las cadenas de bits
     de toda la poblacion */
void muta(poblacion *p,float proba_mutacion)
{
	int i,m,n;
    float aux;
	aux = p->tampoblacion * p->bpi * proba_mutacion;
	//aux=floor(aux);
	for (i = 0; i < (int)aux; i++)
	{
		/*escoger una cadena al azar y una posicion al azar*/
		m = random(numvars) % (p->tampoblacion);              //()
		n = random(numvars) % (p->bpi);               //()
		/* cambiar la posicion */
		p->bits[m][n] = 1 - p->bits[m][n];
		/* reevaluamos las cadenas mutadas*/
		p->es_evaluado[m] = 0;
	}
}



/* hace una evaluaion, seleccion, cruzamiento y mutacion */
void evolucionar_poblacion(poblacion *p, float proba_cruzamiento,float proba_mutacion)
{
	/* assumes the whole poblacion has been es_evaluado */
	if ((proba_cruzamiento < 0) || (proba_cruzamiento > 1) || (proba_mutacion < 0) || (proba_mutacion > 1))
    {
		printf("Probabilidades de cruzamiento y mutacion deben estar entre [0,1]\n");
		exit(1);
	}
	cruza(p,proba_cruzamiento);
	muta(p,proba_mutacion);
	haz_evaluacion(p);
}

/* imprime la poblacion   individuo  aptitud  evaluado  aptitud acumulada*/
void imprime_poblacion(poblacion *p)
{
	int i,j;
	for (i = 0; i < p->tampoblacion; i++)
	{
		for (j = 0; j < p->bpi; j++)
		{
			printf("%i", p->bits[i][j]);
		}
		printf(" %i",p->aptitud[i]);
		printf(" %i",p->es_evaluado[i]);
		printf(" %i\n",p->aptitud_acumulada[i]);
	}
}

/* aptitud promedio, evaluamos y despues sacamos el promedio */
int aptitud_promedio(poblacion *p)
{
	int i,sum;
    haz_evaluacion(p);
	sum = 0;
	for (i = 0; i < p->tampoblacion; i++)
	{
       sum += p->aptitud[i];
	}
	return (int) sum / p->tampoblacion;
}

/* mejor aptitud */
int mejor_aptitud(poblacion *p)
{
	return p->aptitud[mejor_indice(p)];
}

/* el mejor individuo */
int *mejor_individuo(poblacion *p)
{
	return p->bits[mejor_indice(p)];
}

/* Mejor indice*/
int mejor_indice(poblacion *p)
{
	int i,el_mejor;
	/* no sabemos cual sea el mejor, entonces evaluamos primero y luego  escogemos*/
	haz_evaluacion(p);
	el_mejor = 0;  /* iniciamos el_mejor a cero y recorremos tampoblacion */
	for (i = 1; i < p->tampoblacion; i++)
	{
		if (p->aptitud[el_mejor] < p->aptitud[i])
			el_mejor = i;
	}
	return el_mejor;
}


/*****************************PRINCIPAL******************************/
int main()
{
	int i;
	poblacion *p;
	char nombre_archivo[255];
	printf("Nombre>");
	scanf("%s",nombre_archivo);
	/* iniciar problema*/
	iniciar(nombre_archivo);
	/* crea una poblacion nueva */
	p = alloc_poblacion(tampoblacion,bpi);
    for (i = 0; i < iter; i++)
	{
		imprime_poblacion(p);
		printf("\n");
		printf("Generacion = %i mejor hasta ahora= %i promedio hasta ahora = %i\n",i,mejor_aptitud(p),aptitud_promedio(p));
		if  (mejor_aptitud(p)>=numclausulas)
        {
			printf("\nProblema Resuelto!!!\n");
			if( mejor_aptitud(p) < numclausulas)
				printf("Formula con alguna Contradiccion?\n");
			break;
        }
        else
		{
			evolucionar_poblacion(p,cruzamiento,mutacion);
			if ( iter == MAXITERACIONES && (mejor_aptitud(p) < aptitud_optimo() ) )
				printf("\nProblema Indeterminado\n");

		}
    }
	libera_poblacion(p);
	printf("\nNumero de Generaciones: %i\nTama¤o de la Poblacion %i\n",i,tampoblacion);
	printf("Numero de Iteraciones: %i\nProbabilidad Cruzamiento %f\n", iter, cruzamiento);
	printf("Probabilidad Mutacion %f\n",mutacion);
	printf("\nNumero de variables en Archivo %i\n",numvars);
	printf("Numero de clausulas en Archivo %i\n",numclausulas);
	getch();
	return 0;
}
