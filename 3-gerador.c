#include <stdio.h>
#include <time.h>
#include <stdbool.h>
#include <stdlib.h>

#define MIN_PARAMETROS 14

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

typedef struct {
    int dia;
    int mes;
    int ano;
    int hora;
    int minuto;
    int segundo;
} Data_E_Hora;


bool sao_data_e_hora_validos(Data_E_Hora dh);
bool eh_ano_bissexto(int ano);
bool eh_intervalo_valido(time_t ts_inicial, time_t ts_final);
time_t converter_para_timestamp(Data_E_Hora dh);

int main(int argc, char* argv[]) {
    if (argc < 12) {
        puts(" > ERRO: Quantidade invalida de parametros");
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
        puts(" > ERRO: Data ou hora inicial invalida");
        return -1;
    }
    if (sao_data_e_hora_validos(dh_final) != true) {
        puts(" > ERRO: Data ou hora final invalida");
        return -1;
    }

    time_t timestamp_inicial = converter_para_timestamp(dh_inicial);
    time_t timestamp_final = converter_para_timestamp(dh_final);

    if (!eh_intervalo_valido(timestamp_inicial, timestamp_final)) {
        puts(" > ERRO: A data final nao pode ser antes da data inicial");
        return -1;
    }

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

