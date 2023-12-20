import subprocess
import time

comando = "/usr/bin/python3"
servidor = "./server.py"
cliente = "./client.py"

CS = [comando, servidor]
SS = [comando, cliente]

B = 3
C = 6 

for i in range(B):
    with open("entradas/S"+str(i)+".txt", "rb") as input_file:
        with open("saidas/S"+str(i)+".txt", "w+") as output_file:
            S = subprocess.Popen([comando, servidor], stdout=output_file, stderr=output_file, stdin=input_file)
            time.sleep(3)

for i in range(C):
    with open("entradas/C"+str(i)+".txt", "rb") as input_file:
        with open("saidas/C"+str(i)+".txt", "w+") as output_file:
            C = subprocess.Popen([comando, cliente], stdin=input_file, stderr=output_file, stdout=output_file)
            time.sleep(1)

print("Simulacao Concluida")
