# Controle de Matriz de LEDs WS2812 e Display OLED SSD1306 com RP2040  

## Descrição  
Este projeto utiliza o **Raspberry Pi Pico (RP2040)** para controlar uma **matriz de LEDs WS2812** e um **display OLED SSD1306** via **I2C**. Além disso, botões físicos são usados para alternar os estados dos LEDs RGB. O sistema pode exibir números de 0 a 9 na matriz e mensagens no display OLED.  

### Funcionalidades  
- **Controle da Matriz de LEDs WS2812 (5x5):**  
  - Exibe números de **0 a 9** conforme entrada do usuário.  
  - Apaga a matriz se um caractere inválido for digitado.  
- **Interação via Terminal USB:**  
  - Usuário pode inserir um número no terminal para ser exibido na matriz.  
- **Display OLED SSD1306:**  
  - Mostra mensagens relacionadas à entrada do usuário.  
- **Botões físicos:**  
  - **Botão A:** Alterna o estado do **LED verde**.  
  - **Botão B:** Alterna o estado do **LED azul**.  
- **Debounce para botões:**  
  - Evita leituras errôneas de múltiplos pressionamentos.  

## Funcionamento  
1. **Na inicialização:**  
   - O display OLED é configurado e limpo.  
   - A matriz de LEDs WS2812 fica apagada.  
2. **Digitando um número (0-9) no terminal:**  
   - O número é exibido na matriz de LEDs.  
   - A mensagem correspondente aparece no display OLED.  
3. **Se um caractere inválido for digitado:**  
   - A matriz é apagada.  
4. **Interação com os botões:**  
   - Pressionar o **botão A** alterna o **LED verde**.  
   - Pressionar o **botão B** alterna o **LED azul**.  

### Aluno
**Andrei Luiz da Silva Rodrigues**

### Matrícula
**TIC370100444**