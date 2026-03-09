#include "listacompras.hpp"

#include <fstream>
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <unordered_set>

std::string ListaCompras::trim(const std::string& s) {
    const std::string whitespace = " \t\r\n";
    const std::size_t inicio = s.find_first_not_of(whitespace);
    if (inicio == std::string::npos) return "";
    const std::size_t fim = s.find_last_not_of(whitespace);
    return s.substr(inicio, fim - inicio + 1);
}

bool ListaCompras::parseLinhaCSV(const std::string& linha,
                                 std::string& dataCompra,
                                 std::string& codCliente,
                                 std::string& codProduto,
                                 std::string& nomeProduto) {
    std::vector<std::string> campos;
    std::string atual;
    bool dentroDeAspas = false;

    for (std::size_t i = 0; i < linha.size(); ++i) {
        char c = linha[i];

        if (c == '"') {
            if (dentroDeAspas && i + 1 < linha.size() && linha[i + 1] == '"') {
                atual += '"';
                ++i;
            } else {
                dentroDeAspas = !dentroDeAspas;
            }
        } else if (c == ',' && !dentroDeAspas) {
            campos.push_back(atual);
            atual.clear();
        } else {
            atual += c;
        }
    }
    campos.push_back(atual);

    if (campos.size() < 4) return false;

    dataCompra = trim(campos[0]);
    codCliente = trim(campos[1]);
    codProduto = trim(campos[2]);
    nomeProduto = trim(campos[3]);
    return !(codCliente.empty() || codProduto.empty());
}

void ListaCompras::carregarCSV(const std::string& caminhoArquivo) {
    codigosClientes.clear();
    mapaClientes.clear();
    codigosProdutos.clear();
    mapaProdutos.clear();
    nomesProdutos.clear();
    comprasPorCliente.clear();

    {
        std::ifstream arquivo(caminhoArquivo);
        if (!arquivo.is_open()) {
            throw std::runtime_error("Nao foi possivel abrir o arquivo: " + caminhoArquivo);
        }

        std::string linha;
        std::getline(arquivo, linha); // cabeçalho

        while (std::getline(arquivo, linha)) {
            if (trim(linha).empty()) continue;

            std::string dataCompra, codCliente, codProduto, nomeProduto;
            if (!parseLinhaCSV(linha, dataCompra, codCliente, codProduto, nomeProduto)) {
                continue;
            }

            if (mapaClientes.find(codCliente) == mapaClientes.end()) {
                const int novoIndiceCliente = static_cast<int>(codigosClientes.size());
                mapaClientes[codCliente] = novoIndiceCliente;
                codigosClientes.push_back(codCliente);
            }

            if (mapaProdutos.find(codProduto) == mapaProdutos.end()) {
                const int novoIndiceProduto = static_cast<int>(codigosProdutos.size());
                mapaProdutos[codProduto] = novoIndiceProduto;
                codigosProdutos.push_back(codProduto);
                nomesProdutos.push_back(nomeProduto);
            }
        }
    }

    comprasPorCliente.resize(codigosClientes.size());
    std::vector<std::unordered_set<int>> produtosJaInseridos(codigosClientes.size());

    {
        std::ifstream arquivo(caminhoArquivo);
        if (!arquivo.is_open()) {
            throw std::runtime_error("Nao foi possivel reabrir o arquivo: " + caminhoArquivo);
        }

        std::string linha;
        std::getline(arquivo, linha); // cabeçalho

        while (std::getline(arquivo, linha)) {
            if (trim(linha).empty()) continue;

            std::string dataCompra, codCliente, codProduto, nomeProduto;
            if (!parseLinhaCSV(linha, dataCompra, codCliente, codProduto, nomeProduto)) {
                continue;
            }

            const int indiceCliente = mapaClientes.at(codCliente);
            const int indiceProduto = mapaProdutos.at(codProduto);

            // Evita produtos repetidos para o mesmo cliente.
            // Isso facilita a Atividade 2, que trabalha com matriz binaria de compras.
            if (produtosJaInseridos[indiceCliente].insert(indiceProduto).second) {
                comprasPorCliente[indiceCliente].push_back(indiceProduto);
            }
        }
    }
}

int ListaCompras::getIndiceCliente(const std::string& codigoCliente) const {
    auto it = mapaClientes.find(codigoCliente);
    return (it == mapaClientes.end()) ? -1 : it->second;
}

int ListaCompras::getIndiceProduto(const std::string& codigoProduto) const {
    auto it = mapaProdutos.find(codigoProduto);
    return (it == mapaProdutos.end()) ? -1 : it->second;
}

const std::vector<std::string>& ListaCompras::getCodigosClientes() const {
    return codigosClientes;
}

const std::vector<std::string>& ListaCompras::getCodigosProdutos() const {
    return codigosProdutos;
}

const std::vector<std::string>& ListaCompras::getNomesProdutos() const {
    return nomesProdutos;
}

const std::vector<std::list<int>>& ListaCompras::getComprasPorCliente() const {
    return comprasPorCliente;
}

std::vector<std::string> ListaCompras::getProdutosCompradosPorCodigoCliente(const std::string& codigoCliente) const {
    std::vector<std::string> resultado;
    const int indiceCliente = getIndiceCliente(codigoCliente);
    if (indiceCliente < 0) {
        return resultado;
    }

    for (int indiceProduto : comprasPorCliente[indiceCliente]) {
        resultado.push_back(nomesProdutos[indiceProduto]);
    }
    return resultado;
}

void ListaCompras::imprimirResumo() const {
    std::cout << "Resumo da base carregada:\n";
    std::cout << "- Total de clientes: " << codigosClientes.size() << '\n';
    std::cout << "- Total de produtos: " << codigosProdutos.size() << '\n';
    std::cout << "- Vetor de compras (tamanho): " << comprasPorCliente.size() << "\n\n";
}

void ListaCompras::imprimirComprasDoCliente(const std::string& codigoCliente) const {
    const int indiceCliente = getIndiceCliente(codigoCliente);
    if (indiceCliente < 0) {
        std::cout << "Cliente " << codigoCliente << " nao encontrado.\n";
        return;
    }

    std::cout << "Cliente " << codigoCliente << " (indice interno " << indiceCliente << ") comprou:\n";
    int contador = 1;
    for (int indiceProduto : comprasPorCliente[indiceCliente]) {
        std::cout << "  [" << contador++ << "] "
                  << nomesProdutos[indiceProduto]
                  << " (cod interno produto = " << indiceProduto
                  << ", cod original = " << codigosProdutos[indiceProduto] << ")\n";
    }
    std::cout << '\n';
}