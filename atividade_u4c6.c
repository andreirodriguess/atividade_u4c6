
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "pico/stdlib.h"
#include "hardware/i2c.h"
#include "inc/ssd1306.h"
#include "hardware/clocks.h"
#include "inc/font.h"
#include "hardware/pio.h"
#define I2C_PORT i2c1
#define I2C_SDA 14
#define I2C_SCL 15
#define endereco 0x3C
// número de LEDs
#define NUM_PIXELS 25

// pino de saída
#define OUT_PIN 7

//incluindo arquivo pio
#include "atividade_u4c6.pio.h"


// rotina para definição da intensidade de cores do led
uint32_t matrix_rgb(double b, double r, double g)
{
    unsigned char R, G, B;
    R = r * 255;
    G = g * 255;
    B = b * 255;
    return (G << 24) | (R << 16) | (B << 8);
}

void animacao_quadrado_azul(PIO pio, uint sm, double r, double g, double b)
{
    uint32_t valor_led;
    const int delay_ms = 200; // Tempo entre os quadros da animação

    // Define os padrões para a animação (quadrado azul em diferentes posições)
    double padrao_1[25] = {1.0, 0.0, 0.0, 0.0, 0.0,
                           1.0, 0.0, 0.0, 0.0, 0.0,
                           1.0, 0.0, 0.0, 0.0, 0.0,
                           1.0, 0.0, 0.0, 0.0, 0.0,
                           1.0, 0.0, 0.0, 0.0, 0.0};

    double padrao_2[25] = {1.0, 1.0, 0.0, 0.0, 0.0,
                           1.0, 0.0, 0.0, 0.0, 0.0,
                           1.0, 0.0, 0.0, 0.0, 0.0,
                           1.0, 0.0, 0.0, 0.0, 0.0,
                           1.0, 1.0, 0.0, 0.0, 0.0};

    double padrao_3[25] = {1.0, 1.0, 1.0, 0.0, 0.0,
                           1.0, 0.0, 0.0, 0.0, 0.0,
                           1.0, 0.0, 0.0, 0.0, 0.0,
                           1.0, 0.0, 0.0, 0.0, 0.0,
                           1.0, 1.0, 1.0, 0.0, 0.0};

    double padrao_4[25] = {1.0, 1.0, 1.0, 1.0, 0.0,
                           1.0, 0.0, 0.0, 0.0, 0.0,
                           1.0, 0.0, 0.0, 0.0, 0.0,
                           1.0, 0.0, 0.0, 0.0, 0.0,
                           1.0, 1.0, 1.0, 1.0, 0.0};

    double padrao_5[25] = {1.0, 1.0, 1.0, 1.0, 1.0,
                           1.0, 0.0, 0.0, 0.0, 1.0,
                           1.0, 0.0, 0.0, 0.0, 1.0,
                           1.0, 0.0, 0.0, 0.0, 1.0,
                           1.0, 1.0, 1.0, 1.0, 1.0};

    // Sequência de padrões para a animação
    double *padroes[] = {padrao_1, padrao_2, padrao_3, padrao_4, padrao_5};
    int num_padroes = sizeof(padroes) / sizeof(padroes[0]);

    // Exibir os padrões em sequência
    for (int ciclo = 0; ciclo < 3; ciclo++) // Repetir a animação 3 vezes
    {
        for (int i = 0; i < num_padroes; i++)
        {
            for (int j = 0; j < NUM_PIXELS; j++)
            {
                valor_led = matrix_rgb(b = padroes[i][24 - j], r, g); // Usa o padrão atual
                pio_sm_put_blocking(pio, sm, valor_led);
            }
            sleep_ms(delay_ms); // Espera antes de passar para o próximo quadro
        }
    }
}


ssd1306_t ssd; // Inicializa a estrutura do display

int main()
{
  printf("Iniciando o programa\n");
    PIO pio = pio0;
    uint16_t i;
    uint32_t valor_led;
     //
    stdio_init_all(); // Inicializa a comunicação com o terminal

    // Inicializa todos os códigos stdio padrão que estão ligados ao binário.
    stdio_init_all();

    // configurações da PIO
    uint offset = pio_add_program(pio, &main_program);
    uint sm = pio_claim_unused_sm(pio, true);
    main_program_init(pio, sm, offset, OUT_PIN);  
  // I2C Initialisation. Using it at 400Khz.
  i2c_init(I2C_PORT, 400 * 1000);

  gpio_set_function(I2C_SDA, GPIO_FUNC_I2C); // Set the GPIO pin function to I2C
  gpio_set_function(I2C_SCL, GPIO_FUNC_I2C); // Set the GPIO pin function to I2C
  gpio_pull_up(I2C_SDA); // Pull up the data line
  gpio_pull_up(I2C_SCL); // Pull up the clock line
  
  ssd1306_init(&ssd, WIDTH, HEIGHT, false, endereco, I2C_PORT); // Inicializa o display
  ssd1306_config(&ssd); // Configura o display
  ssd1306_send_data(&ssd); // Envia os dados para o display

  // Limpa o display. O display inicia com todos os pixels apagados.
  ssd1306_fill(&ssd, false);
  ssd1306_send_data(&ssd);

  while (true)
  {
    animacao_quadrado_azul(pio, sm, 0.0, 0.0, 1.0);    
    apresentar_display(ssd, "BEM VINDO", "AO", "BITDOG LAB");
    sleep_ms(2000);
    apresentar_display(ssd, "OLA", "MUNDO", "EMBARCADO");
    sleep_ms(2000);
  }
}