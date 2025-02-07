/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#include "ff/Console.hpp"
#include <ff-graphics-metal/MetalBlendStateHelpers.hmm>

namespace ff {
    MTLBlendOperation convertBlendOperationToMetalBlendOperation(const BlendOperation& blendOperation) {
        switch(blendOperation) {
            case BlendOperation::Add:
                return MTLBlendOperationAdd;
            case BlendOperation::Subtract:
                return MTLBlendOperationSubtract;
            case BlendOperation::ReverseSubtract:
                return MTLBlendOperationReverseSubtract;
            case BlendOperation::Min:
                return MTLBlendOperationMin;
            case BlendOperation::Max:
                return MTLBlendOperationMax;

            default:
                FF_CONSOLE_ERROR("Unknown conversion of blend operation `%s` to Metal.", (int)blendOperation);
                return (MTLBlendOperation)0;
        }
    }
    BlendOperation convertMetalBlendOperationToBlendOperation(const MTLBlendOperation& blendOperation) {
        switch(blendOperation) {
            case MTLBlendOperationAdd:
                return BlendOperation::Add;
            case MTLBlendOperationSubtract:
                return BlendOperation::Subtract;
            case MTLBlendOperationReverseSubtract:
                return BlendOperation::ReverseSubtract;
            case MTLBlendOperationMin:
                return BlendOperation::Min;
            case MTLBlendOperationMax:
                return BlendOperation::Max;

            default:
                return BlendOperation::Invalid;
        }
    }

    MTLBlendFactor convertBlendFactorToMetalBlendFactor(const BlendFactor& blendFactor) {
        switch(blendFactor) {
            case BlendFactor::Zero:
                return MTLBlendFactorZero;
            case BlendFactor::One:
                return MTLBlendFactorOne;
            
            case BlendFactor::Source:
                return MTLBlendFactorSourceColor;
            case BlendFactor::OneMinusSource:
                return MTLBlendFactorOneMinusSourceColor;

            case BlendFactor::SourceAlpha:
                return MTLBlendFactorSourceAlpha;
            case BlendFactor::OneMinusSourceAlpha:
                return MTLBlendFactorOneMinusSourceAlpha;

            case BlendFactor::Dest:
                return MTLBlendFactorDestinationColor;
            case BlendFactor::OneMinusDest:
                return MTLBlendFactorOneMinusDestinationColor;

            case BlendFactor::DestAlpha:
                return MTLBlendFactorDestinationAlpha;
            case BlendFactor::OneMinusDestAlpha:
                return MTLBlendFactorOneMinusDestinationAlpha;

            default:
                FF_CONSOLE_ERROR("Unknown conversion of blend factor `%s` to Metal.", (int)blendFactor);
                return (MTLBlendFactor)0;
        }
    }
    BlendFactor convertMetalBlendFactorToBlendFactor(const MTLBlendFactor& blendFactor) {
        switch(blendFactor) {
            case MTLBlendFactorZero:
                return BlendFactor::Zero;
            case MTLBlendFactorOne:
                return BlendFactor::One;
            
            case MTLBlendFactorSourceColor:
                return BlendFactor::Source;
            case MTLBlendFactorOneMinusSourceColor:
                return BlendFactor::OneMinusSource;

            case MTLBlendFactorSourceAlpha:
                return BlendFactor::SourceAlpha;
            case MTLBlendFactorOneMinusSourceAlpha:
                return BlendFactor::OneMinusSourceAlpha;

            case MTLBlendFactorDestinationColor:
                return BlendFactor::Dest;
            case MTLBlendFactorOneMinusDestinationColor:
                return BlendFactor::OneMinusDest;

            case MTLBlendFactorDestinationAlpha:
                return BlendFactor::DestAlpha;
            case MTLBlendFactorOneMinusDestinationAlpha:
                return BlendFactor::OneMinusDestAlpha;

            default:
                return BlendFactor::Invalid;
        }
    }
}
