#include <iostream>
#include <fstream>
#include <chrono>
#include <cstdio>

using namespace std;

// --- FUNÇÕES AUXILIARES E DE MENU ---

void limparBufferCin() {
    cin.ignore(10000, '\n'); 
}

bool ehNumero(const string& str, bool permitePonto) {
    if (str.empty()) {
        return false;
    }
    bool jaTemPonto = !permitePonto;
    for (char const &c : str) {
        if (isdigit(c)) continue;
        if (!jaTemPonto && c == '.') {
            jaTemPonto = true;
            continue;
        }
        return false;
    }
    return true;
}

string lerNovoRegistro() {
    string campos[14];
    string titulos[] = {
        "Series_reference", "Period", "Data_value", "Status", "Units",
        "Magnitude", "Subject", "Periodicity", "Group", "Series_title_1",
        "Series_title_2", "Series_title_3", "Series_title_4", "Series_title_5"
    };

    cout << "\n--- Inserindo/Alterando Registro ---\n";
    limparBufferCin();

    for (int i = 0; i < 14; ++i) {
        cout << titulos[i] << ": ";
        getline(cin, campos[i]);

        if (i == 1) { // Period
            while (!ehNumero(campos[i], true)) {
                cout << "Entrada invalida. Digite um numero (ex: 2018.12): ";
                getline(cin, campos[i]);
            }
        } else if (i == 2 || i == 5) { // Data_value e Magnitude
            while (!ehNumero(campos[i], false)) {
                cout << "Entrada invalida. Digite apenas numeros inteiros: ";
                getline(cin, campos[i]);
            }
        }
    }

    string linha;
    for (int i = 0; i < 14; ++i) {
        linha += campos[i] + (i == 13 ? "" : ",");
    }
    return linha;
}

void imprimirTodosRegistros() {
    ifstream arq("dados.bin", ios::binary);
    if (!arq.is_open()) {
        cout << "ERRO: O arquivo 'dados.bin' nao foi encontrado. Tente reiniciar o programa.\n";
    } else {
        cout << "\n--- Conteudo do Arquivo Binario ---\n";
        int pos = 0;
        int len;
        while (arq.read(reinterpret_cast<char*>(&len), sizeof(int))) {
            string linha(len, '\0');
            arq.read(&linha[0], len);
            cout << "Posicao " << pos++ << ": " << linha << endl;
        }

        if (pos == 0) {
            cout << "O arquivo esta vazio.\n";
        }
        arq.close();
    }
}

void visualizarRegistros() {
    ifstream arqCheck("dados.bin");
    if (!arqCheck.is_open()) {
        cout << "ERRO: O arquivo 'dados.bin' nao foi encontrado. Tente reiniciar o programa.\n";
    } else {
        arqCheck.close();
        int inicio, fim;
        cout << "Digite a posicao inicial: ";
        cin >> inicio;
        cout << "Digite a posicao final: ";
        cin >> fim;

        if (inicio < 0 || fim < inicio) {
            cout << "Erro: intervalo de posicoes invalido.\n";
        } else {
            ifstream arq("dados.bin", ios::binary);
            cout << "\n--- Visualizando Registros de " << inicio << " a " << fim << " ---\n";
            int pos = 0;
            int len;
            bool encontrou = false;
            while (arq.read(reinterpret_cast<char*>(&len), sizeof(int))) {
                if (pos > fim) break;
                string linha(len, '\0');
                arq.read(&linha[0], len);
                if (pos >= inicio) {
                    cout << "Posicao " << pos << ": " << linha << endl;
                    encontrou = true;
                }
                pos++;
            }
            if(!encontrou) {
                cout << "Nenhum registro encontrado nesse intervalo.\n";
            }
            arq.close();
        }
    }
}

void adicionarRegistro() {
    int pos_inserir;
    cout << "Digite a posicao onde deseja inserir o novo registro: ";
    cin >> pos_inserir;

    if (pos_inserir < 0) {
        cout << "Erro: posicao invalida.\n";
    } else {
        string novo_registro = lerNovoRegistro();
        int len_novo = novo_registro.length();

        ifstream arq_orig("dados.bin", ios::binary);
        if (!arq_orig.is_open()) {
             cout << "ERRO: O arquivo 'dados.bin' nao foi encontrado. Tente reiniciar o programa.\n";
        } else {
            ofstream arq_temp("temp_add.bin", ios::binary | ios::trunc);
            int pos_atual = 0;
            bool inserido = false;
            int len;
            
            while (arq_orig.read(reinterpret_cast<char*>(&len), sizeof(int))) {
                if (pos_atual == pos_inserir) {
                    arq_temp.write(reinterpret_cast<const char*>(&len_novo), sizeof(int));
                    arq_temp.write(novo_registro.c_str(), len_novo);
                    inserido = true;
                }
                string linha(len, '\0');
                arq_orig.read(&linha[0], len);
                arq_temp.write(reinterpret_cast<const char*>(&len), sizeof(int));
                arq_temp.write(linha.c_str(), len);
                pos_atual++;
            }

            if (!inserido && pos_atual == pos_inserir) {
                arq_temp.write(reinterpret_cast<const char*>(&len_novo), sizeof(int));
                arq_temp.write(novo_registro.c_str(), len_novo);
                inserido = true;
            }
            
            if (inserido) {
                cout << "Registro adicionado com sucesso!\n";
            } else {
                cout << "Aviso: a posicao de insercao e maior que o final do arquivo. O elemento foi adicionado no final.\n";
                arq_temp.write(reinterpret_cast<const char*>(&len_novo), sizeof(int));
                arq_temp.write(novo_registro.c_str(), len_novo);
            }

            arq_orig.close();
            arq_temp.close();

            remove("dados.bin");
            rename("temp_add.bin", "dados.bin");
        }
    }
}

void alterarRegistro() {
    int pos_alterar;
    cout << "Digite a posicao do registro que deseja alterar: ";
    cin >> pos_alterar;

    if (pos_alterar < 0) {
        cout << "Erro: posicao invalida.\n";
    } else {
        ifstream arq_orig("dados.bin", ios::binary);
        if (!arq_orig.is_open()) {
            cout << "ERRO: O arquivo 'dados.bin' nao foi encontrado. Tente reiniciar o programa.\n";
        } else {
            ofstream arq_temp("temp_alt.bin", ios::binary | ios::trunc);
            int pos_atual = 0;
            bool alterado = false;
            int len;

            while (arq_orig.read(reinterpret_cast<char*>(&len), sizeof(int))) {
                string linha(len, '\0');
                arq_orig.read(&linha[0], len);

                if (pos_atual == pos_alterar) {
                    cout << "Registro atual: " << linha << endl;
                    string novo_registro = lerNovoRegistro();
                    int len_novo = novo_registro.length();
                    arq_temp.write(reinterpret_cast<const char*>(&len_novo), sizeof(int));
                    arq_temp.write(novo_registro.c_str(), len_novo);
                    alterado = true;
                } else {
                    arq_temp.write(reinterpret_cast<const char*>(&len), sizeof(int));
                    arq_temp.write(linha.c_str(), len);
                }
                pos_atual++;
            }

            arq_orig.close();
            arq_temp.close();

            if (!alterado) {
                cout << "Erro: posicao " << pos_alterar << " nao encontrada no arquivo.\n";
                remove("temp_alt.bin");
            } else {
                remove("dados.bin");
                rename("temp_alt.bin", "dados.bin");
                cout << "Registro alterado com sucesso!\n";
            }
        }
    }
}

void trocarRegistros() {
    int pos1, pos2;
    cout << "Digite a primeira posicao para trocar: ";
    cin >> pos1;
    cout << "Digite a segunda posicao para trocar: ";
    cin >> pos2;

    if (pos1 < 0 || pos2 < 0 || pos1 == pos2) {
        cout << "Erro: posicoes invalidas ou iguais.\n";
    } else {
        if (pos1 > pos2) {
            int temp = pos1;
            pos1 = pos2;
            pos2 = temp;
        }

        ifstream arq("dados.bin", ios::binary);
        if (!arq.is_open()) {
            cout << "ERRO: O arquivo 'dados.bin' nao foi encontrado. Tente reiniciar o programa.\n";
        } else {
            string reg1, reg2;
            int len1 = -1, len2 = -1;
            int pos_atual = 0;
            int len_temp;
            
            while (pos_atual <= pos2 && arq.read(reinterpret_cast<char*>(&len_temp), sizeof(int))) {
                if (pos_atual == pos1) {
                    reg1.resize(len_temp);
                    arq.read(&reg1[0], len_temp);
                    len1 = len_temp;
                } else if (pos_atual == pos2) {
                    reg2.resize(len_temp);
                    arq.read(&reg2[0], len_temp);
                    len2 = len_temp;
                } else {
                    arq.seekg(len_temp, ios::cur);
                }
                pos_atual++;
            }

            if (len1 == -1 || len2 == -1) {
                cout << "Erro: uma ou ambas as posicoes nao foram encontradas.\n";
            } else {
                arq.clear();
                arq.seekg(0);
                ofstream arq_temp("temp_swap.bin", ios::binary | ios::trunc);
                pos_atual = 0;
                while (arq.read(reinterpret_cast<char*>(&len_temp), sizeof(int))) {
                    string linha(len_temp, '\0');
                    arq.read(&linha[0], len_temp);

                    if (pos_atual == pos1) {
                        arq_temp.write(reinterpret_cast<const char*>(&len2), sizeof(int));
                        arq_temp.write(reg2.c_str(), len2);
                    } else if (pos_atual == pos2) {
                        arq_temp.write(reinterpret_cast<const char*>(&len1), sizeof(int));
                        arq_temp.write(reg1.c_str(), len1);
                    } else {
                        arq_temp.write(reinterpret_cast<const char*>(&len_temp), sizeof(int));
                        arq_temp.write(linha.c_str(), len_temp);
                    }
                    pos_atual++;
                }
                arq_temp.close();
                remove("dados.bin");
                rename("temp_swap.bin", "dados.bin");
                cout << "Registros das posicoes " << pos1 << " e " << pos2 << " trocados com sucesso!\n";
            }
            arq.close();
        }
    }
}


// --- FUNÇÕES DE ORDENAÇÃO E PREPARAÇÃO ---

string getPeriod(const string &line) {
    size_t pos1 = line.find(',');
    if (pos1 == string::npos) return "";
    size_t pos2 = line.find(',', pos1 + 1);
    if (pos2 == string::npos) return "";
    return line.substr(pos1 + 1, pos2 - pos1 - 1);
}

void mergeMultiway(int k, const string& cabecalho) {
    const string INPUT_FILE_PREFIX = "temp/F";
    const string OUTPUT_FILE_PREFIX = "temp/S";

    ifstream entradaBin("dados.bin", ios::binary);
    if (!entradaBin.is_open()) {
        cout << "Erro: Nao foi possivel abrir o arquivo 'dados.bin' para ordenacao.\n";
    } else {
        // Distribuição inicial a partir do dados.bin
        for (int i = 0; i < k; i++) {
            ofstream file(INPUT_FILE_PREFIX + to_string(i) + ".bin", ios::binary | ios::trunc);
        }

        int totalRegistros = 0;
        int len;
        while (entradaBin.read(reinterpret_cast<char *>(&len), sizeof(int))) {
            string linha(len, '\0');
            entradaBin.read(&linha[0], len);

            int idx = totalRegistros % k;
            ofstream fout(INPUT_FILE_PREFIX + to_string(idx) + ".bin", ios::binary | ios::app);
            fout.write(reinterpret_cast<const char *>(&len), sizeof(int));
            fout.write(linha.data(), len);
            fout.close();
            totalRegistros++;
        }
        entradaBin.close();

        long long blocoTamanho = 1;
        bool flip = true;
        auto inicio = chrono::high_resolution_clock::now();

        // Passadas de intercalação
        while (blocoTamanho < totalRegistros) {
            string prefixIn = (flip ? INPUT_FILE_PREFIX : OUTPUT_FILE_PREFIX);
            string prefixOut = (flip ? OUTPUT_FILE_PREFIX : INPUT_FILE_PREFIX);

            for (int i = 0; i < k; i++) {
                ofstream of(prefixOut + to_string(i) + ".bin", ios::binary | ios::trunc);
            }

            ifstream inputs[k];
            for (int i = 0; i < k; i++) {
                inputs[i].open(prefixIn + to_string(i) + ".bin", ios::in | ios::binary);
            }

            string registro[k];
            long long lidos[k];
            bool presente[k];
            int runIndex = 0;
            bool aindaTemRuns = true;

            while (aindaTemRuns) {
                int somaPresentes = 0;
                for (int i = 0; i < k; i++) {
                    lidos[i] = 0;
                    presente[i] = false;
                    int temp_len;
                    if (inputs[i] && inputs[i].read(reinterpret_cast<char *>(&temp_len), sizeof(int))) {
                        registro[i].resize(temp_len);
                        inputs[i].read(&registro[i][0], temp_len);
                        lidos[i] = 1;
                        presente[i] = true;
                        somaPresentes++;
                    }
                }
                
                if (somaPresentes == 0) {
                    aindaTemRuns = false;
                } else {
                    int countRunsNaVez = somaPresentes;
                    int arqDestino = runIndex % k;
                    ofstream output(prefixOut + to_string(arqDestino) + ".bin", ios::binary | ios::app);
                    
                    while (countRunsNaVez > 0) {
                        int menorIdx = -1;
                        string menorPeriod;
                        for (int i = 0; i < k; i++) {
                            if (presente[i]) {
                                string per = getPeriod(registro[i]);
                                if (menorIdx < 0 || per < menorPeriod) {
                                    menorIdx = i;
                                    menorPeriod = per;
                                }
                            }
                        }
                        
                        int lenOut = int(registro[menorIdx].size());
                        output.write(reinterpret_cast<const char *>(&lenOut), sizeof(int));
                        output.write(registro[menorIdx].data(), lenOut);

                        if (lidos[menorIdx] < blocoTamanho) {
                            int lenNext;
                            if (inputs[menorIdx] && inputs[menorIdx].read(reinterpret_cast<char *>(&lenNext), sizeof(int))) {
                                registro[menorIdx].resize(lenNext);
                                inputs[menorIdx].read(&registro[menorIdx][0], lenNext);
                                lidos[menorIdx]++;
                            } else {
                                presente[menorIdx] = false;
                                countRunsNaVez--;
                            }
                        } else {
                            presente[menorIdx] = false;
                            countRunsNaVez--;
                        }
                    }
                    output.close();
                    runIndex++;
                }
            }

            for (int i = 0; i < k; i++) {
                if(inputs[i].is_open()) inputs[i].close();
            }

            blocoTamanho *= k;
            flip = !flip;
        }

        // Finalização: sobrescreve o dados.bin com o resultado ordenado
        string finalPrefix = (flip ? INPUT_FILE_PREFIX : OUTPUT_FILE_PREFIX);
        string arquivoFinalOrdenado = finalPrefix + "0.bin";
        
        remove("dados.bin");
        if (rename(arquivoFinalOrdenado.c_str(), "dados.bin") != 0) {
            perror("Erro ao renomear arquivo final");
        }

        // Cria o arquivo CSV final para visualização
        ifstream inputBin("dados.bin", ios::binary);
        ofstream outputCSV("dados_ordenado.csv", ios::trunc);
        outputCSV << cabecalho << '\n';
        int final_len;
        while (inputBin.read(reinterpret_cast<char *>(&final_len), sizeof(int))) {
            string linha_csv(final_len, '\0');
            inputBin.read(&linha_csv[0], final_len);
            outputCSV << linha_csv << '\n';
        }
        inputBin.close();
        outputCSV.close();

        cout << "\nOrdenacao concluida. Total de registros: " << totalRegistros << "\n";
        auto fim = chrono::high_resolution_clock::now();
        auto duracao = chrono::duration_cast<chrono::milliseconds>(fim - inicio).count();
        cout << "Tempo total de execucao: " << duracao << " ms\n";
        cout << "Arquivo 'dados.bin' foi atualizado com os dados ordenados.\n";
        cout << "Arquivo CSV ordenado para visualizacao: 'dados_ordenado.csv'\n";
    }
}

string converterCsvParaBinario() {
    ifstream csv("property-transfer-statistics-march-2022-quarter-csv.csv");
    if (!csv.is_open()) {
        cout << "ERRO CRITICO: Arquivo 'property-transfer-statistics-march-2022-quarter-csv.csv' nao encontrado.\n";
        return "";
    }

    ofstream bin("dados.bin", ios::binary | ios::trunc);
    if (!bin.is_open()) {
        cout << "ERRO CRITICO: Nao foi possivel criar o arquivo de trabalho 'dados.bin'.\n";
        return "";
    }
    
    string cabecalho;
    getline(csv, cabecalho); // Lê o cabeçalho

    string linha;
    while (getline(csv, linha)) {
        int len = linha.length();
        bin.write(reinterpret_cast<const char*>(&len), sizeof(int));
        bin.write(linha.c_str(), len);
    }

    cout << "Arquivo de dados convertido para 'dados.bin' com sucesso.\n";
    csv.close();
    bin.close();
    return cabecalho; // Retorna o cabeçalho para ser usado depois
}

void finalizarPrograma() {
    cout << "\nPrograma encerrando. Limpando arquivos...\n";

    const string INPUT_FILE_PREFIX_CLEAN = "temp/F";
    const string OUTPUT_FILE_PREFIX_CLEAN = "temp/S";

    for (int i = 0; i < 20; i++) { // Limpa até 20 arquivos, um valor seguro
        remove((INPUT_FILE_PREFIX_CLEAN + to_string(i) + ".bin").c_str());
        remove((OUTPUT_FILE_PREFIX_CLEAN + to_string(i) + ".bin").c_str());
    }
    remove("temp_add.bin");
    remove("temp_alt.bin");
    remove("temp_swap.bin");
    remove("dados.bin");
    remove("dados_ordenado.csv");
    
    cout << "Arquivos temporarios e de trabalho removidos.\n";
    cout << "Obrigado por usar o programa!\n";

    cout << "Pressione Enter para sair...";
    limparBufferCin();
    cin.get();
}

// --- FUNÇÃO DE MENU ---

void menu(const string& cabecalho) {
    int escolha = 0;
    do {
        cout << "\n";
        cout << "                      ########::                                                                                              \n";
        cout << "                ####################::                                                                                        \n";
        cout << "              ############################                                                                                    \n";
        cout << "            ################################                                                                                  \n";
        cout << "          ##########################mm########         MMMM++   MMMMMM     MMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM     MMMMMMMMMMMMMM     \n";
        cout << "        @@########################  ##########         MMMM++   MMMMMM     MMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM   MMMMMMMMMMMMMMMM--   \n";
        cout << "        ########################    ##########@@       MMMM++   MMMMMM     MMMMMMMM..................MMMMMM   MMMMMMMM     MMMMMMMM   \n";
        cout << "      ######################MM  @@mm++############     MMMM++   MMMMMM     MMMMMM                   MMMMMM   MMMMMM           MMMMMM  \n";
        cout << "      ####################    ####  ##############     MMMM++   MMMMMM     MMMMMM                   MMMMMM MMMMMM               MMMM  \n";
        cout << "      ##################    ######  ################   MMMM++   MMMMMM     MMMMMM    ..MMMMMMMMMM++   MMMMMM MMMMmm     MMMM      MMMM  \n";
        cout << "      ##############@@  MM##mm--##  ################   MMMM++   MMMMMM     MMMMMM    MMMMMMMMMMMM++   MMMMMM MMMM--   MMMMMM      MMMM  \n";
        cout << "      ############    ####    ####  ################   MMMM++   MMMMMM     MMMMMM          MMMM++   MMMMMM MMMM--   MMMMMM      MMMM  \n";
        cout << "      ##########    ####      ##  @@################   MMMM++   MMMMMM     MMMMMM          MMMM++   MMMMMMMMMMMM--   MMMMMM      MMMM  \n";
        cout << "      ######@@  ++####  @@##  ##  ##################   MMMM++   MMMMMM     MMMMMM          MMMM++   MMMMMMMMMMMM--   MMMMMM      MMMM  \n";
        cout << "      ####    ####..  ######  ##  ################..   MMMMMM     MMMM     MMMMMM    MMMMMMMMMMMM++   ..MMMMMMMMMM--   MMMMMM      MMMM  \n";
        cout << "      ##    ####    ######  @@##  ################     MMMMMM             MMMMMM    MMMMMMMMMMMMMM             MMMMMM      MMMM  \n";
        cout << "          ####    ########  ##MM  ################     MMMMMMMM         MMMMMMMM    MMMMMM  MMMMMM             MMMMMM      MMMM  \n";
        cout << "        ##MM  ############  ##  ################       MMMMMMMM   MMMMMMMMMMMM    MMMMMM    MMMMMMMM             MMMMMM      MMMM  \n";
        cout << "              ############++  ##  ##############           MMMMMMMMMMMMMMMMMMMMMM    MMMMMM      ::MMMMMMMMMMMMMMMMMMMMMMMMMMMMMM      MMMM  \n";
        cout << "              ############  --##  ############::             ..MMMMMMMMMMMM  MMMMMM    MMMMMM        MMMMMMMMMMMMMMMMMMMMMMMMMMMM      MMMM  \n";
        cout << "                ##########  ####  ##########                                                                                        \n";
        cout << "                      ######  ##  --########----------------------------------------------------------------------------------------------------  \n";
        cout << "                            ..  ##  ##############################################################################################################  \n";
        cout << "\n--- MENU DE OPERACOES ---";
        cout << "\n1. Ordenar Arquivo de Dados";
        cout << "\n2. Adicionar um elemento";
        cout << "\n3. Visualizar registros";
        cout << "\n4. Alterar um registro";
        cout << "\n5. Trocar dois registros";
        cout << "\n6. Imprimir todos os registros";
        cout << "\n0. Sair";
        cout << "\n\nEscolha uma opcao: ";
        
        cin >> escolha;

        if (cin.fail()) {
            cout << "Opcao invalida. Por favor, digite um numero.\n";
            cin.clear();
            limparBufferCin();
            escolha = -1;
            continue;
        }

        switch (escolha) {
            case 1: {
                int k_usuario;
                cout << "\nDigite o numero de caminhos (k) para a ordenacao (ex: 16): ";
                cin >> k_usuario;
                if (cin.fail() || k_usuario < 2) {
                    cout << "Valor de 'k' invalido. Usando o padrao k=16.\n";
                    k_usuario = 16;
                    cin.clear();
                    limparBufferCin();
                }
                cout << "\nIniciando ordenacao com k = " << k_usuario << "...\n";
                mergeMultiway(k_usuario, cabecalho);
                break;
            }
            case 2: adicionarRegistro(); break;
            case 3: visualizarRegistros(); break;
            case 4: alterarRegistro(); break;
            case 5: trocarRegistros(); break;
            case 6: imprimirTodosRegistros(); break;
            case 0: cout << "Saindo...\n"; break;
            default: cout << "Opcao invalida. Tente novamente.\n";
        }

    } while (escolha != 0);
}

int main() {
    string cabecalho = converterCsvParaBinario();
    if (!cabecalho.empty()) { // Só executa o menu se a conversão foi bem-sucedida
        menu(cabecalho);
    }
    finalizarPrograma();
    return 0;
}
