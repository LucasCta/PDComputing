# Simulação de Transações Clientes/Bancos Simultâneas
##### Linguagem: Python
##### Tecnologia: XMLRPC

### Como Funciona:

- `bancos/`  : Pasta onde cada banco terá um arquivo com as contas e os saldos de cada conta cadastrada neles 
- `entradas/`  : Pasta para escrever as operações que cada processo servidor e cliente irão realizar na simulação
- `saidas/` : Pasta onde serão escritos os outputs de cada processo cliente e servidor simulado
- `client.py` : Programa que inicia o processo cliente
    - Se conecta em um banco
    - Pode cadastrar uma nova conta
    - Pode fazer um deposito em uma conta
    - Pode realizar uma transferencia entre contas
    - Pode consultar o saldo de uma conta
- `server.py` : Programa que inicia o processo servidor
    - Inicia um servidor banco, caso o banco já exista, coleta os dados do banco em bancos/json, que contém contas e seus saldos e seu número de porta. O nome do arquivo corresponde nome do banco.
    - Pode realizar as operações listadas acima que o cliente solicitar, sempre atualizando em seu arquivo json
    - Quando o cliente deseja fazer uma transferência para uma conta que não existe neste banco, ele consulta os arquivos dos outros bancos. Caso encontre, se torna "cliente" do banco que possui a conta, e solicita a realização de um depósito, alem de subtrair da conta que está realizando a transferência.
- `simulacao.py`: Programa que inicia a simulação de clientes e servidores
    - O processo inicia N clientes e N servidores (bancos), que utilizarão as entradas da pasta "entradas" no padrão C0.txt e S0.txt, e escreverão a saída padrão em "saidas" no mesmo padrao
