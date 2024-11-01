/*
 * modulo_ejemplo.c
 *
 *  Created on: [Fecha de creación]
 *      Author: [Tu nombre]
 */

#include "modulo_ejemplo.h"
#include "blink_control.h"
#include "timer.h"

//BlinkControl blink_control1;

/***********************************************
*        PASO 4: FUNCIONES DE TRANSICIÓN       *
* Definimos las funciones de transición que    *
* dependen de las condiciones para cambiar de  *
* estado.                                      *
***********************************************/

/**
 * @brief Condición para pasar del estado 1 al estado 2.
 */
static int condicion_estado1_a_estado2(void *context) {
	// Esta operacion de casteo se hace para poder acceder a los
	// miembros de la estructura que define a nuestro modulo.
    ModuloEjemplo *ejemplo = (ModuloEjemplo *)context;
    // Esta condición chequea que determinado pin se encuentre seteado
    return HAL_GPIO_ReadPin(ejemplo->Port, ejemplo->Pin2) == GPIO_PIN_SET;
}

/**
 * @brief Condición para pasar del estado 2 al estado 3.
 */
static int condicion_estado2_a_estado3(void *context) {
    ModuloEjemplo *ejemplo = (ModuloEjemplo *)context;
    return timer_has_expired(&ejemplo->timer1);
}

static int condicion_estado3_a_estado4(void *context) {
    ModuloEjemplo *ejemplo = (ModuloEjemplo *)context;
    return timer_has_expired(&ejemplo->timer1);
}

/**
 * @brief Condición para pasar del estado 3 al estado 1.
 */
static int condicion_estado4_a_estado1(void *context) {
    ModuloEjemplo *ejemplo = (ModuloEjemplo *)context;
    return timer_has_expired(&ejemplo->timer1);
}

/***********************************************
*          PASO 5: FUNCIONES DE ACCIÓN         *
* Aquí se definen las acciones que se ejecutan *
* al entrar en un nuevo estado.                *
***********************************************/

/**
 * @brief Acción al entrar en el estado 1.
 */
void on_state_estado1(void *context) {
    ModuloEjemplo *ejemplo = (ModuloEjemplo *)context;
    // Ejemplo de acción: Cambiar el estado del pin GPIO a RESET
    //HAL_GPIO_WritePin(ejemplo->Port, ejemplo->Pin, GPIO_PIN_SET);
    *ejemplo->periodo = 1;
    HAL_GPIO_WritePin(ejemplo->Port, ejemplo->Pin, GPIO_PIN_SET);
    *ejemplo->periodo2 = 2000000;
}

/**
 * @brief Acción al entrar en el estado 2.
 */
void on_state_estado2(void *context) {
    ModuloEjemplo *ejemplo = (ModuloEjemplo *)context;
    // Ejemplo de acción: Iniciar el timer asociado
    timer_start(&ejemplo->timer1, 2000); // Iniciar un timer de 5 segundos
    *ejemplo->periodo = 200;

}

void on_state_estado3(void *context) {
    ModuloEjemplo *ejemplo = (ModuloEjemplo *)context;
    // Ejemplo de acción: Iniciar el timer asociado
    timer_start(&ejemplo->timer1, 5000); // Iniciar un timer de 5 segundos
    *ejemplo->periodo = 40000;
    *ejemplo->periodo2 = 1;
}

void on_state_estado4(void *context) {
    ModuloEjemplo *ejemplo = (ModuloEjemplo *)context;
    // Ejemplo de acción: Iniciar el timer asociado
    timer_start(&ejemplo->timer1, 2000); // Iniciar un timer de 5 segundos
    *ejemplo->periodo2 = 200;

}
// Donde esta la definición de on_state_estado3 ??
// Cuando lleguemos al PASO7 vamos a enterarnos!


/***********************************************
*          PASO 6: DECLARAR TRANSICIONES       *
* Aquí declaramos las transiciones de estado,  *
* especificando las condiciones que hacen      *
* posible cambiar entre estados.               *
***********************************************/

// Transiciones desde ESTADO_1
static Transition Estado1Transitions[] = {
    {condicion_estado1_a_estado2, ESTADO_2},  // Transición al estado 2
};

// Transiciones desde ESTADO_2 (2 transiciones)
// El Estado 2 tiene "dos flechas salientes", es decir, puede transicionar
// a mas de un estado. Por ese motivo este array tiene dos entradas y no una.
static Transition Estado2Transitions[] = {
    {condicion_estado2_a_estado3, ESTADO_3},  // Transición al estado 3
};

// Transiciones desde ESTADO_3
static Transition Estado3Transitions[] = {
    {condicion_estado3_a_estado4, ESTADO_4}   // Transición al estado 1
};

static Transition Estado4Transitions[] = {
    {condicion_estado4_a_estado1, ESTADO_1}   // Transición al estado 1
};

/***********************************************
*           PASO 7: ENSAMBLAR EL MÓDULO        *
* Aquí se ensamblan los estados, las           *
* transiciones y las acciones correspondientes.*
***********************************************/

static FSMState ModuloEjemploEstados[] = {
    {Estado1Transitions, 1, on_state_estado1},  // Estado 1: tiene 1 transición y ejecuta una acción
    {Estado2Transitions, 1, on_state_estado2},  // Estado 2: tiene 2 transiciones y ejecuta una acción
    {Estado3Transitions, 1, on_state_estado3},
	{Estado4Transitions, 1, on_state_estado4}
};

/***********************************************
*         PASO 8: INICIALIZAR EL MÓDULO        *
* Aquí inicializamos el módulo con el estado   *
* inicial y el contexto adecuado.              *
***********************************************/

/**
 * @brief Inicializa el módulo.
 */
void modulo_ejemplo_init(ModuloEjemplo *ejemplo,GPIO_TypeDef *Port, uint16_t Pin3, uint16_t Pin2, uint16_t Pin,  Timer timer1, Timer timer2, int* p, int* p2, BlinkControl *blink) {
    // Iniciar la FSM en el estado 1
    fsm_init(&ejemplo->fsm, ModuloEjemploEstados, ESTADO_1, ejemplo);
    // Inicializar el puerto y pin GPIO
    ejemplo->Port = Port;
    ejemplo->Pin = Pin;
    ejemplo->Pin2 = Pin2;
    ejemplo->Pin3 = Pin3;
    ejemplo->periodo = p;
    ejemplo->periodo2 = p2;
    ejemplo->timer1 = timer1;
    ejemplo->timer2 = timer2;
    ejemplo->blinkcontrol = *blink;

}

/***********************************************
*         PASO 9: ACTUALIZAR EL MÓDULO         *
* Esta función se llama en cada ciclo para     *
* verificar las transiciones y actualizar el   *
* estado actual si corresponde.                *
***********************************************/

/**
 * @brief Actualiza el módulo.
 */
void modulo_ejemplo_update(ModuloEjemplo *ejemplo) {
    fsm_update(&ejemplo->fsm);  // Llamar a fsm_update para gestionar las transiciones
}
