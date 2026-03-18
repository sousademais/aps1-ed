#ifndef LISTA_COMPRAS_HPP
#define LISTA_COMPRAS_HPP

#include <list>
#include <string>
#include <unordered_map>
#include <vector>

class ListaCompras {
private:
    std::vector<std::string> codigosClientes;
    std::unordered_map<std::string, int> mapaClientes;

    std::vector<std::string> codigosProdutos;
    std::unordered_map<std::string, int> mapaProdutos;

    std::vector<std::string> nomesProdutos;
    std::vector<std::list<int>> comprasPorCliente;

    static std::string trim(const std::string& s);
    static bool parseLinhaCSV(const std::string& linha,
                              std::string& dataCompra,
                              std::string& codCliente,
                              std::string& codProduto,
                              std::string& nomeProduto);

public:
    void carregarCSV(const std::string& caminhoArquivo);

    int getIndiceCliente(const std::string& codigoCliente) const;
    int getIndiceProduto(const std::string& codigoProduto) const;
    int getNumeroClientes() const;

    const std::vector<std::string>& getCodigosClientes() const;
    const std::vector<std::string>& getCodigosProdutos() const;
    const std::vector<std::string>& getNomesProdutos() const;
    const std::vector<std::list<int>>& getComprasPorCliente() const;

    std::vector<std::string> getProdutosCompradosPorCodigoCliente(const std::string& codigoCliente) const;

    void imprimirResumo() const;
    void imprimirComprasDoCliente(const std::string& codigoCliente) const;
};

#endif