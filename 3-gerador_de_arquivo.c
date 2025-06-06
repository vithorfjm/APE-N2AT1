#include <stdio.h>
#include <time.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#define MIN_PARAMETROS 14
#define POSICAO_INICIAL_SENSORES 13

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

#define FEV 2
#define ABR 4
#define JUN 6
#define SET 9
#define NOV 11

#define MIN_RAND_INT 10
#define MAX_RAND_INT 1000

#define QNTD_REGISTROS 2000

typedef struct {
    int dia;
    int mes;
    int ano;
    int hora;
    int minuto;
    int segundo;
} Data_E_Hora;

typedef struct {
    char nome_sensor[5];
    char tipo[7];
} Sensor;

bool sao_data_e_hora_validos(Data_E_Hora dh);
bool eh_ano_bissexto(int ano);
bool eh_intervalo_valido(time_t ts_inicial, time_t ts_final);
time_t converter_para_timestamp(Data_E_Hora dh);
time_t gerar_timestamp_aleatorio(time_t timestamp_inicial, time_t timestamp_final);
void escrever_valor_aleatorio(FILE *arq, time_t ts, char* sensor, char* tipo_sensor);

int main(int argc, char* argv[]) {
    srand(time(0));
    if (argc < MIN_PARAMETROS) {
        puts(" > ERRO: Quantidade invalida de parametros.");
        return -1;
    }

    Data_E_Hora dh_inicial = {
        .dia = atoi(argv[1]),
        .mes = atoi(argv[2]),
        .ano = atoi(argv[3]),
        .hora = atoi(argv[4]),
        .minuto = atoi(argv[5]),
        .segundo = atoi(argv[6])
    };

    Data_E_Hora dh_final = {
        .dia = atoi(argv[7]),
        .mes = atoi(argv[8]),
        .ano = atoi(argv[9]),
        .hora = atoi(argv[10]),
        .minuto = atoi(argv[11]),
        .segundo = atoi(argv[12])
    };

    if (sao_data_e_hora_validos(dh_inicial) != true) {
        puts(" > ERRO: Data ou hora inicial invalida.");
        return -1;
    }
    if (sao_data_e_hora_validos(dh_final) != true) {
        puts(" > ERRO: Data ou hora final invalida.");
        return -1;
    }

    time_t timestamp_inicial = converter_para_timestamp(dh_inicial);
    time_t timestamp_final = converter_para_timestamp(dh_final);

    if (!eh_intervalo_valido(timestamp_inicial, timestamp_final)) {
        puts(" > ERRO: A data final nao pode ser antes da data inicial.");
        return -1;
    }

    int qntd_sensores_e_tipos = argc - POSICAO_INICIAL_SENSORES;
    if (qntd_sensores_e_tipos%2 != 0) {
        puts(" > ERRO: Cada sensor deve ter um tipo correspondente.");
        return -1;
    }
    int qntd_sensores = qntd_sensores_e_tipos / 2;
    Sensor sensores[qntd_sensores];
    for (int i = 0 ; i < qntd_sensores ; i++) {
        strncpy(sensores[i].nome_sensor, argv[POSICAO_INICIAL_SENSORES + 2*i], 4);
        sensores[i].nome_sensor[4] = '\0';
        strncpy(sensores[i].tipo, argv[POSICAO_INICIAL_SENSORES + 2*i + 1], 6);
        sensores[i].tipo[6] = '\0';
        if (strcmp(sensores[i].tipo, "FLOAT") != 0 &&
            strcmp(sensores[i].tipo, "DOUBLE") != 0 &&
            strcmp(sensores[i].tipo, "BOOL") != 0 &&
            strcmp(sensores[i].tipo, "INT") != 0) {
            printf(" > ERRO: O tipo %s eh invalido. Utilize FLOAT, DOUBLE, INT ou BOOL.\n", sensores[i].tipo);
            exit(-1);
        }
    }

    puts("GERANDO ARQUIVO DE TESTE...");
    FILE *arquivo;
    arquivo = fopen("registro.txt", "a");
    if (arquivo == NULL) {
        puts(" > ERRO: Falha ao criar o arquivo.");
        return -1;
    }

    for (int i = 0 ; i < qntd_sensores ; i++) {
        for (int j = 0 ; j < QNTD_REGISTROS ; j++ ) {
            time_t ts_aleatorio = gerar_timestamp_aleatorio(timestamp_inicial, timestamp_final);
            char* nome_sensor = sensores[i].nome_sensor;
            escrever_valor_aleatorio(arquivo, ts_aleatorio, nome_sensor, sensores[i].tipo);
        }
    }
    fclose(arquivo);
    printf("Arquivo gerado com sucesso. %d dados foram inseridos", QNTD_REGISTROS * qntd_sensores);
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

bool eh_intervalo_valido(time_t ts_inicial, time_t ts_final) {
    if (ts_inicial > ts_final) return false;
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

time_t gerar_timestamp_aleatorio(time_t timestamp_inicial, time_t timestamp_final) {
    time_t timestamp_aleatorio = timestamp_inicial + rand() % (timestamp_final - timestamp_inicial + 1);
    return timestamp_aleatorio;
}

void escrever_valor_aleatorio(FILE *arq, time_t ts, char* sensor, char* tipo_sensor) {
    fprintf(arq, "%d %s ", ts, sensor);
    
    if (strcmp(tipo_sensor, "FLOAT") == 0) {
        float valor = (float)(rand() % 1000) / 10.0;
        fprintf(arq, "%.2f\n", valor);
        return;
    }
    if (strcmp(tipo_sensor, "DOUBLE") == 0) {
        double valor = (double)(rand() % 1000) / 10.0;
        fprintf(arq, "%.3lf\n", valor);
        return;
    }
    if (strcmp(tipo_sensor, "INT") == 0) {
        int valor = rand() % (MAX_RAND_INT - MIN_RAND_INT + 1) + MIN_RAND_INT;
        fprintf(arq, "%d\n", valor);
        return;
    }
    if (strcmp(tipo_sensor, "BOOL") == 0) {
        int valor = rand() % 2;
        fprintf(arq, "%d\n", valor);
        return;
    }
    printf(" > ERRO: O tipo %s eh invalido. Utilize FLOAT, DOUBLE ou INT.\n", tipo_sensor);
    exit(-1);
}