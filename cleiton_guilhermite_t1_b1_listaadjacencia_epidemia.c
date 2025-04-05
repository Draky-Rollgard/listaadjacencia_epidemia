#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
////////////////////////////////////////////////
/*
ORIENTAÇÕES DE ESTADO
0 - SEM MÁSCARA, SEM INFECÇÃO
1 - COM MÁSCARA, SEM INFECÇÃO
2 - COM MÁSCARA, COM INFECÇÃO
3 - SEM MÁSCARA, COM INFECÇÃO

ORIENTAÇÕES DE PROBABILIDADE DE INFECÇÃO
SEM MÁSCARA, SEM INFECÇÃO <-> SEM MÁSCARA, SEM INFECÇÃO == 0%
SEM MÁSCARA, SEM INFECÇÃO <-> COM MÁSCARA, SEM INFECÇÃO == 0%
SEM MÁSCARA, SEM INFECÇÃO <-> COM MÁSCARA, COM INFECÇÃO == 50%
SEM MÁSCARA, SEM INFECÇÃO <-> SEM MÁSCARA, COM INFECÇÃO == 100%

COM MÁSCARA, SEM INFECÇÃO <-> SEM MÁSCARA, SEM INFECÇÃO == 0%
COM MÁSCARA, SEM INFECÇÃO <-> COM MÁSCARA, SEM INFECÇÃO == 0%
COM MÁSCARA, SEM INFECÇÃO <-> COM MÁSCARA, COM INFECÇÃO == 25%
COM MÁSCARA, SEM INFECÇÃO <-> SEM MÁSCARA, COM INFECÇÃO == 50%
*/
//////////////////////////////////////////////PRE-DECLARAÇÕES//////////////////////////////////////////////////////////////
struct INDIVIDUO *criarverticeINDIVIDUO(char v, int estado);
struct GRAFO *criargrafoINDIVIDUOS();
void adicionarrelacao(struct GRAFO *grafo, char origem, int estadoorigem, char destino, int estadodestino);
void imprimirgrafo(struct GRAFO *grafo);
void lerlistadeindividuos(struct GRAFO **grafo, const char *arquivo);
int calculaprobabilidade(int estado1, int estado2);
void gravamaiorprobabilidade(struct GRAFO *grafo, const char *arquivo);
void gravarelacoes(struct GRAFO *grafo, const char *arquivo);
void obter_timestamp(char* buffer, size_t tamanho_buffer);
///////////////////////////////////////////ESTRUTURA DE DADOS/////////////////////////////////////////////////////////////////

struct RELACAO
{
	char id;
	int estado;
	struct RELACAO *proximo;
};

struct INDIVIDUO
{
    char id;
    int estado;
    struct INDIVIDUO *proximo;
		struct RELACAO *relacoes;
};

struct GRAFO
{
    int numeroindividuos;
    struct INDIVIDUO **listadeindividuos;
};

///////////////////////////////////////////CRIAÇÃO DE ESTRUTURA//////////////////////////////////////////////////////////////
struct RELACAO *criarrelacaoINDIVIDUO(char v, int estado)
{
	struct RELACAO *novaRELACAO;

	novaRELACAO = (struct RELACAO *) malloc(sizeof(struct RELACAO));
	novaRELACAO->id = v;
	novaRELACAO->estado = estado;
	novaRELACAO->proximo = NULL;

	return (novaRELACAO);
}
struct INDIVIDUO *criarverticeINDIVIDUO(char v, int estado)
{
    struct INDIVIDUO *novoINDIVIDUO;

    novoINDIVIDUO = (struct INDIVIDUO *) malloc(sizeof(struct INDIVIDUO));
    novoINDIVIDUO->id = v;
    novoINDIVIDUO->estado = estado;
    novoINDIVIDUO->proximo = NULL;
		novoINDIVIDUO->relacoes = NULL;

    return (novoINDIVIDUO);
}

struct GRAFO *criargrafoINDIVIDUOS()
{
    struct GRAFO *grafo;

    grafo = (struct GRAFO *) malloc(sizeof(struct GRAFO));
    grafo->numeroindividuos = 0;
    grafo->listadeindividuos = (struct INDIVIDUO **) malloc(sizeof(struct INDIVIDUO *));
		(*grafo->listadeindividuos) = NULL;
    return(grafo);
}

struct INDIVIDUO *buscaINDIVIDUO(struct GRAFO *grafo, char id)
{
	struct INDIVIDUO *temporario = (*grafo->listadeindividuos);
        
    while (temporario != NULL)
    {
        if (temporario->id == id)
        {
            return temporario;
        }
        temporario = temporario->proximo;
    }
    return NULL; 
}

void adicionarrelacao(struct GRAFO *grafo, char origem, int estadoorigem, char destino, int estadodestino)
{
    struct INDIVIDUO *individuo1 = buscaINDIVIDUO(grafo, origem);

    if (!individuo1)
    {
        individuo1 = criarverticeINDIVIDUO(origem, estadoorigem);
       	individuo1->proximo = (*grafo->listadeindividuos);
				(*grafo->listadeindividuos) = individuo1;
    }

    struct INDIVIDUO *individuo2 = buscaINDIVIDUO(grafo, destino);

    if (!individuo2)
    {
        individuo2 = criarverticeINDIVIDUO(destino, estadodestino);
        individuo2->proximo = (*grafo->listadeindividuos);
				(*grafo->listadeindividuos) = individuo2;
    }

    // aqui vc a adiciona a relacao dos individuos
		// utiliza a funcao criarrelacaoINDIVIDUO
    struct RELACAO *relacao1 = criarrelacaoINDIVIDUO(destino, estadodestino);
    relacao1->proximo = individuo1->relacoes;
    individuo1->relacoes = relacao1;
    
    struct RELACAO *relacao2 = criarrelacaoINDIVIDUO(origem, estadoorigem);
    relacao2->proximo = individuo2->relacoes;
    individuo2->relacoes = relacao2;
}

///////////////////////////////////////////////CÁLCULOS///////////////////////////////////////////////////////////
int calculaprobabilidade(int estado1, int estado2)
{
    
    if (estado1 == 0) // a sofre risco de ser infectado
    {
        if (estado2 == 2)
        {
            return 50;
        }

        if (estado2 == 3) 
        {
            return 100;
        }   
    }
        
    if (estado1 == 1) 
    {
        if (estado2 == 2)
        {
            return 25;
        } 

        if (estado2 == 3)
        {
            return 50;
        }
    }

    if (estado1 == 2 || estado1 == 3)
    {
        return 100; // ambos já estão infectados
    }
    
    if ((estado1 == 0 && estado2 == 1) || (estado1 == 1 && estado2 == 0))
    {
        return 0; // ambos não infectados
    }

    return 0;

}

int maiorprobabilidade(struct INDIVIDUO *individuo) 
{
    int maior = 0;
    struct RELACAO *relacao = individuo->relacoes;

    while (relacao != NULL) 
    {
        int probabilidade = calculaprobabilidade(individuo->estado, relacao->estado);
        if (probabilidade > maior)
        {
            maior = probabilidade;
        }
        
        relacao = relacao->proximo;
    }
    return maior;
}

//////////////////////////////////////////////IMPRESSÃO DE TESTES///////////////////////////////////////////////////////////
void imprimir_probabilidades(struct GRAFO *grafo) 
{
    printf("\n");
    for (int i = 0; i < grafo->numeroindividuos; i++) 
    {
//        int maior_prob = maiorprobabilidade(grafo, i);
//        printf("Individuo %c%d: %d%%\n", i, grafo->listadeindividuos[i]->estado, maior_prob);
    }
}
void imprimirgrafo(struct GRAFO *grafo)
{
    struct INDIVIDUO *temporario1 = *(grafo->listadeindividuos);
		while(temporario1 != NULL)
		{
			struct RELACAO *temporario2 = temporario1->relacoes;
			while(temporario2 != NULL)
			{
				printf("%c -> %c: %d %%\n", temporario1->id, temporario2->id, calculaprobabilidade(temporario1->estado, temporario2->estado));
				temporario2 = temporario2->proximo;
			}
			temporario1 = temporario1->proximo;
		}
}

//////////////////////////////////////////////GERAR TEMPO ////////////////////////////////////////////////////////////////////////////////
void obter_timestamp(char* buffer, size_t tamanho_buffer) 
{
    time_t rawtime;
    struct tm *info;
    
    time(&rawtime);
    info = localtime(&rawtime);
    
    strftime(buffer, tamanho_buffer, "%Y%m%d%H%M%S%z", info);
}
///////////////////////////////////////////////LEITURA E ESCRITA(GRAVAÇÃO)/////////////////////////////////////////////////////////////////
void lerlistadeindividuos(struct GRAFO **grafo, const char *arquivo)
{
    FILE *arq1 = fopen(arquivo, "r");
    char id1, id2;
    int estado1, estado2;
    int total_vertices = 0;

    if(arq1 == NULL)
    {
        printf("Arquivo inexistente\n");
        return;
    }

    // Contagem do número de indivíduos
    while(fscanf(arq1, " %c,%d,%c,%d", &id1, &estado1, &id2, &estado2) != EOF)
    {
        int origem_index = id1 - 'A';
        int destino_index = id2 - 'A';
        if (origem_index >= total_vertices) total_vertices = origem_index + 1;
        if (destino_index >= total_vertices) total_vertices = destino_index + 1;
    }
    fclose(arq1);

    *grafo = criargrafoINDIVIDUOS(total_vertices);

    arq1 = fopen(arquivo, "r");
    while(fscanf(arq1, " %c,%d,%c,%d", &id1, &estado1, &id2, &estado2) != EOF)
    {
        adicionarrelacao(*grafo, id1, estado1, id2, estado2);
    }

    fclose(arq1);
}

void gravamaiorprobabilidade(struct GRAFO *grafo, const char *arquivo_base) 
{
    char timestamp[20];
    obter_timestamp(timestamp, sizeof(timestamp));

    char arquivo[256];
    snprintf(arquivo, sizeof(arquivo), "%s.maior.saida.%s.txt", arquivo_base, timestamp);
    
    FILE *p = fopen(arquivo, "w");
    if (p == NULL) 
    {
        printf("Erro ao abrir arquivo %s\n", arquivo);
        return;
    }

		struct INDIVIDUO *temporario = (*grafo->listadeindividuos);
    while (temporario != NULL) 
    {
        int maior_prob = maiorprobabilidade(temporario);
        fprintf(p, "%c: %d%%\n", temporario->id, maior_prob);
    		temporario = temporario->proximo;
		}

    fclose(p);
}

void gravarelacoes(struct GRAFO *grafo, const char *arquivo_base) 
{
    char timestamp[20];
    obter_timestamp(timestamp, sizeof(timestamp));
    
    char arquivo[256];
    snprintf(arquivo, sizeof(arquivo), "%s.1por1.saida.%s.txt", arquivo_base, timestamp);
    
    FILE *arquivo2 = fopen(arquivo, "w");
    if (arquivo2 == NULL) 
    {
        printf("Erro ao abrir arquivo %s\n", arquivo);
        return;
    }

    struct INDIVIDUO *temporario1 = *(grafo->listadeindividuos);
		while(temporario1 != NULL)
		{
			struct RELACAO *temporario2 = temporario1->relacoes;
			while(temporario2 != NULL)
			{
				fprintf(arquivo2, "%c -> %c: %d %%\n", temporario1->id, temporario2->id, calculaprobabilidade(temporario1->estado, temporario2->estado));
				temporario2 = temporario2->proximo;
			}
			temporario1 = temporario1->proximo;
		}

    fclose(arquivo2);
}

void liberar_memoria(struct GRAFO *grafo)
{
    for(int i = 0; i < grafo->numeroindividuos; i++)
    {
        struct INDIVIDUO *individuo = grafo->listadeindividuos[i];

        while(individuo != NULL)
        {
            struct INDIVIDUO *temp = individuo;
            individuo = individuo->proximo;
            free(temp);
        }
    }

    free(grafo->listadeindividuos);
    free(grafo);
}

////////////////////////////////////////////////////MAIN///////////////////////////////////////////////////////////////////////////////
int main()
{
    struct GRAFO *grafo = NULL;
  
    lerlistadeindividuos(&grafo, "cleiton_guilhermite_t1_b1_listaadjacencia_epidemia.csv");
	//imprimirgrafo(grafo);
    gravamaiorprobabilidade(grafo, "cleiton_guilhermite_t1_b1_listaadjacencia_epidemia");
    gravarelacoes(grafo, "cleiton_guilhermite_t1_b1_listaadjacencia_epidemia");
    
    liberar_memoria(grafo);

    return(0);
}