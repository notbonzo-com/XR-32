#ifndef SYMBOL_TABLE_HPP
#define SYMBOL_TABLE_HPP

#include <string>
#include <unordered_map>
#include <cstdint>
#include <optional>
#include <vector>
#include <ranges>
#include <coroutine>
#include <generator>
#include <iterator>
#include <format>

namespace Assembler {

/**
 * @brief Manages mapping of symbols to addresses.
 *
 * The SymbolTable class provides a way to map symbols (e.g., labels) to addresses.
 * It supports adding, retrieving, removing, and iterating over symbols with modern C++23 features.
 */
class SymbolTable {
public:
    /**
     * @brief Adds a new symbol to the table.
     *
     * If the symbol already exists, this will update the associated address.
     *
     * @param name The name of the symbol to add.
     * @param address The address associated with the symbol.
     */
    void addSymbol(const std::string& name, uint32_t address) {
        symbolMap[name] = address;
    }

    /**
     * @brief Retrieves the address associated with a symbol.
     *
     * @param name The name of the symbol to retrieve the address for.
     * @return An optional containing the address if the symbol exists, or std::nullopt otherwise.
     */
    std::optional<uint32_t> getSymbolAddress(const std::string& name) const {
        if (auto it = symbolMap.find(name); it != symbolMap.end()) {
            return it->second;
        }
        return std::nullopt;
    }

    /**
     * @brief Checks if a symbol exists in the table.
     *
     * @param name The name of the symbol to check for.
     * @return true if the symbol exists, false otherwise.
     */
    bool contains(const std::string& name) const {
        return symbolMap.contains(name);
    }

    /**
     * @brief Removes a symbol from the table.
     *
     * @param name The name of the symbol to remove.
     * @return true if the symbol was successfully removed, false if the symbol did not exist.
     */
    bool removeSymbol(const std::string& name) {
        return symbolMap.erase(name) > 0;
    }

    /**
     * @brief Clears all symbols from the table.
     */
    void clear() {
        symbolMap.clear();
    }

    /**
     * @brief Returns the number of symbols in the table.
     *
     * @return The number of symbols currently stored in the table.
     */
    std::size_t size() const {
        return symbolMap.size();
    }

    /**
     * @brief Checks if the symbol table is empty.
     *
     * @return true if the table is empty, false otherwise.
     */
    bool isEmpty() const {
        return symbolMap.empty();
    }

    /**
     * @brief Retrieves a range of all symbols in the table.
     *
     * @return A range of symbol names.
     */
    auto getAllSymbols() const {
        return symbolMap | std::views::keys;
    }

    /**
     * @brief Retrieves a range of all symbol-address pairs.
     *
     * @return A range of key-value pairs in the symbol table.
     */
    auto getAllEntries() const {
        return symbolMap | std::views::all;
    }

    /**
     * @brief Coroutine for iterating over symbol addresses.
     *
     * Allows usage of co_yield for address iteration.
     * @return An iterable generator of symbol addresses.
     */
    auto getAddressGenerator() const -> std::generator<uint32_t> {
        for (const auto& [name, address] : symbolMap) {
            co_yield address;
        }
    }

    /**
     * @brief Accesses or inserts a symbol.
     *
     * If the symbol does not exist, it is inserted with the default value of 0.
     * @param name The name of the symbol.
     * @return A reference to the address associated with the symbol.
     */
    uint32_t& operator[](const std::string& name) {
        return symbolMap[name];
    }

    /**
     * @brief Checks for equality between two SymbolTable instances.
     *
     * @param other The other SymbolTable to compare with.
     * @return true if both SymbolTable instances have the same symbols and addresses, false otherwise.
     */
    bool operator==(const SymbolTable& other) const {
        return symbolMap == other.symbolMap;
    }

    /**
     * @brief Checks for inequality between two SymbolTable instances.
     *
     * @param other The other SymbolTable to compare with.
     * @return true if the SymbolTable instances are different, false otherwise.
     */
    bool operator!=(const SymbolTable& other) const {
        return !(*this == other);
    }

    /**
     * @brief Merges another SymbolTable into this one.
     *
     * Symbols in the other table will overwrite those in this table if they share the same name.
     * @param other The other SymbolTable to merge.
     * @return A reference to the modified SymbolTable.
     */
    SymbolTable& operator+=(const SymbolTable& other) {
        for (const auto& [name, address] : other.symbolMap) {
            symbolMap[name] = address;
        }
        return *this;
    }

    /**
     * @brief Returns a new SymbolTable that is the union of two SymbolTables.
     *
     * @param other The other SymbolTable to merge.
     * @return A new SymbolTable containing all entries from both tables.
     */
    SymbolTable operator+(const SymbolTable& other) const {
        SymbolTable result = *this;
        result += other;
        return result;
    }

private:
    /**
     * @brief The underlying map that stores the symbol-address pairs.
     */
    std::unordered_map<std::string, uint32_t> symbolMap;
};

} // namespace Assembler

// Formatter specialization for std::format to enable formatting of SymbolTable
template <>
struct std::formatter<Assembler::SymbolTable> : std::formatter<std::string> {
    auto format(const Assembler::SymbolTable& table, auto& ctx) {
        std::string output;
        for (const auto& [name, address] : table.getAllEntries()) {
            output += std::format("{}: {}\n", name, address);
        }
        return std::formatter<std::string>::format(output, ctx);
    }
};

#endif // SYMBOL_TABLE_HPP
