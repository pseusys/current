from os.path import realpath
from pathlib import Path
import json


# The script fixes code cell numbers in .ipynb files for them to start from 1. Yes it IS important.
for notebook in [ path for path in Path(".").rglob("*.ipynb") if not any(x in realpath(path) for x in ("venv", ".ipynb_checkpoints", "__pycache__")) ]:
    parsed = json.loads(notebook)
    cell_iterator = 1
    for cell in parsed["cells"]:
        if cell["cell_type"] == "code":
            cell["execution_count"] = cell_iterator
            for output in cell["outputs"]:
                output["execution_count"] = cell_iterator
            cell_iterator += 1
    notebook.write_text(json.dumps(parsed, indent=2))
