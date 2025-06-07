# PROJETO PLANTA INDUSTRIAL

Uma planta industrial inteligente realiza medições constantes por meio de sensores distribuídos na linha de produção. As leituras desses sensores são registradas sequencialmente em um arquivo bruto, sem distinção de tipo de sensor nem garantia de ordem temporal.

Com o crescimento do volume de dados, tornou-se necessário implementar um sistema que permita consultas rápidas e precisas com base em instantes específicos de tempo.

Como desenvolvedor de software, você é responsável por implementar 3 programas para este sistema:
<hr/>

## Programa 1 – Organização dos Dados
O primeiro programa deve processar um arquivo contendo diversas leituras no seguinte formato:

**<TIMESTAMP\> <ID_SENSOR\> <VALOR\>**

Onde os campos representam;

- **<TIMESTAMP\>** data e hora da medida no formato unix epoch.
- **<ID_SENSOR\>** string sem espaço que representa cada sensor
- **<VALOR\>** Valor informado do sensor. Cada sensor informa sempre o mesmo tipo de dado. Sensores diferentes podem informar valores de tipos diferentes. O programa deve suportar pelo menos 4 tipos de dados diferentes.

Essas leituras referem-se a diferentes sensores, que aparecem misturados ao longo do arquivo.

Esse programa deverá:

- Tratar o arquivo indicado como argumento da linha de comando de execução do programa.
- Identificar os diferentes sensores presentes, dentro de uma lista de finita de tipos surportatos.
- Separar os dados em arquivos distintos para cada sensor (ex: TEMP.txt, PRES.txt, VIBR.txt).
- Em cada arquivo, ordenar as leituras por timestamp (data e hora representada em unix epoch) em ordem crescente.

A ordenação dos dados dentro de cada arquivo não é opcional: ela é necessária para permitir o uso de busca binária no processo de consulta descrito no Programa 2.

<hr/>

## Programa 2 – Consulta por Instante
O segundo programa deve pesquisar a medida de um sensor especifico mais próxima de uma data e hora informada.

Esse programa deve receber da linha de comanda:

- O nome de um sensor (por exemplo, TEMP)
- Uma data e hora da medida consultada

O programa deve então localizar, usando busca binária, a leitura registrada com timestamp (data e hora representada em unix epoch)  mais próximo daquele instante no arquivo correspondente ao sensor, e exibir as informações da leitura.

A **busca binária** é obrigatória para garantir desempenho adequado em grandes volumes de dados.

<hr/>

## Programa 3 – Geração de Arquivo de Teste
Crie um programa adicional que gere um arquivo de teste com as seguintes características:

- O programa deve receber como argumento da linha de comando:
    - data e hora de início do intervalo de amostras geradas aleatoriamente
    - data e hora de fim do intervalo de amostras  geradas aleatoriamente.
    - nome dos sensores (ex: TEMP, PRES, VIBR, UMID, FLUX).
    - tipo do dado da amostra informada por cada tipo de sensr.
- Para cada sensor, o programa deve criar 2000 leituras aleatórias com valores numéricos aleatórios e timestamps sorteados dentro da data fornecida pelo usuário.

- O arquivo gerado deve seguir o formato: **<TIMESTAMP\> <ID_SENSOR\> <VALOR\>**

    - **<TIMESTAMP\>** data e hora da medida no formato unix epoch.
    - **<ID_SENSOR\>** string sem espaço que representa cada sensor
    - **<VALOR\>** Valor aleatório da amostra de acordo com o tipo indicado para o sensor.