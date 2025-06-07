#include <stdio.h>
#include <time.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#define MAX_LEN_NOME_ARQUIVO 9
#define MAX_LEN_NOME_SENSOR 5
#define MAX_LEN_STR_VALOR 10

#define MIN_DIA 1
#define MAX_DIAS_MES_31 31
#define MAX_DIAS_MES_30 30
#define MAX_DIAS_FEV 28
#define MAX_DIAS_FEV_BISSEXTO 29
#define MIN_MES 1
#define MAX_MES 12
#define MIN_ANO 2000
#define MAX_ANO 2025

#define MIN_HORA 0
#define MAX_HORA 23
#define MIN_MINUTO 0
#define MAX_MINUTO 59
#define MIN_SEG 0
#define MAX_SEG 59

#define TAMANHO_BUFFER 65536

#define FEV 2
#define ABR 4
#define JUN 6
#define SET 9
#define NOV 11

typedef struct {
    int dia;
    int mes;
    int ano;
    int hora;
    int minuto;
    int segundo;
} Data_E_Hora;

typedef struct {
    time_t timestamp;
    char id_sensor[MAX_LEN_NOME_SENSOR];
    char str_valor[MAX_LEN_STR_VALOR];
} Sensor;

bool sao_data_e_hora_validos(Data_E_Hora dh);
bool eh_ano_bissexto(int ano);
time_t converter_para_timestamp(Data_E_Hora dh);
int contar_linhas_arquivo(FILE* arq);
void armazenar_dados_arquivo(FILE* arq, int qntd_linhas, Sensor lista[]);
Sensor* busca_binaria(Sensor sensores[], int qntd_linhas, time_t alvo);

int main(int argc, char* argv[]) {
    if (argc != 8) {
        puts(" > ERRO: Informe na linha de comando o nome de um sensor e uma data e hora.");
        return -1;
    }

    Data_E_Hora data_hora = {
        .dia = atoi(argv[2]),
        .mes = atoi(argv[3]),
        .ano = atoi(argv[4]),
        .hora = atoi(argv[5]),
        .minuto = atoi(argv[6]),
        .segundo = atoi(argv[7])
    };

    if (!sao_data_e_hora_validos(data_hora)) {
        puts(" > ERRO: Data ou hora invalida.");
        return -1;
    }
    time_t input_timestamp = converter_para_timestamp(data_hora);
    char nome_arquivo[9];
    strcpy(nome_arquivo, argv[1]);
    strcat(nome_arquivo, ".txt");

    FILE *arq = fopen(nome_arquivo, "r");
    if (arq == NULL) {
        printf(" > ERRO: Nao existe um arquivo registrados do sensor %s.", argv[1]);
        return -1;
    }
    int qntd_linhas = contar_linhas_arquivo(arq);
    Sensor lista_sensores[qntd_linhas];
    armazenar_dados_arquivo(arq, qntd_linhas, lista_sensores);
    fclose(arq);
    Sensor *sensor_encontrado = busca_binaria(lista_sensores, qntd_linhas, input_timestamp);
    printf("[ ! ] A data informada eh %d em timestamp\n", input_timestamp);
    printf("[ ! ] Registro encontrado:\n");
    printf("TIMESTAMP\tSENSOR\tVALOR\n");
    printf("%d\t%s\t%s", sensor_encontrado->timestamp, sensor_encontrado->id_sensor, sensor_encontrado->str_valor);
    
}

bool sao_data_e_hora_validos(Data_E_Hora dh) {

    // VALIDACAO DATA
    if (dh.mes < MIN_MES || dh.mes > MAX_MES) return false;

    if (dh.dia < MIN_DIA || dh.dia > MAX_DIAS_MES_31) return false;

    if (dh.ano < MIN_ANO || dh.ano > MAX_ANO) return false;

    if (dh.mes == FEV) {
        if (eh_ano_bissexto(dh.ano)) {
            if (dh.dia > MAX_DIAS_FEV_BISSEXTO) return false;
        } else {
            if (dh.dia > MAX_DIAS_FEV) return false;
        }
    }

    // meses com 30 dias
    if ((dh.mes == ABR || dh.mes == JUN || dh.mes == SET || dh.mes == NOV) && dh.dia > MAX_DIAS_MES_30) return false;

    // VALIDACAO HORA
    if (dh.hora < MIN_HORA || dh.hora > MAX_HORA) return false;
    if (dh.minuto < MIN_MINUTO || dh.minuto > MAX_MINUTO) return false;
    if (dh.segundo < MIN_SEG || dh.segundo > MAX_SEG) return false;

    return true;
}

bool eh_ano_bissexto(int ano) {
    if (ano % 400 == 0) return true;
    if (ano % 100 == 0) return false;
    if (ano % 4 == 0) return true;
    return false;
}

time_t converter_para_timestamp(Data_E_Hora dh) {
    struct tm t;

    t.tm_year = dh.ano - 1900;
    t.tm_mon = dh.mes - 1;
    t.tm_mday = dh.dia;
    t.tm_hour = dh.hora;
    t.tm_min = dh.minuto;
    t.tm_sec = dh.segundo;
    t.tm_isdst = -1;

    time_t timestamp = mktime(&t);
    if (timestamp == -1) {
        printf("Data inválida. Tente novamente.\n");
    } else {
        return timestamp;
    }
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

void armazenar_dados_arquivo(FILE* arq, int qntd_linhas, Sensor lista[]) {
    rewind(arq);
    char linha[100];
    time_t timestamp;
    char nome_sensor[MAX_LEN_NOME_SENSOR];
    char str_valor[MAX_LEN_STR_VALOR];
    
    for (int i = 0 ; i < qntd_linhas ; i++) {
        fgets(linha, sizeof(linha), arq);
        int qntd_campos_na_linha = sscanf(linha, "%d %s %s", &timestamp, nome_sensor, str_valor);
        if (qntd_campos_na_linha != 3) {
            printf(" > ERRO: A linha %d do arquivo esta mal formatada.", i+1);
            exit(-1);
        }
        Sensor sensor;
        sensor.timestamp = timestamp;
        strcpy(sensor.id_sensor, nome_sensor);
        strcpy(sensor.str_valor, str_valor);
        lista[i] = sensor;
    }   
}

Sensor* busca_binaria(Sensor sensores[], int qntd_linhas, time_t alvo) {
    int esq = 0;
    int dir = qntd_linhas - 1;
    int melhor_indice = -1;
    long menor_diferenca = 1000000000;

    while (esq <= dir) {
        int meio = (esq + dir) / 2;
        time_t atual = sensores[meio].timestamp;
        long diferenca = labs(atual - alvo);

        if (diferenca < menor_diferenca) {
            menor_diferenca = diferenca;
            melhor_indice = meio;
        }
        if ((long)atual > (long)alvo) {
            dir = meio - 1;
        } else if ((long)atual < (long)alvo) {
            esq = meio + 1;
        } else {
            return &sensores[meio];
        }
    }

    if (melhor_indice != -1) {
        return &sensores[melhor_indice];
    }
    return NULL;
}
