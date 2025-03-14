import firebase_admin
from firebase_admin import credentials, db
import csv

# Inicialize o Firebase
cred = credentials.Certificate("project-informations.json")
firebase_admin.initialize_app(cred, {
    "databaseURL": "https://nivel-sonoro-default-rtdb.firebaseio.com/"
})

# Referência para os dados
ref = db.reference("07-03-2025/dados_microfone")
dados = ref.get()

# Criar e salvar os dados em um arquivo CSV
with open("dados_microfone.csv", "w", newline="") as csvfile:
  writer = csv.writer(csvfile)
  writer.writerow(["Timestamp", "Hora", "Valor do Microfone"])
  
  for timestamp, valores in dados.items():
      writer.writerow([timestamp, valores["data_hora"], valores["microfone"]])

print("Arquivo CSV gerado com sucesso!")
