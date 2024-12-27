

# Cálculo de Decibéis com Interpolação

Este projeto utiliza um microfone MAX9814 conectado ao ESP32 para medir níveis de som em decibéis (dB) a partir de valores analógicos obtidos pelo ADC. A interpolação foi empregada para fornecer uma conversão precisa entre os valores do ADC e os níveis sonoros em dB com base em dados experimentais. Visto que a transformação do valores ADC para o valor real de intesidade sonora em decibéis, não era uma conversão direta. Para encontrar os pontos de interpolação relacionando os valores adc com os valores em decibéis foi usado o app Sound Meter para encontrar os valores decibéis correspondentes. Com isso, 4 pontos foram identificados:

- 1690 (valor adc): 58db (nível sonoro correspondente)
- 2262 (valor adc): 70db (nível sonoro correspondente)
- 2305 (valor adc): 75db (nível sonoro correspondente)
- 2352 (valor adc): 80db (nível sonoro correspondente)

## 📋 Descrição do Projeto

O código realiza leituras do ADC (resolução de 12 bits) e converte os valores medidos para níveis de som em decibéis (dB). Como a relação entre os valores ADC e o nível sonoro não é linear, utilizamos diferentes funções para modelar a relação em diferentes intervalos de medição:

1. **Função Logarítmica:** Usada para valores ADC de **0 até 2050**. Esse modelo captura o comportamento inicial da relação ADC-dB com alta precisão.
2. **Função Linear:** Conecta suavemente os valores entre os pontos ADC de **2050 até 2262**. Essa função serve como um "ponte" entre os dois modelos principais.
3. **Função Polinomial de 2ª Ordem:** Utilizada para valores ADC de **2262 em diante**, ajustando o comportamento observado para níveis mais altos.

Essa abordagem de interpolação por intervalos garante transições suaves e maior precisão em todas as faixas de valores do ADC.

## ⚙️ Configuração do Sistema

- **Hardware:** ESP32 com microfone conectado ao GPIO35.
- **Resolução do ADC:** 12 bits (valores de 0 a 4095).
- **Intervalo de Tensão:** 0 a 3.3V.

## 🛠️ Funcionalidade do Código

1. **Leitura do ADC:** A cada ciclo, o ESP32 realiza a leitura de valores analógicos do microfone.
2. **Conversão para dB:** O valor do ADC é passado para a função conjunta de interpolação, que determina o nível sonoro correspondente.
3. **Exibição dos Resultados:** O nível sonoro é exibido no monitor serial e é emitido um alerta caso o som ultrapasse o limite configurado.

## 📊 Detalhes da Interpolação

### 1. Função Logarítmica
Usada para valores de 0 a 2050:
\[
y = a \cdot \log(x) + c
\]
**Constantes:**
- \( a = 56.79 \)
- \( c = -364.60 \)

### 2. Função Linear
Usada para valores entre 2050 e 2262:
\[
y = m \cdot x + b
\]
**Constantes:**
- Inclinação (\(m\)): Calculada com base nos pontos \(x = 2050\) e \(x = 2262\).
- Intercepto (\(b\)): Determinado pela continuidade da função.

### 3. Função Polinomial
Usada para valores acima de 2262:
\[
y = p_2 \cdot x^2 + p_1 \cdot x + p_0
\]
**Constantes:**
- \( p_2 = 0.000135 \)
- \( p_1 = -0.5123 \)
- \( p_0 = 538.10 \)

## 🚀 Como Executar o Código

1. Carregue o código na ESP32 usando a Arduino IDE.
2. Conecte o microfone ao GPIO35.
3. Abra o monitor serial para visualizar os níveis sonoros em decibéis.
4. Configure o limite de alerta na constante `NOISE_THRESHOLD` (padrão: 80 dB).

## 🌟 Resultados Esperados

O sistema calculará o nível de som com precisão em diferentes faixas, exibindo os valores em decibéis e emitindo alertas quando o som ultrapassar o limite configurado.
