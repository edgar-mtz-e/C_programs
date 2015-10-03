#include <stdio.h>
#include <stdlib.h>
#define BUFFER_tam 100
//tama¤o maximo por linea

int espacioBlanco(char car);
void LineasCodigo(FILE *entrada,FILE *salida);

int main ()
{
  FILE  *Entrada;
  FILE  *respaldo;
  char nomArchivo[20];
  char buf[BUFFER_tam];

   printf("Proporcione el nombre del archivo con su extension\nEj. prueba.c \n");
   printf("Nombre del archivo>");
   scanf("%s", nomArchivo);

    /* Verifica que el archivo se pueda abrir */
    if (! (Entrada = fopen(nomArchivo, "rb")) )
    {
     printf("ERROR: No se puede abrir el archivo %s\n", nomArchivo);
     return 0;
    }

    /* Abrir el archivo de entrada y salida */
    Entrada  = fopen(nomArchivo, "r");
    respaldo = fopen("respaldo.c", "w");
/*Imprimimos las lineas de codigo del archivo Entrada al archivo respaldo*/
    LineasCodigo(Entrada,respaldo);
    fclose(Entrada);
    fclose(respaldo);

return 0;
}


// Dice si el caracter es un espacio
int espacioBlanco(char car)
{
 return car <= 32; //el 32 es espacio en ASCCI
}
// Lee las lineas de codigo de un archivo
void LineasCodigo(FILE *entrada,FILE *salida)
{
    char buf[BUFFER_tam];
    int tam;
//esLineaCodigo: bandera para saber si es linea de codigo
    int i; //contador
    char n;
    // Variable Bandera para saber si es un comentario de varias lineas
    int comentarios = 0; //ponemos la bandera en falso
    // Leer cada linea una a una hasta encontrar una que sea linea de codigo

  while (!feof(entrada) && fgets(buf,sizeof buf, entrada) == buf)
  {
   // Ignorar los espacios en blanco en una linea
   for (i = 0; i < tam && buf[i] && espacioBlanco(buf[i]); i++);
	// analizar la linea hasta que no tenga espacios en blanco
	for (; i < tam && buf[i]; i++)
	{
	// hacemos n igual al siguiente caracter si todavia no nos salimos del rango
	  if (i + 1 < tam)
		  n = buf[i + 1];
	// Aun no nos encontramos con comentarios de varias lineas
	    if (!comentarios)
	    {
		// Puede ser un comentario
		if (buf[i] == '/')
		{
		//  tenemos "//" termina la linea
			if (n == '/')
			      break;
		// comentarios de varias lineas comienzan con "/*"
			if (n == '*')
			{
			 i++;
			 comentarios=1; //ponemos la bandera en verdadero
			}
		 // Si el caracter no es espacio en blanco esta linea se cuenta
			else if (!espacioBlanco(buf[i]))
			{
		      //	 esLineaCodigo = 1; //la bandera esta en verdadero esta es una linea de codigo
			 fprintf(salida,"%c",buf[i]);
			}
		}//fin   if (buf[i] == '/')

	       // vemos si esta es una linea
	       else if (!espacioBlanco(buf[i]))
			{
		    //	esLineaCodigo = 1;//la bandera esta en verdadero esta es una linea de codigo
			if(buf[i+1]==32)
			fprintf(salida,"%c ",buf[i]);
			else
			fprintf(salida,"%c",buf[i]);
			}
	    }//fin if (!comentarios)

	  // comentario de varias lineas
	  else
	  {
	  // buscamos hasta donde termine
	     if (buf[i] == '*' && n == '/')
	     {
	       i++;
	       comentarios = 0;//la bandera esta en falso
	      }//fin if
	  }//fin else
	}//fin for (; i < tam && buf[i]; i++)

 /*Ponemos esta condicion para que no imprima el caracter '/'
       cuando tenemos el tipo de comentario '//'en lugar no imprimimos ''*/
    if(buf[i]=='/'&&buf[i+1]=='/')
      {
       fprintf(salida,"\n");
//imprimimos '\n' para que las lineas con '//' sean separadas
      }
    else //es linea de codigo
    {
    fprintf(salida,"%c\n",buf[i]);
    }
  }//fin while
}// fincuentaLineas
