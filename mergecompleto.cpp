#include <chrono>
#include <fstream>
#include <iostream>
#include <cstdio>

using namespace std;

// FUNÇÕES OBRIGATÓRIAS DO PROJETO E AUXILIARES
void limparBufferCin() {
    // Ignora até 10000 (Joguei um valor genérico porém grande o suficiente) caracteres ou até encontrar um '\n'
    cin.ignore(10000, '\n'); 
}

bool ehNumero(const string& str, bool permitePonto) {
    if (str.empty()) return false;
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

        if (i == 1) { // Period (ex: 2018.12)
            while (!ehNumero(campos[i], true)) {
                cout << "Entrada invalida. Digite um numero (ex: 2018.12): ";
                getline(cin, campos[i]);
            }
        } else if (i == 2 || i == 5) { // Data_value e Magnitude (inteiros)
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


// --- FUNÇÕES DE OPERAÇÃO DO MENU ---

void imprimirTodosRegistros(const string& nomeArquivo) {
    ifstream arq(nomeArquivo, ios::binary);
    if (!arq.is_open()) {
        cout << "Erro: nao foi possivel abrir o arquivo " << nomeArquivo << endl;
        return;
    }

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

void visualizarRegistros(const string& nomeArquivo) {
    int inicio, fim;
    cout << "Digite a posicao inicial: ";
    cin >> inicio;
    cout << "Digite a posicao final: ";
    cin >> fim;

    if (inicio < 0 || fim < inicio) {
        cout << "Erro: intervalo de posicoes invalido.\n";
        return;
    }

    ifstream arq(nomeArquivo, ios::binary);
    if (!arq.is_open()) {
        cout << "Erro: nao foi possivel abrir o arquivo " << nomeArquivo << endl;
        return;
    }

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
    if(!encontrou) cout << "Nenhum registro encontrado nesse intervalo.\n";
    arq.close();
}

void adicionarRegistro(const string& nomeArquivo) {
    int pos_inserir;
    cout << "Digite a posicao onde deseja inserir o novo registro: ";
    cin >> pos_inserir;

    if (pos_inserir < 0) {
        cout << "Erro: posicao invalida.\n";
        return;
    }

    string novo_registro = lerNovoRegistro();
    int len_novo = novo_registro.length();

    ifstream arq_orig(nomeArquivo, ios::binary);
    if (!arq_orig.is_open()) {
        cout << "Arquivo '" << nomeArquivo << "' nao encontrado. Criando um novo.\n";
        ofstream arq_novo(nomeArquivo, ios::binary);
        arq_novo.write(reinterpret_cast<const char*>(&len_novo), sizeof(int));
        arq_novo.write(novo_registro.c_str(), len_novo);
        arq_novo.close();
        cout << "Registro adicionado na posicao 0.\n";
        return;
    }
    
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

    remove(nomeArquivo.c_str());
    rename("temp_add.bin", nomeArquivo.c_str());
}

void alterarRegistro(const string& nomeArquivo) {
    int pos_alterar;
    cout << "Digite a posicao do registro que deseja alterar: ";
    cin >> pos_alterar;

    if (pos_alterar < 0) {
        cout << "Erro: posicao invalida.\n";
        return;
    }

    ifstream arq_orig(nomeArquivo, ios::binary);
    if (!arq_orig.is_open()) {
        cout << "Erro: nao foi possivel abrir o arquivo " << nomeArquivo << endl;
        return;
    }

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
        remove(nomeArquivo.c_str());
        rename("temp_alt.bin", nomeArquivo.c_str());
        cout << "Registro alterado com sucesso!\n";
    }
}

void trocarRegistros(const string& nomeArquivo) {
    int pos1, pos2;
    cout << "Digite a primeira posicao para trocar: ";
    cin >> pos1;
    cout << "Digite a segunda posicao para trocar: ";
    cin >> pos2;

    if (pos1 < 0 || pos2 < 0 || pos1 == pos2) {
        cout << "Erro: posicoes invalidas ou iguais.\n";
        return;
    }
    if (pos1 > pos2) {
        int temp = pos1;
        pos1 = pos2;
        pos2 = temp;
    }

    ifstream arq(nomeArquivo, ios::binary);
    if (!arq.is_open()) {
        cout << "Erro: nao foi possivel abrir o arquivo " << nomeArquivo << endl;
        return;
    }

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
        arq.close();
        return;
    }
    
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

    arq.close();
    arq_temp.close();

    remove(nomeArquivo.c_str());
    rename("temp_swap.bin", nomeArquivo.c_str());

    cout << "Registros das posicoes " << pos1 << " e " << pos2 << " trocados com sucesso!\n";
}

// --- FUNÇÃO PRINCIPAL DO MENU (chama as outras) ---

void menu() {
    int escolha = 0;
    const string arquivoBinario = "ordenado.bin";

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
        cout << "\nArquivo: " << arquivoBinario;
        cout << "\n1. Adicionar um elemento em uma posicao especifica";
        cout << "\n2. Visualizar registros entre duas posicoes";
        cout << "\n3. Alterar os dados de um registro";
        cout << "\n4. Trocar dois registros de posicao";
        cout << "\n5. Imprimir todos os registros";
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
            case 1: adicionarRegistro(arquivoBinario); break;
            case 2: visualizarRegistros(arquivoBinario); break;
            case 3: alterarRegistro(arquivoBinario); break;
            case 4: trocarRegistros(arquivoBinario); break;
            case 5: imprimirTodosRegistros(arquivoBinario); break;
            case 0: cout << "Encerrando o programa...\n"; break;
            default: cout << "Opcao invalida. Tente novamente.\n";
        }

    } while (escolha != 0);
}

// FUNÇÕES PRINCIPAIS DO MERGESORT 

// Ordena pelo campo Period //
string getPeriod(const string &line) {
  int pos1 = line.find(',');
  if (pos1 == -1)
    return "";
  int pos2 = line.find(',', pos1 + 1);
  if (pos2 == -1)
    return "";
  return line.substr(pos1 + 1, pos2 - pos1 - 1);
}


void mergeMultiway(int k, string entrada){
  const string INPUT_FILE_PREFIX = "temp/F";
  const string OUTPUT_FILE_PREFIX = "temp/S";

  ifstream entradaCSV(entrada, ios::in | ios::binary);
  string cabecalho;
  getline(entradaCSV, cabecalho);

  for (int i = 0; i < k; i++) {
    ofstream file(INPUT_FILE_PREFIX + to_string(i) + ".bin",
                  ios::out | ios::binary | ios::trunc);
    file.close();
  }

  int totalRegistros = 0;
  string linha;
  while (getline(entradaCSV, linha)) {
    int len = int(linha.size());
    int idx = totalRegistros % k;
    ofstream fout(INPUT_FILE_PREFIX + to_string(idx) + ".bin",
                  ios::out | ios::binary | ios::app);
    fout.write(reinterpret_cast<const char *>(&len), sizeof(int));
    fout.write(linha.data(), len);
    fout.close();
    totalRegistros++;
  }
  entradaCSV.close();

  int blocoTamanho = 1;
  bool flip = true;
  auto inicio = chrono::high_resolution_clock::now();

  while (blocoTamanho < totalRegistros) {
    string prefixIn = (flip ? INPUT_FILE_PREFIX : OUTPUT_FILE_PREFIX);
    string prefixOut = (flip ? OUTPUT_FILE_PREFIX : INPUT_FILE_PREFIX);

    for (int i = 0; i < k; i++) {
      ofstream of(prefixOut + to_string(i) + ".bin",
                  ios::out | ios::binary | ios::trunc);
      of.close();
    }

    ifstream inputs[k];
    for (int i = 0; i < k; i++)
      inputs[i].open(prefixIn + to_string(i) + ".bin", ios::in | ios::binary);

    string registro[k];
    int lidos[k];
    bool presente[k];

    int runIndex = 0;
    bool aindaTemRuns = true;

    while (aindaTemRuns) {
      int somaPresentes = 0;
      for (int i = 0; i < k; i++) {
        lidos[i] = 0;
        presente[i] = false;
        int len;
        if (inputs[i].read(reinterpret_cast<char *>(&len), sizeof(int))) {
          registro[i].resize(len);
          inputs[i].read(&registro[i][0], len);
          lidos[i] = 1;
          presente[i] = true;
          somaPresentes++;
        }
      }
      
      if (somaPresentes == 0) {
        aindaTemRuns = false;
      }

      int countRunsNaVez = somaPresentes;
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
        int arqDestino = runIndex % k;
        int lenOut = int(registro[menorIdx].size());

        ofstream output(prefixOut + to_string(arqDestino) + ".bin",
                        ios::out | ios::binary | ios::app);
        output.write(reinterpret_cast<const char *>(&lenOut), sizeof(int));
        output.write(registro[menorIdx].data(), lenOut);
        output.close();

        if (lidos[menorIdx] < blocoTamanho) {
          int lenNext;
          if (inputs[menorIdx].read(reinterpret_cast<char *>(&lenNext),
                                    sizeof(int))) {
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
      runIndex++;
    }

    for (int i = 0; i < k; i++)
      inputs[i].close();

    blocoTamanho *= k;
    flip = !flip;
  }

  string finalPrefix = (flip ? INPUT_FILE_PREFIX : OUTPUT_FILE_PREFIX);
  string ordenadoBin = "ordenado.bin";
  string ordenadoCSV = "ordenado.csv";

  ifstream inputFinal(finalPrefix + "0.bin", ios::in | ios::binary);
  ofstream outputFinal(ordenadoBin, ios::out | ios::binary | ios::trunc);
  char buffer[4096];
  while (inputFinal.good()) {
    inputFinal.read(buffer, sizeof(buffer));
    streamsize l = inputFinal.gcount();
    outputFinal.write(buffer, l);
    outputFinal.flush();
  }

    ifstream inputBin(ordenadoBin, ios::in | ios::binary);
    ofstream outputCSV(ordenadoCSV, ios::out | ios::trunc);
    outputCSV << cabecalho << '\n';
    int len;
    while (inputBin.read(reinterpret_cast<char *>(&len), sizeof(int))) {
      char *linha = new char[len];
      inputBin.read(linha, len);
      outputCSV.write(linha, len);
      outputCSV << '\n';
      delete[] linha;
      outputCSV.flush();
    }

  cout << "Ordenação concluída. Total de registros: " << totalRegistros << "\n";
  auto fim = chrono::high_resolution_clock::now();
  auto duracao =
      chrono::duration_cast<chrono::milliseconds>(fim - inicio).count();
  cout << "Tempo total de execução: " << duracao << " ms\n";
  cout << "Quantidade de arquivos: " << k << "\n";

}

int main() {
  const int k = 16;
  string entrada = "dados.csv";
  mergeMultiway(k, entrada);
  menu();

return 0;
}