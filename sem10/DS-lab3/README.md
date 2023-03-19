+# Distributed system lab: RabbitMQ
The algorithm provided here is close enough to be an implementation of the task #6.
Its ultimate goal is to make an algorithm described using notation similar to what we had on lecture executable.
The algorithm is implemented with Python3, using `aio-pika` RabbitMQ support package.

**NB!** The provided notation is VERY limited, in particular, it supports only message-driven systems with a few event types, but it can be extended.

## Configuration
The algorithm takes as an input a configuration file and instantiate the distributed system it describes.
The configuration file format is YAML (chosen for its readability).  
The files are obliged the have the following structure:

```yaml
templates:
  template_1:
    one_shot: bool
    variables:
      var_1: val_1
      ...
    in_pipes:
      - ...
    out_pipes:
      - ...
    init:
      ACTION_1
      ...
    rules:
      RULE_1
      ...
  ...

matrix:
  node_1:
    template: template_name
    pipes:
      pipe_name: queue_name
      ...
  ...
```

### `templates`
Under this key the node templates are stored.
In general, (referring to lecture) every working node in a distributed system should run the same code.
The node template is this code, a program description that will be run by the node.  
However, some nodes eventually should differ: for example, an initialization or result collection node should run some other code.
The different programs are represented by different node templates.

The keys of the dictionary are template names, the values are template configurations.  
The template can have the following keys:
- `one_shot`: a boolean value, if set to `True` the `rules` key of the template will be ignored.  
  Default value `False`.
- `variables`: a dictionary of variables, set for this template.
  Keys of the dictionary are variable names (should be in snake case), values are default values or [magic variables](#magic-variables).  
  Default: `[empty dictionary]`.
- `in_pipes`: List of input [pipe](#pipes) names of the template.
  The list elements should be unique (the behavior is undefined otherwise).
  The pipes are backed up by RabbitMQ queues, but this list elements **won't be used for queue names**. 
  The elements are local identifiers inside of this template only.  
  Default: `[empty list]`.
- `out_pipes`: Same as `in_pipes`, but for output [pipe](#pipes) names.  
  Default: `[empty list]`.
- `init`: Dictionary containing [actions](#actions) to be executed on template [start](#node-lifecycle).
  The dictionary keys are action types, the values are action configurations.
  Default: `[empty dictionary]`.
- `rules`: Dictionary containing [rules](#rules) to be triggered by messages, accepted by this node.
  The dictionary keys are action types, the values are action configurations.  
  Default: `[empty dictionary]`.

### `matrix`
Under this key the node instances are stored.
Each node instance _must_ implement one of the templates.
The implementation defines values local to each exact node.

The keys of the dictionary are node names, the values are node configurations.  
The node implementation can have the following keys:
- `template`: The name of the template this node is based on.  
  **Required!**
- `pipes`: Dictionary, defining mapping between global message channels (RabbitMQ queues) and local node pipes.  
  **WARNING!** The keys of the dictionary should match elements of `in_pipes` and `out_pipes` of the template.
  If any of the local template pipes is not mapped to global message channel, an exception will be thrown.  
  Default: `[empty dictionary]` (only if `in_pipes` and `out_pipes` are empty, otherwise **Required!**).

### Magic variables
The magic variables are the strings that will be replaced with something in run time.
Currently only two types of magic variables are supported:
- `self`: Strings with pattern `self.SOME_NAME` will be replaced by value of current node variable named "SOME_NAME".
- `message`: Strings with pattern `message.SOME_NAME` will be replaced by value of current message variable named "SOME_NAME" (in [rule](#rules) context).

If no replacement is found for a magic variable, the string value is returned unmodified.

There are also some special variables that are automatically defined for a node on creation.  
The currently supported special variables:
- `NODE_ID`: unique integer node identifier (not defined before node creation).

### Pipes
Pipes are local node interfaces of global message channels (RabbitMQ queues).
Node can listen to some pipes (by declaring rules, consuming messages from it) and send messages to some pipes (by executing `send` action).

### Actions
Actions are the changes that the node can emit upon initialization and in runtime.
The changes can be both external (to the other nodes in the system) and internal (to the node variables).  
These are the currently supported actions:

#### `send`
Sends a message to one of the node output pipes.  
Has the following structure:

```yaml
send:
  pipe: pipe_name
  message:
    var_1: val_1
    ...
```

Here `pipe_name` is the name of the node output pipe the message will be sent to. **Required!**  
The `var_1: val_1`, `...` are variables (or [magic variables](#magic-variables)) that will be present in the message body. **Required!**

#### `set`
Sets a variable of the current node.  
Has the following structure:

```yaml
set:
  var_1: val_1
```

The `var_1: val_1`, `...` are variables (or [magic variables](#magic-variables)) that will be set for the current node.

### Rules
Rules are the external events that happen when the node receives a message.
The rules are processed for not `one_shot` nodes only and are becoming active on node [start](#node-lifecycle).
The rules have the following structure:

```yaml
rule_name:
  pipe: input
  if: 'some valid python executable string'
  actions:
    ACTION_1
    ...
```

Here `pipe` is the name of the node input pipe the rule is listening to.
The rule can be activated by messages, coming from this pipe only. **Required!**  
The rule is only activated if `if` condition is met.
This condition should be a valid python expression, returning a value that can be cast to `bool`.
No globals can be used in the expression, but instead `self` and `message` locals are available (they work just like the [magic variables](#magic-variables)).
By default, if no condition is specified, the rule is always executed.  
The `actions` are the actions that will be executed if the message is received and passes the condition.
By default, if no actions is specified, nothing will happen.

**NB!** If a message triggers two or more rules, an exception is risen.
Otherwise, the message is consumed from the pipe.

### Node lifecycle
The node lifecycle consists of three stages:
1. **Creation**  
   Upon creation, node object is created, corresponding data read from YAML is parsed and processed.
   The internal variables of the node are set, all the rules, actions, etc. are created.
2. **Launch**  
   Upon launch, the node connects to RabbitMQ and creates all the queues required to back up its pipes.
3. **Start**
   Upon start, node executes its `init` actions (if any) and closes connection to RabbitMQ if it is `one_shot`.
   Otherwise, the node starts consuming messages from its input pipes.

## System structure
System contains a few python files:
- `choreographer.py`: The main script, accepts console args, builds and launches system, catches exceptions.
- `logger.py`: Prints logs, terminates system on critical (declaration) errors.
- `node.py`: Creates, launches, starts and controls execution of a node.
- `utils.py`: Utility functions and classes.

Task configurations are stored in YAML files:
- `pingpong.yml`: Ping-pong task.
- `elections.yml`: Node elections in circle.
- `diffusion.yml`: Implements diffusion algorithm on a node tree.

## Build and run system
System can be run with the following command:
```shell
make run CONF=CONF_FILE
```
where 'CONF_FILE' is a name of configuration file.

Following optional parameters are provided:
- `LOGS`: Name of logging file, _default_: generated.
- `VERB`: System verbosity level, should be one of (DEBUG, INFO, WARNING, ERROR, CRITICAL), _default_: INFO.

## TODOs:
1. Implement `diffusion.yml` (diffusion on a tree, task 7).
2. Add `help` command to Makefile.
3. Implement another (better) `pingpong` system (now it fails successfully half of the times).
4. Catch random async errors (in the end).
5. Write a report.
