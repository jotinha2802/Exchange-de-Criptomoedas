# 🪙 Exchange de Criptomoedas

Simulação de uma corretora de criptomoedas no terminal, desenvolvida em **linguagem C** para a disciplina **CCP230 – Desenvolvimento de Algoritmos**, do 2º semestre de Ciência da Computação na **FEI**.

O investidor faz login com CPF e senha, movimenta saldo em reais e compra e vende **Bitcoin (BTC)**, **Ethereum (ETH)** e **Ripple (XRP)**. Cotações, saldos e extratos ficam salvos em arquivos `.txt`.

---

## ✨ Funcionalidades

- 🔐 **Login** com CPF e senha
- 💰 **Consultar saldo** em reais, BTC, ETH e XRP
- 📜 **Extrato** com o histórico de operações do investidor
- ➕ **Depósito** de reais
- ➖ **Saque** de reais
- 🛒 **Compra de criptomoedas:** o usuário informa o valor em reais e a taxa é cobrada por cima
- 💸 **Venda de criptomoedas:** o usuário informa a quantidade de moedas e a taxa é descontada do valor recebido
- 📈 **Atualizar cotação** das criptomoedas

---

## 🗂️ Estrutura de arquivos

| Arquivo | Função |
|---|---|
| `Projeto.c` | Código-fonte completo e comentado |
| `usuarios.txt` | Investidores cadastrados: `cpf;senha;nome;reais;btc;eth;xrp` |
| `cotacoes.txt` | Cotação atual de cada criptomoeda |
| `extrato_<CPF>.txt` | Histórico de operações de cada investidor (gerado pelo programa) |

---

## ▶️ Como executar

Pré-requisito: compilador **GCC**.

```bash
git clone https://github.com/SEU-USUARIO/exchange-criptomoedas.git
cd exchange-criptomoedas
gcc -Wall -Wextra Projeto.c -o Projeto
./Projeto
```

> No Windows (PowerShell), rode com `.\Projeto`

### 🔑 Login de teste

| CPF | Senha |
|---|---|
| `12345678900` | `123456` |

---

## 🛠️ Conceitos aplicados

- Funções e modularização
- Structs e `typedef`
- Ponteiros e passagem por referência
- Manipulação de strings (`string.h`)
- Leitura e escrita de arquivos (`fopen`, `fprintf`, `fscanf`)
- Persistência de dados entre execuções

---

👤 Desenvolvido por **João Pedro**, Ciência da Computação, FEI
