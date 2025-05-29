import firebase_admin
from firebase_admin import credentials, db
import csv

# Inicialize o Firebase
cred = credentials.Certificate("informations.json")
firebase_admin.initialize_app(cred, {
    "databaseURL": "https://nivel-sonoro-default-rtdb.firebaseio.com/"
})

day = "14-03-2025"

# Referência para os dados
ref = db.reference(f"{day}/dados_microfone")
dados = ref.get()

# Criar e salvar os dados em um arquivo CSV
with open("dados_microfone.csv", "w", newline="") as csvfile:
    writer = csv.writer(csvfile)
    writer.writerow(["Timestamp", "Hora", "Valor do Microfone"])

    for timestamp, valores in dados.items():
        if not isinstance(valores, dict):  # Verifica se é um dicionário
            print(f"Aviso: Dados inesperados em {timestamp}: {valores}")
            continue
        
        # Obtém os valores de forma segura
        data_hora = valores.get("data_hora", "Desconhecido")
        microfone = valores.get("microfone", "N/A")
        
        writer.writerow([timestamp, data_hora, microfone])

print("Arquivo CSV gerado com sucesso!")
