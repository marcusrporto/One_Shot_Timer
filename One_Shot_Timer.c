#include <stdio.h>
#include "pico/stdlib.h"
#include "pico/time.h"

const uint LED_GREEN_PIN = 11;   // GPIO 11 para o primeiro LED
const uint LED_BLUE_PIN = 12;    // GPIO 12 para o segundo LED
const uint LED_RED_PIN = 13;     // GPIO 13 para o terceiro LED
const uint BUTTON_PIN = 5;       // GPIO 5 para o botão

bool bloqueio = false;
bool led_active[3] = {false, false, false};  // Array para armazenar o estado de 3 LEDs
int leds_pins[3] = {LED_GREEN_PIN, LED_BLUE_PIN, LED_RED_PIN};  //Array para armazenar os pinos dos 3 LEDs

// Função de callback para desligar os LEDs após o tempo programado
int64_t turn_off_callback(alarm_id_t id, void *user_data) {
    //Condição if para primeiro alarme, elsif para o segundo alarme e elsif para o terceiro alarme;
    if (led_active[0] == true) {
        gpio_put(LED_GREEN_PIN, false);  // Desliga o LED 1
        led_active[0] = false;
    } else if (led_active[1] == true) {
        gpio_put(LED_BLUE_PIN, false);  // Desliga o LED 2
        led_active[1] = false;
    } else if (led_active[2] == true) {
        gpio_put(LED_RED_PIN, false);  // Desliga o LED 3
        led_active[2] = false;
        bloqueio = false;              // Libera Bloqueio de botão.
    }

    // Retorna 0 para garantir que o alarme não se repita
    return 0;
}

int main() {
    stdio_init_all();

     // Inicializa e configura os pinos dos segmentos como saída
    for (int i = 0; i < 3; i++) {
        gpio_init(leds_pins[i]);
        gpio_set_dir(leds_pins[i], GPIO_OUT);
    }

    // Inicializa o pino do botão
    gpio_init(BUTTON_PIN);
    gpio_set_dir(BUTTON_PIN, GPIO_IN);
    gpio_pull_up(BUTTON_PIN);  // Habilita o resistor pull-up interno para o botão

    while (true) {
        // Verifica se o botão foi pressionado e se o sistema não está bloqueado
        if (gpio_get(BUTTON_PIN) == 0 && !bloqueio) {
            sleep_ms(50);  // Debounce do botão

            // Verifica novamente o estado do botão
            if (gpio_get(BUTTON_PIN) == 0) {
                
                // Inicializa e configura os pinos dos segmentos como saída
                for (int i = 0; i < 3; i++) {
                    gpio_put(leds_pins[i],true);
                    led_active[i] = true;
                }

                bloqueio = true;  // Bloqueia a execução para evitar múltiplas ativações

                // Agenda os alarmes para desligar os LEDs após 3, 6 e 9 segundos
                add_alarm_in_ms(3000, turn_off_callback, NULL, false);  // Dispara Evento Desliga LED 1 após 3 segundos
                add_alarm_in_ms(6000, turn_off_callback, NULL, false);  // Dispara Evento Desliga LED 2 após 6 segundos
                add_alarm_in_ms(9000, turn_off_callback, NULL, false);  // Dispara Evento Desliga LED 3 após 9 segundos
            }
        }

        // Introduz uma pequena pausa para reduzir o uso da CPU
        sleep_ms(10);
    }

    return 0;
}
