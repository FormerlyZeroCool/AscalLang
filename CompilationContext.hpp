/*
 * CompilationContext.hpp
 *
 *  Created on: Apr 13, 2022
 *      Author: andrewrubinstein
 */
#pragma once
#include "Object.hpp"
#include "string_view.hpp"
#include <vector>

class CompilationContext {
    public:
    struct Token {
        static inline uint_fast16_t null = 0, KEYWORD = 1, NUMBER = 2, OPERATOR = 3, VARIABLE = 4, DELIMITER = 5, KEYWORD_END = 6, CTXDELIMITER = 7, VARIABLE_PART = 8;
        string_view source;
        uint_fast32_t start = 0;
        uint_fast16_t type = 0;
        double constantValue = std::numeric_limits<double>::max();
        Token(string_view s, uint_fast32_t start, uint_fast16_t type): source(s), start(start), type(type) {}
        Token() {}
    };
    struct LocalRecord {
        static inline const uint8_t DOUBLE = 0, OWNED_OBJECT = 1, REFERENCED_OBJECT = 2;
        uint16_t stack_index = 0;
        uint8_t type = 0;
        uint8_t paramsCount = 0;
        bool isDouble() const
        {
            return type == LocalRecord::DOUBLE;
        }
        bool isOwnedObject() const
        {
            return type == LocalRecord::OWNED_OBJECT;
        }
        bool isRefedObject() const
        {
            return type == LocalRecord::REFERENCED_OBJECT;
        }
        bool isObject() const
        {
            return isOwnedObject() || isRefedObject();
        }
        LocalRecord() {}
        LocalRecord(uint16_t stack_index, uint8_t paramsCount, uint8_t type): stack_index(stack_index), type(type), paramsCount(paramsCount) {}
    };
    std::vector<Token> lastTokens;
    uint32_t currentToken = 0;
    stack<CompilationContext::Token> stack; 
    string_view source;
    Object &target;
    AscalExecutor &runtime;
    uint64_t lastVarIndex = 0;
    uint64_t paramsCount = 0;
    FlatMap<string_view, LocalRecord > localMemory;
    int32_t src_index;
    CompilationContext(string_view src, Object &target, AscalExecutor &runtime): target(target), source(src),
        runtime(runtime), src_index(0), localMemory()
    {
        //target.resizeInstructions(src.length() + Object::SMALL_EXP);
        this->target.clearList();
        this->target.shrinkInstructions(0);
    }
    template <typename PLAIN_OLD_OBJECT>
    void writeToTargetAtByte(PLAIN_OLD_OBJECT &obj, uint32_t index)
    {
        memcpy(&this->target.getInstructions()[index], &obj, sizeof(obj));
    }
    uint32_t addOwnedLocal(string_view name, uint8_t paramsCount)
    {
        localMemory.insert(name, LocalRecord(lastVarIndex, paramsCount, LocalRecord::OWNED_OBJECT));
        return lastVarIndex++;
    }
    uint32_t addRefedLocal(string_view name, uint8_t paramsCount)
    {
        localMemory.insert(name, LocalRecord(lastVarIndex, paramsCount, LocalRecord::REFERENCED_OBJECT));
        return lastVarIndex++;
    }
    uint32_t addDoubleLocal(string_view name, uint8_t paramsCount)
    {
        localMemory.insert(name, LocalRecord(lastVarIndex, paramsCount, LocalRecord::DOUBLE));
        return lastVarIndex++;
    }
    template <typename t>
    void append(const t &val)
    {
        target.append(val);
    }
    uint32_t getSrcIndex(Token token)
    {
        uint32_t sourceIndex = 0;
        while(reinterpret_cast<std::uintptr_t>(&source[sourceIndex]) < reinterpret_cast<std::uintptr_t>(&token.source[0]))
            sourceIndex++;
        return sourceIndex;
    }
    void syncTokenIndexToSrcIndex()
    {
        const auto &tokens = this->lastTokens;
        while(tokens.size() > this->currentToken && reinterpret_cast<std::uintptr_t>(&tokens[this->currentToken].source.back()) <= reinterpret_cast<std::uintptr_t>(&this->source[this->src_index]))
        {
            this->currentToken++;
        } 
    }

};