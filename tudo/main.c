#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <io.h>      // Para _findfirst, _findnext, _findclose
#include <direct.h>  // Para _mkdir
#include <errno.h>   // Para errno

// Função para criar um diretório
void criar_diretorio(const char* nome) {
    if (_mkdir(nome) != 0) {
        if (errno != EEXIST) { // EEXIST indica que o diretório já existe
            perror("Nao foi possivel criar o diretorio");
        }
    }
}

// Função para obter o nome da pasta baseado na extensão do arquivo
const char* obter_nome_pasta(const char* extensao) {
    if (strcmp(extensao, ".txt") == 0) return "textos";
    if (strcmp(extensao, ".pdf") == 0) return "PDFs";
    if (strcmp(extensao, ".doc") == 0 || strcmp(extensao, ".docx") == 0) return "documentos";
    if (strcmp(extensao, ".jpg") == 0 || strcmp(extensao, ".jpeg") == 0) return "imagens";
    if (strcmp(extensao, ".png") == 0) return "imagens";
    if (strcmp(extensao, ".csv") == 0) return "planilhas";
    return "outros";
}

// Função para verificar se um arquivo tem uma das extensões especificadas
int tem_extensao(const char* nome_arquivo, const char* extensoes[], int num_extensoes) {
    const char* dot = strrchr(nome_arquivo, '.');
    if (!dot) return 0; // Arquivo sem extensão
    for (int i = 0; i < num_extensoes; i++) {
        if (strcmp(dot, extensoes[i]) == 0) {
            return 1;
        }
    }
    return 0;
}

// Função para mover arquivos com extensões específicas
void mover_arquivos(const char* diretorio_origem, const char* diretorio_destino, const char* extensoes[], int num_extensoes) {
    struct _finddata_t entrada;
    char caminho_origem[256];
    intptr_t dir = _findfirst(diretorio_origem, &entrada);

    if (dir == -1) {
        perror("Nao foi possivel abrir o diretorio");
        return;
    }

    // Cria o diretório de destino se não existir
    criar_diretorio(diretorio_destino);

    do {
        if (!(entrada.attrib & _A_SUBDIR)) { // Ignora subdiretórios
            const char* extensao = strrchr(entrada.name, '.');
            if (extensao && tem_extensao(entrada.name, extensoes, num_extensoes)) {
                const char* nome_pasta = obter_nome_pasta(extensao);
                char pasta_destino[256];
                char origem[256];
                char destino[256];

                // Cria a pasta para o tipo de arquivo
                snprintf(pasta_destino, sizeof(pasta_destino), "%s\\%s", diretorio_destino, nome_pasta);
                criar_diretorio(pasta_destino);

                // Move o arquivo para a pasta correspondente
                snprintf(origem, sizeof(origem), "%s\\%s", diretorio_origem, entrada.name);
                snprintf(destino, sizeof(destino), "%s\\%s\\%s", diretorio_destino, nome_pasta, entrada.name);

                printf("Tentando mover o arquivo: %s -> %s\n", origem, destino);

                if (rename(origem, destino) == 0) {
                    printf("Arquivo movido: %s -> %s\n", origem, destino);
                } else {
                    perror("Erro ao mover arquivo");
                }
            }
        }
    } while (_findnext(dir, &entrada) == 0);

    _findclose(dir);
}

int main() {
    // Caminho da pasta Downloads no Windows
    const char* diretorio_origem = "C:\\Users\\lucas\\Downloads\\*.*";
    const char* diretorio_destino = "C:\\Users\\lucas\\Downloads\\arquivos";

    // Defina as extensões de arquivo que você deseja mover
    const char* extensoes[] = {".txt", ".pdf", ".doc", ".docx", ".jpg", ".jpeg", ".png", ".csv"};
    int num_extensoes = sizeof(extensoes) / sizeof(extensoes[0]);

    mover_arquivos(diretorio_origem, diretorio_destino, extensoes, num_extensoes);

    return 0;
}
