#include "similaridade.hpp"
#include <iostream>

Similaridade::Similaridade(const ListaCompras& lista) {
    numClientes = lista.getCodigosClientes().size();
    numProdutos = lista.getCodigosProdutos().size();

    std::cout << "-> Construindo Matriz Densa de Compras A (" << numClientes << "x" << numProdutos << ")...\n";
    std::vector<std::vector<int>> matrizA(numClientes, std::vector<int>(numProdutos, 0));
    
    const auto& compras = lista.getComprasPorCliente();
    for (int i = 0; i < numClientes; ++i) {
        for (int produto : compras[i]) {
            matrizA[i][produto] = 1;
        }
    }

    std::cout << "-> Construindo Matriz Transposta AT (" << numProdutos << "x" << numClientes << ")...\n";
    std::vector<std::vector<int>> matrizAT(numProdutos, std::vector<int>(numClientes, 0));
    for (int i = 0; i < numClientes; ++i) {
        for (int j = 0; j < numProdutos; ++j) {
            matrizAT[j][i] = matrizA[i][j];
        }
    }

    std::cout << "-> Multiplicando matrizes para achar a Intersecao I = A * AT...\n";
    // Aviso: Esta etapa tem complexidade O(n^2 * m) e pode levar alguns segundos na base maior!
    std::vector<std::vector<int>> matrizI(numClientes, std::vector<int>(numClientes, 0));
    for (int i = 0; i < numClientes; ++i) {
        for (int j = 0; j < numClientes; ++j) {
            for (int k = 0; k < numProdutos; ++k) {
                matrizI[i][j] += matrizA[i][k] * matrizAT[k][j];
            }
        }
    }

    std::cout << "-> Calculando Matriz de Similaridade de Jaccard...\n";
    matrizSimilaridade.assign(numClientes, std::vector<double>(numClientes, 0.0));
    
    for (int i = 0; i < numClientes; ++i) {
        double tamanho_Pi = compras[i].size(); 
        
        for (int j = 0; j < numClientes; ++j) {
            if (i == j || tamanho_Pi == 0) {
                // Distancia do cliente para ele mesmo ou caso sem produtos comprados
                matrizSimilaridade[i][j] = 0.0;
            } else {
                double intersecao = matrizI[i][j];
                // Formula: s(i,j) = 1 - (|Pi int Pj| / |Pi|)
                matrizSimilaridade[i][j] = 1.0 - (intersecao / tamanho_Pi);
            }
        }
    }
}

double Similaridade::getSimilaridade(int indiceClienteA, int indiceClienteB) const {
    if (indiceClienteA >= 0 && indiceClienteA < numClientes && 
        indiceClienteB >= 0 && indiceClienteB < numClientes) {
        return matrizSimilaridade[indiceClienteA][indiceClienteB];
    }
    return -1.0; 
}

int Similaridade::getClienteMaisSimilar(int indiceCliente) const {
    if (indiceCliente < 0 || indiceCliente >= numClientes) return -1;

    int vizinhoMaisProximo = -1;
    double menorDistancia = 2.0; 

    for (int j = 0; j < numClientes; ++j) {
        if (indiceCliente != j) {
            double distancia = matrizSimilaridade[indiceCliente][j];
            // Queremos a menor distância possível e que seja válida (< 1.0)
            if (distancia < menorDistancia && distancia < 1.0) {
                menorDistancia = distancia;
                vizinhoMaisProximo = j;
            }
        }
    }
    return vizinhoMaisProximo;
}