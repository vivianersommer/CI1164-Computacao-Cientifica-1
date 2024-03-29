//VIVIANE DE ROSA SOMMER GRR20182564
//VINICIUS OLIVEIRA DOS SANTOS GRR20182592

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "utils.h"
#include <inttypes.h>
#include <assert.h>
#include <matheval.h>

#define MAX 100 //maximo de caracteres para um nome de arquivo de saida

int main (int argc, char **argv){

    //ler dados de sistemas.dat -------------------------------------------------------------------------------------------------------------
    FILE *arq, *arq2;
    
    arq = stdin;

    char* output = malloc(MAX * sizeof(char));
    output=malloc(MAX * sizeof(char)); // reservo espaço para um nome de ate 100 letras
    output = le_nome(argc, argv);

    if (output == NULL){
		arq2 = stdout; //caso nao tenha sido passado um nome, pegue da saida padrao
    } else{
        arq2 = fopen(output, "w"); //Crio arquivo
    }

    confere(arq, arq2);

    int cont_bag = 0;
    while(!feof(arq)){
        bag *b = malloc(sizeof(bag)); //declaracao de ponteiro para a estrutura contendo variaveis de acordo com formato proposto

        b->ttotal = 0;
        b->tjacobiana = 0;
        b->tsl = 0;
        b->tderivadas = 0;

        b->ttotal = timestamp();


        // b->max_eq ------------------------------------------------------------------------------------------------------------------------
        char max_eq[MAX];
        fgets(max_eq, MAX, arq); 
        clean_fgets(max_eq);
        b->max_eq = atoi(max_eq);
        // ----------------------------------------------------------------------------------------------------------------------------------

        // b->eq ----------------------------------------------------------------------------------------------------------------------------
        b->eq = malloc (b->max_eq * sizeof(char*)); 
        for (int i=0; i<b->max_eq; i++){
            b->eq[i] = malloc(MAX * sizeof(char));
        }

        for(int i=0; i<=b->max_eq -1; i++){
            // b->eq[i] = malloc(sizeof(500)); //crio vetor auxiliar para ir recebendo por linha as funcoes dadas no dat
            fgets(b->eq[i], MAX, arq);
            char ch;

            //para garantir que vai ser puxada uma linha nao vazia garantindo q nao venha lixo
            if(strlen(b->eq[i]) > 0){
                ch = b->eq[i][0];
            }


            // analiza se foi feita a leitura de string inválida, confere se nao eh "\r" ou "\n"
            if(b->eq[i] == NULL || ch == 13 || ch==10){ 
                fgets(b->eq[i], MAX, arq);
            }

        }
        // -------------------------------------------------------------------------------------------------------------------------------------


        // b->x0 -------------------------------------------------------------------------------------------------------------------------------
        b->x0 = malloc((b->max_eq) * sizeof(double)); 
        char* x0 = malloc(MAX * sizeof(char));

        fgets(x0, MAX, arq);

        if(x0 == NULL){
            fgets(x0, MAX, arq);
        }

        char **tokens;
        int count, i;
        const char *str = x0;

        count = split(str, ' ', &tokens);
        for (i = 0; i < count; i++){
            b->x0[i] = atof(tokens[i]);
        }
        
        // -------------------------------------------------------------------------------------------------------------------------------------

        // b->epsilon --------------------------------------------------------------------------------------------------------------------------
        char ep[30];
        fgets(ep, 30, arq);
        b->epsilon = atof(ep);
        // -------------------------------------------------------------------------------------------------------------------------------------

        // b->max_iter -------------------------------------------------------------------------------------------------------------------------
        char max_iter[MAX];
        fgets(max_iter, MAX, arq); //ler do arquivo dat maximo de equacoes possiveis
        clean_fgets(max_iter);
        b->max_iter = atoi(max_iter);
        // -------------------------------------------------------------------------------------------------------------------------------------

         //"falhando graciosamente"
        if(!b->x0 || !b->eq){
            fprintf(stderr, "Erro ao alocar variáveis\n");
        }

        // Prints ------------------------------------------------------------------------------------------------------------------------------
        fprintf(arq2,"%d\n", b->max_eq);
        void *f;
        for(int i = 0; i < b->max_eq; i++){
            clean_fgets(b->eq[i]);
            f = evaluator_create(b->eq[i]);
            assert(f);
            fprintf(arq2,"%s = 0\n", evaluator_get_string(f));
        }
        evaluator_destroy(f);
        // -------------------------------------------------------------------------------------------------------------------------------------

        // Método de Newton. -------------------------------------------------------------------------------------------------------------------
        newton(b, arq2, cont_bag);
        
        
        b->ttotal = timestamp() - b->ttotal;

        // -------------------------------------------------------------------------------------------------------------------------------------
        // Print tempos ------------------------------------------------------------------------------------------------------------------------
        fprintf(arq2, "###########\n");
        fprintf(arq2, "# Tempo Total: %f\n", b->ttotal);
        fprintf(arq2, "# Tempo Derivadas: %f\n", b->tderivadas);
        fprintf(arq2, "# Tempo Jacobiana: %f\n", b->tjacobiana);
        fprintf(arq2, "# Tempo SL: %f\n", b->tsl);   
        fprintf(arq2, "###########\n");   
        fprintf(arq2, "\n");     
        // -------------------------------------------------------------------------------------------------------------------------------------

        fgetc(arq); // ler \n que sobrou

        //free em todo mundo
        for(int c=0; c<b->max_eq; c++)
            free(b->eq[c]);
        free(b);
        free(x0);
        cont_bag++; 
    }
    // -----------------------------------------------------------------------------------------------------------------------------------------

    //fechar arquivos
    fclose(arq);
    fclose(arq2);
}
