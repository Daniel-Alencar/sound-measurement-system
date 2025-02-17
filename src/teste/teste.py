import numpy as np
import matplotlib.pyplot as plt
from scipy.interpolate import interp1d, CubicSpline

# Dados fornecidos
adc_values = np.array([1419, 1434, 1456, 1480, 1500, 1520, 1650, 1900, 2100, 2300])
db_values = np.array([40, 45, 52, 56, 59, 61, 65, 70, 75, 80])

# Criando diferentes interpolações
linear_interp = interp1d(adc_values, db_values, kind='linear')
cubic_spline = CubicSpline(adc_values, db_values)

# Criando um conjunto de pontos para visualização mais suave
adc_fine = np.linspace(adc_values.min(), adc_values.max(), 200)
db_linear = linear_interp(adc_fine)
db_cubic = cubic_spline(adc_fine)

# Plotando os resultados
plt.figure(figsize=(8, 5))
plt.scatter(adc_values, db_values, color='red', label="Pontos originais")
plt.plot(adc_fine, db_linear, label="Interpolação Linear", linestyle="dashed")
plt.plot(adc_fine, db_cubic, label="Spline Cúbico", linestyle="solid")
plt.xlabel("ADC Values")
plt.ylabel("dB Values")
plt.legend()
plt.title("Comparação de Interpolações")
plt.grid()
plt.show()
