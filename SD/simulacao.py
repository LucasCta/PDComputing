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
        with open("saidas/S"+str(i)+".txt", "w+") as output_file:
            S = subprocess.Popen([comando, servidor], stdout=output_file, stderr=output_file, stdin=input_file)
            time.sleep(5)

for i in range(C):
    with open("entradas/C"+str(i)+".txt", "rb") as input_file:
        with open("saidas/C"+str(i)+".txt", "w+") as output_file:
            C = subprocess.Popen([comando, cliente], stdin=input_file, stderr=output_file, stdout=output_file)

print("Simulacao Concluida")