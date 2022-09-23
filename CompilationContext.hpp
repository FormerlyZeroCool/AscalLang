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
        static inline uint_fast16_t null = 0, KEYWORD = 1, NUMBER = 2, OPERATOR = 3, VARIABLE = 4, DELIMITER = 5, KEYWORD_END = 6, CODE_BODY_DELIMITER = 7, VARIABLE_PART = 8;
        string_view source;
        uint_fast32_t start = 0;
        uint_fast16_t type = 0;
        double constantValue = std::numeric_limits<double>::max();
        Token(string_view s, uint_fast32_t start, uint_fast16_t type): source(s), start(start), type(type) {}
        Token(): type(null) {}
    };
    struct LocalRecord {
        static inline const uint8_t DOUBLE = 0, OWNED_OBJECT = 1, REFERENCED_OBJECT = 2;
        static inline const uint8_t DOUBLE_LIST = 1, OBJECT_LIST = 2;
        uint8_t stack_index = 0;
        uint8_t type = 0;
        uint8_t list = 0;
        bool isDouble() const noexcept
        {
            return type == LocalRecord::DOUBLE;
        }
        bool isOwnedObject() const noexcept
        {
            return type == LocalRecord::OWNED_OBJECT;
        }
        bool isRefedObject() const noexcept
        {
            return type == LocalRecord::REFERENCED_OBJECT;
        }
        bool isObject() const noexcept
        {
            return isOwnedObject() || isRefedObject();
        }
        bool isList() const noexcept
        {
            return this->list;
        }
        LocalRecord() {}
        LocalRecord(uint16_t stack_index, uint8_t paramsCount, uint8_t type): stack_index(stack_index), type(type) {}
    };
    std::vector<Token> lastTokens;
    uint32_t currentToken = 0;
    stack<CompilationContext::Token> tokenStack; 
    string_view source;
    Object &target;
    AscalExecutor &runtime;
    uint64_t lastVarIndex = 0;
    uint64_t paramsCount = 0;
    FlatMap<string_view, LocalRecord > localMemory;
    int32_t src_index;
    CompilationContext(string_view src, Object &target, AscalExecutor &runtime): source(src), target(target),
        runtime(runtime), localMemory(), src_index(0)
    {
        //target.resizeInstructions(src.length() + Object::SMALL_EXP);
        this->target.clearList();
        this->target.shrinkInstructions(0);
    }
    CompilationContext::Token findTokenAtLevel(string_view source_text, const uint32_t accepted_level, const char opening = '{', const char closing = '}') const noexcept
    {
        uint32_t index = findTokenIndexAtLevel(source_text, accepted_level, opening, closing);
        if(index != std::numeric_limits<uint32_t>::max())
            return this->lastTokens[index];

        return CompilationContext::Token(this->source.substr(source.size(), 0), -1, CompilationContext::Token::null);
    }
    uint32_t findTokenIndexAtLevel(string_view source_text, const uint32_t accepted_level, const char opening = '{', const char closing = '}') const noexcept
    {
        uint32_t index = this->currentToken;
        uint32_t level = 0;
        bool found = false;
        while(!found) 
        {
            while(!level && this->lastTokens.size() > index && this->lastTokens[index].source != source_text)
            {
                const auto token = this->lastTokens[index];
                level += (token.source[0] == opening) - (token.source[0] == closing);
                index++;
            }

            found = this->lastTokens[index].source == source_text;

            while(level && this->lastTokens.size() > index)
            {
                const auto token = this->lastTokens[index];
                level += (token.source[0] == opening) - (token.source[0] == closing);
                index++;
            }
        }
        if(found)
            return index;
        return std::numeric_limits<uint32_t>::max();
    }
    SubStrSV mergeTokens(CompilationContext::Token start, CompilationContext::Token end)
    {
        uint32_t startIndex = reinterpret_cast<uintptr_t>(&start.source[0]) - reinterpret_cast<uintptr_t>(&this->source[0]);
        uint32_t endIndex = reinterpret_cast<uintptr_t>(&end.source.back() + 1) - reinterpret_cast<uintptr_t>(&this->source[0]);
        return SubStrSV(this->source.substr(startIndex, endIndex - startIndex), startIndex, endIndex);
    }
    CompilationContext::Token current()
    {
        if(currentToken >= lastTokens.size())
        {
            throw std::string("Error in compilation accessing token past end of stream, note this is a system error please submit bug report");
        }
        return lastTokens[currentToken];
    }
    CompilationContext& operator++()
    {
        currentToken++;
        return *this;
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