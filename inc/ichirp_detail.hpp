#pragma once

// This header contains private template helpers and implementation details for IChirp.
// It is included by inc/ichirp.h at the end of the file.

#include <any>
#include <map>
#include <string>
#include <type_traits>
#include <utility>
#include <vector>

#include "chirp_error.h"

// NOTE: This header assumes IChirp class declaration is visible

// Private helpers (definitions remain the same as in the original inc/ichirp.h)

template<typename... T, size_t... I>
bool IChirp::validateCastsImpl(const std::vector<std::any>& args, std::index_sequence<I...>) {
    return ((std::any_cast<std::decay_t<T>>(&args[1 + I]) != nullptr) && ...);
}

template<typename... T>
bool IChirp::validateCasts(const std::vector<std::any>& args) {
    if (args.size() != sizeof...(T) + 1) {
        return false;
    }
    return validateCastsImpl<T...>(args, std::index_sequence_for<T...>{});
}

template<typename Obj, typename Ret, typename... Args>
ChirpError::Error IChirp::executeHandler(Obj* object,
                                         Ret(Obj::*method)(Args...),
                                         const std::vector<std::any>& args) {
    ChirpError::Error validateResult = validateArgCount<Args...>(args, this->getServiceName());
    if (validateResult != ChirpError::SUCCESS) {
        if (_validationCallback) { _validationCallback(validateResult); }
        if (_asyncValidationCallback) { _asyncValidationCallback(validateResult); }
        return validateResult;
    }

    if (_validateOnly) {
        if (!validateCasts<Args...>(args)) {
            if (_validationCallback) { _validationCallback(ChirpError::INVALID_ARGUMENTS); }
            if (_asyncValidationCallback) { _asyncValidationCallback(ChirpError::INVALID_ARGUMENTS); }
            return ChirpError::INVALID_ARGUMENTS;
        }
        return ChirpError::SUCCESS;
    }

    if (!validateCasts<Args...>(args)) {
        if (_validationCallback) { _validationCallback(ChirpError::INVALID_ARGUMENTS); }
        if (_asyncValidationCallback) { _asyncValidationCallback(ChirpError::INVALID_ARGUMENTS); }
        return ChirpError::INVALID_ARGUMENTS;
    }

    std::vector<std::any> slicedArgs(args.begin() + 1, args.end());
    executeHandlerImpl(object, method, slicedArgs, std::index_sequence_for<Args...>{});
    return ChirpError::SUCCESS;
}

template<typename Obj, typename Ret, typename... Args, size_t... I>
void IChirp::executeHandlerImpl(Obj* object,
                                Ret(Obj::*method)(Args...),
                                const std::vector<std::any>& args,
                                std::index_sequence<I...>) {
    (object->*method)(std::any_cast<Args>(args[I])...);
}

template<typename Obj, typename Ret, typename... Args, size_t... I>
void IChirp::executeHandlerImpl(Obj* object,
                                Ret(Obj::*method)(Args...) const,
                                const std::vector<std::any>& args,
                                std::index_sequence<I...>) {
    (object->*method)(std::any_cast<Args>(args[I])...);
}

template<typename Obj, typename Ret, typename... Args>
ChirpError::Error IChirp::executeConstHandler(Obj* object,
                                              Ret(Obj::*method)(Args...) const,
                                              const std::vector<std::any>& args) {
    ChirpError::Error validateResult = validateArgCount<Args...>(args, this->getServiceName());
    if (validateResult != ChirpError::SUCCESS) {
        if (_validationCallback) { _validationCallback(validateResult); }
        if (_asyncValidationCallback) { _asyncValidationCallback(validateResult); }
        return validateResult;
    }

    if (_validateOnly) {
        if (!validateCasts<Args...>(args)) {
            if (_validationCallback) { _validationCallback(ChirpError::INVALID_ARGUMENTS); }
            if (_asyncValidationCallback) { _asyncValidationCallback(ChirpError::INVALID_ARGUMENTS); }
            return ChirpError::INVALID_ARGUMENTS;
        }
        return ChirpError::SUCCESS;
    }

    if (!validateCasts<Args...>(args)) {
        if (_validationCallback) { _validationCallback(ChirpError::INVALID_ARGUMENTS); }
        if (_asyncValidationCallback) { _asyncValidationCallback(ChirpError::INVALID_ARGUMENTS); }
        return ChirpError::INVALID_ARGUMENTS;
    }

    std::vector<std::any> slicedArgs(args.begin() + 1, args.end());
    executeHandlerImpl(object, method, slicedArgs, std::index_sequence_for<Args...>{});
    return ChirpError::SUCCESS;
}
