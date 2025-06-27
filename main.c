//Código para Controle de Estoque 
//Aluno: João Vitor dos Santos e Sousa

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define CADASTRADO 0
#define REMOVIDO 1
#define VENCIDO 2

//Principais estruturas

typedef struct Data{
    int dia, mes, ano;
}Data;

typedef struct Lote{
    char numLote[10];
    int qtd;
    Data validade;
    struct Lote *prox; 
}Lote;

typedef struct Produto{
    int id;
    char nome[35];
    struct Lote *primeiroLote; 
    struct Produto *prox; 
}Produto;

typedef struct Historico{
    int tipo;
    char nomeProd[35];
    char numLote[10];
    int qtd;
    struct Historico *prox;
}Historico;

//Pilha vazia
Historico *pilhaHistorico = NULL; 


//Estoque inicial (TESTE)
Lote l1_2 = {"S02", 150, {15, 9, 2026}, NULL};
Lote l1_1 = {"S01", 100, {10, 8, 2026}, &l1_2};
Lote l2_1 = {"P01", 45, {30, 6, 2025}, NULL};
Lote l3_1 = {"Q01", 12, {30, 6, 2025}, NULL};
//Lote vencido para teste
Lote l4_1 = {"L01", 3, {15, 5, 2024}, NULL};

Produto p4 = {4, "Leite", &l4_1, NULL};
Produto p3 = {3, "Queijo", &l3_1, &p4};
Produto p2 = {2, "Pão", &l2_1, &p3};
Produto p1 = {1, "Suco", &l1_1, &p2};

Produto *estoque = &p1;
int proxId = 5;

//Funcao para comparr datas
int comparar(Data d1, Data d2){
    if(d1.ano != d2.ano){
        return d1.ano - d2.ano;
    } 
    if(d1.mes != d2.mes){
       return d1.mes - d2.mes; 
    } 
    return d1.dia - d2.dia;
}

//Funcao para adicionar registro ao historico
void novoLog(int tipo, const char* nome, const char* num, int qtd){
    Historico *novoRegistro = (Historico *)malloc(sizeof(Historico));
    if(novoRegistro == NULL){
        printf("Erro ao alocar memoria\n");
        return;
    }
    novoRegistro->tipo = tipo;
    strcpy(novoRegistro->nomeProd, nome);
    strcpy(novoRegistro->numLote, num);
    novoRegistro->qtd = qtd;

    novoRegistro->prox = pilhaHistorico;
    pilhaHistorico = novoRegistro;
}

//Funcao para exibir estoque
void verProdutos(){
    printf("\n--- PRODUTOS EM ESTOQUE ---\n");
    if(estoque == NULL) {
        printf("Estoque vazio.\n");
        return;
    }
    time_t agora = time(NULL);
    struct tm hoje = *localtime(&agora);
    Data dataHoje;
    dataHoje.dia = hoje.tm_mday;
    dataHoje.mes = hoje.tm_mon + 1;
    dataHoje.ano = hoje.tm_year + 1900;
    printf("Data: %02d/%02d/%d\n", dataHoje.dia, dataHoje.mes, dataHoje.ano);

    Produto *prodAtual = estoque;
    while(prodAtual != NULL){
        printf("\n---Produto: %s (ID: %d):\n", prodAtual->nome, prodAtual->id);
        if(prodAtual->primeiroLote == NULL){
            printf("  Nenhum lote em estoque.\n");
        }else{
            Lote *loteAtual = prodAtual->primeiroLote;
            while(loteAtual != NULL){
                const char* status = "";
                if(comparar(loteAtual->validade, dataHoje) < 0){
                    status = "(VENCIDO)";
                }
                printf("  Lote: %-10s | Validade: %02d/%02d/%d | Qtd: %d %s\n",
                loteAtual->numLote, loteAtual->validade.dia, loteAtual->validade.mes,
                loteAtual->validade.ano, loteAtual->qtd, status);
                loteAtual = loteAtual->prox;
            }
        }
        prodAtual = prodAtual->prox;
    }
    printf("--- FIM DO ESTOQUE ---\n");
}

//Funcao para cadastrar produtos
void cadastrarProduto(){
    char nomeProd[35] = {0};
    char nomeLote[10] = {0};
    int qtd;
    Data validade;
    printf("\n--- CADASTRAR PRODUTO/LOTE ---\n");
    printf("Digite o nome do produto: ");
    scanf(" %s", nomeProd);

    Produto* existe = NULL;
    Produto* prodAtual = estoque;
    while(prodAtual != NULL){
        if(strcmp(prodAtual->nome, nomeProd) == 0){
            existe = prodAtual;
            break;
        }
        prodAtual = prodAtual->prox;
    }

    int idAlvo;
    if(existe != NULL){
        printf("Produto '%s' ja existe. Adicionando novo lote.\n", nomeProd);
        idAlvo = existe->id;
    }else{
        printf("Produto '%s' nao encontrado. Criando novo lote.\n", nomeProd);
        idAlvo = proxId;
        
        Produto *novoProd = (Produto *)malloc(sizeof(Produto));
        if(novoProd != NULL){
            novoProd->id = idAlvo;
            strcpy(novoProd->nome, nomeProd);
            novoProd->primeiroLote = NULL;
            novoProd->prox = NULL;
            if(estoque == NULL){
                estoque = novoProd;
            }else {
                Produto *tmp = estoque;
                while(tmp->prox != NULL) tmp = tmp->prox;
                tmp->prox = novoProd;
            }
        }else{
            printf("Erro ao alocar memoria\n");
            return;
        }
        proxId++;
    }

    printf("Digite o numero do lote (ex: L04): ");
    scanf("%s", nomeLote);
    printf("Digite a quantidade de produtos: ");
    scanf("%d", &qtd);
    printf("Digite a data de validade (DD MM AAAA): ");
    scanf("%d %d %d", &validade.dia, &validade.mes, &validade.ano);

    Produto *alvo = estoque;
    while(alvo != NULL && alvo->id != idAlvo){
        alvo = alvo->prox;
    }
    if(alvo != NULL){
        Lote *novoLote = (Lote *)malloc(sizeof(Lote));
        if (novoLote != NULL){
            strcpy(novoLote->numLote, nomeLote);
            novoLote->validade = validade;
            novoLote->qtd = qtd;
            novoLote->prox = NULL;
            
            if(alvo->primeiroLote == NULL || comparar(validade, alvo->primeiroLote->validade) < 0){
                novoLote->prox = alvo->primeiroLote;
                alvo->primeiroLote = novoLote;
            }else{
                Lote *lote_atual = alvo->primeiroLote;
                while(lote_atual->prox != NULL && comparar(validade, lote_atual->prox->validade) >= 0){
                    lote_atual = lote_atual->prox;
                }
                novoLote->prox = lote_atual->prox;
                lote_atual->prox = novoLote;
            }
            printf("Lote %s (Qtd: %d) adicionado ao produto '%s'.\n", nomeLote, qtd, nomeProd);
    
            novoLog(CADASTRADO, nomeProd, nomeLote, qtd);
        }else{
            printf("Erro ao alocar memoria\n");
        }
    }
}

//Funcao para remover produtos
void removerProduto(){
    int idProduto, qtd;
    printf("\n--- REMOVER PRODUTOS ---\n");
    printf("Produtos disponiveis:\n");
    Produto *p = estoque;
    if(!p){
        printf("  Nenhum produto cadastrado.\n");
    }
    while(p){
        printf("  ID: %d | Nome: %s\n", p->id, p->nome);
        p = p->prox;
    }
    printf("----------------------------------------\n");
    
    printf("Digite o ID do produto para ser removido: ");
    scanf("%d", &idProduto);
    printf("Digite a quantidade para ser removida: ");
    scanf("%d", &qtd);

    Produto *prod = estoque;
    while(prod != NULL && prod->id != idProduto){
        prod = prod->prox;
    }
    if(prod == NULL){
        printf("Produto com ID %d nao encontrado.\n", idProduto);
        return;
    }
    if(prod->primeiroLote == NULL){
        printf("Nenhum lote disponivel para '%s'.\n", prod->nome);
        return;
    }

    int removidoTotal = 0;
    Lote *loteAtual = prod->primeiroLote;
    Lote *ant = NULL;

    while(loteAtual != NULL && qtd > 0){
        int removerLote = (qtd < loteAtual->qtd) ? qtd : loteAtual->qtd;
        
        novoLog(REMOVIDO, prod->nome, loteAtual->numLote, removerLote);
        
        loteAtual->qtd -= removerLote;
        qtd -= removerLote;
        removidoTotal += removerLote;
        
        if(loteAtual->qtd == 0){
            Lote *tmp = loteAtual;
            if(ant == NULL) {
                prod->primeiroLote = loteAtual->prox;
            }else{
                ant->prox = loteAtual->prox;
            }
            loteAtual = loteAtual->prox;
            
            if(tmp != &l1_1 && tmp != &l1_2 && tmp != &l2_1 && tmp != &l3_1 && tmp != &l4_1){
                free(tmp);
            }
        }else{
            ant = loteAtual;
            loteAtual = loteAtual->prox;
        }
    }

    printf("%d unidade(s) do produto '%s' removidas.\n", removidoTotal, prod->nome);
    if(qtd > 0){
        printf("Erro ao remover %d unidades. Estoque insuficiente.\n", qtd);
    }
}

//Funcao para remover produtos vencidos
void removerVencidos(){
    time_t agora = time(NULL);
    struct tm hoje = *localtime(&agora);
    Data dataHoje;
    dataHoje.dia = hoje.tm_mday;
    dataHoje.mes = hoje.tm_mon + 1;
    dataHoje.ano = hoje.tm_year + 1900;

    int itensRemovidos = 0;
    Produto* prodAtual = estoque;
    while(prodAtual != NULL){
        Lote* loteAtual = prodAtual->primeiroLote;
        Lote* ant= NULL;
        
        while(loteAtual != NULL){
            if(comparar(loteAtual->validade, dataHoje) < 0){
                printf("\nRemovendo lote %s (Qtd: %d) do produto '%s' (VENCIDO).\n",
                loteAtual->numLote, loteAtual->qtd, prodAtual->nome);
                
                novoLog(VENCIDO, prodAtual->nome, loteAtual->numLote, loteAtual->qtd);
                
                itensRemovidos += loteAtual->qtd;
                
                Lote* removerLote = loteAtual;
                if(ant == NULL){
                    prodAtual->primeiroLote = loteAtual->prox;
                }else{
                    ant->prox = loteAtual->prox;
                }
                loteAtual = loteAtual->prox;
                
                if(removerLote != &l1_1 && removerLote != &l1_2 && removerLote != &l2_1 && removerLote != &l3_1 && removerLote != &l4_1){
                    free(removerLote);
                }
            } else{
                ant = loteAtual;
                loteAtual = loteAtual->prox;
            }
        }
        prodAtual = prodAtual->prox;
    }
    if(itensRemovidos == 0){
        printf("Nenhum item vencido encontrado.\n");
    }else{
        printf("\n%d produtos vencidos foram removidos.\n", itensRemovidos);
    }
}

//funcao para exibir historico de movimentações
void logHistorico(){
    printf("\n--- HISTORICO ---\n");
    
    if(pilhaHistorico == NULL){
        printf("Sem registros.\n");
        return;
    }

    Historico *atual = pilhaHistorico; //Topo da pilha
    while(atual != NULL){ //Percorre a pilha
        switch(atual->tipo){
            case CADASTRADO:
                printf("CADASTRADO: %d un. de '%s' no lote %s.\n",
                atual->qtd, atual->nomeProd, atual->numLote);
                break;
            case REMOVIDO:
                printf("REMOVIDO:   %d un. de '%s' removidas do lote %s.\n",
                atual->qtd, atual->nomeProd, atual->numLote);
                break;
            case VENCIDO:
                printf("VENCIDO:    %d un. de '%s' removidas do lote %s.\n",
                atual->qtd, atual->nomeProd, atual->numLote);
                break;
        }
        atual = atual->prox; //Vai para o próximo registro
    }
    printf("--- FIM DO HISTORICO ---\n");
}

//Funcao para liberar memoria
void liberar(){
    Produto *prodAtual = estoque;
    while(prodAtual != NULL){
        Lote *loteAtual = prodAtual->primeiroLote;
        while(loteAtual != NULL){
            Lote *tempLote = loteAtual;
            loteAtual = loteAtual->prox;
            if(tempLote != &l1_1 && tempLote != &l1_2 && tempLote != &l2_1 && tempLote != &l3_1 && tempLote != &l4_1){
                free(tempLote);
            }
        }
        
        Produto *tempProd = prodAtual;
        prodAtual = prodAtual->prox;
        if(tempProd != &p1 && tempProd != &p2 && tempProd != &p3 && tempProd != &p4){
            free(tempProd);
        }
    }
    
    estoque = NULL;
    
    Historico *logAtual = pilhaHistorico;
    while(logAtual != NULL){
        Historico *tempLog = logAtual;
        logAtual = logAtual->prox;
        free(tempLog);
    }
    pilhaHistorico = NULL;
    
    printf("\nMemoria liberada. Encerrar programa.\n");
}

int main(){
    int op = 0;
    
    //Menu de escolha
    do{
        printf("\n------------------------------\n");
        printf("-    CONTROLE DE ESTOQUE    -\n");
        printf("------------------------------\n");
        printf("1 - Ver estoque\n");
        printf("2 - Cadastrar produto\n");
        printf("3 - Remover produto\n");
        printf("4 - Remover itens vencidos\n");
        printf("5 - Mostrar historico\n");
        printf("6 - Sair\n");
        printf("----------------------------------------\n");
        printf("Escolha uma opcao: ");
        scanf("%d",&op);
       
        switch(op){
            case 1: 
                verProdutos();
                break;
            case 2: 
                cadastrarProduto();
                break;
            case 3: 
                removerProduto();
                break;
            case 4: 
                removerVencidos();
                break;
            case 5: 
                logHistorico();
                break;
            case 6: 
                liberar();
                break;
            default: 
                printf("Opcao invalida.\n");
                break;
        }
        
    }while(op!=6);
  
    return 0;
}