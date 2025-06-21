# 📊 Projeto de Ordenação Externa com K-Way Merge Sort em C++

![Linguagem](https://img.shields.io/badge/Linguagem-C%2B%2B-blue.svg)
![Plataforma](https://img.shields.io/badge/Plataforma-Terminal-brightgreen.svg)
![Status](https://img.shields.io/badge/Status-Conclu%C3%ADdo-sucesso)

Este projeto, desenvolvido como parte da disciplina de Estrutura de Dados, implementa um algoritmo de **ordenação externa (External Merge Sort)** para processar e ordenar um grande volume de dados que não caberia na memória RAM. O programa utiliza a técnica de **intercalação de k-vias (K-Way Merge)** e, após a ordenação, oferece um menu interativo para a manipulação dos registros.

O conjunto de dados utilizado como base é o *"Property Transfer Statistics - March 2022 Quarter"* da Nova Zelândia, contido no arquivo `property-transfer-statistics-march-2022-quarter-csv.csv`.

---

## 🚀 Conceitos e Tecnologias

Este projeto foi construído utilizando C++ e explora conceitos fundamentais de manipulação de arquivos e algoritmos eficientes.

-   **Ordenação Externa (External Merge Sort):** O núcleo do projeto. O algoritmo é projetado para ordenar dados que são grandes demais para a memória principal. Ele funciona em duas fases principais:
    1.  **Fase de Distribuição:** O arquivo de entrada grande é lido e seus registros são distribuídos em `k` arquivos temporários menores.
    2.  **Fase de Intercalação (Merge):** Os `k` arquivos são lidos simultaneamente, e seus registros são intercalados de forma ordenada em um novo conjunto de arquivos. Este processo é repetido, com os blocos de dados ordenados crescendo exponencialmente a cada passada, até que reste um único arquivo completamente ordenado.

-   **Manipulação de Arquivos Binários:** Para otimizar a performance de I/O (leitura/escrita), todos os arquivos temporários e o arquivo de trabalho principal (`dados.bin`) são binários. Os registros são armazenados no formato `[tamanho][dados]`, onde `tamanho` é um `int` de 4 bytes que precede os dados do registro em si.

-   **Interface de Linha de Comando (CLI):** O programa possui um menu interativo que permite ao usuário realizar operações no arquivo de dados.

---

## ✨ Funcionalidades

O programa oferece um menu completo para interagir com o arquivo de dados:

1.  **Converter para Binário:** Na inicialização, o programa converte o arquivo `.csv` original para um formato binário (`dados.bin`), que é usado para todas as outras operações.
2.  **Ordenar Arquivo de Dados:** Permite ao usuário definir o número de "caminhos" (`k`) e iniciar o processo de ordenação externa. O arquivo `dados.bin` é sobrescrito com a versão ordenada.
3.  **Adicionar Registro:** Insere um novo registro em uma posição específica do arquivo.
4.  **Visualizar Registros:** Exibe na tela os registros contidos em um intervalo de posições.
5.  **Alterar Registro:** Modifica os dados de um registro em uma posição específica.
6.  **Trocar Registros:** Troca a posição de dois registros no arquivo.
7.  **Imprimir Todos os Registros:** Lista todos os registros contidos no arquivo binário.

---

## ⚙️ Como Compilar e Executar

Para compilar e rodar este projeto, você precisará de um compilador C++, como o `g++`.

### Pré-requisitos
-   Compilador `g++` (ou outro compilador C++ moderno).
-   O arquivo de dados `property-transfer-statistics-march-2022-quarter-csv.csv` na mesma pasta do código-fonte.

### Passos para Execução

1.  **Crie a pasta de arquivos temporários:**
    ```bash
    mkdir temp
    ```

2.  **Compile o código-fonte:**
    Abra o terminal na pasta do projeto e execute o seguinte comando. Isso criará um arquivo executável chamado `meu_programa`.
    ```bash
    g++ seu_arquivo.cpp -o meu_programa
    ```
    *(Substitua `seu_arquivo.cpp` pelo nome real do seu arquivo C++)*

3.  **Execute o programa:**
    ```bash
    ./meu_programa
    ```

4.  **Siga as instruções do menu:**
    -   O programa irá primeiro converter o `.csv` para `dados.bin`.
    -   A partir do menu, você pode escolher a opção de ordenar o arquivo ou realizar outras manipulações.

---

## 📁 Estrutura do Projeto


.
├── property-transfer-statistics-march-2022-quarter-csv.csv  (Arquivo de entrada original)
├── mergecompleto.cpp                                          (Seu código-fonte)
└── temp/                                                    (Pasta para arquivos temporários da ordenação)


**Arquivos gerados durante a execução:**
-   `dados.bin`: Arquivo de trabalho binário.
-   `dados_ordenado.csv`: Versão CSV do arquivo após a ordenação (para fácil visualização).
-   `temp/F*.bin`, `temp/S*.bin`: Arquivos temporários usados durante as passadas de intercalação.
-   `temp_*.bin`: Arquivos temporários usados pelas funções de manipulação do menu.

---

## 👤 Autores

**João Vitor Reis Alvarenga, Giovane Godoi Oliveira, Caio Teixeira**

-   **GitHub:** (https://github.com/giogodoi)
-   **LinkedIn:** (https://www.linkedin.com/in/giovane-felipe-godoi-oliveira-9a477a190/))

*Projeto desenvolvido em Junho de 2025 para a disciplina de Estrutura de Dados na Universidade Federal de Lavras (UFLA).*
