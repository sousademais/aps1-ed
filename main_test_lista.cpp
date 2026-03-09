#include "listacompras.hpp"

#include <iostream>
#include <string>
#include <vector>

int main(int argc, char* argv[]) {
    if (argc < 2) {
        std::cerr << "Uso: ./teste_lista <arquivo_csv> [cliente1] [cliente2] [cliente3]\n";
        return 1;
    }

    const std::string caminhoCSV = argv[1];

    try {
        ListaCompras lista;
        lista.carregarCSV(caminhoCSV);
        lista.imprimirResumo();

        std::vector<std::string> clientesTeste;
        for (int i = 2; i < argc; ++i) {
            clientesTeste.push_back(argv[i]);
        }

        if (clientesTeste.empty()) {
            const auto& clientes = lista.getCodigosClientes();
            for (std::size_t i = 0; i < clientes.size() && i < 3; ++i) {
                clientesTeste.push_back(clientes[i]);
            }
            std::cout << "Nenhum codigo de cliente foi informado.\n";
            std::cout << "Vou testar automaticamente com os 3 primeiros clientes da base.\n\n";
        }

        int exibidos = 0;
        for (const auto& codigoCliente : clientesTeste) {
            lista.imprimirComprasDoCliente(codigoCliente);
            ++exibidos;
            if (exibidos == 3) break;
        }

        if (exibidos < 3) {
            std::cout << "Aviso: o testador exibiu menos de 3 clientes porque voce passou menos argumentos.\n";
        }

    } catch (const std::exception& e) {
        std::cerr << "Erro: " << e.what() << '\n';
        return 1;
    }

    return 0;
}