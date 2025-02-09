
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "pico/stdlib.h"
#include "hardware/i2c.h"
#include "inc/ssd1306.h"
#include "hardware/clocks.h"
#include "hardware/pio.h"
//incluindo arquivo pio
#include "atividade_u4c6.pio.h"
//arquivo com as fontes dos caracteres

#include "inc/font.h"
#define I2C_PORT i2c1
#define I2C_SDA 14
#define I2C_SCL 15
#define endereco 0x3C
// número de LEDs
#define NUM_PIXELS 25
// pino de saída
#define OUT_PIN 7
// pinos dos leds
#define red_pin 13
#define blue_pin 12
#define green_pin 11
// pinos do botoes
#define button_a 5
#define button_b 6

static volatile uint32_t last_time = 0; // Armazena o tempo do último evento (em microssegundos)
bool green_state; //armazena o estado do led verde
bool blue_state; //armazena o estado do led vermelho


// variavel para armazenar o numero apresentado
int numero_apresentado = 0;



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

// rotina da interrupção
static void gpio_irq_handler(uint gpio, uint32_t events);

ssd1306_t ssd; // Inicializa a estrutura do display

int main()
{
    PIO pio = pio0;
    uint16_t i;
    uint32_t valor_led;
     //
    stdio_init_all(); // Inicializa a comunicação com o terminal

    // configurações da PIO
    uint offset = pio_add_program(pio, &main_program);
    uint sm = pio_claim_unused_sm(pio, true);
    main_program_init(pio, sm, offset, OUT_PIN);
    /*funcoes dos botoes*/
        //inicializar leds
        gpio_init(red_pin);
        gpio_init(blue_pin);
        gpio_init(green_pin);
    
        //
        gpio_set_dir(red_pin, GPIO_OUT);
        gpio_set_dir(blue_pin, GPIO_OUT);
        gpio_set_dir(green_pin, GPIO_OUT);
        // inicializar o botão de interrupção - GPIO5
        gpio_init(button_a);
        gpio_set_dir(button_a, GPIO_IN);
        gpio_pull_up(button_a);
    
        // inicializar o botão de interrupção - GPIO6
        gpio_init(button_b);
        gpio_set_dir(button_b, GPIO_IN);
        gpio_pull_up(button_b);
    // interrupção da gpio habilitada
    gpio_set_irq_enabled_with_callback(button_a, GPIO_IRQ_EDGE_FALL, 1, &gpio_irq_handler);
    gpio_set_irq_enabled_with_callback(button_b, GPIO_IRQ_EDGE_FALL, 1, &gpio_irq_handler);

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
    apresentar_display(ssd, "abcdefghijk", " lmnopqrst ", "uvwxyz");
  }
}

// rotina de interrupção
static void gpio_irq_handler(uint gpio, uint32_t events)//alterna o estado do led rgb
{
    uint32_t current_time = to_us_since_boot(get_absolute_time());

    // Verifica se passou tempo suficiente desde o último evento (debouncing)
    if (current_time - last_time > 300000) // 300 ms 
    {
        last_time = current_time; // Atualiza o tempo do último evento

        if (gpio == button_a) // Se for o botão A, incrementa
        {
            gpio_put(green_pin, !green_state);
            green_state = !green_state;
            printf("alterando led verde\n");
        }
        else if (gpio == button_b) // Se for o botão B, decrementa
        {
            gpio_put(blue_pin, !blue_state);
            blue_state = !blue_state;
            printf("alterando led azul\n");
        }
   }
}