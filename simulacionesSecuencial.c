#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>

#define NUM_SIMULACIONES 10000
#define Plazo 10

typedef enum { alto, bajo, estable } Estado;

// Matriz de transición de estados
double Matriz[3][3] = {
    {0.8, 0.1, 0.1},  // rendimiento alto
    {0.2, 0.6, 0.2},  // rendimiento bajo
    {0.1, 0.1, 0.8}   // rendimiento estable
};

// Rendimientos esperados y desviaciones estándar para cada estado
double returns[3] = {0.12, -0.05, 0.02};
double stdDevs[3] = {0.15, 0.10, 0.05};

// Función para actualizar estados
Estado getNextState(Estado estadoActual) {
    double r = (double)rand() / RAND_MAX;
    double cumulative = 0.0;
    for (int i = 0; i < 3; i++) {
        cumulative += Matriz[estadoActual][i];
        if (r < cumulative) {
            return (Estado)i;
        }
    }
    return estadoActual;
}

// Generar números aleatorios siguiendo la distribución normal
double generateNormal(double mean, double std) {
    double pi = atan2(1.0, 0.0) * 4.0;
    double u1, u2;
    do {
        u1 = (double)rand() / RAND_MAX;
    } while (u1 <= 0.0);
    u2 = (double)rand() / RAND_MAX;
    double z0 = sqrt(-2.0 * log(u1)) * cos(2.0 * pi * u2);
    return z0 * std + mean;
}

// Función para la simulación Monte Carlo mediante integrales
double monteCarloIntegracion(int numPuntos, double invInicial) {
    double a = 0.0;
    double b = 1.0;
    double suma = 0.0;

    for (int i = 0; i < numPuntos; ++i) {
        double x = a + (b - a) * ((double)rand() / RAND_MAX);
        double fx = exp(-x * x);
        suma += fx;
    }

    double integral = (b - a) * suma / numPuntos;
    double valorFinalInversion = invInicial * (1 + integral);  // valor final
    double rentabilidad = ((valorFinalInversion - invInicial) / invInicial) * 100;  // Rentabilidad en porcentaje
    return rentabilidad;
}

int main() {
    // Medir tiempo de ejecución
    clock_t start = clock();

    // Cargar Datos Históricos (Simulación simple para este ejemplo)
    double inversiones[3] = {10000.0, 5000.0, 8000.0};  // Valores históricos
    double rentabilidadesHistoricas[3] = {0.08, 0.05, 0.1};  // Rentabilidad histórica

    double invInicial = 10000.0;
    double rentabilidadIntegral = monteCarloIntegracion(NUM_SIMULACIONES, invInicial);
    printf("Resultado de la simulación con integrales: %.2f%%\n", rentabilidadIntegral);

    double finalValues[NUM_SIMULACIONES];

    // Simulación con cadenas de Markov (Secuencial)
    for (int i = 0; i < NUM_SIMULACIONES; ++i) {
        int seed = i;
        srand(seed);
        double investmentValue = invInicial;
        Estado currentState = estable;

        for (int year = 0; year < Plazo; ++year) {
            // Actualiza el estado basado en la matriz de transición
            currentState = getNextState(currentState);
            double annualReturn = generateNormal(returns[currentState], stdDevs[currentState]);
            investmentValue *= (1 + annualReturn);
        }

        finalValues[i] = investmentValue;
    }

    // Calcular rentabilidad promedio de la simulación con cadenas de Markov
    double averageReturn = 0;
    for (int i = 0; i < NUM_SIMULACIONES; ++i) {
        averageReturn += finalValues[i];
    }
    averageReturn /= NUM_SIMULACIONES;

    double rentabilidadMarkov = ((averageReturn - invInicial) / invInicial) * 100;

    // Unir resultados y determinar la incertidumbre
    double rentabilidadFinal = (rentabilidadIntegral + rentabilidadMarkov) / 2;
    double incertidumbre = fabs(rentabilidadIntegral - rentabilidadMarkov);

    // Imprimir resultados finales
    printf("Rentabilidad Promedio con Cadenas de Markov: %.2f%%\n", rentabilidadMarkov);
    printf("Rentabilidad Final Combinada: %.2f%%\n", rentabilidadFinal);
    printf("Incertidumbre entre ambos métodos: %.2f%%\n", incertidumbre);

    // Medir tiempo final y mostrar la diferencia
    clock_t end = clock();
    double tiempoTotal = (double)(end - start) / CLOCKS_PER_SEC;
    printf("Tiempo total de ejecución (sec): %.2f\n", tiempoTotal);

    return 0;
}
