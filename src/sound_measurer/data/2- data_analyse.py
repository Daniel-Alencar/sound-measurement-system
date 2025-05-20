import pandas as pd
import matplotlib.pyplot as plt
import seaborn as sns

import os

# Cria a pasta 'images' se ela não existir
os.makedirs("graphs", exist_ok=True)

# Configura o estilo dos gráficos
sns.set(style="whitegrid")

# Carregar os dados (ajuste o caminho do arquivo, se necessário)
arquivo = "dados_microfone.csv"
df = pd.read_csv(arquivo)

# Tenta converter a coluna Hora para datetime, tratando erros
df["Hora"] = pd.to_datetime(df["Hora"], format="%H:%M:%S", errors="coerce")

# Remove as linhas onde a conversão falhou (Hora virou NaT)
df = df.dropna(subset=["Hora"])

# Estatísticas descritivas
print("📊 Estatísticas Descritivas:")
print(df["Valor do Microfone"].describe())

# Identificar picos acima de 65 dB
picos = df[df["Valor do Microfone"] > 65]
print(f"\n🔺 Picos encontrados (> 65 dB): {len(picos)}")
print(picos)

# Gráfico de linha: Hora x Valor do Microfone
plt.figure(figsize=(10, 5))
plt.plot(df["Hora"], df["Valor do Microfone"], marker="o", linestyle="-", color="royalblue")
plt.title("Intensidade Sonora ao Longo do Tempo")
plt.xlabel("Hora")
plt.ylabel("Valor do Microfone (dB)")
plt.xticks(rotation=45)
plt.tight_layout()
plt.grid(True)
plt.savefig("graphs/grafico_linha.png")
plt.show()

# Histograma
plt.figure(figsize=(8, 5))
sns.histplot(df["Valor do Microfone"], bins=10, kde=True, color="darkgreen")
plt.title("Distribuição da Intensidade Sonora")
plt.xlabel("Valor do Microfone (dB)")
plt.ylabel("Frequência")
plt.tight_layout()
plt.savefig("graphs/histograma.png")
plt.show()

# Boxplot
plt.figure(figsize=(6, 4))
sns.boxplot(x=df["Valor do Microfone"], color="salmon")
plt.title("Boxplot da Intensidade Sonora")
plt.xlabel("Valor do Microfone (dB)")
plt.tight_layout()
plt.savefig("graphs/boxplot.png")
plt.show()
