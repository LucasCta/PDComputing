import subprocess
import time

comando = "/usr/bin/python3"
servidor = "/home/lucas/PDComputing/SD/server.py"
cliente = "/home/lucas/PDComputing/SD/client.py"

CS = [comando, servidor]
SS = [comando, cliente]

B = 2
C = 2

for i in range(B):
    with open("entradas/S"+str(i)+".txt", "rb") as input_file:
        S = subprocess.Popen([comando, servidor],stdout=subprocess.PIPE, stdin=input_file)
        time.sleep(2)

for i in range(C):
    with open("entradas/C"+str(i)+".txt", "rb") as input_file:
        C = subprocess.Popen([comando, cliente], stdin=input_file)
        C.wait()

print("Simulacao Concluida")
