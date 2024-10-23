from .layers import *
from .fast_layers import *


def affine_relu_forward(x, w, b):
    """Convenience layer that performs an affine transform followed by a ReLU.

    Inputs:
    - x: Input to the affine layer
    - w, b: Weights for the affine layer

    Returns a tuple of:
    - out: Output from the ReLU
    - cache: Object to give to the backward pass
    """
    a, fc_cache = affine_forward(x, w, b)
    out, relu_cache = relu_forward(a)
    cache = (fc_cache, relu_cache)
    return out, cache

def affine_relu_backward(dout, cache):
    """Backward pass for the affine-relu convenience layer.
    """
    fc_cache, relu_cache = cache
    da = relu_backward(dout, relu_cache)
    dx, dw, db = affine_backward(da, fc_cache)
    return dx, dw, db

# *****START OF YOUR CODE (DO NOT DELETE/MODIFY THIS LINE)*****

def generic_forward(x, params, stepnum, norm=None, bn_params=None, dropout=False, dropout_param=None):
    """Convenience layer that performs fully-connected network forward step.

    Inputs:
    - x: Input to the affine layer
    - params: Step parameters
    - stepnum: Number of the step
    - norm: Normalization layer type (should be either "batchnorm", "layernorm" or None)
    - dropout: Whether the dropout layer is enabled

    Returns a tuple of:
    - out: Output from the layer
    - cache: Object to give to the backward pass
    """
    out, fc_cache = affine_forward(x, params[f"W{stepnum}"], params[f"b{stepnum}"])
    if norm == "batchnorm":
        out, bn_cache = batchnorm_forward(out, params[f"gamma{stepnum}"], params[f"beta{stepnum}"], bn_params[stepnum - 1])
    elif norm == "layernorm":
        out, bn_cache = layernorm_forward(out, params[f"gamma{stepnum}"], params[f"beta{stepnum}"], bn_params[stepnum - 1])
    else:
        bn_cache = None
    out, relu_cache = relu_forward(out)
    if dropout:
        out, dropout_cache = dropout_forward(out, dropout_param)
    else:
        dropout_cache = None
    cache = (fc_cache, bn_cache, relu_cache, dropout_cache)
    return out, cache

def generic_backward(dout, cache, stepnum, norm=None, dropout=False):
    """Backward pass for the fully-connected network.
    """
    grads = dict()
    fc_cache, bn_cache, relu_cache, dropout_cache = cache
    if dropout:
        dout = dropout_backward(dout, dropout_cache)
    dout = relu_backward(dout, relu_cache)
    if norm == "batchnorm":
        dout, grads[f"gamma{stepnum}"], grads[f"beta{stepnum}"] = batchnorm_backward_alt(dout, bn_cache)
    elif norm == "layernorm":
        dout, grads[f"gamma{stepnum}"], grads[f"beta{stepnum}"] = layernorm_backward(dout, bn_cache)
    dx, grads[f"W{stepnum}"], grads[f"b{stepnum}"] = affine_backward(dout, fc_cache)
    return dx, grads

# *****END OF YOUR CODE (DO NOT DELETE/MODIFY THIS LINE)*****

def conv_relu_forward(x, w, b, conv_param):
    """A convenience layer that performs a convolution followed by batch normalization and ReLU.

    Inputs:
    - x: Input to the convolutional layer
    - w, b, conv_param: Weights and parameters for the convolutional layer

    Returns a tuple of:
    - out: Output from the ReLU
    - cache: Object to give to the backward pass
    """
    a, conv_cache = conv_forward_fast(x, w, b, conv_param)
    out, relu_cache = relu_forward(a)
    cache = (conv_cache, relu_cache)
    return out, cache


def conv_relu_backward(dout, cache):
    """Backward pass for the conv-relu convenience layer.
    """
    conv_cache, relu_cache = cache
    da = relu_backward(dout, relu_cache)
    dx, dw, db = conv_backward_fast(da, conv_cache)
    return dx, dw, db


def conv_bn_relu_forward(x, w, b, gamma, beta, conv_param, bn_param):
    """Convenience layer that performs a convolution, a batch normalization, and a ReLU.

    Inputs:
    - x: Input to the convolutional layer
    - w, b, conv_param: Weights and parameters for the convolutional layer
    - pool_param: Parameters for the pooling layer
    - gamma, beta: Arrays of shape (D2,) and (D2,) giving scale and shift
      parameters for batch normalization.
    - bn_param: Dictionary of parameters for batch normalization.

    Returns a tuple of:
    - out: Output from the pooling layer
    - cache: Object to give to the backward pass
    """
    a, conv_cache = conv_forward_fast(x, w, b, conv_param)
    an, bn_cache = spatial_batchnorm_forward(a, gamma, beta, bn_param)
    out, relu_cache = relu_forward(an)
    cache = (conv_cache, bn_cache, relu_cache)
    return out, cache


def conv_bn_relu_backward(dout, cache):
    """Backward pass for the conv-bn-relu convenience layer.
    """
    conv_cache, bn_cache, relu_cache = cache
    dan = relu_backward(dout, relu_cache)
    da, dgamma, dbeta = spatial_batchnorm_backward(dan, bn_cache)
    dx, dw, db = conv_backward_fast(da, conv_cache)
    return dx, dw, db, dgamma, dbeta


def conv_relu_pool_forward(x, w, b, conv_param, pool_param):
    """Convenience layer that performs a convolution, a ReLU, and a pool.

    Inputs:
    - x: Input to the convolutional layer
    - w, b, conv_param: Weights and parameters for the convolutional layer
    - pool_param: Parameters for the pooling layer

    Returns a tuple of:
    - out: Output from the pooling layer
    - cache: Object to give to the backward pass
    """
    a, conv_cache = conv_forward_fast(x, w, b, conv_param)
    s, relu_cache = relu_forward(a)
    out, pool_cache = max_pool_forward_fast(s, pool_param)
    cache = (conv_cache, relu_cache, pool_cache)
    return out, cache


def conv_relu_pool_backward(dout, cache):
    """Backward pass for the conv-relu-pool convenience layer.
    """
    conv_cache, relu_cache, pool_cache = cache
    ds = max_pool_backward_fast(dout, pool_cache)
    da = relu_backward(ds, relu_cache)
    dx, dw, db = conv_backward_fast(da, conv_cache)
    return dx, dw, db
