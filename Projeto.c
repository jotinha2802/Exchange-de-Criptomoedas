/*
 * Login padrao: CPF 12345678900 / senha 123456
 */

#include <stdio.h>   
#include <stdlib.h>  
#include <string.h>  
#include <time.h>    

#define MAX_USUARIOS 10
#define ARQ_USUARIOS "usuarios.txt"
#define ARQ_COTACOES "cotacoes.txt"

#define QTD_CRIPTO 3
const char *nomeCripto[QTD_CRIPTO]  = {"Bitcoin", "Ethereum", "Ripple"};
const char *siglaCripto[QTD_CRIPTO] = {"BTC", "ETH", "XRP"};
const double taxaCompra[QTD_CRIPTO] = {0.02, 0.01, 0.01}; 
const double taxaVenda[QTD_CRIPTO]  = {0.03, 0.02, 0.01}; 

/* Struct com os dados de um investidor */
typedef struct {
    char cpf[12];              
    char senha[7];            
    char nome[50];
    double reais;              
    double cripto[QTD_CRIPTO]; 
} Usuario;

Usuario usuarios[MAX_USUARIOS];
int qtdUsuarios = 0;
double cotacao[QTD_CRIPTO];

/* ================= FUNCOES DE ARQUIVO ================= */

/* Salva todos os dados de cada usuario(sobrescreve o arquivo inteiro).
   Formato de cada linha: cpf;senha;nome;reais;btc;eth;xrp */
void salvarUsuarios(void) {
    int i;
    FILE *f = fopen(ARQ_USUARIOS, "w");
    if (f == NULL) {
        printf("Erro ao salvar %s\n", ARQ_USUARIOS);
        return;
    }
    for (i = 0; i < qtdUsuarios; i++) {
        fprintf(f, "%s;%s;%s;%lf;%lf;%lf;%lf\n",
                usuarios[i].cpf, usuarios[i].senha, usuarios[i].nome,
                usuarios[i].reais, usuarios[i].cripto[0],
                usuarios[i].cripto[1], usuarios[i].cripto[2]);
    }
    fclose(f);
}

/* Le os usuarios do arquivo. Se o arquivo nao existir,
   cria um usuario padrao para conseguir testar. */
void carregarUsuarios(void) {
    FILE *f = fopen(ARQ_USUARIOS, "r");
    if (f == NULL) {
        Usuario u = {"12345678900", "123456", "Joao Pedro", 0, {0, 0, 0}};
        usuarios[0] = u;
        qtdUsuarios = 1;
        salvarUsuarios();
        return;
    }
    qtdUsuarios = 0;
    Usuario *u = &usuarios[0];
    /* %[^;] le tudo ate encontrar um ';' 
       fscanf precisa ler os 7 campos. */
    while (qtdUsuarios < MAX_USUARIOS &&
           fscanf(f, " %11[^;];%6[^;];%49[^;];%lf;%lf;%lf;%lf",
                  u->cpf, u->senha, u->nome, &u->reais,
                  &u->cripto[0], &u->cripto[1], &u->cripto[2]) == 7) {
        qtdUsuarios++;
        u = &usuarios[qtdUsuarios];
    }
    fclose(f);
}

/* Grava as 3 cotacoes em uma linha: btc eth xrp */
void salvarCotacoes(void) {
    FILE *f = fopen(ARQ_COTACOES, "w");
    if (f == NULL) {
        printf("Erro ao salvar %s\n", ARQ_COTACOES);
        return;
    }
    fprintf(f, "%lf %lf %lf\n", cotacao[0], cotacao[1], cotacao[2]);
    fclose(f);
}

/* Le as cotacoes*/
void carregarCotacoes(void) {
    FILE *f = fopen(ARQ_COTACOES, "r");
    if (f == NULL ||
        fscanf(f, "%lf %lf %lf", &cotacao[0], &cotacao[1], &cotacao[2]) != 3) {
        cotacao[0] = 350000.0; /* Bitcoin em reais*/ 
        cotacao[1] = 16108.0;  /*  Ethereum em reais*/ 
        cotacao[2] = 2.57;     /*  Ripple em reais */
        salvarCotacoes();
    }
    if (f != NULL) fclose(f);
}

/* Acrescenta uma linha no extrato do usuario */
void registrarExtrato(Usuario *u, char sinal, double valor,
                      const char *moeda, double cot, double taxa) {
    char nomeArq[40];
    sprintf(nomeArq, "extrato_%s.txt", u->cpf); /* um arquivo por CPF */

    FILE *f = fopen(nomeArq, "a");
    if (f == NULL) {
        printf("Erro ao gravar extrato\n");
        return;
    }

    /* Pega data e hora atuais no formato dd-mm-aaaa hh:mm */
    char data[20];
    time_t agora = time(NULL);
    strftime(data, sizeof(data), "%d-%m-%Y %H:%M", localtime(&agora));

    fprintf(f, "%s %c %.2f %-4s CT: %.2f TX: %.2f REAL: %.2f BTC: %.6f ETH: %.6f XRP: %.6f\n",
            data, sinal, valor, moeda, cot, taxa,
            u->reais, u->cripto[0], u->cripto[1], u->cripto[2]);
    fclose(f);
}

/* ================= FUNCOES ================= */

/* Descarta o que sobrou digitado no teclado (evita loop infinito
   quando o usuario digita uma letra no lugar de um numero) */
void limparBuffer(void) {
    int c;
    while ((c = getchar()) != '\n' && c != EOF) {}
}

/* Le um numero real do teclado. Retorna -1 se for invalido. */
double lerValor(void) {
    double v;
    if (scanf("%lf", &v) != 1) {
        limparBuffer();
        return -1;
    }
    return v;
}


int confirmarSenha(Usuario *u) {
    char senha[20];
    printf("Digite sua senha: ");
    scanf("%19s", senha);
    if (strcmp(senha, u->senha) == 0) return 1;
    printf("Senha incorreta!\n");
    return 0;
}

/* Mostra nome, CPF formatado (123.456.789-00) e os saldos */
void mostrarSaldo(Usuario *u) {
    int i; 
    printf("\nNome: %s\n", u->nome);
    /* %.3s imprime so 3 caracteres */
    printf("CPF: %.3s.%.3s.%.3s-%.2s\n\n", u->cpf, u->cpf + 3, u->cpf + 6, u->cpf + 9);
    printf("Reais: %.2f\n", u->reais);
    for (i = 0; i < QTD_CRIPTO; i++)
        printf("%s: %.6f\n", nomeCripto[i], u->cripto[i]);
}

/* Lista as cotacoes atuais e as taxas de cada moeda */
void mostrarCotacoes(void) {
    int i;
    printf("\n--- Cotacoes ---\n");
    for (i = 0; i < QTD_CRIPTO; i++)
        printf("%d. %-8s R$ %.2f  (taxa compra %.0f%% | venda %.0f%%)\n",
               i + 1, nomeCripto[i], cotacao[i],
               taxaCompra[i] * 100, taxaVenda[i] * 100);
}

/* ================= OPERACOES DO MENU ================= */

/* 1. Consultar saldo (pede senha) */
void consultarSaldo(Usuario *u) {
    if (confirmarSenha(u)) mostrarSaldo(u);
}

/* 2. Consultar extrato (pede senha) */
void consultarExtrato(Usuario *u) {
    if (!confirmarSenha(u)) return;

    char nomeArq[40], linha[256];
    sprintf(nomeArq, "extrato_%s.txt", u->cpf);

    printf("\nNome: %s\n", u->nome);
    printf("CPF: %.3s.%.3s.%.3s-%.2s\n\n", u->cpf, u->cpf + 3, u->cpf + 6, u->cpf + 9);

    FILE *f = fopen(nomeArq, "r");
    if (f == NULL) {
        printf("Nenhuma operacao registrada.\n");
        return;
    }
    while (fgets(linha, sizeof(linha), f) != NULL)
        printf("%s", linha);
    fclose(f);
}

/* 3. Depositar reais */
void depositar(Usuario *u) {
    printf("Valor do deposito: R$ ");
    double valor = lerValor();
    if (valor <= 0) {
        printf("Valor invalido!\n");
        return;
    }
    u->reais += valor;
    salvarUsuarios();                              /* persiste o saldo */
    registrarExtrato(u, '+', valor, "REAL", 0, 0); /* anota no extrato */
    printf("Deposito realizado!\n");
    mostrarSaldo(u);
}

/* 4. Sacar reais  */
void sacar(Usuario *u) {
    printf("Valor do saque: R$ ");
    double valor = lerValor();
    if (valor <= 0) {
        printf("Valor invalido!\n");
        return;
    }
    if (valor > u->reais) {
        printf("Saldo insuficiente!\n");
        return;
    }
    u->reais -= valor;
    salvarUsuarios();
    registrarExtrato(u, '-', valor, "REAL", 0, 0);
    printf("Saque realizado!\n");
    mostrarSaldo(u);
}

/* Pergunta qual cripto (1 a 3) e devolve o indice (0 a 2), ou -1 */
int escolherCripto(void) {
    printf("Escolha a criptomoeda (1-3): ");
    int op = (int)lerValor();
    if (op < 1 || op > QTD_CRIPTO) {
        printf("Opcao invalida!\n");
        return -1;
    }
    return op - 1;
}

/* 5. Comprar cripto: o usuario informa quantos REAIS quer investir. */
void comprar(Usuario *u) {
    if (!confirmarSenha(u)) return;
    mostrarCotacoes();

    int i = escolherCripto();
    if (i == -1) return;

    printf("Quantos reais deseja investir em %s? R$ ", nomeCripto[i]);
    double valor = lerValor();
    if (valor <= 0) {
        printf("Valor invalido!\n");
        return;
    }

    double total = valor * (1 + taxaCompra[i]); 
    if (total > u->reais) {
        printf("Saldo insuficiente! Total com taxa: R$ %.2f\n", total);
        return;
    }

    u->reais -= total;
    u->cripto[i] += valor / cotacao[i]; 
    salvarUsuarios();
    registrarExtrato(u, '+', valor, siglaCripto[i], cotacao[i], taxaCompra[i]);

    printf("Compra realizada! Taxa paga: R$ %.2f\n", total - valor);
    mostrarSaldo(u);
}

/* 6. Vender cripto */
void vender(Usuario *u) {
    if (!confirmarSenha(u)) return;
    mostrarCotacoes();

    int i = escolherCripto();
    if (i == -1) return;

    printf("Voce tem %.6f %s. Quantidade a vender: ", u->cripto[i], siglaCripto[i]);
    double qtd = lerValor();
    if (qtd <= 0) {
        printf("Valor invalido!\n");
        return;
    }
    if (qtd > u->cripto[i]) {
        printf("Saldo de %s insuficiente!\n", nomeCripto[i]);
        return;
    }

    double bruto = qtd * cotacao[i];               
    double recebido = bruto * (1 - taxaVenda[i]);  

    u->cripto[i] -= qtd;
    u->reais += recebido;
    salvarUsuarios();
    registrarExtrato(u, '-', bruto, siglaCripto[i], cotacao[i], taxaVenda[i]);

    printf("Venda realizada! Recebido: R$ %.2f (taxa R$ %.2f)\n", recebido, bruto - recebido);
    mostrarSaldo(u);
}

/* 7. Atualizar cotacao*/
void atualizarCotacao(void) {
    int i; 
    for (i = 0; i < QTD_CRIPTO; i++) {
        double variacao = (rand() % 1001 - 500) / 10000.0;
        cotacao[i] = cotacao[i] * (1 + variacao);
        printf("%-8s variou %+.2f%%\n", nomeCripto[i], variacao * 100);
    }
    salvarCotacoes();
    mostrarCotacoes();
}

/* ================= CADASTRO ================= */

/* Confere se o texto tem exatamente 'tam' caracteres e todos sao
   digitos de 0 a 9 */
int somenteDigitos(const char *texto, int tam) {
    int i;
    if ((int)strlen(texto) != tam) return 0;
    for (i = 0; i < tam; i++) {
        if (texto[i] < '0' || texto[i] > '9') return 0;
    }
    return 1;
}

/* Procura um CPF no vetor. */
int buscarCpf(const char *cpf) {
    int i; 
    for (i = 0; i < qtdUsuarios; i++) {
        if (strcmp(usuarios[i].cpf, cpf) == 0) return i;
    }
    return -1;
}

/* Cadastra um novo investidor (comeca com todos os saldos zerados) */
void cadastrar(void) {
    int i; 
    if (qtdUsuarios >= MAX_USUARIOS) {
        printf("Limite de %d usuarios atingido!\n", MAX_USUARIOS);
        return;
    }

    Usuario novo;

    printf("\n=== CADASTRO ===\n");
    printf("Nome: ");
    /* " %49[^\n]" le a linha inteira (aceita espacos no nome) */
    scanf(" %49[^\n]", novo.nome);
    /* o ';' e o separador do arquivo, entao nao pode estar no nome */
    if (strchr(novo.nome, ';') != NULL) {
        printf("O nome nao pode conter ';'\n");
        return;
    }

    printf("CPF (11 numeros, sem pontos e traco): ");
    scanf("%11s", novo.cpf);
    limparBuffer();
    if (!somenteDigitos(novo.cpf, 11)) {
        printf("CPF invalido! Digite 11 numeros.\n");
        return;
    }
    if (buscarCpf(novo.cpf) != -1) {
        printf("Ja existe um investidor com esse CPF!\n");
        return;
    }

    printf("Senha (6 numeros): ");
    scanf("%6s", novo.senha);
    limparBuffer();
    if (!somenteDigitos(novo.senha, 6)) {
        printf("Senha invalida! Digite exatamente 6 numeros.\n");
        return;
    }

    /* Saldos iniciais zerados */
    novo.reais = 0;
    for (i = 0; i < QTD_CRIPTO; i++) novo.cripto[i] = 0;

    usuarios[qtdUsuarios] = novo; /* coloca no fim do vetor */
    qtdUsuarios++;
    salvarUsuarios();             /* grava no arquivo */
    printf("Investidor cadastrado com sucesso! Ja pode fazer login.\n");
}

/* ================= LOGIN E MENU ================= */

/* Procura o usuario com CPF e senha informados.
   Retorna o endereco dele no vetor */
Usuario *login(void) {
    int i; 
    char cpf[20], senha[20];
    printf("CPF (somente numeros): ");
    scanf("%19s", cpf);
    printf("Senha (6 digitos): ");
    scanf("%19s", senha);

    for (i = 0; i < qtdUsuarios; i++) {
        if (strcmp(usuarios[i].cpf, cpf) == 0 &&
            strcmp(usuarios[i].senha, senha) == 0)
            return &usuarios[i];
    }
    return NULL;
}

int main(void) {
    srand(time(NULL)); /* semente aleatoria diferente a cada execucao */
    carregarUsuarios();
    carregarCotacoes();

    Usuario *u = NULL;
    while (u == NULL) {
        printf("\n=== CAMBIO DE CRIPTOMOEDAS DO JOTA ===\n");
        printf("1. Entrar\n");
        printf("2. Cadastrar novo investidor\n");
        printf("3. Sair\n");
        printf("Opcao: ");
        int inicio = (int)lerValor();

        if (inicio == 1) {
            u = login();
            if (u == NULL) printf("CPF ou senha incorretos!\n");
        } else if (inicio == 2) {
            cadastrar();
        } else if (inicio == 3) {
            printf("Ate logo!\n");
            return 0; /* encerra o programa */
        } else {
            printf("Opcao invalida!\n");
        }
    }
    printf("\nBem-vindo(a), %s!\n", u->nome);

    int opcao = 0;
    while (opcao != 8) {
        printf("\n----- MENU -----\n");
        printf("1. Consultar saldo\n");
        printf("2. Consultar extrato\n");
        printf("3. Depositar\n");
        printf("4. Sacar\n");
        printf("5. Comprar criptomoedas\n");
        printf("6. Vender criptomoedas\n");
        printf("7. Atualizar cotacao\n");
        printf("8. Sair\n");
        printf("Opcao: ");
        opcao = (int)lerValor();
        printf("\n----------------\n");
        
        if (opcao == 1) {
            consultarSaldo(u);
        } else if (opcao == 2) {
            consultarExtrato(u);
        } else if (opcao == 3) {
            depositar(u);
        } else if (opcao == 4) {
            sacar(u);
        } else if (opcao == 5) {
            comprar(u);
        } else if (opcao == 6) {
            vender(u);
        } else if (opcao == 7) {
            atualizarCotacao();
        } else if (opcao == 8) {
            printf("Saindo... ate logo!\n");
        } else {
            printf("Opcao invalida!\n");
        }
    }
    return 0;
}
