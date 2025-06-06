#include <stdio.h>
#include <time.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#define TAMANHO_BUFFER 65536
#define MAX_TAMANHO_NOME_SENSOR 4
#define MAX_SENSORES 10

#define MAX_LEN_NOME_ARQUIVO 30

typedef enum {
    INTEIRO,
    FLUTUANTE,
    DUPLO,
    BOOLEANO
} TipoDeDado;

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
    TipoDeDado tipo;
} Sensor;

int contar_linhas_arquivo(FILE* arq);
int processar_entrada(FILE* arq, int qntd_linhas, Sensor sensores[]);
Sensor definir_tipo_e_valor(char* tipo, Sensor sensor);
void ordenar_sensores(Sensor sensores[], int qnt_registros);
int comparar_timestamp(const void *a, const void *b);

int main(int argc, char* argv[]) {
    if (argc != 2) {
        puts(" > ERRO: Informe na linha de comando o nome do arquivo que deve ser lido.");
        return -1;
    }
    if (strlen(argv[1]) > MAX_LEN_NOME_ARQUIVO) {
        printf(" > ERRO: O nome do arquivo deve ter no maximo %d caracteres.", MAX_LEN_NOME_ARQUIVO);
        return -1;
    }
    char nome_do_arquivo[MAX_LEN_NOME_ARQUIVO];
    strcpy(nome_do_arquivo, argv[1]);
    FILE * arquivo = fopen(nome_do_arquivo, "r");
    if (arquivo == NULL) {
        printf(" > ERRO: Falha ao abrir arquivo.");
        return -1;
    }

    
    int qntd_linhas = contar_linhas_arquivo(arquivo);
    Sensor sensores[qntd_linhas];
    int qntd_sensores_distintos = processar_entrada(arquivo, qntd_linhas, sensores);
    fclose(arquivo);
    ordenar_sensores(sensores, qntd_linhas);


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

int processar_entrada(FILE* arq, int qntd_linhas, Sensor sensores[]) {
    rewind(arq);
    char sensores_distintos[MAX_SENSORES][MAX_LEN_NOME_ARQUIVO];
    int qntd_sensores_distintos = 0;
    char nome_sensor[5];
    time_t timestamp; 
    char str_valor[10];
    char linha[100];
    for (int i = 0 ; i < qntd_linhas ; i++) {
        fgets(linha, sizeof(linha), arq);
        int qntd_campos_na_linha = sscanf(linha, "%d %4s %s", &timestamp, nome_sensor, str_valor);

        if (qntd_campos_na_linha != 3) {
            printf(" > ERRO: A linha %d do arquivo esta mal formatada.", i+1);
            exit(-1);
        }

        strcpy(sensores[i].id_sensor, nome_sensor);
        sensores[i].timestamp = timestamp;
        sensores[i] = definir_tipo_e_valor(str_valor, sensores[i]);
        
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

Sensor definir_tipo_e_valor(char* str_valor, Sensor sensor) {
    if (strcmp(str_valor, "1") == 0 || strcmp(str_valor,"0") == 0) {
        sensor.tipo = BOOLEANO;
        sensor.valor.booleano = strcmp(str_valor, "1") == 0 ? true : false;
        return sensor;
    }

    for (int i = 0 ; i < strlen(str_valor) ; i++) {
        if (str_valor[i] == '.') {
            if (strlen(str_valor) - i-1 <= 2) {
                sensor.tipo = FLUTUANTE;
                float valor = atof(str_valor);
                sensor.valor.flutuante = valor;
                return sensor;
            }
            if (strlen(str_valor) - i-1 >= 3) {
                sensor.tipo = DUPLO;
                char *end;
                double valor = strtod(str_valor, &end);
                sensor.valor.flutuante = valor;
                return sensor;
            }
            else {
                printf(" > ERRO: Falha ao converter ponto flutuante '%s'.", str_valor);
                exit(-1);
            }
        }
    }
    char *endptr;
    long temp = strtol(str_valor, &endptr, 10);
    if (*endptr == '\0') {
        int valor =(int)temp;
        sensor.tipo = INTEIRO;
        sensor.valor.inteiro = valor;
        return sensor;
    }
    printf(" > ERRO: O valor '%s' nao pode ser convertido.\n", str_valor);
    exit(-1);
}

void ordenar_sensores(Sensor sensores[], int qnt_registros) {
    qsort(sensores, qnt_registros, sizeof(Sensor), comparar_timestamp);
}

int comparar_timestamp(const void *a, const void *b) {
    Sensor *sensor_a = (Sensor *)a;
    Sensor *sensor_b = (Sensor *)b;

    if (sensor_a->timestamp < sensor_b->timestamp) return -1;
    if (sensor_a->timestamp > sensor_b->timestamp) return 1;
    return 0;
}
