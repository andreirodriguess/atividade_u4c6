#include <stdio.h>           // Biblioteca padrão para entrada e saída de dados
#include <stdlib.h>          // Biblioteca padrão para alocação de memória e conversões
#include <math.h>            // Biblioteca matemática padrão
#include "pico/stdlib.h"     // Biblioteca do Raspberry Pi Pico para GPIO, temporização e comunicação serial
#include "hardware/i2c.h"    // Biblioteca para comunicação via I2C
#include "inc/ssd1306.h"     // Biblioteca para controle do display OLED SSD1306
#include "hardware/clocks.h" // Biblioteca para controle do clock do sistema
#include "hardware/pio.h"    // Biblioteca para uso do periférico PIO

// Inclui o código PIO compilado externamente
#include "atividade_u4c6.pio.h"

// Biblioteca de fontes para o display OLED
#include "inc/font.h"

// Configuração do barramento I2C e endereço do display OLED
#define I2C_PORT i2c1
#define I2C_SDA 14
#define I2C_SCL 15
#define endereco 0x3C

// Número total de LEDs na matriz WS2812
#define NUM_PIXELS 25
// Pino de saída para o WS2812
#define OUT_PIN 7

// Definição dos pinos dos LEDs RGB
#define red_pin 13
#define blue_pin 12
#define green_pin 11

// Definição dos pinos dos botões
#define button_a 5
#define button_b 6

// Variáveis globais para controle do estado dos LEDs
static volatile uint32_t last_time = 0; // Armazena o tempo do último evento (usado para debounce)
bool green_state;                       // Estado do LED verde (ligado/desligado)
bool blue_state;                        // Estado do LED azul (ligado/desligado)

// Variável para armazenar o número apresentado na matriz de LEDs
int numero_apresentado = 4;

// Função para converter valores RGB para o formato WS2812 (8 bits por canal)
uint32_t matrix_rgb(double b, double r, double g)
{
    unsigned char R, G, B;
    R = r * 255;
    G = g * 255;
    B = b * 255;
    return (G << 24) | (R << 16) | (B << 8);
}

// Definição dos padrões numéricos para a matriz de LEDs (5x5)
double padrao_0[25] = {0.0, 0.8, 0.8, 0.8, 0.0,
                       0.0, 0.8, 0.0, 0.8, 0.0,
                       0.0, 0.8, 0.0, 0.8, 0.0,
                       0.0, 0.8, 0.0, 0.8, 0.0,
                       0.0, 0.8, 0.8, 0.8, 0.0};
double padrao_1[25] = {0.0, 0.0, 0.8, 0.0, 0.0,
                       0.0, 0.0, 0.8, 0.8, 0.0,
                       0.0, 0.0, 0.8, 0.0, 0.0,
                       0.0, 0.0, 0.8, 0.0, 0.0,
                       0.0, 0.0, 0.8, 0.0, 0.0};

double padrao_2[25] = {0.0, 0.8, 0.8, 0.8, 0.0,
                       0.0, 0.8, 0.0, 0.0, 0.0,
                       0.0, 0.8, 0.8, 0.8, 0.0,
                       0.0, 0.0, 0.0, 0.8, 0.0,
                       0.0, 0.8, 0.8, 0.8, 0.0};

double padrao_3[25] = {0.0, 0.8, 0.8, 0.8, 0.0,
                       0.0, 0.8, 0.0, 0.0, 0.0,
                       0.0, 0.8, 0.8, 0.8, 0.0,
                       0.0, 0.8, 0.0, 0.0, 0.0,
                       0.0, 0.8, 0.8, 0.8, 0.0};

double padrao_4[25] = {0.0, 0.8, 0.0, 0.8, 0.0,
                       0.0, 0.8, 0.0, 0.8, 0.0,
                       0.0, 0.8, 0.0, 0.8, 0.0,
                       0.8, 0.8, 0.8, 0.8, 0.0,
                       0.0, 0.0, 0.0, 0.8, 0.0};

double padrao_5[25] = {0.0, 0.8, 0.8, 0.8, 0.0,
                       0.0, 0.0, 0.0, 0.8, 0.0,
                       0.0, 0.8, 0.8, 0.8, 0.0,
                       0.0, 0.8, 0.0, 0.0, 0.0,
                       0.0, 0.8, 0.8, 0.8, 0.0};

double padrao_6[25] = {0.0, 0.8, 0.8, 0.8, 0.0,
                       0.0, 0.0, 0.0, 0.8, 0.0,
                       0.0, 0.8, 0.8, 0.8, 0.0,
                       0.0, 0.8, 0.0, 0.8, 0.0,
                       0.0, 0.8, 0.8, 0.8, 0.0};

double padrao_7[25] = {0.0, 0.8, 0.8, 0.8, 0.0,
                       0.0, 0.8, 0.0, 0.0, 0.0,
                       0.0, 0.0, 0.0, 0.8, 0.0,
                       0.0, 0.8, 0.0, 0.0, 0.0,
                       0.0, 0.0, 0.0, 0.8, 0.0};

double padrao_8[25] = {0.0, 0.8, 0.8, 0.8, 0.0,
                       0.0, 0.8, 0.0, 0.8, 0.0,
                       0.0, 0.8, 0.8, 0.8, 0.0,
                       0.0, 0.8, 0.0, 0.8, 0.0,
                       0.0, 0.8, 0.8, 0.8, 0.0};

double padrao_9[25] = {0.0, 0.8, 0.8, 0.8, 0.0,
                       0.0, 0.8, 0.0, 0.8, 0.0,
                       0.0, 0.8, 0.8, 0.8, 0.0,
                       0.0, 0.8, 0.0, 0.0, 0.0,
                       0.0, 0.8, 0.8, 0.8, 0.0};

// Array contendo os padrões de todos os números
double *padroes[] = {padrao_0, padrao_1, padrao_2, padrao_3, padrao_4,
                     padrao_5, padrao_6, padrao_7, padrao_8, padrao_9};

// Função para exibir um número na matriz WS2812
void apresentar_numero(PIO pio, uint sm, double r, double g, double b, int numero_apresentado)
{
    uint32_t valor_led;
    for (int j = 0; j < NUM_PIXELS; j++)
    {
        valor_led = matrix_rgb(b, r = padroes[numero_apresentado][24 - j], g); // Aplica o padrão do número selecionado
        pio_sm_put_blocking(pio, sm, valor_led);
    }
}

// Função para limpar a matriz de LEDs (desligar todos os LEDs)
void limpa_matriz(PIO pio, uint sm)
{
    uint32_t valor_led;
    for (int j = 0; j < NUM_PIXELS; j++)
    {
        valor_led = matrix_rgb(0.0, 0.0, 0.0); // Define todos os LEDs como apagados
        pio_sm_put_blocking(pio, sm, valor_led);
    }
}

// Declaração da rotina de interrupção para os botões
static void gpio_irq_handler(uint gpio, uint32_t events);

// Estrutura para controlar o display OLED
ssd1306_t ssd;

// Função principal
int main()
{
    PIO pio = pio0;
    uint16_t i;
    uint32_t valor_led;

    // Inicializa a comunicação serial para debug via USB
    stdio_init_all();

    // Configuração da PIO para controle da matriz WS2812
    uint offset = pio_add_program(pio, &main_program);
    uint sm = pio_claim_unused_sm(pio, true);
    main_program_init(pio, sm, offset, OUT_PIN);

    // Inicialização dos LEDs
    gpio_init(red_pin);
    gpio_init(blue_pin);
    gpio_init(green_pin);

    gpio_set_dir(red_pin, GPIO_OUT);
    gpio_set_dir(blue_pin, GPIO_OUT);
    gpio_set_dir(green_pin, GPIO_OUT);

    // Inicialização dos botões
    gpio_init(button_a);
    gpio_set_dir(button_a, GPIO_IN);
    gpio_pull_up(button_a);

    gpio_init(button_b);
    gpio_set_dir(button_b, GPIO_IN);
    gpio_pull_up(button_b);

    // Configuração das interrupções para os botões
    gpio_set_irq_enabled_with_callback(button_a, GPIO_IRQ_EDGE_FALL, 1, &gpio_irq_handler);
    gpio_set_irq_enabled_with_callback(button_b, GPIO_IRQ_EDGE_FALL, 1, &gpio_irq_handler);

    // Inicialização do barramento I2C para o display OLED
    i2c_init(I2C_PORT, 400 * 1000);
    gpio_set_function(I2C_SDA, GPIO_FUNC_I2C);
    gpio_set_function(I2C_SCL, GPIO_FUNC_I2C);
    gpio_pull_up(I2C_SDA);
    gpio_pull_up(I2C_SCL);

    // Inicializa o display OLED SSD1306
    ssd1306_init(&ssd, WIDTH, HEIGHT, false, endereco, I2C_PORT);
    ssd1306_config(&ssd);
    ssd1306_send_data(&ssd);

    // Limpa o display na inicialização
    ssd1306_fill(&ssd, false);
    ssd1306_send_data(&ssd);

    // Loop principal aguardando entrada do usuário via USB
    while (true)
    {
        char c;
        if (stdio_usb_connected()) // Verifica se o USB está conectado
        {
            printf("Digite um caractere: \n");
            if ((c = getchar())) // Lê um caractere do terminal
            {
                printf("Recebido: '%c'\n", c);
                apresentar_display(ssd, "Caractere lido: ", c, " ");

                if (c >= '0' && c <= '9') // Se for um número de 0 a 9
                {
                    numero_apresentado = c - '0';
                    printf("Número apresentado: %d\n", numero_apresentado);
                    apresentar_numero(pio, sm, 1.0, 0.0, 0.0, numero_apresentado);
                }
                else // Se não for um número, apaga a matriz
                {
                    limpa_matriz(pio, sm);
                }
            }
        }
    }
}

// Rotina de interrupção para alternar os LEDs RGB ao pressionar os botões
static void gpio_irq_handler(uint gpio, uint32_t events)
{
    uint32_t current_time = to_us_since_boot(get_absolute_time());

    // Implementação de debounce (evita leituras errôneas causadas por oscilações mecânicas no botão)
    if (current_time - last_time > 300000) // 300 ms
    {
        last_time = current_time;

        if (gpio == button_a) // Se o botão A for pressionado, alterna o LED verde
        {
            gpio_put(green_pin, !green_state);
            green_state = !green_state;
            apresentar_display(ssd, " Alterando", 'O', " Led verde...");
            printf("Alternando LED verde\n");
        }
        else if (gpio == button_b) // Se o botão B for pressionado, alterna o LED azul
        {
            gpio_put(blue_pin, !blue_state);
            blue_state = !blue_state;
            printf("Alternando LED azul\n");
            apresentar_display(ssd, " Alterando", 'O', " Led azul...");
        }
    }
}
