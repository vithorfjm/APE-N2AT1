#include <stdio.h>
#include <time.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#define TAMANHO_BUFFER 65536
#define MAX_TAMANHO_NOME_SENSOR 4
#define MAX_SENSORES 10

#define MAX_LEN_ARQUIVO 30

typedef union {
    int inteiro;
    float flutuante;
    double duplo;
    bool booleano;
} Valor;

typedef struct {
    time_t timestamp;
    char id_sensor[5];
    Valor valor;
} Sensor;

int contar_linhas_arquivo(FILE* arq);
int contar_sensores_distintos(FILE* arq, int qntd_linhas);

int main(int argc, char* argv[]) {
    if (argc != 2) {
        puts(" > ERRO: Informe na linha de comando o nome do arquivo que deve ser lido.");
        return -1;
    }
    if (strlen(argv[1]) > MAX_LEN_ARQUIVO) {
        printf(" > ERRO: O nome do arquivo deve ter no maximo %d caracteres.", MAX_LEN_ARQUIVO);
        return -1;
    }
    char nome_do_arquivo[MAX_LEN_ARQUIVO];
    strcpy(nome_do_arquivo, argv[1]);
    FILE * arquivo = fopen(nome_do_arquivo, "r");
    if (arquivo == NULL) {
        printf(" > ERRO: Falha ao abrir arquivo.");
        return -1;
    }
    int qntd_linhas = contar_linhas_arquivo(arquivo);
    int qntd_sensores_distintos = contar_sensores_distintos(arquivo, qntd_linhas);
    
    fclose(arquivo);
}

int contar_linhas_arquivo(FILE* arq) {
    char buffer[TAMANHO_BUFFER];
    int contador = 0;
    for(;;) {
        size_t res = fread(buffer, 1, TAMANHO_BUFFER, arq);
        if (ferror(arq)) {
            puts(" > ERRO: Falha ao ler arquivo.");
            exit(-1);
        }

        for(int i = 0; i < res; i++)
            if (buffer[i] == '\n')
                contador++;

        if (feof(arq))
            break;
    }

    return contador;
}

int contar_sensores_distintos(FILE* arq, int qntd_linhas) {
    rewind(arq);
    char sensores_distintos[MAX_SENSORES][MAX_LEN_ARQUIVO];
    int qntd_sensores_distintos = 0;
    char nome_sensor[5];
    int timestamp; 
    float valor[10];
    char linha[100];
    for (int i = 0 ; i < qntd_linhas ; i++) {
        fgets(linha, sizeof(linha), arq);
        int qntd_campos_na_linha = sscanf(linha, "%d %4s %f", &timestamp, nome_sensor, &valor);
        if (qntd_campos_na_linha != 3) {
            printf(" > ERRO: A linha %d do arquivo esta mal formatada.", i+1);
            exit(-1);
        }
        
        bool eh_novo = true;

        for (int j = 0 ; j < qntd_sensores_distintos ; j++) {
            if (strcmp(nome_sensor, sensores_distintos[j]) == 0) {
                eh_novo = false;
                break;
            }
        }
        if (eh_novo) {
            strcpy(sensores_distintos[qntd_sensores_distintos], nome_sensor);
            qntd_sensores_distintos++;
        }

    }
    return qntd_sensores_distintos;
}





