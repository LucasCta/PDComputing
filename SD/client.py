import json
import xmlrpc.client

while True:
    banco = input("Qual Banco deseja se conectar? ")
    if banco == "sair":
        break
    with open('./bancos/'+banco+'.json') as arquivo:
        contas = json.load(arquivo)
        porta = contas["porta"]
        with xmlrpc.client.ServerProxy("http://localhost:"+str(porta)+"/"+banco) as s:
            while True:
                print("<<<< O que deseja fazer? >>>>")
                print("1- Cadastro")
                print("2- Deposito")
                print("3- Transferencia")
                print("4- Consultar")
                print("5- Sair")
                opt = int(input("Digite o numero: "))
                if opt == 1:
                    print(s.cadastro(input("Digite a conta: ")))
                elif opt == 2:    
                    conta = input("Digite a conta: ")
                    moedas = int(input("Digite o valor em moedas: "))
                    print(s.deposito(conta, moedas))
                elif opt == 3:
                    contaA = input("Digite a conta que ira transferir: ")
                    contaB = input("Digite a conta que ira receber: ")
                    moedas = int(input("Digite o valor em moedas: "))
                    print(s.transferencia(contaA, contaB, moedas))
                elif opt == 4:
                    print(s.consulta(input("Digite a conta: ")))
                elif opt == 5:
                    print("Saindo do terminal")
                    break
                else:
                    print("Opcao Invalida")
        
