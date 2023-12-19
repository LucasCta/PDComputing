import json
import xmlrpc.client

while True:
    banco = input("Qual Banco deseja se conectar? \n")
    if banco == "sair":
        break
    with open('./bancos/'+banco+'.json') as arquivo:
        contas = json.load(arquivo)
        porta = contas["porta"][0]
        with xmlrpc.client.ServerProxy("http://localhost:"+str(porta)+"/"+banco) as s:
            while True:
                print("<<<< O que deseja fazer? >>>>")
                print("1- Cadastro")
                print("2- Deposito")
                print("3- Transferencia")
                print("4- Consultar")
                print("5- Sair")
                print("6- Testes")
                opt = int(input("Digite o numero: \n"))
                if opt == 1:
                    print(s.cadastro(input("Digite a conta: \n")))
                elif opt == 2:    
                    conta = input("Digite a conta: \n")
                    moedas = int(input("Digite o valor em moedas: \n"))
                    print(s.deposito(conta, moedas))
                elif opt == 3:
                    contaA = input("Digite a conta que ira transferir: \n")
                    contaB = input("Digite a conta que ira receber: \n")
                    moedas = int(input("Digite o valor em moedas: \n"))
                    print(s.transferencia(contaA, contaB, moedas))
                elif opt == 4:
                    print(s.consulta(input("Digite a conta: \n")))
                elif opt == 5:
                    print("Saindo do terminal")
                    break
                elif opt == 6:
                    contaA = input("Digite a conta que ira transferir: \n")
                    contaB = input("Digite a conta que ira receber: \n")
                    moedas = int(input("Digite o valor em moedas: \n"))
                    for i in range(moedas):
                        print(s.transferencia(contaA, contaB, 1))
                else:
                    print("Opcao Invalida")
        
