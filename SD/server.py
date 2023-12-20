from xmlrpc.server import SimpleXMLRPCServer
from xmlrpc.server import SimpleXMLRPCRequestHandler
import xmlrpc.client

import os
import json
import asyncio
import threading
import time
import statistics

nomeBanco = input("Digite o nome do Banco: \n")
with open('./bancos/'+nomeBanco+'.json', 'a+') as arquivo:
    arquivo.seek(0, 2)
    if arquivo.tell() == 0:
        porta = 8000 + len(os.listdir('./bancos'))
        contas = {"porta": [porta, {}]}
    else:
        arquivo.seek(0, 0)
        contas = json.load(arquivo)
porta = contas["porta"][0]

class RequestHandler(SimpleXMLRPCRequestHandler):
    rpc_paths = ('/RPC2','/'+nomeBanco)

class Banco():
    def __init__(self, nome):
        self.nome = nome
        self.contas = contas
        self.serverStartTime = time.time()
        self.numOp = 0
        self.meanRTT = 0
        self.maxOp = 600
        self.stdevRTT = 0
        self.lock = True
        self.salvaArquivo()
    def salvaArquivo(self):
        while self.lock == False:
            continue
        self.lock = False
        with open('./bancos/'+self.nome+'.json', 'w') as arquivo:
            json.dump(self.contas, arquivo)
        self.lock = True
    def cadastro(self, conta):
        if conta in self.contas:
            return "ERRO: Conta Já Existente"
        else:
            self.contas[conta] = [0, {}]
            self.salvaArquivo()
            return "Conta {} Criada".format(conta)
    def consulta(self, conta):
        if conta not in self.contas:
            return "ERRO: Conta Inexistente" 
        else:
            return "Conta {}: {}".format(conta, self.contas[conta])
    def deposito(self, conta, moedas):
        if conta not in self.contas:
            return "ERRO: Conta Inexistente" 
        elif moedas <= 0:
            return "ERRO: Valor Inválido"
        else:
            self.contas[conta][0] += moedas
            self.salvaArquivo()
            return "Deposito Efetuado, Saldo Atual {}: {}".format(conta, self.contas[conta][0])
    def transferencia(self, contaA, contaB, moedas):
        if moedas <= 0:
            return "ERRO: Valor Inválido"
        elif contaA not in self.contas:
            return "ERRO: Conta {} Inexistente".format(contaA)
        elif self.contas[contaA][0] < moedas:
            return "ERRO: Conta {} Nao Possui Saldo".format(contaA)
        elif contaB in self.contas: 
            self.contas[contaA][0] -= moedas
            self.contas[contaB][0] += moedas
            self.salvaArquivo()
            return "Tranferencia Local Efetuada com Sucesso"
        else:
            if contaA == "A":
                portO = 8001
            elif contaA == "B":
                portO = 8002
            else:
                portO = 8003
            if contaB == "A":
                portD = 8001
            elif contaB == "B":
                portD = 8002
            else:
                portD = 8003
            idTransacao = self.nome + str(len(self.contas[contaA][1]))
            self.contas[contaA][0] -= moedas 
            self.contas[contaA][1][idTransacao] = {
                "ID": idTransacao,
                "Origem": [contaA, portO, contaA],
                "Destino": [contaB, portD, contaB],
                "Quantia": moedas,
                "ACK": 0,
                "StartTime": time.time(),
                "EndTime": 0
            }
            self.salvaArquivo()
            return "Transacao entre 2 Bancos em Andamento"
    def acknowlege(self, transacao):
        if (transacao["ACK"] == 0):
            if not (transacao["ID"] in self.contas[transacao["Destino"][2]][1]):
                transacao["ACK"] = 1
                self.contas[transacao["Destino"][2]][1][transacao["ID"]] = transacao
        elif (transacao["ACK"] == 1):
            if not (self.contas[transacao["Origem"][2]][1][transacao["ID"]]["ACK"] >= 1):
                transacao["ACK"] = 2
                self.contas[transacao["Origem"][2]][1][transacao["ID"]] = transacao
        elif (transacao["ACK"] == 2):
            if (self.contas[transacao["Destino"][2]][1][transacao["ID"]]["ACK"] >= 2):
                transacao["ACK"] = 5
                self.contas[transacao["Destino"][2]][1][transacao["ID"]] = transacao
            else:
                transacao["ACK"] = 3
                self.contas[transacao["Destino"][2]][0] += transacao["Quantia"]
                self.contas[transacao["Destino"][2]][1][transacao["ID"]] = transacao
        elif (transacao["ACK"] == 3):
            if not (self.contas[transacao["Origem"][2]][1][transacao["ID"]]["ACK"] >= 3):
                transacao["ACK"] = 4
                transacao["EndTime"] = time.time() 
                self.contas[transacao["Origem"][2]][1][transacao["ID"]] = transacao
                self.numOp += 1
                self.meanRTT += transacao["EndTime"] - transacao["StartTime"] 
                if self.numOp == self.maxOp:
                    data = []
                    for conta in list(banco.contas):
                        if conta == "porta":
                            continue
                        for t in list(banco.contas[conta][1]):
                            transacao = banco.contas[conta][1][t]
                            data.append(transacao["EndTime"] - transacao["StartTime"])
                    self.stdevRTT = statistics.stdev(data)
                    if self.nome == "A":
                        v = 9
                    elif self.nome == "B":
                        v = 6
                    else:
                        v = 3
                    self.contas["porta"][1]["troughput"] = self.maxOp / (time.time() - self.serverStartTime - v)
                    self.contas["porta"][1]["meanRTT"] = self.meanRTT / self.maxOp
                    self.contas["porta"][1]["stdevRTT"] = self.stdevRTT
        self.salvaArquivo()
        return "Sending ACK"

async def pendingTransactions(banco):
    for conta in list(banco.contas):
        if conta == "porta":
            continue
        for t in list(banco.contas[conta][1]):
            transacao = banco.contas[conta][1][t]
            porta = transacao["Destino"][1] if transacao["Origem"][0] == banco.nome else transacao["Origem"][1]
            if transacao["ACK"] != 3 and transacao["ACK"] != 4:
                if transacao["ACK"] == 5:
                    transacao["ACK"] = 3
                try:
                    with xmlrpc.client.ServerProxy("http://localhost:"+str(porta)+"/RPC2") as s:
                        s.acknowlege(transacao)
                except Exception as e:
                    print("ACK Failed, Error Msg: {}".format(str(e)))

banco = Banco(nomeBanco)

def startBank():
    with SimpleXMLRPCServer(('localhost', porta), requestHandler=RequestHandler) as server:
        server.register_introspection_functions()
        server.register_instance(banco)
        server.serve_forever()

async def transactionLoop():
    while(True):
        await pendingTransactions(banco)

def startTransactions():
    new_loop = asyncio.new_event_loop()
    asyncio.set_event_loop(new_loop)
    new_loop.run_until_complete(transactionLoop())

threading.Thread(target=startBank).start()
threading.Thread(target=startTransactions).start()
