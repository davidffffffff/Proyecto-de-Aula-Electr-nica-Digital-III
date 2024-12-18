/*
 * @file carrito.c
 * @brief Control de un carrito que esquiva obstáculos para llegar a una coordenada especificada en cm.
 * 
 * Este código permite controlar un carrito utilizando la Raspberry Pi Pico W. Utiliza sensores de ultrasonido 
 * para detectar obstáculos y un encoder para medir la distancia recorrida. Los motores se controlan con 
 * señales PWM y el carrito puede girar a la izquierda o derecha para esquivar los obstáculos.
 * 
 * @author [David Florez, Cristian Ibarra]
 * @date [18/12/2024]
 */



#include "pico/stdlib.h"
#include "hardware/pwm.h"
#include <stdio.h>

// Definición de pines
#define IN1 2        // Motor izquierdo - dirección
#define IN2 3        // Motor izquierdo - dirección
#define IN3 4        // Motor derecho - dirección
#define IN4 5        // Motor derecho - dirección
#define EN 6         // Pin único para control de velocidad (PWM)
#define ENCODER_PIN 16 // Pin del encoder

// Pines del sensor de ultrasonido
#define TRIG_FRONT 15  // Pin TRIG del sensor frontal
#define ECHO_FRONT 14  // Pin ECHO del sensor frontal
#define TRIG_LEFT 18   // Pin TRIG del sensor izquierdo
#define ECHO_LEFT 19   // Pin ECHO del sensor izquierdo
#define TRIG_RIGHT 20  // Pin TRIG del sensor derecho
#define ECHO_RIGHT 21  // Pin ECHO del sensor derecho

// Variables globales
volatile uint32_t pulse_count = 0;       // Contador de pulsos del encoder
const float wheel_circumference = 21.28; // Circunferencia de la llanta (cm)
const uint32_t pulses_per_revolution = 40; // Pulsos por vuelta del encoder
const float target_distance = 150.0;     // Distancia máxima a recorrer (en cm)
const float width = 13.5;                // Ancho del carrito (cm)
const float correction_factor = 0.40;    // Factor de corrección del giro (40%)
const uint32_t timeout_us = 40000;       // Timeout para ultrasonido (40 ms)
/**
 * @brief Callback para manejar interrupciones del encoder.
 * 
 * Incrementa el contador de pulsos cuando se detecta un flanco de subida en el pin del encoder.
 * 
 * @param gpio El pin que generó la interrupción.
 * @param events Los eventos que generaron la interrupción.
 */
// --- Callback del encoder ---
volatile uint32_t last_interrupt_time = 0;
void gpio_callback(uint gpio, uint32_t events) {
    uint32_t current_time = time_us_32(); // Tiempo actual en microsegundos
    if (gpio == ENCODER_PIN && (current_time - last_interrupt_time) > 1000) { // Filtro de 1 ms
        pulse_count++; // Incrementar el contador de pulsos
        last_interrupt_time = current_time;
    }
}
/**
 * @brief Inicializa los pines GPIO para los motores, encoder y sensores de ultrasonido.
 * 
 * Configura los pines de entrada/salida y habilita la interrupción del encoder.
 */
// --- Inicialización de GPIO ---
void init_gpio() {
    // Pines del motor
    gpio_init(IN1); gpio_set_dir(IN1, GPIO_OUT);
    gpio_init(IN2); gpio_set_dir(IN2, GPIO_OUT);
    gpio_init(IN3); gpio_set_dir(IN3, GPIO_OUT);
    gpio_init(IN4); gpio_set_dir(IN4, GPIO_OUT);

    // Pin PWM
    gpio_set_function(EN, GPIO_FUNC_PWM);

    // Pin del encoder
    gpio_init(ENCODER_PIN);
    gpio_set_dir(ENCODER_PIN, GPIO_IN);
    gpio_pull_down(ENCODER_PIN);
    gpio_set_irq_enabled_with_callback(ENCODER_PIN, GPIO_IRQ_EDGE_RISE, true, &gpio_callback);

    // Pines de ultrasonido
    gpio_init(TRIG_FRONT); gpio_set_dir(TRIG_FRONT, GPIO_OUT);
    gpio_init(ECHO_FRONT); gpio_set_dir(ECHO_FRONT, GPIO_IN);
    gpio_init(TRIG_LEFT); gpio_set_dir(TRIG_LEFT, GPIO_OUT);
    gpio_init(ECHO_LEFT); gpio_set_dir(ECHO_LEFT, GPIO_IN);
    gpio_init(TRIG_RIGHT); gpio_set_dir(TRIG_RIGHT, GPIO_OUT);
    gpio_init(ECHO_RIGHT); gpio_set_dir(ECHO_RIGHT, GPIO_IN);
}
/**
 * @brief Configura los motores para moverse hacia adelante.
 * 
 * Establece la dirección de los motores izquierdo y derecho.
 */
// --- Control de motores ---
void set_motor_forward() {
    gpio_put(IN1, 0); 
    gpio_put(IN2, 1);
    gpio_put(IN3, 0); 
    gpio_put(IN4, 1);
}
/**
 * @brief Detiene los motores del carrito.
 * 
 * Apaga todas las señales de control de los motores.
 */
void stop_motors() {
    gpio_put(IN1, 0); 
    gpio_put(IN2, 0);
    gpio_put(IN3, 0); 
    gpio_put(IN4, 0);
}
/**
 * @brief Configura la velocidad del motor usando PWM.
 * 
 * @param slice Número de slice de PWM.
 * @param speed Nivel de velocidad de PWM (0-65535).
 */
void set_motor_speed(uint slice, uint16_t speed) {
    pwm_set_gpio_level(slice, speed);
}
/**
 * @brief Mide la distancia utilizando un sensor de ultrasonido.
 * 
 * Envía una señal de disparo (TRIG) y mide el tiempo que tarda en regresar la señal (ECHO).
 * 
 * @param trig_pin Pin de disparo TRIG del sensor.
 * @param echo_pin Pin de recepción ECHO del sensor.
 * @return Distancia medida en cm.
 */
// --- Sensor de ultrasonido ---
float measure_distance(uint trig_pin, uint echo_pin) {
    // Generar el pulso de TRIG
    gpio_put(trig_pin, 0);
    sleep_us(2);
    gpio_put(trig_pin, 1);
    sleep_us(10);
    gpio_put(trig_pin, 0);

    // Esperar a que el pin ECHO se active
    uint32_t start_time = time_us_32();
    while (gpio_get(echo_pin) == 0) {
        if (time_us_32() - start_time > timeout_us) {
            return -1.0f; // Timeout
        }
    }

    // Medir el tiempo que el pin ECHO está activo
    uint32_t pulse_start = time_us_32();
    while (gpio_get(echo_pin) == 1) {
        if (time_us_32() - pulse_start > timeout_us) {
            return -1.0f; // Timeout
        }
    }
    uint32_t pulse_end = time_us_32();

    // Calcular la distancia
    uint32_t pulse_duration = pulse_end - pulse_start;
    return (pulse_duration * 0.0343) / 2.0; // Distancia en cm
}

// --- Función para giro de 90 grados hacia la izquierda ---
void turn_90_degrees_left() {
    uint32_t pulses_needed_turn = (uint32_t)((width * 3.14159) * correction_factor / wheel_circumference * pulses_per_revolution);

    // Girar los motores en direcciones opuestas
    gpio_put(IN1, 0); 
    gpio_put(IN2, 1); // Motor izquierdo hacia atrás
    gpio_put(IN3, 1); 
    gpio_put(IN4, 0); // Motor derecho hacia adelante

    pulse_count = 0;

    // Esperar a que el carrito complete el giro
    while (pulse_count < pulses_needed_turn) {
        tight_loop_contents(); // Esperar en un bucle vacío
    }

    // Detener los motores después del giro
    stop_motors();
}

// --- Función para giro de 90 grados hacia la derecha ---
void turn_90_degrees_right() {
    uint32_t pulses_needed_turn = (uint32_t)((width * 3.14159) * correction_factor / wheel_circumference * pulses_per_revolution);

    // Girar los motores en direcciones opuestas
    gpio_put(IN1, 1); 
    gpio_put(IN2, 0); // Motor izquierdo hacia adelante
    gpio_put(IN3, 0); 
    gpio_put(IN4, 1); // Motor derecho hacia atrás

    pulse_count = 0;

    // Esperar a que el carrito complete el giro
    while (pulse_count < pulses_needed_turn) {
        tight_loop_contents(); // Esperar en un bucle vacío
    }

    // Detener los motores después del giro
    stop_motors();
}
/**
 * @brief Función principal del programa.
 * 
 * Inicializa el sistema, configura el PWM, establece la dirección del motor y controla el movimiento del carrito 
 * para esquivar obstáculos detectados con sensores de ultrasonido.
 * 
 * @return int Código de retorno del programa.
 */
// --- Main ---
int main() {
    sleep_ms(500);
    stdio_init_all();
        /** 
     * @brief Configuración de la señal PWM.
     * 
     * Se configura el canal de la señal PWM para controlar los motores del carrito.
     */
    init_gpio();

    // Configuración PWM
    uint slice_num = pwm_gpio_to_slice_num(EN);
    pwm_set_wrap(slice_num, 65535);
    pwm_set_chan_level(slice_num, PWM_CHAN_A, 55000);
    pwm_set_enabled(slice_num, true);

    pulse_count = 0;
    uint32_t pulses_needed_forward = (uint32_t)((target_distance / wheel_circumference) * pulses_per_revolution);

    set_motor_forward();
        /** 
     * @brief Bucle principal de control.
     * 
     * Mide la distancia hacia adelante, izquierda y derecha para detectar obstáculos y 
     * decide la acción correspondiente (giro o avance) para esquivar los obstáculos.
     */
    while (1) {
        float front_distance = measure_distance(TRIG_FRONT, ECHO_FRONT);
        sleep_ms(50); // Evitar interferencia entre sensores
        float left_distance = measure_distance(TRIG_LEFT, ECHO_LEFT);
        sleep_ms(50); // Evitar interferencia entre sensores
        float right_distance = measure_distance(TRIG_RIGHT, ECHO_RIGHT);
            /**
         * @brief Verifica si hay un obstáculo frontal.
         * 
         * Si la distancia hacia adelante es menor o igual a 30 cm, el carrito se detiene y decide la dirección de giro 
         * basada en las distancias medidas a la izquierda y a la derecha.
         */
        if (front_distance > 0 && front_distance <= 30.0f) { // Obstáculo frontal
            stop_motors();
            sleep_ms(1000);

            if (left_distance > 0 && left_distance <= 30.0f && right_distance > 0 && right_distance <= 30.0f) {
                break;
            } else if (left_distance > 0 && left_distance <= 30.0f) {
                turn_90_degrees_right();
            } else if (right_distance > 0 && right_distance <= 30.0f) {
                turn_90_degrees_left();
            } else {
                turn_90_degrees_left();
            }

            set_motor_forward(); // Reanudar movimiento
        }
                /** 
         * @brief Verifica si se ha alcanzado la distancia objetivo. 
         * 
         * Si la cantidad de pulsos contados supera los pulsos necesarios para alcanzar la distancia objetivo, 
         * el carrito se detiene y finaliza el programa.
         */
        if (pulse_count >= pulses_needed_forward) { // Distancia alcanzada
            stop_motors();
            break;
        }
    }

    return 0;
}