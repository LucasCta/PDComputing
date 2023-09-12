from xmlrpc.server import SimpleXMLRPCServer
from xmlrpc.server import SimpleXMLRPCRequestHandler
import xmlrpc.client

import os
import json

nomeBanco = input("Digite o nome do Banco: ")
with open('./bancos/'+nomeBanco+'.json', 'a+') as arquivo:
    arquivo.seek(0, 2)
    if arquivo.tell() == 0:
        porta = 8000 + len(os.listdir('./bancos'))
        contas = {"porta": porta}
    else:
        arquivo.seek(0, 0)
        contas = json.load(arquivo)
porta = contas["porta"]

class RequestHandler(SimpleXMLRPCRequestHandler):
    rpc_paths = ('/RPC2','/'+nomeBanco)

class Banco():
    def __init__(self, nome):
        self.nome = nome
        self.contas = contas
        self.salvaArquivo()
    def salvaArquivo(self):
        with open('./bancos/'+self.nome+'.json', 'w') as arquivo:
            json.dump(self.contas, arquivo)
    def cadastro(self, conta):
        if conta in self.contas:
            return "ERRO: Conta Já Existente"
        else:
            self.contas[conta] = 0
            self.salvaArquivo()
            return "Conta {} Criada".format(conta)
    def consulta(self, conta):
        if conta not in self.contas:
            return "ERRO: Conta Inexistente" 
        else:
            return "Saldo {}: {}".format(conta, self.contas[conta])
    def deposito(self, conta, moedas):
        if conta not in self.contas:
            return "ERRO: Conta Inexistente" 
        elif moedas <= 0:
            return "ERRO: Valor Inválido"
        else:
            self.contas[conta] += moedas
            self.salvaArquivo()
            return "Deposito Efetuado, Saldo Atual {}: {}".format(conta, self.contas[conta])
    def transferencia(self, contaA, contaB, moedas):
        if contaA not in self.contas:
            return "ERRO: Conta {} Inexistente".format(contaA)
        elif contaB not in self.contas:
            bancos = os.listdir('./bancos')
            for i in range(0, len(bancos)):
                with open('./bancos/'+bancos[i]) as B:
                    contas = json.load(B)
                    if contaB in contas:
                        with xmlrpc.client.ServerProxy("http://localhost:"+str(contas["porta"])+"/RPC2") as s:
                            self.contas[contaA] -= moedas
                            s.deposito(contaB, moedas)
                            self.salvaArquivo()
                            return "Tranferencia de {} moedas Efetuada com Sucesso {} {}"\
                                    .format(moedas, self.consulta(contaA), s.consulta(contaB)) 
            return "ERRO: Conta {} Inexistente".format(contaB)
        elif moedas <= 0:
            return "ERRO: Valor Inválido"
        elif self.contas[contaA] < moedas:
            return "ERRO: Conta {} Nao Possui Saldo".format(contaA)
        else: 
            self.contas[contaA] -= moedas
            self.contas[contaB] += moedas
            self.salvaArquivo()
            return "Tranferencia de {} moedas Efetuada com Sucesso, Novos Saldos {}: {} {}: {}"\
                    .format(moedas, contaA, self.contas[contaA], contaB, self.contas[contaB]) 

with SimpleXMLRPCServer(('localhost', porta), requestHandler=RequestHandler) as server:
    server.register_introspection_functions()
    server.register_instance(Banco(nomeBanco))
    server.serve_forever()
