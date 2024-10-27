from itertools import product
from typing import Any, Callable, Dict, List, Optional


class Hypervisor:
    def __init__(self, model: Callable[[Dict[str, Any]], Any], solver: Callable[[Dict[str, Any]], Any], model_params: Optional[Dict[str, List[Any]]] = None, model_const: Optional[Dict[str, Any]] = None, solver_params: Optional[Dict[str, List[Any]]] = None, solver_const: Optional[Dict[str, Any]] = None) -> None:
        self.model = model
        self.solver = solver
        self.model_params = model_params if model_params is not None else dict()
        self.model_const = model_const if model_const is not None else dict()
        self.solver_params = solver_params if solver_params is not None else dict()
        self.solver_const = solver_const if solver_const is not None else dict()
        self.solver_const["verbose"] = False

    def _check_one(self, data: Dict[str, Any], solver_conf: Optional[Dict[str, Any]] = None, model_conf: Optional[Dict[str, Any]] = None) -> Any:
        solver_conf = solver_conf if solver_conf is not None else dict()
        model_conf = model_conf if model_conf is not None else dict()
        model = self.model(**self.model_const, **model_conf)
        solver = self.solver(model, data, **self.solver_const, **solver_conf)
        solver.train()
        return solver

    def resolve(self, data: Dict[str, Any]) -> Any:
        base_solver = self._check_one(data)
        best_model, best_val_accuracy = base_solver.model, base_solver.best_val_acc
        print(f"Baseline accuracy is {best_val_accuracy}, tuning hyperparameters...")
        for solver_vals in list(product(*self.solver_params.values())) + [tuple()]:
            solver_conf = dict(zip(self.solver_params.keys(), solver_vals))
            for model_vals in product(*self.model_params.values()):
                model_conf = dict(zip(self.model_params.keys(), model_vals))
                solver = self._check_one(data, solver_conf, model_conf)
                if solver.best_val_acc > best_val_accuracy:
                    print(f"Better accuracy {solver.best_val_acc} found for model: {model_conf}, solver: {solver_conf}")
                    best_model, best_val_accuracy = solver.model, solver.best_val_acc
        print(f"Best accuracy received is {best_val_accuracy}!")
        return best_model
        
